#ifndef __I3_OPT_BUILD_NAVIGATION_MESH_H__
#define __I3_OPT_BUILD_NAVIGATION_MESH_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildNavigationMesh : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildNavigationMesh, i3SceneOptimizer);
protected:
	bool				m_bRotateAxisFor3DMAX = false;

	i3GeometryAttr*		m_pGeometryAttr = nullptr;

	i3ToolProgressCtrl		*m_pProgressCtrl = nullptr;

protected:
	bool				Rec_Find( i3Node * pNode);

	INT32				FindPos( i3Vector3Array * pArray, VEC3D * pPos);
	i3NavigationMesh *	ConvertIndexTriList( i3IndexArray * pIA, i3VertexArray * pVA);
	i3NavigationMesh *	ConvertTriList( i3VertexArray * pVA);
	i3NavigationMesh *	ConvertIndexTriStrip( i3GeometryAttr * pGeoAttr);
	i3NavigationMesh *	ConvertTriStrip( i3GeometryAttr * pGeoAttr);
	void				OptimizePosition( i3NavigationMesh * pMesh, i3Vector3Array * pArray, INT32 Count);

	i3NavigationMesh *	BuildNavigationMesh( i3GeometryAttr * pGeoAttr);

	bool				BuildGraphInfo( i3NavigationMesh *pMesh );
	bool				CheckNeighborConnection( i3NavigationMesh *pMesh );

	void				SetProgressCtrl( i3ToolProgressCtrl *pCtrl );
	void				SetProgressRange( INT32 nRange );
	void				SetProgressPos( INT32 nPos );

public:
	i3OptBuildNavigationMesh(void);
	virtual ~i3OptBuildNavigationMesh(void);

	bool				GetRotateAxisEnable(void)				{ return m_bRotateAxisFor3DMAX; }
	void				SetRotateAxisEnable( bool bFlag)		{ m_bRotateAxisFor3DMAX = bFlag; }

	bool				Build( i3Node * pRoot, i3ToolProgressCtrl *pProgressCtrl );
};

#endif //__I3_OPT_BUILD_NAVIGATION_MESH_H__
