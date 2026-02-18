#include "i3CommonType.h"
#include "i3XMLFile.h"
#include "i3XML.h"
#include "i3NamedElement.h"
#include "i3ResourceObject.h"
#include "i3String.h"
#include "i3InstanceProxy.h"

#include "itl/vector_util.h"

I3_CLASS_INSTANCE( i3XMLFile, i3ElementBase);

i3XMLFile::i3XMLFile(void)
{
	m_pDoc			= NULL;
	m_pRoot			= NULL;
	m_pCur			= NULL;

	m_IDCount		= 0;
	m_pIDTable		= NULL;

	m_pKeyObject	= NULL;

	m_bOverwriteExternalRes = false;
	m_szFileName[0]	= 0;

	m_LastError		= i3Error::_NOERROR;

	i3mem::FillZero( &m_Header, sizeof( m_Header));
}

i3XMLFile::~i3XMLFile(void)
{
	I3_SAFE_DELETE( m_pDoc);

	I3MEM_SAFE_FREE( m_pIDTable);

}

void i3XMLFile::SetObjects( i3::vector<i3PersistantElement*>& ObjList)
{
	m_ObjList = ObjList;
}


void i3XMLFile::_setLastError( i3Error::CODE error, const char * pszMsg, bool bLog)
{
	m_LastError = error;

	if( bLog)
	{
		if( pszMsg == NULL)
		{
			I3ERRLOG( error);
		}
		else
		{
			I3ERRLOG_MSG( error, pszMsg);
		}
	}
}

i3PersistantElement *	i3XMLFile::FindObjectByID( OBJREF InstanceID)
{
	if( m_ObjList.size() <= 0)
		return NULL;

	UINT32 idx = (UINT32)InstanceID & 0x0000FFFF;

	I3ASSERT( (idx >= 1) && (idx <= m_ObjList.size()));

	return m_ObjList[ idx - 1];
}

i3PersistantElement *	i3XMLFile::FindObjectByMeta( i3ClassMeta * pMeta)
{
	i3PersistantElement * pObj;

	I3ASSERT( pMeta != NULL);

	for(size_t i = 0; i < m_ObjList.size(); i++)
	{
		pObj = m_ObjList[i];

		if( pObj->IsTypeOf( pMeta))
			return pObj;
	}

	return NULL;
}

OBJREF i3XMLFile::GetObjectPersistID( i3PersistantElement * pObj)
{
	INT32 idx;

	if( pObj == NULL)
		return 0;

	idx = i3::vu::index_of(m_ObjList, pObj);
	if (idx == INT32(m_ObjList.size()))
		idx = -1;

	idx = idx + 1;

	if( pObj->IsExternRes())
	{
		idx = 0xFFFF0000 | (0x0000FFFF & idx);
	}

	return (OBJREF) idx;
}

void i3XMLFile::_CreateXMLDocument( const char * pszPath)
{
	I3_SAFE_DELETE( m_pDoc);

	m_pDoc = new TiXmlDocument( pszPath);

	//	XML 포멧 선언 (한글이 포함되는 경우 반드시 euc-kr 설정할 것)
	TiXmlDeclaration decl("1.0", "euc-kr", "yes");

	m_pDoc->InsertEndChild( decl);
}

i3PersistantElement *	i3XMLFile::_ResolveExtern( i3XMLElement * pXML)
{
	char * pszName, szFull[ MAX_PATH], * pszClassName;
	i3ClassMeta * pMeta;
	i3PersistantElement * pObj;
	i3NamedResourceManager * pResManager;
	BOOL Rv;
	UINT32 Rc;
			
	// 7603 ms
	pszClassName = (char *) pXML->getAttr( "Class");
	I3ASSERT( pszClassName != NULL);

	pszName = (char *) pXML->getAttr( "ExternPath");
	if( pszName == NULL)
	{
		_setLastError( i3Error::_INVALIDFORMAT);
		return NULL;
	}

	pMeta = i3ClassMeta::FindClassMetaByName( pszClassName);

	pResManager = i3ResourceFile::GetResourceManager( pMeta);
	if( pResManager != NULL)
	{
		pObj = (i3PersistantElement *) pResManager->GetResource( m_szFileName, i3ResourceFile::GetWorkingFolder(), pszName);
	}
	else
	{
		i3ResourceFile file;
		i3FileStream stream;

		if( i3ResourceFile::GetWorkingFolder()[0] != 0)
			sprintf( szFull, "%s/%s", i3ResourceFile::GetWorkingFolder(), pszName);
		else
			strcpy( szFull, pszName);

		Rv = stream.Open( szFull, STREAM_READ | STREAM_SHAREREAD);
		if( Rv == FALSE)
		{
			I3FATAL( "Could not load %s external referenced file.", szFull);
			return NULL;
		}

		stream.AddRef();
		stream.AddRef();		// 절대 해제되지 않도록 하기 위한..
		file.SetStream( &stream);

		pObj = (i3PersistantElement *) CREATEINSTANCE( pMeta);

		Rc = pObj->OnLoad( &file);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not load %s external class object information", pObj->GetMeta()->GetClassName());
			return NULL;
		}

		if( pObj->IsTypeOf( i3NamedElement::GetClassMeta()))
		{
			((i3NamedElement *) pObj)->SetName( pszName);
		}

		file.SetStream( NULL);

		stream.Close();
	}


	if( pObj != NULL)
	{
		pObj->SetExternRes( TRUE);

		i3ResourceFile::CallExternRefCallback( (i3ResourceObject *) pObj, pszName);
	}

	return pObj;
}

UINT32 i3XMLFile::_SaveExtern( INT32 i, i3ResourceObject * pObj, i3XMLElement * pXML)
{
	i3FileStream stream;
	i3ClassMeta * pMeta;
	i3NamedResourceManager * pResManager;
	char * pszName, szFull[ MAX_PATH], * pszClassName;
	char * pszWorkDir = i3ResourceFile::GetWorkingFolder();
	BOOL Rv;
	UINT32 Rc = STREAM_ERR;

	pszClassName = pObj->GetMeta()->GetPersistClassName();
	pMeta = i3ClassMeta::FindClassMetaByName( pszClassName);
	I3ASSERT( pMeta != NULL);

	pszName = (char *) pObj->GetName();
	I3ASSERT( pszName != NULL);

	pResManager = i3ResourceFile::GetResourceManager( pMeta);
	if( pResManager != NULL)
	{
		Rc = pResManager->SaveResource( pszWorkDir, pObj); 
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "");
			return STREAM_ERR;
		}
	}
	else
	{
		if( pszWorkDir[0] != 0)
		{
			sprintf( szFull, "%s/%s", pszWorkDir, pszName);
		}
		else
		{
			strcpy( szFull, pszName);
		}

		i3ResourceFile file;

		file.setKeyObject( pObj);
	
		Rv = file.Save( szFull);
	}

	return Rc;
}


bool i3XMLFile::Load( const char * pszPath)
{
	i3XMLElement * pRoot;
	i3XMLElement * pHeader;
	i3XMLElement * pXML;
	bool bResult = false;
	INT32 temp;
	INT32 i;
	i3PersistantElement * pObj;
	char szName[256];
	UINT32 id;

	m_pDoc = NULL;

	m_LastError		= i3Error::_NOERROR;

	// Pack 파일에서도 읽을 수 있도록 수정.. swoongoo. 2011.5.27
	i3FileStream file;

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		m_LastError		= i3Error::_OPENFAIL;
		return false;
	}

	// Document의 생성
	{
		//m_pDoc = new TiXmlDocument( pszPath);
		m_pDoc = new TiXmlDocument();

		if( m_pDoc->LoadFile( &file) == false)
		{
			_setLastError( i3Error::_READFAIL);
			goto ExitPart;
		}
	}

	// 첫번째 Node가 Root Node이다.
	{
		pRoot = (i3XMLElement *) m_pDoc->LastChild();
		if( pRoot == NULL)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}
		
		if( i3String::Compare( pRoot->Value(), "i3EngineResourceFile") != 0)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}
	}

	// Header
	{
		pHeader = (i3XMLElement *) pRoot->FirstChild( "Header");
		if( pHeader == NULL)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}

		pHeader->getAttr( "MajorVersion", & m_Header.m_MajorVersion);
		pHeader->getAttr( "MinorVersion", & m_Header.m_MinorVersion);
		pHeader->getAttr( "ObjectCount", & m_Header.m_ObjectCount);
		pHeader->getAttr( "KeyObject", & temp);
		m_Header.m_pKeyObject = (OBJREF64) temp;
	}

	// Object Creation
	// 올바른 Loading을 위해 우선 해당 Class의 Object를 생성하여 List를 구축한다.
	{
//		m_ObjList.SetCount( m_Header.m_ObjectCount);
		m_ObjList.resize(m_Header.m_ObjectCount);

		for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
		{
			sprintf( szName, "Object%d", i);

			i3XMLElement * pXML = (i3XMLElement *) pRoot->FirstChild( szName);
			if( pXML == NULL)
			{
				_setLastError( i3Error::_INVALIDFORMAT);
				goto ExitPart;
			}

			const char * pszMetaName = pXML->getAttr( "Class");
			I3ASSERT( pszMetaName != NULL);

			pXML->getAttr( "ID", &id);

			if( (id & 0xFFFF0000) == 0xFFFF0000)
			{
				// External File
				pObj = _ResolveExtern( pXML);

				m_ObjList[i] = pObj;
			}
			else
			{
				i3ClassMeta * pMeta = i3ClassMeta::FindClassMetaByName( pszMetaName);
				
				if( pMeta != NULL)
				{
					pObj = (i3PersistantElement *) CREATEINSTANCE( pMeta);

					m_ObjList[i] = pObj;
				}
				else
				{
					_setLastError( i3Error::_UNKNOWNCLASSMETA, pszMetaName);
					I3TRACE( "Invalid Class Meta Name : %s\n", pszMetaName);
					m_ObjList[i] = NULL;
				}
			}
		}
	}

	// 다시 돌면서 실질적인 Object의 Loading을 처리
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		sprintf( szName, "Object%d", i);

		pXML = (i3XMLElement *) pRoot->FirstChild( szName);
		if( pXML == NULL)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}

		pObj = m_ObjList[i];

		pXML->getAttr( "ID", &id);

		if((id & 0xFFFF0000) == 0xFFFF0000)
		{
			// External File
		}
		else
		{
			if( pObj == NULL)
				continue;

			if( pObj->IsLoaded())
				continue;

			pushCurrentObject( pObj);

			if( pObj->OnLoadXML( this, pXML) == false)
			{
				_setLastError( i3Error::_READFAIL);
				goto ExitPart;
			}

			popCurrentObject();

			if( pObj->IsExactTypeOf( i3InstanceProxy::GetClassMeta()))
			{
				i3InstanceProxy * pInstProxy = (i3InstanceProxy *) pObj;

				// Instance Proxy Class가 생성한 Instance로 대체...
				m_ObjList[i] = pInstProxy->getInstance();

				// Instance Proxy는 제거..
				pObj->AddRef();
				pObj->Release();
			}
		}

		if( pObj != NULL)
			pObj->SetLoaded( TRUE);
	}

	if( m_Header.m_pKeyObject != 0)
	{
		i3PersistantElement * pKeyObj = (i3PersistantElement *) FindObjectByID( (OBJREF) m_Header.m_pKeyObject);
		I3ASSERT( pKeyObj != NULL);

		setKeyObject( pKeyObj);
	}
	
	for( i = 0; i < INT32(m_ObjList.size()); i++)
	{
		pObj = m_ObjList[i];

		if( pObj != NULL)
			pObj->OnInitAfterLoad();
		else
		{
//			m_ObjList.Delete( i);
			m_ObjList.erase( m_ObjList.begin() + i);
			i--;
		}
	}

	bResult = true;

ExitPart:

	if( m_pDoc != NULL)
	{
		delete m_pDoc;
		m_pDoc = NULL;
	}

	file.Close();

	return bResult;
}

bool i3XMLFile::Save( const char * pszPath)
{

	i3PersistantElement * pObj;
	BOOL bOldState = FALSE;
	i3XMLElement * pHeader = NULL;
	i3XMLElement * pRoot;
	char szName[MAX_PATH];
	bool bResult = false;

	m_LastError		= i3Error::_NOERROR;

//	m_ObjList.SetOnceMode();

	{
//		i3List TempList;
//		TempList.AddFromList( &m_ObjList);
		i3::vector<i3PersistantElement*> TempList(m_ObjList.begin(), m_ObjList.end());

		if( m_pKeyObject != NULL)
		{
//			if( TempList.IndexOf( m_pKeyObject) == -1)
//				TempList.Add( m_pKeyObject);
			if ( i3::find(TempList.begin(), TempList.end(), m_pKeyObject) == TempList.end() )
				TempList.push_back(m_pKeyObject);

			bOldState = m_pKeyObject->IsExternRes();
			m_pKeyObject->SetExternRes( FALSE);
		}

		m_ObjList.clear();

		for(size_t i = 0; i < TempList.size(); i++)
		{
			pObj = TempList[i];

			pObj->OnBuildObjectList( m_ObjList);
		}

		// 중복 오브젝트 후처리 검사..
		i3::vu::remove_duplicates(m_ObjList);
		//	
	}

	I3ASSERT( m_ObjList.size() > 0);

	_CreateXMLDocument( pszPath);

	{
		pRoot = new i3XMLElement( "i3EngineResourceFile");

		m_pDoc->LinkEndChild( pRoot);
	}

	// header information
	{
		pHeader = new i3XMLElement( "Header");

		pHeader->SetAttribute( "MajorVersion", 2);
		pHeader->SetAttribute( "MinorVersion", 0);
		pHeader->SetAttribute( "ObjectCount",	INT32(m_ObjList.size()));
		pHeader->SetAttribute( "KeyObject",		(INT32) GetObjectPersistID( getKeyObject()));
		
		pRoot->LinkEndChild( pHeader);
	}

	// Objects
	const size_t obj_size = m_ObjList.size();

	for(size_t i = 0; i < obj_size; i++)
	{
		i3ClassMeta * pMeta;
		i3XMLElement * pXML;

		pObj = m_ObjList[i];

		pMeta = pObj->GetMeta();

		sprintf( szName, "Object%d", i);

		pXML = new i3XMLElement( szName);

		pXML->SetAttribute( "Class", pMeta->GetClassName());
		pXML->SetAttribute( "ID", (INT32) GetObjectPersistID( pObj));

		pRoot->LinkEndChild( pXML);

		pushCurrentObject( pObj);

		if( pObj->IsExternRes())
		{
			I3ASSERT( pObj->IsTypeOf( i3ResourceObject::GetClassMeta()));

			// External Reference
			if( m_bOverwriteExternalRes )
			{
				if( _SaveExtern( i, (i3ResourceObject *) pObj, pXML) == STREAM_ERR)
				{
					I3FATAL( "_SaveExtern() failed");
				}
			}

			pXML->addAttr( "ExternPath", ((i3ResourceObject *)pObj)->GetName());
		}
		else
		{
			if( pObj->OnSaveXML( this, pXML) == false)
			{
				I3FATAL( "Could not write %s class object.", pMeta->GetClassName());				
				goto ExitPart;
			}
		}

		popCurrentObject();
	}

	bResult = m_pDoc->SaveFile();

	delete m_pDoc;
	m_pDoc = NULL;

ExitPart:

	if( m_pKeyObject != NULL)
	{
		m_pKeyObject->SetExternRes( bOldState);
	}

	return bResult;
}

