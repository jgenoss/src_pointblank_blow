#ifndef	__I3_NAVIGATION_POINT_PATH_FINDER_H__
#define __I3_NAVIGATION_POINT_PATH_FINDER_H__

class i3NavigationPoint;
class i3NavigationPointPathNode;

struct NvPointPathNodeCostComp
{
	bool operator()(const i3NavigationPointPathNode* p1, const i3NavigationPointPathNode* p2) const;
};

struct NvPointPathNodeIndexComp
{
	bool operator()(const i3NavigationPointPathNode* p1, const i3NavigationPointPathNode* p2) const;
};
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3NavigationPointPathFinder : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationPointPathFinder, i3ElementBase );
	
protected:
	i3NavigationPoint	*m_pNvPoint = nullptr;
	i3NavigationPointPathNode	*m_pCurrent = nullptr;

	i3::vector_multiset<i3NavigationPointPathNode*, NvPointPathNodeCostComp> 			m_OpenList;
	i3::vector_multiset<i3NavigationPointPathNode*, NvPointPathNodeIndexComp>			m_CreatedList;
	i3::vector<i3NavigationPointPathNode*>	m_Solution;

	VEC2D	m_vecGoal;
	UINT8	m_nStartIndex = 0, m_nGoalIndex = 0;

	bool	m_bFoundGoal = false;
	bool	m_bIsDone = false;

	REAL32	m_HeuristicWeight = 0.0f;
	
public:
	virtual ~i3NavigationPointPathFinder();

	// 길찾기 결과 얻어옴. 저장 속도 문제로 역순으로 저장되어 있음.
	const i3::vector<i3NavigationPointPathNode*>& GetSoultion(){ return m_Solution;	}
	void SetHeuristicWeight( REAL32 HeuristicWeight )	{ 	m_HeuristicWeight	= HeuristicWeight;	}

	void Create( i3NavigationPoint *pNvPoint, UINT8 StartIndex, UINT8 GoalIndex, REAL32 HeuristicWeight );
	void Run();
	bool IsDone(){ 	return m_bIsDone;	}
	void ShowInfo();
	void CleanUp();
	i3NavigationPointPathNode* GetFromClosedList( UINT8 nIndex );
};

#endif // __I3_NAVIGATION_POINT_PATH_FINDER_H__
