/**
 * \file	GameCharaActionContext.h
 *
 * Declares the game chara action context class.
 */


#if !defined( __GAME_CHARA_ACTION_CONTEXT_H__)
#define __GAME_CHARA_ACTION_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaActionControl;
class CGameCharaActionControl1PV;
class CGameCharaActionControl3PV;

class CGameCharaActionContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaActionContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaAnimContext;
	
	friend class CGameCharaActionControl;
	friend class CGameCharaActionControl1PV;
	friend class CGameCharaActionControl3PV;
	
protected:
	// ActionControl
	CGameCharaActionControl *			m_pCurrentActionControl = nullptr;
	CGameCharaActionControl1PV *		m_p1PVActionControl = nullptr;
	CGameCharaActionControl3PV *		m_p3PVActionControl = nullptr;

	CHARA_ACTION_BODYLOWER				m_ActionBody = CHARA_BODYLOWER_STAND;		//	Stand or Sit State
	CHARA_ACTION_BODYUPPER				m_ActionBody2 = CHARA_BODYUPPER_ATTACKIDLE;		//	AttackIdle or Idle or Zoom or Grenade State
	CHARA_ACTION_LOWER					m_ActionLower = CHARA_LOWER_NONE;		//	하체 State
	CHARA_ACTION_UPPER					m_ActionUpper = CHARA_UPPER_NONE;		//	상체 State

	CHARA_ACTION_UPPER					m_ActionUpperLeft = CHARA_UPPER_NONE;	// 좌우 분리
	CHARA_ACTION_COMMON					m_ActionCommon = CHARA_ACTION_COMMON_NONE;
	NPC_ACTION_STATE					m_NPCActionState = NPC_ACTION_NONE;

public:
	/** \brief 하체 상태를 반환합니다. */
	CHARA_ACTION_BODYLOWER	getBodyLower( void)						{ return m_ActionBody;				}

	/** \brief 하체 상태를 설정합니다.*/
	void				setBodyLower( CHARA_ACTION_BODYLOWER body)	{ m_ActionBody = body; }

	/** \brief 상체 상태를 반환합니다. */
	CHARA_ACTION_BODYUPPER	getBodyUpper( void)						{ return m_ActionBody2;				}

	/** \brief 상체 상태를 설정합니다. */
	void				setBodyUpper( CHARA_ACTION_BODYUPPER body2)	{ m_ActionBody2 = body2; }

	/** \brief 하체 액션을 반환합니다. */
	CHARA_ACTION_LOWER	getLower( void)								{ return m_ActionLower;				}

	/** \brief 하체 액션을 설정합니다. */
	void				setLower( CHARA_ACTION_LOWER lower)			
	{ 
        m_ActionLower = lower; 
	}

	/** \brief 상체 액션을 반환합니다. */
	CHARA_ACTION_UPPER	getUpper( void)								{ return m_ActionUpper;				}	

	/** \brief 상체 액션을 설정합니다. */
	void				setUpper( CHARA_ACTION_UPPER upper)			{ m_ActionUpper = upper; }

	/** \brief 상체 Left 액션을 반환합니다. */
	CHARA_ACTION_UPPER	getUpperLeft( void)							{ return m_ActionUpperLeft; }

	/** \brief 상체 Left 액션을 설정합니다. */
	void				setUpperLeft( CHARA_ACTION_UPPER upper)		{ m_ActionUpperLeft = upper; }

	/** \brief common 액션을 반환합니다. */
	CHARA_ACTION_COMMON	getCommon( void)							{ return m_ActionCommon;				}

	/** \brief Common 액션을 설정합니다. */
	void				setCommon( CHARA_ACTION_COMMON act)			{ m_ActionCommon	= act;				}

	/** \brief 하체 상태를 확인합니다. */
	bool				isBody( CHARA_ACTION_BODYLOWER nState)		{ return (m_ActionBody == nState);	}

	/** \brief 상체 상태를 확인합니다. */
	bool				isBody2( CHARA_ACTION_BODYUPPER nState)		{ return (m_ActionBody2 == nState);	}

	/** \brief 하체 액션을 확인합니다. */
	bool				isLower( CHARA_ACTION_LOWER nState)			{ return (m_ActionLower == nState);	}

	/** \brief 상체 액션을 확인합니다. */
	bool				isUpper( CHARA_ACTION_UPPER nState)			{ return (m_ActionUpper == nState);	}

	/** \brief 오브젝트를 탄 여부 확인합니다. */
	bool				isEnableOnObject()							{ return (m_ActionUpper == CHARA_UPPER_IDLE || m_ActionUpper == CHARA_UPPER_MOVE || m_ActionUpper == CHARA_UPPER_DAMAGE); }

	/** \brief Common 액션을 확인합니다. */
	bool				isCommon( CHARA_ACTION_COMMON nState)		{ return (m_ActionCommon == nState);	}

public:
	CGameCharaActionContext() {}
	virtual ~CGameCharaActionContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	virtual void	OnChangeAI() override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief 하체 상태를 설정하고 View type을 설정합니다. */
	void			ApplyActionBody( CHARA_ACTION_BODYLOWER state);

	/** \brief 상체 상태를 설정하고 view type을 설정합니다. */
	void			ApplyActionBody2( CHARA_ACTION_BODYUPPER state);


public:
	/** \brief 모든 애니메이션을 정지한다.
		\note 기본 애니메이션으로 설정됩니다. */
	void	_All_Stop();

	/** \brief Dual upper animation을 설정한다.
		\param[in] aiID ID_UPPER_DUAL_AI index
		\param[in] startTime 시작 시간
		\param[in] scale 애니메이션 속도 */
	void	_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief 하체 animation을 설정한다.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime animation 시작 시간
		\param[in] scale animation scale 값	*/
	void	_Play_Anim_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief 상체 animation을 설정한다.
		\param[in] aiID ID_UPPER_AI index
		\param[in] startTime animation 시작 시간
		\param[in] scale animation scale 값 */
	void	_Play_Anim_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief Body State를 설정합니다. */
	void	_Play_Body_Sync();

	/** \brief Ceremony animation을 합니다. */
	bool	_Play_Ceremony( void);

	/** \brief 앉기 animation을 한다. */
	void	_Play_Crouch( void);

	/** \brief Damage 동작을 한다.
		\param[in] part 맞은 부위
		\param[in] pDir 맞은 방향
		\param[in] bFalling 낙하 데미지 여부	*/
	void	_Play_Damage( CHARA_HIT_PART part = CHARA_HIT_UNKNOWN, VEC3D * pDir = nullptr, bool bFalling = false);

	/** \brief Death 동작을 한다.
		\param[in] iKillIdx Killer index
		\param[in] type CHARA_DEATH_TYPE
		\param[in] pDir 맞은 방향 	*/
	void	_Play_Death( INT32 iKiilIdx, CHARA_DEATH_TYPE type, VEC3D * pDir);

	/** \brief Dual의 왼손을 idle로 설정한다. */
	void	_Play_Dual_Idle_Left( void);

	/** \brief Dual의 오른손을 idle로 설정한다. */
	void	_Play_Dual_Idle_Right( void);

	/** \brief Dual의 왼손에 대해서 Jump 동작을 설정한다. */
    void	_Play_Dual_Jump_Left();

	/** \brief Dual의 오른손에 대해서 Jump 동작을 설정한다. */
    void	_Play_Dual_Jump_Right();

	/** \brief Dual의 왼손에 대해서 Jump End 동작을 설정한다. */
    void	_Play_Dual_JumpEnd_Left();

	/** \brief Dual의 오른손에 대해서 Jump End 동작을 설정한다. */
    void	_Play_Dual_JumpEnd_Right();

	/** \brief Dual의 왼손에 대해서 Run 동작을 설정한다. */
    void	_Play_Dual_Run_Left( void);
	
	/** \brief Dual의 오른손에 대해서 Run 동작을 설정한다. */
    void	_Play_Dual_Run_Right( void);

	/** \brief 무기 Extension 동작을 설정한다.
		\param[in] bEnable Extension 활성 유무	*/
	bool	_Play_Extension( bool bEnable);

	/** \brief Jump 동작을 설정한다. */
	bool	_Play_JumpStart( void);

	/** \brief Jump 동작을 설정한다. */
	bool	_Play_JumpStartForDino( void);

	/** \brief 착지 동작을 설정한다. */
	void	_Play_JumpEnd( void);

	/** \brief 착지 동작을 설정한다. */
	void	_Play_JumpEndForDino( void);

	/* \brief force jump를 시작한다. */
	void    _Play_ForceJump( VEC3D *pDir, REAL32 rSpeed, REAL32 rJumpPower );

	/** \brief 하체를 기본 선 동작으로 설정한다. */
	void	_Play_Lower_Idle();

	/** \brief Dino하체를 기본 선 동작으로 설정한다. */
	void	_Play_DinoLower_Idle();

	/** \brief 하체에 대해서 점프 동작을 설정한다. */
    void	_Play_Lower_Jump();

	/** \brief 하체에 대해서 Idle 또는 뛰기 동작으로 설정한다. */
	void	_Play_Lower_Return( void);

	/** \brief 하체에 대해서 뛰기 동작을 설정한다.
		\param[in] pDir 이동방향		*/
    void	_Play_Lower_Run( VEC3D * pDir);

	/** \brief 하체에 대해서 뛰기 동작을 설정한다.
		\param[in] pDir 이동방향		*/
	void	_Play_Dino_Lower_Run( VEC3D * pDir);

	/** \brief 하체에 대해서 걷기 동작을 설정한다.
		\param[in] pDir 이동방향 */
    void	_Play_Lower_Walk( VEC3D * pDir);

	/** \brief Profile 동작 A를 한다. */
	void	_Play_Profile_Idle_A( void);

	/** \brief Profile 동작 B를 한다. */
	void	_Play_Profile_Idle_B( void);

	/** \brief Profile Change 동작을 한다. */
	void	_Play_Profile_Idle_Change( bool bIsSameWeapon);

	/** \brief Profile Stand 동작을 한다. */
	void	_Play_Profile_Idle_Stand( void);

	/** \brief 일어서기 동작을 한다. */
    void	_Play_StandUp();

	/** \brief 무기 교환 동작을 한다. */
	void	_Play_Upper_ChangeWeapon();

	/** \brief 무기 공격 동작을 한다.
		\param[in] iSlot 무기 슬롯
		\param[in] order 공격 방식		*/
    bool	_Play_Upper_Fire( INT32 iSlot = -1, WEAPON::FIRE_ORDER order = WEAPON::PRIMARY_FIRE);

	/** \brief 상체에 대해서 기본 자세로 설정한다. */
	bool	_Play_Upper_Idle();

	/** \brief 상체에 대해서 착지 동작을 설정한다. */
	void	_Play_Upper_JumpEnd( void);

	/** \brief 재장전 동작을 한다.
		\param[in] type 재장전 종류 */
    bool	_Play_Upper_Reload( ACTION_LOAD_MAG_TYPE type = ACTION_LOAD_MAG);

	/** \brief 상체에 대해서 Idle 또는 이동 동작으로 설정한다. */
	bool	_Play_Upper_Return( void);

	/** \brief 상체에 대해서 이동 동작으로 설정한다. */
	bool	_Play_Upper_Run();
	
	bool	_Play_Upper_Walk();//EscapeMission: 기존에 걷기 상체동작이 없어 새로 추가

	/** \brief 줌 동작을 한다. */
	bool	_Play_Zoom();

	/** \brief 줌을 해제하는 동작으로 설정한다. */
	bool	_Play_ZoomCancel( void);

	/** \brief Body state를 변경한다. */
	void	_Toggle_BodyState( void);

	/** \brief Animation을 강제로 정지 시킨다. */
	void	_Stop_Anim( void);
};

#endif
