#if !defined( __OBJECT_RIDING_HELICOPTER_H__)
#define __OBJECT_RIDING_HELICOPTER_H__

#include "ObjectWithWeapon.h"

enum HELICOPTER_PATH_ANIM
{
	HPA_NONE = 0,
	HPA_PATH1,
	HPA_PATH2,
	HPA_PATH3,
	HPA_GROUND_IDLE_IN_TUTORIAL,
	HPA_TAKE_OFF_IN_TUTORIAL,
	HPA_AIR_IDLE_IN_TUTORIAL,
	HPA_LAND_IN_TUTORIAL,
	HPA_COUNT,
};

enum HELICOPTER_SOUND
{
	HSND_STANDING = 0,
	HSND_LANDING,
	HSND_TAKEOFF,
	HSND_FLYING,

	HSND_COUNT,
};

enum HELICOPTER_SOUND_STATE
{
	HSND_STATE_TAKEOFF = 0,
	HSND_STATE_LANDING,

	HSND_STATE_COUNT,
};

class WeaponM197;
class CGameCharaBase;
class CStageBattle;

class CGameObjectRidingHelicopter : public CGameObjectWithWeapon
{
	I3_EXPORT_CLASS_DEFINE( CGameObjectRidingHelicopter, CGameObjectWithWeapon);

protected:
	HELICOPTER_PATH_ANIM	m_PathKey;
	i3Animation *			m_pPathAnim[ HPA_COUNT];
	REAL32					m_rPathLocalTime;

	i3TimeStamp *			m_pUH60_DustEffect;
	VEC3D					m_vFieldPos;

	bool					m_bMyEnterObject;
	bool					m_bDestroyed;

	REAL32					m_rRespawnTime;
	REAL32					m_rSendCountTime;

	i3Object *				m_pArrowMark;
	i3Transform *			m_pRootDummy;

	i3TimeSequence *		m_pStateTimeSequence[ HSND_STATE_COUNT];
	i3SoundNode		*		m_pStateSound[ HSND_COUNT];
	HELICOPTER_SOUND_STATE	m_nSoundState;

	bool					m_bHelicopterRiding;

	bool					m_bWarningNotice;

protected:
	void					_SetPathAnimKey( HELICOPTER_PATH_ANIM pathKey, REAL32 rStartTime = 0.f);
	void					_ProcessLanding( REAL32 rDeltaSeconds);
	void					_PlayStateSound( HELICOPTER_SOUND_STATE state);

public:
	void					setMyEnterHelicopter( bool bVal)			{ m_bMyEnterObject = bVal; }
	bool					isEnableDamage( void)						{ return (m_PathKey != HPA_NONE); }

	i3Transform *			getRootDummy( void)							{ return m_pRootDummy; }

	HELICOPTER_PATH_ANIM	getPathKey( void)							{ return m_PathKey; }
	REAL32					getLocalAnimTime( void)						{ return m_rPathLocalTime; }

public:
	CGameObjectRidingHelicopter( void);
	virtual ~CGameObjectRidingHelicopter( void);

	virtual void			InitObject() override;
	virtual void			ResetObject() override;
	virtual void			EnableObject() override;
	virtual void			ProcessObject( REAL32 rDeltaSeconds) override;
	virtual void			SetControledObjectState( INT32 state) override;
	virtual void			ProcessIntrude( const GAMEINFO_OBJECT_NETDATA * pNetData) override;

	void					InitAnimation( void);
	void					Destroy(void);
	void					EnableDustEffect( bool bEnable);
	void					Process( REAL32 rDeltaSeconds);
	void					InitSearchArrowMarkObject( void);
	bool					IsEnableObject();
	void					SelectRidingMark(CStageBattle* pStage);
	void					EnterLeaveProcess(UINT64 keyStroke, bool bOnlyLeave = false);
	bool					IsPossibleEnterLeaveInObject();

	// 헬기 패킷 처리 
private:
	bool m_bUseObject_Flag;
	INT32 m_iUseObject_Count;

	bool m_bLeaveWeapon_Flag;
	INT32 m_iLeaveWeapon_Count;

	bool m_bEnterWeapon_Flag;
	INT32 m_iEnterWeapon_Count;

public:
	void					InitFlag();
	void					SendLeaveSyncUH60();
};

#endif
