#include "i3SceneDef.h"
#include "i3NavigationPointPathNode2.h"

I3_CLASS_INSTANCE( i3NavigationPointPathNode2);

INT32 i3NavigationPointPathNode2::m_nNodesConstructed = 0;
INT32 i3NavigationPointPathNode2::m_nNodesDestructed  = 0;

i3NavigationPointPathNode2::i3NavigationPointPathNode2()
{
	m_nNodesConstructed++;
}

i3NavigationPointPathNode2::~i3NavigationPointPathNode2()
{
	m_nNodesDestructed++;
}

void i3NavigationPointPathNode2::Create( UINT16 nIndex, VEC3D *pMiddle, i3NavigationPointPathNode2 *pParent )
{
//	m_nNodesConstructed++;

	m_nIndex = nIndex;
	i3Vector::Copy( &m_vecMiddle, pMiddle );
	m_pParent = pParent;
}

void i3NavigationPointPathNode2::ComputeHeuristicCost( VEC3D *pGoal )
{
	VEC3D vecDir;
	i3Vector::Sub( &vecDir, pGoal, &m_vecMiddle );
	m_HeuristicCost = i3Vector::Length( &vecDir );
}

void i3NavigationPointPathNode2::Dump()
{
	I3TRACE("i3NavigationPointPathNode2: x: %f, y: %f, z: %f, g: %.1f h: %.1f, f: %f\n",
		i3Vector::GetX( &m_vecMiddle ), i3Vector::GetY( &m_vecMiddle ), i3Vector::GetZ( &m_vecMiddle ), 
		m_GivenCost, m_HeuristicCost, m_FinalCost
	);
}
