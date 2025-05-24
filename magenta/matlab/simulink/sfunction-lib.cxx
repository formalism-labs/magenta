
using magenta_v4::SFunction;

///////////////////////////////////////////////////////////////////////////////////////////////

#undef THIS
#define THIS (*(SFunction *) ssGetPWork(S)[0])(S)

#define CATCH_ALL \
	catch (sgi_stl::string s) { SFunction(S).error( s.c_str() ); } \
	catch (const char *s) { SFunction(S).error( s ); } \
	catch (...) { ssSetErrorStatus(S, "unknown XSystem error"); }

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(MDL_CHECK_PARAMETERS)
static void mdlCheckParameters(SimStruct *S)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		sfx->checkParameters();
	}
	CATCH_ALL
	delete sfx;
}
#endif

//---------------------------------------------------------------------------------------------

static void mdlInitializeSizes(SimStruct *S)
{
	SFunction *sfx = 0;
	try
	{
		const int *argc = sfx_lib.expectedArgc(S);
		
		int n = ssGetSFcnParamsCount(S), N = *argc;
		for (int i = 1; i <= N; ++i)
			if (n-1 == argc[i])
				break;
		if (i > N)
		{
			ssSetErrorStatus(S, "wrong number of arguments to S-Function");
			return;
		}
		ssSetNumSFcnParams( S, n );

		if (ssGetErrorStatus(S) != NULL)
			return;

		sfx = sfx_lib(S);
		if (ssGetErrorStatus(S) != NULL)
			return;

#if defined(MDL_CHECK_PARAMETERS)
		sfx->checkParameters();
		if (ssGetErrorStatus(S) != NULL)
			return;
#endif

		sfx->initializeSizes();
		ssSetNumPWork(S, 1);
	} 
	CATCH_ALL
	delete sfx;
}

//---------------------------------------------------------------------------------------------

static void mdlInitializeSampleTimes(SimStruct *S)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		if (ssGetErrorStatus(S) != NULL)
			return;
		sfx->initializeSampleTimes();
	}
	CATCH_ALL
	delete sfx;
}

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_INPUT_PORT_WIDTH) || defined(MDL_SET_OUTPUT_PORT_WIDTH)

static void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T width)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		sfx->setInputPortWidth( port, width );
	}
	CATCH_ALL
	delete sfx;
}

static void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T width)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		sfx->setOutputPortWidth( port, width );
	}
	CATCH_ALL
	delete sfx;
}

#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_DEFAULT_PORT_DIMENSION_INFO)
static void mdlSetDefaultPortDimensionInfo(SimStruct *S)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		sfx->setDefaultPortDimInfo();
	}
	CATCH_ALL
	delete sfx;
}
#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_WORK_WIDTHS)
static void mdlSetWorkWidths(SimStruct *S)
{
	SFunction *sfx = 0;
	try
	{
		sfx = sfx_lib(S);
		sfx->setWorkWidths();
	}
	CATCH_ALL
	delete sfx;
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

#define MDL_START
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{
	try
	{
		*ssGetPWork(S) = sfx_lib(S, true);

		THIS.checkParameters();
		if (ssGetErrorStatus(S) != NULL)
			return;
	}
	CATCH_ALL
}
#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_INITIALIZE_CONDITIONS)
static void mdlInitializeConditions(SimStruct *S)
{
	try
	{
		THIS.initializeConditions();
	}
	CATCH_ALL
}
#endif

//---------------------------------------------------------------------------------------------

static void mdlOutputs(SimStruct *S, int_T tid)
{
	THIS.output( tid );
}

//---------------------------------------------------------------------------------------------

#if defined(MDL_UPDATE)
static void mdlUpdate(SimStruct *S, int_T tid)
{
	THIS.update( tid );
}
#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_DERIVATIVES)
static void mdlDerivatives(SimStruct *S)
{
	THIS.derivatives();
}
#endif

//---------------------------------------------------------------------------------------------

static void mdlTerminate(SimStruct *S)
{
	delete &THIS;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#undef THIS
#undef CATCH_ALL

#ifdef  MATLAB_MEX_FILE
#include SIMULINK_HEADER(simulink.c)
#else
#include SIMULINK_HEADER(cg_sfun.h)
#endif
