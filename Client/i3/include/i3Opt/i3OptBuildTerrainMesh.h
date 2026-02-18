#if !defined( __I3_OPT_BUILD_TERRAIN_MESH_H)
#define __I3_OPT_BUILD_TERRAIN_MESH_H

#include "i3SceneOptimizer.h"

struct BTM_OPTINFO
{
	INT32	_posCount = 0;
	INT32	_normCount = 0;
	INT32	_triCount = 0;
	i3CollideeMesh * _pMesh = nullptr;
} ;

class I3_EXPORT_OPT i3OptBuildTerrainMesh : public i3SceneOptimizer
{
	I3_EXPORT_CLASS_DEFINE( i3OptBuildTerrainMesh, i3SceneOptimizer);
protected:
	NxTriangleMeshShapeDesc * _ConvertToNxTerrain( BTM_OPTINFO *);

	i3::vector<I3_PHYSIX_TERRAIN*>	m_MaterialList;
	I3_PHYSIX_TRI		*		m_pTriArray = nullptr;
	bool				m_bDynamic = false;
	REAL32				m_Density = 1.0f;
	REAL32				m_Mass = 0.0f;

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

	virtual void Trace( i3Node * pRoot) override;
};

#endif
