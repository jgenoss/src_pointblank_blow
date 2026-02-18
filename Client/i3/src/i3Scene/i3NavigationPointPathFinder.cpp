#include "i3SceneDef.h"
#include "i3NavigationPointPathFinder.h"
#include "i3NavigationPoint.h"
#include "i3NavigationPointPathNode.h"
#include "i3Base/itl/vector_set_util.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3NavigationPointPathFinder);

// ============================================================================
// i3NavigationPointPathFinder
// ============================================================================

bool NvPointPathNodeCostComp::operator()(const i3NavigationPointPathNode* p1, const i3NavigationPointPathNode* p2) const
{
	return p1->compareAStar( p2 ) < 0;
}

bool NvPointPathNodeIndexComp::operator()(const i3NavigationPointPathNode* p1, const i3NavigationPointPathNode* p2) const
{
	return p1->getIndex() < p2->getIndex();
}

i3NavigationPointPathFinder::~i3NavigationPointPathFinder()
{
	CleanUp();
}

void i3NavigationPointPathFinder::Create( i3NavigationPoint *pNvPoint, UINT8 StartIndex, UINT8 GoalIndex, REAL32 HeuristicWeight )
{
	CleanUp();
 
	m_OpenList.clear();
	m_CreatedList.clear();

	i3Vector::Set( &m_vecGoal, 0.0f, 0.0f );

	m_pCurrent				= nullptr;
	m_bFoundGoal			= false;
	m_bIsDone = false;
	m_HeuristicWeight		= 1;


	m_pNvPoint			= pNvPoint;

	m_nStartIndex	= StartIndex;
	m_nGoalIndex	= GoalIndex;

	m_pCurrent		= nullptr;
	m_bFoundGoal	= false;
	m_bIsDone		= false;

	i3::pack::NAVPOINT_INFO *pGoal = m_pNvPoint->GetPoint( GoalIndex );
	i3Vector::Copy( &m_vecGoal, &pGoal->vecPos );	
	
	i3::pack::NAVPOINT_INFO *pStart = m_pNvPoint->GetPoint( StartIndex );
	i3NavigationPointPathNode *pRoot = i3NavigationPointPathNode::new_object();
	pRoot->Create( StartIndex, &pStart->vecPos, nullptr );

	pRoot->ComputeHeuristicCost( &m_vecGoal );
	pRoot->setFinalCost( m_HeuristicWeight * pRoot->getHeuristicCost() );

	m_OpenList.insert( pRoot );
	m_CreatedList.insert( pRoot );
}

void i3NavigationPointPathFinder::Run()
{
	i3NavigationPointPathNode	*pSuccessor = nullptr;
	i3NavigationPointPathNode	*pFoundNode = nullptr;

	INT32 nOpeListCount = 0;
	while( ( nOpeListCount = (INT32)m_OpenList.size() ) > 0 )
	{
		// OpenList에서 가장 비용이 싼 노드 얻기
		m_pCurrent = m_OpenList.get_vector()[nOpeListCount - 1 ];
		m_OpenList.erase(m_OpenList.begin() + nOpeListCount - 1 );

		// is it the goal?
		if( m_pCurrent->getIndex() == m_nGoalIndex )
		{
			//I3TRACE( "goal found\n" );
			m_bFoundGoal = true;

			// 솔루션 리스트 작성. 역순으로 저장됨.
			while( m_pCurrent != nullptr )
			{
				m_Solution.push_back( m_pCurrent );
				m_pCurrent = m_pCurrent->getParent();
			}

#if 0
			I3TRACE( "Soultion : " );
			INT32 i;
			for( i = 0; i < m_pSolution->GetCount(); i++ )
			{
				i3NavigationPointPathNode *pNode = (i3NavigationPointPathNode *)m_pSolution->GetItem( i );
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
		for( UINT8 i = 0; i < 3; i++ )
		{
			UINT8 successorIndex;
			i3::pack::VEC2D *pMiddlePos;

			i3::pack::NAVPOINT_INFO *pTri;
			successorIndex	= m_pNvPoint->GetNeighborIndex( m_pCurrent->getIndex(), i );

			if( successorIndex == I3_NVPOINT_INDEX_NONE ) continue;

			pTri = m_pNvPoint->GetPoint( successorIndex );
			I3ASSERT( pTri != nullptr );

			pMiddlePos = &pTri->vecPos;

			// pSuccessor의 주어진 비용 계산하기
			VEC2D  vecDir;
			REAL32 DistanceCost;
			REAL32 successorG;

			// 중점간 거리로 계산
			i3Vector::Sub( &vecDir, m_pCurrent->getMiddle(), &pTri->vecPos );
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
				size_t k, nOpenCount = m_OpenList.size();
				for( k = 0; k < nOpenCount; k++ )
				{
					i3NavigationPointPathNode *pNode = m_OpenList.get_vector()[k];

					if( successorIndex == pNode->getIndex() )
					{
						i3::vsu::linear_remove_1st(m_OpenList, pNode);
						// m_pOpenList->Remove( pNode );
						break;								// break추가...해당 반복문 루틴에 문제가 있어보여 추가한다..(2012.05.22)
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
				pSuccessor = i3NavigationPointPathNode::new_object();
				pSuccessor->Create( successorIndex, pMiddlePos, m_pCurrent );
				pSuccessor->setGivenCost( successorG );
				pSuccessor->ComputeHeuristicCost( &m_vecGoal );
				pSuccessor->setFinalCost( pSuccessor->getGivenCost() + m_HeuristicWeight * pSuccessor->getHeuristicCost() );

				// OpenList에 추가하기
				m_OpenList.insert( pSuccessor );

				// 생성 리스트에 추가하기
				m_CreatedList.insert( pSuccessor );
			}			
		}

		// 주어진 시간을 초과한 경우 그냥 리턴할 수도 있다.
	}

	I3ASSERT( m_bFoundGoal );
	m_bIsDone = true;
}

void i3NavigationPointPathFinder::ShowInfo()
{
}


void i3NavigationPointPathFinder::CleanUp()
{

	i3::cu::for_each_safe_release_clear(m_CreatedList, &i3NavigationPointPathNode::Release);
	/*
	if( m_pCreatedList != nullptr )
	{
		m_pCreatedList->SafeReleaseAll();
	}
	I3_SAFE_RELEASE( m_pCreatedList );
	*/

	// 솔루션의 Item은 Create List 에 포함되어 있으므로 제거할 필요가 없음.
	m_Solution.clear();

	// OpenList의 Item의 Create List 에 포함되어 있으므로 제거할 필요가 없음.
	//I3_SAFE_RELEASE( m_pOpenList );
	m_OpenList.clear();
}

i3NavigationPointPathNode* i3NavigationPointPathFinder::GetFromClosedList( UINT8 nIndex )
{
	i3NavigationPointPathNode Node;
	Node.setIndex( nIndex );

	i3::vector_multiset<i3NavigationPointPathNode*, NvPointPathNodeIndexComp>::iterator it = m_CreatedList.find(&Node);
	
	return (it != m_CreatedList.end()) ? *it : nullptr;
}
