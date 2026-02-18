#include "stdafx.h"
#include "i3LevelResDatabase.h"
#include "i3LevelResChara.h"
#include "i3LevelResObject.h"
#include "i3LevelResCamera.h"
#include "i3LevelResPrim.h"
#include "i3LevelResPrimitive_Box.h"
#include "i3LevelResPrimitive_Sphere.h"
#include "i3LevelResPrimitive_Capsule.h"
#include "i3LevelResPrimitive_Respawn.h"
#include "i3LevelResAnimation.h"
#include "i3LevelResTexture.h"
#include "i3LevelResSound.h"
#include "i3LevelResWorld.h"
#include "i3LevelResLight.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelResEvent.h"
#include "i3LevelResPath.h"
#include "i3LevelResRef.h"

#include "i3Base/itl/vector_set_util.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/extract_filetitle_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/algorithm/to_upper.h"

//I3_CLASS_INSTANCE( i3LevelResDatabase, i3PersistantElement);
I3_CLASS_INSTANCE( i3LevelResDatabase);

/*
static INT32 _ResCompareProc( i3LevelRes * p1, i3LevelRes * p2)
{
	I3ASSERT( p1->IsTypeOf( i3LevelRes::GetClassMeta()));
	I3ASSERT( p2->IsTypeOf( i3LevelRes::GetClassMeta()));

	if( p1->getHashCode()[0] > p2->getHashCode()[0])
		return 1;
	else if( p1->getHashCode()[0] < p2->getHashCode()[0])
		return -1;

	if( p1->getHashCode()[1] > p2->getHashCode()[1])
		return 1;
	else if( p1->getHashCode()[1] < p2->getHashCode()[1])
		return -1;

	if( p1->getHashCode()[2] > p2->getHashCode()[2])
		return 1;
	else if( p1->getHashCode()[2] < p2->getHashCode()[2])
		return -1;

	if( p1->getHashCode()[3] > p2->getHashCode()[3])
		return 1;
	else if( p1->getHashCode()[3] < p2->getHashCode()[3])
		return -1;

	return 0;
}
*/
bool		operator<(const i3LevelRes& a1, const i3LevelRes& a2)
{

	if( a1.getHashCode()[0] < a2.getHashCode()[0])
		return true;
	else if ( a1.getHashCode()[0] > a2.getHashCode()[0])
		return false;	

	if( a1.getHashCode()[1] < a2.getHashCode()[1])
		return true;
	else if ( a1.getHashCode()[1] > a2.getHashCode()[1])
		return false;

	if( a1.getHashCode()[2] < a2.getHashCode()[2])
		return true;
	else if( a1.getHashCode()[2] > a2.getHashCode()[2])
		return false;

	if( a1.getHashCode()[3] < a2.getHashCode()[3])
		return true;
	else if( a1.getHashCode()[3] > a2.getHashCode()[3])
		return false;

	return false;
}

/*
static INT32 _ResFindProc( i3LevelRes * p1, UINT32 * hash)
{
	if( p1->getHashCode()[0] > hash[0])
		return 1;
	else if( p1->getHashCode()[0] < hash[0])
		return -1;

	if( p1->getHashCode()[1] > hash[1])
		return 1;
	else if( p1->getHashCode()[1] < hash[1])
		return -1;

	if( p1->getHashCode()[2] > hash[2])
		return 1;
	else if( p1->getHashCode()[2] < hash[2])
		return -1;

	if( p1->getHashCode()[3] > hash[3])
		return 1;
	else if( p1->getHashCode()[3] < hash[3])
		return -1;

	return 0;
}
*/
struct _ResFindProc
{
	bool comp( const UINT32* hash1, const UINT32 * hash2  ) const 
	{
		if( hash1[0] > hash2[0])
			return false;
		else if( hash1[0] < hash2[0])
			return true;

		if( hash1[1] > hash2[1])
			return false;
		else if(hash1[1] < hash2[1])
			return true;

		if( hash1[2] > hash2[2])
			return false;
		else if( hash1[2] < hash2[2])
			return true;

		if( hash1[3] > hash2[3])
			return false;
		else if( hash1[3] < hash2[3])
			return true;

		return false;
	}
	bool operator()( const i3LevelRes* p1, const UINT32 * hash2  ) const 
	{
		return comp(p1->getHashCode(), hash2);
	}
	bool operator()( const UINT32* hash1, const i3LevelRes * p2  ) const 
	{
		return comp(hash1, p2->getHashCode());
	}

};


static INT32 _ResCRCFindProc( i3LevelRes * p1, UINT32 code)
{
	char szTemp[ MAX_PATH];

	strncpy( szTemp, p1->getPath(), sizeof(szTemp) - 1);
	i3::to_upper( szTemp);

	UINT32 crc = CRC32( 0xFFFFFFFF, (UINT8 *) szTemp, strlen( szTemp));

	if( crc > code)
		return 1;
	else if( crc < code)
		return -1;

	return 0;
}

i3LevelResDatabase::i3LevelResDatabase(void)
{
	_BuildDefTree();
}

i3LevelResDatabase::~i3LevelResDatabase(void)
{
	I3_SAFE_RELEASE( m_pRoot);
	I3_SAFE_RELEASE( m_pThread);
}

void i3LevelResDatabase::Reset(void)
{
	RemoveAllRes();

	m_bModified = false;
}

void i3LevelResDatabase::BindScene( i3LevelScene * pScene)
{
	if( m_pRoot != nullptr)
	{
		m_pRoot->BindScene( pScene);
	}
}

static char	s_szTypeRootName[I3LV_RESDB_TYPE_COUNT][128] =
{
	"Level Primitive",
	"Character",
	"Object",
	"World",
	"Light",
	"Camera",
	"Scene-Graph",
	"Effect",
	"Animation",	
	"Sound",		
	"Texture",
	"Event",
	"Path",
};

void i3LevelResDatabase::_BuildDefTree(void)
{
	INT32 i;

	// Root
	m_pRoot = i3LevelResRef::new_object();
	m_pRoot->SetName( "Resource Database");

	// Type Roots
	for( i = 0; i < I3LV_RESDB_TYPE_COUNT; i++)
	{
		m_pTypeRoot[ i]  = i3LevelResRef::new_object_ref();

		m_pTypeRoot[i]->SetName( s_szTypeRootName[i]);
		m_pTypeRoot[i]->setIconStyle( I3LV_RESREF_ICON_GROUPROOT);

		m_pRoot->AddChild( m_pTypeRoot[i]);
	}

	//Light
	{
		i3LevelResLight * pLight = i3LevelResLight::new_object_ref();
		pLight->SetName("Light");

		i3LevelResRef * pNode = i3LevelResRef::new_object_ref();
		pNode->setRes( pLight);

		m_pTypeRoot[I3LV_RESDB_TYPE_LIGHT]->AddChild( pNode);
	}

	// Level Primitives
	{
		i3LevelResRef * pNode;

		// Box
		{
			i3LevelResPrimitive_Box * pRes = i3LevelResPrimitive_Box::new_object_ref();
			pRes->SetName( "Box");

			pNode = i3LevelResRef::new_object_ref();
			pNode->setRes( pRes);

			m_pTypeRoot[ I3LV_RESDB_TYPE_PRIMITIVE]->AddChild( pNode);
		}	

		// Sphere
		{
			i3LevelResPrimitive_Sphere * pRes = i3LevelResPrimitive_Sphere::new_object_ref();
			pRes->SetName( "Sphere");

			pNode = i3LevelResRef::new_object_ref();
			pNode->setRes( pRes);

			m_pTypeRoot[ I3LV_RESDB_TYPE_PRIMITIVE]->AddChild( pNode);
		}

		// Capsule
		{
			i3LevelResPrimitive_Capsule * pRes = i3LevelResPrimitive_Capsule::new_object_ref();
			pRes->SetName( "Capsule");

			pNode = i3LevelResRef::new_object_ref();
			pNode->setRes( pRes);

			m_pTypeRoot[ I3LV_RESDB_TYPE_PRIMITIVE]->AddChild( pNode);
		}

		// Respawn
		{
			i3LevelResPrimitive_Respawn * pRes = i3LevelResPrimitive_Respawn::new_object_ref();
			pRes->SetName( "Respawn");
			pRes->setPath( "Level_Primitive_Respawn");

			pNode = i3LevelResRef::new_object_ref();
			pNode->setRes( pRes);

			m_pTypeRoot[ I3LV_RESDB_TYPE_PRIMITIVE]->AddChild( pNode);
		}
	}

	//Camera
	{
		i3LevelResCamera * pCam = i3LevelResCamera::new_object_ref();
		pCam->SetName( "Camera");

		i3LevelResRef * pNode = i3LevelResRef::new_object_ref();
		pNode->setRes( pCam);

		m_pTypeRoot[I3LV_RESDB_TYPE_CAMERA]->AddChild( pNode);
	}

	//_RefreshResList();
	// 
}

void i3LevelResDatabase::RemoveAllRes(void)
{
	m_Mutex.Enter();

	I3_SAFE_RELEASE( m_pRoot);

	memset( m_pTypeRoot, 0, sizeof(m_pTypeRoot));

	_BuildDefTree();

	m_Mutex.Leave();
	m_ResList.clear();
	m_RemoveResPathList.clear();

	m_bModified = true;
}
/*
i3LevelRes * i3LevelResDatabase::getRes( const char * szResPath)
{
	INT32	nResCount = m_ResList.GetCount();
	for( INT32 i = 0; i<nResCount; ++i)
	{
		i3LevelRes * pRes = (i3LevelRes *)m_ResList.Items[i];
		I3ASSERT( pRes != nullptr);

		if( i3String::Compare( pRes->getPath(), szResPath) == 0)
			return pRes;
	}

	return nullptr;
}
*/

i3LevelResRef *	i3LevelResDatabase::_PreparePath( i3LevelResRef * pRoot, char * pszPath)
{
	// Path를 Parsing하여 붙일 마지막 Leaf Node를 구성하고 반환한다.
	i3LineLexer lexer;
	char szToken[MAX_PATH], sep;
	i3LevelResRef * pCur;

	lexer.SetMasterString( pszPath);

	while( lexer.GetNextToken( szToken, &sep, "/\\") > 0)
	{
		pCur = (i3LevelResRef *) pRoot->FindChildByName( szToken);

		if( pCur == nullptr)
		{
			pCur = i3LevelResRef::new_object_ref();

			pCur->SetName( szToken);

			pRoot->AddChild( pCur);
		}

		pRoot = pCur;
	}

	return pRoot;
}

i3LevelRes * i3LevelResDatabase::AddRes( const char * pszPath, WIN32_FIND_DATAA * pInfo)
{
	i3::stack_string strExt;
	i3LevelRes * pRes = nullptr;
	WIN32_FIND_DATAA findData;
	char szFull[MAX_PATH];

	if( i3System::isAbsolutePath( pszPath) == false)
	{
		sprintf_s( szFull, _countof( szFull), "%s/%s", g_szResPath, pszPath);
	}
	else
	{
		sprintf_s( szFull, _countof( szFull), "%s", pszPath);
	}

	if( pInfo == nullptr)
	{
		HANDLE hFind = FindFirstFileA( szFull, &findData);

		if( hFind == INVALID_HANDLE_VALUE)
			return nullptr;

		pInfo = &findData;

		FindClose( hFind);
	}

//	i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);
	i3::extract_fileext(pszPath, strExt);

	if( i3::generic_is_iequal( strExt, "I3CHR") )
	{
		pRes = i3LevelResChara::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "I3OBJ") )
	{
		pRes = i3LevelResObject::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "I3WRD") )
	{
		pRes = i3LevelResWorld::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "I3S") )
	{
		pRes = i3LevelResSceneGraph::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "I3A") )
	{
		pRes = i3LevelResAnimation::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "I3I") || i3::generic_is_iequal( strExt, "TGA") )
	{
		pRes = i3LevelResTexture::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "WAV") )
	{
		pRes = i3LevelResSound::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "i3Light") )
	{
		pRes = i3LevelResLight::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "i3Evt") )
	{
		pRes = i3LevelResEvent::new_object();
	}
	else if( i3::generic_is_iequal( strExt, "i3Path") )
	{
		pRes = i3LevelResPath::new_object();
	}

	if( pRes != nullptr)
	{
		char szRelPath[MAX_PATH], szName[MAX_PATH];

		i3::make_relative_path( g_szResPath, szFull, szRelPath);

		i3::extract_filetitle(pszPath, szName);

		i3::make_unix_path( szRelPath);

		pRes->SetName( szName);
		pRes->setPath( szRelPath);
		pRes->setLastUpdateFileTime( &pInfo->ftLastWriteTime);

		if( FindResByPath( szRelPath) != nullptr)
		{
			I3_SAFE_RELEASE( pRes);
			return nullptr;
		}

		if( pRes->OnValidateBrief( true))
		{
			AddRes( pRes);
			I3_MUST_RELEASE(pRes);
		}
		else
		{
			I3_MUST_RELEASE(pRes);
			pRes = nullptr;
		}
	}

	return pRes;
}

void i3LevelResDatabase::AddRes( i3LevelRes * pRes)
{
	I3ASSERT( pRes != nullptr);
	i3LevelResRef * pNode, * pParent = nullptr;

	switch( pRes->getResType())
	{
		case I3LV_TYPE_PRIMITIVE :		pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_PRIMITIVE];	break;
		case I3LV_TYPE_CHARA :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_CHARA];	break;
		case I3LV_TYPE_WORLD :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_WORLD];	break;
		case I3LV_TYPE_OBJECT :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_OBJECT];	break;
		case I3LV_TYPE_EFFECT :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_EFFECT];	break;
		case I3LV_TYPE_LIGHT :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_LIGHT];	break;
		case I3LV_TYPE_CAMERA :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_CAMERA];	break;
		case I3LV_TYPE_TEXTURE :		pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_TEXTURE];	break;
		case I3LV_TYPE_ANIMATION :		pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_ANIMATION];	break;
		case I3LV_TYPE_SOUND :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_SOUND];	break;
		case I3LV_TYPE_SCENEGRAPH :		pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_SCENEGRAPH];	break;
		case I3LV_TYPE_EVENT:			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_EVENT];	break;
		case I3LV_TYPE_PATH :			pParent = m_pTypeRoot[ I3LV_RESDB_TYPE_PATH];	break;
	}

	I3ASSERT_RETURN( pParent != nullptr);

	if( i3::generic_is_equal( pRes->GetName() , "") )
	{
		I3TRACE("FOUND nullptr RESOURCE NAME\n");
	}

	pNode = _PreparePath( pParent, (char *) pRes->getPath());
	I3ASSERT( pNode != nullptr);

	if( pNode->getRes() != nullptr)
	{
	//	m_ResList.Remove( pNode->getRes());
		i3::vsu::linear_remove(m_ResList, pNode->getRes());
		m_RemoveResPathList.insert( pNode->getRes()->getPath() );
	}

	pNode->setRes( pRes);
	m_ResList.insert( pRes);

	i3::set< i3::string >::iterator itr;

	itr = m_RemoveResPathList.find( pRes->getPath() );
	if( m_RemoveResPathList.end() != itr )
	{
		m_RemoveResPathList.erase( itr );
	}

	m_bModified = true;

	if( m_bUpdateEnable)
	{
		i3TDK::Update( nullptr, I3_TDK_UPDATE_ADD, pRes);
	}
}

void i3LevelResDatabase::AddRes( i3LevelResRef * pParent, i3LevelResRef * pNode)
{
	m_Mutex.Enter();

	pParent->AddChild( pNode);

	m_bModified = true;

	m_Mutex.Leave();
}

void i3LevelResDatabase::RemoveRes( i3LevelRes * pRes)
{
	i3LevelResRef * pRef, * pParent;

	I3ASSERT( pRes != nullptr);

	m_Mutex.Enter();

	pRef = GetResRef( pRes);
	//I3ASSERT( pRef != nullptr);

	if( nullptr != pRef )
	{
		pParent = (i3LevelResRef *) pRef->getParent();
		I3ASSERT( pParent != nullptr);

		m_RemoveResPathList.insert( pRes->getPath() );
	// m_ResList.Remove( pRes);
	i3::vsu::linear_remove(m_ResList, pRes);
	//

		i3::vsu::linear_remove(m_ResList, pRes);

		if( m_bUpdateEnable)
		{
			i3TDK::Update( nullptr, I3_TDK_UPDATE_REMOVE, pRes);
		}

		pParent->RemoveChild( pRef);		
	}

	m_Mutex.Leave();

	if( nullptr != pRef )
	{
		m_bModified = true;
	}
}

i3LevelRes *	i3LevelResDatabase::FindResByPathHash( UINT32 * hashcode)
{
//	INT32 idx;
//	m_ResList.SetCompareProc( (COMPAREPROC) _ResFindProc);
//	idx = m_ResList.FindItem( (void *) hashcode);
//	m_ResList.SetCompareProc( (COMPAREPROC) _ResCompareProc);
	i3::pair<i3::vector_set<i3LevelRes*>::const_iterator, bool> pr = i3::vsu::binary_find(m_ResList, hashcode, _ResFindProc());
	
	if( pr.second == false)
		return nullptr;

	return *pr.first;
}

i3LevelRes *	i3LevelResDatabase::FindResByOldPathHash( UINT32 code)
{
	
	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		i3LevelRes * pRes = m_ResList.get_vector()[i];

		if( _ResCRCFindProc( pRes, code) == 0)
		{
			return pRes;
		}
	}

	return nullptr;
}

i3LevelRes *	i3LevelResDatabase::FindResByPath( const char * pszPath)
{
	UINT32 code[4];
	char szTemp[ MAX_PATH], szFull[MAX_PATH];

	if( i3System::isAbsolutePath( pszPath))
	{
		sprintf_s( szFull, _countof( szFull), "%s", pszPath);
	}
	else
	{
		sprintf_s( szFull, _countof( szFull), "%s/%s", g_szResPath, pszPath);
	}

	i3::make_relative_path( g_szResPath, szFull, szTemp);

	i3::make_unix_path( szTemp);
	i3::to_upper( szTemp);

	i3MD5::Gen( szTemp, strlen( szTemp), (char *) code);

	i3LevelRes * pResult =  FindResByPathHash( code);

	if( pResult == nullptr)
	{
		// 대소문자 등의 문제로 인해 제대로 검색하지 못했다.
		// 문자열 비교로 다시 한번 찾는다.
		i3LevelRes * pTemp;

		for(size_t i = 0; i < m_ResList.size(); i++)
		{
			pTemp = m_ResList.get_vector()[i];

			if( i3::generic_is_iequal( pTemp->getPath(), pszPath) )
				return pTemp;
		}
	}

	return pResult;
}

void i3LevelResDatabase::FindResByType( i3ClassMeta * pMeta, i3::vector<i3LevelRes*>& List)
{
	INT32 i;
	i3LevelRes * pRes;

	for( i = 0; i < getResCount(); i++)
	{
		pRes = getRes( i);

		if( pRes->kind_of( pMeta))
		{
			List.push_back( pRes);
		}
	}
}

i3LevelRes * i3LevelResDatabase::GetRes( const char * pszPath)
{
	i3LevelRes * pRes;

	pRes = FindResByPath( pszPath);

	if( pRes == nullptr)
	{
		pRes = AddRes( pszPath, nullptr);
	}

	return pRes;
}

////////////////////////////////////////////////////////////////////////////////////
//

BOOL i3LevelResDatabase::_FindRefResProc( INT32 depth, i3LevelResRef * pNode, __FIND_RESREF_INFO * pInfo)
{
	i3LevelRes * pRes = pNode->getRes();

	if( pRes == pInfo->m_pRes)
	{
		pInfo->m_pRef = pNode;
		return FALSE;
	}

	return TRUE;
}

void i3LevelResDatabase::GetHierarchyPath( i3LevelRes * pRes, i3::vector<i3LevelResRef*>& List)
{
	i3LevelResRef * pTemp;
	__FIND_RESREF_INFO info;

	// pRes를 참조하고 있는 Res-Ref를 찾는다.
	info.m_pRef = nullptr;
	info.m_pRes = pRes;

	i3TreeNode::Traverse( m_pRoot, (I3_TREE_TRAVERSE_PROC) _FindRefResProc, &info);

	if( info.m_pRef == nullptr)
		return;

	pTemp = info.m_pRef;

	while( pTemp != nullptr)
	{
		List.insert( List.begin(), pTemp);

		pTemp = (i3LevelResRef *) pTemp->getParent();
	}
}

i3LevelResRef *	i3LevelResDatabase::GetResRef( i3LevelRes * pRes)
{
	__FIND_RESREF_INFO info;

	// pRes를 참조하고 있는 Res-Ref를 찾는다.
	info.m_pRef = nullptr;
	info.m_pRes = pRes;

	i3TreeNode::Traverse( m_pRoot, (I3_TREE_TRAVERSE_PROC) _FindRefResProc, &info);

	return info.m_pRef;
}

void i3LevelResDatabase::GetUniqueResName( I3LV_RESDB_ROOT_TYPE type, const char * pszPath, const char * pszPrefix, char * pszName, UINT16 nBufSize)
{
	// Path를 Parsing하여 붙일 마지막 Leaf Node를 구성하고 반환한다.
	i3LineLexer lexer;
	char szToken[MAX_PATH], sep;
	INT32 id = 0;
	i3LevelResRef * pCur = m_pRoot;
	i3LevelResRef * pRoot;

	strncpy( pszName, pszPrefix, nBufSize);

	pRoot = getResTypeRoot( type);

	lexer.SetMasterString( (char *) pszPath);

	while( lexer.GetNextToken( szToken, &sep, "/\\") > 0)
	{
		pCur = (i3LevelResRef *) pRoot->FindChildByName( szToken);

		if( pCur == nullptr)
		{
			// 기존에 존재하지 않는 Path에의 Res이기 때문에
			// 이미 Unique하다.
			return;
		}

		pRoot = pCur;
	}

	char szExt[MAX_PATH], szFileName[MAX_PATH];

//	i3String::SplitFileExt( pszName, szExt, sizeof(szExt) - 1);
//	i3String::SplitFileName( pszName, szFileName, FALSE);
	i3::extract_filetitle_fileext(pszName, szFileName, szExt);

	while( pCur->FindChildByName( pszName) != nullptr)
	{
		sprintf( pszName, "%s_%d.%s", szFileName, id, szExt);
		id++;
	}
}

////////////////////////////////////////////////////////////////////////////////////
//
BOOL i3LevelResDatabase::_FlushResProc( INT32 depth, i3LevelResRef * pNode, void * pUserData)
{
	//i3LevelResDatabase * pDB = (i3LevelResDatabase *) pUserData;
	i3LevelRes * pRes = pNode->getRes();

	if( pRes == nullptr)
		return TRUE;

	if( pRes->isModified() == false)
		return TRUE;

	char szPath[ MAX_PATH];

	sprintf( szPath, "%s/%s", g_szResPath, pRes->getPath());

	// Directory Prepare
	{
		char szDir[MAX_PATH];

	//	i3String::SplitPath( szPath, szDir);
		i3::extract_directoryname(szPath, szDir);

		i3System::PrepareDirectory( szDir);
	}

	pRes->OnSerialize();

	pRes->removeResState( I3LV_RES_STATE_MODIFIED);

	return TRUE;
}  

void i3LevelResDatabase::FlushDB(void)
{
	// 제거된 Resource 의 파일 삭제
	{
		char szPath[ MAX_PATH ];
		for each( const i3::string& path in m_RemoveResPathList )
		{
			sprintf_s( szPath, "%s/%s", g_szResPath, path.c_str() );
			::DeleteFileA( szPath);
		}

		m_RemoveResPathList.clear();
	}

	// 개별 Resource를 순회하면서 저장해야한다.
	{
		i3TreeNode::Traverse( m_pRoot, (I3_TREE_TRAVERSE_PROC) _FlushResProc, this);
	}

	if( m_bModified)
	{
		// DB 자체의 Flush
		m_bModified = false;

		//_RefreshResList();
	}
}


////////////////////////////////////////////////////////////////////////////////////
//
BOOL i3LevelResDatabase::_RefreshResListProc( INT32 depth, i3LevelResRef * pNode, void * pUserData)
{
	i3LevelResDatabase * pDB = (i3LevelResDatabase *) pUserData;
	i3::vector_set<i3LevelRes*>& ResList = pDB->m_ResList;

	if( pNode->getRes() != nullptr)
	{
		i3LevelRes * pRes = pNode->getRes();

		I3ASSERT( i3::kind_of<i3LevelRes*>(pRes));

		ResList.insert( pNode->getRes());
	}

	if( pNode->getIconStyle() & I3LV_RESREF_ICON_GROUPROOT)
	{
		INT32 i;

		for( i = 0; i < I3LV_RESDB_TYPE_COUNT; i++)
		{
			if( i3::generic_is_iequal( pNode->GetName(), s_szTypeRootName[i]) )
			{
				pDB->m_pTypeRoot[i] = pNode;
				break;
			}
		}
	}

	return TRUE;
}  

void i3LevelResDatabase::_RefreshResList(void)
{
	m_ResList.clear();
	m_RemoveResPathList.clear();

	i3TreeNode::Traverse( m_pRoot, (I3_TREE_TRAVERSE_PROC) _RefreshResListProc, this);

	/*
	INT32 i;
	i3LevelRes * pRes;

	for( i = 0; i < m_ResList.GetCount(); i++)
	{
		pRes = (i3LevelRes *) m_ResList.Items[i];

		I3TRACE( "%08X : %s\n", pRes->getHashCode(), pRes->getPath());
	}
	*/
}

////////////////////////////////////////////////////////////////////////////////////
//
static bool _DumpProc( INT32 depth, i3TreeNode * pNode, void * pUserData)
{
	INT32 i;
	i3LevelRes * pRes;

	for( i = 0; i < depth; i++)
	{
		I3TRACE( "\t");
	}

	pRes = ((i3LevelResRef *)pNode)->getRes();

	if( pRes != nullptr)
	{
		I3ASSERT( i3::kind_of< i3LevelRes*>(pRes));
		I3TRACE( "+ %s [%s]\n", pNode->GetName(), pRes->meta()->class_name());
	}
	else
	{
		I3TRACE( "+ %s [nullptr]\n", pNode->GetName());
	}

	return true;
}


void i3LevelResDatabase::Dump(void)
{
	I3TRACE( "------------------------  Resource Database ----------------------\n");
	if( m_pRoot != nullptr)
	{
		i3TreeNode::Traverse( m_pRoot, _DumpProc, nullptr);
	}
	else
	{
		I3TRACE( "Empty\n");
	}
	I3TRACE( "------------------------------------------------------------------\n");
}

void i3LevelResDatabase::Validate( i3LevelScene * pScene, i3LevelRes * pRes)
{
	pRes->OnValidate( pScene, false);
}

void i3LevelResDatabase::StartScan( HWND hwnd, const char * pszPath, const i3::set< i3::string >& ignoreFolder, I3LV_RES_SCAN_PROC pProc, bool bForce )
{
	if( bForce == true)
	{
		RemoveAllRes();
	}

	m_pThread = i3LevelResScanThread::new_object();

	m_pThread->setResourcePath( pszPath);
	m_pThread->setResDatabase( this);
	m_pThread->setHWND( hwnd);
	m_pThread->InsertIgnoreFolder( ignoreFolder );

	//m_pThread->OnRunning( this);
	m_pThread->Create( "ResScanThread");
}

void i3LevelResDatabase::StopScan(void)
{
	if( m_pThread != nullptr)
	{
		m_pThread->Terminate();
		I3_MUST_RELEASE(m_pThread);
		m_pThread = nullptr;
	}
}

void i3LevelResDatabase::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pRoot != nullptr)
	{
		m_pRoot->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)					// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct RES_DATABASE
	{
		UINT8			m_ID[4] = { 'R', 'D', 'B', '2' };
		OBJREF			m_pRoot = 0;
		UINT32			pad[128] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResDatabase::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::RES_DATABASE data;

	result = 0;

	data.m_pRoot	=	pResFile->GetObjectPersistID( m_pRoot);

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	m_bModified = false;

	return result;
}

UINT32 i3LevelResDatabase::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result = 0;
	pack::RES_DATABASE data;

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pRoot != 0)
	{
		i3LevelResRef * pNode = (i3LevelResRef *) pResFile->FindObjectByID( data.m_pRoot);
		I3ASSERT( pNode != nullptr);

		I3_REF_CHANGE( m_pRoot, pNode);
	}

	_RefreshResList();

	m_bModified = false;

	return result;
}

void i3LevelResDatabase::CheckDuplicateID(void)
{
	INT32 i, j, cnt = 0, k;
	i3LevelRes * pRes1, * pRes2;

	Check();

	for( i = 0; i < getResCount() - 1; i++)
	{
		for( j = i + 1; j < getResCount(); j++)
		{
			pRes1 = getRes( i);
			pRes2 = getRes( j);

			if( pRes1->getHashCode() == pRes2->getHashCode())
			{
				UINT32 crc1, crc2;
				char md51[16], md52[16];
				char szTemp[ MAX_PATH];

				strncpy( szTemp, pRes1->getPath(), sizeof(szTemp) - 1);
				i3::to_upper( szTemp);
				crc1 = CRC32( 0xFFFFFFFF, (UINT8 *) szTemp, strlen( szTemp));
				i3MD5::Gen( szTemp, strlen( szTemp), md51);

				strncpy( szTemp, pRes2->getPath(), sizeof(szTemp) - 1);
				i3::to_upper( szTemp);
				crc2 = CRC32( 0xFFFFFFFF, (UINT8 *) szTemp, strlen( szTemp));
				i3MD5::Gen( szTemp, strlen( szTemp), md52);

				I3TRACE( "SAME RES-------------\n");
				I3TRACE( "[%d] %08X(%08X) : %s (%s)\n", i, pRes1->getHashCode(), crc1, pRes1->GetName(), pRes1->meta()->class_name());

				for( k = 0; k < 16; k++)
					I3TRACE( "%02X", md51[k] & 0xFF);
				I3TRACE( "\n");

				I3TRACE( "[%d] %08X(%08X) : %s (%s)\n", i, pRes2->getHashCode(), crc2, pRes2->GetName(), pRes2->meta()->class_name());

				for( k = 0; k < 16; k++)
					I3TRACE( "%02X", md52[k] & 0xFF);
				I3TRACE( "\n");

				cnt ++;
			}
		}
	}

	TCHAR conv[256];

	if( cnt == 0)
	{
		i3::snprintf( conv, 256, _T("Resource Database에서 문제가 발견되지 않았습니다.") );
	}
	else
	{
		i3::snprintf( conv, 256, _T("%d 개의 Resource가 중복 Hash Code를 가지고 있습니다."), cnt);
	}

	MessageBox( nullptr, conv, _T("Report"), MB_OK);
}

void i3LevelResDatabase::Check(void)
{
	
	i3LevelRes * pRes;

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList.get_vector()[i];

		I3ASSERT( i3::kind_of<i3LevelRes*>(pRes)); 
	}
}
