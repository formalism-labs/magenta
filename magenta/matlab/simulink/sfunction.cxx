
#undef THIS
#define THIS (*(S_FUNCTION_CLASS *) ssGetPWork(S)[0])(S)

#define CATCH_ALL \
	catch (sgi_stl::string s) { magenta_v4::SFunction(S).error( s.c_str() ); } \
	catch (...) { ssSetErrorStatus(S, "unknown XSystem error"); }

///////////////////////////////////////////////////////////////////////////////////////////////

#if defined(MDL_CHECK_PARAMETERS)
static void mdlCheckParameters(SimStruct *S)
{
	try
	{
		S_FUNCTION_CLASS(S, false).checkParameters();
	}
	CATCH_ALL
}
#endif

//---------------------------------------------------------------------------------------------

static void mdlInitializeSizes(SimStruct *S)
{
	try
	{
		const int *argc = S_FUNCTION_CLASS::Arguments::expectedArgc();
		
		int n = ssGetSFcnParamsCount(S), N = *argc;
		for (int i = 1; i < N; ++i)
			if (n == argc[i])
				break;
		if (i == N)
		{
			ssSetErrorStatus(S, "wrong number of arguments to S-Function");
			return;
		}
		ssSetNumSFcnParams( S, n );

		if (ssGetErrorStatus(S) != NULL)
			return;

		S_FUNCTION_CLASS sfx(S, false);
		if (ssGetErrorStatus(S) != NULL)
			return;

#if defined(MDL_CHECK_PARAMETERS)
		sfx.checkParameters();
		if (ssGetErrorStatus(S) != NULL)
			return;
#endif

		sfx.initializeSizes();
		ssSetNumPWork(S, 1);
	} 
	CATCH_ALL
}

//---------------------------------------------------------------------------------------------

static void mdlInitializeSampleTimes(SimStruct *S)
{
	try
	{
		S_FUNCTION_CLASS sfx(S,false);
		if (ssGetErrorStatus(S) != NULL)
			return;
		sfx.initializeSampleTimes();
	}
	CATCH_ALL
}

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_INPUT_PORT_WIDTH) || defined(MDL_SET_OUTPUT_PORT_WIDTH)

static void mdlSetInputPortWidth(SimStruct *S, int_T port, int_T width)
{
	try
	{
		S_FUNCTION_CLASS(S, false).setInputPortWidth( port, width );
	}
	CATCH_ALL
}

static void mdlSetOutputPortWidth(SimStruct *S, int_T port, int_T width)
{
	try
	{
		S_FUNCTION_CLASS(S, false).setOutputPortWidth( port, width );
	}
	CATCH_ALL
}

#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_DEFAULT_PORT_DIMENSION_INFO)
static void mdlSetDefaultPortDimensionInfo(SimStruct *S)
{
	try
	{
		S_FUNCTION_CLASS(S, false).setDefaultPortDimInfo();
	}
	CATCH_ALL
}
#endif

//---------------------------------------------------------------------------------------------

#if defined(MDL_SET_WORK_WIDTHS)
static void mdlSetWorkWidths(SimStruct *S)
{
	try
	{
		S_FUNCTION_CLASS(S, false).setWorkWidths();
	}
	CATCH_ALL
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////

#define MDL_START
#if defined(MDL_START) 
static void mdlStart(SimStruct *S)
{
	try
	{
		*ssGetPWork(S) = new S_FUNCTION_CLASS( S, true );

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
