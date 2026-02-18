// PropertyEditorView.cpp : CPropertyEditorView 클래스의 구현
//

#include "stdafx.h"
#include "PropertyEditor.h"
#include "MainFrm.h"

#include "PropertyEditorDoc.h"
#include "PropertyEditorView.h"
#include ".\propertyeditorview.h"
#include "GlobalVariable.h"
#include "CodePageSelectDlg.h"

#include "OptionDialog.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		DGL_FILTER_I3_REG	"i3Engine Registry File(*.PEF;*.ENV;*.i3Reg;*.i3RegXML;)|*.PEF;*.ENV;*.i3Reg;*.i3RegXML;||"
#define		DLG_FILTER_XML		"i3Engine Registry File(*.XML)|*.XML||"

// CPropertyEditorView

IMPLEMENT_DYNCREATE(CPropertyEditorView, CView)

BEGIN_MESSAGE_MAP(CPropertyEditorView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//ON_COMMAND(ID_TEST, OnFileTest) PB_KDY한테 물어보고 쓰세염. 친절히 설명해드립니다.
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_EXPORT_XML, OnExportXml)
	ON_COMMAND(ID_IMPORT_XML, OnImportXml)
	ON_COMMAND(ID_EXPORT_32800, OnExportString)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_OPTION, &CPropertyEditorView::OnOption)
	ON_COMMAND(ID_LV_FONT_DOWNSIZE, &CPropertyEditorView::OnLVFontDownSize)
	ON_COMMAND(ID_LV_FONT_UPSIZE, &CPropertyEditorView::OnLvFontUpSize)

	ON_UPDATE_COMMAND_UI(ID_LV_FONT_DOWNSIZE, &CPropertyEditorView::OnUpdateUI_LVFontDownSize)
	ON_UPDATE_COMMAND_UI(ID_LV_FONT_UPSIZE, &CPropertyEditorView::OnUpdateUI_LVFontUpSize)

	ON_COMMAND(ID_ADJUST_PEFDATACNT_TO_NATION_COUNT, &CPropertyEditorView::OnAdjustKeyRegDataCntToNationCount)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_PEFDATACNT_TO_NATION_COUNT, &CPropertyEditorView::OnUpdateAdjustPefDataCntToNationCountMenuItem)

END_MESSAGE_MAP()

// CPropertyEditorView 생성/소멸

CPropertyEditorView::CPropertyEditorView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_bQuit = false;
	m_bSizeBarClicked = false;
}

CPropertyEditorView::~CPropertyEditorView()
{
	CMainFrame* mainFrm = AfxGetMainFrame();
	mainFrm->GetSearchToolBar()->SetCallback(NULL);
}

BOOL CPropertyEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// CPropertyEditorView 그리기

void CPropertyEditorView::OnDraw(CDC* /*pDC*/)
{
	CPropertyEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CPropertyEditorView 인쇄

BOOL CPropertyEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CPropertyEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CPropertyEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CPropertyEditorView 진단

#ifdef _DEBUG
void CPropertyEditorView::AssertValid() const
{
	CView::AssertValid();
}

void CPropertyEditorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPropertyEditorDoc* CPropertyEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPropertyEditorDoc)));
	return (CPropertyEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CPropertyEditorView 메시지 처리기

void CPropertyEditorView::OnInitialUpdate()
{	
	CView::OnInitialUpdate();

	CMainFrame* mainFrm = AfxGetMainFrame();
	mainFrm->GetSearchToolBar()->SetCallback((CSearchComboBoxCallBack*)this);

	// TODO: Add your specialized code here and/or call the base class
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine( cmdInfo);
	
	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		_OpenFile(cmdInfo.m_strFileName);
	}

	_AutoMD5Process();
}

int CPropertyEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rtWnd( 0, 0, 640, 480);

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if( g_pProfile->m_WorkDirectoryPath.IsEmpty() )
	{
		AfxMessageBox("Work Media Directory is not installed.");
		SendMessage(WM_COMMAND, ID_OPTION);
	}

	if( m_bQuit )
	{
		return -1;
	}

	DragAcceptFiles( TRUE);
	// TODO:  Add your specialized creation code here

	m_RegCtrl.Create( WS_CHILD | WS_VISIBLE, rtWnd, this, 100);
	i3RegKey * pKey = m_Registry.getRoot();
	pKey->SetName( "Registry Root");
	pKey->setNodeStyle( I3REG_STYLE_ROOT);

	m_RegCtrl.SetRoot( m_Registry.getRoot() );

	m_Registry.SetListCtrlPtr( m_RegCtrl.GetListCtrlPtr() );

	//	기본 언어로 설정
	SetSelCodePage( i3Language::GetCurrentCodePage());

	AfxGetMainFrame()->SetWindowTextEx();

	return 0;
}

void CPropertyEditorView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_RegCtrl))
	{
		m_RegCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPropertyEditorView::ReSizeWindow()
{
	CRect rect, rect2;
	GetClientRect(rect);

	rect.MoveToY(24);

	rect.right += 4;
	rect.bottom += 3;
	MoveWindow( rect );

	rect.bottom += 1;
	MoveWindow( rect );
}

void CPropertyEditorView::OnFileSave()
{
	if( m_CurFileName.IsEmpty() )
	{
		OnFileSaveAs();
		return;
	}

	_SaveRegistry(m_CurFileName);
}

BOOL	CPropertyEditorView::_OpenFile(LPCTSTR pFilePath)
{
	I3ASSERT(pFilePath != NULL);
	if ( ! pFilePath )
		return FALSE;

	BeginWaitCursor();

	REGISTRY_FILE_TYPE	RegFileType	= REGISTRY_FILE_UNKNOWN;
	INT32				CodePage	= I3_LANG_CODE_ANSI;
	char	szExt[32] = {};

	//Item & WeaponFile 유무를 검사한다.
	m_RegCtrl.SetItemWeaponPef( false );
	/*
	CString Path(pFilePath);

	Path.MakeLower();
	*/
//	if( Path.Find( "itemlist.i3regxml" ) != -1 )
//		m_RegCtrl.SetItemWeaponPef( true );

//	if( Path.Find( "itemlist" ) != -1 )
//		m_RegCtrl.SetItemWeaponPef( true );


//	i3String::SplitFileExt( pFilePath, szExt, 32);
	i3::extract_fileext(pFilePath, szExt);

	if(	i3::generic_is_iequal( szExt, "PEF") 	|| 
		i3::generic_is_iequal( szExt, "i3Reg") ||
		i3::generic_is_iequal( szExt, "env")  )
	{
		CodePage = I3_LANG_CODE_ANSI;
		RegFileType = REGISTRY_FILE_BINARY;
	}
	else if(i3::generic_is_iequal(szExt, "xml")  ||
		i3::generic_is_iequal(szExt, "i3RegXML") )
	{
#if defined(USE_CODEPAGE_SELECTING)
		CCodePageSelectDlg CPDlg;
		if( CPDlg.DoModal() != IDOK)
			return TRUE;
		CodePage = CPDlg.GetCP();
#endif
		RegFileType = REGISTRY_FILE_XML_BY_UTF8;
	}
	else
	{
		return FALSE;
	}

	I3ASSERT(REGISTRY_FILE_UNKNOWN != RegFileType);
	I3ASSERT(I3_LANG_CODE_UNKNOWN != CodePage);

	m_Registry.SetCodePage( CodePage );
	UINT32	Rv = m_Registry.LoadRegistrySet( pFilePath, RegFileType);			

	m_RegCtrl.SetRoot( m_Registry.getRoot() );

	ReSizeWindow();

	BOOL	Success = (Rv != STREAM_ERR);
	if ( Success )
	{
		AfxGetMainFrame()->SetWindowTextEx(pFilePath);
		m_CurFileName = pFilePath;
		g_pProfile->m_CurrentFilePath = pFilePath;
	}

	EndWaitCursor();

	return Success;
}

bool	CPropertyEditorView::_DoTest(LPCTSTR pFilePath)
{
	I3ASSERT(pFilePath != NULL);
	if (!pFilePath)
		return FALSE;

	BeginWaitCursor();

	REGISTRY_FILE_TYPE	RegFileType = REGISTRY_FILE_UNKNOWN;
	INT32				CodePage = I3_LANG_CODE_ANSI;
	char	szExt[32] = {};

	m_RegCtrl.SetItemWeaponPef(false);
	i3::extract_fileext(pFilePath, szExt);

	if (i3::generic_is_iequal(szExt, "PEF") ||
		i3::generic_is_iequal(szExt, "i3Reg") ||
		i3::generic_is_iequal(szExt, "env"))
	{
		CodePage = I3_LANG_CODE_ANSI;
		RegFileType = REGISTRY_FILE_BINARY;
	}
	else if (i3::generic_is_iequal(szExt, "xml") ||
		i3::generic_is_iequal(szExt, "i3RegXML"))
	{
#if defined(USE_CODEPAGE_SELECTING)
		CCodePageSelectDlg CPDlg;
		if (CPDlg.DoModal() != IDOK)
			return TRUE;
		CodePage = CPDlg.GetCP();
#endif
		RegFileType = REGISTRY_FILE_XML_BY_UTF8;
	}
	else
	{
		return FALSE;
	}

	I3ASSERT(REGISTRY_FILE_UNKNOWN != RegFileType);
	I3ASSERT(I3_LANG_CODE_UNKNOWN != CodePage);

	i3RegistrySet_Impl copy_Registry;
	copy_Registry.SetCodePage(CodePage);
	UINT32	Rv = copy_Registry.LoadRegistrySet(pFilePath, RegFileType);

	i3RegKey* copy_Data = (i3RegKey*)copy_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Beret"); 
	i3RegKey* replaceData = (i3RegKey*)m_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Beret");
	for (int i = 0 ; i < copy_Data->getChildCount(); ++i)
	{
		i3RegKey* tempData = NULL;
		tempData = (i3RegKey*)replaceData->FindChildByName(copy_Data->getChild(i)->GetName())->FindChildByName("Common");
		tempData->RemoveAllChild();

		for (int j = 0; j < copy_Data->getChild(i)->FindChildByName("Common")->getChildCount(); ++j)
		{
			tempData->AddChild(copy_Data->getChild(i)->FindChildByName("Common")->getChild(j));
		}
	}

	copy_Data = (i3RegKey*)copy_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Facegear");
	replaceData = (i3RegKey*)m_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Facegear");
	for (int i = 0; i < copy_Data->getChildCount(); ++i)
	{
		i3RegKey* tempData = NULL;
		tempData = (i3RegKey*)replaceData->FindChildByName(copy_Data->getChild(i)->GetName())->FindChildByName("Common");
		tempData->RemoveAllChild();

		for (int j = 0; j < copy_Data->getChild(i)->FindChildByName("Common")->getChildCount(); ++j)
		{
			tempData->AddChild(copy_Data->getChild(i)->FindChildByName("Common")->getChild(j));
		}
	}

	copy_Data = (i3RegKey*)copy_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Head");
	replaceData = (i3RegKey*)m_Registry.getRoot()->FindChildByName("Equipment")->FindChildByName("Head");
	for (int i = 0; i < copy_Data->getChildCount(); ++i)
	{
		i3RegKey* tempData = NULL;
		tempData = (i3RegKey*)replaceData->FindChildByName(copy_Data->getChild(i)->GetName())->FindChildByName("Common");
		tempData->RemoveAllChild();

		for (int j = 0; j < copy_Data->getChild(i)->FindChildByName("Common")->getChildCount(); ++j)
		{
			tempData->AddChild(copy_Data->getChild(i)->FindChildByName("Common")->getChild(j));
		}
	}
	
	m_RegCtrl.SetRoot(m_Registry.getRoot());

	ReSizeWindow();

	BOOL	Success = (Rv != STREAM_ERR);
	if (Success)
	{
		AfxGetMainFrame()->SetWindowTextEx(pFilePath);
		m_CurFileName = pFilePath;
		g_pProfile->m_CurrentFilePath = pFilePath;
	}

	EndWaitCursor();

	return Success;
}

BOOL CPropertyEditorView::_SaveRegistry(const char * pszFileName)
{
	std::vector<std::string> AdjustedLog;
	m_RegCtrl.AdjustKeyRegDataCntToNationCount(&AdjustedLog);

	INT32	CodePage	= I3_LANG_CODE_ANSI;

#if defined(USE_CODEPAGE_SELECTING)
	CCodePageSelectDlg CPDlg;
	if( CPDlg.DoModal() != IDOK)
		return TRUE;
	CodePage = CPDlg.GetCP();
#endif

	BeginWaitCursor();

	m_Registry.SetCodePage( CodePage);

	char szFileName[MAX_PATH] = ""; 
	char szExt[MAX_PATH] = "";

	i3::safe_string_copy( szFileName, pszFileName, MAX_PATH );
	i3::safe_string_copy( szExt, pszFileName, MAX_PATH );
//	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt));
	i3::extract_fileext(pszFileName, szExt);

	// Sorting
	m_RegCtrl._GenerateTimeStamp();
	m_RegCtrl._Sort( SORT_TYPE_KEY );
	m_RegCtrl.SortReg();

	//	저장 형식이 i3RegXML이면 기본 저장되는 Binary 파일은 PEF 파일이된다.
	if( i3::generic_is_iequal( szExt, "i3RegXML") )
	{
		i3::change_file_ext_copy(szFileName, pszFileName, "pef");
	}

	//	Binary로 저장
	if (AfxGetMainFrame()->GetSearchToolBar()->IsPefSave())
	{
		if (m_Registry.SaveRegistrySet(szFileName, REGISTRY_FILE_BINARY) == STREAM_ERR)
		{
			EndWaitCursor();
			return FALSE;
		}
		else
		{
			AfxGetMainFrame()->SetWindowTextEx(szFileName);
		}
	}

	i3::change_file_ext_copy(szFileName, pszFileName, "i3RegXML");

	//	i3RegXML로 저장

	if( m_Registry.SaveRegistrySet( szFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		EndWaitCursor();
		return FALSE;
	}
	else
	{
		AfxGetMainFrame()->SetWindowTextEx(szFileName);
	}

	EndWaitCursor();

	return TRUE;
}

BOOL CPropertyEditorView::_SaveRegistryByUTF8(const char * pszFileName)
{
	BeginWaitCursor();

	//	XML(UTF8)로 저장
	if( m_Registry.SaveRegistrySet( pszFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		EndWaitCursor();
		return FALSE;
	}
	else
	{
		AfxGetMainFrame()->SetWindowTextEx(pszFileName);
	}

	EndWaitCursor();

	return TRUE;
}

void CPropertyEditorView::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "PEF", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		DGL_FILTER_I3_REG, this);

	if( Dlg.DoModal() != IDOK)	return;

	BOOL	Rv = _OpenFile(Dlg.GetPathName());
	if ( ! Rv )
		MessageBox("파일을 읽을 수 없습니다.", "읽기 실패", MB_OK);
}

void CPropertyEditorView::OnFileOpen(const std::string path)
{
	BOOL Rv = _OpenFile(path.c_str());
	if ( ! Rv )
		MessageBox("파일을 읽을 수 없습니다.", "읽기 실패", MB_OK);
}

void CPropertyEditorView::OnFileTest()
{
	CFileDialog	Dlg(TRUE, "PEF", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		DGL_FILTER_I3_REG, this);

	if (Dlg.DoModal() != IDOK)	return;

	BOOL	Rv = _DoTest(Dlg.GetPathName());
	if (!Rv)
		MessageBox("파일을 읽을 수 없습니다.", "읽기 실패", MB_OK);
}

void CPropertyEditorView::OnFileSaveAs()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES,  
		DGL_FILTER_I3_REG, this);

	if( Dlg.DoModal() != IDOK)	return;

	if( _SaveRegistry(Dlg.GetPathName()))
	{
		m_CurFileName = Dlg.GetPathName();
		MessageBox("파일을 저장했습니다.", "파일 저장", MB_OK);
	}
	else
	{
		MessageBox("파일을 저장할 수 없습니다.", "저장 실패", MB_OK);
	}
}

void CPropertyEditorView::OnFileNew()
{
	// TODO: Add your command handler code here
	i3RegKey * pNewKey = i3RegKey::new_object();
	pNewKey->SetName( "Registry Root");
	pNewKey->setNodeStyle( I3REG_STYLE_ROOT);

	m_Registry.setRoot(pNewKey);
	
	m_RegCtrl.SetRoot( m_Registry.getRoot());
	
	AfxGetMainFrame()->SetWindowTextEx();
}

void CPropertyEditorView::OnExportXml()
{
	// TODO: Add your command handler code here

	//	로딩하기 전에 XML에서 읽어올 언어를 선택한다.
	INT32	CodePage	= I3_LANG_CODE_ANSI;
#if defined(USE_CODEPAGE_SELECTING)
	CCodePageSelectDlg CPDlg;
	if( CPDlg.DoModal() != IDOK)
		return;

	CodePage = CPDlg.GetCP();
#endif

	m_Registry.SetCodePage( CodePage);

	CFileDialog	Dlg( FALSE, "", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES,  
		DLG_FILTER_XML, this);

	if( Dlg.DoModal() != IDOK)	return;

	char temp[MAX_PATH] = "";
	i3::change_file_ext_copy( temp, LPCTSTR( Dlg.GetPathName()), "XML");

	if( _SaveRegistryByUTF8(temp))
		MessageBox("내보내기에 성공했습니다.\n", "내보내기 성공", MB_OK);
	else
		MessageBox("내보내기를 할 수 없습니다..", "실패", MB_OK);
}

void CPropertyEditorView::OnImportXml()
{
	// TODO: Add your command handler code here

	INT32	CodePage	= I3_LANG_CODE_ANSI;
#if defined(USE_CODEPAGE_SELECTING)
	//	저장하기 전에 XML로 저장할 언어를 선택한다.
	CCodePageSelectDlg CPDlg;
	if( CPDlg.DoModal() != IDOK)
		return;

	CodePage = CPDlg.GetCP();
#endif
	m_Registry.SetCodePage( CodePage);

	CFileDialog	Dlg( TRUE, "XML", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		DLG_FILTER_XML, this);

	if( Dlg.DoModal() != IDOK)	return;

	BOOL	Rv = _OpenFile(Dlg.GetPathName());
	if ( ! Rv )
		MessageBox("파일을 읽을 수 없습니다.\n\n잘못된 XML파일 형식입니다.\n", "불러오기 실패", MB_OK);
	else
		MessageBox("불러오기에 성공했습니다.\n", "불러오기 성공", MB_OK);
}

void CPropertyEditorView::OnExportString()
{
	// TODO: Add your command handler code here
	if( m_Registry.getRoot()->getChildCount() == 0)
	{
		return;
	}

	INT32	CodePage	= I3_LANG_CODE_ANSI;
#if defined(USE_CODEPAGE_SELECTING)
	CCodePageSelectDlg CPDlg;
	if( CPDlg.DoModal() != IDOK)
		return;
	CodePage = CPDlg.GetCP();
#endif

	CFileDialog	Dlg( FALSE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES,  
		"Text File(*.txt)|*.txt||", this);

	if( Dlg.DoModal() != IDOK)
		return;

	m_Registry.SetCodePage( CodePage);

	CString	FileName = Dlg.GetPathName();

	if ( ! m_Registry.ExportStringFile(FileName) )
	{
		MessageBox("내보내기 실패", "내보내기 실패", MB_OK);
		return;
	}


	MessageBox("내보내기에 성공했습니다.\n", "내보내기 성공", MB_OK);
}


void CPropertyEditorView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	char szName[MAX_PATH];
	int Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);
	for( int i = 0 ; i < Count ; i++)
	{
		DragQueryFile( hDropInfo, i, szName, sizeof(szName) - 1);
	}

	BOOL Rv = _OpenFile(szName);
	if ( ! Rv )
		MessageBox("파일을 읽을 수 없습니다.", "읽기 실패", MB_OK);

	DragFinish(hDropInfo);

	CView::OnDropFiles(hDropInfo);
}

void CPropertyEditorView::OnOption()
{
	COptionDialog Dlg;

	if( Dlg.DoModal() == IDCANCEL )
		m_bQuit = true;
	else
		m_bQuit = false;
}

void CPropertyEditorView::ReSizeControl(POINT pt, int BarIndex)
{
	CPoint Length(pt);
	Length = Length - m_CurrentPt;

	switch( BarIndex )
	{
	case 1:
		if( !m_RegCtrl.GetItemWeaponPef() )
		{
			g_pProfile->m_SimpleTree.right += Length.x;
			g_pProfile->m_SimpleList.left += Length.x;		
		}
		else
		{
			g_pProfile->m_Tree1.right += Length.x;
			g_pProfile->m_Tree2.left += Length.x;
			g_pProfile->m_List1.left += Length.x;
		}
		break;
	case 2:
		g_pProfile->m_Tree2.right += Length.x;
		g_pProfile->m_Image.left += Length.x;
		break;
	case 5:
		g_pProfile->m_List1.right += Length.x;
		g_pProfile->m_List2.left += Length.x;
		break;

	case 3:
		g_pProfile->m_Tree2.bottom += Length.y;
		g_pProfile->m_List1.top += Length.y;
		g_pProfile->m_Image.bottom += Length.y;
		g_pProfile->m_List2.top += Length.y;
		break;
	}

	m_CurrentPt = pt;
	g_pProfile->UpdateGapRect();
	m_RegCtrl.ReSizeControl();
}

BOOL CPropertyEditorView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	static HCURSOR mCurrentCursor = GetCursor();
	static bool bAdjustControlSize = false;

	static HCURSOR mVerCursor = AfxGetApp()->LoadStandardCursor( IDC_SIZEWE );
	static HCURSOR mHorCursor = AfxGetApp()->LoadStandardCursor( IDC_SIZENS );

	int BarIndex = 0;
	POINT Pt = pMsg->pt;
	ScreenToClient( &Pt );
	UINT Result = g_pProfile->MousePointOnSizeBar( Pt, BarIndex );

	switch( pMsg->message )
	{
	case WM_MOUSEMOVE:
		{
			if( bAdjustControlSize )
				ReSizeControl(Pt, m_nBarIndex);

			switch( Result )
			{
			case SBT_VER:
				if( mVerCursor != GetCursor() )
					SetCursor( mVerCursor );
				break;
			case SBT_HOR:
				if( mHorCursor != ::GetCursor() )
					SetCursor(mHorCursor);
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if( BarIndex != 0 )
			{
				SetCapture();

				m_nBarIndex = BarIndex;
				bAdjustControlSize = true;
				m_CurrentPt = Pt;
			}
		}
		break;
	case WM_LBUTTONUP: case WM_NCLBUTTONUP:
		{
			if( bAdjustControlSize )
				::ReleaseCapture();
			bAdjustControlSize = false;
		}
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}

//
// CSearchComboBox Callback
//

INT CPropertyEditorView::CSearchComboBoxCallBack_CollectCandidateByText(
	CandidateRst& rst, const char* text, DWORD opt)
{
	if(!text)
		return 0;

	HTREEITEM startItem = TVI_ROOT;

	if(opt & CSearchComboBoxCallBack::EOPT_FROM_SELECTION)
	{
		startItem = m_RegCtrl.GetSelectdTreeItem();
		if(!startItem)
			startItem = TVI_ROOT;
	}

	bool findInPrefix = (opt & CSearchComboBoxCallBack::EOPT_SEARCH_PREFIX_ONLY) ? true : false;

	std::vector<i3TDKRegistryCtrl_Impl::StrTreeItem> collectedTreeItem;
	m_RegCtrl.CollectTreeItemByStr(collectedTreeItem, text, findInPrefix, startItem);

	for(INT32 i=0; i<(INT32)collectedTreeItem.size(); i++)
	{
		CSearchComboBoxCallBack::Candidate candidate;

		candidate.m_name = collectedTreeItem[i].m_str;
		candidate.m_arg = collectedTreeItem[i].m_item;

		rst.push_back(candidate);
	}

	return 0;
}

void CPropertyEditorView::CSearchComboBoxCallBack_OnSelectItem(void* arg)
{
	if(!arg)
		return;

	m_RegCtrl.SelectItemInTreeCtrl((HTREEITEM)arg);
}

void CPropertyEditorView::CSearchComboBoxCallBack_SelectClosestItem(const char* itemName, DWORD opt)
{
	if(!itemName)
		return;

	HTREEITEM startItem = TVI_ROOT;
	if(opt & CSearchComboBoxCallBack::EOPT_FROM_SELECTION)
	{	
		startItem = m_RegCtrl.GetSelectdTreeItem();
		if(!startItem)
			startItem = TVI_ROOT;
	}

	std::vector<i3TDKRegistryCtrl_Impl::StrTreeItem> collectedTreeItem;
	m_RegCtrl.CollectTreeItemByStr(collectedTreeItem, itemName, 0, startItem);


	INT32 foundClosest = -1;
	INT32 foundExactly = -1;

	for(size_t i=0; i< collectedTreeItem.size(); i++)
	{
		const char* curName = collectedTreeItem[i].m_str.c_str();

		if(foundClosest == -1 && !strnicmp(curName, itemName, strlen(itemName)))
			foundClosest = i;

		if(foundExactly == -1 && !stricmp(curName, itemName))
			foundExactly = i;
	}

	HTREEITEM selItem = NULL;

	if(foundExactly != -1)
		selItem = (HTREEITEM)collectedTreeItem[foundExactly].m_item;
	else if(foundClosest != -1)
		selItem = (HTREEITEM)collectedTreeItem[foundClosest].m_item;
	
	if(selItem)
		m_RegCtrl.SelectItemInTreeCtrl(selItem);		
}


void CPropertyEditorView::OnLVFontDownSize()
{
	m_RegCtrl.GetListCtrlPtr()->Decrease_LogFont();
}

void CPropertyEditorView::OnLvFontUpSize()
{
	m_RegCtrl.GetListCtrlPtr()->Increase_LogFont();
}

void CPropertyEditorView::OnUpdateUI_LVFontDownSize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CPropertyEditorView::OnUpdateUI_LVFontUpSize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


static BOOL CALLBACK LogMsgDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{	
		case WM_INITDIALOG :
			{
				HWND lb = GetDlgItem(hWnd, IDC_LOGMSGDLG_LB_LOG);

				std::vector<std::string>* logMsgVec = (std::vector<std::string>*)lParam;

				for(size_t i=0; i<logMsgVec->size(); i++)
				{
					const char* logstr = ((*logMsgVec)[i]).c_str();
					SendMessage(lb, LB_ADDSTRING, 0, (LPARAM)logstr);
				}
			}
			break;

		case WM_COMMAND:
			{
				if(LOWORD(wParam) == IDOK)
					::EndDialog(hWnd, 1);
			}
			break;

		default:
			break;
	}

	return FALSE;
}

void CPropertyEditorView::OnAdjustKeyRegDataCntToNationCount()
{
//	INT32 numNation = m_RegCtrl.GetFriendListPtr()->GetNationListRef()->size();
	std::vector<std::string> AdjustedLog;

	m_RegCtrl.AdjustKeyRegDataCntToNationCount(&AdjustedLog);

	DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_LOGMSG), GetSafeHwnd(), LogMsgDlgProc, (LPARAM)&AdjustedLog);
}

void CPropertyEditorView::OnUpdateAdjustPefDataCntToNationCountMenuItem(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CPropertyEditorView::_AutoMD5Process()
{	
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine( cmdInfo);

	std::string AutoMD5Result;
	std::string AutoMD5Nation;
	
	if(__argc < 3 )
		return;

	if(__argv[2] != NULL)
	{
		AutoMD5Result+=__argv[2];
		AutoMD5Result.erase(0,1);
	}
	if(__argv[3] != NULL)
	{
		AutoMD5Nation+=__argv[3];
		AutoMD5Nation.erase(0,1);
	}

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		_OpenFile(cmdInfo.m_strFileName);

		if( AutoMD5Nation.length() > 0 && AutoMD5Result.length() > 0 )
		{
			i3::rc_wstring New_MD5Value;
			i3::utf8_to_utf16(AutoMD5Result,New_MD5Value);
			int Nation_index = m_RegCtrl.GetRegNation(AutoMD5Nation.c_str());
			i3RegKey* CheckFile = (i3RegKey*)m_RegCtrl.GetRoot()->FindChildByName("LocaleValue")->FindChildByName("CheckFile");
			i3RegArray* pArrayReg = (i3RegArray*)CheckFile->FindDataByName("NxCharacter.2.8.1.dll");
			pArrayReg->replaceRegData( Nation_index, New_MD5Value.c_str() );

			OnFileSave();
			::PostQuitMessage(0);
		}
	}
}