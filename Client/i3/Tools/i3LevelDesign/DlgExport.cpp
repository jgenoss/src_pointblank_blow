// DlgExport.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "DlgExport.h"
#include "ExportThread.h"
#include "GlobalVariable.h"
#include ".\dlgexport.h"

#include "i3Base/string/ext/extract_fileext.h"

// CDlgExport dialog

IMPLEMENT_DYNAMIC(CDlgExport, CDialog)
CDlgExport::CDlgExport(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExport::IDD, pParent)
{
	m_pThread = NULL;
}

CDlgExport::~CDlgExport()
{
	I3_SAFE_RELEASE( m_pThread);
}

void CDlgExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_STATE, m_StateTextCtrl);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
	DDX_Control(pDX, IDC_LOGLIST, m_LogCtrl);
	DDX_Control(pDX, IDC_ED_GAMEBIN, m_ED_GameBin);
	DDX_Control(pDX, IDC_CHK_EXPORT_RES, m_CHK_SerializeRes);
	DDX_Control(pDX, IDC_CHECK2, m_CHK_LaunchGame);
	DDX_Control(pDX, IDC_CB_STAGETYPE, m_CB_StageType);
}


BEGIN_MESSAGE_MAP(CDlgExport, CDialog)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHK_EXPORT_RES, OnBnClickedChkExportRes)
	ON_CBN_SELCHANGE(IDC_CB_STAGETYPE, OnCbnSelchangeCbStagetype)
END_MESSAGE_MAP()


// CDlgExport message handlers
static BOOL _FindGameBinProc( INT32 Level, char * pszPath, WIN32_FIND_DATA * pFileInfo, void * pUserData)
{
	char * pszBinName = (char *) pUserData;
	char szExt[128];

//	i3String::SplitFileExt( pFileInfo->cFileName, szExt, sizeof(szExt) - 1);
	i3::extract_fileext( pFileInfo->cFileName, szExt);

	if( i3::generic_is_iequal( szExt, "i3Exec") )
	{
		strcpy( pszBinName, pFileInfo->cFileName);
	}

	return TRUE;
}

BOOL CDlgExport::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LogCtrl.InsertColumn( 0, "Message", LVCFMT_LEFT, 1024);
	}

	{
		if( g_pProfile->m_bSerializeRes)
			m_CHK_SerializeRes.SetCheck( BST_CHECKED);

		if( g_pProfile->m_bLaunchGame)
			m_CHK_LaunchGame.SetCheck( BST_CHECKED);

		{
			char szPath[MAX_PATH];

			szPath[0] = 0;

			i3System::LookForFiles( g_pProfile->m_szWorkingDir, "*.*", (FINDFILEPROC) _FindGameBinProc, szPath, 0);

			m_ED_GameBin.SetWindowText( szPath);
		}
	}

	{
		i3::vector<i3ClassMeta*> list;
		
		i3ClassMeta * pMeta;

		i3Stage::static_meta()->get_all_derived_metas( list);

		m_CB_StageType.AddString( "i3Stage");

		for( size_t i = 0; i < list.size(); i++)
		{
			pMeta = list[i];

			m_CB_StageType.AddString( pMeta->class_name().c_str());
		}

		m_CB_StageType.SetWindowText( g_pProfile->m_szStageType);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgExport::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_EXPORT_STATE :
			m_StateTextCtrl.SetWindowText( (LPCTSTR) wParam);
			break;

		case WM_EXPORT_END :
			m_StateTextCtrl.SetWindowText( "Complete...");

			{
				INT32 lower, upper;

				m_ProgressCtrl.GetRange( lower, upper);
				m_ProgressCtrl.SetPos( upper);
			}

			I3ASSERT( m_pThread != NULL);

			I3_SAFE_RELEASE( m_pThread);

			_LaunchGame();
			break;

		case WM_EXPORT_PROGRESS_SET :
			m_ProgressCtrl.SetRange( 0, (short) wParam);
			m_ProgressCtrl.SetPos( 0);
			break;

		case WM_EXPORT_PROGRESS :
			m_ProgressCtrl.SetPos( (int) wParam);
			break;

		case WM_EXPORT_LOG :
			{
				INT32 idx = m_LogCtrl.GetItemCount();

				m_LogCtrl.InsertItem( idx, (LPCTSTR) wParam, 0);
				break;
			}
			break;

		case WM_EXPORT_ERROR :
			{
				NotifyBox( m_hWnd, (LPCTSTR) wParam);
			}
			break;

		case WM_EXPORT_CANCEL :
			{
				m_StateTextCtrl.SetWindowText( "Canceled...");

				m_pThread->Terminate();
				I3_SAFE_RELEASE( m_pThread);
			}
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CDlgExport::_LaunchGame(void)
{
	if( g_pProfile->m_bLaunchGame == FALSE)
		return;

	char szCmd[512];
	char szApp[512];
	char szBinName[256];

	m_ED_GameBin.GetWindowText( szBinName, sizeof(szBinName) - 1);

	if( szBinName[0] == 0)
	{
		NotifyBox( NULL, "Game을 Launch 시키기 위해서는 Binary Name을 지정해 주어야합니다.");
		return;
	}

	{
		m_CB_StageType.GetWindowText( g_pProfile->m_szStageType, sizeof(g_pProfile->m_szStageType) - 1);
	}

	sprintf( szApp, "D:\\i3\\bin\\i3Viewer.exe");
	sprintf( szCmd, "%s %s %s(%s)", szApp, szBinName, g_pScene->GetName(), g_pProfile->m_szStageType);

	{
		PROCESS_INFORMATION info;
		STARTUPINFO si;

		memset( &si, 0, sizeof(si));

		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_SHOW;

		if( CreateProcess( szApp, szCmd, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, g_pProfile->m_szWorkingDir, &si, &info) == FALSE)
		{
			NotifyBox( NULL, "Game을 Launch 시키지 못했습니다.");
			return;
		}

		if( info.hProcess != NULL)
		{
			WaitForSingleObject( info.hProcess, INFINITE);

			CloseHandle( info.hThread);
			CloseHandle( info.hProcess);
		}
	}
}

void CDlgExport::OnOK()
{
	// Resource DB를 Flush한다.
	if( g_pProfile->m_bSerializeRes)
	{
		g_pScene->PrepareSerialize();

		g_pResDB->FlushDB();
	}

	// Background Export Thread의 생성
	{
		m_pThread = CExportThread::new_object();

		m_pThread->setScene( g_pScene);
		m_pThread->setHWND( m_hWnd);
		m_pThread->setDestPath( g_pProfile->m_szWorkingDir);

		m_pThread->Create( "Export Thread");
	}
}

void CDlgExport::OnBnClickedCheck2()
{
	if( m_CHK_LaunchGame.GetCheck() == BST_CHECKED)
		g_pProfile->m_bLaunchGame = TRUE;
	else
		g_pProfile->m_bLaunchGame = FALSE;
}

void CDlgExport::OnBnClickedChkExportRes()
{
	if( m_CHK_SerializeRes.GetCheck() == BST_CHECKED)
		g_pProfile->m_bSerializeRes = TRUE;
	else
		g_pProfile->m_bSerializeRes = FALSE;
}

void CDlgExport::OnCbnSelchangeCbStagetype()
{
	INT32 idx = m_CB_StageType.GetCurSel();
	
	if( idx != -1)
	{
		m_CB_StageType.GetLBText( idx, g_pProfile->m_szStageType);
	}
}
