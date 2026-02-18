#include "i3SceneDef.h"
#include "i3NavigationMeshPathFinder.h"
#include "i3NavigationMesh.h"

#include "i3Base/itl/vector_set_util.h"
#include "i3Base/itl/container_util.h"


I3_CLASS_INSTANCE( i3NvMeshPathNode);
I3_CLASS_INSTANCE( i3NavigationMeshPathFinder);

//WARNING: the should be the upper bound. for biggerMaps this has to increase
#define FINAL_COST_WORST	3.402823466e+38F	// FLT_MAX


// ============================================================================
// 
// ============================================================================
INT32 i3NvMeshPathNode::m_nNodesConstructed = 0;
INT32 i3NvMeshPathNode::m_nNodesDestructed  = 0;

i3NvMeshPathNode::i3NvMeshPathNode() : m_FinalCost(FINAL_COST_WORST)
{
	m_nNodesConstructed++;
}

i3NvMeshPathNode::~i3NvMeshPathNode()
{
	m_nNodesDestructed++;
}

void i3NvMeshPathNode::Create( I3NM_INDEX nIndex, VEC2D *pMiddle, i3NvMeshPathNode *pParent )
{
	m_nNodesConstructed++;

	m_GivenCost = m_HeuristicCost = 0;
	//m_pHashTableNext = nullptr;
	m_FinalCost = FINAL_COST_WORST;

	m_nIndex = nIndex;
	i3Vector::Copy( &m_vecMiddle, pMiddle );
	m_pParent = pParent;
}

void i3NvMeshPathNode::ComputeHeuristicCost( VEC2D *pGoal )
{
	VEC2D vecDir;
	i3Vector::Sub( &vecDir, pGoal, &m_vecMiddle );
	m_HeuristicCost = i3Vector::Length( &vecDir );
}

void i3NvMeshPathNode::Dump()
{
	I3TRACE("i3NvMeshPathNode: x: %f, y: %f, g: %.1f h: %.1f, f: %f\n",
		i3Vector::GetX( &m_vecMiddle ), i3Vector::GetY( &m_vecMiddle ),
		m_GivenCost, m_HeuristicCost, m_FinalCost
	);
}

// ============================================================================
// i3NavigationMeshPathFinder
// ============================================================================
/*
static INT32	_NvMeshCostCompareProc( i3NvMeshPathNode * pNode1, i3NvMeshPathNode * pNode2 )
{
	return pNode1->compareAStar( pNode2 );
}

static INT32	_NvMeshIndexCompareProc( i3NvMeshPathNode * pNode1, i3NvMeshPathNode * pNode2 )
{
	return pNode1->m_nIndex - pNode2->m_nIndex;
}
*/


i3NavigationMeshPathFinder::~i3NavigationMeshPathFinder()
{
	CleanUp();
}

void i3NavigationMeshPathFinder::Create( i3NavigationMesh *pNvMesh, I3NM_INDEX StartIndex, I3NM_INDEX GoalIndex, REAL32 HeuristicWeight )
{
	CleanUp();
 
	m_OpenList.clear();
	m_CreatedList.clear();
	
	i3Vector::Set( &m_vecGoal, 0.0f, 0.0f );

	m_pCurrent				= nullptr;
	m_bFoundGoal			= false;
	m_bIsDone = false;
	m_HeuristicWeight		= 1.0f;


	m_pNvMesh			= pNvMesh;

	m_nStartIndex	= StartIndex;
	m_nGoalIndex	= GoalIndex;

	m_pCurrent		= nullptr;
	m_bFoundGoal	= false;
	m_bIsDone		= false;

	i3Vector::Copy( &m_vecGoal, m_pNvMesh->GetTriMiddlePos( GoalIndex ) );
	
	i3NvMeshPathNode *pRoot = i3NvMeshPathNode::new_object();
	pRoot->Create( StartIndex, m_pNvMesh->GetTriMiddlePos( StartIndex ), nullptr );

	pRoot->ComputeHeuristicCost( &m_vecGoal );
	pRoot->m_FinalCost = m_HeuristicWeight * pRoot->m_HeuristicCost;

	m_OpenList.insert( pRoot );
	m_CreatedList.insert( pRoot );
}

bool i3NavigationMeshPathFinder::Run()
{
	i3NvMeshPathNode	*pSuccessor = nullptr;
	i3NvMeshPathNode	*pFoundNode = nullptr;

	INT32 nOpeListCount = 0;
	while( ( nOpeListCount = (INT32)m_OpenList.size() ) > 0 )
	{
		// OpenList에서 가장 비용이 싼 노드 얻기
		m_pCurrent = (i3NvMeshPathNode*)m_OpenList.get_vector()[nOpeListCount - 1 ];
		m_OpenList.erase( m_OpenList.begin() + nOpeListCount - 1 );

		// is it the goal?
		if( m_pCurrent->m_nIndex == m_nGoalIndex )
		{
			//I3TRACE( "goal found\n" );
			m_bFoundGoal = true;

			// 솔루션 리스트 작성. 역순으로 저장됨.
			while( m_pCurrent != nullptr )
			{
				m_Solution.push_back( m_pCurrent );
				m_pCurrent = m_pCurrent->m_pParent;
			}

#if 0
			I3TRACE( "Soultion : " );
			INT32 i;
			for( i = 0; i < m_pSolution->GetCount(); i++ )
			{
				i3NvMeshPathNode *pNode = (i3NvMeshPathNode *)m_pSolution->GetItem( i );
				if( i == 0 )
				{
					I3TRACE( "%d", pNode->m_nIndex );
				}
				else
				{
					I3TRACE( ", %d", pNode->m_nIndex );
				}				
			}
			I3TRACE( "\n" );
#endif

			break;
		}

		// for every possible pSuccessor
		for( I3NM_DIR i = 0; i < I3_MAX_NVMESH_DIR; i++ )
		{
			I3NM_INDEX successorIndex;
			VEC2D *pMiddlePos;

			successorIndex	= m_pNvMesh->GetNeighborIndex( m_pCurrent->m_nIndex, i );

			if( successorIndex == I3_NVMESH_INDEX_NONE ) continue;

			pMiddlePos = m_pNvMesh->GetTriMiddlePos( successorIndex );

			// pSuccessor의 주어진 비용 계산하기
			VEC2D  vecDir;
			REAL32 DistanceCost;
			REAL32 successorG;

			// 중점간 거리로 계산
			i3Vector::Sub( &vecDir, m_pCurrent->GetMiddle(), pMiddlePos );
			DistanceCost = i3Vector::Length( &vecDir );
			successorG = m_pCurrent->m_GivenCost + DistanceCost;

			// closed list에 있으면 재활용.
			pFoundNode = GetFromClosedList( successorIndex );

			// skip nodes that we have already generated AND are worst.
			if( pFoundNode && (pFoundNode->m_GivenCost <= successorG ) )
				continue;

			if( pFoundNode)
			{
				// OpenList에 있다면, 제거!
				size_t k, nOpenCount = m_OpenList.size();

				for( k = 0; k < nOpenCount; k++ )
				{
					i3NvMeshPathNode *pNode = m_OpenList.get_vector()[k];

					if( successorIndex == pNode->m_nIndex )
					{
						i3::vsu::linear_remove_1st(m_OpenList, pNode );		// break가 없었기 때문에, 삭제된 다음번노드가 무효화될수도 있고, 인덱스도 맞지 않는다..(2012.05.22.수빈)
						break;												// 만약...노드 유일성이 보장된다면 간단하게..break;를 거는게 좋겠다..break추가..
					}
				}

				pFoundNode->m_pParent	= m_pCurrent;
				pFoundNode->m_GivenCost	= successorG;
				pFoundNode->m_FinalCost	= pFoundNode->m_HeuristicCost + m_HeuristicWeight * pFoundNode->m_GivenCost;

				// OpenList에 추가하기
				m_OpenList.insert( pFoundNode );
			}
			else
			{
				// 자식 설정
				pSuccessor = i3NvMeshPathNode::new_object();
				pSuccessor->Create( successorIndex, pMiddlePos, m_pCurrent );
				pSuccessor->m_GivenCost = successorG;
				pSuccessor->ComputeHeuristicCost( &m_vecGoal );
				pSuccessor->m_FinalCost = pSuccessor->m_GivenCost + m_HeuristicWeight * pSuccessor->m_HeuristicCost;

				// OpenList에 추가하기
				m_OpenList.insert( pSuccessor );

				// 생성 리스트에 추가하기
				m_CreatedList.insert( pSuccessor );
			}			
		}

		// 주어진 시간을 초과한 경우 그냥 리턴할 수도 있다.
	}

	m_bIsDone = true;

	return m_bFoundGoal;
}

void i3NavigationMeshPathFinder::ShowInfo()
{
}


void i3NavigationMeshPathFinder::CleanUp()
{
	i3::cu::for_each_safe_release_clear(m_CreatedList, &i3NvMeshPathNode::Release);
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

i3NvMeshPathNode* i3NavigationMeshPathFinder::GetFromClosedList( I3NM_INDEX nIndex )
{
	i3NvMeshPathNode Node;
	Node.m_nIndex = nIndex;

//	INT32 nFound = m_pCreatedList->FindItem( &Node );
	i3::vector_multiset<i3NvMeshPathNode*, NvMeshPathNodeIndexComp>::iterator it = m_CreatedList.find(&Node); 

	if ( it == m_CreatedList.end() ) return nullptr;

	return *it;
}
