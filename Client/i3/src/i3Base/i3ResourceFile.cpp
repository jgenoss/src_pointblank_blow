#include "i3CommonType.h"
#include "i3ResourceFile.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Memory.h"
#include "i3Macro.h"
#include "i3String.h"
#include "i3NamedElement.h"
#include "i3ResourceObject.h"
#include "i3Performance.h"
#include "i3InstanceProxy.h"
#include "i3MD5.h"
#include <stdio.h>

#include "string/ext/extract_filename.h"
#include "string/ext/make_relative_path.h"
#include "string/ext/remove_end_slash.h"
#include "string/compare/generic_is_equal.h"
#include "string/algorithm/to_upper.h"
#include "i3Base/string/ext/safe_string_copy.h"

#if defined( I3_WINDOWS)
#include "i3DefExternRefResolveWin32.h"
#endif

#include "itl/vector_util.h"
#include "i3StringListBuffer.h"
#include "i3System.h"



// ================================================================================================
//
// i3ResourceFile
//
// ================================================================================================
#define I3RF_ALIGN_UNIT			1

bool	operator<( const I3_RESOURCE_MANAGER_INFO& a1, const I3_RESOURCE_MANAGER_INFO& a2 )
{
	return a1.m_pMeta < a2.m_pMeta;
}

template<> struct i3::less<I3_RESOURCE_MANAGER_INFO*> : i3::less_ptr<I3_RESOURCE_MANAGER_INFO*> {};


static i3::vector_set<I3_RESOURCE_MANAGER_INFO*>		s_ResManagerList;

static char						s_szWorkingFolder[MAX_PATH] = "\x0";
static bool						s_bProfEnable = false;
static I3_EXTERNALREF_REPORT	s_pExternRefCallbackProc = nullptr;
static void *					s_pExternRefCallbackUserData = nullptr;
bool							s_bSearchMissingFile = false;
#if defined( I3_WINDOWS)
	static I3_EXTERNALREF_RESOLVE	s_pExternRefResolveProc = _DefExternResolveProcWin32;
#else
	static I3_EXTERNALREF_RESOLVE	s_pExternRefResolveProc = nullptr;
#endif
static void *					s_pExternRefResolveUserData = nullptr;

#define		__PF_S( a)			if( s_bProfEnable) { __SP( a); }
#define		__PF_E( a)			if( s_bProfEnable) { __EP( a); }

I3_CLASS_INSTANCE( i3ResourceFile);

i3ResourceFile::i3ResourceFile(void)
{
}

i3ResourceFile::~i3ResourceFile(void)
{
	// 레퍼런스카운트가 1인 오브젝트는 활용이 안되는 것으로 간주하고, TRACE보고후 삭제를 하는게 좋을지도 모른다.
	// 이부분은 툴과 게임실행까지 완벽하게 진행되면 코드를 확정...(2012.09.11.수빈)
	const size_t num_obj = m_ObjList.size();	
	for (size_t i = 0 ; i < num_obj ; ++i)
	{
		i3PersistantElement* obj = m_ObjList[i];
		if (!obj) 
			continue;					// 읽을때 널요소를 없애기 때문에 아닐것을 기대하지만 안전하게...

		if ( obj->GetRefCount() == 1 )		// 카운팅도 되지 않고, 활용되지 않은 리소스..
		{
#if defined(I3_DEBUG) && 0		// 디버그 모드에 트레이스를 보고한다..(추정으로는 메모리릭의 원인이 될수도 있었는데, 일단 제거함수 추가)
			if ( i3::kind_of<i3NamedElement*>(obj) )
			{
				i3NamedElement* ne = static_cast<i3NamedElement*>(obj);
				if (ne->GetNameString().empty() == false )
					I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT NAME = %s, OBJECT META = %s\n", 
						this->getFileName(), i, num_obj, ne->GetName(), ne->meta()->class_name() );
				else
					I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT META = %s\n", 
						this->getFileName(), i, num_obj, obj->meta()->class_name() );
			}
			else
			{
				I3TRACE("[ALERT] Unused object exist : RESOURCEFILE NAME = %s, INDEX_OF_OBJECTLIST = %d of %d, OBJECT META = %s\n", 
					this->getFileName(), i, num_obj, obj->meta()->class_name() );
			}
#endif
			obj->destroy_instance();
		}
		else
		{
			I3_MUST_RELEASE( obj);
		}
	}
	//
	I3MEM_SAFE_FREE_POINTER( m_pObjInfo );
	I3MEM_SAFE_FREE( m_pIDTable);
}

bool i3ResourceFile::_isExtern( OBJREF ref)
{
	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion >= 8))
	{
		if( ref & 0x80000000)
			return true;
	}
	else
	{
		if( (ref & 0xFFFF0000) == 0xFFFF0000)
			return true;
	}

	return false;
}

INT32 i3ResourceFile::_getRefStringIndex( OBJREF ref)
{
	if( _isExtern( ref))
	{
		if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion >= 8))
			return (ref & 0x7FFF0000) >> 16;
		else
			return (ref & 0x0000FFFF);
	}

	return -1;
}

INT32 i3ResourceFile::_getRefOrdinal( OBJREF ref)
{
	return (ref & 0x0000FFFF) - 1;
}

void i3ResourceFile::SetObjects( i3::vector<i3PersistantElement*>& List)
{
	i3::for_each(List.begin(), List.end(), i3::mem_fun(&i3PersistantElement::AddRef));
	i3::cu::for_each_SafeRelease(m_ObjList);
	m_ObjList = List;
}

UINT32	i3ResourceFile::GetAlignedPos( UINT32 pos)
{
	return ((pos + I3RF_ALIGN_UNIT - 1) / I3RF_ALIGN_UNIT) * I3RF_ALIGN_UNIT;
}


UINT32 i3ResourceFile::Align( i3Stream * pStream)
{
	UINT32 OldPos, NewPos;

	OldPos = pStream->GetPosition();

	NewPos = GetAlignedPos( OldPos);

	if( OldPos != NewPos)
	{
		pStream->SetPosition( NewPos, STREAM_BEGIN);
	}

	return NewPos;
}
void i3ResourceFile::DeleteObject(INT32 nID)
{
	i3PersistantElement* pElem = m_ObjList[nID];
//	pElem->AddRef();		// m_ObjList는 이제 레퍼런스카운팅되므로, AddRef()를 괜이 하지 않음..(2012.09.17.수빈)
	I3_SAFE_RELEASE(pElem);
	m_ObjList.erase( m_ObjList.begin() + nID);
}
INT32 i3ResourceFile::FindObjectInfoByID( OBJREF InstanceID)
{
	UINT32 i;

	for( i = 0; i < m_Header.m_ObjectCount; i++)
	{
		if( m_pObjInfo[i].m_InstanceID == InstanceID)
		{
			return i;
		}
	}

	return -1;
}

i3PersistantElement * i3ResourceFile::FindObjectByID( OBJREF InstanceID)
{
	INT32 start, end;
	INT32 idx, ordinal;
	i3PersistantElement * pObj;

	// Version 1.7 이상부터 직접 Indexing으로 변경.
	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion >= 8))
	{
		// 1.8부터는 Extern Code에 대한 정의를 MSB만으로 줄인다.
		if ( InstanceID & 0x80000000)		// 익스턴인 경우이다...
		{
			idx = static_cast<INT32>(InstanceID & 0x0000FFFF) - 1;
		}
		else
		{
			idx = static_cast<INT32>(InstanceID)-1;
		}
//		idx = _getRefOrdinal( InstanceID);

		I3_BOUNDCHK( idx, INT32(m_ObjList.size()) );

		return m_ObjList[idx];
	}

	if( m_ObjList.size() <= 0)
		return nullptr;

	start = 0;
	end = m_IDCount - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;
		
		if( m_pIDTable[idx].m_ID == InstanceID)
		{
			// External Reference의 경우, 동일 ID가 있을 수 있다.
			//
			ordinal = m_pIDTable[idx].m_Ordinal;
			pObj = m_ObjList[ordinal];
			
			return pObj;
		}
		 
		if( InstanceID < m_pIDTable[idx].m_ID)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;
		
		if( m_pIDTable[idx].m_ID == InstanceID)
		{
			ordinal = m_pIDTable[idx].m_Ordinal;
			return m_ObjList[ ordinal];
		}
	}

	return nullptr;
}

i3PersistantElement * i3ResourceFile::FindObjectByMeta( i3ClassMeta * pMeta)
{
	i3PersistantElement * pObj;

	for(size_t i = 0; i < m_ObjList.size(); i++)
	{
		pObj = m_ObjList[i];

		if( pObj->kind_of( pMeta))
			return pObj;
	}

	return nullptr;
}

OBJREF				i3ResourceFile::GetObjectPersistID( i3PersistantElement * pObj)
{
	OBJREF ref;
	bool bNoname = false;

	if( pObj == nullptr)
		return 0;

	if( pObj->IsExternRes())
	{
		INT32 idxStr, idx;
		char szName[256];

		if( i3::kind_of<i3NamedElement*>(pObj) )
		{
			i3NamedElement * pElm = (i3NamedElement *) pObj;

			if( pElm->hasName())
				i3::safe_string_copy( szName, pElm->GetNameString(), 256);
			else
				bNoname = true;
		}
		else
			bNoname = true;

		if( bNoname)
		{
			i3::snprintf( szName, 256, (PSTR) "%s_%08X", pObj->meta()->class_name(), (UINT32)pObj);
		}

		// 이름을 String Table에 등록한다.
		idxStr = i3::vu::index_of(m_StrTable, szName);
		if (idxStr == INT32(m_StrTable.size())) idxStr = -1;
				// idxStr = m_StrTable.IndexOf( szName);
		I3_BOUNDCHK( idxStr, 0x7FFF);

		idx = i3::vu::int_index_of(m_ObjList, pObj) + 1;

		I3_BOUNDCHK( idx, 0xFFFF);

		ref = 0x80000000 | ((0x00007FFF & idxStr) << 16) | (0x0000FFFF & idx);
	}
	else
	{
		ref = (OBJREF) (i3::vu::int_index_of(m_ObjList, pObj) + 1);
	}

	return ref;
}

//#define I3_DEBUG_LOADTEST
i3::rc_string		i3ResourceFile::FindNameByID( OBJREF InstanceID)
{
	INT32 idx = _getRefStringIndex(InstanceID);

	if (idx == -1 || idx >= INT32(m_StrTable.size()) ) 
		return i3::rc_string();

	return m_StrTable[idx];
}

i3PersistantElement *	i3ResourceFile::_ResolveExtern( INT32 i, bool bCommonRes)
{
	char  szFull[ MAX_PATH];
	i3ClassMeta * pMeta;
	INT32 idx;
	i3PersistantElement * pObj;
	i3NamedResourceManager * pResManager;
	bool Rv;
	UINT32 Rc;
	bool bExit = true;
	char szPath[ MAX_PATH];


	// 7603 ms
	idx = _getRefStringIndex( (OBJREF) m_pObjInfo[i].m_InstanceID);

	i3::rc_string strName = m_StrTable[idx];

	i3::rc_string strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex];

	pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassName);

	if( bCommonRes )
	{	// CommonRes 로드중에는 i3SG를 제외한 리소스는 로드하지 않는다.
		if( (i3::generic_is_equal( strClassName, "i3SceneGraphInfo") == false) )
			//&& (i3::generic_is_equal( strClassName, "i3Texture") == false) )
			return nullptr;
	}

	if ( this->CheckExternNeedLoad(pMeta) == false )		// 내부 가상함수 사용
		return nullptr;

	pResManager = GetResourceManager( pMeta);
	
	i3::safe_string_copy( szPath, strName.c_str(), MAX_PATH );

	do
	{
		if( pResManager != nullptr)
		{
			pObj = (i3PersistantElement *) pResManager->GetResource( m_FileName.c_str(), s_szWorkingFolder, szPath, m_bBackgroundLoad);
		}
		else
		{
			i3FileStream stream;

			if( s_szWorkingFolder[0] != 0)
				sprintf( szFull, "%s/%s", s_szWorkingFolder, szPath);
			else
				strncpy( szFull, szPath, sizeof( szFull)-1);

			Rv = stream.Open( szFull, STREAM_READ | STREAM_SHAREREAD);
			if( Rv == false)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %s external referenced file.", szFull);
				return nullptr;
			}

			stream.AddRef();
			SetStream( &stream);

			if( m_bBackgroundLoad == false)
			{
				pObj = (i3PersistantElement *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			}
			else
			{
				pObj = (i3PersistantElement *) pMeta->create_instance_ghost(); //CREATEINSTANCE_TYPE( pMeta, I3_CLASSMETA_TYPE_GHOST);
			}

			Rc = pObj->OnLoad( this);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not load %s external class object information", pObj->meta()->class_name());
				return nullptr;
			}

			if( i3::kind_of<i3NamedElement*>(pObj))
			{
				((i3NamedElement *) pObj)->SetName( szPath );
			}
			
			stream.Close();
		}

		if( pObj != nullptr)
		{
			if( s_pExternRefCallbackProc != nullptr)
				s_pExternRefCallbackProc( pObj, strName.c_str(), s_pExternRefCallbackUserData);

			bExit = true;
		}
		else
		{
			// 로그 삭제
			//I3PRINTLOG(I3LOG_NOTICE, "Can not find external resource file %s from %s", strName, m_FileName);
//#if defined(I3_DEBUG)
			//I3PRINTLOG(I3LOG_FATAL, "Can not find external resource file %s from %s", pszName, m_szFileName);
//#endif

			if( getExternRefResolveCallback() != nullptr)
			{
				bExit = false;

				char szTemp[MAX_PATH];

				bool rv = getExternRefResolveCallback()( strName.c_str(), szTemp, sizeof(szTemp), s_szWorkingFolder, s_bSearchMissingFile);

				if( rv == false)
					bExit = true;
				else
				{
					i3::make_relative_path( s_szWorkingFolder, szTemp, szPath);
				}
			}
		}
	} while( bExit == false);

	return pObj;
}

UINT32 i3ResourceFile::_SaveExtern( INT32 i, i3ResourceObject * pObj)
{
	i3FileStream stream;
	INT32 idx;
	i3ClassMeta * pMeta;
	i3NamedResourceManager * pResManager;
	char szFull[ MAX_PATH] ;
	bool Rv;
	UINT32 Rc;

	//idx = (INT32)(m_pObjInfo[i].m_InstanceID & 0x0000FFFF);
	idx = (INT32)(0x00007FFF & (m_pObjInfo[i].m_InstanceID >> 16));
	i3::rc_string	strName = m_StrTable[idx];
	i3::rc_string   strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex];

	pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassName);
			// i3ClassMeta::FindClassMetaByName( strClassName.c_str());
	I3ASSERT( pMeta != nullptr);

	pResManager = GetResourceManager( pMeta);
	if( pResManager != nullptr)
	{
		pObj->SetExternRes(false);
		Rc = pResManager->SaveResource( s_szWorkingFolder, pObj); 
		pObj->SetExternRes( true);

		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "");
			return STREAM_ERR;
		}
	}
	else
	{
		if( s_szWorkingFolder[0] != 0)
		{
			sprintf( szFull, "%s/%s", s_szWorkingFolder, strName.c_str() );
		}
		else
		{
			strncpy( szFull, strName.c_str(), sizeof( szFull) - 1);
		}
	
		Rv = stream.Create( szFull, STREAM_WRITE);
		if( Rv == false)
		{
			i3Log( "i3ResourceFile::Save()", "Could not create %s external object file.", szFull);
			return STREAM_ERR;
		}

		SetStream( &stream);

		Rc = pObj->OnSave( this);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ResourceFile::Save()", "Could not write %s external class object.", pMeta->class_name().c_str());
			I3MEM_SAFE_FREE_POINTER( m_pObjInfo);
			return STREAM_ERR;
		}
	}

	return Rc;
}

void i3ResourceFile::_Rec_QuickSort( I3_RES_OBJECT_ID_INFO * pTable, INT32 idxMin, INT32 idxMax)
{
	INT32 i = idxMin;
	INT32 j = idxMax;
	OBJREF id = pTable[ (i + j) >> 1].m_ID;

	do
	{
		while( pTable[i].m_ID < id)	i++;
		while( pTable[j].m_ID > id) j--;

		if( i <= j)
		{
			OBJREF tempID		= pTable[i].m_ID;
			INT32	tempOrdinal = pTable[i].m_Ordinal;

			pTable[i].m_ID		= pTable[j].m_ID;
			pTable[i].m_Ordinal	= pTable[j].m_Ordinal;

			pTable[j].m_ID		= tempID;
			pTable[j].m_Ordinal	= tempOrdinal;

			i++;
			j--;
		}

	} while( i <= j);

	if( idxMin < j)		_Rec_QuickSort( pTable, idxMin, j);
	if( i < idxMax)		_Rec_QuickSort( pTable, i, idxMax);
}

void i3ResourceFile::_BuildIDTable( INT32 count)
{
	INT32 i, j;

	m_IDCount = count;

	// ObjectTable의 값을 IdTable로 복사한다.
	for( i = 0; i < count; i++)
	{
		m_pIDTable[i].m_ID		= (OBJREF) m_pObjInfo[i].m_InstanceID;
		m_pIDTable[i].m_Ordinal = i;
	}

	// ID Table을 Sorting한다.
	_Rec_QuickSort( m_pIDTable, 0, m_IDCount - 1);

	// 동일 ID가 있는지 확인하고, 제거한다.
	for( i = 0; i < (m_IDCount - 1); i++)
	{
		// 동일한 ID들 중에서는 가장 먼저 Loading되어질, 최소 Ordinal Object만을 남긴다.
		INT32 minIdx = m_pIDTable[i].m_Ordinal;

		for( j = i + 1; (j < m_IDCount) && (m_pIDTable[i].m_ID == m_pIDTable[j].m_ID); j++)
		{
			minIdx = min( minIdx,  m_pIDTable[j].m_Ordinal);
		}
		
		if( j > (i + 1))
		{
			// 동일 ID들은 필요없기 때문에 ID Table에서 제거
			if( j < m_IDCount)
				memcpy( &m_pIDTable[ i + 1], &m_pIDTable[j], sizeof(I3_RES_OBJECT_ID_INFO) * (m_IDCount - j));

			// 최소 Ordinal Index만 남긴다.
			m_pIDTable[i].m_Ordinal = minIdx;
			m_IDCount -= j - (i + 1);
		}
	}
}

UINT32 i3ResourceFile::LoadHeader( i3Stream * pStream)
{
	INT32 i;
	UINT32	BasePos = pStream->GetPosition();
	UINT32 Result = 0, Rc;
	bool	bOldVersion = false;

	SetStream( pStream);

	m_StrTable.clear();
//	m_ObjList.clear();
	i3::cu::for_each_SafeRelease_clear(m_ObjList);		// 이제는 릴리즈처리를 병행해야함.(2012.09.17.수빈)

	{
		UINT8	id[4];

		Rc = pStream->Read( id, 4);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not read file-identifier.");
			Result = STREAM_ERR;
			goto ExitPart;
		}
		Result += Rc;

		if( memcmp( id, I3_RSCFILE_HEAD_10, 4) == 0)
			bOldVersion = true;
	}

	// Header Information
	{
		if( bOldVersion)
		{
			i3::pack::RESOURCE_FILE_HEADER1 OldHeader;

			Rc = pStream->Read( &OldHeader, sizeof( OldHeader));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read header(version 1.0f) information.");
				Result = STREAM_ERR;
				goto ExitPart;
			}
			Result += Rc;
			
			m_Header.m_MajorVersion			= OldHeader.m_MajorVersion;
			m_Header.m_MinorVersion			= OldHeader.m_MinorVersion;
			m_Header.m_ObjectCount			= OldHeader.m_ObjectCount;
			m_Header.m_ObjectDirOffset		= OldHeader.m_ObjectDirOffset;
			m_Header.m_ObjectDirSize		= OldHeader.m_ObjectDirSize;
			m_Header.m_StringTableCount		= OldHeader.m_StringTableCount;
			m_Header.m_StringTableOffset	= OldHeader.m_StringTableOffset;
			m_Header.m_StringTableSize		= OldHeader.m_StringTableSize;
		}
		else
		{
			i3::pack::RESOURCE_FILE_HEADER2 head;
			Rc = pStream->Read( &head, sizeof(head));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read header information.");
				Result = STREAM_ERR;
				goto ExitPart;
			}
			Result += Rc;

			m_Header.CopyFromFile( &head);
		}
	}

	// String Table
	{
		char * pTemp;		

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_StringTableOffset), STREAM_BEGIN);

		m_StrTable.clear();
		
		pTemp = (char *) i3MemAlloc( (INT32)m_Header.m_StringTableSize);
		I3ASSERT( pTemp != nullptr);

		Rc = pStream->Read( pTemp, (INT32)m_Header.m_StringTableSize);
		if( Rc == STREAM_ERR)
		{
			I3MEM_SAFE_FREE_POINTER( pTemp);

			i3Log( "i3ResourceFile::Load()", "Could not read strign table.");
			Result = STREAM_ERR;
			goto ExitPart;
		}
		Result += Rc;

		CopyFromBufferToStringList(m_StrTable, pTemp);

		I3MEM_SAFE_FREE_POINTER( pTemp ); 
	}
	
	// Object Directory
	{		
		UINT32 ObjInfoSz = sizeof(i3::pack::RESOURCE_FILE_OBJECT_INFO2) * m_Header.m_ObjectCount;

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_ObjectDirOffset), STREAM_BEGIN);

		m_pObjInfo  = (i3::pack::RESOURCE_FILE_OBJECT_INFO2 *) i3MemAlloc( ObjInfoSz);
		I3ASSERT( m_pObjInfo != nullptr);

		m_pIDTable = (I3_RES_OBJECT_ID_INFO *) i3MemAlloc( sizeof(I3_RES_OBJECT_ID_INFO) * m_Header.m_ObjectCount);

		if( bOldVersion )
		{
			i3::pack::RESOURCE_FILE_OBJECT_INFO1	info;

			memset( m_pObjInfo, 0, ObjInfoSz);

			for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
			{
				Rc = pStream->Read( &info, sizeof(info));
				if( Rc == STREAM_ERR)
				{
					I3PRINTLOG(I3LOG_FATAL,  "Could not read object directory information.");
					Result = STREAM_ERR;
					goto ExitPart;
				}
				Result += Rc;

				m_pObjInfo[i].m_ClassNameIndex = info.m_ClassNameIndex;
				m_pObjInfo[i].m_InstanceID = info.m_InstanceID;
				m_pObjInfo[i].m_Offset = info.m_Offset;
				m_pObjInfo[i].m_Size = info.m_Size;
			}
		}
		else
		{
			Rc = pStream->Read( m_pObjInfo, ObjInfoSz);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read object directory information.");
				Result = STREAM_ERR;
				goto ExitPart;
			}
			Result += Rc;
		}
	}

ExitPart:

	return Result;
}

static bool		s_bDump = false;
#define		DTRACE					if( s_bDump) I3TRACE

UINT32 i3ResourceFile::Load( i3Stream * pStream, bool bCommonRes)
{
	INT32 i;
	UINT32 Result = STREAM_ERR, Rc;
	i3PersistantElement * pObj;
	UINT32	BasePos = pStream->GetPosition();

	if( LoadHeader( pStream) == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Object Preparation
	//
	// Loading 시에 다른 Object에대한 Reference가 발생했을 때, 올바르게 검색할 수 있도록 하기
	// 위해, 모든 Object를 먼저 생성하여 List에 등록해 둔다.

	m_ObjList.resize(m_Header.m_ObjectCount);

	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		pObj = nullptr;

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID) == false)
		{
			if( m_pObjInfo[i].m_ClassNameIndex >= (UINT32) m_StrTable.size() )
			{
				I3ASSERT_0;
			}
			else
			{
				i3::rc_string strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex];
				i3ClassMeta * pMeta;

				// find class meta by name
				pMeta = i3ClassMetaSystem::i()->find_class_meta(strClassName); 
					// i3ClassMeta::FindClassMetaByName( strClassName.c_str() );
				if( pMeta == nullptr)
				{
					I3PRINTLOG(I3LOG_WARN, "[%s] Unknown Class Meta .(%s).", getFileName(), strClassName ); 

					m_pObjInfo[i].m_ClassNameIndex = 0xFFFF;
				}
				else
				{
					// create a instance of specified class
					//I3TRACE( "1 : %s\n", pMeta->GetClassName());

					if( m_bBackgroundLoad == false)
					{
						pObj = (i3PersistantElement *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
					}
					else
					{
						pObj = (i3PersistantElement *) pMeta->create_instance_ghost(); //CREATEINSTANCE_TYPE( pMeta, I3_CLASSMETA_TYPE_GHOST);
					}
					
					if( pObj == nullptr)
					{
						::DebugBox(NULL, "i3ResourceFile::Load() - CreateInstance failed. (%s) : It could be non-ConcreteClass.", strClassName.c_str());
						I3ASSERT_0;
					}
					else
					{
						I3_MUST_ADDREF( pObj);		// 주요변경사항..(소멸자에서 내림)..2012.09.17.수빈..
					}
				}
			}
		}

		// External Referenced File은 이 시점에서는 단순히 nullptr로 설정해둔다.
		
		m_ObjList[i] = pObj;		// nullptr일 수 있다.
	}

	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion < 8))
	{
		// 1.8 이전의 경우에는 ID Table을 준비하여야 한다.
		_BuildIDTable( m_Header.m_ObjectCount);
	}


	// 2008.09.23
	// ExternRef 파일들은 종속성이 없기 때문에 선행 로드를 해두어
	// 여타 오브젝트에서의 디펜던시 문제가 발생하지 않도록 해준다. 
	// 만약 아래와 같이 ExternRef 파일들을 선행 로드하지 않으면
	// OnBuildObjectList에서 리스트에서 추가하는 순서에 제약이 발생한다.
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		if( m_pObjInfo[i].m_ClassNameIndex == 0xFFFF)
			continue;

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID) )
		{
			pObj = _ResolveExtern( i, bCommonRes);

			SetStream( pStream);

			if( pObj != nullptr)
			{
				pObj->SetExternRes(true);
				I3_MUST_ADDREF( pObj);		// 주요변경사항..(소멸자에서 내림)..2012.09.17.수빈..
				m_ObjList[i] = pObj;
			}
		}
	}


	// Objects
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		if( m_pObjInfo[i].m_ClassNameIndex == 0xFFFF)
			continue;

		if( m_pUserProgressProc != nullptr)
		{
			m_pUserProgressProc( m_pUserData, i, m_Header.m_ObjectCount);
		}

		pObj = m_ObjList[i];

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID))
		{
			// External Object는 이미 로딩이 된 상태
		}
		else
		{
			// set stream position
			pStream->SetPosition( (INT32)(BasePos + m_pObjInfo[i].m_Offset), STREAM_BEGIN);

			#if defined( I3_DEBUG_LOADTEST)
				I3TRACE( "LOAD : %s start\n", pObj->meta()->class_name() );
			#endif

			if( pObj->IsLoaded())
				continue;

			//I3TRACE( "LOADING : %s...", pObj->GetMeta()->GetClassName());

			Rc = pObj->OnLoad( this);
			if( Rc == STREAM_ERR)
			{
				I3TRACE( "Could not load object data [%s]", pObj->meta()->class_name() );
				goto ExitPart;
			}

			if( i3::same_of<i3InstanceProxy*>(pObj))
			{
				i3InstanceProxy * pInstProxy = (i3InstanceProxy *) pObj;

				// Instance Proxy Class가 생성한 Instance로 대체...
				m_ObjList[i] = pInstProxy->getInstance();

				I3_SAFE_ADDREF( m_ObjList[i]);
				// Instance Proxy는 제거..
				// pObj->AddRef();			// 이제 레퍼런스카운팅이 미리 되므로, 여기선 주석처리..(2012.09.17.수빈)
				I3_SAFE_RELEASE( pObj);
			}

			DTRACE( "Done\n");
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
	
	for( i = 0; i < INT32(m_ObjList.size()); i++)
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

	if( m_pUserProgressProc != nullptr)
	{
		m_pUserProgressProc( m_pUserData, m_Header.m_ObjectCount, m_Header.m_ObjectCount);
	}

	Result = GetAlignedPos( pStream->GetPosition() - BasePos);

ExitPart:

	return Result;
}

//
//  저장쪽은 m_ObjList가 저장전에 OnBuildObjList를 통해 확장되기 때문에 레퍼런스카운팅에 좀더 조심해야한다..(2012.09.17.수빈)
//


UINT32 i3ResourceFile::Save( i3Stream * pStream)
{
	INT32 i;
	UINT32 Result = 0, Rc = STREAM_ERR, ObjInfoSz, BasePos = pStream->GetPosition();
	i3PersistantElement * pObj;
	bool bOldState = false;
	SetStream( pStream);


	{
		i3::vector<i3PersistantElement*> TempList(m_ObjList.begin(), m_ObjList.end());

		if( m_pKeyObject != nullptr)
		{
			if( i3::find(TempList.begin(), TempList.end(), m_pKeyObject) == TempList.end() )
			{
				TempList.push_back( m_pKeyObject);	
				I3_MUST_ADDREF( m_pKeyObject);		// 추가...나중에 TempList가 릴리즈된다...(2012.09.17.수빈)
			}

			bOldState = m_pKeyObject->IsExternRes();
			m_pKeyObject->SetExternRes(false);
		}

		m_ObjList.clear();	// TempList에 원본포인터목록이 카피되었는데...나중에 이걸 릴리즈한다.(2012.09.17.수빈)

		for( i = 0; i < INT32(TempList.size()) ; i++)
		{
			pObj = TempList[i];

			pObj->OnBuildObjectList( m_ObjList);
		}
		//
		i3::vu::remove_duplicates(m_ObjList);// m_ObjList가 확장되었을지도 모른다..
		i3::cu::for_each_container(m_ObjList, i3::mem_fun(&i3PersistantElement::AddRef));	// 여기서 모두 AddRef()..
		i3::cu::for_each_SafeRelease(TempList);		// 그다음에 TempList를 모두 릴리즈한다..(2012.09.17.수빈)
		//

	}

	// External Reference  되는 Object는 제거한다.
	m_StrTable.clear();

	for( i = 0; i < INT32(m_ObjList.size()); i++)
	{
		bool bAdd;

		bAdd = false;

		pObj = m_ObjList[i];

		if( pObj->IsExternRes())
		{
			if( !i3::kind_of<i3NamedElement*>(pObj))
			{
				bAdd = true;
			}
			else
			{
				i3NamedElement * pElm = (i3NamedElement *) pObj;

				if( pElm->hasName() == false)
					bAdd = true;
			}

			if( bAdd)
			{
				char szName[256];

				sprintf( (PSTR) szName, (PSTR) "%s_%08X", pObj->meta()->class_name().c_str(), (UINT32) pObj);

				m_StrTable.push_back( szName);
			}

			//m_ObjList.Delete( i);

			//i--;
		}
	}

	I3ASSERT( INT32(m_ObjList.size()) > 0);

	// Build String Table
	{
		for( i = 0; i < INT32(m_ObjList.size()); i++)
		{
			pObj = m_ObjList[i];
			pObj->OnQueryStrings( m_StrTable);
		}
	}
	i3::vu::remove_duplicates(m_StrTable);

	// header information
	{
		Rc = pStream->Write( I3_RSCFILE_HEAD_20, 4);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write I3R2 file header.");
			goto ExitPart;
		}

		i3::pack::RESOURCE_FILE_HEADER2 head;
		i3mem::FillZero( &head, sizeof(head));
		head.m_MajorVersion = 1;
		head.m_MinorVersion = 9;

		head.m_StringTableCount = (UINT32) m_StrTable.size();
		head.m_StringTableOffset = (UINT64) GetAlignedPos( sizeof(i3::pack::RESOURCE_FILE_HEADER2));
		head.m_StringTableSize =  (UINT64)	GetStringListBufferSize(m_StrTable);	// m_StrTable.GetBufferSize();

		head.m_ObjectCount = (UINT32) m_ObjList.size();
		head.m_ObjectDirOffset = GetAlignedPos( (UINT32)( head.m_StringTableOffset + head.m_StringTableSize));
		ObjInfoSz = sizeof(i3::pack::RESOURCE_FILE_OBJECT_INFO2) * head.m_ObjectCount;
		head.m_ObjectDirSize = ObjInfoSz;
		head.m_pKeyObject = (OBJREF64) GetObjectPersistID( getKeyObject());

		//파일 저장 시 LocaleTime 기록 하지 않도록 합니다.
		//SYSTEMTIME systime;
		//i3mem::FillZero( &systime, sizeof(SYSTEMTIME));
		//GetLocalTime( &systime);

		//FILETIME ftime;
		//i3mem::FillZero( &ftime, sizeof(FILETIME));
		//SystemTimeToFileTime( &systime, &ftime);

		//m_Header.pad[LOW_DATA_TIME] = (UINT32)ftime.dwLowDateTime; 
		//m_Header.pad[HIGH_DATA_TIME] = (UINT32)ftime.dwHighDateTime;

		Rc = pStream->Write( &head, sizeof(i3::pack::RESOURCE_FILE_HEADER2));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write I3R file header.");
			goto ExitPart;
		}

		Result += Rc;

		m_Header.CopyFromFile( &head);
	}

	// String Table
	if( m_Header.m_StringTableSize > 0)
	{
		char * pTemp;		
		pStream->SetPosition( (INT32)(BasePos + m_Header.m_StringTableOffset), STREAM_BEGIN);

		pTemp = (char *) i3MemAlloc( (INT32)(m_Header.m_StringTableSize) );
		I3ASSERT( pTemp != nullptr);

		CopyFromStringListToBuffer(m_StrTable, pTemp, (INT32)m_Header.m_StringTableSize);
	//	m_StrTable.CopyToBuffer( pTemp, (INT32)m_Header.m_StringTableSize);

		Rc = pStream->Write( pTemp, (INT32) m_Header.m_StringTableSize);
		if( Rc == STREAM_ERR)
		{
			i3MemFree( pTemp );
			I3PRINTLOG(I3LOG_FATAL,  "Could not write string table information.");
			goto ExitPart;
		}

		i3MemFree( pTemp );

		Result += Rc;
	}

	// Object Info (Placeholder)
	//
	// After storing object information, it will be updated again.
	{		
		pStream->SetPosition( (INT32)(BasePos + m_Header.m_ObjectDirOffset), STREAM_BEGIN);

		//m_ObjInfoIdx = i3MemAlloc( ObjInfoSz);
		m_pObjInfo = (i3::pack::RESOURCE_FILE_OBJECT_INFO2 *) i3MemAlloc( ObjInfoSz);

		Rc = pStream->Write( m_pObjInfo, ObjInfoSz);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not write object directory.");
			goto ExitPart;
		}
	}

	Align( pStream);

	// Objects

	for( i = 0; i < INT32(m_ObjList.size()); i++)
	{
		i3ClassMeta * pMeta;

		if( m_pUserProgressProc != nullptr)
		{
			m_pUserProgressProc( m_pUserData, i, INT32(m_ObjList.size()) );
		}

		pObj = m_ObjList[i];

		pMeta = pObj->meta();

		m_pObjInfo[i].m_ClassNameIndex = i3::vu::int_index_of(m_StrTable, pMeta->persist_class_name());
		m_pObjInfo[i].m_Offset = pStream->GetPosition() - BasePos;
		m_pObjInfo[i].m_InstanceID = (UINT32) GetObjectPersistID( pObj);

		if( pObj->IsExternRes())
		{
			// External Reference
			if( m_bOverwriteExternalRes )
			{
				I3ASSERT( i3::kind_of<i3ResourceObject*>(pObj));

				if( _SaveExtern( i, (i3ResourceObject *) pObj) == STREAM_ERR)
				{
					I3PRINTLOG(I3LOG_FATAL,  "");
				}

				SetStream( pStream);
			}

			Rc = 0;
		}
		else
		{
			Rc = pObj->OnSave( this);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not write %s class object.", pMeta->class_name());				
				goto ExitPart;
			}
		}

		m_pObjInfo[i].m_Size = Rc;
		Result += Rc;

		Align( pStream);
	}

	// Update Object Directory
	{
		pStream->SetPosition( (INT32)(BasePos + m_Header.m_ObjectDirOffset), STREAM_BEGIN);

		Rc = pStream->Write( m_pObjInfo, ObjInfoSz);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Could not update object directory information.");
			goto ExitPart;
		}
	}

	if( m_pUserProgressProc != nullptr)
	{
		m_pUserProgressProc( m_pUserData, INT32(m_ObjList.size()), INT32(m_ObjList.size()) );
	}

ExitPart:

	if( Rc == STREAM_ERR)
		Result = STREAM_ERR;

	if( m_pKeyObject != nullptr)
	{
		m_pKeyObject->SetExternRes( bOldState);
	}

	// Termination
	I3MEM_SAFE_FREE_POINTER( m_pObjInfo );		

	return Result;
}

UINT32 i3ResourceFile::Load( const char * pszPath, bool bCommonRes)
{
	i3FileStream Stream;
	bool Rv;
	UINT32 Result;

	m_FileName = pszPath;

	m_pStream = &Stream;
	I3_SAFE_ADDREF( m_pStream);		// 혹시라도 RefCount가 0인 것이 문제가 될 수 있기 때문에...
	
	Rv = Stream.Open( pszPath, STREAM_READ | STREAM_SHAREREAD);
	if( Rv == false)
	{
		i3Log( "i3ResourceFile::Load()", "Could not open %s file.", pszPath);
		return STREAM_ERR;
	}

	Result = Load( m_pStream, bCommonRes);

	m_pStream->Close();

	m_pStream = nullptr;

	return Result;
}

UINT32 i3ResourceFile::Save( const char * pszPath)
{
	i3FileStream Stream;
	bool Rv;
	UINT32 Result;

	m_FileName = pszPath;
	m_pStream = &Stream;

	Rv = Stream.Create( pszPath, STREAM_WRITE | STREAM_READ);
	if( Rv == false)
	{
		i3Log( "i3ResourceFile::Save()", "Could not create %s file.", pszPath);
		return STREAM_ERR;
	}

	Result = Save( m_pStream);

	m_pStream->Close();

	m_pStream = nullptr;

	return Result;
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
static bool _ResourceManagerTerminateProc( void * pData)
{
	if (s_ResManagerList.empty())
		return true;


	I3_RESOURCE_MANAGER_INFO * pInfo;

	for( size_t i = 0; i < s_ResManagerList.size(); i++)
	{
		pInfo = s_ResManagerList.get_vector()[i];
		I3_SAFE_RELEASE( pInfo->m_pManager);
		I3MEM_SAFE_FREE( pInfo);
	}
	
	s_ResManagerList.clear();

	return true;
}



void	i3ResourceFile::AddResourceManager( i3ClassMeta * pMeta, i3NamedResourceManager * pResMnger)
{
	if( s_ResManagerList.empty() )
	{
		// 종료함수 추가.
		i3System::AddTerminateProc( 9, _ResourceManagerTerminateProc, nullptr);
	}

	I3_RESOURCE_MANAGER_INFO * pNew = (I3_RESOURCE_MANAGER_INFO *) i3MemAlloc(sizeof(I3_RESOURCE_MANAGER_INFO));

	memset( pNew, 0, sizeof( I3_RESOURCE_MANAGER_INFO));
	pNew->m_pMeta = pMeta;
	pNew->m_pManager = pResMnger;

	I3_SAFE_ADDREF( pResMnger);

	s_ResManagerList.insert( pNew);
}

void	i3ResourceFile::RemoveResourceManager( i3ClassMeta * pMeta)
{
	I3_RESOURCE_MANAGER_INFO temp, * pFind;

	temp.m_pMeta = pMeta;
	
	i3::vector_set<I3_RESOURCE_MANAGER_INFO*>::iterator it = s_ResManagerList.find(&temp);

	if( it != s_ResManagerList.end() )
	{
		pFind = *it;

		I3_MUST_RELEASE( pFind->m_pManager);
		i3MemFree( pFind);

		s_ResManagerList.erase( it );
	}
}

i3NamedResourceManager * i3ResourceFile::GetResourceManager( i3ClassMeta * pMeta)
{
	I3_RESOURCE_MANAGER_INFO  * pFind;
	
	if( s_ResManagerList.empty() )
		return nullptr;

	for( size_t i = 0; i < s_ResManagerList.size(); i++)
	{
		pFind = s_ResManagerList.get_vector()[i];

		if( pMeta->kind_of( pFind->m_pMeta))
		{
			return pFind->m_pManager;
		}
	}

	return nullptr;
}

void i3ResourceFile::SetExternalRefCallback( I3_EXTERNALREF_REPORT pProc, void * pData)
{
	s_pExternRefCallbackProc = pProc;
	s_pExternRefCallbackUserData = pData;
}

void i3ResourceFile::CallExternRefCallback( i3ResourceObject * pObj, const char * pszName)
{
	if( s_pExternRefCallbackProc != nullptr)
	{
		s_pExternRefCallbackProc( pObj, pszName, s_pExternRefCallbackUserData);
	}
}

char *	i3ResourceFile::GetWorkingFolder(void)
{
	return s_szWorkingFolder;
}

void	i3ResourceFile::SetWorkingFolder( char * pszPath)
{
	i3::safe_string_copy( s_szWorkingFolder, pszPath, MAX_PATH );
//	i3String::NormalizePath( s_szWorkingFolder);
	i3::remove_end_slash(s_szWorkingFolder);
}

void	i3ResourceFile::setProfileEnable( bool bEnable)
{
	s_bProfEnable = bEnable;
}

#define		READ_UNIT			0x0000FFFF

static bool _CalcFileChecksum( const char * pszPath, const char * pKey, INT32 length, UINT32 * pCode)
{
	i3FileStream file;
	i3::pack::RESOURCE_FILE_HEADER2 head;
	UINT32 rc, Size;
	md5_context ctx;

	// Start
	i3MD5::HMAC_Start( &ctx, (char *) pKey, length);

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	Size = file.GetSize();

	// FileName을 Checksum에 추가
	{
		char szName[MAX_PATH];
		
		i3::extract_filename(pszPath, szName);
		i3::to_upper( szName);

		i3MD5::HMAC_Update( &ctx, szName, strlen( szName));
	}

	// Resource File Header를 Checksum에 추가
	// 주의 : I3_RESOURCE_FILE_HEADER2::m_ChecksumCode 자체는 Checksum에 포함되어선
	//			안되기 때문에 0xFFFFFFFF로 Clear를 한 후 Checksum에 반영한다.
	{
		file.SetPosition( 4, STREAM_BEGIN);			// ID분만큼 스킵

		rc = file.Read( &head, sizeof(head));
		if( rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "");
			return false;
		}

		head.m_ChecksumCode[0] = 0xFFFFFFFF;
		head.m_ChecksumCode[1] = 0xFFFFFFFF;
		head.m_ChecksumCode[2] = 0xFFFFFFFF;
		head.m_ChecksumCode[3] = 0xFFFFFFFF;

		i3MD5::HMAC_Update( &ctx, (char *) &head, sizeof(head));
	}

	// 실 파일의 내용에 대한 Checksum
	{
		UINT32 i, readsz;
		char buf[ READ_UNIT];

		for( i = (sizeof(head) + 4); i < Size;)
		{
			readsz = min( READ_UNIT, Size - i);

			rc = file.Read( buf, readsz);
			if( rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "");
				return false;
			}

			i3MD5::HMAC_Update( &ctx, buf, readsz);

			i += readsz;
		}
	}

	i3MD5::HMAC_Finish( &ctx, (char *) pCode);

	return true;
}

bool i3ResourceFile::SetProtectionCode( const char * pszPath, const char * pKey, INT32 length)
{
	i3FileStream file;
	i3::pack::RESOURCE_FILE_HEADER2 head;
	UINT32 rc, code[4];

	if( _CalcFileChecksum( pszPath, pKey, length, code) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일에 대한 Encription Code를 생성할 수 없습니다.", pszPath);
		return false;
	}

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD | STREAM_WRITE | STREAM_SHAREWRITE) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일을 열 수 없습니다.");
		return false;
	}

	// Head에 Checksum Code를 기록하고 저장
	{
		file.SetPosition( 4, STREAM_BEGIN);			// ID 분만큼 스킵

		rc = file.Read( &head, sizeof(head));
		if( rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "");
			return false;
		}
	}

	{
		head.m_ChecksumCode[0] = code[0];
		head.m_ChecksumCode[1] = code[1];
		head.m_ChecksumCode[2] = code[2];
		head.m_ChecksumCode[3] = code[3];

		file.SetPosition( 4, STREAM_BEGIN);		// ID분 만큼 스킵
		rc = file.Write( &head, sizeof(head));
		if( rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "");
			return false;
		}
	}

	file.Close();
	
	I3TRACE( "RES ENCRIPT : %s -> %08X %08X %08X %08X\n", pszPath, head.m_ChecksumCode[0], head.m_ChecksumCode[1], head.m_ChecksumCode[2], head.m_ChecksumCode[3]);

	return true;
}

bool i3ResourceFile::CheckProtectionCode( const char * pszPath, const char * pKey, INT32 length)
{
	i3FileStream file;
	UINT32 rc;
	UINT32 code[4];
	i3::pack::RESOURCE_FILE_HEADER2 head;

	// 파일에 대한 Code를 구한다.
	if( _CalcFileChecksum( pszPath, pKey, length, code) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일에 대한 Encription Code를 생성할 수 없습니다.", pszPath);
		return false;
	}

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		I3PRINTLOG(I3LOG_FATAL,  "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	file.SetPosition( 4, STREAM_BEGIN);				// ID

	rc = file.Read( &head, sizeof(head));
	if( rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "");
		return false;
	}

	file.Close();

	if( (head.m_ChecksumCode[0] != code[0]) || 
		(head.m_ChecksumCode[1] != code[1]) || 
		(head.m_ChecksumCode[2] != code[2]) || 
		(head.m_ChecksumCode[3] != code[3]))
	{
		return false;
	}

	return true;
}

void i3ResourceFile::setExternRefResolveCallback( I3_EXTERNALREF_RESOLVE pUserProc, void * pUserData)
{
	s_pExternRefResolveProc = pUserProc;
	s_pExternRefResolveUserData = pUserData;
}

I3_EXTERNALREF_RESOLVE i3ResourceFile::getExternRefResolveCallback(void)
{
	return s_pExternRefResolveProc;
}

void i3ResourceFile::setSearchExternRefEnable( bool bFlag)
{
	s_bSearchMissingFile = bFlag;
}


i3PersistantElement * i3ResourceFile::ResolveExtern( INT32 objIdx)
{
	if( _isExtern( (OBJREF) m_pObjInfo[ objIdx].m_InstanceID) )
	{
		i3PersistantElement * pObj = _ResolveExtern( objIdx, false);

		if( pObj != nullptr)
		{
			pObj->SetExternRes(true);
			I3_MUST_ADDREF( pObj);		// 대입전에 AddRef()가 필요함..(2012.09.17.수빈)
			I3ASSERT(m_ObjList[objIdx] == nullptr);	// 만약 이게 널이 아닐수 있다면, Release()처리로 바꿔야함..(2012.09.17.수빈)

			return m_ObjList[ objIdx] = pObj;		// nullptr일 수 있다.
		}
	}

	return nullptr;
}

void	i3ResourceFile::RemoveAllResourceList()
{
	_ResourceManagerTerminateProc(nullptr);
}