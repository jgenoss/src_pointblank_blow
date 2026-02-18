// i3RSCGenView.cpp : Ci3RSCGenView 클래스의 구현
//

#include "stdafx.h"
#include "i3RSCGen.h"

#include "i3RSCGenDoc.h"
#include "i3RSCGenView.h"

#include "GlobalVar.h"
#include ".\i3rscgenview.h"

#include "i3Base/i3StringListBuffer.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/compare/generic_is_niequal.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
// Ci3RSCGenView

IMPLEMENT_DYNCREATE(Ci3RSCGenView, CView)

BEGIN_MESSAGE_MAP(Ci3RSCGenView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
END_MESSAGE_MAP()

// Ci3RSCGenView 생성/소멸


Ci3RSCGenView::Ci3RSCGenView()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	m_pRoot = NULL;

	ZeroMemory( m_pTypeRoot, sizeof( RSCNode *) * I3RSC_TYPE_COUNT);

	m_szFileName[0] = 0;
	
}

Ci3RSCGenView::~Ci3RSCGenView()
{
	I3_SAFE_RELEASE( m_pRoot);
}

BOOL Ci3RSCGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.
	cs.style |= WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

// Ci3RSCGenView 그리기

void Ci3RSCGenView::OnDraw(CDC* /*pDC*/)
{
	Ci3RSCGenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// Ci3RSCGenView 인쇄

BOOL Ci3RSCGenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void Ci3RSCGenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void Ci3RSCGenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// Ci3RSCGenView 진단

#ifdef _DEBUG
void Ci3RSCGenView::AssertValid() const
{
	CView::AssertValid();
}

void Ci3RSCGenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

Ci3RSCGenDoc* Ci3RSCGenView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Ci3RSCGenDoc)));
	return (Ci3RSCGenDoc*)m_pDocument;
}
#endif //_DEBUG


// Ci3RSCGenView 메시지 처리기

int Ci3RSCGenView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rtWnd( 0, 0, 640, 480);

	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles( TRUE);

	// TODO:  Add your specialized creation code here
	{
		m_wndSplit1.Create(0, WS_CHILD | WS_VISIBLE, rtWnd, 48, this, IDC_SPLITTER0);
		m_wndSplit2.Create( I3_SPLS_EX_VERT, WS_CHILD | WS_VISIBLE, rtWnd, 48, &m_wndSplit1, IDC_SPLITTER1);

		m_PaneTree.Create( CPaneTree::IDD, &m_wndSplit1);
		m_PaneSpec.Create( CPaneSpec::IDD, &m_wndSplit2);
		m_PaneState.Create( CPaneState::IDD, &m_wndSplit2);

		m_wndSplit1.AddPane( &m_PaneTree);
		m_wndSplit1.AddPane( &m_wndSplit2);
		// 2011-05-06 김재진
	//	m_PaneTree.FileListLink(&m_FileList);
		////////
		m_wndSplit2.AddPane( &m_PaneSpec);
		m_wndSplit2.AddPane( &m_PaneState);
	}

	// 2011-04-28 김재진
	m_bFirst = true;
	m_bFile = false;
	///
	
	CreateRootScene();

	int num = 0;
	CStringW strw(GetCommandLine());

	LPWSTR* argv = CommandLineToArgvW(strw, &num);
	int argvCount = 0;
	if( num > 1 )
	{
		char temp[512] = {0,};
		
		int nLen = ( int )wcslen( argv[argvCount] );
		wcstombs( temp, argv[argvCount++], nLen + 1 );

		if( strcmp("-b", temp) == 0 )
		{
			int nLen2 = ( int )wcslen( argv[1] );
			wcstombs( temp, argv[1], nLen2 + 1 );

			std::string sRSGPath = temp;
			INT32 nPos = sRSGPath.rfind("\\");
			if( nPos != -1 )
			{
				sRSGPath = sRSGPath.substr(0, nPos);
			}
			nPos = sRSGPath.rfind("\\");
			if( nPos != -1 )
			{
				sRSGPath = sRSGPath.substr(0, nPos);
			}
			sRSGPath += "\\Weapon.RSG";
			OpenRSCFile(sRSGPath.c_str());
			AddFile( temp );
			OnFileSave();
			TerminateProcess(GetCurrentProcess(),0);
		}
		else if( strcmp("-SaveRSC", temp) == 0 )
		{
			//미디어경로
			int nLen2 = ( int )wcslen( argv[argvCount] );
			wcstombs( temp, argv[argvCount++], nLen2 + 1 );

			std::string sMediaPath = temp;

			//RSG경로
			nLen2 = ( int )wcslen( argv[argvCount] );
			wcstombs( temp, argv[argvCount++], nLen2 + 1 );

		
			std::string sRSGPath = sMediaPath;
			sRSGPath += "\\";
			sRSGPath += temp;

			OpenRSCFile(sRSGPath.c_str());

			//i3파일경로
			nLen2 = ( int )wcslen( argv[argvCount] );
			wcstombs( temp, argv[argvCount++], nLen2 + 1 );

			char chDotName[512] = {0,};
			nLen2 = ( int )wcslen( argv[argvCount] );
			wcstombs( chDotName, argv[argvCount++], nLen2 + 1 );

			WIN32_FIND_DATA FindData;
			char _TempPath[MAX_PATH];
			sprintf( _TempPath, "%s\\%s", temp, chDotName);
			HANDLE hFind = FindFirstFile( _TempPath, &FindData);

			if( hFind != INVALID_HANDLE_VALUE)
			{
				do
				{
					if((strcmp( FindData.cFileName, ".") != 0) && ( strcmp( FindData.cFileName, "..") != 0))
					{
						std::string filePath = temp;
						filePath += "\\";
						filePath += FindData.cFileName;
						AddFile( filePath.c_str() );
					}
				}while( FindNextFile( hFind, &FindData) == TRUE);

				FindClose( hFind);
			}
			OnFileSave();

			TerminateProcess(GetCurrentProcess(),0);
		}
	}

	return 0;
}

RSCNode * Ci3RSCGenView::_GetTypeRoot( INT32 nRscType)
{
	I3ASSERT( nRscType < I3RSC_TYPE_COUNT);

	if( m_pTypeRoot[nRscType] == NULL)
	{
		const char * szRootName[I3RSC_TYPE_COUNT] = 	{	"Scene", "Charactor", "World", "Animation", "Texture", "Sound",	"Binary", "AnimationPack", "Object", "UI"};
		
		m_pTypeRoot[nRscType] = RSCNode::new_object_ref();
		m_pTypeRoot[nRscType]->SetName( szRootName[nRscType]);
		m_pTypeRoot[nRscType]->SetStyle( I3RSC_ATTR_FOLDER | nRscType);

		m_pRoot->AddChild( m_pTypeRoot[nRscType]);
	}

	return m_pTypeRoot[nRscType];
}

void Ci3RSCGenView::CreateRootScene()
{
	SetRoot( NULL);
	{
		RSCNode * pNewRoot;

		//SetRoot를 통해서 root가 되기때문에 new_object_ref()로
		pNewRoot = RSCNode::new_object_ref();
		pNewRoot->SetName( "/");
		pNewRoot->SetStyle( I3RSC_ATTR_FOLDER);

		SetRoot( pNewRoot);

		i3mem::FillZero( m_pTypeRoot, sizeof( RSCNode*) * I3RSC_TYPE_COUNT);
	}
}

void Ci3RSCGenView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_wndSplit1.m_hWnd))
	{
		m_wndSplit1.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3RSCGenView::MakeExceptList( void)
{
	m_ExceptList.clear();

	CopyFromBufferToStringList(m_ExceptList, g_pProfile->m_szExceptRull);
	//	m_ExceptList.CopyFromBuffer( g_pProfile->m_szExceptRull);
}

BOOL Ci3RSCGenView::IsAcceptable( char * pszStr)
{
	
	for(size_t i = 0; i < m_ExceptList.size(); i++)
	{
	//	if( i3String::Compare( pszStr, m_ExceptList.Items[i]) == 0)
		if ( pszStr == m_ExceptList[i] )
			return FALSE;
	}

	return TRUE;
}

void Ci3RSCGenView::_ClearAll()
{
	g_pRoot = NULL;
	m_PaneTree.UpdateAll();
	m_PaneState.ClearLog();
	m_PaneSpec.SetNode( NULL);
}

void Ci3RSCGenView::SetRoot( RSCNode * pRoot)
{
	if( pRoot != NULL)
	{
		pRoot->AddRef();
	}

	_ClearAll();

	I3_SAFE_RELEASE( m_pRoot);
	m_NodeMap.clear();

	m_pRoot = pRoot;
	g_pRoot = pRoot;

	if( m_pRoot != NULL)
	{
		m_pRoot->GatherAllNodeToPathMap( m_NodeMap);
	}

	m_PaneTree.UpdateAll();
}

void Ci3RSCGenView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	INT32	nCount, i;
	char	TempName[MAX_PATH << 1];
	HANDLE	hFind;
	WIN32_FIND_DATA fi;

	// file drag count
	nCount = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);
	
	MakeExceptList();
	
	for( i = 0; i < nCount; ++i)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof( TempName) -1);

		hFind = FindFirstFile( TempName, &fi);
		FindClose( hFind);

		if( fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			AddDirectory( TempName);
		}
		else
		{
			char Ext[32];
			// 확장자를 알기위한 절차
		//	i3String::SplitFileExt( TempName, Ext, 31);
			i3::extract_fileext(TempName, Ext);

			if( i3::generic_is_iequal( Ext, "RSG") )
			{
				//2011-05-03 김재진
				m_bFile = false;
				m_bFirst = true;
				////////////////
				OpenRSCFile( TempName);
			}
			else
			{
				AddFile( TempName);
			}
		}
	}

	DragFinish( hDropInfo);
}

void Ci3RSCGenView::CreateNodeInfo( RSCNode * pNode, INT32 nStyle)
{
	I3ASSERT( pNode != NULL);

	CFieldPack * pPack = new CFieldPack;


	pPack->Create( nStyle);

	pNode->GetData()->m_spField.reset(pPack);

	{//Working Path를 기준으로한 Relative Path를 설정한다.
		RSC_FIELD_INFO * pNameField = pPack->GetField(1);

		if( pNameField != NULL)
		{
			if(	i3::generic_is_iequal( pNameField->m_strFieldName, "Path")  ||
				i3::generic_is_iequal( pNameField->m_strFieldName, "Scene") )
			{
				pNameField->m_FieldData = pNode->GetRelativePath();
			}
		}
	}

	if( nStyle == I3RSC_TYPE_SOUND)
	{
		RSC_FIELD_INFO * pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 4);
		pInfo->m_FieldData = 100;

		pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 5);
		pInfo->m_FieldData = 100;

		//사용하지 않는거
		//pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 2);
		//pInfo->m_FieldData = 5.0f;

		pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 3);
		pInfo->m_FieldData = 50.0f;
	}
}

RSCNode *	Ci3RSCGenView::FindByRelPath( const char * pszRelPath)
{
	
	i3::fixed_string key =	RSCNode::CalcKeyFixedString(pszRelPath);
	
	i3::unordered_map<i3::fixed_string, RSCNode*>::iterator it = m_NodeMap.find(key);
	
	return ( it == m_NodeMap.end()) ? NULL : it->second;
}

void Ci3RSCGenView::AddFile( const char * pszFileName)
{
	i3::rc_string	strRelPath;
	char szExt[32] = { 0, };

	const char * pszName;

	m_bFile = true;


	i3::vector<i3::rc_string> StrList;

	UINT32	nStyle = 0;
	RSCNode * pNode, * pChild;
	HTREEITEM hitemParent;

	// 파일 전체경로 정보 중 WorkDir 경로 삭제
	// 2011-04-27 김재진
	if(!AutoWorkPath(pszFileName, false)) return;
	
	i3::make_relative_path( m_szWorkDir, pszFileName, strRelPath);

	//상위 폴더에 있는 파일이다.
	if( m_UpStageFile )
	{
		i3::string new_path = m_szWorkDir;

		new_path.erase( 0, new_path.find_last_of( '\\' ) + 1 );
		new_path.append( "\\" );
		new_path.append( strRelPath) ;
		
		strRelPath = new_path;

	}
	

	
	// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리
//	i3String::MakeRelativePath( m_vecWorkingDir[m_iIndex - 1], pszFileName, szRelPath);

	i3::resolve_path_token( strRelPath, StrList);

//	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt) -1);
	i3::extract_fileext(pszFileName, szExt);

	if( IsAcceptable( (char*)pszFileName))
	{
		//File Attr
		nStyle = Geti3RscTypeWithFileExt( szExt);

		pNode = _GetTypeRoot(nStyle);

		hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pNode);

		if (hitemParent == NULL)		// pNode에 해당하는 TREEITEM을 못찾은것에 대응해야한다.(2014.04.18.수빈)
		{
			hitemParent = m_PaneTree.AddTree(m_PaneTree.GetRootItem(), pNode);			
		}

		pszName =	StrList.back().c_str();		// StrList.Items[ StrList.GetCount() -1];	//StrList의 Last가 실제 File Name이다.

		//매파일마다 추가작업이 들어가 많이 느립니다.
		// pChild = pNode->_Rec_FindChildByName( pszName, szRelPath);		// 원래 주석..
		//
		
		pChild = FindByRelPath( strRelPath.c_str() );

		if( pChild == NULL && CheckSameName(pszName))
		{
			pChild = RSCNode::new_object_ref();

			pChild->SetName( pszName);
			pChild->SetRelativePath( strRelPath);

			CreateNodeInfo( pChild, nStyle);

			pNode->AddChild( pChild);

			pChild->AddThisToPathMap(m_NodeMap);

			{
				HANDLE hFind;
				WIN32_FIND_DATA FindData;

				hFind = FindFirstFile( pszFileName, &FindData);

				if( hFind != INVALID_HANDLE_VALUE)
				{
					pChild->AddStyle( nStyle);

					FindClose( hFind);
				}
			}
		
			hitemParent = m_PaneTree.AddTree( hitemParent, pChild);

			//pChild->setTreeItemHandle( hitemParent);
			// 2011-05-04 김재진 : 새로 등록된 File명 정보 저장을 위한 AddFileToMap() 호출
			AddFileToMap(pszName);

			{
				I3PRINTLOG(I3LOG_NOTICE, "Added File: %s", strRelPath.c_str());
			}
		}
		else
		{
			hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pChild, hitemParent, FALSE);
			//hitemParent = pChild->getTreeItemHandle();
			//I3ASSERT( hitemParent != NULL);

			{
				I3PRINTLOG(I3LOG_WARN,"Already Exists: %s", strRelPath.c_str());
			}
		}
	}

	//m_PaneTree.UpdateAll();
}

void Ci3RSCGenView::AddDirectory( const char * pszPath)
{
	// 2011-04-27 김재진
	if(!AutoWorkPath(pszPath))
		return;

	LookForFiles( pszPath, m_pRoot);

	m_PaneTree.UpdateAll();
}

BOOL Ci3RSCGenView::LookForFiles( const char * pszPath, RSCNode * pParent)
{
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	char _TempPath[MAX_PATH];
	char _szRelPath[MAX_PATH];
	RSCNode * pNode, *pChild;
	char szExt[32] = { 0, };

	sprintf( _TempPath, "%s\\*.*", pszPath);

	hFind = FindFirstFile( _TempPath, &FindData);

	if( hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if((strcmp( FindData.cFileName, ".") != 0) && ( strcmp( FindData.cFileName, "..") != 0))
			{
				if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					!(FindData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
				{
					sprintf( _TempPath, "%s\\%s", pszPath, FindData.cFileName);

					//
					LookForFiles( _TempPath, m_pRoot);
				}
				else
				{
					if( IsAcceptable( FindData.cFileName))
					{
						// SHGetFileInfo();
					//	i3String::SplitFileExt( FindData.cFileName, szExt, sizeof( szExt) - 1);
						i3::extract_fileext(FindData.cFileName, szExt);

						i3::safe_string_copy( _szRelPath, _TempPath, MAX_PATH );

						i3::remove_slash_filename(_szRelPath);
				//		i3String::RemoveFileName( _szRelPath);

						strcat( _szRelPath, "\\");
						strcat( _szRelPath, FindData.cFileName);
						
						// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리
						//i3String::MakeRelativePath( g_pProfile->m_szWorkDir, _szRelPath, _szRelPath);
						i3::make_relative_path( m_szWorkDir, _szRelPath, _szRelPath );
						
						pChild = m_pRoot->_Rec_FindChildByName( FindData.cFileName, _szRelPath);

						if( pChild == NULL)
						{
							if( IsAcceptable( szExt))
							{
								pNode = RSCNode::new_object_ref();

								pNode->SetName( FindData.cFileName);
								pNode->SetRelativePath( _szRelPath);

								INT32 nStyle = Geti3RscTypeWithFileExt( szExt);
								
								pParent = _GetTypeRoot(nStyle);
								pNode->AddStyle( nStyle);

								CreateNodeInfo( pNode, nStyle);

								pParent->AddChild( pNode);
								pNode->AddThisToPathMap(m_NodeMap);

								I3PRINTLOG(I3LOG_NOTICE, "Added File: %s", _szRelPath);
							}
						}
						else
						{
							I3PRINTLOG(I3LOG_WARN, "Already Exists : %s", _szRelPath);
						}
					}
				}
			}
		}while( FindNextFile( hFind, &FindData) == TRUE);

		FindClose( hFind);
	}

	return TRUE;
}

void Ci3RSCGenView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CCommandLineInfo cmdInfo;

	AfxGetApp()->ParseCommandLine( cmdInfo);

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		char szFileName[MAX_PATH];
		char szExt[32];
		strcpy( szFileName, (LPCTSTR)cmdInfo.m_strFileName);

	//	i3String::SplitFileExt( szFileName, szExt, 32);
		i3::extract_fileext(szFileName, szExt);

		if( i3::generic_is_iequal( szExt, "RSG") )	
		{
			OpenRSCFile( szFileName);
		}
		else if( i3::generic_is_iequal( szExt, "i3Reg") )
		{
			i3Registry::Init( szFileName);

			i3RegKey * pOption = FIND_REG_KEY( REG_OPTIONAL_ROOT);
			if( pOption != NULL)
			{
				i3::rc_wstring	wstr;
				FIND_REG_DATA( pOption, "Working Directory", wstr );
				
				i3::string strFilename;		i3::utf16_to_mb(wstr, strFilename);

							
				// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리 
				// strcpy( g_pProfile->m_szWorkDir, szFileName);

				// i3ResourceFile::SetWorkingFolder( g_pProfile->m_szWorkDir);
				if(! AutoWorkPath(strFilename.c_str(), true) ) return;
				
				i3ResourceFile::setSearchExternRefEnable( true);
			}

			i3RegKey * pConfig = FIND_REG_KEY( "Config");
			if( pConfig != NULL)
			{
				i3::rc_wstring	wstr;
				FIND_REG_DATA( pConfig, "RSCFileName", wstr );
				i3::string strFileName;		i3::utf16_to_mb(wstr, strFileName);
				
				i3::change_file_ext( strFileName, "RSG");

				OpenRSCFile( strFileName.c_str() );
			}
		}
	}
}

void Ci3RSCGenView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CView::OnLButtonDown(nFlags, point);
}

void Ci3RSCGenView::OnFileSaveAs()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "RSG", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Resource File (*.RSG)|*.RSG||", NULL);

	if( Dlg.DoModal() != IDOK)	return;
	if( m_pRoot == NULL)		return;

	char szFileName[MAX_PATH];
	strcpy( szFileName, LPCTSTR( Dlg.GetPathName()));

	BuildRSCFile( szFileName);
	if (AfxGetMainFrame()->GetRSGToolBar()->IsRSCSave())
		ExportRSCBin( szFileName);
}

void Ci3RSCGenView::OnFileSave()
{
	// TODO: Add your command handler code here
	if( m_szFileName[0] == 0)
	{
		OnFileSaveAs();
		return;
	}
	
	BuildRSCFile( m_szFileName);
	if( AfxGetMainFrame()->GetRSGToolBar()->IsRSCSave() )
		ExportRSCBin( m_szFileName);
}

void Ci3RSCGenView::_ReadRSCFile( i3FileStream * pStream)//const char * szRSCFileName)
{
	i3IniParser	Parser;

	if( !Parser.Open( pStream))
	{
		I3PRINTLOG(I3LOG_FATAL,"Could not Open Stream", pStream->GetFileName());
		return;
	}

	strcpy( m_szFileName, pStream->GetFileName());

	INT32 i;
	INT32 nCount = Parser.GetSectionCount();
	for( i =0; i< nCount; ++i)
	{
		if( Parser.ReadSectionByIndex( i))
		{
			i3::rc_string strFileName;

			RSCNode * pNode = RSCNode::new_object_ref();
			RSC_DATA * pData = pNode->GetData();

			i3IniSectionInfo * pSection = Parser.GetSection(i);
			
			pData->m_strFilePath = pSection->Name;
		
			i3::extract_filename(pData->m_strFilePath, strFileName);
			
			//2011-02-10 하동익//
			AddFileToMap(strFileName.c_str());
			////////////////////

			pNode->SetName( strFileName);

			CFieldPack * pPack = new CFieldPack;
			pPack->Load( &Parser, pSection);

			pData->m_spField.reset(pPack);

			i3::rc_string* any_val = i3::unsafe_any_cast<i3::rc_string>(&pPack->GetField(0)->m_FieldData);

			for( INT32 j = 0; j < I3RSC_TYPE_COUNT; ++j)
			{
				if (*any_val == g_RSCStyleDef[j].m_strFieldPackName)
				{
					pData->m_Attr = j;
					break;
				}
			}

			pNode->GatherAllNodeToPathMap( m_NodeMap);

			_GetTypeRoot( pData->m_Attr & I3RSC_TYPE_MASK)->AddChild( pNode);

			I3PRINTLOG(I3LOG_NOTICE, "Added File: %s", pData->m_strFilePath.c_str());
		}
	}
	
	m_PaneTree.UpdateAll();

	Parser.Close();
}

void Ci3RSCGenView::BuildRSCFile( const char * szRSCFileName)
{
	i3IniParser	Parser;
	
	if( !Parser.Create(szRSCFileName))
	{
		I3PRINTLOG(I3LOG_FATAL,"Could not Open File", (char*)szRSCFileName);
		return;
	}

	strcpy( m_szFileName, szRSCFileName);

	_Rec_BuildRSCFile( &Parser, m_pRoot);

	Parser.Close();
}

void Ci3RSCGenView::_Rec_BuildRSCFile( i3IniParser * pParser, RSCNode * pNode)
{
	if( pNode == NULL)	return;

	CFieldPack * pField = pNode->GetData()->m_spField.get();

	if( (pNode->GetStyle() & I3RSC_ATTR_FOLDER) == 0)
	{
		if( pField != NULL)
		{
			pField->Save( pParser);
		}
	}

	INT32 nCount = pNode->GetChildCount();
	RSCNode * pChildNode  = NULL;
	for( INT32 i = 0; i< nCount; ++i)
	{
		pChildNode = pNode->GetChild( i);

		_Rec_BuildRSCFile( pParser, pChildNode);
	}
}

void Ci3RSCGenView::OnFileOpen()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( TRUE, "MVF", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Resource File (*.RSG)|*.RSG||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	// 2011-04-28 김재진
	m_bFirst = true;
	m_bFile = false;
	///
	OpenRSCFile( LPCTSTR( Dlg.GetPathName()));
}

void Ci3RSCGenView::OpenRSCFile( const char * pszFileName)
{
	i3FileStream File;
	if( !File.Open(pszFileName, STREAM_READ))
	{
		I3PRINTLOG(I3LOG_FATAL,"Couldn't Open File: %s", (char*)pszFileName);
		return;
	}

	// 2011-04-27 김재진
	if(!AutoWorkPath(pszFileName, true)) return;
	
	m_PaneState.ClearLog();
	CreateRootScene();

	_ReadRSCFile( &File);

	File.Close();
}


static void _ExportRSC( const i3::vector<RSCNode*>& List, char * pszFileName)
{
	I3ASSERT( pszFileName != NULL);
	
	i3FileStream FileStream;
	char szFileName[MAX_PATH];
	i3::change_file_ext_copy( szFileName, pszFileName, "RSC");

	UINT32	nResult = 0;
	UINT8	nID[4];
	INT32	nCount = (INT32)List.size();

	if( FileStream.Create( szFileName, STREAM_WRITE | STREAM_SHAREWRITE) == STREAM_ERR)
	{
		return;
	}

	i3mem::Copy( nID, "RSCF", sizeof( UINT8) * 4);

	// File ID
	FileStream.Write( nID, sizeof( UINT8) * 4);

	// list Count
	FileStream.Write( &nCount, sizeof( INT32));

	for( INT32 i = 0; i < nCount ; ++i)
	{
		RSCNode * pNode = List[i];
		nResult = pNode->Export( &FileStream);
	}

	return;
}

void Ci3RSCGenView::_Rec_AddExportList( RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList)
{
	if( pParent == NULL)
		return;

	if( i3::same_of<RSCNode*>(pParent)) //->IsExactTypeOf( RSCNode::static_meta()))
	{
		RSC_DATA * pData = pParent->GetData();
		if(pParent != m_pRoot)
		{
			if( (pData->m_Attr & I3RSC_ATTR_FOLDER ) == 0)
			{
				ExportList.insert( pParent);
			}
		}
	}

	for( INT32 i = 0; i < pParent->GetChildCount(); ++i)
	{
		RSCNode * pChild = (RSCNode *)pParent->GetChild( i);
		if( pChild != NULL)
		{
			_Rec_AddExportList( pChild, ExportList);
		}
	}
}



void Ci3RSCGenView::ExportRSCBin( char * pszFileName)
{
	if( pszFileName == NULL)
		return;

	if( m_pRoot == NULL)
		return;

	i3::vector<RSCNode*> ExportList;
	m_pRoot->GatherAllNode_NonFolder(ExportList);
	i3::sort(ExportList.begin(), ExportList.end(), RSCNode_ExportComp());

	_ExportRSC( ExportList, pszFileName);
}

void Ci3RSCGenView::OnFileNew()
{
	// TODO: Add your command handler code here
	m_bFirst = true;
	m_bFile = false;
	CreateRootScene();
}

void Ci3RSCGenView::AddFileToMap(const char *name)
{
	set<std::string> & FileList = m_PaneTree.GetFileNameList();

	FileList.insert( name );

}

bool Ci3RSCGenView::CheckSameName(const char *name)
{
	set< std::string >::iterator it = m_PaneTree.GetFileNameList().find(name);
	if( it != m_PaneTree.GetFileNameList().end())
		return false;
	else
		return true;
}

// 2011-04-27 김재진
// Working Path 설정
bool Ci3RSCGenView::AutoWorkPath(const char* pszFileName, bool bPrsv)
{
	m_UpStageFile = false;

	if(pszFileName == NULL)	return false;

	int iPosition = 0;
	// 파일 경로를 임시저장 변수
	char TempPath[MAX_PATH];
	// 최종 경로 저장 변수
	char CopyPath[MAX_PATH];
	memset(CopyPath, 0x00, sizeof(char) * MAX_PATH);			// 잘못된 주소참조 수정.(2014.04.14.수빈)
	i3::safe_string_copy( TempPath, pszFileName, MAX_PATH );
	// 경로명만 문자열에 저장
	//i3String::RemoveFileName( TempPath);
	i3::remove_slash_filename(TempPath);
	
	if(bPrsv || m_bFile && m_bFirst)
	{
		// 불러온 파일보다 상위폴더로 저장
		for( int charlen = 0 ; charlen <= MAX_PATH ; charlen++ )
		{
			// 문자열 종료시 반복문 종료
			if( TempPath[charlen] == '\0' ) break;
			// 현재 경로명에서 상위 폴더를 정보를 구하기 위한 정보
			if( TempPath[charlen] == '\\') 
				iPosition  = charlen;
		}
		// 현재 경로명에서 상위 폴더를 복사
		// iPosition : 상위 폴더 위치정보
		i3::string_ncopy_nullpad(CopyPath, TempPath, iPosition);
		CopyPath[iPosition + 1] = '\0';
		
		// 현재 작업중인 폴더명을 저장한다.
		memset(m_szWork, 0x00, sizeof(m_szWork));		// 잘못된 주소참조 수정.(2014.04.14.수빈)
		i3::make_relative_path(CopyPath, TempPath, m_szWork );
	}
	else
	{
		i3::safe_string_copy(CopyPath, TempPath, MAX_PATH );
	}
	// 시작할때만 Working Folder 추가
	if( m_bFirst )
	{
		memset(m_szWorkDir, 0x00, sizeof(m_szWorkDir));		// 잘못된 주소참조 수정.(2014.04.14.수빈)
		i3::safe_string_copy(m_szWorkDir, CopyPath, MAX_PATH );
		m_bFirst = false;
	}
	
	// 처음 등록된 위치와 동일하지 않을 경우 메시지 출력
	char szTemp[ MAX_PATH];
	i3::make_relative_path(m_szWorkDir, TempPath, szTemp );
	if( i3::generic_is_niequal(m_szWork, szTemp, strlen(m_szWork)) == false )
	{
		if( AfxMessageBox("상위폴더 내용을 추가 하시겠습니까?", MB_YESNO|MB_ICONSTOP
			| MB_ICONQUESTION | MB_DEFBUTTON2) == IDNO )
		{
			return false;
		}
		else
		{
			m_UpStageFile = true;
		}
	}

	// 현재경로 변경
	if(	!WorkingFolderYN( CopyPath) )	return false;
	
	return true;
}

// Working Folder 변경여부 함수
bool Ci3RSCGenView::WorkingFolderYN(const char* pszNewFolder)
{
	if( i3::generic_is_niequal(m_szWorkDir, pszNewFolder, strlen(m_szWorkDir)) )
	{
		return true;
	}
	else
	{
		AfxMessageBox("상위폴더로 추가할 수 없습니다.");
		return false;
	}
}