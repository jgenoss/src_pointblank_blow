#if !defined( I3OPT_SPLIT_GEOMETRY_H__)
#define I3OPT_SPLIT_GEOMETRY_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptSplitGeometry : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptSplitGeometry);

protected:
	i3Geometry		*	_GatherGeometry( i3List * list);
	i3GeometryAttr	*	_OnGeometry( i3Geometry * pGeo);

	i3GeometryAttr	*	_GatherGeometryAttr( i3Geometry * pGeom);

	void				_CalcDimensions( VEC3D * vCenter, REAL32 rSize, UINT32 id);
	void				_GetNewCenter( VEC3D * vOutCenter, UINT32 id);
	UINT32				_DivideByAAPlane( VEC3D	*pTri, VEC3D * pvCenter);
	UINT32				_DivideByAAPlaneExt( VEC3D	*pTri, VEC3D * pvCenter, VEC3D * pTriCenter);

	void				_Rec_SplitGeometry( i3Node * pRoot);
	bool				_SplitGeometry( i3Geometry * pParent, i3GeometryAttr * pAttr, REAL32 size, VEC3D	*vCenter);

	void				_Rec_SplitGeometryAttr( i3Node * pAttrSet, i3Node * pNode);
	void				_SplitGeometryAttr( i3Node * pParent, i3Geometry * pGeom);


	REAL32				m_rWorldSize;
	VEC3D				m_vWorldCenter;
	INT32				m_nCurDepth;

	REAL32				m_rCurrentSize;
	VEC3D				m_vCurrentCenter;

public:
	i3OptSplitGeometry( void);
	virtual ~i3OptSplitGeometry(void);

	virtual	BOOL OnNode( i3Node * pNode);
	virtual void Trace( i3Node * pRoot);
};

#endif