#if !defined( __I3_OPT_BUILD_COLLISION_MESH_H)
#define __I3_OPT_BUILD_COLLISION_MESH_H

#include "i3SceneOptimizer.h"

typedef struct _tagOptCollMeshLinkInfo
{
	i3Node *	m_pSrc;
	i3Node *	m_pDest;
} I3_OPT_REPLACENODE_INFO;

class I3_EXPORT_OPT i3OptBuildCollisionMesh : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildCollisionMesh);
protected:
	BOOL				m_bCollapseTransform;
	BOOL				m_bReplaceNode;

	i3List				m_ReplaceList;

	COLOR				m_LastColor;
	i3TextureCube *		m_pLastReflectMap;
	I3_TERRAIN_TYPE		m_TerrainType;
	INT32				m_LastPortalID;

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

	BOOL			GetCollapseTransformEnable(void)		{ return  m_bCollapseTransform; }
	void			SetCollapseTransformEnable( BOOL bFlag)	{ m_bCollapseTransform = bFlag; }

	BOOL			GetReplaceNodeEnable( void)				{ return m_bReplaceNode; }
	void			SetReplaceNodeEnable( BOOL bFlag)		{ m_bReplaceNode = bFlag; }

	i3CollideeMesh *	ConvertTo( i3GeometryAttr * pGeoAttr, INT32 * pIndexTable = NULL);

	virtual BOOL OnPreNode( i3Node * pNode);
	virtual BOOL OnPostNode( i3Node * pNode);
	virtual void Trace( i3Node * pRoot);

	virtual void SetProperty( char * pszFieldName, char * pszValue);

	virtual const char *	getDesc(void);
};

#endif