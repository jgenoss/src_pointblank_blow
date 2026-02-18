#include "i3SceneDef.h"
#include "i3NavigationPoint2.h"
#include "i3Base/string/compare/generic_icompare.h"

I3_CLASS_INSTANCE( i3NavigationPoint2);

void i3NavigationPoint2::SetCount( INT32 nCount )
{
	m_nPointCount = nCount;
	I3MEM_SAFE_FREE_POINTER( m_pPoint ); 

	if( nCount > 0)
	{	
		m_pPoint = (I3_NAVPOINT_WITHSPHERE *) i3MemAlloc( sizeof(I3_NAVPOINT_WITHSPHERE) * nCount );
		I3ASSERT( m_pPoint);
	}
}

void i3NavigationPoint2::AddNavPoint( I3_NAVPOINT_WITHSPHERE * pNavPoint )
{
	INT32	nNavPointCount = getNavPointCount();
	I3ASSERT( pNavPoint );

	if( m_pPoint )
	{
		m_pPoint = (I3_NAVPOINT_WITHSPHERE *) i3MemReAlloc( m_pPoint, sizeof( I3_NAVPOINT_WITHSPHERE ) * ( nNavPointCount + 1 ) );
		setIndex( (UINT16) nNavPointCount );
		setPosition( &pNavPoint->m_vecPosition );
		setRadius( pNavPoint->m_fRadius );
	}
	else
	{
		m_pPoint = (I3_NAVPOINT_WITHSPHERE *) i3MemAlloc( sizeof( I3_NAVPOINT_WITHSPHERE ) );
		I3ASSERT( m_pPoint );
		setIndex( (UINT16) 0 );
		setPosition( &pNavPoint->m_vecPosition );
		setRadius( pNavPoint->m_fRadius );
	}
}

UINT8 i3NavigationPoint2::GetPathDir( UINT16 nStart, UINT16 nTarget )
{
	INT32 nBitIndex = ( ( nTarget * m_nPointCount ) + nStart ) << 1;
	m_ByteIndex = nBitIndex >> 3;		// nBitIndex / 8
	m_ByteShift = nBitIndex & 7;			// nBitIndex % 8
	return UINT8( ( m_pNavInfo[ m_ByteIndex ] >> m_ByteShift ) & 3 );
}

void i3NavigationPoint2::SetPathDir( UINT16 nStart, UINT16 nTarget, UINT8 nDir )
{
	INT8	nPrevValue = GetPathDir( nStart, nTarget );
	if( nPrevValue != I3_NAVPOINT_DIR_NONE )
	{
		if( nPrevValue != nDir )
		{
			I3TRACE( "WARN : %d -> %d, nPrev = %d, nCur = %d\n", nStart, nTarget, nPrevValue, nDir );
		}
	}
	UINT8	nEx = (UINT8)( ~( UINT8(I3_NAVPOINT_DIR_NONE) << m_ByteShift ) );
	m_pNavInfo[ m_ByteIndex ] &= nEx;
	m_pNavInfo[ m_ByteIndex ] |= nDir << m_ByteShift;
}

bool i3NavigationPoint2::IsNeighbor( UINT16 nNeighborIndex )
{
	INT32	i;
	I3ASSERT( nNeighborIndex != I3_NAVPOINT_INDEX_NONE );
	for( i = 0; i < getNavPoint()->m_nNeighborCount; i ++ )
	{
		if(getNavPoint()->m_nNeighborArray[ i ] == nNeighborIndex )
		{
			return true;
		}
	}
	return false;
}

I3_NAVPOINT_WITHSPHERE * i3NavigationPoint2::GetNeighbor( INT32 idx )
{
	I3ASSERT( idx < getNeighborCount() );
	UINT16 nNeighborIndex = getNavPoint()->m_nNeighborArray[ idx ];
	if( nNeighborIndex == I3_NAVPOINT_INDEX_NONE )
	{
		return nullptr;
	}
	return getNavPoint( nNeighborIndex );
}

void i3NavigationPoint2::AddNeighbor( UINT16 idx )
{
	INT32	nNeighborCount = getNeighborCount();
	UINT16 * pNeighbor = getNavPoint()->m_nNeighborArray;
	if( pNeighbor )
	{	
		I3ASSERT( nNeighborCount < I3_NAVPOINT_MAX_NEIGHBORCOUNT );
		pNeighbor = (UINT16*) i3MemReAlloc( pNeighbor, sizeof( UINT16 ) * ( nNeighborCount + 1 ));
		I3ASSERT( pNeighbor );
		pNeighbor[ nNeighborCount ] = idx;
		setNeighborCount( nNeighborCount + 1 );
	}
	else
	{
		pNeighbor = (UINT16*) i3MemAlloc( sizeof( UINT16 ) );
		I3ASSERT( pNeighbor );
		* pNeighbor = idx;
		setNeighborCount( 1 );
	}
	getNavPoint()->m_nNeighborArray = pNeighbor;
}

void i3NavigationPoint2::AddNeighborBox( VEC2D * pVec1, VEC2D *pVec2 )
{
	INT32	nNeighborCount = getNeighborCount( );
	VEC2D * pVec = getNavPoint()->m_pNeighborBox;
	if( pVec )
	{
		I3ASSERT( nNeighborCount < I3_NAVPOINT_MAX_NEIGHBORCOUNT );
		pVec = (VEC2D *) i3MemReAlloc( pVec, sizeof( VEC2D ) * ( ( nNeighborCount + 1 ) * 2 ));
		I3ASSERT( pVec );
		i3Vector::Copy( &pVec[ nNeighborCount * 2 ], pVec1 );
		i3Vector::Copy( &pVec[ nNeighborCount * 2 + 1 ], pVec2 );
	}
	else
	{
		pVec = (VEC2D *) i3MemAlloc( sizeof( VEC2D ) * 2 );
		I3ASSERT( pVec );
		i3Vector::Copy( &pVec[ 0 ], pVec1 );
		i3Vector::Copy( &pVec[ 1 ], pVec2 );
	}
	getNavPoint()->m_pNeighborBox = pVec;
}

void i3NavigationPoint2::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	i3Node::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack( push, 1 )
#endif

namespace pack
{
	struct ALIGN4 PATHPOINT_INFO
	{
		UINT8		m_ID[4] = { 'P', 'T', 'H', 'P' };
		INT32		m_nPointCount = 0;
	};

	struct ALIGN4 PATHPOINT
	{
		i3::pack::VEC3D		m_vecPosition;					// ÁÂÇ¥
		REAL32				m_fRadius = 0.0f;					// 

		INT32				m_nNeighborCount = 0;			// ÀÌ¿ôÇÑ Á¡µé¿¡ ´ëÇÑ ÀÎµ¦½º Ä«¿îÆ®
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack( pop )
#endif

UINT32 i3NavigationPoint2::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	INT32	i, j;
	i3Stream * pStream = pResFile->GetStream();

	pack::PATHPOINT_INFO		Info;
	pack::PATHPOINT			Point;
	i3::pack::VEC2D * pNeighborBox;
	
	Info.m_nPointCount			= m_nPointCount;

	Rc = pStream->Write( &Info, sizeof( pack::PATHPOINT_INFO ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnSave() - Could not save pathpoint info." );
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < Info.m_nPointCount; i++ )
	{
		setIndex( (UINT16) i );
		Point.m_vecPosition.x		= i3Vector::GetX( getPosition() );
		Point.m_vecPosition.y		= i3Vector::GetY( getPosition() );
		Point.m_vecPosition.z		= i3Vector::GetZ( getPosition() );

		Point.m_fRadius				= getRadius();

		Point.m_nNeighborCount		= getNeighborCount();
		Rc = pStream->Write( &Point, sizeof( pack::PATHPOINT ) );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnSave() - Could not save pathpoint." );
			return STREAM_ERR;
		}
		Result += Rc;

		Rc = pStream->Write( getNeighborArray(), sizeof(UINT16) * Point.m_nNeighborCount );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnSave() - Could not save neighbor." );
			return STREAM_ERR;
		}
		Result += Rc;

		pNeighborBox		= (i3::pack::VEC2D*) i3MemAlloc( sizeof(i3::pack::VEC2D) * Point.m_nNeighborCount * 2 );
		for( j = 0; j < Point.m_nNeighborCount; j++ )
		{	
			pNeighborBox[ j * 2 ].x		= i3Vector::GetX( getNeighborBox1( j ) );
			pNeighborBox[ j * 2 ].y		= i3Vector::GetY( getNeighborBox1( j ) );
			pNeighborBox[ j * 2  + 1].x	= i3Vector::GetX( getNeighborBox2( j ) );
			pNeighborBox[ j * 2  + 1].y	= i3Vector::GetY( getNeighborBox2( j ) );
		}
		Rc = pStream->Write( pNeighborBox, sizeof(i3::pack::VEC2D) * Point.m_nNeighborCount * 2 );
		I3MEM_SAFE_FREE_POINTER( pNeighborBox );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnSave() - Could not save neighborbox." );
			return STREAM_ERR;
		}
		Result += Rc;

	}

	return Result;
}

UINT32 i3NavigationPoint2::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	INT32	i, j;
	INT32	nCount = 0;
	i3Stream * pStream = pResFile->GetStream();

	pack::PATHPOINT_INFO	Info;
	pack::PATHPOINT		Point;
	i3::pack::VEC2D *				pNeighborBox;

	Rc = pStream->Read( &Info, sizeof( pack::PATHPOINT_INFO ) );
	if( Rc == STREAM_ERR )
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnLoad() - Could not read pathpointinfo." );
		return STREAM_ERR;
	}
	Result += Rc;

	if( i3::generic_icompare( (const char*) Info.m_ID, "PTHP" ) >=0 )		//!= -1 )
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnLoad() - Invalid read info." );
		return STREAM_ERR;
	}
	setNavPointCount( Info.m_nPointCount );

	if( m_pPoint )
	{
		// Release Points

	}

	m_pPoint		= (I3_NAVPOINT_WITHSPHERE *) i3MemAlloc( sizeof(I3_NAVPOINT_WITHSPHERE) * getNavPointCount() );
	I3ASSERT( m_pPoint );

	for( i = 0; i < getNavPointCount(); i++ )
	{
		nCount = 0;

		setIndex( (UINT16) i );

		Rc = pStream->Read( &Point, sizeof( pack::PATHPOINT ) );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnLoad() - Could not read pathpoint." );
			goto EXIT_LAVEL;
		}
		Result += Rc;

		i3Vector::SetX( getPosition(), Point.m_vecPosition.x );
		i3Vector::SetY( getPosition(), Point.m_vecPosition.y );
		i3Vector::SetZ( getPosition(), Point.m_vecPosition.z );

		setRadius( Point.m_fRadius );
		setNeighborCount( Point.m_nNeighborCount );

		getNavPoint()->m_nNeighborArray		= (UINT16*) i3MemAlloc( sizeof(UINT16) * getNeighborCount() );
		Rc = pStream->Read( getNavPoint()->m_nNeighborArray, sizeof(UINT16) * Point.m_nNeighborCount );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnLoad() - could not read pathpoint neighbor." );
			goto EXIT_LAVEL;
		}
		Result += Rc;
		
		pNeighborBox			= (i3::pack::VEC2D*) i3MemAlloc( sizeof(i3::pack::VEC2D) * getNeighborCount() * 2 );
		Rc = pStream->Read( pNeighborBox, sizeof(i3::pack::VEC2D) * Point.m_nNeighborCount * 2 );
		if( Rc == STREAM_ERR )
		{
			I3PRINTLOG(I3LOG_FATAL,  "i3NavigationPoint2::OnLoad() - Could not read pathpoint neighborbox." );
			I3MEM_SAFE_FREE_POINTER( pNeighborBox );
			goto EXIT_LAVEL;
		}
		Result += Rc;

		getNavPoint()->m_pNeighborBox	= (VEC2D*) i3MemAlloc( sizeof(VEC2D) * getNeighborCount() * 2 );
		for( j = 0; j < Point.m_nNeighborCount * 2; j++ )
		{
			i3Vector::Set( &getNavPoint()->m_pNeighborBox[ j ],
				pNeighborBox[ j ].x, pNeighborBox[ j ].y );
		}

		I3MEM_SAFE_FREE_POINTER( pNeighborBox );
	}

	return Result;
EXIT_LAVEL :
	for( i = 0; i < nCount; i++ )
	{	
		I3MEM_SAFE_FREE_POINTER( m_pPoint[i].m_nNeighborArray );
		I3MEM_SAFE_FREE_POINTER( m_pPoint[i].m_pNeighborBox );
	}

	I3MEM_SAFE_FREE_POINTER( m_pPoint );
	return STREAM_ERR;
}

