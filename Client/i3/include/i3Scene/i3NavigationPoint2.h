#if !defined( __I3_NAVIGATIONPOINT2_H )
#define __I3_NAVIGATIONPOINT2_H

#include "i3Node.h"

// 네비게이션 포인터 2( 원 기반 포인터 )		2006. 3. 18. by 정순구
// 
// 각 포인터는 원을 갖으며 해당 원의 어느 지점으로도 이동이 가능해야 된다.

#define I3_NAVPOINT_INDEX_NONE				0xFFFF

#define I3_NAVPOINT_MAX_NEIGHBORCOUNT		0xF
#define I3_NAVPOINT_DIR_NONE				0xF		// 방향성에 대해 방향 결정이 없는 값

// 포인터 구조
struct ALIGN4 I3_NAVPOINT_WITHSPHERE
{
	VEC3D		m_vecPosition;				// 좌표
	REAL32		m_fRadius = 0.0f;			// 영역
	
	INT32		m_nNeighborCount = 0;		// 이웃한 포인터 갯수
	UINT16 *	m_nNeighborArray = nullptr;
	VEC2D *		m_pNeighborBox = nullptr;	// 이웃한 포인터에 대한 박스(2개점만 가지고 있으면 된다)
};

class I3_EXPORT_SCENE i3NavigationPoint2 : public i3Node
{
	I3_EXPORT_CLASS_DEFINE( i3NavigationPoint2, i3Node );
protected:
	// Persistent member
	INT32		m_nPointCount = 0;				// 전체 포인트 갯수
	I3_NAVPOINT_WITHSPHERE *	m_pPoint = nullptr;	// 전체 포인트

	UINT16		m_nIndex = 0;					// 현재 노드의 인덱스

	// Volatile member
	UINT8 *		m_pNavInfo = nullptr;					// 현재 포인트에서 이웃한 포인트에 대한 방향
	INT32		m_NavInfoSize = 0;

	INT32		m_ByteIndex = 0;
	INT32		m_ByteShift = 0;
	
public:
	INT32		getNavPointCount( void )						{ return m_nPointCount; }
	void		setNavPointCount( INT32 nCount )				{ m_nPointCount = nCount; }

	I3_NAVPOINT_WITHSPHERE * getNavPoint( UINT16 idx )			{ return m_pPoint + idx; }
	I3_NAVPOINT_WITHSPHERE * getNavPoint( void )				{ return m_pPoint + m_nIndex; }

	UINT16		getIndex( void )								{ return m_nIndex; }
	void		setIndex( UINT16 nIndex )						{ m_nIndex = nIndex; }

	INT32		getCount( void )								{ return m_nPointCount; }
	void		SetCount( INT32 nCount );

	VEC3D *		getPosition( void )								{ return &getNavPoint()->m_vecPosition; }
	void		setPosition( VEC3D * pVec )						{ i3Vector::Copy( &getNavPoint()->m_vecPosition, pVec ); }

	REAL32		getRadius( void )								{ return getNavPoint()->m_fRadius; }
	void		setRadius( REAL32 fRadius )						{ getNavPoint()->m_fRadius = fRadius; }

	INT32		getNeighborCount( void )						{ return getNavPoint()->m_nNeighborCount; }
	void		setNeighborCount( INT32 nCount )				{ getNavPoint()->m_nNeighborCount = nCount; }

	// 길찾기 정보 얻기
	UINT8		GetPathDir( UINT16 nStart, UINT16 nTarget );
	// 길찾기 정보 설정
	void		SetPathDir( UINT16 nStart, UINT16 nTarget, UINT8 nDir );
	// 이웃의 방향 인덱스 구하기
	UINT8		getNeighborDirFromTriIndex( UINT8 nIndex, UINT16 nFind )
	{	
		for( UINT8 i = 0; i < I3_NAVPOINT_MAX_NEIGHBORCOUNT; i++ )
		{
			if( getNavPoint( nIndex )->m_nNeighborArray[ i ] == nFind )
			{
				return i;
			}
		}
		return I3_NAVPOINT_DIR_NONE;
	}

	bool		IsNeighbor( UINT16 nNeighborIndex );
	UINT16		getNeighborIndex( INT32 nCount )
	{
		I3ASSERT( nCount < getNeighborCount() );
		return getNavPoint()->m_nNeighborArray[ nCount ];
	}
	UINT16 *	getNeighborArray( void )						{ return getNavPoint()->m_nNeighborArray; }
	I3_NAVPOINT_WITHSPHERE * GetNeighbor( INT32 idx );
	void		AddNeighbor( UINT16 idx );

	VEC2D *		getNeighborBox1( INT32 nNeighbor )	{ return &getNavPoint()->m_pNeighborBox[nNeighbor * 2 ]; }
	VEC2D *		getNeighborBox2( INT32 nNeighbor )	{ return &getNavPoint()->m_pNeighborBox[nNeighbor * 2 + 1]; }

	void		AddNeighborBox( VEC2D * pVec1, VEC2D * pVec2 );

public:	
	void	AddNavPoint( I3_NAVPOINT_WITHSPHERE * pNavPoint );
	
	virtual void OnBuildObjectList( i3::vector<i3PersistantElement*>& List ) override;
	virtual	UINT32 OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32 OnLoad( i3ResourceFile * pResFile ) override;
};

#endif	// __I3_NAVIGATIONPOINT2_H
