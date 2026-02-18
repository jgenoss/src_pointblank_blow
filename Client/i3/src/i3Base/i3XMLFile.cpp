#include "i3CommonType.h"
#include "i3XMLFile.h"
#include "i3XML.h"
#include "i3NamedElement.h"
#include "i3ResourceObject.h"
#include "i3String.h"
#include "i3InstanceProxy.h"

#include "itl/vector_util.h"

#include "i3Base/string/compare/generic_is_iequal.h"

I3_CLASS_INSTANCE( i3XMLFile);

//I3_CLASS_INSTANCE( i3XMLFile, i3ElementBase);

i3XMLFile::i3XMLFile(void)
{
	i3mem::FillZero( &m_Header, sizeof( m_Header));
}

i3XMLFile::~i3XMLFile(void)
{
	// 레퍼런스카운트가 1인 오브젝트는 활용이 안되는 것으로 간주하고, TRACE보고후 삭제를 하는게 좋을지도 모른다.
	// 이부분은 툴과 게임실행까지 완벽하게 진행되면 코드를 확정...(2012.09.11.수빈)
	const size_t num_obj = m_ObjList.size();	
	for (size_t i = 0 ; i < num_obj ; ++i)
	{
		i3PersistantElement* obj = m_ObjList[i];
		if (!obj)
			continue;

		if ( obj->GetRefCount() == 1 )		// 카운팅도 되지 않고, 활용되지 않은 리소스..
		{
#if defined(I3_DEBUG)		// 디버그 모드에 트레이스를 보고한다..(추정으로는 메모리릭의 원인이 될수도 있었는데, 일단 제거함수 추가)
			if ( i3::kind_of<i3NamedElement*>(obj) )
			{
				i3NamedElement* ne = static_cast<i3NamedElement*>(obj);
				if (ne->GetNameString().empty() == false)
					I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT NAME = %s, OBJECT META = %s\n", 
						this->getFileName(), i, num_obj, ne->GetName(), ne->meta()->class_name().c_str() );
				else
					I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT META = %s\n", 
						this->getFileName(), i, num_obj, obj->meta()->class_name().c_str() );
			}
			else
			{
				I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT META = %s\n", 
					this->getFileName(), i, num_obj, obj->meta()->class_name().c_str() );
			}
#endif
			obj->destroy_instance();
		}
		else
		{
			I3_MUST_RELEASE( obj);
		}
	}

	I3_SAFE_DELETE( m_pDoc);
	I3MEM_SAFE_FREE( m_pIDTable);
}

void i3XMLFile::SetObjects( i3::vector<i3PersistantElement*>& ObjList)
{
	i3::for_each(ObjList.begin(), ObjList.end(), i3::mem_fun(&i3PersistantElement::AddRef));
	i3::cu::for_each_SafeRelease(m_ObjList);
	m_ObjList = ObjList;	
}

void i3XMLFile::_setLastError( i3Error::CODE error, const char * pszMsg, bool bLog)
{
	m_LastError = error;

	if( bLog)
	{
		if( pszMsg == nullptr)
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
		return nullptr;

	UINT32 idx = (UINT32)InstanceID & 0x0000FFFF;

	I3ASSERT( (idx >= 1) && (idx <= m_ObjList.size()));

	return m_ObjList[ idx - 1];
}

i3PersistantElement *	i3XMLFile::FindObjectByMeta( i3ClassMeta * pMeta)
{
	i3PersistantElement * pObj;

	I3ASSERT( pMeta != nullptr);

	for(size_t i = 0; i < m_ObjList.size(); i++)
	{
		pObj = m_ObjList[i];

		if( pObj->kind_of( pMeta))
			return pObj;
	}

	return nullptr;
}

OBJREF i3XMLFile::GetObjectPersistID( i3PersistantElement * pObj)
{
	INT32 idx;

	if( pObj == nullptr)
		return 0;
	
	idx = i3::vu::int_index_of(m_ObjList, pObj) + 1;
	
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
	bool Rv;
	UINT32 Rc;
			
	// 7603 ms
	pszClassName = (char *) pXML->getAttr( "Class");
	I3ASSERT( pszClassName != nullptr);

	pszName = (char *) pXML->getAttr( "ExternPath");
	if( pszName == nullptr)
	{
		_setLastError( i3Error::_INVALIDFORMAT);
		return nullptr;
	}

	pMeta = i3ClassMetaSystem::i()->find_class_meta(pszClassName);
		//i3ClassMeta::FindClassMetaByName( pszClassName);

	pResManager = i3ResourceFile::GetResourceManager( pMeta);
	if( pResManager != nullptr)
	{
		pObj = (i3PersistantElement *) pResManager->GetResource( m_FileName.c_str(), i3ResourceFile::GetWorkingFolder(), pszName);
	}
	else
	{
		i3ResourceFile file;
		i3FileStream stream;

		if( i3ResourceFile::GetWorkingFolder()[0] != 0)
			sprintf( szFull, "%s/%s", i3ResourceFile::GetWorkingFolder(), pszName);
		else
			strncpy( szFull, pszName, sizeof( szFull)-1);

		Rv = stream.Open( szFull, STREAM_READ | STREAM_SHAREREAD);
		if( Rv == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not load %s external referenced file.", szFull);
			return nullptr;
		}

		stream.AddRef();
		stream.AddRef();		// 절대 해제되지 않도록 하기 위한..
		file.SetStream( &stream);

		pObj =	(i3PersistantElement *) pMeta->create_instance();

		Rc = pObj->OnLoad( &file);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not load %s external class object information", pObj->meta()->class_name());
			return nullptr;
		}

		if( i3::kind_of<i3NamedElement*>(pObj))
		{
			((i3NamedElement *) pObj)->SetName( pszName);
		}

		file.SetStream(nullptr);

		stream.Close();
	}


	if( pObj != nullptr)
	{
		pObj->SetExternRes(true);

		i3ResourceFile::CallExternRefCallback( (i3ResourceObject *) pObj, pszName);
	}

	return pObj;
}

UINT32 i3XMLFile::_SaveExtern( INT32 i, i3ResourceObject * pObj, i3XMLElement * pXML)
{
	i3FileStream stream;
	i3ClassMeta * pMeta;
	i3NamedResourceManager * pResManager;
	char szFull[ MAX_PATH];
	i3::fixed_string pszClassName;
	char * pszWorkDir = i3ResourceFile::GetWorkingFolder();
	UINT32 Rc = STREAM_ERR;

	pszClassName = pObj->meta()->persist_class_name(); //->GetPersistClassName();
	pMeta = i3ClassMetaSystem::i()->find_class_meta(pszClassName);
		//i3ClassMeta::FindClassMetaByName( pszClassName);
	I3ASSERT( pMeta != nullptr);

	pResManager = i3ResourceFile::GetResourceManager( pMeta);
	if( pResManager != nullptr)
	{
		Rc = pResManager->SaveResource( pszWorkDir, pObj); 
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "");
			return STREAM_ERR;
		}
	}
	else
	{
		if( pszWorkDir[0] != 0)
		{
			i3::snprintf( szFull, MAX_PATH, "%s/%s", pszWorkDir, pObj->GetNameString());
		}
		else
		{
			i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);
		}

		i3ResourceFile file;
		file.setKeyObject( pObj);
		Rc = file.Save( szFull);
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

	m_FileName = pszPath;			// i3ResourceFile에는 있는데..XML에는 없었음.추가.(12.09.18.수빈)

	m_pDoc = nullptr;

	m_LastError		= i3Error::_NOERROR;

	// Pack 파일에서도 읽을 수 있도록 수정.. swoongoo. 2011.5.27
	i3FileStream file;

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
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
		if( pRoot == nullptr)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}
		
		if( i3::generic_is_iequal( pRoot->Value(), "i3EngineResourceFile") == false)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}
	}

	// Header
	{
		pHeader = (i3XMLElement *) pRoot->FirstChild( "Header");
		if( pHeader == nullptr)
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
		i3::cu::for_each_SafeRelease_clear(m_ObjList);	// 추가..(2012.09.17.수빈)
		m_ObjList.resize(m_Header.m_ObjectCount);

		for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
		{
			sprintf( szName, "Object%d", i);

			i3XMLElement * pXML = (i3XMLElement *) pRoot->FirstChild( szName);
			if( pXML == nullptr)
			{
				_setLastError( i3Error::_INVALIDFORMAT);
				goto ExitPart;
			}

			const char * pszMetaName = pXML->getAttr( "Class");
			I3ASSERT( pszMetaName != nullptr);

			pXML->getAttr( "ID", &id);

			if( (id & 0xFFFF0000) == 0xFFFF0000 && id != -1 )
			{
				// External File
				pObj = _ResolveExtern( pXML);	
				I3_SAFE_ADDREF( pObj);	// 추가..(2012.09.17.수빈)
				m_ObjList[i] = pObj;
			}
			else
			{
				i3ClassMeta * pMeta = i3ClassMetaSystem::i()->find_class_meta(pszMetaName);
					// i3ClassMeta::FindClassMetaByName( pszMetaName);
				
				if( pMeta != nullptr)
				{
					pObj = (i3PersistantElement *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
					I3_SAFE_ADDREF( pObj);	// 추가..(2012.09.17.수빈)
					m_ObjList[i] = pObj;	
				}
				else
				{
					_setLastError( i3Error::_UNKNOWNCLASSMETA, pszMetaName);
					I3TRACE( "Invalid Class Meta Name : %s\n", pszMetaName);
					m_ObjList[i] = nullptr;
				}
			}
		}
	}

	// 다시 돌면서 실질적인 Object의 Loading을 처리
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		sprintf( szName, "Object%d", i);

		pXML = (i3XMLElement *) pRoot->FirstChild( szName);
		if( pXML == nullptr)
		{
			_setLastError( i3Error::_INVALIDFORMAT);
			goto ExitPart;
		}

		pObj = m_ObjList[i];

		pXML->getAttr( "ID", &id);

		if((id & 0xFFFF0000) == 0xFFFF0000 && id != -1)
		{
			// External File
		}
		else
		{
			if( pObj == nullptr)
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

			if( i3::same_of<i3InstanceProxy*>(pObj))
			{
				i3InstanceProxy * pInstProxy = (i3InstanceProxy *) pObj;

				// Instance Proxy Class가 생성한 Instance로 대체...
				m_ObjList[i] = pInstProxy->getInstance();
				I3_SAFE_ADDREF( m_ObjList[i]);
				// Instance Proxy는 제거..
			//	pObj->AddRef();			// 미리 더했으므로, 여기서는 주석(12.09.17.수빈)
				I3_MUST_RELEASE( pObj);
			}
		}

		if( pObj != nullptr)
			pObj->SetLoaded(true);
	}

	if( m_Header.m_pKeyObject != 0)
	{
		i3PersistantElement * pKeyObj = (i3PersistantElement *) FindObjectByID( (OBJREF) m_Header.m_pKeyObject);
		I3ASSERT( pKeyObj != nullptr);

		setKeyObject( pKeyObj);
	}
	
	for( i = 0; i < INT32(m_ObjList.size()) ; i++)
	{
		pObj = m_ObjList[i];

		if( pObj != nullptr)
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

	if( m_pDoc != nullptr)
	{
		delete m_pDoc;
		m_pDoc = nullptr;
	}

	file.Close();

	return bResult;
}

//
//  저장쪽은 m_ObjList가 저장전에 OnBuildObjList를 통해 확장되기 때문에 레퍼런스카운팅에 좀더 조심해야한다..(2012.09.17.수빈)
//

bool i3XMLFile::Save( const char * pszPath)
{
//	INT32 i;
	i3PersistantElement * pObj;
	bool bOldState = false;
	i3XMLElement * pHeader = nullptr;
	i3XMLElement * pRoot;
	char szName[MAX_PATH];
	bool bResult = false;

	m_FileName = pszPath;					// i3ResourceFile에는 있는데..XML에는 없었음.추가.(12.09.18.수빈)

	m_LastError		= i3Error::_NOERROR;


//	m_ObjList.SetOnceMode();		// 중복 Object의 push_back을 차단하는 플래그...후처리 검사하는게 더 나을것이다..

	{
		i3::vector<i3PersistantElement*> TempList(m_ObjList.begin(), m_ObjList.end());

		if( m_pKeyObject != nullptr)
		{
			if ( i3::find(TempList.begin(), TempList.end(), m_pKeyObject) == TempList.end() )
			{
				TempList.push_back(m_pKeyObject);		
				I3_MUST_ADDREF(m_pKeyObject);		// 추가...나중에 TempList가 릴리즈된다...(2012.09.17.수빈)
			}

			bOldState = m_pKeyObject->IsExternRes();
			m_pKeyObject->SetExternRes(false);
		}

		m_ObjList.clear();		// TempList에 원본포인터목록이 카피되었는데...나중에 이걸 릴리즈한다.(2012.09.17.수빈)

		for(size_t i = 0; i < TempList.size(); i++)
		{
			pObj = TempList[i];

			pObj->OnBuildObjectList( m_ObjList);
		}

		// 중복 오브젝트 후처리 검사..
		i3::vu::remove_duplicates(m_ObjList);		// m_ObjList가 확장되었을지도 모른다..
		i3::cu::for_each_container(m_ObjList, i3::mem_fun(&i3PersistantElement::AddRef));	// 여기서 모두 AddRef()..
		i3::cu::for_each_SafeRelease(TempList);		// 그다음에 TempList를 모두 릴리즈한다..(2012.09.17.수빈)
		//
	}

	I3ASSERT( m_ObjList.size() > 0);

	_CreateXMLDocument( pszPath);

	{
		pRoot = new i3XMLElement( "i3EngineResourceFile");
		pRoot = (i3XMLElement*) m_pDoc->LinkEndChild( pRoot);
		I3ASSERT( pRoot != nullptr);
	}

	// header information
	{
		pHeader = new i3XMLElement( "Header");

		pHeader->SetAttribute( "MajorVersion", 2);
		pHeader->SetAttribute( "MinorVersion", 1);
		pHeader->SetAttribute( "ObjectCount",	INT32(m_ObjList.size()));
		pHeader->SetAttribute( "KeyObject",		(INT32) GetObjectPersistID( getKeyObject()));
		
		pHeader = (i3XMLElement*) pRoot->LinkEndChild( pHeader);
		I3ASSERT( pHeader != nullptr);
	}

	// Objects
	const size_t obj_size = m_ObjList.size();

	for(size_t i = 0; i < obj_size; ++i)
	{
		i3ClassMeta * pMeta;
		i3XMLElement * pXML;

		pObj = m_ObjList[i];

		pMeta = pObj->meta();

		sprintf( szName, "Object%d", i);

		pXML = new i3XMLElement( szName);

		pXML->SetAttribute( "Class", pMeta->class_name().c_str());
		pXML->SetAttribute( "ID", (INT32) GetObjectPersistID( pObj));

		pXML = (i3XMLElement*) pRoot->LinkEndChild( pXML);
		I3ASSERT( pXML != nullptr);

		pushCurrentObject( pObj);

		if( pObj->IsExternRes())
		{
			I3ASSERT( i3::kind_of<i3ResourceObject*>(pObj));

			// External Reference
			if( m_bOverwriteExternalRes )
			{
				if( _SaveExtern( i, (i3ResourceObject *) pObj, pXML) == STREAM_ERR)
				{
					I3PRINTLOG(I3LOG_FATAL,  "_SaveExtern() failed");
				}
			}

			pXML->addAttr( "ExternPath", ((i3ResourceObject *)pObj)->GetName());
		}
		else
		{
			if( pObj->OnSaveXML( this, pXML) == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not write %s class object.", pMeta->class_name());				
				goto ExitPart;
			}
		}

		popCurrentObject();
	}

	bResult = m_pDoc->SaveFile();

	delete m_pDoc;
	m_pDoc = nullptr;

ExitPart:

	if( m_pKeyObject != nullptr)
	{
		m_pKeyObject->SetExternRes( bOldState);
	}

	return bResult;
}

