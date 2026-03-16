#include "i3CommonType.h"
#include "i3ResourceFile.h"
#include "i3Debug.h"
#include "i3ErrorHandler.h"
#include "i3Memory.h"
#include "i3Macro.h"
#include "i3String.h"
#include "i3BinList.h"
#include "i3NamedElement.h"
#include "i3ResourceObject.h"
#include "i3Performance.h"
#include "i3InstanceProxy.h"
#include "i3MD5.h"
#include <stdio.h>

#if defined( I3_WINDOWS)
#include "i3DefExternRefResolveWin32.h"
#endif

#include "itl/vector_util.h"

// ================================================================================================
//
// i3ResourceFile
//
// ================================================================================================
#define I3RF_ALIGN_UNIT			1

static i3BinList *				s_pResManagerList = NULL;
static char						s_szWorkingFolder[MAX_PATH] = "\x0";
static bool						s_bProfEnable = false;
static I3_EXTERNALREF_REPORT	s_pExternRefCallbackProc = NULL;
static void *					s_pExternRefCallbackUserData = NULL;
bool							s_bSearchMissingFile = false;
#if defined( I3_WINDOWS)
	static I3_EXTERNALREF_RESOLVE	s_pExternRefResolveProc = _DefExternResolveProcWin32;
#else
	static I3_EXTERNALREF_RESOLVE	s_pExternRefResolveProc = NULL;
#endif
static void *					s_pExternRefResolveUserData = NULL;

#define		__PF_S( a)			if( s_bProfEnable) { __SP( a); }
#define		__PF_E( a)			if( s_bProfEnable) { __EP( a); }

I3_CLASS_INSTANCE( i3ResourceFile, i3ElementBase);

i3ResourceFile::i3ResourceFile(void)
{
	m_pStream = NULL;

	m_pUserProgressProc = NULL;
	m_pUserData = NULL;

	m_StrTable.SetOnceMode( TRUE);

	m_pObjInfo = NULL;
	m_pIDTable = NULL;
	m_pKeyObject = NULL;
	m_bOverwriteExternalRes = false;

	m_szFileName[0] = 0;

	i3mem::FillZero( &m_Header, sizeof( m_Header));
	m_IDCount = 0;

	m_bBackgroundLoad = false;
}

i3ResourceFile::~i3ResourceFile(void)
{
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

void i3ResourceFile::SetObjects( i3::vector<i3PersistantElement*>& ObjList)
{
	m_ObjList = ObjList;
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
		idx = _getRefOrdinal( InstanceID);

		I3_BOUNDCHK( idx, INT32(m_ObjList.size()));

		return m_ObjList[idx];
	}

	if( m_ObjList.size() <= 0)
		return NULL;

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
			
			I3ASSERT( pObj != NULL);
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

	return NULL;
}

i3PersistantElement * i3ResourceFile::FindObjectByMeta( i3ClassMeta * pMeta)
{
	i3PersistantElement * pObj;

	for(size_t i = 0; i < m_ObjList.size(); i++)
	{
		pObj = m_ObjList[i];

		if( pObj->IsTypeOf( pMeta))
			return pObj;
	}

	return NULL;
}

OBJREF				i3ResourceFile::GetObjectPersistID( i3PersistantElement * pObj)
{
	OBJREF ref;
	bool bNoname = false;

	if( pObj == NULL)
		return 0;

	if( pObj->IsExternRes())
	{
		INT32 idxStr, idx;
		char szName[256];

		if( pObj->IsTypeOf( i3NamedElement::GetClassMeta()))
		{
			i3NamedElement * pElm = (i3NamedElement *) pObj;

			if( pElm->hasName())
				strcpy( szName, pElm->GetName());
			else
				bNoname = true;
		}
		else
			bNoname = true;

		if( bNoname)
		{
			sprintf( (PSTR) szName, (PSTR) "%s_%08X", pObj->GetMeta()->GetClassName(), (UINT32)pObj);
		}

		// 이름을 String Table에 등록한다.
		idxStr = m_StrTable.IndexOf( szName);
		I3_BOUNDCHK( idxStr, 0x7FFF);

		idx = i3::vu::index_of(m_ObjList, pObj);
		if (idx == INT32(m_ObjList.size()) ) idx = 0;
		else idx += 1;

		I3_BOUNDCHK( idx, 0xFFFF);

		ref = 0x80000000 | ((0x00007FFF & idxStr) << 16) | (0x0000FFFF & idx);
	}
	else
	{
		INT32 obj_idx = i3::vu::index_of(m_ObjList, pObj);
		if (obj_idx == INT32(m_ObjList.size()) ) obj_idx = -1;

		ref = (OBJREF) obj_idx + 1;
	}

	return ref;
}

//#define I3_DEBUG_LOADTEST

i3PersistantElement *	i3ResourceFile::_ResolveExtern( INT32 i)
{
	char * pszName, szFull[ MAX_PATH], * pszClassName;
	i3ClassMeta * pMeta;
	INT32 idx;
	i3PersistantElement * pObj;
	i3NamedResourceManager * pResManager;
	BOOL Rv;
	UINT32 Rc;
	bool bExit = true;
	char szPath[ MAX_PATH];

	// 7603 ms
	idx = _getRefStringIndex( (OBJREF) m_pObjInfo[i].m_InstanceID);

	pszName = m_StrTable.Items[idx];
	pszClassName = m_StrTable.Items[ m_pObjInfo[i].m_ClassNameIndex];

	pMeta = i3ClassMeta::FindClassMetaByName( pszClassName);

	pResManager = GetResourceManager( pMeta);

	i3String::Copy( szPath, pszName, MAX_PATH );

	do
	{
		if( pResManager != NULL)
		{
			pObj = (i3PersistantElement *) pResManager->GetResource( m_szFileName, s_szWorkingFolder, szPath);
		}
		else
		{
			i3FileStream stream;

			if( s_szWorkingFolder[0] != 0)
				sprintf( szFull, "%s/%s", s_szWorkingFolder, szPath);
			else
				strcpy( szFull, szPath);

			Rv = stream.Open( szFull, STREAM_READ | STREAM_SHAREREAD);
			if( Rv == FALSE)
			{
				I3FATAL( "Could not load %s external referenced file.", szFull);
				return NULL;
			}

			stream.AddRef();
			SetStream( &stream);

			if( m_bBackgroundLoad == false)
			{
				pObj = (i3PersistantElement *) CREATEINSTANCE( pMeta);
			}
			else
			{
				pObj = (i3PersistantElement *) CREATEINSTANCE_TYPE( pMeta, I3_CLASSMETA_TYPE_GHOST);
			}

			Rc = pObj->OnLoad( this);
			if( Rc == STREAM_ERR)
			{
				I3FATAL( "Could not load %s external class object information", pObj->GetMeta()->GetClassName());
				return NULL;
			}

			if( pObj->IsTypeOf( i3NamedElement::GetClassMeta()))
			{
				((i3NamedElement *) pObj)->SetName( szPath);
			}
			
			stream.Close();
		}

		if( pObj != NULL)
		{
			if( s_pExternRefCallbackProc != NULL)
				s_pExternRefCallbackProc( pObj, pszName, s_pExternRefCallbackUserData);

			bExit = true;
		}
		else
		{
			I3NOTICE("Can not find external resource file %s from %s", pszName, m_szFileName);
//#if defined(I3_DEBUG)
			//I3FATAL("Can not find external resource file %s from %s", pszName, m_szFileName);
//#endif

			if( getExternRefResolveCallback() != NULL)
			{
				bExit = false;

				char szTemp[MAX_PATH];

				bool rv = getExternRefResolveCallback()( pszName, szTemp, sizeof(szTemp), s_szWorkingFolder, s_bSearchMissingFile);

				if( rv == false)
					bExit = true;
				else
				{
					i3String::MakeRelativePath( s_szWorkingFolder, szTemp, szPath, MAX_PATH);
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
	char * pszName, szFull[ MAX_PATH], * pszClassName;
	BOOL Rv;
	UINT32 Rc;

	idx = (INT32)(m_pObjInfo[i].m_InstanceID & 0x0000FFFF);
	pszName = m_StrTable.Items[idx];
	pszClassName = m_StrTable.Items[ m_pObjInfo[i].m_ClassNameIndex];

	pMeta = i3ClassMeta::FindClassMetaByName( pszClassName);
	I3ASSERT( pMeta != NULL);

	pResManager = GetResourceManager( pMeta);
	if( pResManager != NULL)
	{
		pObj->SetExternRes( FALSE);
		Rc = pResManager->SaveResource( s_szWorkingFolder, pObj); 
		pObj->SetExternRes( TRUE);

		if( Rc == STREAM_ERR)
		{
			I3FATAL( "");
			return STREAM_ERR;
		}
	}
	else
	{
		if( s_szWorkingFolder[0] != 0)
		{
			sprintf( szFull, "%s/%s", s_szWorkingFolder, pszName);
		}
		else
		{
			strcpy( szFull, pszName);
		}
	
		Rv = stream.Create( szFull, STREAM_WRITE);
		if( Rv == FALSE)
		{
			i3Log( "i3ResourceFile::Save()", "Could not create %s external object file.", szFull);
			return STREAM_ERR;
		}

		SetStream( &stream);

		Rc = pObj->OnSave( this);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3ResourceFile::Save()", "Could not write %s external class object.", pMeta->GetClassName());
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

	m_StrTable.Clear();
	m_ObjList.clear();

	{
		UINT8	id[4];

		Rc = pStream->Read( id, 4);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not read file-identifier.\n");
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
			I3_RESOURCE_FILE_HEADER1 OldHeader;

			Rc = pStream->Read( &OldHeader, sizeof( OldHeader));
			if( Rc == STREAM_ERR)
			{
				I3FATAL( "Could not read header(version 1.0f) information.\n");
				Result = STREAM_ERR;
				goto ExitPart;
			}
			Result += Rc;

			memset( &m_Header, 0, sizeof(m_Header));

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
			Rc = pStream->Read( &m_Header, sizeof( I3_RESOURCE_FILE_HEADER2));
			if( Rc == STREAM_ERR)
			{
				I3FATAL( "Could not read header information.");
				Result = STREAM_ERR;
				goto ExitPart;
			}
			Result += Rc;
		}
	}

	// String Table
	{
		char * pTemp;		

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_StringTableOffset), STREAM_BEGIN);

		m_StrTable.Clear();
		
		pTemp = (char *) i3MemAlloc( (INT32)m_Header.m_StringTableSize);
		I3ASSERT( pTemp != NULL);

		Rc = pStream->Read( pTemp, (INT32)m_Header.m_StringTableSize);
		if( Rc == STREAM_ERR)
		{
			I3MEM_SAFE_FREE_POINTER( pTemp);

			i3Log( "i3ResourceFile::Load()", "Could not read strign table.");
			Result = STREAM_ERR;
			goto ExitPart;
		}
		Result += Rc;

		m_StrTable.CopyFromBuffer( pTemp);

		I3MEM_SAFE_FREE_POINTER( pTemp ); 
	}
	
	// Object Directory
	{		
		UINT32 ObjInfoSz = sizeof( I3_RESOURCE_FILE_OBJECT_INFO2) * m_Header.m_ObjectCount;

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_ObjectDirOffset), STREAM_BEGIN);

		m_pObjInfo  = (I3_RESOURCE_FILE_OBJECT_INFO2 *) i3MemAlloc( ObjInfoSz);
		I3ASSERT( m_pObjInfo != NULL);

		m_pIDTable = (I3_RES_OBJECT_ID_INFO *) i3MemAlloc( sizeof(I3_RES_OBJECT_ID_INFO) * m_Header.m_ObjectCount);

		if( bOldVersion )
		{
			I3_RESOURCE_FILE_OBJECT_INFO1	info;

			memset( m_pObjInfo, 0, ObjInfoSz);

			for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
			{
				Rc = pStream->Read( &info, sizeof(info));
				if( Rc == STREAM_ERR)
				{
					I3FATAL( "Could not read object directory information.");
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
				I3FATAL( "Could not read object directory information.");
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

UINT32 i3ResourceFile::Load( i3Stream * pStream)
{
	INT32 i;
	UINT32 Result = STREAM_ERR, Rc;
	i3PersistantElement * pObj;
	UINT32	BasePos = pStream->GetPosition();
	//i3ClassMeta *pi3NameElementMeta = NULL;
	i3NamedResourceManager * pResManager;

	if( LoadHeader( pStream) == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Object Preparation
	//
	// Loading 시에 다른 Object에대한 Reference가 발생했을 때, 올바르게 검색할 수 있도록 하기
	// 위해, 모든 Object를 먼저 생성하여 List에 등록해 둔다.

	//	m_ObjList.SetCount( (INT32) m_Header.m_ObjectCount);
	m_ObjList.resize(m_Header.m_ObjectCount);

	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		pObj = NULL;
		pResManager = NULL;

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID) == false)
		{
			if( m_pObjInfo[i].m_ClassNameIndex >= (UINT32) m_StrTable.GetCount() )
			{
				I3ASSERT_0;
			}
			else
			{
				char * pszClassName = m_StrTable.Items[ m_pObjInfo[i].m_ClassNameIndex];
				i3ClassMeta * pMeta;

				// find class meta by name
				pMeta = i3ClassMeta::FindClassMetaByName( pszClassName);
				if( pMeta == NULL)
				{
					I3WARN("[%s] Unknown Class Meta .(%s).", getFileName(), pszClassName); 

					m_pObjInfo[i].m_ClassNameIndex = 0xFFFF;
				}
				else
				{
					// create a instance of specified class
					//I3TRACE( "1 : %s\n", pMeta->GetClassName());

					if( m_bBackgroundLoad == false)
					{
						pObj = (i3PersistantElement *) CREATEINSTANCE( pMeta);
					}
					else
					{
						pObj = (i3PersistantElement *) CREATEINSTANCE_TYPE( pMeta, I3_CLASSMETA_TYPE_GHOST);
					}
					
					if( pObj == NULL )
					{
						DebugBox( NULL, "i3ResourceFile::Load() - CreateInstance failed. (%s) : It could be non-ConcreteClass.", pszClassName);
						I3ASSERT_0;
					}
				}
			}
		}

		// External Referenced File은 이 시점에서는 단순히 NULL로 설정해둔다.

		m_ObjList[i] = pObj;		// NULL일 수 있다.
	}

	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion < 8))
	{
		// 1.8 이전의 경우에는 ID Table을 준비하여야 한다.
		_BuildIDTable( m_Header.m_ObjectCount);
	}

	//pi3NameElementMeta = i3NamedElement::GetClassMeta();

	// 2008.09.23
	// ExternRef 파일들은 종속성이 없기 때문에 선행 로드를 해두어
	// 여타 오브젝트에서의 디펜던시 문제가 발생하지 않도록 해준다. 
	// 만약 아래와 같이 ExternRef 파일들을 선행 로드하지 않으면
	// OnBuildObjectList에서 리스트에서 추가하는 순서에 제약이 발생한다.
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		if( m_pObjInfo[i].m_ClassNameIndex == 0xFFFF)
			continue;

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID))
		{
			pObj = _ResolveExtern( i);

			SetStream( pStream);

			if( pObj != NULL)
			{
				pObj->SetExternRes( TRUE);

				m_ObjList[i] = pObj;		// NULL일 수 있다.
			}
		}
	}


	// Objects
	for( i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
	{
		if( m_pObjInfo[i].m_ClassNameIndex == 0xFFFF)
			continue;

		if( m_pUserProgressProc != NULL)
		{
			m_pUserProgressProc( m_pUserData, i, m_Header.m_ObjectCount);
		}

		pObj = m_ObjList[i];

		if( _isExtern( (OBJREF) m_pObjInfo[i].m_InstanceID))
		{
			// External Object

			//I3TRACE( "EXTERN[%x] : %08X\n", i, m_pObjInfo[i].m_InstanceID);
		}
		else
		{
			// set stream position
			pStream->SetPosition( (INT32)(BasePos + m_pObjInfo[i].m_Offset), STREAM_BEGIN);

			#if defined( I3_DEBUG_LOADTEST)
				I3TRACE( "LOAD : %s start\n", pObj->GetMeta()->GetClassName());
			#endif

			if( pObj->IsLoaded())
				continue;

			//I3TRACE( "LOADING : %s...", pObj->GetMeta()->GetClassName());

			Rc = pObj->OnLoad( this);
			if( Rc == STREAM_ERR)
			{
				I3TRACE( "Could not load object data [%s]", pObj->GetMeta()->GetClassName());
				goto ExitPart;
			}

			if( pObj->IsExactTypeOf( i3InstanceProxy::GetClassMeta()))
			{
				i3InstanceProxy * pInstProxy = (i3InstanceProxy *) pObj;

				// Instance Proxy Class가 생성한 Instance로 대체...
				m_ObjList[i] = pInstProxy->getInstance();

				// Instance Proxy는 제거..
				pObj->AddRef();
				pObj->Release();
			}

			DTRACE( "Done\n");
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
	
	for( i = 0; i < INT32(m_ObjList.size()) ; i++)
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

	if( m_pUserProgressProc != NULL)
	{
		m_pUserProgressProc( m_pUserData, m_Header.m_ObjectCount, m_Header.m_ObjectCount);
	}

	Result = GetAlignedPos( pStream->GetPosition() - BasePos);

ExitPart:

	return Result;
}

UINT32 i3ResourceFile::Save( i3Stream * pStream)
{
	INT32 i;
	UINT32 Result = 0, Rc = STREAM_ERR, ObjInfoSz, BasePos = pStream->GetPosition();
	i3PersistantElement * pObj;
	BOOL bOldState = FALSE;

	SetStream( pStream);

//	m_ObjList.SetOnceMode();

	{
		//		i3List TempList;
		//		TempList.AddFromList( &m_ObjList);
		i3::vector<i3PersistantElement*> TempList(m_ObjList.begin(), m_ObjList.end());

		if( m_pKeyObject != NULL)
		{
			if( i3::find(TempList.begin(), TempList.end(), m_pKeyObject) == TempList.end() )
				TempList.push_back( m_pKeyObject);

			bOldState = m_pKeyObject->IsExternRes();
			m_pKeyObject->SetExternRes( FALSE);
		}

		m_ObjList.clear();

		for( i = 0; i < INT32(TempList.size()); i++)
		{
			pObj = TempList[i];

			pObj->OnBuildObjectList( m_ObjList);
		}

		i3::vu::remove_duplicates(m_ObjList);

	}

	// External Reference  되는 Object는 제거한다.
	m_StrTable.Clear();

	for( i = 0; i < INT32(m_ObjList.size()); i++)
	{
		bool bAdd;

		bAdd = false;

		pObj = m_ObjList[i];

		if( pObj->IsExternRes())
		{
			if( pObj->IsTypeOf( i3NamedElement::GetClassMeta()) == FALSE)
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

				sprintf( (PSTR) szName, (PSTR) "%s_%08X", pObj->GetMeta()->GetClassName(), (UINT32) pObj);

				m_StrTable.Add( szName);
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

			pObj->OnQueryStrings( & m_StrTable);
		}
	}


	// header information
	{
		Rc = pStream->Write( I3_RSCFILE_HEAD_20, 4);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write I3R2 file header.");
			goto ExitPart;
		}

		i3mem::FillZero( &m_Header, sizeof(m_Header));
		m_Header.m_MajorVersion = 1;
		m_Header.m_MinorVersion = 8;

		m_Header.m_StringTableCount = (UINT32) m_StrTable.GetCount();
		m_Header.m_StringTableOffset = (UINT64) GetAlignedPos( sizeof( I3_RESOURCE_FILE_HEADER2));
		m_Header.m_StringTableSize =  (UINT64) m_StrTable.GetBufferSize();

		m_Header.m_ObjectCount = (UINT32) m_ObjList.size();
		m_Header.m_ObjectDirOffset = GetAlignedPos( (UINT32)(m_Header.m_StringTableOffset + m_Header.m_StringTableSize));
		ObjInfoSz = sizeof( I3_RESOURCE_FILE_OBJECT_INFO2) * m_Header.m_ObjectCount;
		m_Header.m_ObjectDirSize = ObjInfoSz;
		m_Header.m_pKeyObject = (OBJREF64) GetObjectPersistID( getKeyObject());

		//파일 저장 시 LocaleTime 기록 하지 않도록 합니다.
		//SYSTEMTIME systime;
		//i3mem::FillZero( &systime, sizeof(SYSTEMTIME));
		//GetLocalTime( &systime);

		//FILETIME ftime;
		//i3mem::FillZero( &ftime, sizeof(FILETIME));
		//SystemTimeToFileTime( &systime, &ftime);

		//m_Header.pad[LOW_DATA_TIME] = (UINT32)ftime.dwLowDateTime; 
		//m_Header.pad[HIGH_DATA_TIME] = (UINT32)ftime.dwHighDateTime;

		Rc = pStream->Write( &m_Header, sizeof(I3_RESOURCE_FILE_HEADER2));
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write I3R file header.");
			goto ExitPart;
		}

		Result += Rc;
	}

	// String Table
	if( m_Header.m_StringTableSize > 0)
	{
		char * pTemp;		
		pStream->SetPosition( (INT32)(BasePos + m_Header.m_StringTableOffset), STREAM_BEGIN);

		pTemp = (char *) i3MemAlloc( (INT32)(m_Header.m_StringTableSize) );
		I3ASSERT( pTemp != NULL);

		m_StrTable.CopyToBuffer( pTemp, (INT32)m_Header.m_StringTableSize);

		Rc = pStream->Write( pTemp, (INT32) m_Header.m_StringTableSize);
		if( Rc == STREAM_ERR)
		{
			i3MemFree( pTemp );
			I3FATAL( "Could not write string table information.");
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
		m_pObjInfo = (I3_RESOURCE_FILE_OBJECT_INFO2 *) i3MemAlloc( ObjInfoSz);

		Rc = pStream->Write( m_pObjInfo, ObjInfoSz);
		if( Rc == STREAM_ERR)
		{
			I3FATAL( "Could not write object directory.");
			goto ExitPart;
		}
	}

	Align( pStream);

	// Objects
	for( i = 0; i < INT32(m_ObjList.size()); i++)
	{
		i3ClassMeta * pMeta;

		if( m_pUserProgressProc != NULL)
		{
			m_pUserProgressProc( m_pUserData, i, INT32(m_ObjList.size()));
		}

		pObj = m_ObjList[i];

		pMeta = pObj->GetMeta();

		m_pObjInfo[i].m_ClassNameIndex = (UINT16) m_StrTable.IndexOf( pMeta->GetPersistClassName());
		m_pObjInfo[i].m_Offset = pStream->GetPosition() - BasePos;
		m_pObjInfo[i].m_InstanceID = (UINT32) GetObjectPersistID( pObj);

		if( pObj->IsExternRes())
		{
			// External Reference
			if( m_bOverwriteExternalRes )
			{
				I3ASSERT( pObj->IsTypeOf( i3ResourceObject::GetClassMeta()));

				if( _SaveExtern( i, (i3ResourceObject *) pObj) == STREAM_ERR)
				{
					I3FATAL( "");
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
				I3FATAL( "Could not write %s class object.", pMeta->GetClassName());				
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
			I3FATAL( "Could not update object directory information.");
			goto ExitPart;
		}
	}

	if( m_pUserProgressProc != NULL)
	{
		m_pUserProgressProc( m_pUserData, INT32(m_ObjList.size()), INT32(m_ObjList.size()) );
	}

ExitPart:

	if( Rc == STREAM_ERR)
		Result = STREAM_ERR;

	if( m_pKeyObject != NULL)
	{
		m_pKeyObject->SetExternRes( bOldState);
	}

	// Termination
	I3MEM_SAFE_FREE_POINTER( m_pObjInfo );		

	return Result;
}

UINT32 i3ResourceFile::Load( const char * pszPath)
{
	i3FileStream Stream;
	BOOL Rv;
	UINT32 Result;

	i3String::Copy( m_szFileName, pszPath, MAX_PATH );

	m_pStream = &Stream;
	m_pStream->AddRef();			// 혹시라도 RefCount가 0인 것이 문제가 될 수 있기 때문에...
	
	Rv = m_pStream->Open( pszPath, STREAM_READ | STREAM_SHAREREAD); 
	if( Rv == FALSE)
	{
		i3Log( "i3ResourceFile::Load()", "Could not open %s file.", pszPath);
		return STREAM_ERR;
	}

	Result = Load( m_pStream);

	m_pStream->Close();

	m_pStream = NULL;

	return Result;
}

UINT32 i3ResourceFile::Save( const char * pszPath)
{
	i3FileStream Stream;
	BOOL Rv;
	UINT32 Result;

	i3String::Copy( m_szFileName, pszPath, MAX_PATH );
	m_pStream = &Stream;

	Rv = m_pStream->Create( pszPath, STREAM_WRITE | STREAM_READ);
	if( Rv == FALSE)
	{
		i3Log( "i3ResourceFile::Save()", "Could not create %s file.", pszPath);
		return STREAM_ERR;
	}

	Result = Save( m_pStream);

	m_pStream->Close();

	m_pStream = NULL;

	return Result;
}

///////////////////////////////////////////////////////////////////////////////////////////
// 
static BOOL _ResourceManagerTerminateProc( void * pData)
{
	if( s_pResManagerList != NULL)
	{
		INT32 i;
		I3_RESOURCE_MANAGER_INFO * pInfo;

		for( i = 0; i < s_pResManagerList->GetCount(); i++)
		{
			pInfo = (I3_RESOURCE_MANAGER_INFO *) s_pResManagerList->Items[i];

			I3_SAFE_RELEASE( pInfo->m_pManager);
			I3MEM_SAFE_FREE( pInfo);
		}

		I3_SAFE_RELEASE( s_pResManagerList);
	}

	return TRUE;
}

static INT32 _ResManagerCompProc( I3_RESOURCE_MANAGER_INFO * p1, I3_RESOURCE_MANAGER_INFO * p2)
{
	return p1->m_pMeta - p2->m_pMeta;
}

void	i3ResourceFile::AddResourceManager( i3ClassMeta * pMeta, i3NamedResourceManager * pResMnger)
{
	I3_RESOURCE_MANAGER_INFO * pNew;	

	if( s_pResManagerList == NULL)
	{
		s_pResManagerList = i3BinList::NewObject();

		// 종료함수 추가.
		i3System::AddTerminateProc( 9, _ResourceManagerTerminateProc, NULL);
	}

	s_pResManagerList->SetCompareProc( (COMPAREPROC) _ResManagerCompProc);

	//INT32 hMem;	
	//hMem = i3MemAlloc(sizeof(I3_RESOURCE_MANAGER_INFO)); 
	pNew = (I3_RESOURCE_MANAGER_INFO *) i3MemAlloc(sizeof(I3_RESOURCE_MANAGER_INFO));

	memset( pNew, 0, sizeof( I3_RESOURCE_MANAGER_INFO));
	pNew->m_pMeta = pMeta;
	pNew->m_pManager = pResMnger;

	pResMnger->AddRef();

	s_pResManagerList->Add( pNew);
}

void	i3ResourceFile::RemoveResourceManager( i3ClassMeta * pMeta)
{
	I3_RESOURCE_MANAGER_INFO temp, * pFind;
	INT32 idx;

	I3ASSERT( s_pResManagerList != NULL );

	s_pResManagerList->SetCompareProc( (COMPAREPROC) _ResManagerCompProc);

	temp.m_pMeta = pMeta;

	idx = s_pResManagerList->FindItem( &temp);
	if( idx != -1)
	{
		pFind = (I3_RESOURCE_MANAGER_INFO *) s_pResManagerList->Items[idx];

		pFind->m_pManager->Release();
		i3MemFree( pFind);

		s_pResManagerList->Delete( idx);

		if( s_pResManagerList->GetCount() == 0 )
		{
			I3_SAFE_RELEASE( s_pResManagerList );
		}
	}
}

i3NamedResourceManager * i3ResourceFile::GetResourceManager( i3ClassMeta * pMeta)
{
	I3_RESOURCE_MANAGER_INFO  * pFind;
	INT32 i;

	if( s_pResManagerList == NULL)
		return NULL;

	for( i = 0; i < s_pResManagerList->GetCount(); i++)
	{
		pFind = (I3_RESOURCE_MANAGER_INFO *) s_pResManagerList->Items[i];

		if( pMeta->IsTypeOf( pFind->m_pMeta))
		{
			return pFind->m_pManager;
		}
	}

	return NULL;
}

void i3ResourceFile::SetExternalRefCallback( I3_EXTERNALREF_REPORT pProc, void * pData)
{
	s_pExternRefCallbackProc = pProc;
	s_pExternRefCallbackUserData = pData;
}

void i3ResourceFile::CallExternRefCallback( i3ResourceObject * pObj, const char * pszName)
{
	if( s_pExternRefCallbackProc != NULL)
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
	i3String::Copy( s_szWorkingFolder, pszPath, MAX_PATH );
	i3String::NormalizePath( s_szWorkingFolder);
}

void	i3ResourceFile::setProfileEnable( bool bEnable)
{
	s_bProfEnable = bEnable;
}

#define		READ_UNIT			0x0000FFFF

static bool _CalcFileChecksum( const char * pszPath, const char * pKey, INT32 length, UINT32 * pCode)
{
	i3FileStream file;
	I3_RESOURCE_FILE_HEADER2 head;
	UINT32 rc, Size;
	md5_context ctx;

	// Start
	i3MD5::HMAC_Start( &ctx, (char *) pKey, length);

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3WARN( "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	Size = file.GetSize();

	// FileName을 Checksum에 추가
	{
		char szName[MAX_PATH];
		
		i3String::SplitFileName( pszPath, szName, TRUE);

		i3String::ToUpper( szName);

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
			I3FATAL( "");
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
				I3FATAL( "");
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
	I3_RESOURCE_FILE_HEADER2 head;
	UINT32 rc, code[4];

	if( _CalcFileChecksum( pszPath, pKey, length, code) == false)
	{
		I3WARN( "%s 파일에 대한 Encription Code를 생성할 수 없습니다.", pszPath);
		return false;
	}

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3WARN( "%s 파일을 열 수 없습니다.");
		return false;
	}

	// Head에 Checksum Code를 기록하고 저장
	{
		file.SetPosition( 4, STREAM_BEGIN);			// ID 분만큼 스킵

		rc = file.Read( &head, sizeof(head));
		if( rc == STREAM_ERR)
		{
			I3FATAL( "");
			return false;
		}
	}

	file.Close();

	if( file.Open( pszPath, STREAM_WRITE) == FALSE)
	{
		I3WARN( "%s 파일을 열 수 없습니다.");
		return false;
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
			I3FATAL( "");
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
	I3_RESOURCE_FILE_HEADER2 head;

	// 파일에 대한 Code를 구한다.
	if( _CalcFileChecksum( pszPath, pKey, length, code) == false)
	{
		I3WARN( "%s 파일에 대한 Encription Code를 생성할 수 없습니다.", pszPath);
		return false;
	}

	if( file.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		I3FATAL( "%s 파일을 열 수 없습니다.", pszPath);
		return false;
	}

	file.SetPosition( 4, STREAM_BEGIN);				// ID

	rc = file.Read( &head, sizeof(head));
	if( rc == STREAM_ERR)
	{
		I3FATAL( "");
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
