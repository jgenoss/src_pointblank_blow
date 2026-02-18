#include "stdafx.h"
#include "i3RscPartialFile.h"

#include "i3Base/pool/create_by_common_pool.h"
#include "i3Base/i3StringListBuffer.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/compare/generic_is_nequal.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/mb_to_utf16.h"

class i3RscPartialFile;

bool	Skip_i3NamedElement( i3Stream* stream);
bool	Read_i3NamedElement( i3Stream* stream, i3::rc_string& name);
bool	Read_i3TreeElement( i3Stream* stream, i3::vector<OBJREF>& childObjRefList, UINT32 StyleMask = 0xFFFFFFFF);
bool	Skip_i3TreeElement( i3Stream* stream, UINT32 StyleMask = 0xFFFFFFFF);
bool	CheckStyle_i3TreeElement( i3Stream* stream, UINT32 StyleMask );

bool	Read_i3RegKey( i3Stream* stream, i3::vector<OBJREF64>& data_objref_list);

struct i3RscPartialFileFindCallback
{
	virtual ~i3RscPartialFileFindCallback() {}
	virtual bool operator()( i3RscPartialFile* rscpf, i3Stream* stream) { return false; }
};


i3RscPartialFile::i3RscPartialFile() : m_pObjInfo(NULL), m_pIDTable(NULL), m_IDCount(0),
m_fnIsExtern(&i3RscPartialFile::isExtern_CurrV), m_fnGetExternStringIndex(&i3RscPartialFile::getExternStringIndex_CurrV),
m_stream_basepos(0), m_pKeyObject(NULL)
{

}

i3RscPartialFile::~i3RscPartialFile()
{

	i3::delete_array_by_common_pool(m_pObjInfo);
	i3::delete_array_by_common_pool(m_pIDTable);
}

bool i3RscPartialFile::Open(const char* pszPath)
{
	i3RscPartialFile::Close();

	if ( m_stream.Open(pszPath, STREAM_READ|STREAM_SHAREREAD) == FALSE)
	{
		i3Log("i3RscPartialFile::Load()", "Could not open %s File.", pszPath);
		return false;
	}

	return  i3RscPartialFile::Open(&m_stream);
}

bool i3RscPartialFile::Open(i3Stream* pStream)
{
	if (i3RscPartialFile::LoadHeader(pStream) == false)
		return false;

	// ID테이블..( 1.8버전 미만인 경우에만 넣는다고한다..)
	if (m_Header.m_MajorVersion >= 1 && m_Header.m_MinorVersion < 8)
		_BuildIDTable();

	return true;
}

void i3RscPartialFile::Close()
{
	m_stream.Close();
	
	::memset(&m_Header, 0, sizeof(I3_RESOURCE_FILE_HEAD_INFO));
	
	i3::delete_array_by_common_pool(m_pObjInfo);
	i3::delete_array_by_common_pool(m_pIDTable);
	m_pObjInfo = NULL;
	m_pIDTable = NULL;
	
	m_StrTable.clear();

	m_IDCount  = 0;
	m_stream_basepos = 0;
	m_pKeyObject = NULL;

	m_fnIsExtern = &i3RscPartialFile::isExtern_CurrV;
	m_fnGetExternStringIndex = &i3RscPartialFile::getExternStringIndex_CurrV;
}


bool i3RscPartialFile::LoadHeader(i3Stream* pStream)
{
	UINT32	BasePos = pStream->GetPosition();
	m_stream_basepos = BasePos;

	// 1. File ID 체크.
	UINT8 id[4];
	bool  bOldVersion = false;

	if ( pStream->Read( id, 4) == STREAM_ERR )
		return false;

	if (::memcmp(id, I3_RSCFILE_HEAD_10, 4) == 0)
		bOldVersion = true;

	// 2. Header Info .. ( 이건 i3ResourceFile의 헤더를 그대로 써도 좋을 것이다.)
	{
		if( bOldVersion)
		{
			I3_RESOURCE_FILE_HEADER1 OldHeader;

			if( pStream->Read( &OldHeader, sizeof( OldHeader)) == STREAM_ERR)
				return false;

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
			I3_RESOURCE_FILE_HEADER2 head;

			if( pStream->Read( &head, sizeof( I3_RESOURCE_FILE_HEADER2)) == STREAM_ERR)
				return false;

			m_Header.CopyFromFile( &head);
		}
	}

	//
	// 무기 pef의 경우 엄청나게 많은 노드를 갖고 있으므로...
	// 스트링테이블과 ObjInfo의 사이즈도 만만찮게 많은 상태이다...
	// 속도병목은 딱히 보이지 않기 때문에 아직 상관은 없지만...
	// 부분 로딩을 깨끗하게 추구하려면..이것도 다 읽어들이면 안될것 같다...
	//

	// String Table
	{

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_StringTableOffset), STREAM_BEGIN);

		m_StrTable.clear();

		char *  pTemp = i3::create_array_by_common_pool(i3::identity<char>(), (size_t)m_Header.m_StringTableSize);

		if( pStream->Read( pTemp, (INT32)m_Header.m_StringTableSize) == STREAM_ERR)
		{
			i3::delete_array_by_common_pool(pTemp);
			return false;
		}

		CopyFromBufferToStringList(m_StrTable, pTemp);

		i3::delete_array_by_common_pool(pTemp);
	}

	// Object Directory
	{		
		i3::delete_array_by_common_pool(m_pObjInfo);

		pStream->SetPosition( (INT32)(BasePos + m_Header.m_ObjectDirOffset), STREAM_BEGIN);

		m_pObjInfo  = i3::create_array_by_common_pool(i3::identity<I3_RESOURCE_FILE_OBJECT_INFO2>(), m_Header.m_ObjectCount);

		I3ASSERT( m_pObjInfo != NULL);


		if( bOldVersion )
		{
			I3_RESOURCE_FILE_OBJECT_INFO1	info;

			memset( m_pObjInfo, 0, sizeof( I3_RESOURCE_FILE_OBJECT_INFO2) * m_Header.m_ObjectCount );

			for(INT32 i = 0; i < (INT32) m_Header.m_ObjectCount; i++)
			{
				if( pStream->Read( &info, sizeof(info)) == STREAM_ERR)
				{
					return false;
				}

				m_pObjInfo[i].m_ClassNameIndex = info.m_ClassNameIndex;
				m_pObjInfo[i].m_InstanceID = info.m_InstanceID;
				m_pObjInfo[i].m_Offset = info.m_Offset;
				m_pObjInfo[i].m_Size = info.m_Size;
			}
		}
		else
		{
			if( pStream->Read( m_pObjInfo, 
				sizeof( I3_RESOURCE_FILE_OBJECT_INFO2) * m_Header.m_ObjectCount ) == STREAM_ERR)
			{
				return false;
			}
		}
	}

	if (m_Header.m_MajorVersion >= 1 && m_Header.m_MinorVersion < 8)
	{
		m_fnIsExtern = &i3RscPartialFile::isExtern_V18Below;
		m_fnGetExternStringIndex = &i3RscPartialFile::getExternStringIndex_V18Below;
	}

	if (m_Header.m_pKeyObject != 0)
		m_pKeyObject = i3RscPartialFile::FindObjectInfo((OBJREF)m_Header.m_pKeyObject);

	return true;
}

namespace
{
	void _Rec_QuickSort( I3_RES_OBJECT_ID_INFO * pTable, INT32 idxMin, INT32 idxMax)
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
}


void i3RscPartialFile::_BuildIDTable()
{
	INT32 count = m_Header.m_ObjectCount;

	i3::delete_array_by_common_pool(m_pIDTable);

	m_pIDTable = i3::create_array_by_common_pool(i3::identity<I3_RES_OBJECT_ID_INFO>(), count);

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

bool i3RscPartialFile::isExtern_V18Below(OBJREF ref)
{
	return (ref & 0xFFFF0000) == 0xFFFF0000;
}

bool i3RscPartialFile::isExtern_CurrV(OBJREF ref)
{
	return (ref & 0x80000000) != FALSE;
}

INT32 i3RscPartialFile::getExternStringIndex_V18Below(OBJREF ref)
{
	if ( !i3RscPartialFile::isExtern_V18Below(ref) ) return -1;
	return (ref & 0x0000FFFF);
}

INT32 i3RscPartialFile::getExternStringIndex_CurrV(OBJREF ref)
{
	if (!i3RscPartialFile::isExtern_CurrV(ref) ) return -1;
	return (ref & 0x7FFF0000) >> 16;
}


//	const char* szTargetClass = "i3RegKey";
i3ResourceFileObjectInfo2*	i3RscPartialFile::FindObjectInfo(const char* tgtClassName, i3RscPartialFileFindCallback* cb)
{
	const size_t num_obj = m_Header.m_ObjectCount;

	for (size_t i = 0 ; i < num_obj ; ++i )
	{
		if ( isExtern( (OBJREF)m_pObjInfo[i].m_InstanceID) )		// extern의 경우 일단 스킵..
			continue;

		i3::rc_string strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex ];

		if (strClassName != tgtClassName)
			continue;

		m_stream.SetPosition( INT32(m_stream_basepos + m_pObjInfo[i].m_Offset), STREAM_BEGIN);

		if ( (*cb)( this, &m_stream ) )
			return &m_pObjInfo[i];
	}

	return NULL;
}

i3ResourceFileObjectInfo2*	i3RscPartialFile::FindObjectInfo(OBJREF instanceID)
{
	INT32 idx, ordinal;

	// Version 1.7 이상부터 직접 Indexing으로 변경.
	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion >= 8))
	{
		// 1.8부터는 Extern Code에 대한 정의를 MSB만으로 줄인다.
		idx = _getRefOrdinal( instanceID);

		I3_BOUNDCHK( idx, INT32(m_Header.m_ObjectCount) );

		return &m_pObjInfo[idx];
	}

	if( m_Header.m_ObjectCount == 0)
		return NULL;

	INT32 start = 0;
	INT32 end = m_IDCount - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;

		if( m_pIDTable[idx].m_ID == instanceID)
		{
			// External Reference의 경우, 동일 ID가 있을 수 있다.
			//
			ordinal = m_pIDTable[idx].m_Ordinal;
			return &m_pObjInfo[ordinal];
		}

		if( instanceID < m_pIDTable[idx].m_ID)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;

		if( m_pIDTable[idx].m_ID == instanceID)
		{
			ordinal = m_pIDTable[idx].m_Ordinal;
			return &m_pObjInfo[ordinal];
		}
	}

	return NULL;

}

INT32	i3RscPartialFile::FindObjectInfoIndex(OBJREF instanceID)
{
	INT32 idx, ordinal;

	// Version 1.7 이상부터 직접 Indexing으로 변경.
	if( (m_Header.m_MajorVersion >= 1) && (m_Header.m_MinorVersion >= 8))
	{
		// 1.8부터는 Extern Code에 대한 정의를 MSB만으로 줄인다.
		idx = _getRefOrdinal( instanceID);
		I3_BOUNDCHK( idx, INT32(m_Header.m_ObjectCount) );
		return idx;
	}

	if( m_Header.m_ObjectCount == 0)
		return -1;

	INT32 start = 0;
	INT32 end = m_IDCount - 1;

	while( start < end)
	{
		idx = (start + end) >> 1;

		if( m_pIDTable[idx].m_ID == instanceID)
		{
			// External Reference의 경우, 동일 ID가 있을 수 있다.
			//
			ordinal = m_pIDTable[idx].m_Ordinal;
			return ordinal;
		}

		if( instanceID < m_pIDTable[idx].m_ID)
			end = idx - 1;
		else
			start = idx + 1;
	}

	if( start <= end)
	{	
		idx = (start + end) >> 1;

		if( m_pIDTable[idx].m_ID == instanceID)
		{
			ordinal = m_pIDTable[idx].m_Ordinal;
			return ordinal;
		}
	}

	return -1;
}



INT32 i3RscPartialFile::_getRefOrdinal( OBJREF ref)
{

	if ( ref & 0x80000000)		// 익스턴인 경우이다...
		return static_cast<INT32>(ref & 0x0000FFFF) - 1;
	
	return static_cast<INT32>(ref)-1;
//	return (ref & 0x0000FFFF) - 1;
}

namespace
{
#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

	typedef struct PACKED _tagi3NodePersistHeader2_0
	{
		INT8			m_ID[4];
		UINT32			m_Style;
		UINT32			m_State;
		INT32			m_nChildCount;
		UINT32			pad[15];
	} I3_PERSIST_TREENODE;

	typedef struct PACKED _tagi3RegPersistKey
	{
		UINT8	m_ID[4];
		INT32	m_nValueCount;
		UINT32	pad[2];
	}I3REG_PERSIST_KEY;

	typedef struct PACKED _tagi3RegPersistString
	{
		UINT8	m_ID[4];
		UINT32	m_nStrLength;
	}I3REG_PERSIST_STRING;


#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

}

bool	Skip_i3NamedElement( i3Stream* stream)
{
	UINT8 len;
	if (stream->Read(&len, sizeof(UINT8)) == STREAM_ERR)
		return false;

	stream->SetPosition(len, STREAM_CURRENT);		// 건너뛰기..
	return true;
}

bool	Read_i3NamedElement( i3Stream* stream, i3::rc_string& name)
{
	UINT8 len;
	if (stream->Read(&len, sizeof(UINT8)) == STREAM_ERR)
		return false;

	i3::rc_string str(len, 0);
	stream->Read(const_cast<char*>(&str[0]), len);
	name = str;

	return true;
}

bool	Read_i3RegString( i3Stream* stream, i3::rc_wstring& outVal)
{
	I3REG_PERSIST_STRING data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;

	if (data.m_nStrLength > 0 )
	{
		if (i3::generic_is_nequal((const char*)data.m_ID, "RGS3", 4) )
		{
			i3::rc_wstring wstr(data.m_nStrLength, 0);
			stream->Read(const_cast<wchar_t*>(&wstr[0]), data.m_nStrLength*sizeof(wchar_t) );
			
			// 이 부분은 반드시 갱신되어야 한다..일단 방치.. (2014.07.01.수빈.)
			outVal = wstr;
			//
		}
		else
		if (i3::generic_is_nequal((const char*)data.m_ID, "RGS2", 4) )
		{
			i3::stack_string str(data.m_nStrLength, 0);
			stream->Read(const_cast<char*>(&str[0]), data.m_nStrLength);
			
			i3::mb_to_utf16(str, outVal);
		}
		else
		{
			i3::stack_string str(data.m_nStrLength, 0);
			stream->Read(&str[0], data.m_nStrLength);
			i3::generic_string_replace_all(str, "\\n", "\n");

			i3::mb_to_utf16(str, outVal);
		}

	}

	return true;
}

bool Read_i3RegStringArrayElement(i3Stream* stream, size_t idx, i3::rc_wstring& outVal)
{
	stream->SetPosition( sizeof(UINT32), STREAM_CURRENT );	// ElementDataType

	UINT32 num;
	stream->Read( &num, sizeof(UINT32));

	if (num == 0) return false;
	if (num == 1) idx = 0;
	
	// 관심항목만 읽고 그냥 멈추도록 처리함..
	I3REG_PERSIST_STRING data;

	if ( idx > 0 )
	{
		stream->Read(&data, sizeof(I3REG_PERSIST_STRING) );
		
		size_t stride = sizeof(char);

		if (i3::generic_is_nequal((const char*)data.m_ID, "RGS3", 4) )
			stride = sizeof(wchar_t);

		stream->SetPosition(data.m_nStrLength * stride, STREAM_CURRENT);

		for (size_t i = 1 ; i < idx ; ++i )
		{
			stream->Read(&data, sizeof(I3REG_PERSIST_STRING) );
			stream->SetPosition(data.m_nStrLength * stride, STREAM_CURRENT);
		}
	}
	
	return Read_i3RegString(stream, outVal);
}

bool Read_i3RegIntArrayElement(i3Stream* stream, size_t idx, INT32& outVal)
{
	stream->SetPosition( sizeof(UINT32), STREAM_CURRENT );	// ElementDataType
	
	UINT32 num;
	stream->Read(&num, sizeof(UINT32));
	
	if (num == 0) return false;
	if (num == 1) idx = 0;

	// 여기서도 마찬가지로 관심항목까지 접근하고 처리하고 끝낸다..

	stream->SetPosition( sizeof(UINT32) * idx, STREAM_CURRENT);
	
	if ( stream->Read(&outVal, sizeof(INT32)) == STREAM_ERR)
		return false;

	return true;
}


namespace
{

#pragma pack( push, 4)

	typedef struct ALIGN4 _tagi3PersistGameNode
	{
		UINT8		m_ID[4];
		UINT32		m_GameNodeStyle;
		UINT32		m_State;
		OBJREF		m_pParent;

		REAL32		m_UpdateInterval;
		REAL32		m_LifeTime;
		UINT8		reserved[8];
	} I3_PERSIST_GAME_NODE;

	typedef struct ALIGN4 _tagi3PersistGameNodeOld
	{
		UINT32		m_State;
		OBJREF		m_pParent;
		REAL32		m_UpdateInterval;
		UINT8		reserved[8];
	} I3_PERSIST_GAME_NODE_OLD;

#pragma pack( pop)

}

bool	Read_i3GameNode( i3Stream* stream, OBJREF& parentObjRef)
{
	I3_PERSIST_GAME_NODE data;
	
	UINT8 id[4];

	if ( stream->Read(id, 4) == STREAM_ERR)
		return false;

	stream->SetPosition( -4, STREAM_CURRENT);

	if( memcmp( id, "GND1", 4) != 0)
	{
		I3_PERSIST_GAME_NODE_OLD oldData;

		if ( stream->Read( &oldData, sizeof(oldData)) == STREAM_ERR )
			return false;

		data.m_State = oldData.m_State;
		data.m_GameNodeStyle = 0;
		data.m_UpdateInterval = oldData.m_UpdateInterval;
		data.m_pParent = oldData.m_pParent;
	}
	else
	{
		if ( stream->Read( &data, sizeof(data)) == STREAM_ERR )
			return false;
	}	
	
	parentObjRef = data.m_pParent;
	
	return true;
}

bool	Skip_i3GameNode( i3Stream* stream)
{
	UINT8 id[4];

	if ( stream->Read(id, 4) == STREAM_ERR)
		return false;

	if( memcmp( id, "GND1", 4) != 0)
	{
		stream->SetPosition(sizeof(I3_PERSIST_GAME_NODE_OLD) - 4, STREAM_CURRENT);
	}
	else
	{
		stream->SetPosition(sizeof(I3_PERSIST_GAME_NODE) - 4, STREAM_CURRENT);
	}

	return true;
}


namespace
{

#pragma pack( push, 8)

typedef struct ALIGN8 _tagi3PersistAIContext
{
	UINT8		m_ID[4];
	UINT32		m_State;
	OBJREF64	m_pAI;
	OBJREF64	m_pInitialAIState;
	UINT32		pad[8];
} I3_PERSIST_AI_CONTEXT ALIGN8;

#pragma pack( pop)

}



bool	Read_i3AIContext( i3Stream* stream, OBJREF& ai_obj)
{
	I3_PERSIST_AI_CONTEXT data;
	
	if ( stream->Read( &data, sizeof(data)) == STREAM_ERR)
		return false;

	ai_obj = (OBJREF) data.m_pAI;
	
	return true;
}

bool    Read_GeneralChildList( i3Stream* stream, i3::vector<OBJREF>& childObjRefList)
{
	UINT32 cnt;

	if (stream->Read(&cnt, sizeof(cnt)) == STREAM_ERR)
		return false;
	
	childObjRefList.resize(cnt);

	if (cnt > 0)
	{
		if ( stream->Read(&childObjRefList[0], sizeof(OBJREF) * cnt) == STREAM_ERR)
			return false;
	}
	
	return true;
}


bool	Read_i3TreeElement( i3Stream* stream, i3::vector<OBJREF>& childObjRefList, UINT32 StyleMask)
{
	// i3TreeNode의 앞부분..
	I3_PERSIST_TREENODE data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;

	if ( StyleMask != 0xFFFFFFFF &&  StyleMask != (data.m_Style & StyleMask) )
		return false;

	if ( i3::generic_is_niequal(reinterpret_cast<const char*>(data.m_ID), "TRN2", 4) )
	{
		Read_GeneralChildList(stream, childObjRefList);
	}
	else
	{
		childObjRefList.resize(data.m_nChildCount);

		if (data.m_nChildCount > 0)
			stream->Read(&childObjRefList[0], sizeof(OBJREF) * data.m_nChildCount);
	}

	return true;
}

namespace
{

#pragma pack( push, 8)

	typedef struct _tagi3PersistAI
	{
		UINT8			m_ID[4];
		UINT32			m_Style;
		UINT32			m_LuaPathLength;
		UINT32			m_DriverMetaNameLength;
		UINT32			pad[5];
	} I3_PERSIST_AI;

#pragma pack( pop)

}


bool	Read_i3AI( i3Stream* stream, i3::vector<OBJREF>& state_list )
{
//	I3_PERSIST_AI data;
	
	stream->SetPosition(sizeof(I3_PERSIST_AI), STREAM_CURRENT);		// 관심사가 안되서 그냥 통째로 넘긴다..

	return Read_GeneralChildList(stream, state_list);
}

namespace
{

#pragma pack( push, 8)

	typedef struct ALIGN8 _tagi3PersistAIState
	{
		UINT8			m_sig[4];

		UINT32			m_Style;
		INT32			m_ID;

		OBJREF64		m_pAnim;
		REAL32			m_timeScale;
		REAL32			m_timeBlend;
		INT32			pad0;

		INT32			m_RunLuaFuncName;
		INT32			m_FinishLuaFuncName;

		REAL32			m_timeLength;

		OBJREF64		m_pNext;
		OBJREF64		m_pTimeSeq;

		UINT32			m_AnimNameLen;

		UINT32			pad[13];

	} I3_PERSIST_AI_STATE;

#pragma pack( pop)

}

bool	Read_i3AIState( i3Stream* stream, OBJREF& anim_obj_ref)
{
	I3_PERSIST_AI_STATE data;

	if (stream->Read(&data, sizeof(data)) == STREAM_ERR )
		return false;
	
	anim_obj_ref = (OBJREF) data.m_pAnim;

/*
	if ( data.m_AnimNameLen > 0)
	{
		i3::rc_string	str( data.m_AnimNameLen, 0);
		if ( stream->Read(const_cast<char*>(&str[0]), data.m_AnimNameLen) == STREAM_ERR)
			return false;

		anim_path = str;
	}
	else
	{
		anim_path.clear();
	}
*/

	return true;
}


bool	Skip_i3TreeElement( i3Stream* stream, UINT32 StyleMask)
{
	// i3TreeNode의 앞부분..
	I3_PERSIST_TREENODE data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;

	if ( StyleMask != 0xFFFFFFFF &&  StyleMask != (data.m_Style & StyleMask) )
		return false;

	if ( i3::generic_is_niequal(reinterpret_cast<const char*>(data.m_ID), "TRN2", 4) )
	{
		UINT32 cnt;
		if (stream->Read(&cnt, sizeof(cnt)) == STREAM_ERR)
			return false;

		stream->SetPosition(sizeof(OBJREF) * cnt, STREAM_CURRENT);
	}
	else
	{
		stream->SetPosition(sizeof(OBJREF) * data.m_nChildCount, STREAM_CURRENT);
	}

	return true;
}

bool	CheckStyle_i3TreeElement( i3Stream* stream, UINT32 StyleMask )
{
	// i3TreeNode의 앞부분..
	I3_PERSIST_TREENODE data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;

	if ( StyleMask != 0xFFFFFFFF &&  StyleMask != (data.m_Style & StyleMask) )
		return false;

	return true;
}

bool	Read_i3RegKey( i3Stream* stream, i3::vector<OBJREF64>& data_objref_list)
{
	I3REG_PERSIST_KEY data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;

	data_objref_list.resize(data.m_nValueCount);

	if (data.m_nValueCount > 0)
	{
		stream->Read(&data_objref_list[0], sizeof(OBJREF64) * data.m_nValueCount);	
	}

	return true;
}

INT32	Read_DataListCount_i3RegKey( i3Stream* stream )
{
	I3REG_PERSIST_KEY data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return 0;
	
	return data.m_nValueCount;
}

namespace
{
	
#pragma pack( push, 8)

	typedef struct _tagi3PersistStageInfo
	{
		UINT8			m_ID[4];
		char			m_szClassName[64];
		UINT32			m_colorFI;
		REAL32			m_timeFI;
		UINT32			m_colorFO;
		REAL32			m_timeFO;
		OBJREF			m_pNextStage;
		UINT32			m_JumpType;
		INT32			m_RSCBufferSize;
		INT32			m_nInstanceManagerInfoCount;
		UINT32			pad[15];

	} I3_PERSIST_STAGE_INFO;

#pragma pack( pop)


}

bool	Read_StageClassNamei3StageInfo(i3Stream* stream, i3::rc_string& val)
{
	I3_PERSIST_STAGE_INFO data;

	if ( stream->Read(&data, sizeof(data)) == STREAM_ERR)
		return false;
	
	val = data.m_szClassName;	
	return true;
}




i3ResourceFileObjectInfo2*		i3RscPartialFile::Findi3RegKeyRoot()
{

	struct i3RscPartialFileFindCallback_i3RegKey : i3RscPartialFileFindCallback
	{
		virtual bool operator()( i3RscPartialFile* rscpf, i3Stream* stream) 
		{ 
			// NamedElement에서..
			if (!Skip_i3NamedElement(stream) )
				return false;

			return CheckStyle_i3TreeElement(stream, I3REG_STYLE_ROOT);
		}

	} cb;

	i3ResourceFileObjectInfo2* res =  this->FindObjectInfo("i3RegKey", &cb);

	SkipNamei3NamedElement(res);

	return res;
}

bool		i3RscPartialFile::ReadNamei3NamedElement(const i3ResourceFileObjectInfo2* info, i3::rc_string& name)
{
	if ( isExtern( (OBJREF)info->m_InstanceID) )		// extern의 경우 일단 스킵..
		return false;

	m_stream.SetPosition( m_stream_basepos + (UINT32)info->m_Offset);
	return Read_i3NamedElement(&m_stream, name);
}

bool		i3RscPartialFile::SkipNamei3NamedElement(const i3ResourceFileObjectInfo2* info)
{
	if ( isExtern( (OBJREF)info->m_InstanceID) )		// extern의 경우 일단 스킵..
		return false;

	m_stream.SetPosition( m_stream_basepos + (UINT32)info->m_Offset);	

	return Skip_i3NamedElement(&m_stream);
}


bool		i3RscPartialFile::ReadChildListi3TreeElement( i3::vector<i3ResourceFileObjectInfo2*>& child_list)
{
	i3::vector<OBJREF> childObjRefList;
	bool res =  Read_i3TreeElement(&m_stream, childObjRefList);

	const size_t num = childObjRefList.size();

	child_list.resize(num);

	for (size_t i = 0 ; i < num ; ++i)
	{
		child_list[i] = i3RscPartialFile::FindObjectInfo(childObjRefList[i]);
	}

	return res;
}

bool		i3RscPartialFile::ReadParenti3GameNode( i3ResourceFileObjectInfo2*& parent)
{
	OBJREF parent_obj_ref;
	if ( Read_i3GameNode(&m_stream, parent_obj_ref) )
	{
		if (parent_obj_ref != 0)
			parent = i3RscPartialFile::FindObjectInfo(parent_obj_ref);
		else
			parent = NULL;
		return true;
	}
	return false;
}

bool		i3RscPartialFile::Skipi3GameNode()
{
	return Skip_i3GameNode(&m_stream);
}


bool		i3RscPartialFile::Readi3AIi3AIContext( i3ResourceFileObjectInfo2*& info_i3AI)
{
	OBJREF i3ai_obj_ref;

	if ( Read_i3AIContext(&m_stream, i3ai_obj_ref) )
	{
		if ( i3ai_obj_ref != 0 )
			info_i3AI = i3RscPartialFile::FindObjectInfo(i3ai_obj_ref);
		else
			info_i3AI = NULL;
		return true;
	}

	return false;	
}

bool		i3RscPartialFile::Readi3AIStateListi3AI( i3::vector<i3ResourceFileObjectInfo2*>& state_list )
{
	i3::vector<OBJREF>	obj_ref_list;
	
	if ( Read_i3AI(&m_stream, obj_ref_list) )
	{
		const size_t num = obj_ref_list.size();

		state_list.resize(num);

		for (size_t i = 0 ; i < num ; ++i )
		{
			state_list[i] = i3RscPartialFile::FindObjectInfo(obj_ref_list[i]);
		}
		
		return true;
	}
	
	return false;
}

bool	i3RscPartialFile::ReadAnimationRefi3AIState( OBJREF& anim_obj_ref)
{
	anim_obj_ref = 0;
	if ( !Read_i3AIState(&m_stream, anim_obj_ref) )
		return false;
	
	return isExtern(anim_obj_ref);
}


bool		i3RscPartialFile::Readi3RegDataListi3RegKey( i3::vector<i3ResourceFileObjectInfo2*>& data_list)
{
	i3::vector<OBJREF64> childObjRefList;				// 파일 포맷을 잘못정해서 망친 케이스다..
														// OBJREF64같은건 왜 만들었을까...
														// 파일 포맷을 아예 교체하는 것은 괜찮지만...
														// 파일포맷 수정시엔 도움이 안되고 용량만 까먹는다...
														// 결국 OBJREF로 하향 캐스팅 되는 케이스가 됨..

	bool res =  Read_i3RegKey(&m_stream, childObjRefList);

	const size_t num = childObjRefList.size();

	data_list.resize(num);

	for (size_t i = 0 ; i < num ; ++i )
	{
		data_list[i] = i3RscPartialFile::FindObjectInfo(OBJREF(childObjRefList[i]));
	}
	
	return res;
}

bool	i3RscPartialFile::Readi3RegData(UINT32& outDataType)
{
	return ( m_stream.Read(&outDataType, sizeof(UINT32)) != STREAM_ERR);
}

bool	i3RscPartialFile::Skipi3RegData()
{
	m_stream.SetPosition(sizeof(UINT32), STREAM_CURRENT);
	return true;
}

bool	i3RscPartialFile::Readi3RegString( i3::rc_wstring& val)
{
	return 	Read_i3RegString(&m_stream, val);
}

bool	i3RscPartialFile::Readi3RegStringArrayElement(size_t idx, i3::rc_wstring& val )
{
	return Read_i3RegStringArrayElement(&m_stream, idx, val);
}

bool	i3RscPartialFile::Readi3RegIntArrayElement(size_t idx, INT32& val)
{
	return Read_i3RegIntArrayElement(&m_stream, idx, val);
}


bool	i3RscPartialFile::Readi3RegString( i3ResourceFileObjectInfo2* info, i3::rc_wstring& val)
{
	i3RscPartialFile::SkipNamei3NamedElement(info);
	i3RscPartialFile::Skipi3RegData();
	
	return i3RscPartialFile::Readi3RegString( val);
}

bool	i3RscPartialFile::Readi3RegStringArrayElement(i3ResourceFileObjectInfo2* info, size_t idx, i3::rc_wstring& val)
{
	i3RscPartialFile::SkipNamei3NamedElement(info);
	i3RscPartialFile::Skipi3RegData();
	
	return i3RscPartialFile::Readi3RegStringArrayElement(idx, val);
}

bool	i3RscPartialFile::Readi3RegIntArrayElement(i3ResourceFileObjectInfo2* info, size_t idx, INT32& val)
{
	i3RscPartialFile::SkipNamei3NamedElement(info);
	i3RscPartialFile::Skipi3RegData();
	
	return i3RscPartialFile::Readi3RegIntArrayElement(idx, val);
}



bool		i3RscPartialFile::SkipChildListi3TreeElement()
{
	return Skip_i3TreeElement(&m_stream);
}

i3ResourceFileObjectInfo2*	i3RscPartialFile::Findi3RegData(i3ResourceFileObjectInfo2* reg_key, const char* tgtName)
{
	if (!i3RscPartialFile::SkipNamei3NamedElement(reg_key) )
		return NULL;
	if (!i3RscPartialFile::SkipChildListi3TreeElement() )
		return NULL;

	return i3RscPartialFile::Findi3RegData(tgtName);
}

i3ResourceFileObjectInfo2*	i3RscPartialFile::Findi3RegData(const char* tgtName)
{
	i3::vector<i3ResourceFileObjectInfo2*> data_list;
	i3RscPartialFile::Readi3RegDataListi3RegKey(data_list);

	return i3RscPartialFile::FindObjectInfoFromList(data_list, tgtName);
}

bool		i3RscPartialFile::Geti3RegDatas(i3ResourceFileObjectInfo2* reg_key, 
											  i3::vector<i3ResourceFileObjectInfo2*>& inout)
{
	if (!i3RscPartialFile::SkipNamei3NamedElement(reg_key) )
		return NULL;
	if (!i3RscPartialFile::SkipChildListi3TreeElement() )
		return NULL;

	return i3RscPartialFile::Readi3RegDataListi3RegKey(inout);
}




i3ResourceFileObjectInfo2*	i3RscPartialFile::FindChildi3TreeElement( const char* tgtName)
{
	i3::vector<i3ResourceFileObjectInfo2*> child_list;
	i3RscPartialFile::ReadChildListi3TreeElement(child_list);

	return i3RscPartialFile::FindObjectInfoFromList(child_list, tgtName);
}

INT32		i3RscPartialFile::Geti3RegDataCount( i3ResourceFileObjectInfo2* reg_key )
{
	if (!i3RscPartialFile::SkipNamei3NamedElement(reg_key) )
		return 0;
	if (!i3RscPartialFile::SkipChildListi3TreeElement() )
		return 0;
	
	return i3RscPartialFile::Geti3RegDataCount();
}
	
INT32		i3RscPartialFile::Geti3RegDataCount()
{
	return Read_DataListCount_i3RegKey(&m_stream);
}

i3ResourceFileObjectInfo2*		i3RscPartialFile::FindObjectInfoFromList(const i3::vector<i3ResourceFileObjectInfo2*>& list, const char* tgtName)
{
	const size_t num = list.size();

	i3::rc_string name;

	for (size_t i = 0 ; i < num ; ++i)
	{
		if ( i3RscPartialFile::ReadNamei3NamedElement(list[i], name) )
		{
			if ( name == tgtName )
				return list[i];
		}
	}

	return NULL;
}



i3ResourceFileObjectInfo2*	i3RscPartialFile::FindChildi3TreeElement( i3ResourceFileObjectInfo2* parent, const char* tgtName)
{
	if (!i3RscPartialFile::SkipNamei3NamedElement(parent) )
		return NULL;

	return i3RscPartialFile::FindChildi3TreeElement(tgtName);
}



void		i3RscPartialFile::CollectExternFileName( i3::vector<i3Rsc_ExternFilename>& out)
{
	out.clear();
	const size_t num_obj = m_Header.m_ObjectCount;

	for (size_t i = 0 ; i < num_obj ; ++i )
	{
		OBJREF ref = OBJREF(m_pObjInfo[i].m_InstanceID);

		
		INT32 string_idx = getExternStringIndex( ref );

		if (string_idx == -1)
			continue;
		
		i3Rsc_ExternFilename res;

		res.m_file_name = m_StrTable[string_idx];
		res.m_class_name = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex ];

		out.push_back(res);			

	}
		
}

i3ResourceFileObjectInfo2*		i3RscPartialFile::Findi3NamedElement(const char* tgtMetaName, const char* tgtName)
{
	const size_t num_obj = m_Header.m_ObjectCount;

	i3::rc_string elem_name;

	for (size_t i = 0 ; i < num_obj ; ++i )
	{
	
		i3::rc_string strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex ];

		if (strClassName != tgtMetaName)
			continue;

		if ( i3RscPartialFile::ReadNamei3NamedElement(&m_pObjInfo[i], elem_name) )
		{
			if (elem_name == tgtName )
				return &m_pObjInfo[i];
		}
	}

	return NULL;
}

void	i3RscPartialFile::CollectObjectInfosByMeta(const char* tgtMetaName, i3::vector<i3ResourceFileObjectInfo2*>& inout)
{
	const size_t num_obj = m_Header.m_ObjectCount;

	UINT32 found_idx = 0xFFFFFFFF;
		
	size_t i = 0;
	for ( ; i < num_obj ; ++i )
	{
		i3::rc_string strClassName = m_StrTable[ m_pObjInfo[i].m_ClassNameIndex ];

		if (strClassName == tgtMetaName)
		{
			found_idx = m_pObjInfo[i].m_ClassNameIndex;
			inout.push_back(&m_pObjInfo[i]);
			break;
		}
	}

	for ( ; i < num_obj ; ++i)
	{
		if ( found_idx == m_pObjInfo[i].m_ClassNameIndex)
		{
			inout.push_back(&m_pObjInfo[i]);
		}
	}
		
}


bool		i3RscPartialFile::CollectAnimPathListi3AIContext( i3ResourceFileObjectInfo2* aicontext_info, 
															   i3::rc_string& aicontext_name,
															   i3::vector<i3::rc_string>& animpath_list)
{
	if (!i3RscPartialFile::ReadNamei3NamedElement(aicontext_info, aicontext_name) )
		return false;

	if (!i3RscPartialFile::Skipi3GameNode())
		return false;

	i3ResourceFileObjectInfo2*  i3AI_Info;

	if (!i3RscPartialFile::Readi3AIi3AIContext( i3AI_Info ) )
		return false;
	
	if (i3AI_Info)
	{
		if (!i3RscPartialFile::SkipNamei3NamedElement(i3AI_Info))
			return false;
		
		i3::vector<i3ResourceFileObjectInfo2*>  state_list;

		if (!i3RscPartialFile::Readi3AIStateListi3AI( state_list ) )
			return false;

		const size_t num_state = state_list.size();
		
		i3::rc_string str_path;
		OBJREF anim_ref;			
		i3::vector<OBJREF> vec_objref;
		vec_objref.reserve(num_state);

		for (size_t i = 0 ; i < num_state ; ++i )
		{
			i3ResourceFileObjectInfo2* state = state_list[i];

			if ( !i3RscPartialFile::SkipNamei3NamedElement(state) )
				continue;
			
			if ( i3RscPartialFile::ReadAnimationRefi3AIState( anim_ref ) )
			{
				vec_objref.push_back(anim_ref);
			}
		}

		i3::vu::remove_duplicates(vec_objref);			// 중복 제거..( INT수준으로 제거하는게 젤 편해보임...)
		
		const size_t num_obj_ref = vec_objref.size();

		for (size_t i = 0 ; i < num_obj_ref ; ++i )
		{
			INT32 string_idx = getExternStringIndex(vec_objref[i]);
			
			animpath_list.push_back(m_StrTable[string_idx]);
		}

	}

	return true;
}



void	i3RscPartialFile::MakeAnimPathNode_CollectAnimPathList_Recursive(const i3::vector<int_node_ptr>& vec_int_node, 
														  INT32 curr_object_idx, i3Rsc_AnimPathNode* node)
{
	i3ResourceFileObjectInfo2* obj =  &m_pObjInfo[curr_object_idx];
	i3::rc_string name;
	i3RscPartialFile::CollectAnimPathListi3AIContext(obj, name, node->m_pathlist);
	node->m_name = name;
	
	const int_node* curr_int_node = vec_int_node[curr_object_idx].get();

	const size_t num_child = node->m_childlist.size();

	for (size_t i = 0 ; i < num_child ; ++i )
	{
		INT32 child_obj_idx = curr_int_node->child_list[i];
		i3Rsc_AnimPathNode* child_node = node->m_childlist[i].get();
		i3RscPartialFile::MakeAnimPathNode_CollectAnimPathList_Recursive(vec_int_node, child_obj_idx, child_node);  
	}

}

namespace
{
	void MakeAnimPathNodeMap_Recursive(const i3::rc_string& filename, const i3Rsc_AnimPathNodePtr& node, i3Rsc_AnimPathNodeMap& inout)
	{
		typedef i3::unordered_map<i3::fixed_string, vector_i3Rsc_AnimPathNode_Filename> map_type;
		map_type& m = inout.m_map;
		
		map_type::iterator it =	m.find(node->m_name);
	
		if ( it == m.end() )
		{
			i3::pair<map_type::iterator, bool> pr =	m.insert( i3::make_pair( node->m_name,  
														vector_i3Rsc_AnimPathNode_Filename() ) );
			it = pr.first;
						
			vector_i3Rsc_AnimPathNode_Filename& v = it->second;
		
			i3Rsc_AnimPathNode_FilenamePtr new_ptr = i3::make_shared<i3Rsc_AnimPathNode_Filename>();		
			
			new_ptr->m_filename = filename;
			new_ptr->m_nodelist.reserve(1);
			new_ptr->m_nodelist.push_back(node);

			v.reserve(1);
			v.push_back(new_ptr);
		}
		else
		{
			vector_i3Rsc_AnimPathNode_Filename& v = it->second;
		
			i3Rsc_AnimPathNode_FilenamePtr&  last_ptr = v.back();
			
			if (last_ptr->m_filename != filename)
			{
				i3Rsc_AnimPathNode_FilenamePtr new_ptr = i3::make_shared<i3Rsc_AnimPathNode_Filename>();		

				new_ptr->m_filename = filename;
				new_ptr->m_nodelist.reserve(1);
				new_ptr->m_nodelist.push_back(node);

				v.push_back(new_ptr);
			}
			else
			{
				last_ptr->m_nodelist.push_back(node);
			}

		}

		const size_t num = node->m_childlist.size();

		for (size_t i = 0 ; i < num ; ++i )
		{
			MakeAnimPathNodeMap_Recursive( filename, node->m_childlist[i], inout);
		}
	}
		
}


i3Rsc_AnimPathNodePtr	i3RscPartialFile::MakeAnimPathNodei3AIContext(i3ResourceFileObjectInfo2* root)
{
	// i3GameNode의 특성상 persist 저장 연결노드가 부모로 지정되있음...
	// 자식리스트로 거꾸로 풀기 위해서 사전 배열 매핑 작업이 필요하다..
	i3Rsc_AnimPathNodePtr result;

	if (!root)
		return result;
	
	const size_t num_obj = m_Header.m_ObjectCount;

	i3::vector<int_node_ptr>	vec_int_node(num_obj);
	INT32 root_index = i3RscPartialFile::FindObjectInfoIndex((OBJREF)root->m_InstanceID);

	{

		vector_i3Rsc_AnimPathNode	vec_obj_node(num_obj);

		i3ResourceFileObjectInfo2*	parent = NULL;

		for (size_t i = 0; i < num_obj ; ++i )
		{
			i3ResourceFileObjectInfo2* obj =  &m_pObjInfo[i];

			if ( root->m_ClassNameIndex != m_pObjInfo[i].m_ClassNameIndex )
				continue;

			vec_obj_node[i] = i3::make_shared<i3Rsc_AnimPathNode>();
			vec_int_node[i] = i3::make_shared<int_node>();
		}

		for (size_t i = 0 ; i < num_obj ; ++i)
		{
			i3Rsc_AnimPathNodePtr& node = vec_obj_node[i];
			if ( !node )
				continue;
			
			i3ResourceFileObjectInfo2* obj =  &m_pObjInfo[i];

			i3RscPartialFile::SkipNamei3NamedElement(obj);

			if ( i3RscPartialFile::ReadParenti3GameNode(parent) && parent != NULL )
			{
				INT32 parent_index = i3RscPartialFile::FindObjectInfoIndex((OBJREF)parent->m_InstanceID);
				if (parent_index != -1 && vec_obj_node[parent_index] )
				{
					vec_obj_node[parent_index]->m_childlist.push_back(node);
					vec_int_node[parent_index]->child_list.push_back(i);
				}
			}
		}

		result = vec_obj_node[root_index];
	}
	///////////////////  여기까지는 root_node 고려없이 동일 meta에 전체 계층구조를 만들게 된다...
	////////////////////////////////////////////////////////////////////////////////////////////////

	i3RscPartialFile::MakeAnimPathNode_CollectAnimPathList_Recursive(vec_int_node, root_index, result.get());
	
	return result;
}

void	MakeAnimPathNodeMap_Acc(const i3::rc_string& filename, const i3Rsc_AnimPathNodePtr& root, i3Rsc_AnimPathNodeMap& inout)
{
	if (!root)
		return;

	MakeAnimPathNodeMap_Recursive(filename, root, inout);
}

void	MakeAnimPathNodeMap_Acc(const i3::string& mediaPath, const i3::rc_string& relPath, i3Rsc_AnimPathNodeMap& inout)
{
	i3RscPartialFile f;
	i3::string full_path = mediaPath;
	full_path += '/';
	full_path.append(relPath.begin(), relPath.end());

	if (!f.Open( full_path.c_str() ) )	// 읽기 실패한 경우..
		return;

	i3ResourceFileObjectInfo2* root_info = f.Findi3NamedElement("i3AIContext", "Upper");
	i3Rsc_AnimPathNodePtr ptr = f.MakeAnimPathNodei3AIContext(root_info);
	i3::rc_string	strFilename;
	i3::extract_filename(full_path, strFilename);
	MakeAnimPathNodeMap_Acc(strFilename, ptr, inout);
}

bool	i3RscPartialFile::ReadStageClassNamei3StageInfo(i3ResourceFileObjectInfo2* info, i3::rc_string& val)
{
	if (!i3RscPartialFile::SkipNamei3NamedElement(info))
		return false;
	return i3RscPartialFile::ReadStageClassNamei3StageInfo(val);
}

bool	i3RscPartialFile::ReadStageClassNamei3StageInfo(i3::rc_string& val)
{
	return Read_StageClassNamei3StageInfo(&m_stream, val);
}

