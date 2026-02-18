// i3MVFGenView.cpp : Ci3MVFGenView 클래스의 구현
//

#include "stdafx.h"
#include "i3MVFGen.h"

#include "i3MVFGenDoc.h"
#include "i3MVFGenView.h"
#include ".\i3mvfgenview.h"
#include "MVFNode.h"
#include "GlobVar.h"
#include "OptionDialog.h"
#include "MVFGenThread.h"
#include "MVFGenDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3MVFGenView

IMPLEMENT_DYNCREATE(Ci3MVFGenView, CView)

BEGIN_MESSAGE_MAP(Ci3MVFGenView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_OPTIONS_OPTIONS, OnOptionsOptions)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_DROPFILES()
	ON_COMMAND(ID_LIST_EXPORT, OnListExport)
	ON_COMMAND(ID_LIST_IMPORT, OnListImport)
END_MESSAGE_MAP()

// Ci3MVFGenView 생성/소멸

Ci3MVFGenView::Ci3MVFGenView()
{
	m_pRoot = NULL;
	m_pThread = NULL;
	m_szFileName[0] = 0;
}

Ci3MVFGenView::~Ci3MVFGenView()
{
}

BOOL Ci3MVFGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// Ci3MVFGenView 그리기

void Ci3MVFGenView::OnDraw(CDC* /*pDC*/)
{
	Ci3MVFGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// Ci3MVFGenView 인쇄

BOOL Ci3MVFGenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void Ci3MVFGenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void Ci3MVFGenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// Ci3MVFGenView 진단

#ifdef _DEBUG
void Ci3MVFGenView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3MVFGenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3MVFGenDoc* Ci3MVFGenView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3MVFGenDoc)));
	return (Ci3MVFGenDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3MVFGenView 메시지 처리기
int Ci3MVFGenView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rect( 0, 0, 640, 480);

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE);

	{
		m_Splitter0.Create( 0, WS_CHILD | WS_VISIBLE, rect, 48, this, IDC_SPLITTER0);
		m_Splitter1.Create( I3_SPLS_EX_VERT, WS_CHILD | WS_VISIBLE, rect, 48, &m_Splitter0, IDC_SPLITTER1);

		m_PaneTree.Create( CPaneTree::IDD, &m_Splitter0);
		m_PaneSpec.Create( CPaneSpec::IDD, &m_Splitter1);
		m_PaneStatus.Create( CPaneStatus::IDD, &m_Splitter1);

		m_Splitter0.AddPane( &m_PaneTree);
		m_Splitter0.AddPane( &m_Splitter1);

		m_Splitter1.AddPane( &m_PaneSpec);
		m_Splitter1.AddPane( &m_PaneStatus);

		m_Splitter0.LoadWindowInfo( "i3MVFGen.CFG");
		m_Splitter0.Rearrange();
	}

	MVFNode * pNewRoot;

	pNewRoot = MVFNode::NewObject();
	pNewRoot->SetName( "/");
	pNewRoot->SetStyle( I3MVF_ATTR_FOLDER);

	SetRoot( pNewRoot);
	return 0;
}

void Ci3MVFGenView::OnDestroy()
{
	m_Splitter0.SaveWindowInfo( "i3MVFGen.CFG" );

	CView::OnDestroy();
}

void Ci3MVFGenView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if( ::IsWindow( m_Splitter0.m_hWnd))
	{
		m_Splitter0.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3MVFGenView::MakeExceptList(void)
{
	m_ExceptList.Clear();

	m_ExceptList.CopyFromBuffer( g_pProfile->m_szExceptRull);
}

BOOL Ci3MVFGenView::IsAcceptable( char * pszStr)
{
	INT32 i;
	
	for( i = 0; i < m_ExceptList.GetCount(); i++)
	{
		if( i3String::Compare( pszStr, m_ExceptList.Items[i]) == 0)
			return FALSE;
	}

	return TRUE;
}

BOOL Ci3MVFGenView::LookForFiles( const char * pszPath, MVFNode * pParent)
{
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
    char _TempPath[MAX_PATH];
	MVFNode * pNode;
	char szExt[MAX_PATH];

    sprintf( _TempPath, "%s\\*.*", pszPath);

    hFind = FindFirstFile( _TempPath, &FindData);
    if( hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if((strcmp( FindData.cFileName, ".") != 0) && (strcmp( FindData.cFileName, "..") != 0))
			{
				// SHGetFileInfo( );

				szExt[0] = 0;
				i3String::SplitFileExt( FindData.cFileName, szExt, sizeof(szExt) - 1);

				if( IsAcceptable( szExt))
				{
					pNode = MVFNode::NewObjectRef();

					pNode->SetName( FindData.cFileName);
					pNode->SetSize( FindData.nFileSizeLow);

					// File Attribute
					if( i3String::Compare( szExt, "I3S") == 0)
						pNode->AddStyle( I3MVF_ATTR_SCENE);
					else if( i3String::Compare( szExt, "I3A") == 0)
						pNode->AddStyle( I3MVF_ATTR_ANIMATION);
					else if(	(i3String::Compare( szExt, "I3I") == 0) ||
								(i3String::Compare( szExt, "TGA") == 0) ||
								(i3String::Compare( szExt, "BMP") == 0))
					{
						pNode->AddStyle( I3MVF_ATTR_IMAGE);
					}

					pParent->AddChild( pNode);

					if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
						!(FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) 
					{
						pNode->AddStyle( I3MVF_ATTR_FOLDER);

						sprintf( _TempPath, "%s\\%s", pszPath, FindData.cFileName);

						// 재귀호출
						LookForFiles( _TempPath, pNode);
					}
				}
			}

		} while( FindNextFile( hFind, &FindData) == TRUE);

		FindClose( hFind);
	}

    return TRUE;
}

void Ci3MVFGenView::OnFileImport()
{
	i3TDKFolderSelectDialog dlg;
	MVFNode * pNewRoot;

	if( dlg.Execute( m_hWnd, "파일을 검색할 Root Folder를 선택하세요", 
			BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE, NULL, 0,
			g_pProfile->m_szImportDir ) == FALSE)
		return;

	pNewRoot = MVFNode::NewObject();
	pNewRoot->SetName( "/");
	pNewRoot->SetStyle( I3MVF_ATTR_FOLDER);

	MakeExceptList();

	LookForFiles( dlg.GetSelectedFolderPath(), pNewRoot);

	SetRoot( pNewRoot);

	strcpy( g_pProfile->m_szImportDir, dlg.GetSelectedFolderPath() );
}

void Ci3MVFGenView::SetRoot( MVFNode * pRoot)
{
	if( pRoot != NULL)
		pRoot->AddRef();

	I3_SAFE_RELEASE( m_pRoot);

	m_pRoot = pRoot;
	g_pRoot = pRoot;

	m_PaneTree.UpdateAll();
}

void Ci3MVFGenView::OnOptionsOptions()
{
	COptionDialog dlg;

	dlg.DoModal();
}

void Ci3MVFGenView::_LogProc( INT32 type, MVFNode * pNode, char * pszMsg, void * pUserData)
{
	Ci3MVFGenView * pView = (Ci3MVFGenView *) pUserData;

	pView->m_PaneStatus.Log( type, pNode, pszMsg);
}

void Ci3MVFGenView::BuildMVF( const char * pszPath)
{
	MVFGenDialog dlg;

	dlg.m_AlignUnit = g_pProfile->m_nAlign;

	if( dlg.DoModal() == IDCANCEL)
		return;

	strcpy( m_szFileName, pszPath);
	GetDocument()->SetTitle( pszPath);

	m_pThread = MVFGenThread::NewObject();

	m_pThread->SetMVFFileName( m_szFileName);
	m_pThread->SetAlignUnit( dlg.m_AlignUnit);
	m_pThread->SetHWND( m_hWnd);
	m_pThread->SetLogProc( _LogProc);
	m_pThread->SetRoot( m_pRoot);
	m_pThread->SetUserData( this);

	m_pThread->Create( "MVFGenThread");
}

void Ci3MVFGenView::OnFileSaveAs()
{
	CFileDialog	Dlg( FALSE, "MVF", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Merged Volume File(*.MVF)|*.MVF||", NULL);

	if( Dlg.DoModal() != IDOK)return;

	BuildMVF( LPCTSTR( Dlg.GetPathName()));
}

void Ci3MVFGenView::OnFileSave()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveAs();
		return;
	}

	BuildMVF( (const char *) m_szFileName);
}

LRESULT Ci3MVFGenView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case THM_PROGRESS :	
			{
				m_PaneStatus.SetProgress( (INT32) wParam);
				m_PaneStatus.Log( 99, NULL, "------------------- MVF Generating --------------------");
			}
			break;

		case THM_FILE :
			{
				MVFNode * pNode = (MVFNode *) wParam;

				m_PaneStatus.AddProgress( (char *) pNode->GetName());
			}
			break;

		case THM_END :
			{
				if( m_pThread != NULL)
				{
					m_PaneStatus.Log( 99, NULL, "------------------- Finish --------------------");
					m_pThread->Release();
					m_pThread = NULL;
				}
			}
			break;
	}

	return CView::WindowProc(message, wParam, lParam);
}

static BOOL _VerifyProc( char * pszFull, I3_MVF_ENTRY * pEntry, void * pUserData)
{
	I3TRACE( "%s\n", pszFull);

	return TRUE;
}

void Ci3MVFGenView::OpenMVF(const char* szMVFFileName )
{
	i3MergedVolumeFile file;
	MVFNode * pNewRoot;

	if( file.LoadMVF( szMVFFileName) == TRUE)
	{
		pNewRoot = BuildNodeFromMVF( &file);

		if( pNewRoot != NULL)
		{
			SetRoot( pNewRoot);
		}
	}
	else
	{
		DebugBox( m_hWnd, "%s 파일을 읽을 수 없습니다.", szMVFFileName);
	}
}

void Ci3MVFGenView::OnFileOpen()
{
	CFileDialog	Dlg( TRUE, "MVF", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Merged Volume File (*.MVF)|*.MVF||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	OpenMVF(LPCTSTR( Dlg.GetPathName()) );
}

void Ci3MVFGenView::Rec_BuildNodeFromMVF( MVFNode * pParent, i3MergedVolumeFile * pFile, I3_MVF_ENTRY * pEntry)
{
	MVFNode * pChild;

	if( pEntry->m_Attr & I3MVF_ATTR_FOLDER)
	{
		UINT32 i, childidx;
		I3_MVF_ENTRY * pChildEntry;

		for( i = 0; i < pEntry->dir.m_ChildCount; i++)
		{
			childidx = pEntry->dir.m_StartChildIndex + i;

			pChildEntry = pFile->GetEntry( childidx);

			pChild = MVFNode::NewObjectRef();

			pChild->SetEntry( pChildEntry);
			pChild->SetName( pFile->GetFileNameTable() + pChildEntry->m_NameIndex);

			pParent->AddChild( pChild);

			Rec_BuildNodeFromMVF( pChild, pFile, pChildEntry);
		}
	}
}

MVFNode *  Ci3MVFGenView::BuildNodeFromMVF( i3MergedVolumeFile * pFile)
{
	MVFNode * pRoot;

	m_NameList.Clear();
	m_NameList.CopyFromBuffer( pFile->GetFileNameTable());

	pRoot = MVFNode::NewObjectRef();
	pRoot->SetEntry( pFile->GetEntry(0));
	pRoot->SetName( pFile->GetFileName( 0));

	Rec_BuildNodeFromMVF( pRoot, pFile, pFile->GetEntry( 0));

	return pRoot;
}

void Ci3MVFGenView::OnDropFiles(HDROP hDropInfo)
{
	int Count, i;
	char TempName[MAX_PATH << 1];
	HANDLE hFind;
	WIN32_FIND_DATA fi;

	Count = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	MakeExceptList();

	for( i = 0; i < Count; i++)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof(TempName) - 1);

		hFind = FindFirstFile( TempName, &fi);
		FindClose( hFind);

		if( fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			AddDirectory( TempName);
		}
		else
		{
			AddFile( TempName);
		}
	}

	DragFinish( hDropInfo);
}

void Ci3MVFGenView::AddFile( const char * pszPath)
{
	char szRelPath[MAX_PATH];
	char * pszName;
	i3StringList StrList;
	INT32 i;
	MVFNode * pNode, * pChild;
	HTREEITEM hitemParent;

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszPath, szRelPath);

	i3String::ResolvePathToken( szRelPath, &StrList);

	pNode = m_pRoot;

	hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, m_pRoot);

	for( i = 0; i < StrList.GetCount(); i++)
	{
		pszName = StrList.Items[i];

		pChild = pNode->FindChildByName( pszName);

		if( pChild == NULL)
		{
			pChild = MVFNode::NewObjectRef();

			pChild->SetName( pszName);

			pNode->AddChild( pChild);

			if( i < StrList.GetCount() - 1)
			{
				// Directory
				pChild->SetSize( 0);
				pChild->SetStyle( I3MVF_ATTR_FOLDER);
			}
			else
			{
				HANDLE hFind;
				WIN32_FIND_DATA FindData;
				char szExt[32];

				szExt[0] = 0;

				hFind = FindFirstFile( pszPath, &FindData);
				if( hFind != INVALID_HANDLE_VALUE)
				{
					pChild->SetSize( FindData.nFileSizeLow);
					
					i3String::SplitFileExt( FindData.cFileName, szExt, sizeof(szExt) - 1);

					// File Attribute
					if( i3String::Compare( szExt, "I3S") == 0)
						pChild->AddStyle( I3MVF_ATTR_SCENE);
					else if( i3String::Compare( szExt, "I3A") == 0)
						pChild->AddStyle( I3MVF_ATTR_ANIMATION);
					else if(	(i3String::Compare( szExt, "I3I") == 0) ||
								(i3String::Compare( szExt, "TGA") == 0) ||
								(i3String::Compare( szExt, "BMP") == 0))
					{
						pChild->AddStyle( I3MVF_ATTR_IMAGE);
					}

					FindClose( hFind);
				}
			}

			hitemParent = m_PaneTree.AddTree( hitemParent, pChild);
		}
		else
		{
			hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pChild, hitemParent, FALSE);
			I3ASSERT( hitemParent != NULL);
		}

		pNode = pChild;
	}
}

void Ci3MVFGenView::AddDirectory( const char * pszPath)
{
	char szRelPath[MAX_PATH];
	char * pszName;
	i3StringList StrList;
	INT32 i;
	MVFNode * pNode, * pChild;
	HTREEITEM hitemParent;

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszPath, szRelPath);

	i3String::ResolvePathToken( szRelPath, &StrList);

	pNode = m_pRoot;

	hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, m_pRoot);

	for( i = 0; i < StrList.GetCount(); i++)
	{
		pszName = StrList.Items[i];

		pChild = pNode->FindChildByName( pszName);

		if( pChild == NULL)
		{
			pChild = MVFNode::NewObjectRef();

			pChild->SetName( pszName);

			pNode->AddChild( pChild);

			// Directory
			pChild->SetSize( 0);
			pChild->SetStyle( I3MVF_ATTR_FOLDER);

			hitemParent = m_PaneTree.AddTree( hitemParent, pChild);
		}
		else
		{
			hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pChild, hitemParent, FALSE);
			I3ASSERT( hitemParent != NULL);
		}

		pNode = pChild;
	}

	LookForFiles( pszPath, pNode);

	m_PaneTree.UpdateAll();
}

///
typedef struct _tagCountData
{
	UINT32	m_Count;
} COUNT_PROC_DATA1;

static void _CountingProc1( char * pszFull, MVFNode * pNode, void * pUserData)
{
	COUNT_PROC_DATA1 * pData = (COUNT_PROC_DATA1 *) pUserData;

	pData->m_Count++;
}

void Ci3MVFGenView::OnListExport()
{
	COUNT_PROC_DATA1 Data;
	i3FileStream	FileStream;
	INT32			nCount = 0;
	char			szData[MAX_PATH];

	CFileDialog	Dlg( FALSE, "MVF List File", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Merged Volume List File (*.MVFList)| *.MVFList||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	BOOL bRet = FileStream.Create( LPCTSTR( Dlg.GetPathName()), STREAM_WRITE );

	if( ! bRet )
	{
		MessageBox( "저장 실패" );
		return;
	}

	strcpy(	szData, "[INFO]\n");
	FileStream.Write( szData,	strlen( szData ));

	// 전체 개수를 구한다.
	{
		Data.m_Count = 0;
		m_pRoot->Traverse( NULL, _CountingProc1, &Data);

		sprintf( szData, "FileCount = %d\n", Data.m_Count - 1);
		FileStream.Write(	szData,	strlen( szData ));
	}

	//Align
	sprintf( szData, "nAlignUnit = %d\n\n" , g_pProfile->m_nAlign );
	FileStream.Write( szData,	strlen( szData ));


	strcpy(	szData, "[FILES]\n");
	FileStream.Write(	szData,	strlen( szData ));

	ExporttoList( m_pRoot, &nCount, &FileStream );

	FileStream.Close();

	MessageBox( "저장 성공" );
}

BOOL Ci3MVFGenView::ExporttoList( MVFNode * pParent, INT32	*nCount, i3FileStream * pStream )
{
	I3ASSERT( pParent != NULL );

	char	szFullPath[MAX_PATH];
	char	szRelPath[MAX_PATH];
	char	szWriteData[MAX_PATH];

	INT32	nChildCount = pParent->GetChildCount();
	INT32	i = 0;

	if( nChildCount <= 0 )	return TRUE;

	for( i = 0; i< nChildCount; ++i)
	{
		MVFNode * pChild = (MVFNode*)pParent->GetChild( i );
		if( pChild )
		{
			pChild->GetFullPath( szFullPath );

			i3String::MakeRelativePath( g_pProfile->m_szWorkDir, szFullPath, szRelPath);
			sprintf( szWriteData, "File_%04d = %c%s%c\n", *nCount, '\"', szRelPath, '\"');

			i3String::MakeCStylePath( szRelPath, szWriteData );

			pStream->Write( &szRelPath, strlen( szRelPath ));
			*nCount += 1;
			ExporttoList( pChild, nCount, pStream );
		}
	}

	return TRUE;
}

void Ci3MVFGenView::AddFileFromList( const char * pszPath)
{
	char szRelPath[MAX_PATH];
	char * pszName;
	i3StringList StrList;
	INT32 i;
	MVFNode * pNode, * pChild;
	HTREEITEM hitemParent;

	i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszPath, szRelPath);

	i3String::ResolvePathToken( szRelPath, &StrList);

	pNode = m_pRoot;

	hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, m_pRoot);

	for( i = 0; i < StrList.GetCount(); i++)
	{
		pszName = StrList.Items[i];

		pChild = pNode->FindChildByName( pszName);

		if( pChild == NULL)
		{
			pChild = MVFNode::NewObjectRef();

			pChild->SetName( pszName);

			pNode->AddChild( pChild);

			HANDLE hFind;
			WIN32_FIND_DATA FindData;
			char szExt[32];

			szExt[0] = 0;

			hFind = FindFirstFile( pszPath, &FindData);
			if( hFind != INVALID_HANDLE_VALUE)
			{
				if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					// Directory
					pChild->SetSize( 0);
					pChild->SetStyle( I3MVF_ATTR_FOLDER);
				}
				else
				{
					pChild->SetSize( FindData.nFileSizeLow);
					
					i3String::SplitFileExt( FindData.cFileName, szExt, sizeof(szExt) - 1);

					// File Attribute
					if( i3String::Compare( szExt, "I3S") == 0)
						pChild->AddStyle( I3MVF_ATTR_SCENE);
					else if( i3String::Compare( szExt, "I3A") == 0)
						pChild->AddStyle( I3MVF_ATTR_ANIMATION);
					else if(	(i3String::Compare( szExt, "I3I") == 0) ||
								(i3String::Compare( szExt, "TGA") == 0) ||
								(i3String::Compare( szExt, "BMP") == 0))
					{
						pChild->AddStyle( I3MVF_ATTR_IMAGE);
					}
				}
				FindClose( hFind);
			}

			hitemParent = m_PaneTree.AddTree( hitemParent, pChild);
		}
		else
		{
			hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pChild, hitemParent, FALSE);
			I3ASSERT( hitemParent != NULL);
		}

		pNode = pChild;
	}
}

//MVFList 파일을 이용하여 리스트를 import합니다.
void Ci3MVFGenView::OnListImport()
{
	i3TDKFolderSelectDialog dlg;
	MVFNode * pNewRoot;

	CFileDialog	Dlg( TRUE, "MVFlst", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Merged Volume List File (*.MVFList)| *.MVFList||", NULL);

	if( Dlg.DoModal() != IDOK)	return;
	
	// List 의 root 생성
	pNewRoot = MVFNode::NewObject();
	pNewRoot->SetName( "/");
	pNewRoot->SetStyle( I3MVF_ATTR_FOLDER);

	SetRoot( pNewRoot);

	ImportFromList( LPCTSTR( Dlg.GetPathName()));
}

BOOL Ci3MVFGenView::ImportFromList( const char * szListFileName)
{
	char			szFileName[MAX_PATH];
	char			szIndex[16];

	INT32			nFileCount = 0;

	i3FileStream	FileStream;
	i3IniParser		Parser;

	if( ! FileStream.Open( szListFileName, STREAM_READ ))
	{
		I3TRACE("File Open Failed\n");
		return FALSE;
	}

	if( ! Parser.Open( &FileStream ) )
	{
		I3TRACE("File Open Failed\n");
		return FALSE;
	}

	if( !Parser.ReadSection("INFO"))
	{
		I3TRACE("Couldn't Find Section MEDIA_DIR\n");
		return FALSE;
	}

	strcpy( szFileName, g_pProfile->m_szWorkDir);
	Parser.GetValue("FileCount", 0, &nFileCount );
	Parser.GetValue("nAlignUnit", 0, &g_pProfile->m_nAlign );

	if( !Parser.ReadSection("FILES"))
	{
		I3TRACE("Couldn't Find Section MEDIA_DIR\n");
		return FALSE;
	}

	for( INT32	i = 0; i < nFileCount; i++)
	{
		sprintf( szIndex, "File_%04d", i );
		Parser.GetValue( szIndex,	"",	szFileName);
		AddFileFromList( szFileName );
	}

    return TRUE;
}

void Ci3MVFGenView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	i3Error::SetDefaultCallback( I3LOG_FATAL, m_hWnd );

	// TODO: Add your specialized code here and/or call the base class
	CCommandLineInfo cmdInfo;
	AfxGetApp()->ParseCommandLine( cmdInfo );

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen )
	{
		char szFileName[MAX_PATH];
		char szExt[32];
		strcpy( szFileName, LPCTSTR( cmdInfo.m_strFileName ));

		i3String::SplitFileExt( szFileName, szExt, 32 );
		if( strcmp( szExt, "MVF" ) == 0 )	OpenMVF( szFileName );
		else if( strcmp( szExt, "MVFList") == 0 ) ImportFromList( szFileName );
	}

}
