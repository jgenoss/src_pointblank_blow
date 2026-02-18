// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// AvatarViewer.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "afxwinappex.h"
#include "AvatarViewer.h"
#include "MainFrm.h"
#include "OptionInfo.h"
#include "GlobalVar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAvatarViewerApp

BEGIN_MESSAGE_MAP(CAvatarViewerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAvatarViewerApp::OnAppAbout)
END_MESSAGE_MAP()


// CAvatarViewerApp construction

CAvatarViewerApp::CAvatarViewerApp()
{

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CAvatarViewerApp object

CAvatarViewerApp theApp;


// CAvatarViewerApp initialization

BOOL CAvatarViewerApp::InitInstance()
{
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

//	afxAmbientActCtx = FALSE;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Zepetto"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();

	{
		i3MemoryInit();

		i3Error::Init();
		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE);
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);
		i3Error::SetChannelName( I3LOG_NOTICE, "AvatarViewer.log");
		i3Error::SetChannelName( I3LOG_FATAL, "AvatarViewer.log");
		i3Error::SetChannelName( I3LOG_WARN, "AvatarViewer.log");
		
		I3PRINTLOG(I3LOG_NOTICE, "RegisterMetas");
		i3FrameworkRegisterMetas();
		i3TDKRegisterMetas();

		BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
		if (FALSE == bDir)
		{
			i3ResourceFile::SetWorkingFolder("");
		}
//		COptionInfo::RegisterMeta();
//		Avatar::RegisterMeta();
	}

	{
		I3PRINTLOG(I3LOG_NOTICE, "Create OptionInfo");
		g_pOption = COptionInfo::new_object();
	}

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	I3PRINTLOG(I3LOG_NOTICE, "Create MainFrame");
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	I3PRINTLOG(I3LOG_NOTICE, "Load MainFrame");
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	I3PRINTLOG(I3LOG_NOTICE, "ShowWindow & UpdateWindow");

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CAvatarViewerApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CAvatarViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAvatarViewerApp customization load/save methods

void CAvatarViewerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CAvatarViewerApp::LoadCustomState()
{
}

void CAvatarViewerApp::SaveCustomState()
{
}

// CAvatarViewerApp message handlers




int CAvatarViewerApp::ExitInstance()
{
	if( g_pOption != NULL)
	{
		// delete g_pOption;
		i3::destroy_instance(g_pOption);
		g_pOption = NULL;
	}

	i3System::TerminateSys();

	i3ElementBase::DumpObjects();
	//i3mem::Dump( I3MEMORY_DEBUG_TEXT | I3MEMORY_DEBUG_EACH);

	return CWinAppEx::ExitInstance();
}
