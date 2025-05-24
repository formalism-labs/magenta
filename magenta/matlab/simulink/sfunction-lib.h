
#ifndef _magenta_matlab_simulink_sfunction_lib_
#define _magenta_matlab_simulink_sfunction_lib_

#include "magenta/matlab/simulink/sfunction.h"

#include "magenta/matlab/mex/defs.h"
#include "magenta/types/defs.h"

namespace magenta
{


using std::string;

///////////////////////////////////////////////////////////////////////////////////////////////

class SFXLibMember
{
public:
	SFXLibMember(string name) : name(name) {}

	const string name;
	virtual SFunction *operator()(SimStruct *S, bool ready) = 0;
	virtual const int *expectedArgc() const = 0;
};

//---------------------------------------------------------------------------------------------

template <class Member>
class SFXLibMemberProxy : public SFXLibMember
{
public:
	SFXLibMemberProxy(string name) : SFXLibMember(name) {}
	
	SFunction *operator()(SimStruct *S, bool ready = false) { return new Member(S, ready); }
	const int *expectedArgc() const { return Member::Arguments::expectedArgc(); }
};

//---------------------------------------------------------------------------------------------

#define SFUNCTION_LIB_BEGIN(Lib) \
	class Lib : public magenta::SFunctionLib \
	{ \
	public: \
		Lib() \
		{ \
			_members

#define SFUNCTION_LIB_MEMBER(Member, name) \
				<< new magenta::SFXLibMemberProxy<Member>(name)

#define SFUNCTION_LIB_END \
			; \
		} \
	} sfx_lib;

///////////////////////////////////////////////////////////////////////////////////////////////

class SFunctionLib // : public SFunction
{
public:
	class Members : public RefHash<SFXLibMember>
	{
	public:
		Members &operator<<(SFXLibMember *member) { insert(member->name, member); return *this; }
	};

	Members _members;

	SFunction *operator()(SimStruct *S, bool ready = false)
	{
		string key = MEX::ConstString(ssGetSFcnParam(S, 0));
		return (*_members[key])(S, ready);
	}

	const int *expectedArgc(SimStruct *S) const
	{
		string key = MEX::ConstString(ssGetSFcnParam(S, 0));
		return _members[key]->expectedArgc();
	}

/*
public:
	SFunctionLib(SimStruct *S) : SFunction(S) {}
	virtual ~SFunctionLib() {}

	SFunction &operator()(SimStruct *S_) { S = S_; return *this; }

	//-----------------------------------------------------------------------------------------
	// Setup Callbacks
	//-----------------------------------------------------------------------------------------
public:
	void checkParameters() {}
	void initializeSizes() {}

	void setInputPortWidth(int_T port, int_T width);
	void setOutputPortWidth(int_T port, int_T width);

	void setDefaultPortDimInfo() {}

	void initializeSampleTimes() {}

	void setWorkWidths() {}

	//-----------------------------------------------------------------------------------------
	// Simulation Callbacks
	//-----------------------------------------------------------------------------------------
public:
	virtual void initializeConditions() {}
	virtual void start() {}
	virtual void output(int_T tid) {}
	virtual void update(int_T tid) {}
	virtual void derivatives() {}
*/
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

///////////////////////////////////////////////////////////////////////////////////////////////

#endif // _magenta_matlab_simulink_sfunction_lib_
