#if !defined( I3_OPT_OCTREE_NODE_H)
#define I3_OPT_OCTREE_NODE_H

#include "i3OptMeshInfo.h"

class i3OptBuildOcTree;
// 익스포트추가->다시 제거. (12.09.19.수빈)

class i3OptOctreeNode : public i3OcTreeNode
{
	I3_CLASS_DEFINE( i3OptOctreeNode, i3OcTreeNode);
protected:
	i3::vector<MESH_INFO*>	m_MeshList;
	i3BoundBox				m_Bound;

protected:
	REAL32			_FindPartitionPoint( i3BoundBox * pBox, VEC3D * pAxis, i3OptBuildOcTree * pOpt);

	void			_SplitMesh( VEC4D * pPlane, INT32 axis, i3OptBuildOcTree * pOpt);
	bool			_IsUsedCenter( VEC3D * pCenter);

	void			_DistributeMesh_Split( bool bSplitX, bool bSplitY, bool bSplitZ, i3OptBuildOcTree * pOpt, VEC3D * pCenter, VEC3D * pSpaceSize);
	void			_DistributeMesh_NonSplit( i3OptBuildOcTree * pOpt, VEC3D * pCenter, i3BoundBox * pSpace);

public:
	virtual ~i3OptOctreeNode(void);

	i3BoundBox *	getWholeBound(void)				{ return &m_Bound; }
	VEC3D *			getWholeBoundMin(void)			{ return m_Bound.GetMin(); }
	VEC3D *			getWholeBoundMax(void)			{ return m_Bound.GetMax(); }

	INT32			getMeshCount(void)				{ return INT32(m_MeshList.size()); }
	INT32			getTotalTriCount(void);
	MESH_INFO *		getMesh( INT32 idx)				{ return m_MeshList[idx]; }
	void			addMesh( i3Geometry * pGeo);
	void			addMesh( MESH_INFO * pInfo);
	void			removeMesh( MESH_INFO * pInfo);
	void			removeAllMesh(void);
	void			vlogAllMesh(void);

	void			Build( i3BoundBox * pSpace, i3OptBuildOcTree * pOpt, char * pszPath = nullptr);

	i3OcTreeNode *	ConvertTo( i3OcTreeNode * pParent);
};

#endif
