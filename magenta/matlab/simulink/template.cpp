
#define S_FUNCTION_NAME  sfx
#define S_FUNCTION_CLASS SFX
#define S_FUNCTION_ARGC 1

#include "magenta/matlab/simulink/defs.h"

///////////////////////////////////////////////////////////////////////////////////////////////

class SFX : public XSystem::SFunction
{
public:
	SFX(SimStruct *S, bool ready);
	~SFX();

public:
	void checkParameters();
	void initializeSizes();

public:
	void initializeConditions();
	void output(int_T tid);
	void update(int_T tid);
};

///////////////////////////////////////////////////////////////////////////////////////////////

SFX::SFX(SimStruct *S, bool ready) : XSystem::SFunction(S)
{
}

//---------------------------------------------------------------------------------------------

SFX::~SFX()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Setup Phase
///////////////////////////////////////////////////////////////////////////////////////////////

inline void SFX::checkParameters()
{
}

//---------------------------------------------------------------------------------------------

inline void SFX::initializeSizes()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Simulation Phase
///////////////////////////////////////////////////////////////////////////////////////////////

void SFX::initializeConditions()
{
}

//---------------------------------------------------------------------------------------------

void SFX::output(int_T tid)
{
}

//---------------------------------------------------------------------------------------------

void SFX::update(int_T tid)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////

#define MDL_CHECK_PARAMETERS
#define MDL_INITIALIZE_CONDITIONS
#define MDL_SET_INPUT_PORT_WIDTH
#define MDL_SET_WORK_WIDTHS
#define MDL_UPDATE
#define MDL_DERIVATIVES

#include "magenta/matlab/simulink/sfunction.cxx"
