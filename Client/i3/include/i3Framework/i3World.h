#if !defined( _I3_WORLD_H__)
#define _I3_WORLD_H__

#include "i3GameObj.h"
#include "i3WorldSectionTable.h"

enum I3FRM_FOG_INDEX
{
	I3FRM_FOG_WORLD	= 0,
	I3FRM_FOG_SKY,
	I3FRM_FOG_COUNT,
};

class i3GfxOccCuller;

namespace i3 
{ 
	class octree; class occ_culler; class occ_quad;  class occ_hw_query_callback;
}

namespace i3
{
	typedef i3::shared_ptr<i3::occ_culler>		occ_culler_ptr;
}

class I3_EXPORT_FRAMEWORK i3World : public i3GameObj
{
	I3_EXPORT_CLASS_DEFINE( i3World, i3GameObj);

protected:

	i3AttrSet			*	m_pWorldRoot = nullptr;
	i3AttrSet			*	m_pSkyRoot = nullptr;
	i3Transform			*	m_pSkyTrans = nullptr;
	
	//Fog
    i3FogEnableAttr		*	m_pFogEnableAttr[I3FRM_FOG_COUNT];
	i3FogColorAttr		*	m_pFogColorAttr[I3FRM_FOG_COUNT];
	i3FogDensityAttr	*	m_pFogDensityAttr[I3FRM_FOG_COUNT];
	i3FogModeAttr		*	m_pFogModeAttr[I3FRM_FOG_COUNT];
	i3FogNearFarAttr	*	m_pFogNearFarAttr[I3FRM_FOG_COUNT];

	i3SceneGraphInfo	*	m_pWorldScene = nullptr;
	i3SceneGraphInfo	*	m_pSkyScene = nullptr;
	i3SceneGraphInfo	*	m_pWorldCollisionRoot = nullptr;	// Chara, Npc등의 이동 관련 Collision
	i3SceneGraphInfo	*	m_pHitCollisionRoot = nullptr;	// Attack판정에 쓰일 Hit Collision
	i3SceneGraphInfo	*	m_pLightVolumeCollisionRoot = nullptr;	//Light Volume판정에 쓰일 Collision

	i3WorldSectionTable *	m_pWorldSectionTable = nullptr;
	i3MapSectorTable	*	m_pMapSectorTable = nullptr;

	i3::shared_ptr<i3::octree>			m_octree;
	i3::shared_ptr<i3::occ_culler>		m_occ_culler;
	i3::shared_ptr<i3::occ_hw_query_callback>	m_query_callback;

	i3AttrSet*						m_pRenderOccQuad = nullptr;
	i3::vector<i3::occ_quad*>		m_occ_quad_list;

public:
	i3World();
	virtual ~i3World();

	virtual void		BindSgRes( i3Framework * pFramework, i3GameResSceneGraph * pRes, bool bInstancing = true, bool bClone = true) override;

	void				InitRoot();
	i3AttrSet *			getSkyRoot()	{	return m_pSkyRoot;}
	i3AttrSet *			getWorldRoot()	{	return m_pWorldRoot;}

	i3SceneGraphInfo *	getWorld(void)							{ return m_pWorldScene; }
	void				SetWorld( i3SceneGraphInfo * pWorld);

	i3SceneGraphInfo *	getSky(void)							{ return m_pSkyScene; }
	void				SetSky( i3SceneGraphInfo * pWorld);

	i3SceneGraphInfo *	getWorldCollision(void)					{ return m_pWorldCollisionRoot; }
	void				SetWorldCollision( i3SceneGraphInfo * pColNode);

	i3SceneGraphInfo *	getHitCollision(void)					{ return m_pHitCollisionRoot; }
	void				SetHitCollision( i3SceneGraphInfo * pNode);

	i3SceneGraphInfo *	getLightVolumeCollision(void)			{ return m_pLightVolumeCollisionRoot; }
	void				SetLightVolumeCollision( i3SceneGraphInfo * pNode);

	i3WorldSectionTable *	getWorldSectionTable(void)			{ return m_pWorldSectionTable; }
	void				setWorldSectionTable( i3WorldSectionTable * pTable)
	{
		I3_REF_CHANGE( m_pWorldSectionTable, pTable);
	}

	i3MapSectorTable *	getMapSectorTable(void)			{ return m_pMapSectorTable; }
	void				setMapSectorTable( i3MapSectorTable * pTable)
	{
		I3_REF_CHANGE( m_pMapSectorTable, pTable);
	}

	bool				GetFogEnable(I3FRM_FOG_INDEX nIdx);
	I3COLOR*			GetFogColor(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogDensity(I3FRM_FOG_INDEX nIdx);
	I3G_FOG_MODE		GetFogMode(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogNear(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogFar(I3FRM_FOG_INDEX nIdx);

	void				SetFogEnable( bool bEnable, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogColor( I3COLOR * pColor, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogDensity( REAL32 rDensity, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogMode( I3G_FOG_MODE FogMode, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogNear( REAL32 rNear, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogFar( REAL32 rFar, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);

	void				CreateFogAttr( i3AttrSet * pParentAttr, I3FRM_FOG_INDEX nIndex);

	virtual	void		SetVisible( bool bFlag = true, REAL32 tm = 0.0f);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual void		OnInstancing( I3_ONINSTANCING_INFO * pInfo) override;

	UINT32				OnLoad1( i3ResourceFile * pResFile);
	UINT32				OnLoad2( i3ResourceFile * pResFile);

	bool				HasWorldOccQuads() const;

	void				CreateOccCull(const i3::occ_mesh_set& occMeshSet);

	void				EnableOccCullMesh(bool bEnable);
	bool				IsEnabledOccCullMesh() const;
	
	virtual void	AttachScene(void) override;
	virtual void	DetachScene(void) override;

};

#endif