// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "MainFrm.h"
#include "GlobalVar.h"
#include "DlgOption.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/generic_string_cat.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_OPTION, OnFileOption)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	g_szPath[0] = 0;
	g_szVolumeName[0] = 0;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//_CrtSetBreakAlloc(1269);
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
		g_pSysImageList = i3TDK::GetSystemImageList();
	}

	{
		m_PanelTreeLive.Create( CPanelTreeLive::IDD, this);
		m_PanelTreeLive.SetWindowText( "Pack File");
		DockWindow( &m_PanelTreeLive, "L", 50, 50);

		m_PanelFileInfo.Create( CPanelFileInfo::IDD, this);
		m_PanelFileInfo.SetWindowText( "Property");
		DockWindow( &m_PanelFileInfo, "L", 50, 50);

		m_PanelFileList.Create( CPanelFileList::IDD, this);
		m_PanelFileList.SetWindowText( "Files");
		DockWindow( &m_PanelFileList, "L", 50, 50);

		m_PanelTree.Create( CPanelTree::IDD, this);
		m_PanelTree.SetWindowText( "Directory");
		DockWindow( &m_PanelTree, "L", 50, 50);

 		m_PanelProjcet.Create(CPanelListProjcet::IDD, this);
 		m_PanelProjcet.SetWindowText("Project");
 		DockWindow(&m_PanelProjcet, "L", 50, 50);

		//m_PanelLog.Create( CPanelLog::IDD, this);
		//m_PanelLog.SetWindowText( "Log");
		//DockWindow( &m_PanelLog, "LD", 100, 100);

		m_PanelTreeScript.Create( CPanelTreeScript::IDD, this);
		m_PanelTreeScript.SetWindowText( "i3Pack Script");
		DockWindow( &m_PanelTreeScript, "R", 100, 100);

//		m_PanelProperty.Create( CPanelProperty::IDD, this);
//		m_PanelProperty.SetWindowText( "i3Pack Property");
//		DockWindow( &m_PanelProperty, "RD", 10, 10);



	}

	LoadDockContext();

	NewPack();

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

void CMainFrame::OnDestroy()
{
	if( g_pOption != NULL)
	{
		delete g_pOption;
		g_pOption = NULL;
	}

	if( g_pSysImageList != NULL)
	{
		delete g_pSysImageList;
		g_pSysImageList = NULL;
	}

	i3TDKDockFrameWnd::OnDestroy();
	i3TDKGlobalRes::Close();

	ClosePack();
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
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::NewPack(void)
{
	I3ASSERT( g_pPack == NULL);

	g_pPack = i3PackNode::new_object();
	g_pPack->SetName( "/");

	m_PanelTree.UpdateAll();
}

void CMainFrame::ClosePack(void)
{
	if(g_pPack)
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pPack, NULL);
	I3_SAFE_RELEASE( g_pPack);
	m_PanelTree.UpdateAll();
}

void CMainFrame::LoadPack( const char * pszPath)
{
	i3PackVolume file;

	if( file.LoadFromFile( pszPath) == false)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", pszPath);
		return;
	}

	i3PackNode * pNode = (i3PackNode *) file.getRoot();

	if(!i3::same_of<i3PackNode*>(pNode)) //->IsExactTypeOf( i3PackNode::static_meta()) == false )
	{
		NotifyBox( m_hWnd, "%s 파일을 올바른 i3Pack 파일이 아닙니다.", pszPath);
		return;
	}

	I3_REF_CHANGE( g_pPack, pNode);

	strcpy( g_szPath, pszPath);
	strcpy( g_szVolumeName, pszPath);

	{
		char conv[MAX_PATH + 32];

		sprintf( conv, "i3PackTool - %s", pszPath);
		SetTitle( conv);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pPack);
}

void CMainFrame::SavePack( const char * pszPath , const char * folderPath)
{
	i3PackVolume file;

	if( g_pPack == NULL)
		return;

	file.setRoot( g_pPack);

	// Working Directory가 맞아야 한다.
	char szOldDir[MAX_PATH];

	GetCurrentDirectory( sizeof(szOldDir), szOldDir);
	SetCurrentDirectory( g_pOption->m_szWorkPath);


	bool iscreate = false;

	if( folderPath == NULL)
		iscreate = file.SaveToFile( pszPath);
	else
		iscreate = file.SaveToFile( folderPath);
		


	if( iscreate == false)
	{
		NotifyBox( m_hWnd, "%s 파일을 저장할 수 없습니다.", pszPath);
		return;
	}

	strcpy( g_szPath, pszPath);
	strcpy( g_szVolumeName, pszPath);

	SetCurrentDirectory( szOldDir);
	
	{
		char conv[MAX_PATH + 32];

		sprintf( conv, "i3PackTool - %s", pszPath);
		SetTitle( conv);
	}
}

void CMainFrame::OnFileNew()
{
	NewPack();
}

void CMainFrame::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "i3Pack", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"i3Pack Volume File(*.i3Pack)|*.i3Pack||", this);

	if( Dlg.DoModal() != IDOK) return;
	
	LoadPack( (const char *) LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnFileSave()
{
	if( g_szPath[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	SavePack( g_szPath);
}

void CMainFrame::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3Pack", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"i3Pack Volume File(*.i3Pack)|*.i3Pack||", this);

	if( Dlg.DoModal() != IDOK)return;

	SavePack( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnFileOption()
{
	CDlgOption dlg;

	dlg.DoModal();
}
void CMainFrame::OnFileExport()
{

	m_PanelTree.OnExport();

}
void CMainFrame::Add( const char * pszPath, BOOL IncludeSubDir)
{
	m_PanelTree.Add( pszPath, IncludeSubDir);
}

void CMainFrame::Remove( const char * pszPath, const BOOL includeSubDirOK)
{
	m_PanelTree.Remove( pszPath, includeSubDirOK);
}

void CMainFrame::EncFile( const char * pszPath)
{
	if( g_pPack == NULL)
		return;

	CFileFind	FindCtx;

	BOOL IsFind = FindCtx.FindFile(pszPath);
	while(IsFind)
	{
		IsFind = FindCtx.FindNextFile();

		// 상대 Path로 만든다.
		char RelPath[512] = {};
		i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR(FindCtx.GetFilePath()), RelPath );

		I3_PACK_FILE_REF * pFile;
		i3PackNode * pNode = g_pPack->FindPath( RelPath, &pFile);

		if((pNode != NULL) && (pFile != NULL))
		{
			pFile->m_Attr |= I3_PACK_FILE_ATTR_SECURE;
		}
	}
}

void	CMainFrame::SetChecksum( const char * pszPath, const char * folderPath)
{
	// Working Directory가 맞아야 한다.
	char szFullPath[MAX_PATH] = "";

	if(folderPath == NULL)
	{
		i3::safe_string_copy( szFullPath, g_pOption->m_szWorkPath, MAX_PATH );
		i3::generic_string_cat( szFullPath, "\\");
		i3::generic_string_cat( szFullPath, pszPath);
	}
	else
	{
		i3::safe_string_copy(szFullPath, folderPath, MAX_PATH);
	}


	i3ResourceFile::SetProtectionCode( szFullPath, "Zepetto Inc.", 12);
}
