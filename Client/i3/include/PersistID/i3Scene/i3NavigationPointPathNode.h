#if !defined( __I3_NAVIGATION_POINTPATHNODE_H )
#define __I3_NAVIGATION_POINTPATHNODE_H


//WARNING: the should be the upper bound. for biggerMaps this has to increase
#define FINAL_COST_WORST	3.402823466e+38F	// FLT_MAX

class I3_EXPORT_SCENE i3NavigationPointPathNode : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavigationPointPathNode );
protected:
	UINT8	m_nIndex;
	VEC2D	m_vecMiddle;

	REAL32	m_HeuristicCost;	// the heuristic of the node
	REAL32	m_GivenCost;		// the given cost

	REAL32	m_FinalCost;		// hight resolution (double) final cost

	static INT32	m_nNodesConstructed;	// counter that counts the number of nodes ever constructed 
	static INT32	m_nNodesDestructed;		// counter that counts the number of nodes ever destructed

	i3NavigationPointPathNode	*m_pParent;				
	i3NavigationPointPathNode	*hashTableNext;			// pointer used by the hash table

public:
	UINT8	getIndex( void )				{ return m_nIndex; }
	void	setIndex( UINT8 idx )			{ m_nIndex = idx; }

	VEC2D *	getMiddle()						{ return &m_vecMiddle; }
	void	setMiddle( VEC2D * pVec )		{ i3Vector::Copy( &m_vecMiddle, pVec ); }

	REAL32	getHeuristicCost( void )		{ return m_HeuristicCost; }
	void	setHeuristicCost( REAL32 Cost )	{ m_HeuristicCost = Cost; }

	REAL32	getGivenCost( void )			{ return m_GivenCost; }
	void	setGivenCost( REAL32 Cost )		{ m_GivenCost = Cost; }

	REAL32	getFinalCost( void )			{ return m_FinalCost; }
	void	setFinalCost( REAL32 Cost )		{ m_FinalCost = Cost; }

	i3NavigationPointPathNode *	getParent( void )	{ return m_pParent; }
	void	setParent( i3NavigationPointPathNode * pNode )	{ m_pParent = pNode; }

	i3NavigationPointPathNode * getNext( void )		{ return hashTableNext; }
	void	setNext( i3NavigationPointPathNode * pNode )	{ hashTableNext = pNode; }

public:
	i3NavigationPointPathNode();
	virtual ~i3NavigationPointPathNode();

	void	Create( UINT8 nIndex, VEC2D *pMiddle, i3NavigationPointPathNode *pParent );

	// computes the heuristic of a node, with respect to the goal 
	void	ComputeHeuristicCost( VEC2D *pGoal );

	// writes the significant data of the node to the console
	void	Dump();

	INT32 compareAStar( i3NavigationPointPathNode *second )
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


#endif	// __I3_NAVIGATION_POINTPATHNODE_H
