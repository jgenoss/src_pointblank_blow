
// i3UITemplateTrashRemover.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "i3UITemplateTrashRemover.h"
#include "i3UITemplateTrashRemoverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


I3ERROR_HANDLE Ci3UITemplateTrashRemoverApp::CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData)
{
	Ci3UITemplateTrashRemoverDlg * pDlg = (Ci3UITemplateTrashRemoverDlg *)theApp.m_pMainWnd;

	if( pDlg)
	{
		char strMessage[256];

		switch( type)
		{
		case I3LOG_FATAL:
			sprintf( strMessage, "[Fatal] %s : %s", pszWhere, pszMsg);
			break;
		case I3LOG_WARN:
			sprintf( strMessage, "[Warning] %s : %s", pszWhere, pszMsg);
			break;
		default:
			sprintf( strMessage, "[Notice] %s", pszMsg);
			break;
		}

		//	Log 창에 메시지 출력
		pDlg->OutputNotice( strMessage, true);
	}

	if( type == I3LOG_FATAL)
		return I3ERROR_HANDLE_BREAK;
	else
		return I3ERROR_HANDLE_CONTINUE;	
}
// Ci3UITemplateTrashRemoverApp

BEGIN_MESSAGE_MAP(Ci3UITemplateTrashRemoverApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Ci3UITemplateTrashRemoverApp construction

Ci3UITemplateTrashRemoverApp::Ci3UITemplateTrashRemoverApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only Ci3UITemplateTrashRemoverApp object

Ci3UITemplateTrashRemoverApp theApp;


// Ci3UITemplateTrashRemoverApp initialization

BOOL Ci3UITemplateTrashRemoverApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();
	AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Zepetto"));

	i3MemoryInit();

	{
		i3Error::Init();
		
		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		//i3Error::SetChannelName( I3LOG_FATAL, "i3UITemplateTrachRemover.log");
		//i3Error::SetChannelName( I3LOG_WARN, "i3UITemplateTrachRemover.log");
		i3Error::SetChannelName( I3LOG_NOTICE, "i3UITemplateTrachRemover.log");

		i3Error::SetCallback( I3LOG_NOTICE, Ci3UITemplateTrashRemoverApp::CallbackLog, NULL);

		i3TDKRegisterMetas();
	}

	Ci3UITemplateTrashRemoverDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
