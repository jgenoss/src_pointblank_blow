// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3GuiEditor.h"
#include "GlobalVariable.h"
#include "GuiToolProject.h"
#include "i3GuiToolResManager.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "GetStringDlg.h"

#include "i3Base/string/ext/extract_fileext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, i3TDKDockFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, i3TDKDockFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE_PROJECT, OnFileSaveProject)
	ON_COMMAND(ID_FILE_LOAD_PROJECT, OnFileLoadProject)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_LOADBG_AS_SKY, OnLoadbgAsSky)
	ON_COMMAND(ID_LOADBG_AS_WORLD, OnLoadbgAsWorld)
	ON_COMMAND(ID_EXPORT_FILE, OnExportFile)
	ON_COMMAND(ID_MENU_SETWORKINGFOLDER, OnMenuSetworkingfolder)
	ON_COMMAND(ID_FILE_TEMPLATE_EXPORT, OnFileTemplateExport)
	ON_COMMAND(ID_FILE_TEMPLATE_IMPORT, OnFileTemplateImport)
	ON_COMMAND(ID_MENU_SETLINECOLOR_WHITE, OnMenuSetlinecolorWhite)
	ON_COMMAND(ID_MENU_SETLINECOLOR_BLACK, OnMenuSetlinecolorBlack)
	
	ON_COMMAND(ID_ERASESHAPE_EMPTY_FOROFFSET, OnEraseshapeEmptyForoffset)
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
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	m_pRoot				= NULL;
	m_pSkyNode			= NULL;
	m_pWorldNode		= NULL;
	m_pBackGroundRoot	= NULL;
	m_nPenColor			= WHITE_PEN;
}

CMainFrame::~CMainFrame()
{
	setRoot( NULL);

	g_pRoot = NULL;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (i3TDKDockFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	{
		//Previewer Window
		m_PanePreviewer.Create( CPanePreviewer::IDD, this );
		DockWindow( &m_PanePreviewer, "L", 100,100 );

		//TreeView
//		m_PaneSceneTreeView.Create( CPaneTreeView::IDD, this );
//		DockWindow( &m_PaneSceneTreeView, "R", 100,100 );

		//UVEdit
		m_PaneUVEdit.Create( CPaneUVEdit::IDD, this);
		DockWindow( &m_PaneUVEdit, "R", 100, 100 );

		//LayOut
		m_PaneLayOut.Create( CPaneLayOut::IDD, this);
		DockWindow( &m_PaneLayOut, "LL", 100, 100 );

		//Property
		m_PaneProperty.Create( CPaneProperty::IDD, this );
		DockWindow( &m_PaneProperty, "RR", 100, 100 );

		//Texture List
		m_PaneResList.Create( CPaneResList::IDD, this );
		DockWindow( &m_PaneResList, "LD", 100, 100 );

		//Event & Action
		m_PaneEventnAction.Create( CPaneEventnAction::IDD, this );
		DockWindow( &m_PaneEventnAction, "LDL", 100, 100 );
		
		//control shape
		m_PaneControlShape.Create( CPaneControlShape::IDD, this );
		DockWindow( &m_PaneControlShape, "LDR", 100, 100 );

		m_PaneGuiTreeView.Create( CPaneGuiTreeView::IDD, this );
		DockWindow( &m_PaneGuiTreeView, "RD",100,100);
	}

	LoadDockContext();

	DragAcceptFiles( TRUE);

	//
	i3GuiRoot * pRoot = i3GuiRoot::new_object_ref();

	setRoot( pRoot);

	//
	{
		m_pBackGroundRoot = i3AttrSet::new_object_ref();

		m_pSkyNode = i3Node::new_object_ref();
		m_pSkyNode->SetName("Sky Root");
		m_pWorldNode = i3Node::new_object_ref();
		m_pSkyNode->SetName("World Root");

		m_pBackGroundRoot->AddChild( m_pWorldNode );
		m_pBackGroundRoot->AddChild( m_pSkyNode );

		g_pFramework->Get3DRoot()->AddChild( m_pBackGroundRoot);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, (i3GameNode*)m_pRoot );

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
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return i3TDKDockFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	int Count, i;
	char TempName[MAX_PATH << 1];
	char szExt[32];

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

	//	i3String::SplitFileExt( TempName, szExt, sizeof(szExt) - 1);
		i3::extract_fileext(TempName, szExt);

		if(i3::generic_is_iequal( szExt, "i3i"))
		{
			m_PaneResList.Loadi3i( TempName ); 
		}
	}

	DragFinish( hDropInfo);
}


///Update Message 를 처리하는 부분입니다.
void CMainFrame::OnUpdate(I3_TDK_UPDATE_INFO * pInfo)
{
}

LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDockFrameWnd::WindowProc(message, wParam, lParam);
}

void CMainFrame::Reset()
{
	setRoot( NULL);

	g_pRoot = NULL;

	g_pFramework->GetViewer()->Render();
}

void CMainFrame::OnFileNew()
{
	i3GuiRoot * pRoot = i3GuiRoot::new_object_ref();

	setRoot( pRoot);
	
	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, (i3GameNode*)m_pRoot);
}


void	CMainFrame::setRoot( i3GameNode * pNode)
{
	if( pNode != NULL )
	{
		pNode->AddRef();
	}

	if( m_pRoot != NULL)
	{
		g_pFramework->Get2DRoot()->RemoveChild( m_pRoot->getSceneGraphNode());
		g_pFramework->RemoveChild( m_pRoot);
		I3_SAFE_RELEASE( m_pRoot);
	}

	m_pRoot = (i3GuiRoot*)pNode;
	g_pRoot = m_pRoot;

	if( m_pRoot != NULL)
	{
		m_pRoot->Create( g_pFramework->Get2DRoot() );

		m_pRoot->SetViewer( g_pFramework->GetViewer());

		_Rec_BuildScene( (i3GuiObjBase*)m_pRoot->getFirstChild() );

		g_pFramework->AddChild( m_pRoot );
	}
}

void	CMainFrame::_Rec_BuildScene( i3GuiObjBase * pObj)
{
	if( pObj == NULL )	return;

	i3GameNode * pParent	 = pObj->getParent();
	i3Node	   * pParentNode = NULL;
	if( i3::kind_of<i3GuiRoot*>(pParent)) //->IsTypeOf( i3GuiRoot::static_meta()))
	{
		pParentNode = ((i3GuiRoot*)pParent)->getSceneGraphNode();
	}
	else if( i3::kind_of<i3GuiObjBase*>(pParent)) //->IsTypeOf( i3GuiObjBase::static_meta()))
	{
		pParentNode = ((i3GuiObjBase*)pParent)->GetNode();
	}

	if( pObj->GetNode() != NULL )
	{
		pParentNode->AddChild( pObj->GetNode());
	}

	//Child
	i3GuiObjBase * pChild = (i3GuiObjBase*)pObj->getFirstChild();
	if( pChild != NULL )
	{
		_Rec_BuildScene( pChild );
	}

	//next
	_Rec_BuildScene( (i3GuiObjBase*)pObj->getNext() );
}

BOOL	CMainFrame::SaveProject( const char * szFileName)
{
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> list;

	i3GameNode * pFirstChild = m_pRoot->getFirstChild();
	if( pFirstChild == NULL )
	{
		MessageBox("저장할 내용이 없습니다.");
		return FALSE;
	}

	CGuiToolProject* project = CGuiToolProject::new_object_ref();

	project->SetRoot( (i3GameNode*)m_pRoot );
	project->SetTemplate( m_PaneResList.getTemplate());
	project->SetTextureList( m_PaneResList.getTextureList());

	list.push_back( project);

	file.SetObjects( list);

	if( file.Save( szFileName) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
		return FALSE;
	}

	SetWindowText(szFileName);

	return TRUE;
}

BOOL CMainFrame::LoadProject( const char * szFileName )
{
	Reset();

	i3ResourceFile file;
	INT32	i;
	i3PersistantElement * pObj;
	CGuiToolProject * pProject;

	if( file.Load( szFileName) == STREAM_ERR)
	{
		AfxMessageBox("couldn't read project");
		return FALSE;
	}

	pProject = NULL;
	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pObj = file.GetObject( i);

		if( i3::same_of<CGuiToolProject*>(pObj)) //->IsExactTypeOf( CGuiToolProject::static_meta()))
		{
			pProject =( CGuiToolProject*)pObj;
		}
	}

	if( pProject == NULL)
	{
		AfxMessageBox("Not a Valid GuiTool Project File");
		return FALSE;
	}
	else
	{
		pProject->AddRef();
	}

	i3::shared_ptr<i3::vector<i3GuiToolImageElement*> >& pList = pProject->getTextureList();
	{
		m_PaneResList.SetTextureList( pList );
	}

	i3GuiControlTemplate * pTemplate = pProject->getTemplate();
	{
		m_PaneResList.SetControlTemplate( pTemplate );
	}

	{
		i3GuiRoot * pRoot = (i3GuiRoot*)pProject->getRoot();

		setRoot( pRoot);
	}

	I3_SAFE_RELEASE( pProject );

	SetWindowText(szFileName);

	return TRUE;
}

void CMainFrame::OnFileSaveProject()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES ,  
		"I3GUI ProjectFile(*.GUIPROJ)| *.guiproj||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		SaveProject( (LPCTSTR) Dlg.GetPathName());
	}
}

void CMainFrame::OnFileLoadProject()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3GUI ProjectFile(*.GUIPROJ)| *.guiproj||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		LoadProject( (LPCTSTR) Dlg.GetPathName());

		//i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, g_pRoot->getSceneGraphNode());
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, (i3GameNode*)m_pRoot );
	}
}

void CMainFrame::OnLoadbgAsSky()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"World Scene(*.i3s)| *.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3SceneFile File;
	if( File.Load((LPCSTR)Dlg.GetFileName()) == STREAM_ERR)
	{
		return;
	}

	i3Node * pSky = File.GetSceneGraph();

	m_pSkyNode->RemoveAllChild();
	m_pSkyNode->AddChild( pSky);
	pSky->Release();
	m_PanePreviewer.Invalidate();
}

void CMainFrame::OnLoadbgAsWorld()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Sky Scene(*.i3s)| *.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3SceneFile File;
	if( File.Load((LPCSTR)Dlg.GetFileName()) == STREAM_ERR)
	{
		return;
	}

	i3Node * pWorld = File.GetSceneGraph();

	m_pWorldNode->RemoveAllChild();
	m_pWorldNode->AddChild( pWorld );
	pWorld->Release();
	m_PanePreviewer.Invalidate();
}

void CMainFrame::OnExportFile()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES ,  
		"I3GUI Export(*.gui)| *.gui||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3ResourceFile file;

	i3GameNode * pFirstChild = m_pRoot->getFirstChild();
	if( pFirstChild == NULL )
	{
		MessageBox("저장할 내용이 없습니다.");
		return ;
	}

	file.setKeyObject( m_pRoot);

	if( file.Save( (LPCSTR)Dlg.GetPathName()) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
		return ;
	}
}

void CMainFrame::OnMenuSetworkingfolder()
{
	// TODO: Add your command handler code here
	i3TDKFolderSelectDialog Dlg;

	if( Dlg.Execute( m_hWnd, "Select Working Folder", BIF_RETURNONLYFSDIRS, 0, 0, g_pProfile->m_szWorkDir) )
	{
		i3::string_ncopy_nullpad(  g_pProfile->m_szWorkDir, Dlg.GetSelectedFolderPath(), MAX_PATH -1 );

		i3ResourceFile::SetWorkingFolder( g_pProfile->m_szWorkDir);
		i3ResourceFile::setSearchExternRefEnable( true);
	}
}

void CMainFrame::OnFileTemplateExport()
{
	// TODO: Add your command handler code here
	m_PaneResList.ExportTemplate();
}

void CMainFrame::OnFileTemplateImport()
{
	// TODO: Add your command handler code here
	m_PaneResList.ImportTemplate();
}

void CMainFrame::OnMenuSetlinecolorWhite()
{
	m_PaneUVEdit.m_UVEditCtrl.SetPenColor( WHITE_PEN );
}

void CMainFrame::OnMenuSetlinecolorBlack()
{
	m_PaneUVEdit.m_UVEditCtrl.SetPenColor( BLACK_PEN );
}

static bool _isInvisibleShape( i3GuiImage * pImage)
{
	i3Texture * pTex = pImage->GetTexture();
	if( pTex == NULL)
		return false;

	I3ASSERT( pImage->m_w + pImage->m_u <= (INT32)pTex->GetWidth());
	I3ASSERT( pImage->m_v + pImage->m_h <= (INT32)pTex->GetHeight());
	I3ASSERT( pImage->m_u >= 0);
	I3ASSERT( pImage->m_v >= 0);

	pTex->Lock( 0);

	COLORREAL Col1, Col2;
	pTex->GetPixel( pImage->m_u, pImage->m_v, &Col1);

	for( INT32 i = pImage->m_u; i < pImage->m_w + pImage->m_u; i++)
	{
		for( INT32 j = pImage->m_v; j < pImage->m_h + pImage->m_v; j++)
		{
			pTex->GetPixel( i, j, &Col2);

			if( !(i3Math::abs( i3Color::GetA( &Col1) - i3Color::GetA( &Col2)) < 0.01f &&
					i3Math::abs( i3Color::GetR( &Col1) - i3Color::GetR( &Col2)) < 0.01f &&
					i3Math::abs( i3Color::GetG( &Col1) - i3Color::GetG( &Col2)) < 0.01f &&
					i3Math::abs( i3Color::GetB( &Col1) - i3Color::GetB( &Col2)) < 0.01f) || i3Color::GetA( &Col2) > 0.1f)
			{
				pTex->Unlock(0);
				return false;
			}
		}
	}

	pTex->Unlock(0);
	return true;
}

static bool _RemEmptyShape( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	if( !i3::kind_of<i3GuiControl*>(pNode)) //->IsTypeOf( i3GuiControl::static_meta()))
	{
		return true;
	}

	if( i3::kind_of<i3GuiSlider*>(pNode)) //->IsTypeOf( i3GuiSlider::static_meta()))
		return true;

	i3GuiControl * pCtrl = (i3GuiControl*)pNode;

	i3GuiControlData * pData = pCtrl->GetControlData();
	if( pData == NULL)
		return true;

	i3GuiTemplateElement * pElement = pData->GetShape();
	if( pElement == NULL)
		return true;

	if( pElement->getShapeCount() < 9)
		return true;

	INT32 nInvisibleShapeCount = 0;
	for( INT32 i = 0; i< 8; ++i)
	{
		i3GuiImage * pImage = pElement->getImage( i);
		
		if( pImage == NULL)
			continue;

		if( !_isInvisibleShape( pImage))
			return true;
			
		nInvisibleShapeCount++;
	}

	if( nInvisibleShapeCount == 8)
	{
		for( INT32 i = 0; i< 8; ++i)
		{
			pElement->RemoveShape( 0);
		}

		//i3TDK::Update( AfxGetMainWnd()->m_hWnd, I3_TDK_UPDATE_SET, NULL, i3GuiRoot::static_meta());

		I3TRACE("비어있는 8개의 초기SHAPE가 지워졌습니다.\n");
	}

	return true;
}

void CMainFrame::OnEraseshapeEmptyForoffset()
{
	// TODO: Add your command handler code here
	if( m_pRoot == NULL || m_pRoot->getFirstChild() == NULL)
		return;

	i3GameNodeTraversal * pTraverse = i3GameNodeTraversal::new_object();
	I3ASSERT( pTraverse != NULL);

	pTraverse->setUserProc( _RemEmptyShape);

	pTraverse->Traverse( m_pRoot->getFirstChild());

	I3_SAFE_RELEASE( pTraverse);
}
