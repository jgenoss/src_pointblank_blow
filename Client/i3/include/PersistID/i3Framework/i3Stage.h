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
	I3_CLASS_DEFINE( i3Stage);
protected:
	i3Framework			*	m_pFramework;
	i3World				*	m_pWorld;
	i3GameSceneInfo		*	m_pGameSceneInfo;

	i3List				*	m_pRespawnList;

	i3BinList				m_InstManagerList;

public:
	i3Stage(void);
	virtual ~i3Stage(void);

	i3Framework *	GetFramework(void)				{ return m_pFramework; }
	void			SetFramework( i3Framework * pfw)	{ m_pFramework = pfw; }

	i3InstanceManager * GetInstanceManager( i3ClassMeta * pMeta);

	void			setWorld( i3World * pWorld)	{	I3_REF_CHANGE( m_pWorld, pWorld);}
	i3World	*		getWorld(void)				{	return m_pWorld;}
	void			addRespawnObject( i3RespawnObj * pRespawn);
	i3List	*		getRespawnObjList(void)					{	return m_pRespawnList;}

	i3List	*		GetStageObjectList( void);
	i3List	*		GetInstanceList( void);

#if defined( I3_PHYSX)
	BOOL			PHYSIX_SetLinearForce( i3Object * pObj, VEC3D * pImpulse, VEC3D * pIntersect, REAL32 rTorque, i3PhysixShapeSet * pHitShape = NULL );
	BOOL			PHYSIX_SetExplosion( i3Object * pObj, REAL32 rRange, REAL32 rPower, VEC3D * pCenterPos = NULL );
#endif

	//virtual	void	CreateWorld( i3StageInfo * pInfo);
	void			CreateGameScene( i3StageInfo * pInfo);
	//
	// Loading이 진행되는 동안 여러 번 호출되어진다.
	// 일반적으로는 Loading 해야 하는 파일들을 Loading하며,
	// 아직 Loading이 완료되지 않았다면 FALSE를 반환하고,
	// Loading이 완전히 완료되었다면 TRUE를 반환한다.
	virtual BOOL	OnQueryLoad( INT32 numStage);

	// 
	virtual void	OnLoading(void);

	virtual void	OnLoadEnd(void);

	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);

	virtual void	OnCreate(void);
	virtual BOOL	OnFinish(void);
	virtual void	OnChangeVideo( i3VideoInfo * pInfo);
};

#endif
