// i3PackFileEditorDoc.cpp : Ci3PackFileEditorDoc 클래스의 구현
//

#include "stdafx.h"
#include "i3PackFileEditor.h"

#include "i3PackFileEditorDoc.h"
#include ".\i3packfileeditordoc.h"
#include "GlobalVariable.h"
#include "PackNode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ci3PackFileEditorDoc

IMPLEMENT_DYNCREATE(Ci3PackFileEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(Ci3PackFileEditorDoc, CDocument)
	ON_COMMAND(ID_TOOL_COMPILE, OnToolCompile)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
	ON_COMMAND(ID_TOOL_EXTRACTI3P, OnToolExtracti3p)
	ON_COMMAND(ID_FILE_IMPORT_FOR_PSP, OnFileImportForPsp)
END_MESSAGE_MAP()


// Ci3PackFileEditorDoc 생성/소멸

Ci3PackFileEditorDoc::Ci3PackFileEditorDoc()
{
	m_pPack = i3PackFile::NewObject();
}

Ci3PackFileEditorDoc::~Ci3PackFileEditorDoc()
{
	I3_SAFE_RELEASE( m_pPack);
}

BOOL Ci3PackFileEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	I3_SAFE_RELEASE( m_pPack);
	m_pPack = i3PackFile::NewObject();

	return TRUE;
}


// Ci3PackFileEditorDoc serialization

void Ci3PackFileEditorDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// Ci3PackFileEditorDoc 진단

#ifdef _DEBUG
void Ci3PackFileEditorDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void Ci3PackFileEditorDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Ci3PackFileEditorDoc 명령

BOOL Ci3PackFileEditorDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	BOOL bOpend = FALSE;

	char szExt[128];
	i3String::SplitFileExt( lpszPathName, szExt, 128 );
	if( stricmp( szExt, "open_log" ) == 0 )
	{
        bOpend = ImportFileList( lpszPathName );
	}
	else
	{
		bOpend = LoadProject( lpszPathName );
	}

	if( bOpend && g_MyCmdInfo.m_bCompile )
	{
		OnToolCompile();
		if( g_MyCmdInfo.m_bCompileAndExit )
		{
			return FALSE;
		}
	}

	return bOpend;
}

BOOL Ci3PackFileEditorDoc::LoadProject( const char *szFileName )
{
	i3ResourceFile file;
	UINT32 Rc;
	INT32 i;
	i3PackFile * pVol = NULL;

	Rc = file.Load( szFileName );
	if( Rc == STREAM_ERR)
	{
		EventLog( szFileName, "파일을 열 수 없습니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return FALSE;
	}

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		i3PersistantElement * pObj = file.GetObject( i);

		if( pObj->IsExactTypeOf( i3PackFile::GetClassMeta()))
		{
			pVol = (i3PackFile *) pObj;
			break;
		}
	}

	if( pVol == NULL)
	{
		EventLog( szFileName, "올바른 i3Engine Volume 파일이 아닙니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return FALSE;
	}

	I3_SAFE_RELEASE( m_pPack);
	m_pPack = pVol;
	m_pPack->AddRef();

	{
		i3PackNode * pRoot = m_pPack->GetRoot();
		i3List List;

		pRoot->GetAllNodeByAttr( &List, I3_VOLATTR_DIRECTORY, 0, true);

		EventLog( szFileName, "Loading 완료 (Total %d files)", List.GetCount());
	}

	Update( NULL, I3_TDK_UPDATE_ADD, m_pPack);

	return TRUE;
}

BOOL Ci3PackFileEditorDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	i3ResourceFile file;
	UINT32 Rc;
	i3List List;

	List.Add( m_pPack);

	file.SetObjects( &List);

	Rc = file.Save( lpszPathName);
	if( Rc == STREAM_ERR)
	{
		EventLog( lpszPathName, "파일을 저장할 수 없습니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return FALSE;
	}

	return TRUE;
}

BOOL IsFileExist( const char *szPath )
{
	FILE *fp = fopen( szPath, "rb" );
	if( fp == NULL )
	{
		return FALSE;
	}
	fclose( fp );
	return TRUE;
}

void Ci3PackFileEditorDoc::AddFile( char * pszPath)
{
	char szRelPath[MAX_PATH], * pszToken;
	i3StringList TokenList;
	INT32 i;
	PackNode * pCur, * pNew = NULL;
	bool bLast = false, bAdd;

	if((pszPath[1] == ':') && (pszPath[2] == '\\'))
	{
		// 절대경로다.
		i3String::MakeRelativePath( g_pProfile->m_szWorkDir, pszPath, szRelPath);
	}
	else
	{
		strcpy( szRelPath, pszPath);
	}

	char szExt[128];
	char szAbsFileName[MAX_PATH];
	char szTemp[MAX_PATH];
	sprintf( szAbsFileName, "%s\\%s", g_pProfile->m_szWorkDir, szRelPath );
	i3String::SplitFileExt( szAbsFileName, szExt, 128 );

	// 파일 존재 확인
	if( ! IsFileExist( szAbsFileName ) )
	{
		CString msg;
		msg.Format( "Could not find file. \"%s\"", szAbsFileName );
		AfxMessageBox( LPCTSTR(msg), MB_OK );
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	if( g_MyCmdInfo.m_bNoSound )
	{
        if( ( stricmp( szExt, "wav" ) == 0 ) ||
			( stricmp( szExt, "vag" ) == 0 ) ||
			( stricmp( szExt, "at3" ) == 0 ) )
		{
			return;
		}
	}

	if( g_MyCmdInfo.m_bLoadPSPFile )
	{
		if( stricmp( szExt, "wav" ) == 0 )
		{
			i3String::SwapExt( szTemp, szAbsFileName, "vag" );
			if( IsFileExist( szTemp ) )
			{
				i3String::SwapExt( szRelPath, szRelPath, "vag" );
			}
			else
			{
				i3String::SwapExt( szTemp, szAbsFileName, "at3" );
				if( IsFileExist( szTemp ) )
				{
					if( g_MyCmdInfo.m_bNoBGM )
					{
						// 스트리밍으로 처리하는 음악 파일이므로 스킵
						return;
					}
					else
					{
						i3String::SwapExt( szRelPath, szRelPath, "at3" );
					}
				}
				else
				{
					CString msg;
					msg.Format( "Could not find proper PSP file. \"%s\"", szRelPath );
					AfxMessageBox( LPCTSTR(msg), MB_OK );
					g_MyCmdInfo.m_bCompileAndExit = FALSE;
					return;
				}
			}
		}
		else
		{
			i3String::AddPostfix( szTemp, szAbsFileName, "_PSP" );
			if( IsFileExist( szTemp ) )
			{
				i3String::AddPostfix( szRelPath, szRelPath, "_PSP" );
			}
		}
	}

	i3String::ResolvePathToken( szRelPath, &TokenList);

	pCur = (PackNode *) m_pPack->GetRoot();

	for( i = 0; i < TokenList.GetCount(); i++)
	{
		pszToken = TokenList.Items[i];

		if( i == TokenList.GetCount() - 1)
			bLast = true;

		bAdd = false;

		if( pCur == NULL)
		{
			// Root도 없는 완전히 빈 상태
			bAdd = true;
		}
		else
		{
			pNew = (PackNode *) pCur->FindChildByName( pszToken);

			if( pNew == NULL)
				bAdd = true;				
		}

		if( bAdd )
		{
			pNew = PackNode::NewObjectRef();
			
			pNew->SetName( pszToken);
			if( bLast == false)
				pNew->SetAttribute( I3_VOLATTR_DIRECTORY);
			else
				pNew->SetPath( szRelPath);

			pCur->AddChild( pNew);

			EventLog( szRelPath, "파일 추가");
	
			Update( NULL, I3_TDK_UPDATE_ADD, pNew);
		}

		pCur = (PackNode *) pNew;
	}
}

void Ci3PackFileEditorDoc::RemoveFile( PackNode * pNode)
{
	PackNode * pParent;

	pParent = (PackNode *) pNode->GetParent();

	// i3PackFile에는 항상 임의의 Root가 있기 때문에
	// pParent가 NULL인 경우는 Root 밖에 없다.
	// Root를 제거하는 경우가 발생해서는 안된다.
	I3ASSERT( pParent != NULL);

	{
		Update( NULL, I3_TDK_UPDATE_REMOVE, pNode);

		pParent->RemoveChild( pNode);
	}
}

void Ci3PackFileEditorDoc::OnCompileI3S( i3PackNode * pNode, char * pszPath)
{
	i3SceneFile file;
	UINT32 Rc;
	INT32 i;
	i3PersistantElement * pObj;

	Rc = file.Load( pszPath);
	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "파일을 읽어 들일 수 없습니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	pNode->SetType( I3_PACK_NODE_SCENEFILE);

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pObj = file.GetObject( i);

		pObj->SetExternRes( FALSE);

		if( pObj->IsTypeOf( i3Texture::GetClassMeta()))
		{
			OnProcessImage( pNode, (i3Texture *) pObj);
		}

		pNode->AddObject( pObj);
	}

	// Scene Root를 설정해 둔다.
	pNode->SetRepresentObject( file.GetSceneGraph());
}

void Ci3PackFileEditorDoc::OnCompileI3A( i3PackNode * pNode, char * pszPath)
{
	i3ResourceFile file;
	UINT32 Rc;
	INT32 i;
	i3PersistantElement * pObj;
	i3Animation * pAnim = NULL;

	Rc = file.Load( pszPath);

	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "올바른 Animation 파일이 아닙니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	pNode->SetType( I3_PACK_NODE_RESOURCEFILE);

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pObj = file.GetObject( i);

		if( pObj->IsTypeOf( i3Animation::GetClassMeta()))
		{
			pAnim = (i3Animation *) pObj;
		}

		pObj->SetExternRes( FALSE);

		pNode->AddObject( pObj);
	}

	if( pAnim == NULL)
	{
		EventLog( pszPath, "올바른 Animation 파일이 아닙니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	// i3Animation Pointer를 설정한다.
	pNode->SetRepresentObject( pAnim);
}

void Ci3PackFileEditorDoc::OnCompileImage( i3PackNode * pNode, char * pszPath)
{
	i3Texture * pTex;
	i3ImageFile file;

	pTex = file.Load( (const char *) pszPath);
	if( pTex == NULL)
	{
		EventLog( pszPath, "올바른 Image 파일이 아닙니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	pTex->SetExternRes( FALSE);

	OnProcessImage( pNode, pTex);

	pNode->SetType( I3_PACK_NODE_IMAGE);
	pNode->SetRepresentObject( pTex);
}

void Ci3PackFileEditorDoc::OnCompileSound( i3PackNode * pNode, char * pszPath)
{
	//i3Sound * pSound;
	//i3ImageFile file;

	//pSound = new i3Sound; 
	//if( ! pSound->Load( (const char *) pszPath) )
	//{
	//	EventLog( pszPath, "올바른 Sound 파일이 아닙니다.");
	//	return;
	//}

	//pSound->SetExternRes( FALSE );

	//pNode->SetType( I3_PACK_NODE_SOUND );
	//pNode->SetRepresentObject( pSound );
}

void Ci3PackFileEditorDoc::OnCompileBinary( i3PackNode * pNode, char * pszPath)
{
	i3FileStream stream;
	BOOL Rv;
	i3MemoryBuffer * pBuffer;
	void * pData;
	UINT32 Rc;

	Rv = stream.Open( pszPath, STREAM_READ);
	if( Rv == FALSE)
	{
		EventLog( pszPath, "파일을 열 수 없습니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	pBuffer = i3MemoryBuffer::NewObject();
	pBuffer->Create( stream.GetSize());

	pData = pBuffer->Lock();

	Rc = stream.Read( pData, stream.GetSize());

	pBuffer->Unlock();

	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "파일을 읽을 수 없습니다.");
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		pBuffer->Release();
		return;
	}

	pNode->SetRepresentObject( pBuffer);
	pBuffer->Release();
}

void Ci3PackFileEditorDoc::Rec_BuildPack( PackNode * pSrc, i3PackNode * pDestParent)
{
	i3PackNode * pNew;
	PackNode * pChild;
	char szExt[MAX_PATH], szFull[MAX_PATH];

	pChild = (PackNode *) pSrc->GetFirstChild();

	while( pChild != NULL)
	{
		// 새로운 Node를 생성한다.
		pNew = i3PackNode::NewObjectRef();
		pDestParent->AddChild( pNew);

		pChild->CopyTo( pNew, I3_COPY_INSTANCE);

		I3ASSERT( strlen( pChild->GetName()) > 0);

		i3String::SplitFileExt( pChild->GetPath(), szExt, sizeof(szExt) - 1);

		if( pChild->IsDirectory() == FALSE)
		{
			sprintf( szFull, "%s\\%s", g_pProfile->m_szWorkDir, pChild->GetPath());

			if( i3String::Compare( szExt, "I3S") == 0)
			{
				// Scene File
				OnCompileI3S( pNew, szFull);

				m_I3SCount++;
			}
			else if( i3String::Compare( szExt, "I3A") == 0)
			{
				// Animation file
				OnCompileI3A( pNew, szFull);

				m_I3ACount++;
			}
			else if((i3String::Compare( szExt, "I3I") == 0) ||
					(i3String::Compare( szExt, "TGA") == 0))
			{
				// Image File
				OnCompileImage( pNew, szFull);

				m_ImageCount++;
			}
			else 
			{
				// Unknown Format (Binary Copy)
				OnCompileBinary(  pNew, szFull);

				m_BinaryCount++;
			}
		}
		
		m_TotalFileCount++;

		Rec_BuildPack( pChild, pNew);

		pChild = (PackNode *) pChild->GetNext();
	}
}

void Ci3PackFileEditorDoc::BuildPack( const char * pszPath)
{
	i3PackNode * pNewRoot;
	i3PackFile * pPackFile;

	m_TotalFileCount = m_I3SCount = m_I3ACount = m_ImageCount = m_BinaryCount = 0;

	EventLog( pszPath, "Compile 시작");

	// Pack File 생성
	{
		pPackFile = i3PackFile::NewObject();
	}

	// 복사본 생성하기
	pNewRoot = i3PackNode::NewObject();

	// Named Resource Manager 등록
	{
		i3NamedResourceManager * pResManager = NULL;
		char szFull[MAX_PATH];

		sprintf( szFull, "%s/%s", g_pProfile->m_szWorkDir, g_pProfile->m_szTexDir);

		pResManager = i3NamedResourceManager::NewObject();
		pResManager->SetRelativePath( szFull);
		i3ResourceFile::AddResourceManager( i3Texture::GetClassMeta(), pResManager);
	}

	Rec_BuildPack( (PackNode *) m_pPack->GetRoot(), pNewRoot);

	// 저장하기
	{
		i3ResourceFile file;
		i3List List;
		UINT32 Rc;

		pPackFile->SetRoot( pNewRoot);

		List.Add( pPackFile);
		file.SetObjects( &List);

		Rc = file.Save( pszPath);
		if( Rc == STREAM_ERR)
		{
			EventLog( pszPath, "파일을 생성할 수 없습니다.");
			g_MyCmdInfo.m_bCompileAndExit = FALSE;
		}
	}

	EventLog( pszPath, "Compile 완료 (Total:%d , Processed file:%d)", m_TotalFileCount, 
					m_I3ACount + m_I3SCount + m_ImageCount + m_BinaryCount);
	EventLog( "-", "I3S file : %d", m_I3SCount);
	EventLog( "-", "I3A file : %d", m_I3ACount);
	EventLog( "-", "Image file : %d", m_ImageCount);
	EventLog( "-", "Unknown file : %d", m_BinaryCount);

	pNewRoot->Release();
	pPackFile->Release();
}
 
void Ci3PackFileEditorDoc::OnToolCompile() 
{
	char szFull[MAX_PATH], szName[MAX_PATH];
	CString name;

	name = GetTitle();

	strcpy( szFull, LPCTSTR( name));
	i3String::SplitFileName( szFull, szName, FALSE);
	sprintf( szFull, "%s/%s.i3p", g_pProfile->m_szOutDir, szName);

	if( g_MyCmdInfo.m_bCompile && ! g_MyCmdInfo.m_bForceOverwrite )
	{
		if( IsFileExist( szFull ) )
		{
			return;
		}
	}

	ClearEventLog(); 

	BuildPack( (const char *) szFull);

	if( ! g_MyCmdInfo.m_bCompileAndExit )
	{
		AfxMessageBox( "Compile 완료", MB_ICONASTERISK | MB_OK); 
	}
}

void Ci3PackFileEditorDoc::OnFileImport()
{
	CFileDialog	Dlg( TRUE, "OPEN_LOG", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Opened File List(*.open_log)|*.OPEN_LOG;*.TXT||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	g_MyCmdInfo.m_bLoadPSPFile = FALSE;

	if( ! ImportFileList( LPCTSTR( Dlg.GetPathName() ) ) )
	{
		AfxMessageBox( "Failed" );
	}
}

void Ci3PackFileEditorDoc::OnFileImportForPsp()
{
	CFileDialog	Dlg( TRUE, "OPEN_LOG", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"Opened File List(*.open_log)|*.OPEN_LOG;*.TXT||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	g_MyCmdInfo.m_bLoadPSPFile = TRUE;

	if( ! ImportFileList( LPCTSTR( Dlg.GetPathName() ) ) )
	{
		AfxMessageBox( "Failed" );
	}
}

BOOL Ci3PackFileEditorDoc::ImportFileList( const char *szFileName )
{
	{
		BOOL Rv;
		char szLine[512];
		i3FileStream Stream;

		Rv = Stream.Open( szFileName, STREAM_READ );
		if( Rv == FALSE)
		{
			return FALSE;
		}

		while( Stream.ReadLine( szLine, sizeof(szLine) - 1) > 0)
		{
			i3String::RemoveBlankFromHead( szLine);
			i3String::RemoveBlankFromTail( szLine);

			if( strlen( szLine) > 0)
			{
				AddFile( szLine);
			}
		}

		Stream.Close();
	}

	CString name;
	name = GetTitle();
	if( name.CompareNoCase( "" ) == 0 || name.CompareNoCase( "Untitled" ) == 0 )
	{
		char szTemp[MAX_PATH];
		strcpy( szTemp, szFileName );
		i3String::RemoveExt( szTemp );
		//if( g_MyCmdInfo.m_bLoadPSPFile )
		//{
		//	i3String::Concat( szTemp, "_PSP" );
		//}
		SetTitle( szTemp ); 
	}

	return TRUE;
}

void Ci3PackFileEditorDoc::OnProcessImage( i3PackNode * pNode, i3Texture * pTex)
{
	if( (pTex->GetPersistFormat() & I3G_IMAGE_FORMAT_MASK_INDEXED) == 0)
	{
		EventLog( pNode->GetName(), "Heavy Texture : [%s] - %d bits image", pTex->GetName(), i3Gfx::GetImageFormatBitCount( pTex->GetPersistFormat()));
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
	}

	i3OptPSPSwizzledImage opt;

	opt.MakeSwizzledImage( pTex);
}

static void _SaveI3S( char * pszPath, i3PackNode * pNode)
{
	i3SceneFile file;
	UINT32 Rc;

	file.SetSceneGraph( (i3Node *) pNode->GetRepresentObject());

	Rc = file.Save( pszPath);
	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "%s i3Scene 파일을 저장할 수 없습니다.", pszPath);
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
	}
}

static void _SaveResource( char * pszPath, i3PackNode * pNode)
{
	i3ResourceFile file;
	UINT32 Rc;

	file.SetObjects( pNode->GetObjectList());

	Rc = file.Save( pszPath);
	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "%s Resource 파일을 저장할 수 없습니다.", pszPath);
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
	}
}

static void _SaveImage( char * pszPath, i3PackNode * pNode)
{
	i3ImageFile file;
	i3Texture * pTex;
	UINT32 Rc;

	pTex = (i3Texture *) pNode->GetRepresentObject();
	I3ASSERT( pTex != NULL);

	Rc = file.SaveI3I( pszPath, pTex);
	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "%s Image 파일을 저장할 수 없습니다.", pszPath);
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
	}
}

static void _SaveBinary( char * pszPath, i3PackNode * pNode)
{
	i3FileStream Stream;
	BOOL Rv;
	UINT32 Rc;
	i3MemoryBuffer * pBuf;
	char * pData;

	pBuf = (i3MemoryBuffer *) pNode->GetRepresentObject();
	I3ASSERT( pBuf != NULL);

	Rv = Stream.Create( pszPath, STREAM_WRITE);
	if( Rv == FALSE)
	{
		EventLog( pszPath, "%s 파일을 생성할 수 없습니다.", pszPath);
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		return;
	}

	pData = (char *) pBuf->Lock();

	Rc = Stream.Write( pData, pBuf->GetSize());
	if( Rc == STREAM_ERR)
	{
		EventLog( pszPath, "%s 파일의 내용을 기록할 수 없습니다.", pszPath);
		g_MyCmdInfo.m_bCompileAndExit = FALSE;
		pBuf->Unlock();
		return;
	}

	Stream.Close();
	pBuf->Unlock();
}

static BOOL _ExtractProc( i3PackNode * pNode, char * pszBase)
{
	if( pNode->IsAttribute( I3_VOLATTR_DIRECTORY))
		return TRUE;

	if( pNode->GetName() == NULL)
		return TRUE;

	// Directory는 무시한다.
	char szPath[MAX_PATH], szFull[MAX_PATH], * pszName, szName[MAX_PATH];

	pNode->GetFullPath( szPath);

	{
		pszName = i3String::SplitFileName( szPath, szName, TRUE);
		*pszName = 0;
		i3String::NormalizePath( szPath);

		// Directory 준비
		sprintf( szFull, "%s/%s", pszBase, szPath);
		i3System::PrepareDirectory( szFull);

		sprintf( szFull, "%s/%s/%s", pszBase, szPath, szName);

		switch( pNode->GetType())
		{
			case I3_PACK_NODE_SCENEFILE :		_SaveI3S( szFull, pNode);	break;
			case I3_PACK_NODE_RESOURCEFILE :	_SaveResource( szFull, pNode);	break;
			case I3_PACK_NODE_IMAGE :			_SaveImage( szFull, pNode);		break;
			//case I3_PACK_NODE_SOUND :			I3ASSERT_0;	break;// TODO
			default :							_SaveBinary( szFull, pNode);	break;
		}
	}

	return TRUE;
}

void Ci3PackFileEditorDoc::OnToolExtracti3p()
{
	CFileDialog	Dlg( TRUE, "i3p", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"i3Pack Files(*.I3P)|*.i3p;*.TXT||", NULL);

	if( Dlg.DoModal() != IDOK)	return;

	i3Win32FolderSelectDialog FolderDlg;

	if( FolderDlg.Execute( NULL, "Extract할 Folder 선택") == FALSE)
		return;

	{
		i3ResourceFile file;
		UINT32 Rc;
		i3PackFile * pFile;

		Rc = file.Load( LPCTSTR( Dlg.GetPathName()));
		if( Rc == STREAM_ERR)
		{
			DebugBox( NULL, "%s 파일을 읽을 수 없습니다.", LPCTSTR( Dlg.GetPathName()));
			return;
		}

		pFile = (i3PackFile *) file.FindObjectByMeta( i3PackFile::GetClassMeta());
		if( pFile == NULL)
		{
			DebugBox( NULL, "%s 파일은 올바른 i3Pack 파일이 아닙니다.", LPCTSTR( Dlg.GetPathName()));
			return;
		}

		pFile->Trace( (I3PACKTRACEPROC) _ExtractProc, FolderDlg.GetSelectedFolderPath());
	}
}

