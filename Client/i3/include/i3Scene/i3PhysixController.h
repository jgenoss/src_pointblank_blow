#if !defined( __I3_PHYSIX_CONTROLLER_H)
#define __I3_PHYSIX_CONTROLLER_H

#if defined ( I3_PHYSX)

#include "i3PhysixDefine.h"
#include "i3PhysixUtil.h"

class I3_EXPORT_SCENE i3PhysixController : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3PhysixController, i3ElementBase);
protected:
	NxController *		m_pController = nullptr;

	INT32				m_Group = 0;
	INT32				m_DestGroupMask = 0;
	REAL32				m_CenterHeight = 0.0f;

	NxShape *			m_pHitShape = nullptr;			// 월드 오브젝트
	VEC3D 				m_HitDir;				// 컬리젼된 방향
	VEC3D				m_HitNormal;			// 컬리젼된 Shape Normal
	VEC3D				m_HitPos;				// 컬리젼된 Shape Pos

	i3EventReceiver *	m_pEventReceiver = nullptr;
	bool				m_bEventTrigger = false;

public:

	NxController *		getNxController(void)				{ return m_pController; }

	NxActor*			getActor()							{ return m_pController->getActor(); }

	NxShape *			getHitShape(void)					{ return m_pHitShape; }
	VEC3D *				getHitDir(void)						{ return &m_HitDir; }

	INT32				getGroup(void)						{ return m_Group; }
	void				setGroup( INT32 group);

	REAL32				getHeight(void)						{ return m_CenterHeight; }
	void				SetCenterHeightAndHeight( REAL32 rCenterHeight, REAL32 rHeight);

	VEC3D	*			getHitNormal( void)					{ return &m_HitNormal; }
	void				setHitNormal( VEC3D * pVec )		{ i3Vector::Copy( &m_HitNormal, pVec ); }

	VEC3D	*			getHitPos( void)					{ return &m_HitPos; }
	void				setHitPos( VEC3D * pVec)			{ i3Vector::Copy( &m_HitPos, pVec); }

	// 이동시킨 결과 위치를 얻어 온다.
	void				getPosition( VEC3D * pPos);
	

	// 위치 강제 지정 (해당 위치로 이동하는 동안 Collision되지 않음.)
	void				setPosition( VEC3D * pPos);

	// void				SetPositionFromCapsule( VEC3D * pPos, REAL32 rRadius);

	void				setCollisionEnable( bool bEnabled = true )
	{
		if( m_pController == nullptr )	return;
		m_pController->setCollision( bEnabled );
	}

public:
	virtual ~i3PhysixController(void);

	bool				Create( INT32 group, NxControllerDesc * pDesc);


	i3EventReceiver *	getEventReceiver( void)									{ return m_pEventReceiver; }
	void				setEventReceiver( i3EventReceiver * pObj);

	// 지정량만큼 이동...
	void				Move( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness = 1.0f);

	void				MakeCapsuleTrigger();

	virtual NxControllerAction		OnShapeHit( const NxControllerShapeHit * pHit);
	virtual	NxControllerAction		OnControllerHit( const NxControllersHit * pHit);
};

#endif

#endif
