#ifndef __I3_NAVIGATION_NODE_H__
#define __I3_NAVIGATION_NODE_H__

#include "i3Node.h"
#include "i3NavigationMesh.h"

class i3NavigationMesh;

enum I3_NVNODE_DIVISION
{
	I3_NVNODE_LT,		// Left Top
	I3_NVNODE_RT,		// Right Top
	I3_NVNODE_LB,		// Left Bottom
	I3_NVNODE_RB,		// Right Bottom
	I3_NVNODE_INSIDE,	// Inside
	I3_NVNODE_ETC,		// Outside or Intersect
};

class I3_EXPORT_SCENE i3NavigationMeshNode : public i3Node
{
	I3_EXPORT_CLASS_DEFINE( i3NavigationMeshNode, i3Node );
protected:
	I3NM_INDEX				m_TriIndex = I3_NVMESH_INDEX_NONE;

	i3NavigationMeshNode	*m_pNvMeshNodeChild[4] = { nullptr, nullptr, nullptr, nullptr };
	i3::vector<i3NavigationMeshNode*>	m_NvMeshLeafList;

public:
	bool IsNvMeshLeaf(){	return ( m_TriIndex != I3_NVMESH_INDEX_NONE );	}

	i3BoundBox* CreateBoundBoxForSubDivision( I3_NVNODE_DIVISION nDivision );
	I3_NVNODE_DIVISION GetSubdivionIndex( i3BoundBox *pBox );

	void	AddNvMeshNodeLeaf( i3NavigationMeshNode *pNode );

	void	SetTriagleIndex( I3NM_INDEX nTriIndex );

	I3NM_INDEX	GetMeshIndex( i3NavigationMesh *pMesh, VEC2D *pPos );

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual	UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

};

#endif //__I3_NAVIGATION_NODE_H__
