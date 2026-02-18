#include "i3SceneDef.h"
#include "i3NavigationPoint2PathFinder.h"
#include "i3NavigationPoint2.h"
#include "i3NavigationPointPathNode2.h"
#include "i3Base/itl/vector_set_util.h"
#include "i3Base/itl/container_util.h"


I3_CLASS_INSTANCE( i3NavigationPoint2Finder);

i3NavigationPoint2Finder::~i3NavigationPoint2Finder()
{
	_ReleasePath();
}

void i3NavigationPoint2Finder::_ReleasePath( void )
{
	/*
	INT32	i;
	I3_NAVPOINT_WITHSPHERE * pPoint;

	if( m_pNvPoint )
	{
		for( i = 0; i < _getNavPointCount(); i++ )
		{	
			pPoint = _getNavPoint( (UINT16) i );

			if( pPoint )
			{
				if( pPoint[ i ].m_pNeighbor )
				{
					i3MemFree( pPoint[ i ].m_pNeighbor );
				}

				i3MemFree( pPoint );
			}
		}

		I3_SAFE_RELEASE( m_pNvPoint );
	}*/
}

void i3NavigationPoint2Finder::Create( i3NavigationPoint2 * pNavPoint, REAL32 HeuristicWeight )
{

}

void i3NavigationPoint2Finder::_CalcMovementArea( void )
{
	// 두 스피어 사이의 영역 계산

	// 두 스피어로 이어지는 박스를 데이터화 해야한다.
}

i3NavigationPointPathNode2 * i3NavigationPoint2Finder::GetFromClosedList( UINT16 nIndex )
{
	i3NavigationPointPathNode2 Node;
	Node.setIndex( nIndex );

	i3::vector_multiset<i3NavigationPointPathNode2*>::iterator it = m_CreatedList.find(&Node);

	if( it == m_CreatedList.end() )
		return nullptr;
	
	return *it;
}

// 적은 코스트로 이동 할 수 있는 포인터 리스트 작성
bool i3NavigationPoint2Finder::_FindPathWay( void )
{
	INT32	i;
	INT32	k;
	INT32	nOpenListCount = 0;
	
	i3NavigationPointPathNode2	*pSuccessor = nullptr;
	i3NavigationPointPathNode2	*pFoundNode = nullptr;
	
	while( ( nOpenListCount = (INT32)m_OpenList.size() ) > 0 )
	{
		// OpenList에서 가장 비용이 싼 노드 얻기
		m_pCurrent = _getOpenNode( nOpenListCount - 1 );
		// 해당 노드를 지워준다.
		_deleteOpenNode( nOpenListCount - 1 );

		// is it the goal?
		if( m_pCurrent->getIndex() == m_nTargetIndex )
		{
			//I3TRACE( "goal found\n" );
			m_bFoundTarget = true;

			// 솔루션 리스트 작성. 역순으로 저장됨.
			while( m_pCurrent != nullptr )
			{
				m_Solution.push_back( m_pCurrent );
				m_pCurrent = m_pCurrent->getParent();
			}

#if 0
			I3TRACE( "Soultion : " );
			for( i = 0; i < m_pSolution->GetCount(); i++ )
			{
				i3NavigationPointPathNode2 *pNode = (i3NavigationPointPathNode2 *)m_pSolution->GetItem( i );
				if( i == 0 )
				{
					I3TRACE( "%d", pNode->getIndex() );
				}
				else
				{
					I3TRACE( ", %d", pNode->getIndex() );
				}				
			}
			I3TRACE( "\n" );
#endif

			break;
		}

		// for every possible pSuccessor
		for( i = 0; i < m_pNvPoint->getNeighborCount(); i++ )
		{	
			UINT16	successorIndex;
			VEC3D *	pMiddlePos;
			VEC3D	vecDir;
			REAL32	DistanceCost;
			REAL32	successorG;
			I3_NAVPOINT_WITHSPHERE *pTri = nullptr;
			
			successorIndex = m_pNvPoint->getNeighborIndex( i );
			if( successorIndex == I3_NAVPOINT_INDEX_NONE )
				continue;
			
			// 이웃한 포인터
			pTri = m_pNvPoint->GetNeighbor( i );
			I3ASSERT( pTri != nullptr );

			// 이웃한 포인터의 위치를 목표점의 중간점으로 설정
			pMiddlePos = &pTri->m_vecPosition;

			// 중점간 거리로 계산
			i3Vector::Sub( &vecDir, m_pCurrent->getMiddle(), pMiddlePos );
			DistanceCost = i3Vector::Length( &vecDir );
			successorG = m_pCurrent->getGivenCost() + DistanceCost;

			// closed list에 있으면 재활용.
			pFoundNode = GetFromClosedList( successorIndex );

			// skip nodes that we have already generated AND are worst.
			if( pFoundNode && (pFoundNode->getGivenCost() <= successorG ) )
				continue;

			if( pFoundNode)
			{
				// OpenList에 있다면, 제거!
				INT32	nOpenCount = (INT32) m_OpenList.size();

				for( k = 0; k < nOpenCount; k++ )
				{
					i3NavigationPointPathNode2 *pNode = m_OpenList.get_vector()[k];

					if( successorIndex == pNode->getIndex() )
					{
						// m_pOpenList->Remove( pNode );
						i3::vsu::linear_remove_1st(m_OpenList, pNode);
						break;
					}
				}

				pFoundNode->setParent( m_pCurrent );
				pFoundNode->setGivenCost( successorG );
				pFoundNode->setFinalCost( pFoundNode->getHeuristicCost() + m_HeuristicWeight * pFoundNode->getGivenCost() );

				// OpenList에 추가하기
				m_OpenList.insert( pFoundNode );
			}
			else
			{
				// 자식 설정
				pSuccessor = i3NavigationPointPathNode2::new_object();
				pSuccessor->Create( successorIndex, pMiddlePos, m_pCurrent );
				pSuccessor->setGivenCost( successorG );
				pSuccessor->ComputeHeuristicCost( &m_vecTarget );
				pSuccessor->setFinalCost( pSuccessor->getGivenCost() + m_HeuristicWeight * pSuccessor->getHeuristicCost() );

				// OpenList에 추가하기
				m_OpenList.insert( pSuccessor );

				// 생성 리스트에 추가하기
				m_CreatedList.insert( pSuccessor );
			}			
		}

		// 주어진 시간을 초과한 경우 그냥 리턴할 수도 있다.
	}

	return false;
	
}


bool i3NavigationPoint2Finder::_FindPath( void )
{
	bool	Result = false;

	// 일단 waypoint를 찾는다.
	Result = _FindPathWay();

	if( getFindType() == I3_PATHFIND_SHORTCUT )
		Result = _FindShortCut();
	else if( getFindType() == I3_PATHFIND_RANDWAY )
		Result = _FindRandWay();

	return Result;
}

UINT16 i3NavigationPoint2Finder::FindNearPath( VEC3D * pPos )
{
	INT32	i;
	VEC3D	Vec;
	REAL32	Length;
	REAL32	ShortLength = 10000.0f;
	I3_NAVPOINT_WITHSPHERE * pPoint;

	for( i = 0; i < _getNavPointCount(); i++ )
	{
		pPoint = _getNavPoint( (UINT16) i );

		i3Vector::Sub( &Vec, &pPoint->m_vecPosition, pPos );

		Length = i3Vector::Length( &Vec );

		if( Length < pPoint->m_fRadius )
		{
			// 해당 포인터의 Area에 속한다.
			return (UINT16) i;
		}
		else
		{
			if( ShortLength > Length )
			{
				// 현재 좌표에서 가까운 지점까지의 컬리젼 검사를 실시해야 된다.


				// 이전의 Path중 가장 가까운지 검사
				ShortLength = Length;
			}
		}
	}
	
	return I3_PATHFIND_NOTFOUND;
}


void i3NavigationPoint2Finder::CleanUp( void )
{
	i3::cu::for_each_safe_release_clear(m_CreatedList, &i3NavigationPointPathNode2::Release);
//	if( m_pCreatedList != nullptr )
//	{
//		m_pCreatedList->SafeReleaseAll();
//	}
//	I3_SAFE_RELEASE( m_pCreatedList );

	// 솔루션의 Item은 Create List 에 포함되어 있으므로 제거할 필요가 없음.
	m_Solution.clear();

	// OpenList의 Item의 Create List 에 포함되어 있으므로 제거할 필요가 없음.
	m_OpenList.clear();
}

