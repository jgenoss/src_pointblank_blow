#if !defined( __I3_NAVIGATIONPOINT2_PATHFINDER_H )
#define __I3_NAVIGATIONPOINT2_PATHFINDER_H

#include "i3NavigationPoint2.h"
#include "i3NavigationPointPathNode2.h"

#define I3_PATHFIND_NONE				0x00000000		// 길을 찾지 않고 머무른다.
#define I3_PATHFIND_SHORTCUT			0x00000001		// 가장 짧은 루트를 찾는다.
#define I3_PATHFIND_PATHWAY				0x00000002		// 기본 포인트를 따라간다.
#define I3_PATHFIND_RANDWAY				0x00000004		// 기본 포인트의 원안의 랜덤 점으로 이동한다.

#define I3_PATHFIND_NOTFOUND			0xFF			// 길찾기 실패

typedef struct ALIGN4 _tagI3_WAYPOINT
{
	UINT32		m_PathPointIndex;		// 이동할 Pathpoint의 포인터( I3_NAVPOINT_WITHSPHERE *)
	VEC3D		m_Position;				// 실제 이동할 좌표
} I3_WAYPOINT;

class i3NavigationPoint2;
class i3NavigationPointPathNode2;

class I3_EXPORT_SCENE i3NavigationPoint2Finder : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationPoint2Finder );

protected:
	// Persist
	UINT32				m_FindType;				// 길찾기 방식

	i3NavigationPoint2	*m_pNvPoint;
	i3NavigationPointPathNode2	*m_pCurrent;

	i3BinList			*m_pOpenList;
	i3BinList			*m_pCreatedList;
	i3List				*m_pSolution;

	// Volatile
	UINT16				m_nStartIndex, m_nTargetIndex;
	
	VEC3D				m_vecTarget;
	REAL32				m_HeuristicWeight;

	BOOL				m_bFoundTarget;			// 목표점 찾은 여부
	BOOL				m_bIsDone;				// 길찾기 완료 여부

	void	_ReleasePath( void );

	// 목표지점으로의 waypoint 계산( Astar finder)
	BOOL	_FindPathWay( void );

	// waypoint에서 짧은 경로를 찾아서 세팅
	BOOL	_FindShortCut( void );
	// waypoint에서 랜덤으로 위치 세팅
	BOOL	_FindRandWay( void );

	BOOL	_FindPath( void );

	// 두 포인터의 스피어를 기준으로 영역을 설정한다.( Tool에서만 계산..? )
	void	_CalcMovementArea( void );

	// 포인터들
	INT32	_getNavPointCount( void )			{ return m_pNvPoint->getNavPointCount(); }
	I3_NAVPOINT_WITHSPHERE * _getNavPoint( UINT16 idx ) { return (I3_NAVPOINT_WITHSPHERE*) m_pNvPoint->getNavPoint( idx ); }

	i3NavigationPointPathNode2 * _getOpenNode( INT32 idx )	{ return (i3NavigationPointPathNode2*) m_pOpenList->Items[ idx ]; }
	void	_deleteOpenNode( INT32 idx )		{ m_pOpenList->Delete( idx ); }

	
public:
	UINT32	getFindType( void )					{ return m_FindType; }
	void	setFindType( UINT32 type )			{ m_FindType = type; }

	UINT16	getStartIndex( void )				{ return m_nStartIndex; }
	void	setStartIndex( UINT16 idx )			{ m_nStartIndex = idx; }

	UINT16	getTargetIndex( void )				{ return m_nTargetIndex; }
	void	setTargetIndex( UINT16 idx )		{ m_nTargetIndex = idx; }

	BOOL	IsFound( void )						{ return m_bFoundTarget; }
	BOOL	IsDone( void )						{ return m_bIsDone;	}

	VEC3D * getTargetVec( void )				{ return &m_vecTarget; }
	void	setTargetVec( VEC3D * pVec )		{ i3Vector::Copy( &m_vecTarget, pVec ); }

	void	setHeuristicWeight( REAL32 HeuristicWeight )	{ m_HeuristicWeight	= HeuristicWeight; }

public:
	i3NavigationPoint2Finder();
	virtual ~i3NavigationPoint2Finder();

	void	Create( i3NavigationPoint2 * pNavPoint, REAL32 HeuristicWeight );
	
	// 시작점에서 목표점까지의 Path를 찾는다.
	// 찾은 Path는 Waypoint에 저장된다.
	BOOL	Run( UINT8 startIndex, UINT8 targetIndex, UINT32 FindType )
	{
		setStartIndex( startIndex );
		setTargetIndex( targetIndex );
		setFindType( FindType );
		return _FindPath();
	}

	// 좌표로 가장 가까운 Path Index를 찾는다.
	// return I3_PATHFIND_NOTFOUND 인 경우 getNearestPathPoint()로 가장 가까운 포인터를 가져온다.(그 포인트의 Area에는 포함되어 있지 않다. )
	UINT16	FindNearPath( VEC3D * pPos );
	VEC3D	GetTargetPosition( void );

	i3NavigationPointPathNode2* GetFromClosedList( UINT16 nIndex );

	void	CleanUp( void );

};

#endif // __I3_NAVIGATIONPOINT2_PATHFINDER_H
