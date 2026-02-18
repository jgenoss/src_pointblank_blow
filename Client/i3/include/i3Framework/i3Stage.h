#if !defined( __I3_STAGE_H)
#define __I3_STAGE_H

#include "i3GameRes.h"
#include "i3GameNode.h"
#include "i3InstanceManager.h"
#include "i3StageInfo.h"
#include "i3World.h"
#include "i3GameSceneInfo.h"
#include "i3RespawnObj.h"

class i3Framework;
class i3Object;

class I3_EXPORT_FRAMEWORK i3Stage : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE( i3Stage, i3GameNode);
protected:
	i3Framework			*	m_pFramework = nullptr;
	i3World				*	m_pWorld = nullptr;
	i3GameSceneInfo		*	m_pGameSceneInfo = nullptr;

	i3::vector<i3RespawnObj*>	m_RespawnList;

	i3::unordered_map<i3ClassMeta*, i3InstanceManager*>	m_InstManagerList;		// 포인터비교뿐이므로 별도비교함수없이 처리 가능..

	i3::pack::GAME_LAYER *	m_pLayer = nullptr;
	INT32					m_LayerCount = 0;

public:
	i3Stage(void);
	virtual ~i3Stage(void);

	i3Framework *	GetFramework(void)				{ return m_pFramework; }
	void			SetFramework( i3Framework * pfw)	{ m_pFramework = pfw; }

	i3InstanceManager * GetInstanceManager( i3ClassMeta * pMeta);

	void			setWorld( i3World * pWorld)	{	I3_REF_CHANGE( m_pWorld, pWorld);}
	i3World	*		getWorld(void)				{	return m_pWorld;}
	void			addRespawnObject( i3RespawnObj * pRespawn);
	i3::vector<i3RespawnObj*>&	getRespawnObjList(void)					{	return m_RespawnList;}

	StageObjectListPtr	GetStageObjectList( void);
	InstanceListPtr		GetInstanceList( void);

	void			CreateLayers( INT32 cnt);
	i3::pack::GAME_LAYER *		getLayer( INT32 idx);
	INT32			getLayerCount(void)						{ return m_LayerCount; }

#if defined( I3_PHYSX)
	bool			PHYSIX_SetLinearForce( i3Object * pObj, VEC3D * pImpulse, VEC3D * pIntersect, REAL32 rTorque, i3PhysixShapeSet * pHitShape = nullptr );
	bool			PHYSIX_SetExplosion( i3Object * pObj, REAL32 rRange, REAL32 rPower, VEC3D * pCenterPos = nullptr );
#endif

	//virtual	void	CreateWorld( i3StageInfo * pInfo);
	void			CreateGameScene( i3StageInfo * pInfo);

	void			ReleasePhysXScene( void);

	//
	// Loading이 진행되는 동안 여러 번 호출되어진다.
	// 일반적으로는 Loading 해야 하는 파일들을 Loading하며,
	// 아직 Loading이 완료되지 않았다면 false를 반환하고,
	// Loading이 완전히 완료되었다면 true를 반환한다.
	virtual bool	OnQueryLoad( INT32 numStage);

	// 
	virtual void	OnLoading(void);
	virtual void	OnLoadingGameData(void);

	virtual void	OnLoadEnd(void);

	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);

	virtual void	OnCreate(void);
	virtual bool	OnFinish(void) { return true; }
	virtual void	OnChangeVideo( i3VideoInfo * pInfo);

#if defined( I3_WINDOWS)
	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;

	virtual	void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;
#endif

	virtual void	OnInput(void) override;

	virtual void	RegisterLuaFunction( lua_State * pState);

	virtual bool	IsLoadCompleteGameData(void)	{ return true;  }
};

#endif
