
#ifndef _magenta_matlab_simulink_sfunction_
#define _magenta_matlab_simulink_sfunction_

#include "magenta/lang/cxx/defs.h"
#include "magenta/types/defs.h"

//---------------------------------------------------------------------------------------------

#ifndef S_FUNCTION_LEVEL
#define S_FUNCTION_LEVEL 2
#endif

#ifndef S_FUNCTION_NAME
#define S_FUNCTION_NAME sfx
//#error S_FUNCTION_NAME undefined. May lead to unexpected behaviour.
#pragma message(__FILE__ " (" TO_STRING(__LINE__) "): implicitly defined S_FUNCTION_NAME") 
#endif

#ifndef MATLAB_MEX_FILE
#define MATLAB_MEX_FILE
#endif

#ifdef MAT6SIM4
#define SIMULINK_HEADER(file)	TO_STRING(matlab-6/simulink/##file)
#else
#define SIMULINK_HEADER(file)	TO_STRING(matlab-5/simulink/##file)
#endif

#include SIMULINK_HEADER(simstruc.h)

//---------------------------------------------------------------------------------------------

namespace magenta
{


///////////////////////////////////////////////////////////////////////////////////////////////

class SFunction
{
public:
	SimStruct *S;

public:
	SFunction(SimStruct *S_, int argBase = 0) : S(S_) {}
	virtual ~SFunction() {}

	SFunction &operator()(SimStruct *S_) { S = S_; return *this; }

	//-----------------------------------------------------------------------------------------
	// Arguments
	//-----------------------------------------------------------------------------------------
public:
	const mxArray *argv(int i) { return ssGetSFcnParam(S, i); }
	int argc() const { return ssGetSFcnParamsCount(S); }
		
	//-----------------------------------------------------------------------------------------
	// States
	//-----------------------------------------------------------------------------------------
public:
	double &dstate(int i = 0) { return ssGetRealDiscStates(S)[i]; }
	double dstate(int i = 0) const { return ssGetRealDiscStates(S)[i]; }

	int dstatesNum() const { return ssGetNumDiscStates(S); }
	void setDStateNum(int n) { ssSetNumDiscStates(S, n); }

	//-----------------------------------------------------------------------------------------
	// I/O Ports
	//-----------------------------------------------------------------------------------------
public:
	double iport(int i = 0, int j = 0) const { return *ssGetInputPortRealSignalPtrs(S, i)[j]; }
	double &oport(int i = 0, int j = 0) { return ssGetOutputPortRealSignal(S, i)[j]; }

	double safeiport(int i = 0, int j = 0) const;
	double &safeoport(int i = 0, int j = 0);

	int_T iportsNum() const { return ssGetNumInputPorts(S); }
	int_T oportsNum() const { return ssGetNumOutputPorts(S); }

	bool setIPortsNum(int n) { return !!ssSetNumInputPorts(S, n); }
	bool setOPortsNum(int n) { return !!ssSetNumOutputPorts(S, n); }

	int iportWidth(int i = 0) const { return ssGetInputPortWidth(S, i); }
	int oportWidth(int i = 0) const { return ssGetOutputPortWidth(S, i); }

	void setIPortWidth(int i, int n) { ssSetInputPortWidth(S, i, n); }
	void setOPortWidth(int i, int n) { ssSetOutputPortWidth(S, i, n); }

	bool iportDFT(int i) const { return !!ssGetInputPortDirectFeedThrough(S, i); }
	void setIPortDFT(int i, bool dft) { ssSetInputPortDirectFeedThrough(S, i, dft ? 1 : 0); }

	void setIPort(int i, int width, bool dft)
	{
		setIPortWidth(i, width);
		setIPortDFT(i, dft);
	}

	//-----------------------------------------------------------------------------------------
	// Model Attributes
	//-----------------------------------------------------------------------------------------
public:
	magenta::String path() const { return ssGetPath(S); }

	//-----------------------------------------------------------------------------------------
	// Setup Callbacks
	//-----------------------------------------------------------------------------------------
public:
	virtual void checkParameters() {}
	virtual void initializeSizes() {}

	virtual void setInputPortWidth(int_T port, int_T width);
	virtual void setOutputPortWidth(int_T port, int_T width);

	virtual void setDefaultPortDimInfo() {}

	virtual void initializeSampleTimes() {}

	virtual void setWorkWidths() {}

	//-----------------------------------------------------------------------------------------
	// Simulation Callbacks
	//-----------------------------------------------------------------------------------------
public:
	virtual void initializeConditions() {}
	virtual void start() {}
	virtual void output(int_T tid) {}
	virtual void update(int_T tid) {}
	virtual void derivatives() {}

	//-----------------------------------------------------------------------------------------
	// Exceptions
	//-----------------------------------------------------------------------------------------
protected:
	static Strings _messages;

public:
	void error(const char *fmt, ...);
	void error(const char *fmt, va_list args);
	void warning(const char *fmt, ...);
	void warning(const char *fmt, va_list args);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAT6SIM4

// From Simulink4
inline void ssWarning(SimStruct *S, const char *msg)
{
	const char fmt[] = "block '%s': %s";
	char *warnMsg = ssGetPath(S) != NULL ?
		(char *) malloc(strlen(msg) + sizeof(fmt) + strlen(ssGetPath(S)) + 1) : NULL;
    if (warnMsg == NULL) 
        mexWarnMsgTxt(msg);
	else 
	{
        sprintf(warnMsg,fmt,ssGetPath(S),msg);
        mexWarnMsgTxt(warnMsg);
        free(warnMsg);
    }
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_matlab_simulink_sfunction_
