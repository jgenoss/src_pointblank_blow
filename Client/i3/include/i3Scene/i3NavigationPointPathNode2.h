#if !defined( __I3_NAVIGATION_POINTPATHNODE_H )
#define __I3_NAVIGATION_POINTPATHNODE_H

#define FINAL_COST_WORST	3.402823466e+38F	// FLT_MAX

class I3_EXPORT_SCENE i3NavigationPointPathNode2 : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3NavigationPointPathNode2, i3ElementBase );
protected:
	UINT16	m_nIndex = 0;
	VEC3D	m_vecMiddle;

	REAL32	m_HeuristicCost = 0.0f;	// the heuristic of the node
	REAL32	m_GivenCost = 0.0f;		// the given cost

	REAL32	m_FinalCost = FINAL_COST_WORST;		// hight resolution (double) final cost

	static INT32	m_nNodesConstructed;	// counter that counts the number of nodes ever constructed 
	static INT32	m_nNodesDestructed;		// counter that counts the number of nodes ever destructed

	i3NavigationPointPathNode2	*m_pParent = nullptr;				
	i3NavigationPointPathNode2	*hashTableNext = nullptr;			// pointer used by the hash table

public:
	UINT16	getIndex( void )				{ return m_nIndex; }
	void	setIndex( UINT16 idx )			{ m_nIndex = idx; }

	VEC3D *	getMiddle()						{ return &m_vecMiddle; }
	void	setMiddle( VEC3D * pVec )		{ i3Vector::Copy( &m_vecMiddle, pVec ); }

	REAL32	getHeuristicCost( void )		{ return m_HeuristicCost; }
	void	setHeuristicCost( REAL32 Cost )	{ m_HeuristicCost = Cost; }

	REAL32	getGivenCost( void )			{ return m_GivenCost; }
	void	setGivenCost( REAL32 Cost )		{ m_GivenCost = Cost; }

	REAL32	getFinalCost( void )			{ return m_FinalCost; }
	void	setFinalCost( REAL32 Cost )		{ m_FinalCost = Cost; }

	i3NavigationPointPathNode2 * getParent( void )	{ return m_pParent; }
	void	setParent( i3NavigationPointPathNode2 * pNode )	{ m_pParent = pNode; }

	i3NavigationPointPathNode2 * getNext( void )		{ return hashTableNext; }
	void	setNext( i3NavigationPointPathNode2 * pNode )	{ hashTableNext = pNode; }

public:
	i3NavigationPointPathNode2();
	virtual ~i3NavigationPointPathNode2();

	void	Create( UINT16 nIndex, VEC3D *pMiddle, i3NavigationPointPathNode2 *pParent );

	// computes the heuristic of a node, with respect to the goal 
	void	ComputeHeuristicCost( VEC3D *pGoal );

	// writes the significant data of the node to the console
	void	Dump();

	INT32	compareAStar( i3NavigationPointPathNode2 *second )
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

#endif //
