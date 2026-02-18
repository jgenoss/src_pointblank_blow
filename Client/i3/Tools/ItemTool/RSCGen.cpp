// i3RSCGenView.cpp : cRSCGen 클래스의 구현
//

#include "stdafx.h"
#include "RSCGen.h"
#include "i3RSCGenProj.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_slash_filename.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( cRSCGen); //, i3ElementBase);

cRSCGen::cRSCGen()
{
	m_pRoot = NULL;
	ZeroMemory( m_pTypeRoot, sizeof( RSCNode *) * I3RSC_TYPE_COUNT);

	m_szFileName[0] = 0;

	CreateRootScene();
}

cRSCGen::~cRSCGen()
{
	I3_SAFE_RELEASE( m_pRoot);
}
// 
// int cRSCGen::OnCreate(LPCREATESTRUCT lpCreateStruct)
// {
// 	CRect rtWnd( 0, 0, 640, 480);
// 
// 	if (CView::OnCreate(lpCreateStruct) == -1)
// 		return -1;
// 
// 	DragAcceptFiles( TRUE);
// 
// 	// 2011-04-28 김재진
// 	m_bFirst = true;
// 	m_bFile = false;
// 	///
// 	
// 	CreateRootScene();
// 	
// 	return 0;
// }

RSCNode * cRSCGen::_GetTypeRoot( INT32 nRscType)
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

void cRSCGen::CreateRootScene()
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

void cRSCGen::MakeExceptList( void)
{
	m_ExceptList.clear();

	//CopyFromBufferToStringList(m_ExceptList, g_pProfile->m_szExceptRull);
	//	m_ExceptList.CopyFromBuffer( g_pProfile->m_szExceptRull);
}

BOOL cRSCGen::IsAcceptable( char * pszStr)
{
	
	for(size_t i = 0; i < m_ExceptList.size(); i++)
	{
	//	if( i3String::Compare( pszStr, m_ExceptList.Items[i]) == 0)
		if ( pszStr == m_ExceptList[i] )
			return FALSE;
	}

	return TRUE;
}

void cRSCGen::_ClearAll()
{
//	g_pRoot = NULL;
// 	m_PaneTree.UpdateAll();
// 	m_PaneState.ClearLog();
// 	m_PaneSpec.SetNode( NULL);
}

void cRSCGen::SetRoot( RSCNode * pRoot)
{
	if( pRoot != NULL)
	{
		pRoot->AddRef();
	}

	_ClearAll();

	I3_SAFE_RELEASE( m_pRoot);
	m_NodeList.clear();

	m_pRoot = pRoot;
//	g_pRoot = pRoot;

	if( m_pRoot != NULL)
	{
		m_pRoot->ScanAllNode( m_NodeList);
	}

	//m_PaneTree.UpdateAll();
}

void cRSCGen::CreateNodeInfo( RSCNode * pNode, INT32 nStyle)
{
	I3ASSERT( pNode != NULL);

	CFieldPack * pPack = CFieldPack::new_object();

	I3ASSERT( pPack != NULL);

	pPack->Create( nStyle);

	pNode->GetData()->m_pField = pPack;

	{//Working Path를 기준으로한 Relative Path를 설정한다.
		RSC_FIELD_INFO * pNameField = pPack->GetField(1);

		if( pNameField != NULL)
		{
			if(i3::generic_is_iequal( pNameField->m_szFieldName, "Path")  ||
			i3::generic_is_iequal( pNameField->m_szFieldName, "Scene") )
			{
				if( pNameField->m_pFieldData != NULL)
				{
					strcpy( (char*)pNameField->m_pFieldData, pNode->GetRelativePath());
				}
			}
		}
	}

	if( nStyle == I3RSC_TYPE_SOUND)
	{
		RSC_FIELD_INFO * pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 4);
		*((INT32*)pInfo->m_pFieldData) = 100;

		pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 5);
		*((INT32*)pInfo->m_pFieldData) = 100;

		pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 2);
		*((REAL32*)pInfo->m_pFieldData) = 5.0f;

		pInfo = ( RSC_FIELD_INFO*)pPack->GetField( 3);
		*((REAL32*)pInfo->m_pFieldData) = 50.0f;
	}
}

RSCNode *	cRSCGen::FindByRelPath( const char * pszRelPath)
{
	UINT128 md5;
	i3MD5::Gen( (char *) pszRelPath, strlen( pszRelPath), (char *) &md5);
//	INT32 idx = m_NodeList.FindItem( &md5);
	i3::vector_set<RSCNode*, RSCNode_NodeComp>::iterator it = m_NodeList.find_other(md5);

	if( it == m_NodeList.end() )
		return NULL;
	return *it; 
}

void cRSCGen::AddFile( const char * pszFileName)
{
	char szRelPath[MAX_PATH];
	char szExt[32] = { 0, };

	const char * pszName;

	m_bFile = true;


	i3::vector<i3::rc_string> StrList;

	UINT32	nStyle = 0;
	RSCNode * pNode, * pChild;
	//HTREEITEM hitemParent;

	// 파일 전체경로 정보 중 WorkDir 경로 삭제
	// 2011-04-27 김재진
	if(!AutoWorkPath(pszFileName, false)) return;
	
	i3::make_relative_path( m_szWorkDir, pszFileName, szRelPath );

	//상위 폴더에 있는 파일이다.
	if( m_UpStageFile )
	{
		std::string new_path = m_szWorkDir;

		new_path.erase( 0, new_path.find_last_of( '\\' ) + 1 );
		new_path.append( "\\" );
		new_path.append( std::string(szRelPath) );

		strcpy_s( szRelPath, MAX_PATH, new_path.c_str() );
	}
	

	
	// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리
//  //	i3String::MakeRelativePath( m_vecWorkingDir[m_iIndex - 1], pszFileName, szRelPath);

	i3::resolve_path_token( szRelPath, StrList);

//	i3String::SplitFileExt( pszFileName, szExt, sizeof( szExt) -1);
	i3::extract_fileext(pszFileName, szExt);

	if( IsAcceptable( (char*)pszFileName))
	{
		//File Attr
		nStyle = GetResType( (const char *) szExt);

		pNode = _GetTypeRoot(nStyle);

		//hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pNode);

		pszName =	StrList.back().c_str();		// StrList.Items[ StrList.GetCount() -1];	//StrList의 Last가 실제 File Name이다.

		//매파일마다 추가작업이 들어가 많이 느립니다.
		// pChild = pNode->_Rec_FindChildByName( pszName, szRelPath);		// 원래 주석..
		//
		
		pChild = FindByRelPath( (const char *) szRelPath);

		if( pChild == NULL && CheckSameName(pszName))
		{
			pChild = RSCNode::new_object_ref();

			pChild->SetName( pszName);
			pChild->SetRelativePath( szRelPath);

			CreateNodeInfo( pChild, nStyle);

			pNode->AddChild( pChild);

			m_NodeList.insert( pChild);

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
		
			//hitemParent = m_PaneTree.AddTree( hitemParent, pChild);

			//pChild->setTreeItemHandle( hitemParent);
			// 2011-05-04 김재진 : 새로 등록된 File명 정보 저장을 위한 AddFileToMap() 호출
			AddFileToMap(pszName);

			{
				I3NOTICE( "Added File: %s", szRelPath);
			}
		}
		else
		{
			//hitemParent = i3TDK::FindTreeItemByData( &m_PaneTree.m_TreeCtrl, pChild, hitemParent, FALSE);
			//hitemParent = pChild->getTreeItemHandle();
			//I3ASSERT( hitemParent != NULL);

			{
				I3WARN("Already Exists: %s", szRelPath);
			}
		}
	}

	//m_PaneTree.UpdateAll();
}

void cRSCGen::AddDirectory( const char * pszPath)
{
	// 2011-04-27 김재진
	if(!AutoWorkPath(pszPath))
		return;

	LookForFiles( pszPath, m_pRoot);

	//m_PaneTree.UpdateAll();
}

BOOL cRSCGen::LookForFiles( const char * pszPath, RSCNode * pParent)
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
					//	i3String::RemoveFileName( _szRelPath);

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

								INT32 nStyle = GetResType( (const char *) szExt);
								
								pParent = _GetTypeRoot(nStyle);
								pNode->AddStyle( nStyle);

								CreateNodeInfo( pNode, nStyle);

								pParent->AddChild( pNode);
								m_NodeList.insert( pNode);

								I3NOTICE( "Added File: %s", _szRelPath);
							}
						}
						else
						{
							I3WARN( "Already Exists : %s", _szRelPath);
						}
					}
				}
			}
		}while( FindNextFile( hFind, &FindData) == TRUE);

		FindClose( hFind);
	}

	return TRUE;
}

void cRSCGen::OnInitialUpdate()
{
	//CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CCommandLineInfo cmdInfo;

	AfxGetApp()->ParseCommandLine( cmdInfo);

	i3::rc_wstring str;

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		char szFileName[MAX_PATH];
		char szExt[32];
		strcpy( szFileName, (LPCTSTR)cmdInfo.m_strFileName);

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
				FIND_REG_DATA( pOption, "Working Directory", str);

				i3::generic_string_copy( szFileName, str);
				
				// 2011-04-27 김재진 : Working Directory 변수 변경으로 인한 주석처리 
				// strcpy( g_pProfile->m_szWorkDir, szFileName);

				// i3ResourceFile::SetWorkingFolder( g_pProfile->m_szWorkDir);
				if(! AutoWorkPath(szFileName, true) ) return;
				
				i3ResourceFile::setSearchExternRefEnable( true);
			}

			i3RegKey * pConfig = FIND_REG_KEY( "Config");
			if( pConfig != NULL)
			{
				char szTemp[MAX_PATH];
				FIND_REG_DATA( pConfig, "RSCFileName", str);
				i3::generic_string_copy( szFileName, str);
				i3::change_file_ext_copy( szTemp, szFileName, "RSG");

				OpenRSCFile( szTemp);
			}
		}
	}
}

void cRSCGen::OnFileSaveAs()
{
	// TODO: Add your command handler code here
	CFileDialog	Dlg( FALSE, "RSG", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Engine Resource File (*.RSG)|*.RSG||", NULL);

	if( Dlg.DoModal() != IDOK)	return;
	if( m_pRoot == NULL)		return;

	char szFileName[MAX_PATH];
	strcpy( szFileName, LPCTSTR( Dlg.GetPathName()));

	BuildRSCFile( szFileName);
	ExportRSCBin( szFileName);
}

void cRSCGen::OnFileSave()
{
	// TODO: Add your command handler code here
	if( m_szFileName[0] == 0)
	{
		OnFileSaveAs();
		return;
	}
	
	BuildRSCFile( m_szFileName);
	ExportRSCBin( m_szFileName);
}

void cRSCGen::_ReadRSCFile( i3FileStream * pStream)//const char * szRSCFileName)
{
	//m_PaneState.ClearLog();
	CreateRootScene();

	i3IniParser	Parser;

	if( !Parser.Open( pStream))
	{
		I3FATAL("Could not Open Stream", pStream->GetFileName());
		return;
	}

	strcpy( m_szFileName, pStream->GetFileName());

	INT32 i;
	INT32 nCount = Parser.GetSectionCount();
	for( i =0; i< nCount; ++i)
	{
		if( Parser.ReadSectionByIndex( i))
		{
			char szFileName[MAX_PATH];

			RSCNode * pNode = RSCNode::new_object_ref();
			RSC_DATA * pData = pNode->GetData();

			i3IniSectionInfo * pSection = Parser.GetSection(i);

			strcpy( pData->m_szFilePath, pSection->Name);

		//	i3String::SplitFileName( pData->m_szFilePath, szFileName, TRUE);
			i3::extract_filename(pData->m_szFilePath, szFileName);
			
			//2011-02-10 하동익//
			AddFileToMap(szFileName);
			////////////////////

			pNode->SetName( szFileName);

			CFieldPack * pPack = CFieldPack::new_object();
			pPack->Load( &Parser, pSection);

			pData->m_pField = pPack;

			for( INT32 j = 0; j < I3RSC_TYPE_COUNT; ++j)
			{
				if(i3::generic_is_iequal( g_RSCStyleDef[j].m_szFieldPackName, (char*)pPack->GetField(0)->m_pFieldData) )
				{
					pData->m_Attr = j;
					break;
				}
			}

			pNode->ScanAllNode( m_NodeList);

			_GetTypeRoot( pData->m_Attr & I3RSC_TYPE_MASK)->AddChild( pNode);

			I3NOTICE( "Added File: %s", pData->m_szFilePath);
		}
	}
	
	//m_PaneTree.UpdateAll();

	Parser.Close();
}

void cRSCGen::BuildRSCFile( const char * szRSCFileName)
{
	i3IniParser	Parser;
	
	if( !Parser.Create(szRSCFileName))
	{
		I3FATAL("Could not Open File", (char*)szRSCFileName);
		return;
	}

	strcpy( m_szFileName, szRSCFileName);

	_Rec_BuildRSCFile( &Parser, m_pRoot);

	Parser.Close();
}

void cRSCGen::_Rec_BuildRSCFile( i3IniParser * pParser, RSCNode * pNode)
{
	if( pNode == NULL)	return;

	CFieldPack * pField = pNode->GetData()->m_pField;

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

void cRSCGen::OpenRSCFile( const char * pszFileName)
{
	char ID[4];
	i3FileStream File;
	if( !File.Open(pszFileName, STREAM_READ))
	{
		I3FATAL("Couldn't Open File: %s", (char*)pszFileName);
		return;
	}

	// 2011-04-27 김재진
	if(!AutoWorkPath(pszFileName, true)) return;
	
	if( File.Read( ID, 4) != STREAM_ERR)
	{
		File.SetPosition( 0, STREAM_BEGIN);

		if( i3::generic_is_niequal( ID, "I3RF", 4) || 
			 i3::generic_is_niequal( ID, "I3R2", 4) )
		{
			_ReadRSCBin( &File);
		}
		else if( ID[0] == '[')
		{
			_ReadRSCFile( &File);
		}
	}

	File.Close();
}


static void _ExportRSC( const i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& List, char * pszFileName)
{
//	I3ASSERT( pList != NULL);
	I3ASSERT( pszFileName != NULL);
	
	i3FileStream FileStream;
	char szFileName[MAX_PATH];
	i3::change_file_ext_copy( szFileName, pszFileName, "RSC");

	UINT32	nResult = 0;
	UINT8	nID[4];
	INT32	nCount = (INT32)List.size();

	if( FileStream.Create( szFileName, STREAM_WRITE) == STREAM_ERR)
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
		RSCNode * pNode = List.get_vector()[i];
		nResult = pNode->Export( &FileStream);
	}

	return;
}

void cRSCGen::_Rec_AddExportList( RSCNode * pParent, i3::vector_multiset<RSCNode*, struct RSCNode_ExportComp>& ExportList)
{
	if( pParent == NULL)
		return;

	if( i3::same_of<RSCNode*>(pParent)) //->IsExactTypeOf( RSCNode::GetClassMeta()))
	{
		RSC_DATA * pData = pParent->GetData();
		if(( pData != NULL) && (pParent != m_pRoot))
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

INT32	GetPriority( UINT32 nType)
{
	switch( nType)
	{
		case  I3RSC_TYPE_BINARY:		return 0;
		case  I3RSC_TYPE_TEXTURE:		return 1;
		case  I3RSC_TYPE_ANIMPACK:		return 2;
		case  I3RSC_TYPE_ANIMATION:		return 3;
		case  I3RSC_TYPE_SOUND:			return 4;
		case  I3RSC_TYPE_SCENE:			return 5;
		case  I3RSC_TYPE_WORLD:			return 6;
		case  I3RSC_TYPE_CHARA:			return 7;
		case  I3RSC_TYPE_OBJECT:		return 8;
		case  I3RSC_TYPE_UI:			return 9;
	}

	return 0;
}

void cRSCGen::ExportRSCBin( char * pszFileName)
{
	if( pszFileName == NULL)
		return;

	if( m_pRoot == NULL)
		return;

//	BinList ExportList;
//	ExportList.SetCompareProc( (COMPAREPROC)_CompareProc);

	i3::vector_multiset<RSCNode*, RSCNode_ExportComp> ExportList;

	_Rec_AddExportList( m_pRoot, ExportList);

	_ExportRSC( ExportList, pszFileName);
}

void cRSCGen::_ReadRSCBin(i3FileStream * pStream)
{
	I3ASSERT( pStream != NULL);

	SetRoot( NULL);

	i3ResourceFile file;
	INT32 i;
	i3PersistantElement *	pObj;
	i3RSCGenProj		*	proj;

	if( file.Load( pStream) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 읽어 들일수 없습니다.");
		return;
	}

	proj = NULL;
	for( i =0; i < file.GetObjectCount(); ++i)
	{
		pObj = file.GetObject( i);

		if( i3::same_of<i3RSCGenProj*>(pObj)) //->IsExactTypeOf( i3RSCGenProj::GetClassMeta()))
		{
			proj = (i3RSCGenProj*)pObj;
		}
	}

	if( proj == NULL)
	{
		AfxMessageBox( "프로젝트를 읽어 들일수 없습니다.");
		return;
	}
	else
	{
		proj->AddRef();
	}

	{
		i3mem::FillZero( m_pTypeRoot, I3RSC_TYPE_COUNT * sizeof( RSCNode *));

		INT32 nStyle	= 0;
		INT32 i			= 0;


		INT32 nCount = proj->GetRoot()->GetChildCount();
		for( i = 0; i < nCount ; ++i)
		{
			RSCNode * pNode = proj->GetRoot()->GetChild( i);
			if( pNode->GetStyle() & I3RSC_ATTR_FOLDER)
			{
				nStyle = pNode->GetStyle() & I3RSC_TYPE_MASK;

				I3ASSERT( m_pTypeRoot[nStyle] == NULL);

				m_pTypeRoot[nStyle] = pNode;
			}
		}
	}

	SetRoot( proj->GetRoot());
	I3NOTICE( "Succeed  Load : %s",pStream->GetFileName());

	I3_SAFE_RELEASE( proj);
}

void cRSCGen::OnFileNew()
{
	// TODO: Add your command handler code here
	m_bFirst = true;
	m_bFile = false;
	CreateRootScene();
}

void cRSCGen::AddFileToMap(const char *name)
{
	//set<std::string> & FileList = m_PaneTree.GetFileNameList();

	//FileList.insert( name );

}

bool cRSCGen::CheckSameName(const char *name)
{
	//set< std::string >::iterator it = m_PaneTree.GetFileNameList().find(name);
// 	if( it != m_PaneTree.GetFileNameList().end())
// 		return false;
// 	else
 		return true;
}

// 2011-04-27 김재진
// Working Path 설정
bool cRSCGen::AutoWorkPath(const char* pszFileName, bool bPrsv)
{
	m_UpStageFile = false;

	if(pszFileName == NULL)	return false;

	int iPosition = 0;
	// 파일 경로를 임시저장 변수
	char TempPath[MAX_PATH];
	// 최종 경로 저장 변수
	char CopyPath[MAX_PATH];
	memset(&CopyPath, 0x00, sizeof(char) * MAX_PATH);
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
		memset(&m_szWork, 0x00, sizeof(m_szWork));
		i3::make_relative_path(CopyPath, TempPath, m_szWork);
	}
	else
	{
		i3::safe_string_copy(CopyPath, TempPath, MAX_PATH );
	}
	// 시작할때만 Working Folder 추가
	if( m_bFirst )
	{
		memset(&m_szWorkDir, 0x00, sizeof(m_szWorkDir));
		i3::safe_string_copy(m_szWorkDir, CopyPath, MAX_PATH );
		m_bFirst = false;
	}
	
	// 처음 등록된 위치와 동일하지 않을 경우 메시지 출력
	i3::make_relative_path(m_szWorkDir, TempPath, TempPath );

	if( i3::generic_is_niequal(m_szWork, TempPath, strlen(m_szWork)) == false )
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
bool cRSCGen::WorkingFolderYN(const char* pszNewFolder)
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