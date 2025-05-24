
#define S_FUNCTION_NAME  sfxlib
#define S_FUNCTION_CLASS SFXLIB

#include "magenta/matlab/simulink/defs.h"

#include "sfx1.h"
#include "sfx2.h"
#include "sfx3.h"

///////////////////////////////////////////////////////////////////////////////////////////////

SFUNCTION_LIB_BEGIN(SFXLIB)
	SFUNCTION_LIB_MEMBER(SFX1, "sfx1")
	SFUNCTION_LIB_MEMBER(SFX2, "sfx2")
	SFUNCTION_LIB_MEMBER(SFX3, "sfx3")
SFUNCTION_LIB_END

///////////////////////////////////////////////////////////////////////////////////////////////

#define MDL_CHECK_PARAMETERS
#define MDL_INITIALIZE_CONDITIONS
#define MDL_SET_INPUT_PORT_WIDTH
#define MDL_SET_WORK_WIDTHS
#define MDL_UPDATE
#define MDL_DERIVATIVES

#include "magenta/matlab/simulink/sfunction-lib.cxx"
