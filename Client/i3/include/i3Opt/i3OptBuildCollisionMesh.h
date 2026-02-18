#if !defined( __I3_OPT_BUILD_COLLISION_MESH_H)
#define __I3_OPT_BUILD_COLLISION_MESH_H

#include "i3SceneOptimizer.h"

struct I3_OPT_REPLACENODE_INFO
{
	i3Node *	m_pSrc = nullptr;
	i3Node *	m_pDest = nullptr;
} ;

class I3_EXPORT_OPT i3OptBuildCollisionMesh : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildCollisionMesh, i3SceneOptimizer);
protected:
	bool				m_bCollapseTransform = true;
	bool				m_bReplaceNode = true;

	i3::vector<I3_OPT_REPLACENODE_INFO*>	m_ReplaceList;

	COLOR				m_LastColor = { 255, 255, 255, 255 };
	i3TextureCube *		m_pLastReflectMap = nullptr;
	I3_TERRAIN_TYPE		m_TerrainType = I3_TERRAIN_CONCRETE;
	INT32				m_LastPortalID = 0;

protected:
	void				RemoveAllReplaceInfo(void);
	void				AddReplaceInfo( i3Node * pSrc, i3Node * pDest);

	INT32				FindPos( i3Vector3Array * pArray, INT32 count, VEC3D * pPos);
	i3CollideeMesh *	ConvertIndexTriList( i3IndexArray * pIA, i3VertexArray * pVA, INT32 * pIndexTable);
	i3CollideeMesh *	ConvertTriList( i3VertexArray * pVA, INT32 * pIndexTable);
	i3CollideeMesh *	ConvertIndexTriStrip( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable);
	i3CollideeMesh *	ConvertTriStrip( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable);
	void				OptimizePosition( i3CollideeMesh * pMesh, i3Vector3Array * pArray, INT32 Count);
	void				OptimizeNormal( i3CollideeMesh * pMesh, i3Vector3Array * pArray, INT32 Count);

	void				TransformTri( i3CollideeMesh * pMesh);

	void				BuildBoundBox( i3Node * pRoot);

	void				_UpdateTerrainType( i3Node * pNode);

public:
	i3OptBuildCollisionMesh(void);
	virtual ~i3OptBuildCollisionMesh(void);

	static	I3_TERRAIN_TYPE		GetTerrainTypeByName( const char * pszName);

	static	const char *		GetTerrainNameByType( INT32 type);

	bool			GetCollapseTransformEnable(void)		{ return  m_bCollapseTransform; }
	void			SetCollapseTransformEnable( bool bFlag)	{ m_bCollapseTransform = bFlag; }

	bool			GetReplaceNodeEnable( void)				{ return m_bReplaceNode; }
	void			SetReplaceNodeEnable( bool bFlag)		{ m_bReplaceNode = bFlag; }

	i3CollideeMesh *	ConvertTo( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable = nullptr);

	virtual bool OnPreNode( i3Node * pNode, i3Node * pParent) override;
	virtual bool OnPostNode( i3Node * pNode, i3Node * pParent) override;
	virtual void Trace( i3Node * pRoot) override;

	virtual void SetProperty( char * pszFieldName, char * pszValue) override;

	virtual const char *	getDesc(void) override;
};

#endif