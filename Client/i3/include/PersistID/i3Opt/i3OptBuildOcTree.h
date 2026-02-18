#if !defined( __I3OPT_BUILD_OCTREE_H_)
#define __I3OPT_BUILD_OCTREE_H_

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildOcTree : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildOcTree);

public:
	i3OptBuildOcTree( void);
	virtual ~i3OptBuildOcTree(void);

public:
	virtual BOOL OnNode( i3Node * pNode);

	virtual void Trace( i3Node * pRoot);

protected:
	/*
	i3Geometry		*	_GatherGeometry( i3List * list);
	i3OcTreeNode	*	_OnGeometry( i3Geometry * pGeometry);

	i3GeometryAttr	*	_GatherGeometryAttr( i3Geometry * pGeom);
	*/
};

#endif //__I3OPT_BUILD_OCTREE_H_