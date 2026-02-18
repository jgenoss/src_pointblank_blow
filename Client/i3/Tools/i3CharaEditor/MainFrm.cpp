// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3CharaEditor.h"

#include "MainFrm.h"
#include "GlobalVar.h"
#include "DlgOption.h"
#include "DlgExport.h"
#include "DlgRefResCpy.h"

#include ".\mainfrm.h"
#include "Dlg_Merge_1PV_3PV.h"
#include "DlgDumpAnim.h"

#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/compare/generic_compare.h"

#ifdef _USE_TOOLAM_
#include "Automate/DlgNogadaMask.h"
#include "Automate/DlgNogadaAIState.h"
#include "DlgAiAni.h"
#endif


template<class T>
static T* DoDailogAsModeless(CWnd* parentCWnd, void (*OnDlgInitBeforeCallBack)(void* ), int nWindowState)
{
	T* modeless = new T(parentCWnd);
	
	if(OnDlgInitBeforeCallBack)
		(OnDlgInitBeforeCallBack)(modeless);

	modeless->Create(T::IDD, CWnd::GetDesktopWindow());	
	modeless->ShowWindow(SW_SHOW);
	//modeless->ShowWindow(nWindowState);

	return modeless;
}


#include <list>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOL_OPTION, OnToolOption)
	ON_COMMAND(ID_FILE_EXPORTI3CHR, OnFileExporti3chr)
	ON_COMMAND(ID_AUTOSET_COLLIDEE, OnAutosetCollidee)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LINERENDER, OnUpdateViewLinerender)
	ON_COMMAND(ID_VIEW_LINERENDER, OnViewLinerender)
	ON_COMMAND(ID_TOOLS_CHECKBONES, OnToolsCheckbones)
	ON_COMMAND(ID_BUILD_ANIMPACK, OnBuildAnimpack)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_TOOL_DUMP_AI, OnToolDumpAi)
	ON_COMMAND(ID_CONVERT_ANIM_TEMPLATE, &CMainFrame::OnConvertAnimTemplate)
	ON_COMMAND(ID_MERGE_1PV_3PV, &CMainFrame::OnMerge1pv3pv)
	ON_COMMAND(ID_COPY_REF_FILE, &CMainFrame::OnCopyRefFile)
#ifdef _USE_TOOLAM_
	ON_COMMAND(ID_POPUPMENUITEM_NOGADA_MASK, &CMainFrame::OnNogadaMask)
	ON_COMMAND(ID_POPUPMENUITEM_NOGADA_AISTATE, &CMainFrame::OnNogadaAIState)
	ON_COMMAND(ID_32817, &CMainFrame::OnNogadaAniAI)
#endif
	ON_COMMAND(ID_TOOL_OUTPUT_SHAREDANIM, &CMainFrame::OnDumpSharedAnim)
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
	g_pChara = NULL;
	m_szFilePath[0] = 0;
	//m_bBackGroundWindow = false;
	//m_nMaskSaveCount = 0;
}

CMainFrame::~CMainFrame()
{
	I3_SAFE_RELEASE( g_pChara);

	m_PanelTimeTrack.DestroyWindow();			// 경고는 제거되는데 메모리릭은 제거되지 않아서 문제의 원인은 아님..
	m_PanelSpec.DestroyWindow();				// MFC는 CWnd계열객체일때, m_ParentWin이 없는 경우 명시적 삭제를 요구하는것 같다..
	m_PanelSceneGraph.DestroyWindow();			// (2012.09.07.수빈)
	m_PanelResource.DestroyWindow();
	m_PanelRender.DestroyWindow();
	m_PanelLuaEditor.DestroyWindow();
	m_PanelLog.DestroyWindow();
	m_PanelAIGraph.DestroyWindow();	
}

typedef void	(*TESTPROC)( void * pData);

#define		REPEAT_COUNT		500000
#define		ITER_COUNT			50000

void	TestPerf( TESTPROC pProc, void * pData, const char * pszMsg, INT32 cnt)
{
	INT64	timeStart, timeElapsed;

	timeStart = i3Timer::GetPerformanceTime();

	INT32 i;

	for( i = 0; i < cnt; i++)
	{
		pProc( pData);
	}

	timeElapsed = i3Timer::GetPerformanceTime() - timeStart;

	I3TRACE( "%s : %f ms\n", pszMsg, timeElapsed * i3Timer::GetProfileResolution());
}
/* 쓰이지 않아서 주석..
void	List_Add( void * pData)
{
	List * pList = (List *) pData;

	pList->Add( (void *) rand());
}
*/
/* 쓰이지 않아 주석..
void	List_Iteration( void * pData)
{
	INT32 i, rv;
	List * pList = (List *) pData;

	rv = 0;

	for( i = 0; i < ITER_COUNT; i++)
	{
		rv += (INT32) pList->Items[i];
	}

	//I3TRACE( "Interation : %d\n", rv);
}
*/
/* 쓰이지 않아 주석..
void	Vector_Add( void * pData)
{
	std::vector<INT32> * pList = (std::vector<INT32> *) pData;

	pList->push_back( rand());
}

void	Vector_Iteration( void * pData)
{
	INT32 rv;
	std::vector<INT32>::iterator iter;
	std::vector<INT32> * pList = (std::vector<INT32> *) pData;

	rv = 0;

	for( iter = pList->begin(); iter != pList->end(); iter++)
	{
		rv += (INT32) *iter;
	}
}
*/

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	/*
	{
		List list;
		std::vector<INT32> Vector;

		TestPerf( List_Add, &list, "List Add", 50000);
		TestPerf( List_Iteration, &list, "List Iteration", 4);

		TestPerf( Vector_Add, &Vector, "Vector Add", 50000);
		TestPerf( Vector_Iteration, &Vector, "Vector Iteration", 4);
	}
	*/

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
		g_pOption = new COptionInfo;
	}

	{
		m_PanelResource.Create( CPanelResource::IDD, this);
		m_PanelResource.SetWindowText( "Resource");
		DockWindow( &m_PanelResource, "L", 100, 100);

		m_PanelAIGraph.Create( CPanelAIGraph::IDD, this);
		m_PanelAIGraph.SetWindowText( "AI Graph");
		DockWindow( &m_PanelAIGraph, "LD", 100, 100);

		m_PanelRender.Create( CPanelRender::IDD, this);
		m_PanelRender.SetWindowText( "Preview");
		DockWindow( &m_PanelRender, "R", 100, 100);

		m_PanelTimeTrack.Create( CPanelTimeTrack::IDD, this);
		m_PanelTimeTrack.SetWindowText( "Tracks");
		DockWindow( &m_PanelTimeTrack, "RD", 100, 100);

		m_PanelSceneGraph.Create( CPanelSceneGraph::IDD, this);
		m_PanelSceneGraph.SetWindowText( "Scene Graph");
		DockWindow( &m_PanelSceneGraph, "RR", 100, 100);

		m_PanelLog.Create( CPanelLog::IDD, this);
		m_PanelLog.SetWindowText( "Log");
		DockWindow( &m_PanelLog, "RDD", 100, 100);


		m_PanelLuaEditor.Create( CPanelLuaEditor::IDD, this);
		m_PanelLuaEditor.SetWindowText( "Lua Editor");
		DockWindow( &m_PanelLuaEditor, "RRD", 100, 100);

		m_PanelSpec.Create( CPanelSpec::IDD, this);
		m_PanelSpec.SetWindowText( "Specifications");
		DockWindow( &m_PanelSpec, "RRDD", 100, 100);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	OnFileNew();


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
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnFileNew()
{
	OnNewChara();
}

void		CMainFrame::OnNewChara(void)
{
	OnCloseChara();

	g_pChara = CChara::new_object();

	g_pFramework->AddChild( g_pChara->geti3Chara());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_ADD, g_pChara);
}

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

void		CMainFrame::OnOpenChara(const char * pszPath)
{
	OnCloseChara();
	i3TDK::SetPendingUpdateState(TRUE);

	CChara* pChara;
	char szExt[MAX_PATH];
	char szPath[MAX_PATH];
	i3::extract_fileext(pszPath, szExt);
	if (i3::generic_compare(szExt, "i3CharaEditor") == 0)
	{
		i3ResourceFile file;
		NormalizeCmdArg(szPath, pszPath);

		if (file.Load(pszPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", pszPath);
			return;
		}

		pChara = (CChara *)file.getKeyObject();

		g_pChara = pChara;
		g_pChara->AddRef();

		g_pFramework->AddChild(g_pChara->geti3Chara());

		g_pChara->geti3Chara()->Instantiate();

		i3TDK::SetPendingUpdateState(FALSE);


		i3TDK::Update(m_hWnd, I3_TDK_UPDATE_ADD, g_pChara);
	}
	else
	{
		i3XMLFile file;
		NormalizeCmdArg(szPath, pszPath);
		if (file.Load(pszPath) == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일을 열 수 없습니다.", pszPath);
			return;
		}
		pChara = (CChara *)file.getKeyObject();
		if (!i3::kind_of<CChara*>(pChara)) //->IsTypeOf( CChara::static_meta()) == FALSE)
		{
			I3PRINTLOG(I3LOG_WARN, "%s 파일은 올바른 i3CharaEditor 파일이 아닙니다.\n", pszPath);
			return;
		}

		g_pChara = pChara;
		g_pChara->AddRef();

		g_pFramework->AddChild(g_pChara->geti3Chara());

		g_pChara->geti3Chara()->Instantiate();

		i3TDK::SetPendingUpdateState(FALSE);


		i3TDK::Update(m_hWnd, I3_TDK_UPDATE_ADD, g_pChara);
	}

	i3::safe_string_copy(m_szFilePath, pszPath, MAX_PATH);
	g_pChara->SetFilePath(m_szFilePath);
}

void		CMainFrame::OnSaveChara( const char * pszPath)
{
	i3ResourceFile file;

	if( g_pChara == NULL)
		return;

	file.setKeyObject( g_pChara);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN, "%s 파일을 저장할 수 없습니다.", pszPath);
		return;
	}

	i3XMLFile xmlFile;
	xmlFile.setKeyObject(g_pChara);

	i3::change_file_ext((char*)pszPath, "i3CharaXML");

	if (xmlFile.Save(pszPath) == false)
	{
		I3PRINTLOG(I3LOG_WARN, "%s 파일을 저장할 수 없습니다.", pszPath);
		return;
	}
}

void		CMainFrame::OnCloseChara(void)
{
	if( g_pChara != NULL)
	{
		g_pFramework->RemoveChild( g_pChara->geti3Chara());
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, g_pChara);
		I3_SAFE_RELEASE( g_pChara);
	}
}

void CMainFrame::OnFileOpen()
{
	CFileDialog	Dlg(TRUE, "i3CharaXML", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"i3CharaEditor|*.i3CharaEditor;*.i3CharaXML||", this);

	if (Dlg.DoModal() != IDOK) return;

	OnOpenChara(LPCTSTR(Dlg.GetPathName()));
}

void CMainFrame::OnFileSave()
{
	if( m_szFilePath[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	OnSaveChara( m_szFilePath);
}

void CMainFrame::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "i3CharaEditor", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3CharaEditor Project File(*.i3CharaEditor)|*.i3CharaEditor||", this);

	if( Dlg.DoModal() != IDOK)return;

	OnSaveChara( LPCTSTR( Dlg.GetPathName()));
}

bool compare_nocase (string first, string second)
{
	unsigned int i=0;
	while ( (i<first.length()) && (i<second.length()) )
	{
		if (tolower(first[i])<tolower(second[i]))
			return true;
		else if (tolower(first[i])>tolower(second[i]))
			return false;
		++i;
	}

	if (first.length()<second.length())
		return true;
	else
		return false;
}


void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];
	

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	std::list<std::string>	FileList;
	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);
		FileList.push_back(TempName);
	}

	FileList.sort(compare_nocase);

	for (std::list<std::string>::iterator iter = FileList.begin() ; iter != FileList.end() ; ++iter)
	{
		I3TRACE2("%d - %s\n\n", ++i, iter->c_str());
		const char * pName = iter->c_str();
		LoadResFile(pName);
	}

	DragFinish( hDropInfo);
}

bool CMainFrame::LoadResFile(const char* resPath, bool silence)
{
	char Ext[32];

	//	i3String::SplitFileExt( pName, Ext, sizeof(Ext) - 1);
	i3::extract_fileext(resPath, Ext);

	if(!PathFileExists(resPath))
	{
		if(!silence)
		{
			std::string fileNotExistMsg = resPath + std::string(" is not exist.. Failed to load..");
			AfxMessageBox(fileNotExistMsg.c_str(), MB_ICONERROR);
		}
		return false;
	}

	if( i3::generic_is_iequal( Ext, "I3S") )
	{		
		return g_pChara->LoadSceneGraph( resPath);
	}
	else if( i3::generic_is_iequal( Ext, "i3A") )
	{
		return g_pChara->LoadAnim( resPath);
	}
	else if(	i3::generic_is_iequal( Ext, "WAV")  ||
		i3::generic_is_iequal( Ext, "I3SND") )
	{
		return g_pChara->LoadSound( resPath);
	}
	else if( i3::generic_is_iequal( Ext, "i3CharaEditor") || i3::generic_is_iequal(Ext, "i3CharaXML"))
	{
		OnOpenChara(resPath);
		return true;
	}

	return false;

}

void CMainFrame::removeAllSceneGraph()
{
	g_pChara->RemoveAllSceneGraph();
}

void CMainFrame::OnDestroy()
{
	if( g_pOption != NULL)
	{
		delete g_pOption;
		g_pOption = NULL;
		//I3_SAFE_RELEASE( g_pOption);
	}

	OnCloseChara();

	i3TDKDockFrameWnd::OnDestroy();
	i3TDKGlobalRes::Close();
}

void CMainFrame::OnToolOption()
{
	CDlgOption dlg;

	dlg.DoModal();
}

void CMainFrame::OnFileExporti3chr()
{
	char szInitName[MAX_PATH];

	i3::safe_string_copy( szInitName, m_szFilePath, MAX_PATH );

	i3::remove_ext( szInitName);
	i3::generic_string_cat( szInitName, ".I3CHR");

	CFileDialog	Dlg( FALSE, "I3CHR", szInitName, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Character File(*.I3CHR)|*.I3CHR||", this, 0, TRUE);	

	if( Dlg.DoModal() == IDCANCEL)
		return;

	Exporti3Chr(Dlg.GetPathName());
}

bool CMainFrame::Exporti3Chr(const char* path, COptionInfo* tmpExpOpt)
{
	if(!path)
		return false;

	CDlgExport exportDlg;
	COptionInfo* preserveOpt = NULL;
	char filenameOnly[MAX_PATH] = {0};
	char rstMsgBuf[MAX_PATH << 1] = {0};
	char* rstMsg = NULL;

	if(tmpExpOpt)
	{		
		// preserve previous option
		preserveOpt = g_pOption->Clone();
		g_pOption->Apply(tmpExpOpt, true);

		i3::extract_filetitle(path, filenameOnly);

		rstMsg = rstMsgBuf;
		strcpy_s(rstMsgBuf, MAX_PATH << 1, path);

		while(*rstMsg != NULL) rstMsg++;
	}
	else
	{
		if(exportDlg.Execute(path) == false)
			return false;

		if(exportDlg.getI3SName())
			strcpy_s(filenameOnly, MAX_PATH, exportDlg.getI3SName());
	}
	
	bool rst = g_pChara->Export(path, filenameOnly, rstMsg);

	if(rstMsg)
	{
		OutputDebugString(rstMsg);
		OutputDebugString("\n");
	}

	if(preserveOpt)
	{
		g_pOption->Apply(preserveOpt);

		delete preserveOpt;
		preserveOpt = NULL;
	}

	/*if( m_bBackGroundWindow )
	{
		std::vector<std::string> heroNames;
		m_pMaskWin->GetCheckedHeroNames(heroNames);

		m_nMaskSaveCount++;

		if(m_nMaskSaveCount >= int(heroNames.size()))
		{
			exit(1);
		}
	}*/

	return rst;
}


LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_EXITSIZEMOVE :
			if( g_pViewer != NULL)
			{
				g_pViewer->UpdateDevice();
			}
			break;

	}

	return i3TDKDockFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::OnAutosetCollidee()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Scene File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	//g_pChara->BuildCollider( LPCTSTR( Dlg.GetPathName()));
}

void CMainFrame::OnUpdateViewLinerender(CCmdUI *pCmdUI)
{
	if( g_pChara->getRenderMode() == I3G_RENDER_LINE)
		pCmdUI->SetCheck( 1);
	else
		pCmdUI->SetCheck( 0);
}

void CMainFrame::OnViewLinerender()
{
	if( g_pChara->getRenderMode() == I3G_RENDER_LINE)
		g_pChara->setRenderMode( I3G_RENDER_POLYGON);
	else
		g_pChara->setRenderMode( I3G_RENDER_LINE);
}

void CMainFrame::OnToolsCheckbones()
{
	g_pChara->CheckBones();
}

void CMainFrame::OnBuildAnimpack()
{
	CFileDialog	Dlg( FALSE, "i3AnimPack", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Animation Pack File(*.i3AnimPack)|*.i3AnimPack||", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;

	g_pChara->BuildAnimPackFile( (char *) LPCTSTR( Dlg.GetPathName()));
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}
	else if( pMsg->message == WM_CHAR)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}

	return i3TDKDockFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if( nChar == VK_LEFT)
	{
		I3TRACE( "Left\n");
	}

	i3TDKDockFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMainFrame::OnToolDumpAi()
{
	g_pChara->DumpAI();
}

void CMainFrame::OnConvertAnimTemplate()
{
	g_pChara->ConvertToAnimTemplate();
}

void CMainFrame::OnMerge1pv3pv()
{
	CDlg_Merge_1PV_3PV dlg;

	dlg.DoModal();
}

void CMainFrame::OnCopyRefFile()
{
	CDlgRefResCpy dlg;

	strcpy_s(dlg.m_ownerWorkDir, MAX_PATH, g_pOption->m_szWorkPath);
	
	if(strlen(m_szFilePath) > 0)
		strcpy_s(dlg.m_filePath, MAX_PATH, m_szFilePath);
		
	dlg.DoModal();
}


//////////////////////////////////////////////////////////////////////////
#ifdef _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////

void CMainFrame::OnNogadaMask()
{
	static bool i3TOOLAM_Init = false;

	if(!i3TOOLAM_Init)
	{
		TOOLAM_CMDQUEUE_MFC_INSTALL(ToolAM::CharTool::QNAME)
		i3TOOLAM_Init = true;
	}

	struct CDlgNogadaMaskOnInitFunctor 
	{
		static void CDlgNogadaMaskOnInit(void* dlg)
		{
			CDlgNogadaMask* impl = (CDlgNogadaMask*)dlg;
			strcpy_s(impl->m_ownerWorkDir, MAX_PATH, g_pOption->m_szWorkPath);
		}
	};
	
	CDlgNogadaMask* dlg = DoDailogAsModeless<CDlgNogadaMask>(this, 
		&CDlgNogadaMaskOnInitFunctor::CDlgNogadaMaskOnInit, SW_SHOW);

	dlg->SetForegroundWindow();
}

//void CMainFrame::GenerateMask(LPWSTR* argv)
//{
//	char temp[256] = {0,};
//	int nLen = ( int )wcslen( argv[1] ); 
//	wcstombs( temp, argv[1], nLen + 1 );
//
//	strcpy_s(g_pOption->m_szWorkPath, 256, temp);
//
//	static bool i3TOOLAM_Init = false;
//
//	if(!i3TOOLAM_Init)
//	{
//		TOOLAM_CMDQUEUE_MFC_INSTALL(ToolAM::CharTool::QNAME)
//			i3TOOLAM_Init = true;
//	}
//
//	struct CDlgNogadaMaskOnInitFunctor 
//	{
//		static void CDlgNogadaMaskOnInit(void* dlg)
//		{
//			CDlgNogadaMask* impl = (CDlgNogadaMask*)dlg;
//			strcpy_s(impl->m_ownerWorkDir, 256, g_pOption->m_szWorkPath);
//		}
//	};
//
//	m_pMaskWin = DoDailogAsModeless<CDlgNogadaMask>(this, 
//		&CDlgNogadaMaskOnInitFunctor::CDlgNogadaMaskOnInit, SW_HIDE);
//
//	nLen = ( int )wcslen( argv[2] ); 
//	wcstombs( temp, argv[2], nLen + 1 );
//
//	m_pMaskWin->GenerateMask(temp);
//
//	
//	m_bBackGroundWindow = true;
//}
void CMainFrame::OnNogadaAIState()
{
	AfxMessageBox("아직만드는중..");
	return;
//  윗줄 리턴문때문에 아랫줄들이 경고가 나와 모두 주석처리..
//	AIStateEdit::DlgNogadaAIState* dlg = DoDailogAsModeless<AIStateEdit::DlgNogadaAIState>(this,NULL, SW_SHOW);
//  dlg->SetForegroundWindow();
}
void CMainFrame::OnNogadaAniAI()
{
	//const char* filepath = g_pChara->GetFilePath();

	//if(filepath && *filepath != NULL)
	//{
	//	if(AfxMessageBox("현재 열어놓은 Chara파일이 있습니다. 저장하시겠습니까?", MB_YESNO) != IDYES)
	//		return;

	//	OnFileSave();
	//	OnCloseChara();
	//}

	//CString path; path.Format("%s\\Chara\\SWAT_Male\\SWAT_Male.i3CharaEditor", g_pOption->m_szWorkPath);
	//CString msg = " 을 로딩해야합니다. 진행하시겠습니까?";

	//if(AfxMessageBox(path+msg, MB_YESNO) != IDYES)
	//	return;

	//OnOpenChara(path);

	CDlgAiAni dlg;
	dlg.DoModal();
}
//
// AIStateEditCallBack
//
void CMainFrame::AIStateEditCB_ScanUpperWeaponClassName(AIStateEdit::WeaponClassScanData* out)
{
	out->Reset();

	i3Chara* chara = g_pChara->geti3Chara();

	if(!chara)
		return;

	AIStateEdit::RecursiveGenerateAICtxTree(chara, &out->m_aiStateTree);
}


//////////////////////////////////////////////////////////////////////////
#endif // end of -#ifdef _USE_TOOLAM_-
//////////////////////////////////////////////////////////////////////////


void CMainFrame::OnDumpSharedAnim( void)
{
	CDlgDumpAnim dlg;

	dlg.DoModal();
}
