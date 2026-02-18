/**
 * \file	Character\GameCharaAnimControl.h
 *
 * Declares the game chara animation control class.
 */


#if !defined( __GAME_CHARA_ANIMCALLBACK_H__)
#define __GAME_CHARA_ANIMCALLBACK_H__

#include "GameCharaDef.h"
#include "GameCharaControlBase.h"
#include "GameCharaAnimControlDefine.h"

#define ANIMTIME_MOVE_DURATION		0.666f			//	MOVE 총길이
#define ANIMTIME_MOVE_RIGHT_STEP	0.322f			//	MOVE 오른발 타이밍
#define ANIMTIME_MOVE_LEFT_STEP		(ANIMTIME_MOVE_RIGHT_STEP * 2.0f - 0.01f)			//	MOVE 왼발 타이밍

class CGameCharaAnimControl : public CGameCharaControlBase
{
	I3_CLASS_DEFINE( CGameCharaAnimControl, CGameCharaControlBase);

protected:
	void			_ProcessJumpEnd();
	REAL32			_PlayFootStepSound( REAL32 rDuration, REAL32 rElapseTime, REAL32 tm);

	void			_DinoChangeAIContext( const char * pszPVState );
	void			_CharaChangeAIContext( const char * pszPVState );
	
public:
	/** \brief 애니메이션 플레이 파일을 보여줍니다. 
		\note GameCharaAnimControl.h에서 ENABLE_CHARA_ANIM_OUTPUT 로 on/off 할 수 있습니다. */
	virtual void	AnimDebug( i3AIContext * pCtx, REAL32 timeStart = 0.0f);

	/** \brief AI Context를 변경합니다.
		\param[in] pszPVState Person View		*/
	virtual void	OnChangeAIContext( const char * pszPVState = "1PV:");
	
	// 상 하체 애니메이션을 전부 정지 시키기에 상하체 전체 애니메이션에만 적용하기 바랍니다.
	virtual void	PlayStop( void);

	/** \brief 하체 애니메이션을 Play합니다. */
	virtual bool	PlayLower( ID_LOWER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief 상체 애니메이션을 Play합니다. */
	virtual bool	PlayUpper( ID_UPPER_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief 공룡 상체 애니메이션을 Play합니다. */
	virtual bool	PlayDinoUpper( ID_UPPER_DINO_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief 공룡 하체 애니메이션을 Play합니다. */
	virtual bool	PlayDinoLower( ID_LOWER_DINO_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief 양손 상체 애니메이션을 Play합니다. */
	virtual bool	PlayUpperDual( ID_UPPER_DUAL_AI id, REAL32 timeStart = 0.f, REAL32 timeScale = 1.f);

	/** \brief 이전 상태의 상체 애니메이션을 Play합니다.
		\note Idle과 Move만 해당됩니다. */
	virtual bool	PlayUpperReturn( void);

	/** \brief 앉기/서기에 대한 Animation을 Play합니다.
		\note Animation Context를 새로 설정하게 됩니다. */
	virtual bool	PlayBodyLowerSync( void);

	/** \brief 자동 리로드를 체크 합니다. */
	virtual void	PlayAutoReload( void);

	/************************************************************************************/
	/*							Animation Callback procedure							*/
	/** \brief Idle중 */
	virtual void	OnIdleRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 이동 동작중 */
	virtual void	OnMoveRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 공격 동작중 */
	virtual void	OnAttackRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 공격 중지 */
	virtual void	OnAttackFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief 공격 준비 동작 중지 */
	virtual void	OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief 뛰기중 */
	virtual void	OnJumpStartRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 착지중 */
	virtual void	OnJumpEndRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 착지 중지 */
	virtual void	OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief 장전중 */
	virtual void	OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 탄약 장전중 */
	virtual void	OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 탄약 장전 중지 */
	virtual void	OnLoadBulletFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 볼트액션중 */
	virtual void	OnFireBulletRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 볼트액션 중지 */
	virtual void	OnFireBulletFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 무기 교체중 */
	virtual void	OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 피격 동작중 */
	virtual void	OnDamageRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief 사망 동작 중지 */
	virtual void	OnDeathFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension 활성화중 */
	virtual void	OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief Extension 비활성화 중 */
	virtual void	OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle A 동작중 */
	virtual void	OnUI_IdleARun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle A 중지 */
	virtual void	OnUI_IdleAFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle B 동작중 */
	virtual void	OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle B 중지 */
	virtual void	OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle Change 동작중 */
	virtual void	OnUI_IdleChangeRun( i3AIContext * pCtx, REAL32 tm);

	/** \brief UI Profile Idle Change 중지 */
	virtual void	OnUI_IdleChangeFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle Stand 동작중 */
	virtual void	OnUI_IdleStandRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief UI Profile Idle Stand 중지 */
	virtual void	OnUI_IdleStandFinish( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 왼쪽 Idle중 */
	virtual void	OnLeft_IdleRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 왼쪽 이동 동작중 */
	virtual void	OnLeft_MoveRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 왼쪽 공격 동작중 */
	virtual void	OnLeft_AttackRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 왼쪽 뛰기중 */
	virtual void	OnLeft_JumpStartRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 왼쪽 착지중 */
	virtual void	OnLeft_JumpEndRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 오른쪽 Idle중 */
	virtual void	OnRight_IdleRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 오른쪽 이동 동작중 */
	virtual void	OnRight_MoveRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 오른쪽 공격 동작중 */
	virtual void	OnRight_AttackRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 오른쪽 뛰기중 */
	virtual void	OnRight_JumpStartRun( i3AIContext * pCtx, REAL32 tm) {}

	/** \brief 오른쪽 착지중 */
	virtual void	OnRight_JumpEndRun( i3AIContext * pCtx, REAL32 tm) {}
};

#endif
