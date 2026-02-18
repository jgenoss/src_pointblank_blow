#ifndef	__I3_NAVIGATION_POINT_PATH_FINDER_H__
#define __I3_NAVIGATION_POINT_PATH_FINDER_H__

class i3NavigationPoint;
class i3NavigationPointPathNode;

class i3NavigationPointPathFinder : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationPointPathFinder );
	
protected:
	i3NavigationPoint	*m_pNvPoint;

	i3NavigationPointPathNode	*m_pCurrent;

	i3BinList			*m_pOpenList;
	i3BinList			*m_pCreatedList;
	i3List				*m_pSolution;

	VEC2D	m_vecGoal;
	UINT8	m_nStartIndex, m_nGoalIndex;

	BOOL	m_bFoundGoal;
	BOOL	m_bIsDone;

	REAL32	m_HeuristicWeight;
	
public:
	i3NavigationPointPathFinder();
	virtual ~i3NavigationPointPathFinder();

	// 길찾기 결과 얻어옴. 저장 속도 문제로 역순으로 저장되어 있음.
	i3List* GetSoultion(){ return m_pSolution;	}
	void SetHeuristicWeight( REAL32 HeuristicWeight )	{ 	m_HeuristicWeight	= HeuristicWeight;	}

	void Create( i3NavigationPoint *pNvPoint, UINT8 StartIndex, UINT8 GoalIndex, REAL32 HeuristicWeight );
	void Run();
	BOOL IsDone(){ 	return m_bIsDone;	}
	void ShowInfo();
	void CleanUp();
	i3NavigationPointPathNode* GetFromClosedList( UINT8 nIndex );
};

#endif // __I3_NAVIGATION_POINT_PATH_FINDER_H__
