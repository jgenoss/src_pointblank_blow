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
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_EXPORT_XML, OnExportXml)
	ON_COMMAND(ID_IMPORT_XML, OnImportXml)
	ON_COMMAND(ID_EXPORT_32800, OnExportString)
	ON_COMMAND(ID_IMPORT_SCRIPT, OnImportString)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CPropertyEditorView 생성/소멸

CPropertyEditorView::CPropertyEditorView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CPropertyEditorView::~CPropertyEditorView()
{
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
	
	// TODO: Add your specialized code here and/or call the base class
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine( cmdInfo);

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		_OpenFile(cmdInfo.m_strFileName);
	}
}

int CPropertyEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rtWnd( 0, 0, 640, 480);

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE);
	// TODO:  Add your specialized creation code here

	m_RegCtrl.Create( WS_CHILD | WS_VISIBLE, rtWnd, this, 100);
	i3RegKey * pKey = m_Registry.getRoot();
	pKey->SetName( "Registry Root");
	pKey->setNodeStyle( I3REG_STYLE_ROOT);

	m_RegCtrl.SetRoot( m_Registry.getRoot());

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


	REGISTRY_FILE_TYPE	RegFileType	= REGISTRY_FILE_UNKNOWN;
	INT32				CodePage	= I3_LANG_CODE_ANSI;
	char	szExt[32] = {};

	i3String::SplitFileExt( pFilePath, szExt, 32);

	if(	(i3String::Compare( szExt, "PEF") == 0)	|| 
		(i3String::Compare( szExt, "i3Reg") == 0) ||
		(i3String::Compare( szExt, "env") == 0) )
	{
		CodePage = I3_LANG_CODE_ANSI;
		RegFileType = REGISTRY_FILE_BINARY;
	}
	else if(i3String::Compare(szExt, "xml") == 0 ||
		i3String::Compare(szExt, "i3RegXML") == 0)
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

	m_RegCtrl.SetRoot( m_Registry.getRoot());

	BOOL	Success = (Rv != STREAM_ERR);
	if ( Success )
	{
		AfxGetMainFrame()->SetWindowTextEx(pFilePath);
		m_CurFileName = pFilePath;
	}

	return Success;
}

BOOL CPropertyEditorView::_SaveRegistry(const char * pszFileName)
{
	INT32	CodePage	= I3_LANG_CODE_ANSI;

#if defined(USE_CODEPAGE_SELECTING)
	CCodePageSelectDlg CPDlg;
	if( CPDlg.DoModal() != IDOK)
		return TRUE;
	CodePage = CPDlg.GetCP();
#endif

	m_Registry.SetCodePage( CodePage);

	char szFileName[MAX_PATH] = ""; 
	char szExt[MAX_PATH] = "";

	i3String::Copy( szFileName, pszFileName);
	i3String::Copy( szExt, pszFileName);
	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt));

	// Sorting
	m_RegCtrl._GenerateTimeStamp();
	//m_RegCtrl._Sort( SORT_TYPE_KEY );

	//for(INT32 i=0; i<m_RegCtrl.GetRoot()->getChildCount(); i++)
	//{
	//	i3RegData* pName = NULL;

	//	for(INT32 j=0; j<((i3RegKey*)(m_RegCtrl.GetRoot()->getChild(i)))->getDataList()->GetCount(); j++)
	//	{
	//		
	//		pName = (i3RegData*)((i3RegKey*)(m_RegCtrl.GetRoot()->getChild(i)))->getDataList()->GetItem(j);

	//		I3TRACE("_SaveRegistry %s \n", pName->GetName());

	//		/*if(pName->getDataType() == I3REG_TYPE_INT32)
	//		{
	//			char szTemp[256];

	//			strncpy(szTemp, pName->GetName(), 256);
	//			pName->SetName(&szTemp[11]);
	//		}*/
	//	}
	//	
	//}
	

	//	저장 형식이 i3RegXML이면 기본 저장되는 Binary 파일은 PEF 파일이된다.
	if( i3String::Compare( szExt, "i3RegXML") == 0)
	{
		i3String::SwapExt(szFileName, pszFileName, "PEF");
	}

	//	Binary로 저장
	if( m_Registry.SaveRegistrySet( szFileName, REGISTRY_FILE_BINARY ) == STREAM_ERR)
	{
		return FALSE;
	}
	else
	{
		AfxGetMainFrame()->SetWindowTextEx(szFileName);
	}

	i3String::SwapExt(szFileName, pszFileName, "i3RegXML");

	//	i3RegXML로 저장
	if( m_Registry.SaveRegistrySet( szFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		return FALSE;
	}
	else
	{
		AfxGetMainFrame()->SetWindowTextEx(szFileName);
	}

	return TRUE;
}

BOOL CPropertyEditorView::_SaveRegistryByUTF8(const char * pszFileName)
{
	//	XML(UTF8)로 저장
	if( m_Registry.SaveRegistrySet( pszFileName, REGISTRY_FILE_XML_BY_UTF8 ) == STREAM_ERR)
	{
		return FALSE;
	}
	else
	{
		AfxGetMainFrame()->SetWindowTextEx(pszFileName);
	}

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
	i3RegKey * pNewKey = i3RegKey::NewObject();
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
	i3String::SwapExt( temp, LPCTSTR( Dlg.GetPathName()), "XML");

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




void CPropertyEditorView::OnImportString()
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

	CFileDialog	Dlg( TRUE, "txt", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Text File(*.txt)|*.txt||", this);

	if( Dlg.DoModal() != IDOK)
		return;
	 
	i3FileStream file;
	CString FileName = Dlg.GetPathName();
	if( !file.Open( FileName, STREAM_READ))
	{
		MessageBox("파일을 읽을 수 없습니다..\n잘못된 파일입니다.", "가져오기 실패", MB_OK);
		return;
	}


	//	ByteOrderMark
	WCHAR16	TxtType = 0;
	file.Read( &TxtType, sizeof( WCHAR16));
	if( 0xfeff != TxtType)
	{
		MessageBox("파일을 읽을 수 없습니다.\n인코딩 방식이 유니코드가 아닙니다.", "가져오기 실패", MB_OK);
		return;
	}
	file.Close();

	m_Registry.SetCodePage( CodePage);

	const char * LogFileName = "PropertyEditorReport.log";

	DeleteFile(LogFileName);

	bool Rv = m_Registry.ImportStringFile(FileName, true, LogFileName);
	if ( Rv )
	{
		MessageBox("불러오기에 성공했습니다.\n", "불러오기 성공", MB_OK);
	}
	else
	{
		char t[MAX_PATH] = "";
		sprintf( t, "NotePad.exe %s", LogFileName);

		WinExec(t, SW_SHOWNORMAL);
	}
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
