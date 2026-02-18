/**
 * \file	Character\GameCharaActionControl.h
 *
 * Declares the game chara action control class.
 */


#if !defined( __GAME_CHARA_ACTION_CONTROL_H__ )
#define __GAME_CHARA_ACTION_CONTROL_H__

#include "GameCharaControlBase.h"
#include "GameCharaActionControlDefine.h"
#include "GameCharaAnimControlDefine.h"

/*
Action 처리 순서 (State와 Anim은 서로 관련없이 작동하기 때문에 싱크가 맞는 Action을 진행하기 위해서는 State와 Anim을 동시에 맞춰야 한다.)

첫째. Action에 맞는 State를 설정한다.
둘째. Action에 맞는 Anim을 플레이한다.
*/

class CGameCharaActionControl : public CGameCharaControlBase
{
	I3_ABSTRACT_CLASS_DEFINE( CGameCharaActionControl, CGameCharaControlBase );

	friend class CGameCharaActionContext;

private:
	/** \brief Jump 애니메이션을 Play합니다. */
	void			__Play_Animation_Jump( void);

	/** \brief 상체의 이동 애니메이션에 대한 Scale Time을 구합니다.
		\note 현재 설정된 이동값에 따라 설정됩니다. */
	REAL32			_CalcRunUpperScaleTime( void);

	/** \brief 상체의 이동 애니메이션에 대한 Scale Time을 구합니다. */
	REAL32			_CalcRunDinoUpperScaleTime( void);

	/** \brief 방향에 따른 Death Animation id를 구합니다. */
	ID_LOWER_AI		_FindDeathAnim( CHARA_DEATH_TYPE deathType, VEC3D * pVec);

	/** \brief 방향에 따른 DinoDeath Animation id를 구합니다. */
	ID_LOWER_DINO_AI _FindDeathAnimForDino( CHARA_DEATH_TYPE deathType, VEC3D * pVec);

protected:
	REAL32			_CalcRunLowerScaleTime( ID_LOWER_AI nId, REAL32 rSpeed);

	REAL32			_CalcRunLowerDinoScaleTime( ID_LOWER_DINO_AI nId, REAL32 rSpeed);

	ID_UPPER_AI		_ValidReloadAnimationID( WeaponBase * pWeapon, CWeaponInfo * pWeaponInfo);

	enum LOWER_AI_TYPE	{ LOWER_AI_DEATH_A = 0, LOWER_AI_DEATH_B, LOWER_AI_DEATH_H, MAX_LOWER_AI_DEATH };
	enum LOWER_AI_DIR	{ LOWER_AI_DIR_FRONT = 0, LOWER_AI_DIR_LEFT, LOWER_AI_DIR_RIGHT, LOWER_AI_DIR_BACK };
	ID_LOWER_AI		GetLowerAnimID(LOWER_AI_TYPE Type, LOWER_AI_DIR Dir);
	ID_LOWER_DINO_AI GetLowerAnimIDForDino(LOWER_AI_TYPE Type, LOWER_AI_DIR Dir);

	/** \brief 하체에 대해서 Idle 상태로 설정한다.*/
	bool	ActionLowerReturn( void);

	/** \brief 상체에 대해서 Idle 상태로 설정한다. */
	bool	ActionUpperReturn( void);

	/** \brief 지향자세를 Toggle합니다. (사용 안함) */
	void	ActionToggleIdle( void);

	/** \brief 왼쪽 팔을 Idle 상태로 설정합니다. */
	bool	ActionUpperLeftReturn( void);

	/** \brief 오른쪽 팔을 Idle 상태로 설정합니다. */
	bool	ActionUpperRightReturn( void);

	/** \brief 왼쪽 팔을 Run 상태로 설정합니다. */
	bool	ActionRunUpperLeft( void);

	/** \brief 오른쪽 팔을 Run 상태로 설정합니다. */
	bool	ActionRunUpperRight( void);

	void	_ActionFireDual( REAL32 startTime, REAL32 scale, bool bRight);

	/****************************************************************************/
	/*							 internal inline								*/
	/****************************************************************************/
	inline bool		isBody( CHARA_ACTION_BODYLOWER body)	{ return getActionContext()->isBody( body); }
	inline bool		isBody2( CHARA_ACTION_BODYUPPER body)	{ return getActionContext()->isBody2( body); }
	inline bool		isLower( CHARA_ACTION_LOWER lower)		{ return getActionContext()->isLower( lower); }
	inline bool		isUpper( CHARA_ACTION_UPPER upper)		{ return getActionContext()->isUpper( upper); }
	inline void		setLower( CHARA_ACTION_LOWER lower)		{ getActionContext()->setLower( lower); }
	inline void		setUpper( CHARA_ACTION_UPPER upper)		{ getActionContext()->setUpper( upper); }
	inline void		setUpperLeft( CHARA_ACTION_UPPER upper)	{ getActionContext()->setUpperLeft( upper); }

	void		_Animation_DinoUpper( ID_UPPER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	inline void		_Animation_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		getActionContext()->_Play_Anim_DualUpper( aiID, startTime, scale);
	}

	inline void		_Animation_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		I3_BOUNDCHK( aiID, ID_LOWER_AI_MAX);
		getActionContext()->_Play_Anim_Lower( aiID, startTime, scale);
	}

	void		_Animation_LowerForDino( ID_LOWER_DINO_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	inline void		_Animation_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f)
	{
		I3_BOUNDCHK( aiID, ID_UPPER_AI_MAX);
		getActionContext()->_Play_Anim_Upper( aiID, startTime, scale);
		//I3PRINTLOG(I3LOG_NOTICE,"Chara rTimeScale: %f", scale);
	}


	inline void		_ApplyActionBody( CHARA_ACTION_BODYLOWER state )
	{
		getActionContext()->ApplyActionBody( state);
	}

	inline void		_ApplyActionBody2( CHARA_ACTION_BODYUPPER state)
	{
		getActionContext()->ApplyActionBody2( state);
	}

public:
	/** \brief 기본 자세로 설정합니다. */
	virtual bool	OnActionReset( void);

	/** \brief 일어서기를 설정합니다. */
	virtual bool	OnActionStandup( void);

	/** \brief 앉기를 설정합니다. */
	virtual bool	OnActionCrouch( void);

	virtual bool	OnActionDinoRunLower( VEC3D * pDir)	{ return true; }

	/** \brief 하체에 대해 달리기 동작을 설정합니다. */
	virtual bool	OnActionRunLower( VEC3D * pDir)	{ return true; }

	/** \brief 하체에 대해 걷기 동작을 설정합니다. */
	virtual bool	OnActionWalkLower( VEC3D * pDir) { return true; }

	/** \brief 상체에 대해 이동 동작을 설정합니다. */
	virtual bool	OnActionRunUpper( void);

	/** \brief 공룡 상체에 대해 이동 동작을 설정합니다. */
	virtual bool	OnActionRunDinoUpper( void);
	
	/** \brief 공룡 걷기 상체에 대해 이동 동작을 설정합니다. */
	virtual bool	OnActionWalkDinoUpper( void);

	virtual bool	OnActionSteadyDinoLower( void);

    /** \brief 하체에 대해 기본 자세를 설정합니다. */
	virtual bool	OnActionSteadyLower( void);

	/** \brief 상체에 대해 기본 자세를 설정합니다. */
	virtual bool	OnActionSteadyUpper( void);

	/** \brief 공룡 상체에 대해 기본 자세를 설정합니다. */
	virtual bool	OnActionSteadyDinoUpper( void);

	/** \brief 뛰기 동작을 설정합니다. */
	virtual bool	OnActionJumpStart( void);

	/** \brief 하체에 대해 착지 완료를 설정합니다. */
	virtual bool	OnActionJumpLowerFinish( void);

	/** \brief 상체에 대해 착지 완료를 설정합니다. */
	virtual bool	OnActionJumpUpperFinish( void);

	/* \brief 강제적인 힘을 받아 점프하는 행동을 설정 합니다. */
	virtual bool	OnActionForceJumpStart( VEC3D * pDir, REAL32 rSpeed, REAL32 rJumpPower );

	/** \brief 피격 동작을 설정합니다. */
	virtual bool	OnActionDamage( CHARA_HIT_PART part, VEC3D * pDir)	{ return true; }

	/** \brief 사망 동작을 설정합니다.  */
	virtual bool	OnActionDeath( INT32 iKillIdx, CHARA_DEATH_TYPE type, VEC3D * pDir);	

	/** \brief ProFileA 동작을 설정합니다. */
	virtual bool	OnActionProfileIdleA( void)	{ return true; }

	/** \brief ProfileB 동작을 설정합니다. */
	virtual bool	OnActionProfileIdleB( void) { return true; }

	/** \brief Profile Idle_Change (무기 장착) 동작을 설정합니다. */
	virtual bool	OnActionProfileIdleChange(bool bIsSameWeapon)	{return true; }

	/** \brief Profile Idle_Stand (대기) 동작을 설정합니다. */
	virtual bool	OnActionProfileIdleStand(void) {return true; }

	/** \brief 공격 동작을 설정합니다.
		\param[in] iSlot 무기 슬롯 번호
		\param[in] order 무기 공격 형태		*/
	virtual bool	OnActionFire( INT32 iSlot = -1, WEAPON::FIRE_ORDER order = WEAPON::PRIMARY_FIRE);

	/** \brief 공룡의 공격 동작을 설정합니다.
		\param[in] iSlot 무기 슬롯 번호
		\param[in] order 공격 형태 */
	bool			OnActionFireDino( INT32 iSlot, WEAPON::FIRE_ORDER order);

	/** \brief 사람의 공격 동작을 설정합니다.
		\param[in] order 공격 형태 */
	bool			OnActionFireHuman( WEAPON::FIRE_ORDER order);

	/** \brief 공격 준비 동작을 설정합니다.
		\note 수류탄 계열만 해당됩니다. */
	virtual bool	OnActionReadyToFire( void);

	/** \brief Extension 동작을 설정합니다.
		\param[in] bActivate 활성화 유무 */
	virtual bool	OnActionExtensionWeapon( bool bActivate);

	/** \brief 견착 동작을 설정합니다. */
	virtual bool	OnActionActiveZoom( void);

	/** \brief 견착 동작을 취소합니다. */
	virtual bool	OnActionCancelZoom( void);	

	/** \brief 무기 교체 동작을 설정합니다. */
	virtual bool	OnActionSwapWeapon( void);

	/** \brief 탄창 장전 동작을 설정합니다. */
	virtual bool	OnActionLoadMagazine( void);

	/** \brief 듀얼 탄창의 퀵 동작을 설정합니다. */
	virtual bool	OnActionLoadMagazineDual( void);

	/** \brief 약실에 탄약을 장전하는 동작을 설정합니다. */
	virtual bool	OnActionLoadBullet( void);

	/** \brief 약실에서 탄약을 제거하는 동작을 설정합니다.
		\note 유탄 계열만 해당됩니다. */
	virtual bool	OnActionRemoveShell( void);

	/** \brief 샷건 3에 해당하는 장전 동작을 설정합니다. */
	virtual bool	OnActionLoad_B_Magazine( void);

	/** \brief 샷건 3에 해당하는 장전 준비 동작을 설정합니다. */
	virtual bool	OnActionLoadMag_Ready( void);

	/** \brief 발사후 볼트액션 동작을 설정합니다. */
	virtual bool	OnActionFireBullet( void);

	// Dual 무기
	// 점프 마지막 동작 처리
	virtual bool	OnActionJumpUpperLeftFinish( void);

	/** \brief 공룡 하체 고함 자세를 설정합니다. */
	virtual bool	OnActionDinoRoar( void);

	/** \brief 하체에 대해 기본 자세를 설정합니다. */
	virtual bool	OnActionDinoIdle( void);

	/** \brief 하체에 대해 기본 자세를 설정합니다. */
	virtual bool	OnActionDinoStun( void);

	
protected:
	REAL32			m_rStartStandUpTime = 0.0f;

	// 보스미션전(공룡) 으로 추가
	REAL32			GetScaleTimeByWalkType(REAL32 fVel, VEC3D* pMoveDir);

protected: 
	// To Version
	REAL32			_CalcReloadTimeScaleToVersion(WeaponBase * pWeapon, REAL32 rAnimDuration);
	REAL32			_CalcSwapTimeScaleToVersion(void);

};

#endif
