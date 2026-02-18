// PanelResult.cpp : implementation file
//

#include "stdafx.h"
#include "i3OptManager.h"
#include "PanelResult.h"
#include "GlobalVar.h"
#include "Warn.h"
#include "WarnTextureFormat.h"
#include "DlgDetectOption.h"
#include ".\panelresult.h"

// CPanelResult dialog

IMPLEMENT_DYNAMIC(CPanelResult, CDialog)
CPanelResult::CPanelResult(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelResult::IDD, pParent)
{
	m_pScanThread = NULL;
}

CPanelResult::~CPanelResult()
{
	_CancelScan();
	_RemoveAllWarn();
	_RemoveAllFile();
}

void CPanelResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_PATH, m_ED_Path);
	DDX_Control(pDX, IDC_ED_STATUS, m_ED_Status);
	DDX_Control(pDX, IDC_LST_FILE, m_LST_File);
	DDX_Control(pDX, IDC_BTN_GO, m_BTN_Go);
	DDX_Control(pDX, IDC_BTN_PATH, m_BTN_Path);
}

void CPanelResult::_RemoveAllWarn(void)
{
	INT32 i;
	Warn * pWarn;

	for( i = 0; i < m_WarnList.GetCount(); i++)
	{
		pWarn = (Warn *) m_WarnList.Items[i];

		pWarn->Release();
	}

	m_WarnList.Clear();
}

void CPanelResult::_RemoveAllFile(void)
{
	INT32 i;
	CFileInfo * pInfo;

	for( i = 0; i < m_FileList.GetCount(); i++)
	{
		pInfo = (CFileInfo *) m_FileList.Items[i];

		pInfo->Release();
	}

	m_FileList.Clear();
}


BEGIN_MESSAGE_MAP(CPanelResult, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_PATH, OnBnClickedBtnPath)
	ON_BN_CLICKED(IDC_BTN_DETECT_OPTION, OnBnClickedBtnDetectOption)
	ON_BN_CLICKED(IDC_BTN_GO, OnBnClickedBtnGo)
END_MESSAGE_MAP()


// CPanelResult message handlers
BOOL CPanelResult::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LST_File.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LST_File.InsertColumn( 0, "Warning", LVCFMT_LEFT, 120);
		m_LST_File.InsertColumn( 1, "Path", LVCFMT_LEFT, 220);
		m_LST_File.InsertColumn( 2, "Message", LVCFMT_LEFT, 220);
	}

	// Initial Path
	{
		m_ED_Path.SetWindowText( g_pOption->m_szWorkPath);
	}

	{
		{
			WarnTextureFormat * pWarn = WarnTextureFormat::NewObject();

			m_WarnList.Add( pWarn);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CPanelResult::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LST_File.m_hWnd))
	{
		CRect rt;

		m_LST_File.GetWindowRect( &rt);

		ScreenToClient( &rt);

		cx -= rt.left + 6;
		cy -= rt.top + 6;

		m_LST_File.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CPanelResult::OnBnClickedBtnPath()
{
	i3TDKFolderSelectDialog dlg;

	if( dlg.Execute( m_hWnd, "작업 폴더 설정", BIF_RETURNONLYFSDIRS, NULL, 0, g_pOption->m_szWorkPath))
	{
		i3String::Copy( g_pOption->m_szWorkPath, dlg.GetSelectedFolderPath());

		m_ED_Path.SetWindowText( g_pOption->m_szWorkPath);
	}
}

void CPanelResult::_StartScan(void)
{
	m_ED_Path.EnableWindow( FALSE);
	m_BTN_Path.EnableWindow( FALSE);
	m_BTN_Go.SetWindowText( "Cancel");

	I3ASSERT( m_pScanThread == NULL);

	m_pScanThread = ScanThread::NewObject();

	m_pScanThread->setHWND( m_hWnd);
	m_pScanThread->setResourcePath( g_pOption->m_szWorkPath);
	m_pScanThread->setWarnList( &m_WarnList);

	m_pScanThread->Create( "ScanThread", 0, 4 * 1024 * 1024);
}

void CPanelResult::_ReportScan( char * pszName)
{
	m_ED_Status.SetWindowText( pszName);
}

void CPanelResult::_EndScan(void)
{
	m_ED_Path.EnableWindow( TRUE);
	m_BTN_Path.EnableWindow( TRUE);
	m_BTN_Go.SetWindowText( "Scan");

	m_ED_Status.SetWindowText( "Finished");
}

void CPanelResult::_CancelScan(void)
{
	if( m_pScanThread != NULL)
	{
		m_pScanThread->Terminate();

		m_pScanThread->Release();
		m_pScanThread = NULL;

		_EndScan();

		m_ED_Status.SetWindowText( "Canceled");
	}
}

void CPanelResult::_AddFile( CFileInfo * pInfo)
{
	m_FileList.Add( pInfo);
}

LRESULT CPanelResult::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_SCAN_START :
			break;

		case WM_SCAN_END :
			_EndScan();
			break;

		case WM_SCAN_FILE :
			_ReportScan( (char *) wParam);
			break;

		case WM_SCAN_ADD :
			_AddFile( (CFileInfo *) wParam);
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanelResult::OnBnClickedBtnDetectOption()
{
	CDlgDetectOption dlg;

	dlg.Execute( &m_WarnList);
}

void CPanelResult::OnBnClickedBtnGo()
{
	if( m_pScanThread == NULL)
	{
		_StartScan();
	}
	else
	{
		_CancelScan();
	}
}
