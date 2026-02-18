// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "GlobalVariable.h"
#include "GuiToolProject.h"
#include "i3UIToolResManager.h"
#include "DlgSelectResolution.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#include "GetStringDlg.h"

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
	ON_COMMAND(ID_CHANGE_RESOLUTION, OnChangeResolution)
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
	m_pSkyNode			= NULL;
	m_pWorldNode		= NULL;
	m_pBackGroundRoot	= NULL;
	m_nPenColor			= WHITE_PEN;
}

CMainFrame::~CMainFrame()
{
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
	{
		m_pBackGroundRoot = i3AttrSet::NewObjectRef();

		m_pSkyNode = i3Node::NewObjectRef();
		m_pSkyNode->SetName("Sky Root");
		m_pWorldNode = i3Node::NewObjectRef();
		m_pSkyNode->SetName("World Root");

		m_pBackGroundRoot->AddChild( m_pWorldNode );
		m_pBackGroundRoot->AddChild( m_pSkyNode );

		g_pFramework->Get3DRoot()->AddChild( m_pBackGroundRoot);
	}

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, (i3GameObjBase*)i3UI::getManager());


	OnFileNew();

	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !i3TDKDockFrameWnd::PreCreateWindow(cs) )
		return FALSE;

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

		i3String::SplitFileExt( TempName, szExt, sizeof(szExt) - 1);

		if(i3String::Compare( szExt, "i3i") == 0)
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
	i3UI::Reset();

	g_pFramework->GetViewer()->Render();
}

void CMainFrame::ChangeResolution(BOOL bForce, INT32 nWidth, INT32 nHeight)
{
	if( bForce == TRUE)
	{
		I3ASSERT( nWidth > 0 && nHeight > 0);
	}

	INT32 width = nWidth;
	INT32 height = nHeight;

	if( bForce == FALSE)
	{
		CDlgSelectResolution Dlg;

		Dlg.setResolutionHeight( i3UI::getManager()->getHeight());
		Dlg.setResolutionWidth( i3UI::getManager()->getWidth());

		if( Dlg.DoModal() == IDOK)
		{
			width = Dlg.getResolutionWidth();
			height = Dlg.getResolutionHeight();
		}
		else
		{
			return;
		}
	}

	if( width != i3UI::getManager()->getWidth() || height != i3UI::getManager()->getHeight())
	{
		m_PanePreviewer.SetViewerResolution( width, height, TRUE);	

		i3UI::getManager()->SetUIResolution((REAL32)width, (REAL32)height);
	}
}

void CMainFrame::OnFileNew()
{
	ChangeResolution( FALSE);

	Reset();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, i3UI::getManager());
}

BOOL	CMainFrame::SaveProject( const char * szFileName)
{
	CGuiToolProject project;
	
	i3List list;

	bool bXML = false;
	{
		char szName[32];
		i3String::SplitFileExt( szFileName, szName, sizeof(szName) - 1);

		if( i3String::Compare( szName, "XML") == 0)
			bXML = true;
	}

	i3GameNode * pFirstChild = i3UI::getManager()->getFirstChild();
	if( pFirstChild == NULL )
	{
		MessageBox("저장할 내용이 없습니다.");
		return FALSE;
	}

	// 저장을 위해 uiroot를 하나 만들고 uimanager의 child들을 uiroot에 모두 옮긴다.
	i3UIRoot * pUIRoot = i3UIRoot::NewObject();
	
	pUIRoot->AddFromUIManager( i3UI::getManager());

	project.SetRoot(pUIRoot );
	project.SetTemplate( m_PaneResList.getTemplate());
	project.SetTextureList( m_PaneResList.getTextureList());

	list.Add( &project);

	BOOL bRet = TRUE;
	if( bXML == false)
	{
		i3ResourceFile file;

		file.setKeyObject( &project);

		if( file.Save( szFileName) == STREAM_ERR)
		{
			AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
			bRet = FALSE;
		}
	}
	else
	{
		i3XMLFile file;

		file.setKeyObject( &project);

		if( file.Save( szFileName) == false)
		{
			AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
			bRet = FALSE;
		}
	}

	I3_SAFE_RELEASE( pUIRoot);

	return bRet;
}

BOOL CMainFrame::LoadProject( const char * szFileName )
{
	Reset();

	CGuiToolProject * pProject;
	bool bXML = false;
	{
		char szName[MAX_PATH];
		i3String::SplitFileExt( szFileName, szName, sizeof(szName) - 1);

		if( i3String::Compare( szName, "XML") == 0)
			bXML = true;
	}

	if( bXML == false)
	{
		i3ResourceFile file;
		if( file.Load( szFileName) == STREAM_ERR)
		{
			AfxMessageBox("couldn't read project");
			return FALSE;
		}

		pProject =( CGuiToolProject*)file.getKeyObject();
	}
	else
	{
		i3XMLFile file;
		if( file.Load( szFileName) == false)
		{
			AfxMessageBox("couldn't read project");
			return FALSE;
		}

		pProject =( CGuiToolProject*)file.getKeyObject();
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

	i3List * pList = pProject->getTextureList();
	{
		m_PaneResList.SetTextureList( pList );
	}

	i3UIControlTemplate * pTemplate = pProject->getTemplate();
	{
		m_PaneResList.SetControlTemplate( pTemplate );
	}

	{
		i3UIRoot * pRoot = pProject->getRoot();
		I3ASSERT( pRoot != NULL);

		i3UI::getManager()->AddFromUIRoot( (i3UIRoot*)pProject->getRoot());

		ChangeResolution( TRUE, pRoot->getWidth(), pRoot->getHeight());
	}

	I3_SAFE_RELEASE( pProject );

	i3TDK::Update( m_hWnd, I3_TDK_USRWND_MAINFRAME | I3_TDK_UPDATE_SELECT, i3UI::getManager());

	return TRUE;
}

void CMainFrame::OnFileSaveProject()
{
	CFileDialog	Dlg( FALSE, "xml", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES ,  
		"xml(*.xml)| *.xml||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	SaveProject( (LPCTSTR) Dlg.GetPathName());
}

void CMainFrame::OnFileLoadProject()
{
	CFileDialog	Dlg( TRUE, "xml", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3GUI ProjectFile(*.XML)| *.xml||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	LoadProject( (LPCTSTR) Dlg.GetPathName());

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SET | I3_TDK_USRWND_MAINFRAME, i3UI::getManager() );
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
	CFileDialog	Dlg( FALSE, "i3UI", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES ,  
		"i3UI Game Data(*.i3UI)| *.i3UI||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	i3ResourceFile file;

	i3GameNode * pFirstChild = i3UI::getManager()->getFirstChild();
	if( pFirstChild == NULL )
	{
		MessageBox("저장할 내용이 없습니다.");
		return ;
	}

	i3UIRoot * pUIRoot = i3UIRoot::NewObject();

	pUIRoot->AddFromUIManager( i3UI::getManager());

	file.setKeyObject( pUIRoot);

	if( file.Save( (LPCSTR)Dlg.GetFileName()) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
		I3_SAFE_RELEASE( pUIRoot);
		return ;
	}

	I3_SAFE_RELEASE( pUIRoot);

	{
		i3GameNode * pTemp = i3UI::getManager()->getFirstChild();

		while( pTemp != NULL)
		{
			if( pTemp->IsTypeOf( i3UIFrameWnd::GetClassMeta()))
			{
				_GenSource( (LPCTSTR) Dlg.GetPathName(), (i3UIFrameWnd *) pTemp);
			}

			pTemp = pTemp->getNext();
		}
	}
}

const char * CMainFrame::_getCtrlPrefix( i3ClassMeta * pMeta)
{
	if( pMeta->IsTypeOf( i3UIButton::GetClassMeta()))
		return "BTN";
	else if( pMeta->IsTypeOf( i3UIEditBox::GetClassMeta()))
		return "ED";
	else if( pMeta->IsTypeOf( i3UICheckBox::GetClassMeta()))
		return "CHK";
	else if( pMeta->IsTypeOf( i3UIFrameWnd::GetClassMeta()))
		return "FRM";
	else if( pMeta->IsTypeOf( i3UIListBox::GetClassMeta()))
		return "LIST";
	else if( pMeta->IsTypeOf( i3UIRadioButton::GetClassMeta()))
		return "RB";
	else if( pMeta->IsTypeOf( i3UIScrollBar::GetClassMeta()))
		return "SCR";
	else if( pMeta->IsTypeOf( i3UIStatic::GetClassMeta()))
		return "ST";

	return "CTRL";
}

void	CMainFrame::_GenSource( const char * pszPath, i3UIFrameWnd * pFrameWnd)
{
	char szPath[MAX_PATH];
	char szFull[MAX_PATH], szName[MAX_PATH];
	const char * pszPrefix;
	char	conv[2048];

	i3String::SplitPath( pszPath, szPath);
	i3String::SplitFileName( pszPath, szName, FALSE);

	if( pFrameWnd->hasName())
		sprintf( szFull, "%s/%s_%s.inc", szPath, szName, pFrameWnd->GetName());
	else
		sprintf( szFull, "%s/%s.inc", szPath, szName);

	i3FileStream file;

	file.Create( szFull, STREAM_WRITE);

	{
		i3List list;
		INT32 i;

		pFrameWnd->GetChildControls( &list);

		// Header Parts
		file.Printf( conv, "\ti3UIFrameWnd *\t\t\tm_pWnd;\r\n");

		for( i = 0; i < list.GetCount(); i++)
		{
			i3UIControl * pCtrl = (i3UIControl *) list.Items[i];
			i3ClassMeta * pMeta = pCtrl->GetMeta();

			pszPrefix = _getCtrlPrefix( pMeta);

			file.Printf( conv, "\t%s *\t\t\tm_p%s_%s;\r\n", pMeta->GetClassName(), pszPrefix, pCtrl->GetName());
		}

		file.Printf( conv, "\r\n\r\n");

		// Initialization Parts
		file.Printf( conv, "\tm_pWnd\t\t= (i3UIFrameWnd *) i3UI::FindFrameWndByName( \"%s\", this);\r\n\tI3ASSERT( m_pWnd != NULL);\r\n\r\n", pFrameWnd->GetName());

		for( i = 0; i < list.GetCount(); i++)
		{
			i3UIControl * pCtrl = (i3UIControl *) list.Items[i];
			i3ClassMeta * pMeta = pCtrl->GetMeta();

			pszPrefix = _getCtrlPrefix( pMeta);

			file.Printf( conv, "\tm_p%s_%s\t\t= (%s *) m_pWnd->FindChildByName( \"%s\");\r\n\tI3ASSERT( m_p%s_%s != NULL);\r\n\r\n", 
				pszPrefix, pCtrl->GetName(), 
				pMeta->GetClassName(),
				pCtrl->GetName(), 
				pszPrefix, pCtrl->GetName());

		}
	}

	file.Close();
}

void CMainFrame::OnMenuSetworkingfolder()
{
	// TODO: Add your command handler code here
	i3TDKFolderSelectDialog Dlg;

	if( Dlg.Execute( m_hWnd, "Select Working Folder", BIF_RETURNONLYFSDIRS, 0, 0, g_pProfile->m_szWorkDir) )
	{
		i3String::NCopy(  g_pProfile->m_szWorkDir, Dlg.GetSelectedFolderPath(), MAX_PATH -1 );

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

static bool _isInvisibleShape( i3UIImage * pImage)
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
				pTex->Unlock();
				return false;
			}
		}
	}

	pTex->Unlock();
	return true;
}

static bool _RemEmptyShape( i3GameNode * pNode, i3GameNodeTraversal * pTraversal, void * pUserData)
{
	/*
	if( !pNode->IsTypeOf( i3UIControl::GetClassMeta()))
	{
		return true;
	}

	i3UIControl * pCtrl = (i3UIControl*)pNode;

	i3GuiControlData * pData = pCtrl->GetControlData();
	if( pData == NULL)
		return true;

	i3UITemplate * pElement = pData->GetShape();
	if( pElement == NULL)
		return true;

	if( pElement->getShapeCount() < 9)
		return true;

	INT32 nInvisibleShapeCount = 0;
	for( INT32 i = 0; i< 8; ++i)
	{
		i3UIImage * pImage = pElement->getShape( i);
		
		if( pImage == NULL)
			continue;

		if( !_isInvisibleShape( pImage))
			return true;
			
		nInvisibleShapeCount++;
	}
*/
	return true;
}

void CMainFrame::OnEraseshapeEmptyForoffset()
{
	// TODO: Add your command handler code here
	if( i3UI::getManager() == NULL || i3UI::getManager()->getFirstChild() == NULL)
		return;

	i3GameNodeTraversal * pTraverse = i3GameNodeTraversal::NewObject();
	I3ASSERT( pTraverse != NULL);

	pTraverse->setUserProc( _RemEmptyShape);

	pTraverse->Traverse( i3UI::getManager()->getFirstChild());

	I3_SAFE_RELEASE( pTraverse);
}

void CMainFrame::OnChangeResolution()
{
	// TODO: Add your command handler code here
	ChangeResolution( FALSE);
}
