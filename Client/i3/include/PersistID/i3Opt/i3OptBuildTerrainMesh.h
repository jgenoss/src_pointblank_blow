#if !defined( __I3_OPT_BUILD_TERRAIN_MESH_H)
#define __I3_OPT_BUILD_TERRAIN_MESH_H

#include "i3SceneOptimizer.h"

typedef struct _tagOptInfo
{
	INT32	_posCount;
	INT32	_normCount;
	INT32	_triCount;
	i3CollideeMesh * _pMesh;
} BTM_OPTINFO;

class I3_EXPORT_OPT i3OptBuildTerrainMesh : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptBuildTerrainMesh);
protected:
	NxTriangleMeshShapeDesc * _ConvertToNxTerrain( BTM_OPTINFO *);

	i3List				m_MaterialList;
	I3_PHYSIX_TRI		*		m_pTriArray;
	bool				m_bDynamic;
	REAL32				m_Density;
	REAL32				m_Mass;

protected:
	void				_RemoveAllMaterials(void);
	INT32				_FindMaterial( INT32 terrain, INT32 reverb, COLOR * pColor, INT32 thickness, INT32 PortalID);
	INT32				_AddMaterial( INT32 terrain, INT32 reverb, COLOR * pColor, INT32 thickness, INT32 PortalID);
	void				_MakeMaterials( i3CollideeMesh * pMesh);

public:
	i3OptBuildTerrainMesh(void);
	virtual ~i3OptBuildTerrainMesh(void);

	bool		getDynamic(void)					{ return m_bDynamic; }
	void		setDynamic( bool bFlag)				{ m_bDynamic = bFlag; }

	REAL32		getDensity(void)					{ return m_Density; }
	void		setDensity( REAL32 val)				{ m_Density = val; }

	REAL32		getMass(void)						{ return m_Mass; }
	void		setMass( REAL32 mass)				{ m_Mass = mass; }

	virtual void Trace( i3Node * pRoot);
};

#endif
