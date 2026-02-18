// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3GameEditor.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_COMMAND(ID_OPTION, OnOption)
END_MESSAGE_MAP()


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	g_pGameFramework = NULL;
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE( g_pGameFramework);

	i3ElementBase::DumpObjects();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();

	m_PanelGame.Create( PanelGame::IDD, this);
	m_PanelGame.SetWindowText("Game Stage");
	DockWindow( &m_PanelGame, "L", 100, 100);

	m_PanelControl.Create( PanelControl::IDD, this);
	m_PanelControl.SetWindowText( "Control View");
	DockWindow( &m_PanelControl, "RL", 100, 100);

	m_PanelSpec.Create( PanelSpec::IDD, this);
	m_PanelSpec.SetWindowText( "Specifications");
	DockWindow( &m_PanelSpec, "RRU", 100, 100);

	m_PanelLog.Create( PanelLog::IDD, this);
	m_PanelLog.SetWindowText( "Log");
	DockWindow( &m_PanelLog, "RRD", 100, 100);	
	
	LoadDockContext();

	DragAcceptFiles( TRUE);

	return 0;
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

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 보기 창으로 포커스를 이동합니다.
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	
	// 그렇지 않으면 기본 처리합니다.
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnDestroy()
{
	//	Profile 저장
	g_Profile.OnSave();

	i3TDKDockFrameWnd::OnDestroy();	
}

void CMainFrame::OnFileNew()
{
	//if( g_pGameFramework->getStageInfoCount() == 0 && g_pGameFramework->getLayerCount() == 0)
	//	return;

	if( MessageBox( "모든 정보가 삭제 됩니다. 새로 하시겠습니까?", "WARNING!", MB_YESNO | MB_ICONWARNING) == IDNO)
		return;

	//	이전 프레임웍 제거
	I3_SAFE_RELEASE( g_pGameFramework);

	//	새로 생성
	g_pGameFramework = i3Framework::new_object();	
	__CHECKF_RT( g_pGameFramework == NULL, "Error! Fail to create new game framework.");

	//	게임 구조 초기화
	m_PanelGame.ResetGame();
	m_PanelSpec.ResetGame();
}

char _strTemp[256];

void CMainFrame::OnFileOpen()
{
	i3ResourceFile File;
	
	const INT32 nMaxLengh = 1024;

	char szFilePath[ nMaxLengh] = "";
	sprintf(szFilePath, "%s.%s", DEFAULT_GAME_NAME, FILE_EXT);

	OPENFILENAME Ofn;
	memset( &Ofn, 0,sizeof( OPENFILENAME));

	Ofn.lStructSize = sizeof( OPENFILENAME);
	Ofn.hwndOwner = this->m_hWnd;
	Ofn.lpstrFilter = "게임파일(*.i3Game)\0*.i3Game\0모든파일(*.*)\0*.*\0";
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFile = szFilePath;
	Ofn.nMaxFile = nMaxLengh;
	Ofn.lpstrDefExt = FILE_EXT;

	if( GetOpenFileName( &Ofn) != 0)
	{
		//	File load 
		if( OpenFile( szFilePath))
		{
			//	Rebuild TreeCtrl in PanelGame
			m_PanelGame.BuildTreeCtrl( g_pGameFramework);

			sprintf( _strTemp, "File Open ok. (%s)", szFilePath);				
		}
		else
		{
			sprintf( _strTemp, "Warning! File Open fail. (%s)", szFilePath);	
		}

		m_PanelSpec.ResetGame();

		I3PRINTLOG(I3LOG_NOTICE, _strTemp);			
	}
}

void CMainFrame::OnFileSave()
{
	//	file save
	if( SaveFile( m_szWorkFileName) == TRUE)
	{
		sprintf( _strTemp, "File Save ok. (%s)", m_szWorkFileName);	
	}
	else
	{
		sprintf( _strTemp, "Warning! File Save fail. (%s)", m_szWorkFileName);	
	}

	I3PRINTLOG(I3LOG_NOTICE, _strTemp);	
}

void CMainFrame::OnFileSaveAs()
{	
	const INT32 nMaxLengh = 1024;

	char szFilePath[ nMaxLengh] = "";
	sprintf(szFilePath, "%s.%s", DEFAULT_GAME_NAME, FILE_EXT);

	OPENFILENAME Ofn;
	memset( &Ofn, 0,sizeof( OPENFILENAME));

	Ofn.lStructSize = sizeof( OPENFILENAME);
	Ofn.hwndOwner = this->m_hWnd;
	Ofn.lpstrFilter = "게임파일(*.i3Game)\0*.i3Game\0모든파일(*.*)\0*.*\0";
	Ofn.nFilterIndex = 1;
	Ofn.lpstrFile = szFilePath;
	Ofn.nMaxFile = nMaxLengh;
	Ofn.lpstrDefExt = FILE_EXT;
	Ofn.Flags = OFN_OVERWRITEPROMPT;

	if( GetSaveFileName( &Ofn) != 0)
	{
		//	file save
		if( SaveFile( szFilePath) == TRUE)
		{
			sprintf( _strTemp, "File Save As ok. (%s)", szFilePath);	
		}
		else
		{
			sprintf( _strTemp, "Warning! File Save As fail. (%s)", szFilePath);	
		}
		
		I3PRINTLOG(I3LOG_NOTICE, _strTemp);	
	}
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	char szFileName[256];
	INT32 count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	if( count > 1)	//	Drop된 파일이 1개 이상이면 에러.
	{
		//goto LABEL_OUT;
		return;
	}

	//	Drop된 파일 알아내기
	DragQueryFile( hDropInfo, 0, szFileName, sizeof( szFileName) -1);
	if( CheckFileName( szFileName, FILE_EXT) == FALSE)
	{
		sprintf( _strTemp, "Warning! Invalid dropped file. (%s)", szFileName);
		I3PRINTLOG(I3LOG_NOTICE, _strTemp);
		return;
		//goto LABEL_OUT;
	}

	//	File load 
	if( OpenFile( szFileName))
	{
		//	Rebuild TreeCtrl in PanelGame
		m_PanelGame.BuildTreeCtrl( g_pGameFramework);
		m_PanelSpec.ResetGame();
	
		sprintf( _strTemp, "File Open ok. (%s)", szFileName);
		I3PRINTLOG(I3LOG_NOTICE, _strTemp);
	}

//LABEL_OUT:
//	i3TDKDockFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::OnOption()
{
	OptionDialog Dlg;
	Dlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CMainFrame::Init( void)
{
	//	Profile 읽기
	g_Profile.OnLoad();
	
	g_pGameFramework = i3Framework::new_object();	
	__CHECKF_RT( g_pGameFramework == NULL, "Error! Fail to create main framework.");

	sprintf( m_szWorkFileName, "%s.%s", DEFAULT_GAME_NAME, FILE_EXT);	

}

BOOL CMainFrame::SaveFile( const char * pszFileName)
{
	i3ResourceFile File;

	File.setKeyObject( g_pGameFramework);

	if( File.Save( pszFileName ) > 0)
	{
		i3::safe_string_copy( m_szWorkFileName, pszFileName, 256 );

		//	Registry
		RegData data;
		data._pi3Framework = g_pGameFramework;
		i3::safe_string_copy( data._szWorkingDir, g_Profile.GetWorkDir(), 256);

		i3TDKGameRegistry::Make( &data);
		i3TDKGameRegistry::Save( pszFileName);

		return TRUE;
	}
	
	return FALSE;
}

BOOL CMainFrame::OpenFile( const char * pszFileName)
{
	__CHECKW_RTV( pszFileName == NULL, "Warning! Invalid FileName.", FALSE);

	i3ResourceFile File;
	i3Framework * pNewFramework = NULL;

	//	file load
	File.Load( pszFileName);

	//	new loaded framework
	pNewFramework = (i3Framework *) File.getKeyObject();
	
	//	error check
	sprintf( _strTemp, "Error! Invalid File. Cannot open (%s) ", pszFileName);
	__CHECKW_RTV(  !i3::kind_of<i3Framework*>(pNewFramework) //->IsTypeOf( i3Framework::static_meta()) == FALSE)
		, _strTemp, FALSE);

	I3_SAFE_RELEASE( g_pGameFramework);
	g_pGameFramework = pNewFramework;
	g_pGameFramework->AddRef();

	i3::safe_string_copy( m_szWorkFileName, pszFileName, 256 );

	return TRUE;
}

BOOL CMainFrame::CheckFileName( const char * pszFileName, char * pszExt)
{
	char Ext[32];

	__CHECKW_RTV( (pszFileName == NULL) || (pszExt == NULL), "Warning! Invalid FileName.", FALSE);

//	i3String::SplitFileExt(pszFileName, Ext, sizeof(Ext) - 1);
	i3::extract_fileext(pszFileName, Ext);

	if( i3::generic_is_iequal( (char *)Ext, (char *) pszExt) == false)
	{
		//File이 아니다. 
		return FALSE; 
	}
	return TRUE; 
}
