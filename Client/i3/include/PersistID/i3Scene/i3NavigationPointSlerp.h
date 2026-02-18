#if !defined( __I3_NAVIGATION_POINTSLERP_H )
#define __I3_NAVIGATION_POINTSLERP_H

#include "i3CollideeLine.h"
#include "i3CollideeMesh.h"
#include "i3CollisionContext.h"

/////////////////////////////////////////////////////////////////////////////////////
// Point2Point Slerp by 정순구 2006. 3. 21
// 시작점에서 목표점까지의 루트를 구한다.
// 중간에 컬리젼이 되면 길을 나눠 목표를 구한다.

// nType
#define I3_NAVPOINT_FINDTYPE_FASTMODE			0x00000001	// 첫번째 검색해서 목표점이 나오면 넘겨준다.
#define I3_NAVPOINT_FINDTYPE_SHORTCUT			0x00000002	// 모두 검색하여 가장 짧은 길을 넘겨준다.
#define I3_NAVPOINT_FINDTYPE_LONGAWAY			0x00000004	// 모두 검색하여 가장 긴 길을 넘겨준다.

typedef struct ALIGN4 _tagI3_WAYPOINTSLERP
{
	INT32		m_nCount;
	REAL32		m_fHeuristicCost;
	VEC3D *		m_pPosition;
} I3_WAYPOINTSLERP;

class I3_EXPORT_SCENE i3NavigationPointSlerp : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationPointSlerp );
protected:
	UINT32		m_nType;

	VEC3D		m_vecStartPos;
	VEC3D		m_vecTargetPos;

	i3List		m_WayPointList;		// (VEC3D)

	i3List		m_tempWayPoint;

	VEC3D		m_vecDirection;
	VEC3D		m_vecCurrentPos;
	REAL32		m_fSlerp;
	REAL32		m_fLength;
	INT32		m_nCurrentWayPoint;
	REAL32		m_fBoundSphereRadius;

	REAL32		m_fHeuristicCost;

	i3CollisionContext *	m_pCollisionContext;
	UINT32		m_nCollisionWorldGroupIndex;

	void	_RemoveAllWayPoint( void );
	void	_CalcCost( VEC3D * pStart, VEC3D * pTarget );
	void	_CalcWayPoint( i3CollideeLine * pLine, I3_WAYPOINTSLERP * pSlerpData, VEC3D * pNextPosition, INT32 nCount );
	void	_calcDirection();

public:
	REAL32	getLength( void )				{ return m_fLength; }
	UINT32	getType( void )					{ return m_nType; }
	void	setType( UINT32 nType )			{ m_nType = nType; }

	VEC3D *	getStartPos( void )				{ return &m_vecStartPos; }
	void	setStartPos( VEC3D * pVec )		{ i3Vector::Copy( &m_vecStartPos, pVec ); }

	VEC3D * getTargetPos( void )			{ return &m_vecTargetPos; }
	void	setTargetPos( VEC3D * pVec )	{ i3Vector::Copy( &m_vecTargetPos, pVec ); }

	REAL32	getHeuristicCost( void )		{ return m_fHeuristicCost; }

	void	setWayPoint( INT32 idx, VEC3D * pVec )		{ m_WayPointList.SetItem( idx, pVec ); }
	
	VEC3D *	getDirection( void )			{ return &m_vecDirection; }

	// 생성된 Path들 중에 현재 좌표
	VEC3D * getCurrentPos( void )			{ return &m_vecCurrentPos; }
	void	setCurrentPos( VEC3D * pVec )	{ i3Vector::Copy( &m_vecCurrentPos, pVec ); }

	REAL32	getSlerp( void )				{ return m_fSlerp; }
	void	setSlerp( REAL32 fSlerp )		{ m_fSlerp = fSlerp; }

	void	setCurrentWayPoint( INT32 nWay ){ m_nCurrentWayPoint = nWay; }

	void	setBoundingSphere( REAL32 fRadius ) { m_fBoundSphereRadius = fRadius; }
	BOOL	IsDone( void );

public:
	i3NavigationPointSlerp();
	virtual ~i3NavigationPointSlerp();

	void	Create( UINT32 nType, REAL32 fRadius, i3CollisionContext * pCollisionContext, UINT32 nWorldGroupIndex );
	// pStart : 시작 위치, pTarget : 목표 위치
	void	Create( VEC3D * pStart, VEC3D * pTarget, UINT32 nType );

	// 이동 구간을 계산하여 세팅한다.
	// 중간에 컬리젼을 행하여 True이면 돌아가는 Path를 구한다.
	void	FindWayPoint( VEC3D * pStart, VEC3D * pTarget, REAL32 fBoundingSphere );
	// 기존 웨이포인트에서 추가한다.
	void	AddWayPoint( VEC3D * pTarget );

	// 현재 진행중인 웨이포인터
	VEC3D *	CalcCurrentPos( REAL32 fmoveLength );

	void	CalcMovement( VEC3D * pCurrentPos );

	void	ClearPath();
};

#endif	// __I3_NAVIGATION_POINTSLERP_H
