#include "pch.h"
#include "AI_NavigationPoint.h"

I3_CLASS_INSTANCE( CAI_NavigationPoint, i3ElementBase ); 

CAI_NavigationPoint::CAI_NavigationPoint()
{
	m_nType	= -1;
}

CAI_NavigationPoint::~CAI_NavigationPoint()
{
	_RemovePointList();
	_RemovePathList();
}

void CAI_NavigationPoint::_RemovePointList( void )
{
	INT32	i;

	if( m_nType == NAVIGATIONPOINT_TYPE_O )
	{
		NavigationPoint * pPoint;

		for( i = 0; i < getPointCount(); i++ )
		{
			pPoint = getPoint( i );

			I3MEM_SAFE_FREE_POINTER( pPoint );
		}
	}
	else if( m_nType == NAVIGATIONPOINT_TYPE_X )
	{
		NavigationPoint2 * pPoint;
		for( i = 0; i < getPointCount(); i++ )
		{
			pPoint = getPoint2( i );

			I3MEM_SAFE_FREE_POINTER( pPoint );
		}
	}

	m_PointList.Clear();
}

void CAI_NavigationPoint::_RemovePathList( void )
{
	INT32	i;
	NavigationPath * pPath;

	for( i = 0; i < getPathCount(); i++ )
	{
		pPath = getPath( i );

		I3MEM_SAFE_FREE_POINTER( pPath );
	}

	m_PathList.Clear();
}

void CAI_NavigationPoint::_AddNextPath( INT32 pathIdx, UINT32 nextIdx )
{
	NavigationPath * pPath;

	pPath = getPath( pathIdx );

	if( pPath == NULL )
		pPath = (NavigationPath *) i3MemAlloc( sizeof( NavigationPath ) );

	if( pPath->m_pNextPath )
	{
		pPath->m_pNextPath = (UINT32 *) i3MemReAlloc( pPath->m_pNextPath, sizeof(UINT32) * pPath->m_nNextPathCount + 1 );
		I3ASSERT( pPath->m_pNextPath );
		pPath->m_pNextPath[ pPath->m_nNextPathCount ] = nextIdx;
	}
	else
	{
		pPath->m_pNextPath = (UINT32 *) i3MemAlloc( sizeof(UINT32) );
		I3ASSERT( pPath->m_pNextPath );
		pPath->m_pNextPath[ 0 ] = nextIdx;
	}
}



#pragma pack( 4 )
typedef struct _tagI3PERSIST_PATHPOINTHEAD
{
	UINT8	m_ID[ 4 ];
	UINT32	m_nPointCount;
	UINT32	m_nLinkCount;
	
} I3PERSIST_PATHPOINTHEAD;

#pragma pack()

BOOL CAI_NavigationPoint::_Import( const char * pszPath )
{	
	UINT32	Rc;
	INT32	i;
	i3FileStream stream;
	
	PERSIST_VEC3D	vec;
	REAL32	fRadius;

	I3PERSIST_PATHPOINTHEAD	head;

	NavigationPoint * pPoint;
	NavigationPoint2 * pPoint2;
	NavigationPath * pPath;

	if( !stream.Open( pszPath, STREAM_READ ) )
	{
		return FALSE;
	}

	// 해더
	Rc = stream.Read( &head, sizeof( head ) );
	if( Rc == STREAM_ERR )
	{
		return FALSE;
	}

	if( !(head.m_ID[ 0 ] == 'L' && head.m_ID[ 1 ] == 'V' && head.m_ID[ 2 ] == 'P') )
		return FALSE;

	if( head.m_ID[ 3 ] == 'T' )
		m_nType	= 1;
	else if( head.m_ID[ 3 ] == 'X' )
		m_nType	= 2;

	m_PointList.SetCount( head.m_nPointCount );

	// 포인트 리스트
	if( m_nType == NAVIGATIONPOINT_TYPE_O )
	{
		for( i = 0; i < m_PointList.GetCount(); i++ )
		{
			pPoint = (NavigationPoint *) i3MemAlloc( sizeof( NavigationPoint ) );

			stream.Read( &vec, sizeof( vec ) );
			stream.Read( &pPoint->m_fRadius, sizeof( fRadius ) );
			
			setX( &pPoint->m_VecPosition, vec.x );
			setY( &pPoint->m_VecPosition, vec.y );
			setZ( &pPoint->m_VecPosition, vec.z );

			m_PointList.SetItem( i, pPoint );
		}
	}
	else if( m_nType == NAVIGATIONPOINT_TYPE_X )
	{
		for( i = 0; i < m_PointList.GetCount(); i++ )
		{
			pPoint2 = (NavigationPoint2 *) i3MemAlloc( sizeof( NavigationPoint2 ) );

			stream.Read( &vec, sizeof( vec ) );
			stream.Read( &pPoint2->m_fRadius, sizeof( REAL32 ) );
			stream.Read( &pPoint2->m_fTheta, sizeof( REAL32 ) );
			
			setX( &pPoint2->m_VecPosition, vec.x );
			setY( &pPoint2->m_VecPosition, vec.y );
			setZ( &pPoint2->m_VecPosition, vec.z );

			m_PointList.SetItem( i, pPoint2 );
		}
	}

	// 링크 리스트
	m_PathList.SetCount( head.m_nLinkCount );

	for( i = 0; i < m_PathList.GetCount(); i++ )
	{
		pPath = ( NavigationPath *) i3MemAlloc( sizeof( NavigationPath ) );

		stream.Read( &pPath->m_nPointCount, sizeof( pPath->m_nPointCount ) );

		pPath->m_pPointIndices = ( UINT32 *) i3MemAlloc( sizeof( UINT32 ) * pPath->m_nPointCount );
		
		stream.Read( pPath->m_pPointIndices, sizeof( UINT32 ) * pPath->m_nPointCount );
		
		m_PathList.SetItem( i, pPath );
	}

	//
	for( i = 0; i < m_PathList.GetCount(); i++ )
	{
		pPath = ( NavigationPath *) m_PathList.GetItem( i );

		Rc = stream.Read( &pPath->m_nPrvPathCount, sizeof( pPath->m_nPrvPathCount ) );
		if( Rc == STREAM_ERR )
		{
			return FALSE;
		}

		Rc = stream.Read( &pPath->m_nNextPathCount, sizeof( pPath->m_nNextPathCount ) );
		if( Rc == STREAM_ERR )
		{
			return FALSE;
		}
		
		if( pPath->m_nPrvPathCount > 0 )
		{
			pPath->m_pPrvPath = (UINT32 *) i3MemAlloc( sizeof( UINT32 ) * pPath->m_nPrvPathCount );
			I3ASSERT( pPath->m_pPrvPath );
			Rc = stream.Read( pPath->m_pPrvPath, sizeof( UINT32 ) * pPath->m_nPrvPathCount );
			if( Rc == STREAM_ERR )
			{
				return FALSE;
			}
		}

		if( pPath->m_nNextPathCount > 0 )
		{
			pPath->m_pNextPath = (UINT32 *) i3MemAlloc( sizeof( UINT32 ) * pPath->m_nNextPathCount );
			I3ASSERT( pPath->m_pNextPath );
			Rc = stream.Read( pPath->m_pNextPath, sizeof( UINT32 ) * pPath->m_nNextPathCount );
			if( Rc == STREAM_ERR )
			{
				return FALSE;
			}
		}
	}

	stream.Close();

	return TRUE;
}

void CAI_NavigationPoint::Create( const char * pszPath )
{
	if( !_Import( pszPath ) )
	{
		I3ASSERT_0;
	}
}

void CAI_NavigationPoint::CalcEndToStart()
{
	INT32	i, j;
	UINT32	pointIndex;
	NavigationPath * pSrcPath;
	NavigationPath * pCmpPath;
	
	for( i = 0; i < getPathCount(); i++ )
	{
		pSrcPath = getPath( i );

		// 마지막 포인트가 시작인 패스를 찾는다.
		pointIndex = pSrcPath->m_pPointIndices[ pSrcPath->m_nPointCount - 1 ];

		for( j = 0; j < getPathCount(); j++ )
		{
			if( i == j )
				continue;
			
			pCmpPath = getPath( j );

			if( pCmpPath->m_pPointIndices[ 0 ] == pointIndex )
			{
				// 시작 포인트가 동일하다.. 이어지는 패스
				_AddNextPath( i, j );
			}
		}
	}
}

VEC3D * CAI_NavigationPoint::GetPosWithPath( UINT32 path, UINT32 point )
{
	if( m_nType == NAVIGATIONPOINT_TYPE_O )
	{
		NavigationPoint * pPoint;
		pPoint = getPointWithPath( path, point );

		return &pPoint->m_VecPosition;
	}
	else
	{
		NavigationPoint2 * pPoint;
		pPoint = getPoint2WithPath( path, point );

		return &pPoint->m_VecPosition;
	}
}

REAL32 CAI_NavigationPoint::GetPointRangeWithPath( UINT32 path, UINT32 point )
{
	if( m_nType == NAVIGATIONPOINT_TYPE_O )
	{
		NavigationPoint * pPoint;
		pPoint = getPointWithPath( path, point );

		return pPoint->m_fRadius;
	}
	else
	{
		NavigationPoint2 * pPoint;
		pPoint = getPoint2WithPath( path, point );

		return pPoint->m_fRadius;
	}
}

REAL32 CAI_NavigationPoint::GetPointThetaWithPath( UINT32 path, UINT32 point )
{
	if( m_nType == NAVIGATIONPOINT_TYPE_X )
	{
		NavigationPoint2 * pPoint;
		pPoint = getPoint2WithPath( path, point );

		return pPoint->m_fTheta;
	}
	else
		return 0.f;
}

UINT32 CAI_NavigationPoint::_FindStarter( NavigationPath * pPath, UINT32 PathIdx )
{
	INT32	i;

	for( i = 0; i < pPath->m_nNextPathCount; i++ )
	{
		if( pPath->m_pNextPath[ i ] == PathIdx )
			return pPath->m_nPointCount - 1;
	}

	for( i = 0; i < pPath->m_nPrvPathCount; i++ )
	{
		if( pPath->m_pPrvPath[ i ] == PathIdx )
			return 0;
	}

	return 0xFFFFFFFF;
}

BOOL CAI_NavigationPoint::RunSequentialPath( UINT32 * pPathIdx, UINT32 * pPointIdx, INT32 nSequential )
{
	INT32	nRand;
	UINT32	prvPathIdx;
	NavigationPath * pPath;

	prvPathIdx = *pPathIdx;
	pPath = getPath( *pPathIdx );
	
	if( pPath->m_nPointCount > (INT32) *pPointIdx )
	{
		// 아직 패스를 도는 중이다.
		if( *pPointIdx == 0xFFFFFFFF )
		{
			// 반대로 돌고 있지만 다음 패스로 가야된다
			if( pPath->m_pPrvPath )
			{
				nRand = i3Math::Rand() % pPath->m_nPrvPathCount;
				*pPathIdx = pPath->m_pPrvPath[ nRand ];
				pPath = getPath( *pPathIdx );
				
				*pPointIdx = _FindStarter( pPath, prvPathIdx ); //pPath->m_nPointCount - 1;
				I3ASSERT( *pPointIdx != 0xFFFFFFFF );
			}
			else
			{
				// 이어지는 패스가 없다면 돌아간다
				(*pPointIdx) -= nSequential ;
				return TRUE;
			}
		}
	}
	else
	{
		// 마지막 포인트에 도착했다.
		// 이어지는 패스가 있는가.?
		if( pPath->m_pNextPath )
		{
			// 균등 확률로 패스를 선택
			nRand = i3Math::Rand() % pPath->m_nNextPathCount;
			*pPathIdx = pPath->m_pNextPath[ nRand ];

			pPath = getPath( *pPathIdx );
			*pPointIdx = _FindStarter( pPath, prvPathIdx );
			I3ASSERT( *pPointIdx != 0xFFFFFFFF );
		}
		else
		{
			// 이어지는 패스가 없다면 되돌아 간다.
			//I3TRACE( "NPC Turn pathroot\n" );
			(*pPointIdx) -= nSequential ;
			return TRUE;
		}
	}

	return FALSE;
}

VEC3D * CAI_NavigationPoint::GetRandPosWithPath( VEC3D * pStartPos, VEC3D * pOut, UINT32 path, UINT32 point, INT32 Sequential )
{
	NavigationPoint * pPoint;
	NavigationPoint2 * pPoint2;
	REAL32	fRand;
	VEC3D	Vec;
	
	if( m_nType == NAVIGATIONPOINT_TYPE_O )
	{
		pPoint = getPointWithPath( path, point );
		i3Vector::Copy( &Vec, &pPoint->m_VecPosition );
		pPoint = getPointWithPath( path, point + Sequential );
		fRand = pPoint->m_fRadius;

		i3Vector::Lerp( pOut, &Vec, &pPoint->m_VecPosition, (REAL32) (i3Math::Rand() % 100 ) / 100.0f );
	}
	else
	{
		pPoint2 = getPoint2WithPath( path, point );
		i3Vector::Copy( &Vec, &pPoint2->m_VecPosition );
		pPoint2 = getPoint2WithPath( path, point + Sequential );
		fRand = pPoint2->m_fRadius;
		i3Vector::Lerp( pOut, &Vec, &pPoint2->m_VecPosition, (REAL32) (i3Math::Rand() % 100 ) / 100.0f );
	}

	i3Vector::Add( pOut, (REAL32)(i3Math::Rand() % 200 - 100 ) / 100.0f * fRand, 0.0f, (REAL32)(i3Math::Rand() % 200 - 100) / 100.0f * fRand );

	return pOut;
}



