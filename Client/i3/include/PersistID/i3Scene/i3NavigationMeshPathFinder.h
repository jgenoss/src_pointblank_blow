#ifndef	__I3_NAVIGATION_MESH_PATH_FINDER_H__
#define __I3_NAVIGATION_MESH_PATH_FINDER_H__

#include "i3NavigationMesh.h"

class I3_EXPORT_SCENE i3NvMeshPathNode : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NvMeshPathNode );

public:
	I3NM_INDEX	m_nIndex;
	VEC2D	m_vecMiddle;

	REAL32	m_HeuristicCost;	// the heuristic of the node
	REAL32	m_GivenCost;		// the given cost

	REAL32	m_FinalCost;		// hight resolution (double) final cost

	static INT32	m_nNodesConstructed;	// counter that counts the number of nodes ever constructed 
	static INT32	m_nNodesDestructed;	// counter that counts the number of nodes ever destructed

	i3NvMeshPathNode	*m_pParent;				
	i3NvMeshPathNode	*hashTableNext;			// pointer used by the hash table


	i3NvMeshPathNode();
	virtual ~i3NvMeshPathNode();

	void	Create( I3NM_INDEX nIndex, VEC2D *pMiddle, i3NvMeshPathNode *pParent );
	VEC2D*	GetMiddle(){ return &m_vecMiddle; }

	// computes the heuristic of a node, with respect to the goal 
	void	ComputeHeuristicCost( VEC2D *pGoal );

	// writes the significant data of the node to the console
	void	Dump();

	int compareAStar( i3NvMeshPathNode *second )
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

class i3NavigationMeshPathFinder : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationMeshPathFinder );
	
protected:
	i3NavigationMesh	*m_pNvMesh;

	i3NvMeshPathNode	*m_pCurrent;

	i3BinList			*m_pOpenList;
	i3BinList			*m_pCreatedList;
	i3List				*m_pSolution;

	VEC2D		m_vecGoal;
	I3NM_INDEX	m_nStartIndex, m_nGoalIndex;

	BOOL	m_bFoundGoal;
	BOOL	m_bIsDone;

	REAL32	m_HeuristicWeight;
	
public:
	i3NavigationMeshPathFinder();
	virtual ~i3NavigationMeshPathFinder();

	// 길찾기 결과 얻어옴. 저장 속도 문제로 역순으로 저장되어 있음.
	i3List* GetSoultion(){ return m_pSolution;	}
	void SetHeuristicWeight( REAL32 HeuristicWeight )	{ 	m_HeuristicWeight	= HeuristicWeight;	}

	void Create( i3NavigationMesh *pNvMesh, I3NM_INDEX StartIndex, I3NM_INDEX GoalIndex, REAL32 HeuristicWeight );
	BOOL Run();
	BOOL IsDone(){ 	return m_bIsDone;	}
	void ShowInfo();
	void CleanUp();
	i3NvMeshPathNode* GetFromClosedList( I3NM_INDEX nIndex );
};

#endif // __I3_NAVIGATION_MESH_PATH_FINDER_H__
