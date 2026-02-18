#include "i3SceneDef.h"
#include "i3NavigationPoint.h"
#include "i3NavigationPointPathFinder.h"
#include "i3NavigationPointPathNode.h"

I3_CLASS_INSTANCE( i3NavigationPoint);

i3NavigationPoint::~i3NavigationPoint()
{
	I3MEM_SAFE_FREE_POINTER( m_pPointInfo ); 
	I3MEM_SAFE_FREE_POINTER( m_pNavInfo ); 
}

void i3NavigationPoint::SetCount( UINT8 nCount)
{
//	I3MEM_SAFE_FREE( m_pPointInfo, m_hPointMemId );
	I3MEM_SAFE_FREE_POINTER( m_pPointInfo ); 

	I3ASSERT( nCount < I3_MAX_NVPOINT_COUNT );
	m_nCount = nCount;

	if( nCount > 0)
	{
		//m_hPointMemId = i3MemAlloc( sizeof(i3::pack::NAVPOINT_INFO) * nCount );
		m_pPointInfo = (i3::pack::NAVPOINT_INFO *) i3MemAlloc( sizeof(i3::pack::NAVPOINT_INFO) * nCount );
		I3ASSERT( m_pPointInfo);
	}
}

// 길찾기 정보 설정
void i3NavigationPoint::SetPathDir( UINT8 nStart, UINT32 nDest, UINT8 nDir )
{
	I3ASSERT( nDir < I3_NVPOINT_DIR_NONE );

	UINT8 nPrevValue = GetPathDir( nStart, nDest );
	if( nPrevValue != I3_NVPOINT_DIR_NONE )
	{
		if( nPrevValue != nDir )
		{
			I3TRACE( "WARN : %d -> %d, nPrev = %d, nCur = %d\n", nStart, nDest, nPrevValue, nDir );
		}
	}

	INT32 nBitIndex = ( ( nDest * m_nCount ) + nStart ) << 1;
	INT32 nByteIndex = nBitIndex >> 3;	// nBitIndex / 8
	INT32 nByteShift = nBitIndex & 7;	// nBitIndex % 8
	UINT8 nEx = (UINT8)( ~( UINT8(I3_NVPOINT_DIR_NONE) << nByteShift ) );
	m_pNavInfo[ nByteIndex ] &= nEx;
	m_pNavInfo[ nByteIndex ] |= nDir << nByteShift;
}

// 길찾기 정보 얻기
UINT8 i3NavigationPoint::GetPathDir( UINT8 nStart, UINT32 nDest )
{
	INT32 nBitIndex = ( ( nDest * m_nCount ) + nStart ) << 1;
	INT32 nByteIndex = nBitIndex >> 3;	// nBitIndex / 8
	INT32 nByteShift = nBitIndex & 7;	// nBitIndex % 8
	return UINT8( ( m_pNavInfo[ nByteIndex ] >> nByteShift ) & 3 );
}


// 길찾기 정보 테이블 작성
bool i3NavigationPoint::BuildNavigationInfo()
{
	I3TRACE( "i3NavigationPoint::BuildNavigationInfo]\n" );

	I3ASSERT( m_pNavInfo == nullptr );
	I3ASSERT( m_nCount > 0 );

	m_NavInfoSize = ( m_nCount * m_nCount ) >> 2;

	m_pNavInfo = (UINT8*)i3MemAlloc( m_NavInfoSize );
	memset( m_pNavInfo, 0xFFFFFFFF, m_NavInfoSize );

	i3NavigationPointPathFinder *pFinder = i3NavigationPointPathFinder::new_object();

	UINT8 i, j, k;
	for( i = 0; i < m_nCount; i++ )
	{
		for( j = 0; j < m_nCount; j++ )
		{
			if( i == j ) continue;

			//I3TRACE( "Start : %d, End : %d\n", i, j );

			pFinder->Create( this, i, j, 1.0f );
			pFinder->Run();
			if( ! pFinder->IsDone() )
			{
				return false;
			}
			const i3::vector<i3NavigationPointPathNode*>& Solution = pFinder->GetSoultion();
			INT32 nCount = (INT32)Solution.size();
			I3ASSERT( nCount >= 2 );

			UINT8 nPrevIndex, nCurIndex, nTriDir;
			i3NavigationPointPathNode *pNode;

			// 첫번째 노드는 종점 노드이므로 결과값을 저장하지 않는다.
			pNode = Solution[0];
			nCurIndex = pNode->getIndex();
			I3ASSERT( j == nCurIndex );
			nPrevIndex = nCurIndex;

			// 두번째 노드 부터 값을 저장한다.
			for( k = 1; k < nCount; k++ )
			{
				pNode = Solution[k];
				nCurIndex = pNode->getIndex();

				nTriDir = GetNeighborDirFromTriIndex( nCurIndex, nPrevIndex );
				I3ASSERT( nTriDir != I3_NVPOINT_DIR_NONE );

				SetPathDir( nCurIndex, j, nTriDir );

				// Test
				#if defined( I3_DEBUG)
				UINT8 nTest = GetPathDir( nCurIndex, j );
				I3ASSERT( nTest == nTriDir );
				#endif

				nPrevIndex = nCurIndex;
			}

			// 마지막 노드 검사
			I3ASSERT( nCurIndex == i );
		}
	}

	I3_SAFE_RELEASE( pFinder );

#if 1
	I3TRACE( "Check Navigation Direction Info\n" );
	for( i = 0; i < m_nCount; i++ )
	{
		for( j = 0; j < m_nCount; j++ )
		{
			if( i == j ) continue;

			UINT8 nDir = GetPathDir( i, j );
			if( nDir == I3_NVPOINT_DIR_NONE )
			{
				I3TRACE( "%3d -> %3d : Direction Value is not specified.", i, j );
			}

			// 이웃의 인덱스를 얻어와서...
			UINT8 nNextIndex = GetNeighborIndex( i, nDir );
			if( nNextIndex != j )
			{	// 그 이웃이 종점이 아니라면...
				// 그 다음 방향이 이전 인덱스인지 여부 검사
				UINT8 nNextDir = GetPathDir( nNextIndex, j );
				UINT8 nNextNeighborIndex = GetNeighborIndex( nNextIndex, nNextDir );
				if( nNextNeighborIndex == i )
				{
					I3TRACE( "ERROR : Endless Track Found!!!." );
					I3ASSERT_0;
				}
			}
		}
	}
	I3TRACE( "Done.\n" );
#endif

	return true;
}

// pStartPos	(In)	: 현재 위치
// pEndPos		(In)	: 최종 목표 위치
UINT8 i3NavigationPoint::GetPathDirection( UINT8 nStartIndex, UINT8 nEndIndex )
{
	I3ASSERT( nStartIndex != I3_NVPOINT_INDEX_NONE );
	I3ASSERT( nEndIndex != I3_NVPOINT_INDEX_NONE );

	// 같은 메쉬안에 있을 경우 바로 이동.
	if( nStartIndex == nEndIndex )
	{
		return nEndIndex;
	}

	// 바로 이웃 메쉬인 경우에도 직접 이동이 보장된다.
	if(	IsNeighbor( nStartIndex, nEndIndex ) )
	{
		return nEndIndex;
	}

	UINT8 nDir;
	nDir = GetPathDir( nStartIndex, nEndIndex );
	if( nDir == I3_NVPOINT_DIR_NONE )
	{
		I3TRACE( "Failed to find Navigation Direction.\n" );
		return I3_NVPOINT_INDEX_NONE;
	}

	return GetNeighborIndex( nStartIndex, nDir );
}

void i3NavigationPoint::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3Node::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 NVMESH
	{
		UINT8				m_nVersion = 1;
		UINT8				m_nCount = 0;
		INT32				m_NavInfoSize = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3NavigationPoint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::NVMESH Data;

	// Node
	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Data.m_nCount		= m_nCount;
	Data.m_NavInfoSize	= m_NavInfoSize;

	Rc = pStream->Write( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationPoint::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	Rc = pStream->Write( m_pPointInfo, sizeof(i3::pack::NAVPOINT_INFO) * m_nCount );
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationPoint::OnSave()", "Could not write m_pPointInfo.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_NavInfoSize > 0 )
	{
		Rc = pStream->Write( m_pNavInfo, m_NavInfoSize );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationPoint::OnSave()", "Could not write m_pNavInfo.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3NavigationPoint::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3NavigationPoint::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::NVMESH Data;

	// node
	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &Data, sizeof(Data));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3NavigationPoint::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	I3ASSERT( Data.m_nVersion );
	m_nCount			= Data.m_nCount;
	m_NavInfoSize		= Data.m_NavInfoSize;

	{
		I3MEM_SAFE_FREE_POINTER( m_pPointInfo ); 
		if( m_nCount > 0)
		{
			m_pPointInfo = (i3::pack::NAVPOINT_INFO *) i3MemAlloc( sizeof(i3::pack::NAVPOINT_INFO) * m_nCount);
			I3ASSERT( m_pPointInfo);
		}

		Rc = pStream->Read( m_pPointInfo, sizeof(i3::pack::NAVPOINT_INFO) * m_nCount );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationPoint::OnLoad()", "Could not read m_pPointInfo.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	if( m_NavInfoSize > 0 )
	{
		I3MEM_SAFE_FREE_POINTER( m_pNavInfo ); 

		I3ASSERT( m_NavInfoSize == ( m_nCount * m_nCount ) >> 2 );
		m_pNavInfo = (UINT8*) i3MemAlloc( m_NavInfoSize );

		Rc = pStream->Read( m_pNavInfo, m_NavInfoSize );
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3NavigationPoint::OnLoad()", "Could not read m_pNavInfo.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}
