// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3Viewer2.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "OptionInfo.h"
#include "DlgOption.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_TOOL_PROJECT_CONFIG, OnToolProjectConfig)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU0, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU1, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU2, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU3, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU4, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU5, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU6, OnUpdateFileMruDef)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU7, OnUpdateFileMruDef)
	ON_COMMAND(ID_FILE_MRU, OnFileMru)
	ON_COMMAND(ID_SETCAMERAPOSITIONS_CAMERA1, OnSetCameraPos1)
	ON_COMMAND(ID_SETCAMERAPOSITIONS_CAMERA2, OnSetCameraPos2)
	ON_COMMAND(ID_SETCAMERAPOSITIONS_CAMERA3, OnSetCameraPos3)
	ON_COMMAND(ID_SETCAMERAPOSITIONS_CAMERA4, OnSetCameraPos4)
	ON_COMMAND(ID_CAMPOS1, OnCameraPos1)
	ON_COMMAND(ID_CAMPOS2, OnCameraPos2)
	ON_COMMAND(ID_CAMPOS3, OnCameraPos3)
	ON_COMMAND(ID_CAMPOS4, OnCameraPos4)
	ON_COMMAND(ID_TOOL_OPTIONS, OnToolOptions)
	ON_WM_DROPFILES()
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_EDIT_GEOMETRYTOOL, OnEditGeometrytool)
	ON_COMMAND(ID_EDIT_TEXTOOL, OnEditTextool)
	ON_COMMAND(ID_VIEWMAPSCALE, OnViewMapScale)
END_MESSAGE_MAP()


static void NormalizeCmdArg( char *szOut, const char *szIn )
{
	while( *szIn )
	{
		if( *szIn != '"' )
		{
			*szOut = *szIn;
			szOut++;
		}
		szIn++;
	}
	*szOut = 0;
}

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	m_szFileName[0] = 0;
	m_pMRUPopup = NULL;
	m_pUI = NULL;
}

CMainFrame::~CMainFrame()
{
	if( m_pMRUPopup != NULL)
	{
		delete m_pMRUPopup;
		m_pMRUPopup = NULL;
	}

	I3_SAFE_RELEASE( m_pUI);
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	{
		HICON hIcon;
		HINSTANCE hInst = AfxGetApp()->m_hInstance;

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICONSM, (LONG) hIcon);

		hIcon = (HICON) ::LoadImage( hInst, MAKEINTRESOURCE( IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		::SetClassLong( m_hWnd, GCL_HICON, (LONG) hIcon);
	}

	{
		//g_pOption = new COptionInfo;
		g_pOption = COptionInfo::NewObject();
	}

	{
		i3TDKDock::SetPendingUpdate( true);

		m_Panel_Toolbar.Create( CPanel_Toolbar::IDD, this);
		m_Panel_Toolbar.SetWindowText( "Tools");
		DockWindow( &m_Panel_Toolbar, "U", 640, 32);

		m_Panel_Viewer.Create( CPanel_Viewer::IDD, this);
		m_Panel_Viewer.SetWindowText( "Preview");
		DockWindow( &m_Panel_Viewer, "D", 640, 480);

		m_Panel_Sg.Create( CPanel_SceneGraph::IDD, this);
		m_Panel_Sg.SetWindowText( "Scene-Graph");
		DockWindow( &m_Panel_Sg, "DR", 200, 320);

		m_Panel_Spec.Create( CPanel_Spec::IDD, this);
		m_Panel_Spec.SetWindowText( "Specification");
		DockWindow( &m_Panel_Spec, "DRD", 200, 100);

		i3TDKDock::SetPendingUpdate( false);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	return 0;
}

void CMainFrame::OnDestroy()
{
	i3TDKDockFrameWnd::OnDestroy();

	if( g_pOption != NULL)
	{
		g_pFramework->OnDestroy();

		delete g_pOption;
		g_pOption = NULL;
	}
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !i3TDKDockFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	i3TDKDockFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	i3TDKDockFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	i3TDKDockFrameWnd::OnSetFocus( pOldWnd);
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	BOOL bRv = i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	if( nCode == CN_UPDATE_COMMAND_UI)
	{
		CCmdUI * pUI = (CCmdUI *) pExtra;

		if((nID >= ID_FILE_MRU0) && (nID <= ID_FILE_MRU7))
			return TRUE;
	}

	return bRv;
}

void CMainFrame::OnToolProjectConfig()
{
	i3TDKDlgEnvSet dlg;

	dlg.Execute();
}

void CMainFrame::OnFileOpen()
{
	char szPath[ MAX_PATH];
	CString FilePath;
	CString FileName; 
	CFileDialog	Dlg( TRUE, "i3s|i3a|i3Uie", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Scene File(*.i3s)|*.i3s|i3Animation File(*.i3a)|*.i3a|i3UI File(*.i3Uie)|*.i3Uie|", this);

	if( Dlg.DoModal() != IDOK)return;

	FileName = Dlg.GetFileName();
	FilePath = Dlg.GetFolderPath();

	i3String::Copy( szPath, LPCTSTR( FilePath));
	i3String::Concat( szPath, "\\");
	i3String::Concat( szPath, LPCTSTR( FileName));

	/*if ( _checkFileName( (char *)LPCTSTR(FileName) ) == FALSE ) 
	{	
		MessageBox( "*.i3s only!", "Error" );
		return; 
	}*/

	//여기부터 실제 적인 씬 파일 로드 입니다. 
	/*CloseI3S(); 

	LoadI3S( (char *)LPCTSTR(FileName) ); */

	LoadFile( szPath);
}

void CMainFrame::OnFileSave()
{
	if( m_szFileName[0] != 0)
	{
		SaveI3S( m_szFileName);
		return;
	}

	OnFileSaveas();
}

void CMainFrame::OnFileSaveas()
{
	CString FileName; 
	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK )
	{
		//Message 
		return; 
	}

	FileName = Dlg.GetFileName(); 

	SaveI3S( (char*)LPCTSTR(FileName) ); 
}

void CMainFrame::_setCaption( const char * pszPath)
{
	if( pszPath == NULL )
	{
		strcpy( m_szFileName, "" );
	}
	else if( pszPath != m_szFileName)
	{
		strcpy( m_szFileName, pszPath ); 
	}

	if( m_szFileName[0] == 0 )
	{
		SetWindowText( "i3Viewer2 version 1.0" );
	}
	else
	{
		char szName[MAX_PATH << 1];
		sprintf( szName, "i3Viewer2 version 1.0 - %s", m_szFileName);
		SetWindowText( szName );
	}
}

bool CMainFrame::CloseI3S(void)
{
	g_pFramework->CloseI3S();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, NULL, i3SceneGraphInfo::GetClassMeta());

	_setCaption( "");

	return true;
}


bool CMainFrame::LoadI3S( const char * pszPath)
{
	char	szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);

	if( g_pFramework->LoadI3S( szPath) == false)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pFramework->getSgInfo());

	_setCaption( szPath);

	g_pOption->addMRU( szPath);

	return true;
}

bool CMainFrame::SaveI3S( const char * pszPath)
{
	if( g_pFramework->getSgInfo() == NULL)
		return false;

	if( g_pFramework->SaveI3S( pszPath) == false)
	{
		NotifyBox( m_hWnd, "%s 파일을 저장할 수 없습니다.", pszPath);
		return false;
	}
	
	_setCaption( pszPath);

	return true;
}

bool CMainFrame::LoadAnimation( const char * pszPath)
{
	char	szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);

	if( g_pFramework->LoadAnimation( (const char*) szPath) == false)
	{
		NotifyBox( m_hWnd, "Body가 없는 Sg. 이거나 파일을 열 수 없습니다.");
		return false;
	}
	
	return true;
}

bool CMainFrame::CloseUI(void)
{
	i3UIManager * pManager = i3UI::getManager();
	I3ASSERT( pManager != NULL);

	if( m_pUI != NULL)
	{
		m_pUI->Detach();

		pManager->CloseVirtualTexture();
		pManager->setUILibrary( NULL);

		I3_SAFE_RELEASE( m_pUI);
	}

	return true;
}

bool CMainFrame::LoadUI(const char * pszPath)
{
	i3UIManager * pManager = i3UI::getManager();
	I3ASSERT( pManager != NULL);

	i3UIScene * pUI = pManager->LoadUIScene((char*)pszPath);

	I3_REF_CHANGE( m_pUI, pUI);

	if( m_pUI != NULL)
	{
		m_pUI->Attach(true, AfxGetApp()->m_hInstance);
	}

	return true;
}

enumEngineFileType CMainFrame::_checkFileName( char * FileName)
{
	char Ext[32];

	i3String::SplitFileExt(FileName, Ext, sizeof(Ext) - 1);

	if( (i3String::Compare( (char *)Ext, (const char *)"i3s") == 0) ||
		(i3String::Compare( (char *)Ext, (const char *)"I3S") == 0) )
		return I3FILE_TYPE_SG;

	if( (i3String::Compare( (char*) Ext, (const char*)"i3a") == 0) ||
		(i3String::Compare( (char*) Ext, (const char*)"I3A") == 0) )
		return I3FILE_TYPE_ANIM;

	if( (i3String::Compare( (char*) Ext, (const char*)"i3Uie") == 0) ||
		(i3String::Compare( (char*) Ext, (const char*)"I3UIE") == 0) )
		return I3FILE_TYPE_UI;

	return I3FILE_TYPE_UNKNOWN;
}

bool CMainFrame::LoadFile( const char * pszPath)
{
	char szPath[ MAX_PATH];

	NormalizeCmdArg( szPath, pszPath);
	enumEngineFileType ck = _checkFileName( szPath );

	switch( ck)
	{
	case I3FILE_TYPE_SG :
		//여기부터 실제 적인 씬 파일 로드 입니다. 
		CloseI3S();
		return LoadI3S( szPath ); 

	case I3FILE_TYPE_ANIM :
		return LoadAnimation( szPath);

	case I3FILE_TYPE_UI :
		return LoadUI( szPath);

	default:
		MessageBox( "not support file type!", "Error" );
		break;
	}

	return false;
}

void CMainFrame::OnUpdateFileMruDef(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( TRUE);
}

void CMainFrame::OnFileMru()
{
	// Nothing to do...
}

void CMainFrame::OnSetCameraPos1()
{
	g_pFramework->RecordCam( 0);
}

void CMainFrame::OnSetCameraPos2()
{
	g_pFramework->RecordCam( 1);
}

void CMainFrame::OnSetCameraPos3()
{
	g_pFramework->RecordCam( 2);
}

void CMainFrame::OnSetCameraPos4()
{
	g_pFramework->RecordCam( 3);
}

void CMainFrame::OnCameraPos1()
{
	g_pFramework->SetCamPos( 0);
}

void CMainFrame::OnCameraPos2()
{
	g_pFramework->SetCamPos( 1);
}

void CMainFrame::OnCameraPos3()
{
	g_pFramework->SetCamPos( 2);
}

void CMainFrame::OnCameraPos4()
{
	g_pFramework->SetCamPos( 3);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT32 id = LOWORD( wParam);

	if( (id >= ID_FILE_MRU0) && (id <= ID_FILE_MRU7))
	{
		INT32 idx = id - ID_FILE_MRU0;

		char	szTemp[ MAX_PATH];

		i3String::Copy( szTemp, g_pOption->m_MRU[idx]);

		LoadFile( szTemp);
	}

	return i3TDKDockFrameWnd::OnCommand(wParam, lParam);
}

void CMainFrame::OnToolOptions()
{
	CDlgOption Dlg;

	Dlg.DoModal();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int count; 
	char FileName[256];

	count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	DragQueryFile( hDropInfo, 0, FileName, sizeof(FileName) - 1);

	LoadFile( (char *)LPCTSTR(FileName));
	/*if ( _checkFileName(FileName) == FALSE ) 
	{
		MessageBox( "*.i3s only!", "Error" );
		return;
	}

	//여기부터 실제 적인 씬 파일 로드 입니다. 
	CloseI3S(); 

	LoadI3S( (char *)LPCTSTR(FileName) ); 
	*/
}


void CMainFrame::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	i3TDKDockFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if( pPopupMenu->GetMenuItemID( 0) != ID_FILE_MRU0)
		return;

	CMenu * pMenu = GetMenu();
	INT32 i, cnt;

	cnt = g_pOption->getMRUCount();
	if( cnt <= 0)
	{
		pMenu->EnableMenuItem( ID_FILE_MRU, MF_BYCOMMAND | MF_GRAYED);
		return;
	}

	pMenu->EnableMenuItem( ID_FILE_MRU, MF_BYCOMMAND | MF_ENABLED);

	// 이전 MRU들을 삭제
	while( pPopupMenu->GetMenuItemCount() > 0)
	{
		pPopupMenu->RemoveMenu( 0, MF_BYPOSITION);
	}
	
	// 새 MRU List를 추가.
	for( i = 0; i < cnt; i++)
	{
		char conv[MAX_PATH << 1];

		sprintf( conv, "&%d\t\t%s", i +1, g_pOption->getMRU( i));
		pPopupMenu->AppendMenu( MF_STRING, (INT_PTR) ID_FILE_MRU0 + i, conv);
	}
}

void CMainFrame::OnEditGeometrytool()
{
	if( g_pFramework == NULL)
		return;


	if( g_pFramework->getSgInfo() == NULL)
		return;

	i3TDKDlgGeometryTool dlg;

	dlg.Execute( g_pFramework->getSgInfo()->getInstanceSg());
}

void CMainFrame::OnEditTextool()
{
	if( g_pFramework == NULL)
		return;

	if( g_pFramework->getSgInfo() == NULL)
		return;

	i3TDKDlgTextureTool dlg;

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();

	if( pInfo->getCommonSg() != NULL)
		dlg.Execute( pInfo->getCommonSg());
	else
		dlg.Execute( pInfo->getInstanceSg());
}

void CMainFrame::OnViewMapScale()
{
	// TODO: Add your command handler code here
	i3BoundBox Box;

	i3SceneGraphInfo * pInfo = g_pFramework->getSgInfo();
	if( pInfo == NULL)
	{
		MessageBox( "작업중인 파일이 없습니다.", "알림", MB_OK);
		return;
	}

	i3Node * pInstRoot = pInfo->getInstanceSg();

	pInstRoot->GetWrappingBound( &Box);

	REAL32 X = Box.GetMax()->x - Box.GetMin()->x;
	REAL32 Y = Box.GetMax()->y - Box.GetMin()->y;
	REAL32 Z = Box.GetMax()->z - Box.GetMin()->z;

	REAL32 scale = MAX( X, Y);
	scale = MAX( Z, scale);

	char conv[MAX_PATH];
	sprintf( conv, "X: %f, Y: %f, Z: %f\n\nMaxScale = %f\n\nMapScaleForMinimap: %f ", X, Y, Z, scale, MAX( X, Z));
	MessageBox( conv, "맵 크기", MB_OK);
}
