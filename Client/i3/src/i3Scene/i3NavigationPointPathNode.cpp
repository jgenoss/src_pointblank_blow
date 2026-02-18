#include "i3SceneDef.h"
#include "i3NavigationPointPathNode.h"

I3_CLASS_INSTANCE( i3NavigationPointPathNode);

INT32 i3NavigationPointPathNode::m_nNodesConstructed = 0;
INT32 i3NavigationPointPathNode::m_nNodesDestructed  = 0;

i3NavigationPointPathNode::i3NavigationPointPathNode()
{
	m_nNodesConstructed++;
}

i3NavigationPointPathNode::~i3NavigationPointPathNode()
{
	m_nNodesDestructed++;
}

void i3NavigationPointPathNode::Create( UINT8 nIndex, i3::pack::VEC2D *pMiddle, i3NavigationPointPathNode *pParent )
{
//	m_nNodesConstructed++;

	m_nIndex = nIndex;
	i3Vector::Copy( &m_vecMiddle, pMiddle );
	m_pParent = pParent;
}

void i3NavigationPointPathNode::ComputeHeuristicCost( VEC2D *pGoal )
{
	VEC2D vecDir;
	i3Vector::Sub( &vecDir, pGoal, &m_vecMiddle );
	m_HeuristicCost = i3Vector::Length( &vecDir );
}

void i3NavigationPointPathNode::Dump()
{
	I3TRACE("i3NavigationPointPathNode: x: %f, y: %f, g: %.1f h: %.1f, f: %f\n",
		i3Vector::GetX( &m_vecMiddle ), i3Vector::GetY( &m_vecMiddle ),
		m_GivenCost, m_HeuristicCost, m_FinalCost
	);
}

