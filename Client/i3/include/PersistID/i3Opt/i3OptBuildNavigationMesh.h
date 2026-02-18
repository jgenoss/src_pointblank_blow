#ifndef __I3_OPT_BUILD_NAVIGATION_MESH_H__
#define __I3_OPT_BUILD_NAVIGATION_MESH_H__

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptBuildNavigationMesh : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildNavigationMesh);
protected:
	BOOL				m_bRotateAxisFor3DMAX;

	i3GeometryAttr*		m_pGeometryAttr;

	i3ToolProgressCtrl		*m_pProgressCtrl;

protected:
	BOOL				Rec_Find( i3Node * pNode);

	INT32				FindPos( i3Vector3Array * pArray, VEC3D * pPos);
	i3NavigationMesh *	ConvertIndexTriList( i3IndexArray * pIA, i3VertexArray * pVA);
	i3NavigationMesh *	ConvertTriList( i3VertexArray * pVA);
	i3NavigationMesh *	ConvertIndexTriStrip( i3GeometryAttr * pGeoAttr);
	i3NavigationMesh *	ConvertTriStrip( i3GeometryAttr * pGeoAttr);
	void				OptimizePosition( i3NavigationMesh * pMesh, i3Vector3Array * pArray, INT32 Count);

	i3NavigationMesh *	BuildNavigationMesh( i3GeometryAttr * pGeoAttr);

	BOOL				BuildGraphInfo( i3NavigationMesh *pMesh );
	BOOL				CheckNeighborConnection( i3NavigationMesh *pMesh );

	void				SetProgressCtrl( i3ToolProgressCtrl *pCtrl );
	void				SetProgressRange( INT32 nRange );
	void				SetProgressPos( INT32 nPos );

public:
	i3OptBuildNavigationMesh(void);
	virtual ~i3OptBuildNavigationMesh(void);

	BOOL				GetRotateAxisEnable(void)				{ return m_bRotateAxisFor3DMAX; }
	void				SetRotateAxisEnable( BOOL bFlag)		{ m_bRotateAxisFor3DMAX = bFlag; }

	BOOL				Build( i3Node * pRoot, i3ToolProgressCtrl *pProgressCtrl );
};

#endif //__I3_OPT_BUILD_NAVIGATION_MESH_H__
