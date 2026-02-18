#if !defined( __I3_PHYSIX_CONTROLLER_H)
#define __I3_PHYSIX_CONTROLLER_H

#if defined ( I3_PHYSX)

#include "i3PhysixDefine.h"
#include "i3PhysixUtil.h"

class I3_EXPORT_SCENE i3PhysixController : public i3ElementBase
{
	I3_CLASS_DEFINE( i3PhysixController);
protected:
	NxController *		m_pController;

	INT32				m_Group;
	INT32				m_DestGroupMask;
	REAL32				m_Height;

	NxShape *			m_pHitShape;			// 월드 오브젝트
	VEC3D 				m_HitDir;				// 컬리젼된 방향
	REAL32				m_rLength;				// 컬리젼된 거리
	VEC3D				m_HitNormal;			// 컬리젼된 Shape Normal
	VEC3D				m_HitPos;				// 컬리젼된 Shape Pos

	i3EventReceiver *	m_pEventReceiver;

public:

	NxController *		getNxController(void)				{ return m_pController; }

	NxShape *			getHitShape(void)					{ return m_pHitShape; }
	VEC3D *				getHitDir(void)						{ return &m_HitDir; }
	REAL32				getHitLength(void)					{ return m_rLength; }

	INT32				getGroup(void)						{ return m_Group; }
	void				setGroup( INT32 group);

	REAL32				getHeight(void)						{ return m_Height; }
	void				setHeight( REAL32 height)			{ m_Height = height; }

	VEC3D	*			getHitNormal( void)					{ return &m_HitNormal; }
	void				setHitNormal( VEC3D * pVec )		{ i3Vector::Copy( &m_HitNormal, pVec ); }

	VEC3D	*			getHitPos( void)					{ return &m_HitPos; }
	void				setHitPos( VEC3D * pVec)			{ i3Vector::Copy( &m_HitPos, pVec); }

	// 이동시킨 결과 위치를 얻어 온다.
	void				getPosition( VEC3D * pPos)
	{
		if( m_pController == NULL )	return;
		NxExtendedVec3 pos;

		pos = m_pController->getPosition();
		pPos->x = (REAL32) pos.x;
		pPos->y = (REAL32) pos.y - m_Height;
		pPos->z = (REAL32) pos.z;
	}

	// 위치 강제 지정 (해당 위치로 이동하는 동안 Collision되지 않음.)
	void				setPosition( VEC3D * pPos)
	{
		if( m_pController == NULL )		return;

		NxExtendedVec3 pos;

		pos.x = pPos->x;
		pos.y = pPos->y + m_Height;
		pos.z = pPos->z;

		m_pController->setPosition( pos);
	}

	void				setCollisionEnable( bool bEnabled = true )
	{
		if( m_pController == NULL )	return;
		m_pController->setCollision( bEnabled );
	}

public:
	i3PhysixController(void);
	virtual ~i3PhysixController(void);

	bool				Create( INT32 group, NxControllerDesc * pDesc);

	i3EventReceiver *	getEventReceiver( void)									{ return m_pEventReceiver; }
	void				setEventReceiver( i3EventReceiver * pObj);

	// 지정량만큼 이동...
	void				Move( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness = 1.0f);

	virtual NxControllerAction		OnShapeHit( const NxControllerShapeHit * pHit);
	virtual	NxControllerAction		OnControllerHit( const NxControllersHit * pHit);
};

#endif

#endif
