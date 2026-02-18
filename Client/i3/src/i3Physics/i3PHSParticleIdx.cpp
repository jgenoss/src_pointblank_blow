#include "i3PhysicsType.h"
#include "i3PHSParticleIdx.h"

I3_CLASS_INSTANCE( i3PHSParticleIdxTable, i3MemoryBuffer );

i3PHSParticleIdxTable::i3PHSParticleIdxTable()
{
	m_VIndexArray = NULL;
	m_VertexCount = 0;
}

i3PHSParticleIdxTable::~i3PHSParticleIdxTable()
{
}

void i3PHSParticleIdxTable::Create( INT32 VertexCount )
{
	m_VertexCount = VertexCount;
	
	//I3HMEM hMem = i3MemAlloc( sizeof(INT32) * VertexCount);
	m_VIndexArray = (INT32 *) i3MemAlloc( sizeof(INT32) * VertexCount);
	I3ASSERT( m_VIndexArray != NULL);
}

void i3PHSParticleIdxTable::Create( INT32 VertexCount, INT32 * pTable)
{
	m_VertexCount = VertexCount;

	m_VIndexArray = pTable;
}

#if defined( I3_COMPILER_VC )
#pragma pack( push, 4)
#endif

typedef struct ALIGN4 _tagI3PSQ_PERSIST_PARTICLE_TABLE_INFO
{	
	UINT8			m_ID[4];
	INT32			m_VertexCount;
	INT8			pad[16];

} I3PSQ_PERSIST_PARTICLE_TABLE_INFO;

#if defined( I3_COMPILER_VC )
#pragma pack( pop )
#endif


UINT32 i3PHSParticleIdxTable::OnSave( i3ResourceFile * pResFile )
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3PSQ_PERSIST_PARTICLE_TABLE_INFO Info;
	
	memset( &Info, 0, sizeof(Info));
	memcpy( Info.m_ID, "PIT1", 4);

	// 테이블 갯수와 버텍스인덱스 사이즈 등의 정보 저장
	Info.m_VertexCount = getVertexCount();

	Rc = pStream->Write( &Info, sizeof( Info ) );
	if( Rc == STREAM_ERR )
	{
		I3FATAL( "Could not write particle index info.\n" );
		return STREAM_ERR;
	}
	Result += Rc;

	// 버텍스인덱스 정보를 저장합니다.
	{
		Rc = pStream->Write( m_VIndexArray, sizeof(INT32) * getVertexCount() );
		if( Rc == STREAM_ERR )
		{
			I3FATAL( "Could not write particle Vertexindex data.\n" );
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3PHSParticleIdxTable::OnLoad( i3ResourceFile * pResFile )
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3PSQ_PERSIST_PARTICLE_TABLE_INFO Info;
	
	// 테이블리스트 정보를 읽습니다.
	Rc = pStream->Read( &Info, sizeof( Info ) );
	if( Rc == STREAM_ERR )
	{
		I3FATAL( "Could not write particle index info.\n" );
		return STREAM_ERR;
	}
	Result += Rc;

	if( Info.m_VertexCount > 0)
	{
		Create( Info.m_VertexCount);

		// 버텍스인덱스 정보를 로딩합니다.
		{
			Rc = pStream->Read( m_VIndexArray, sizeof(INT32) * getVertexCount() );
			if( Rc == STREAM_ERR )
			{
				I3FATAL( "Could not write particle Vertexindex data.\n" );
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	return Result;
}
