#ifndef	__I3_NAVIGATION_MESH_PATH_FINDER_H__
#define __I3_NAVIGATION_MESH_PATH_FINDER_H__

#include "i3NavigationMesh.h"

class I3_EXPORT_SCENE i3NvMeshPathNode : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NvMeshPathNode, i3ElementBase );

public:
	I3NM_INDEX	m_nIndex = 0;
	VEC2D	m_vecMiddle;

	REAL32	m_HeuristicCost = 0.0f;	// the heuristic of the node
	REAL32	m_GivenCost = 0.0f;		// the given cost

	REAL32	m_FinalCost;		// hight resolution (double) final cost [initialize at constructor]

	static INT32	m_nNodesConstructed;	// counter that counts the number of nodes ever constructed 
	static INT32	m_nNodesDestructed;	// counter that counts the number of nodes ever destructed

	i3NvMeshPathNode	*m_pParent = nullptr;
	i3NvMeshPathNode	*hashTableNext = nullptr;			// pointer used by the hash table


	i3NvMeshPathNode();
	virtual ~i3NvMeshPathNode();

	void	Create( I3NM_INDEX nIndex, VEC2D *pMiddle, i3NvMeshPathNode *pParent );
	VEC2D*	GetMiddle(){ return &m_vecMiddle; }

	// computes the heuristic of a node, with respect to the goal 
	void	ComputeHeuristicCost( VEC2D *pGoal );

	// writes the significant data of the node to the console
	void	Dump();

	int compareAStar( const i3NvMeshPathNode *second ) const
	{
		if ( second->m_FinalCost > m_FinalCost )
		{
			return 1; // Cost가 크므로 앞쪽에 배치
		}
		else if ( second->m_FinalCost < m_FinalCost )
		{
			return -1; // Cost가 작으므로 뒤에다가 배치
		}

		return 0;
	}
};

struct NvMeshPathNodeCostComp
{
	bool operator()(const i3NvMeshPathNode * pNode1, const i3NvMeshPathNode * pNode2 ) const {	return pNode1->compareAStar( pNode2 ) < 0; 	}
};
struct NvMeshPathNodeIndexComp
{
	bool operator()(const i3NvMeshPathNode * pNode1, const i3NvMeshPathNode * pNode2 ) const {	return pNode1->m_nIndex < pNode2->m_nIndex;	}
};
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3NavigationMeshPathFinder : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationMeshPathFinder, i3ElementBase );
	
protected:
	i3NavigationMesh	*m_pNvMesh = nullptr;

	i3NvMeshPathNode	*m_pCurrent = nullptr;

	i3::vector_multiset<i3NvMeshPathNode*, NvMeshPathNodeCostComp> m_OpenList;
	i3::vector_multiset<i3NvMeshPathNode*, NvMeshPathNodeIndexComp> m_CreatedList;
	i3::vector<i3NvMeshPathNode*> m_Solution;

	VEC2D		m_vecGoal;
	I3NM_INDEX	m_nStartIndex = 0, m_nGoalIndex = 0;

	bool	m_bFoundGoal = false;
	bool	m_bIsDone = false;

	REAL32	m_HeuristicWeight = 1.0f;
	
public:
	virtual ~i3NavigationMeshPathFinder();

	// 길찾기 결과 얻어옴. 저장 속도 문제로 역순으로 저장되어 있음.
	const i3::vector<i3NvMeshPathNode*>& GetSoultion(){ return m_Solution;	}
	void SetHeuristicWeight( REAL32 HeuristicWeight )	{ 	m_HeuristicWeight	= HeuristicWeight;	}

	void Create( i3NavigationMesh *pNvMesh, I3NM_INDEX StartIndex, I3NM_INDEX GoalIndex, REAL32 HeuristicWeight );
	bool Run();
	bool IsDone(){ 	return m_bIsDone;	}
	void ShowInfo();
	void CleanUp();
	i3NvMeshPathNode* GetFromClosedList( I3NM_INDEX nIndex );
};

#endif // __I3_NAVIGATION_MESH_PATH_FINDER_H__
