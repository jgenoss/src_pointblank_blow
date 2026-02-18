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
class I3_EXPORT_FRAMEWORK i3World : public i3GameObj
{
	I3_CLASS_DEFINE( i3World);

protected:

	i3AttrSet			*	m_pWorldRoot;
	i3AttrSet			*	m_pSkyRoot;
	i3Transform			*	m_pSkyTrans;
	
	//Fog
    i3FogEnableAttr		*	m_pFogEnableAttr[I3FRM_FOG_COUNT];
	i3FogColorAttr		*	m_pFogColorAttr[I3FRM_FOG_COUNT];
	i3FogDensityAttr	*	m_pFogDensityAttr[I3FRM_FOG_COUNT];
	i3FogModeAttr		*	m_pFogModeAttr[I3FRM_FOG_COUNT];
	i3FogNearFarAttr	*	m_pFogNearFarAttr[I3FRM_FOG_COUNT];

	i3SceneGraphInfo	*	m_pWorldScene;
	i3SceneGraphInfo	*	m_pSkyScene;
	i3SceneGraphInfo	*	m_pWorldCollisionRoot;	// Chara, Npc등의 이동 관련 Collision
	i3SceneGraphInfo	*	m_pHitCollisionRoot;	// Attack판정에 쓰일 Hit Collision
	i3SceneGraphInfo	*	m_pLightVolumeCollisionRoot;	//Light Volume판정에 쓰일 Collision

	i3WorldSectionTable *	m_pWorldSectionTable;

public:
	i3World();
	virtual ~i3World();

	virtual void		BindSgRes( i3Framework * pFramework, i3GameResSceneGraph * pRes, bool bInstancing = true, bool bClone = true);

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

	BOOL				GetFogEnable(I3FRM_FOG_INDEX nIdx);
	I3COLOR*			GetFogColor(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogDensity(I3FRM_FOG_INDEX nIdx);
	I3G_FOG_MODE		GetFogMode(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogNear(I3FRM_FOG_INDEX nIdx);
	REAL32				GetFogFar(I3FRM_FOG_INDEX nIdx);

	void				SetFogEnable( BOOL bEnable, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogColor( I3COLOR * pColor, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogDensity( REAL32 rDensity, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogMode( I3G_FOG_MODE FogMode, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogNear( REAL32 rNear, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);
	void				SetFogFar( REAL32 rFar, I3FRM_FOG_INDEX nIdx = I3FRM_FOG_WORLD);

	void				CreateFogAttr( i3AttrSet * pParentAttr, I3FRM_FOG_INDEX nIndex);

	virtual	void		SetVisible( BOOL bFlag = TRUE, REAL32 tm = 0.0f);
	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		 OnLoad( i3ResourceFile * pResFile);
	virtual void		OnInstancing( void * pUserDefault = NULL);

	UINT32				OnLoad1( i3ResourceFile * pResFile);
	UINT32				OnLoad2( i3ResourceFile * pResFile);
};
#endif