// i3TDK.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "i3TDKApp.h"
#include "i3TDKGlobalVariable.h"
#include "i3TDKMeta.h"

/*
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// i3TDKApp

BEGIN_MESSAGE_MAP(i3TDKApp, CWinApp)
END_MESSAGE_MAP()


// i3TDKApp construction

i3TDKApp::i3TDKApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only i3TDKApp object

i3TDKApp theApp;


// i3TDKApp initialization

BOOL i3TDKApp::InitInstance()
{
	CWinApp::InitInstance();

#ifdef _DEBUG	// Debug
	g_hInstTDK = GetModuleHandle( "i3TDK_DEBUG.dll");
#else			// Distribute
	g_hInstTDK = GetModuleHandle( "i3TDK_DISTRIBUTE.dll");
#endif
	I3ASSERT( g_hInstTDK != nullptr);

	i3MemoryInit( ); 

	i3TDKRegisterMetas();

	g_pTDKEnvSet = i3TDKEnvSet::new_object();

	i3TDKGlobalRes::Init();

	if( g_pTDKEnvSet->getCurWorkDir()[0] != '\0')
		i3ResourceFile::SetWorkingFolder( (char*) g_pTDKEnvSet->getCurWorkDir());

	return TRUE;
}

int i3TDKApp::ExitInstance()
{
	i3TDKGlobalRes::Close();

	I3_SAFE_RELEASE( g_pTDKEnvSet);

	return CWinApp::ExitInstance();
}

*/