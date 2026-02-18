#if !defined( __GAME_CHARA_INPUT_CONTROL_AI_H__)
#define __GAME_CHARA_INPUT_CONTROL_AI_H__

#include "GameCharaInputControl.h"

//자세 선/앉은/뛴
//걷기 선/걷기/뛰기
//이동 선/앞/뒤/좌/우
//방향 위/아래/좌/우
//상태 멀뚱/주시/순찰/전투중

//전투중에 행할수 있는행동 : 서서/앉아서/점프/앉아서 이동/접전지 이탈/회피

//공격 행동: 거리별 무기조절, 수류탄 투척, 연막/플래시뱅 투척

//전투중에 발생할수있는 상황 : 근처에 수류탄 발견, 근접무기 착용, 


#define AI_ACTION_STAND			0x0000000000000000
#define AI_ACTION_CROUCH		0x0000000000000001
#define AI_ACTION_JUMP			0x0000000000000002

#define AI_ACTION_IDLE			0x0000000000000000
#define AI_ACTION_FOOTSTEP		0x0000000000000010
#define AI_ACTION_EVASION		0x0000000000000020
#define AI_ACTION_MOVE			0x0000000000000040


#define AI_ACTION_ATTACK		0x0000000000000100
#define AI_ACTION_STOPATTACK	0x0000000000000200
#define AI_ACTION_SWAPWEAPON0	0x0000000000001000
#define AI_ACTION_SWAPWEAPON1	0x0000000000002000
#define AI_ACTION_SWAPWEAPON2	0x0000000000004000
#define AI_ACTION_SWAPWEAPON3	0x0000000000008000
#define AI_ACTION_SWAPWEAPON4	0x0000000000010000
#define AI_ACTION_SWAPWEAPON5	0x0000000000020000

#define AI_ACTION_FIRE			0x0000000001000000
#define AI_ACTION_DROPWEAPON	0x0000000002000000
#define AI_ACTION_SPECIAL		0x0000000004000000
#define AI_ACTION_RELOAD		0x0000000008000000
#define AI_ACTION_SUBFUNC		0x0000000010000000
#define AI_ACTION_EXT_ACTIVATE	0x0000000020000000
#define AI_ACTION_STOPFIRE		0x0000000040000000

#define AI_MOVE_LEFT			0x0000000000000001
#define AI_MOVE_RIGHT			0x0000000000000002
#define AI_MOVE_FORWARD			0x0000000000000004
#define AI_MOVE_BACKWARD		0x0000000000000008

#define AI_STATE_MASK_HOLDPOSITION		0x80000000					// 다음 위치를 이동할 것
#define AI_STATE_MASK_THROW_CONSIDER	0x40000000

enum AI_STATE
{
	AI_STATE_NONE				= 0,
	AI_STATE_GOPATH				= (0 | AI_STATE_MASK_THROW_CONSIDER),								//PATH를 벗어난 행동을 한경우 가장 가까운 PATH POINT를 찾아감
	AI_STATE_KEEPWATCH			= (1 | AI_STATE_MASK_HOLDPOSITION | AI_STATE_MASK_THROW_CONSIDER),	//경계
	AI_STATE_MOVE				= (2 | AI_STATE_MASK_THROW_CONSIDER),								//이동
	AI_STATE_FIGHT				= 3,																//교전
	AI_STATE_ESCAPE				= 4,																//현재 위치 이탈
	AI_STATE_THROW				= (5 | AI_STATE_MASK_HOLDPOSITION),			// (수류탄, etc...) 던지기 공격
	AI_STATE_REMOVE_OBJ			= (6 | AI_STATE_MASK_HOLDPOSITION),

	AI_STATE_MAX_COUNT,
};

enum AI_MOVE_RETURN_VALUE
{
	AI_MOVE_RET_OK = 0,
	AI_MOVE_RET_ARRIVED,
	AI_MOVE_RET_BLOCKED,

	AI_MOVE_RET_MAX,
};

#define		MAX_PATH_DEPTH		50
#define		NORMAL_PATH_DEPTH	20

//리스폰->다음 패스로 이동 ->이동중 적군 만나면 공격 -> 적이 없어지면 다시 이동
struct TMPOBJ_INFO;

class CGameCharaInputControlAI : public CGameCharaInputControl
{
	I3_CLASS_DEFINE( CGameCharaInputControlAI, CGameCharaInputControl);

	i3GamePath *	m_pGamePath = nullptr;

	PB_AI_DATA		m_AIData;
	REAL32			m_EvasionRate[AIET_COUNT];			// [ initialize at constructor]

	//state
	AI_STATE		m_nAIState = AI_STATE::AI_STATE_NONE;
	AI_STATE		m_nOldAIState = AI_STATE::AI_STATE_NONE;

	UINT64			m_nActionState = 0;
	UINT64			m_nMoveState = 0;

	//target
	INT32			m_CurrentTargetIdx = -1;
	VEC3D			m_vTargetDir;

	I3_POINTIDX		m_idxOldTargetPoint = -1;
	I3_POINTIDX		m_idxCurTargetPoint = -1;

	//
	REAL32			m_InterpolateTime = 0.2f;
	REAL32			m_rThetaTimer = 0.0f;
	REAL32			m_rOldTheta = 0.0f;
	REAL32			m_rTargetTheta = 0.0f;

	INT32			m_WayPointCount = 0;
	I3_POINTIDX		m_CurWayPointIndex = 0;
	I3_POINTIDX		m_WayPoint[ MAX_PATH_DEPTH];		// [initialize at constructor]

	//timer
	REAL32			m_rElapsedKeepWatchTime = 0.0f;
	REAL32			m_rElapsedTraceTime = 0.0f;	//현재 패스에서 얼마나 오래 머물렀는지 계산. 고립 상태에서 벗어나기
	REAL32			m_rElapsedJumpTime = 0.0f;
	REAL32			m_rAIUpdateElapsedTime = 10.0f;

	//수류탄 투척후
	REAL32			m_rAfterThrowTimer = 0.0f;

	REAL32			m_rElapsedAutoFireTime = 0.0f;	//점사 타이머

	REAL32			m_rEvasionStateTime = 0.0f;	//회피 타이머
	
	bool			m_bMoveFlag[2] = { false, false };

	//flag
	bool			m_bLeft = false;				//게걸음 회피기술에서 좌우를 인식하기위한 플래그

	i3::vector<TMPOBJ_INFO*> m_RemoveObjectList;		//뽀개버릴 오브젝트 리스트

	//
	AI_GRENADE_PATTERN	m_GrenadePattern = AI_GRENADE_RANDOM;		// 수류탄 투척 패턴 (바로 던지기 : 교전 중 던지기)
	REAL32				m_timeGrenadeThrow = 0.0f;
	INT32				m_SmokeProbability = 100;		// 연막탄을 투척하는 확률 (0~100)
	bool				m_bThrowSmoke = false;
	WEAPON_SLOT_TYPE	m_ThrowingWeapon = WEAPON_SLOT_UNKNOWN;

	VEC3D			m_vThrowPosition;
	bool			m_bForceEvadeSelfThrow = false;		//자신의 수류탄이 범위내에 있어 무조건 피하도록 함

	// 출력정보
	i3::rc_wstring	m_nickname;
	INT32			m_rank = 0;

	INT32			m_primaryWeaponName = 0;
	INT32			m_secondaryWeaponName = 0;
	INT32			m_meleeWeaponName = 0;
	INT32			m_throwWeaponName = 0;
	INT32			m_itemWeaponName = 0;
	INT32			m_redCharaName = 0;	
	INT32			m_blueCharaName = 0;
	INT32			m_headCharaName = 0;
	INT32			m_beretCharaName = 0;

	INT32			m_hp = 0;
	INT32			m_respawnTime = 0;
	
	bool			m_bAttack_Death_Player = false;
	bool			m_bAttacking_Death_Player = false;

protected:
	bool			isWeaponExist( WEAPON_SLOT_TYPE type);
	WEAPON_SLOT_TYPE	_GetAvailableWeapon(void);
	bool			_SwapAvailableWeapon();

public:
	CGameCharaInputControlAI();
	virtual ~CGameCharaInputControlAI();
	
	virtual void			OnFirstUpdate( REAL32 rDeltatime) override;
	virtual void			OnUpdate( REAL32 rDeltatime) override;
	virtual void			OnLastUpdate( REAL32 rDeltatime) override;

	void					OnResetAI();
	void					BindAI(AI_BIND_DATA* pData);

	UINT64					getActionState(void)			{	return m_nActionState;}
	UINT64					getMoveState(void)				{	return m_nMoveState;}

	I3_POINTIDX				getCurTargetPoint(void)			{	return m_idxCurTargetPoint;}

	//탐색
	INT32					FindStartPosition(void);										//현재 위치에서 가장 가까운 이동가능한 point찾기
	INT32					SelectNextPath( INT32 oldPoint, INT32 curPoint);				//다음 이동할 path얻기
	INT32					GetClosestPoint( VEC3D * pvTargetPos);							//지정 위치와 가장 가까운 point구하기
	void					FindNextTargetPosition(void);									//다음 이동할 path를 얻어 설정
	void					FindTargetPosition(I3_POINTIDX idxStart, I3_POINTIDX idxEnd, INT32 nDepth = NORMAL_PATH_DEPTH);	// 지정 위치에서 지정2위치로 이동가능한 경로 찾기

	INT32					GetClosestDirectionPoint( VEC3D * pvTargetPos, VEC3D * pvDirection);	//지정위치와 가까운 POINT중에 가장 근접한 DIRECTION을 가지는 위치 찾기
	INT32					GetClosestDirectionPoint( INT32 idxStart, VEC3D * pvDirection);			//지정된 포인트에 인접한 포인트중 가장 근접한 DIRECTION을 가지는 위치 찾기

	//목표를 향하는 벡터 구하기
	REAL32					GetAttackDirection( CGameCharaBase * pTargetChara, VEC3D * pvStart, VEC3D * pvDir, bool bForce = true);

	bool					MakeKeyInput( VEC3D * pvDir, MATRIX * pOwnerMtx);

	//이동
	AI_MOVE_RETURN_VALUE	MoveTo( VEC3D * vPos, REAL32 rDeltaTime);		//이동, 목적지와 0.7m이내로 가까워지면 false리턴
	virtual bool			MoveToTarget( i3GameObj* pTarget, REAL32 rMinLength, REAL32 rDeltaSeconds);

	bool					SelectWeapon();

	bool					IsThisNegativeByWatchTime();
	AI_STATE				OnArrived_FindNext(void);

	void					SeeTargetPosition( VEC3D * pvPos, REAL32 rTime = 0.2f, bool bRho = false);			// 대상 위치 바라보기
	void					SeeTargetPosition(i3::pack::VEC3D * pvPos, REAL32 rTime = 0.2f, bool bRho = false);			// 대상 위치 바라보기
	void					SeeTarget( VEC3D *pvDir, REAL32 rTime = 0.2f, bool bRho = true);	// 대상 방향 바라보기

	void					OnInterpolateTheta( REAL32 rDeltaSeconds);	// 케릭터 theta interpolate

	//탐색
	INT32					FindTarget();																	//현재 무기( 최소 20m) 사정거리 안의 적 탐색

	//교전 대상 설정
	bool					FightWidth( INT32 idx);

	//교전시 패턴
	void					SelectFightPose();							// 교전시 행동패턴 설정
	void					ActivateExtension( bool bActivate);
	REAL32					GetTargetRate(void);
	
	//교전시 이동
	void					MoveOnFight( REAL32 rDistance, REAL32 rDeltaSeconds);
	void					ProcessEvasion( REAL32 rDeltaSeconds);

	// 교전시 공격
	void					Attack( INT32 idx, REAL32 rDeltaSeconds);			// 케릭터 인덱스에 따른공격
	void					Attack( VEC3D * pVDir, REAL32 rDeltaTime);			// 대상 방향으로 공격
	void					Attack( i3GameObj * pTargetObj, REAL32 rDeltaTime);	//대상 object를 바라보고 공격

	//
	bool					DestroyIIObject( REAL32 rDeltaSeconds);
	INT32					_CheckObjectCollision( i3::vector<TMPOBJ_INFO*>& List, REAL32 rRange, REAL32 rDeltaSeconds);
	void					OnRemoveObject( REAL32 rDeltaSeconds);
	//교전 종료
	void					StopAttack( void);
	AI_STATE				AfterFight(void);



	// 수류탄 투척 관련
	REAL32					CalcRhoForThrow( const VEC3D * pTargetPos, VEC3D * pLookPos);
	void					ProcessGrenadeForBusyPattern( REAL32 tm, WEAPON_SLOT_TYPE type);
	bool					ProcessGrenadeForLazyPattern( VEC3D * pLastPos);

	void					OnHit( INT32 idx);

	INT32					IsThrowWasSafeZone(INT32 * pIdx);			//자신이 던진 수류탄이 자신에게 안전한 위치에 떨어졌는지 체크
	void					EvadeSelfThrow( REAL32 rDeltaTime);	//자신이 던진 수류탄에 죽지 않도록 피함

	///////////////////////////////////////////////////////////////////////
	//AI STATE
	void	OnGoPath(REAL32 rDeltaSeconds, bool bFirst);

	void	OnKeepWatch(REAL32 rDeltaSeconds, bool bFirst);

	void	OnMove(REAL32 rDeltaSeconds, bool bFirst);

	void	OnFight( REAL32 rDeltaSeconds, bool bFirst);

	void	OnEscape( REAL32 rDeltaSeconds, bool bFirst);

	void	OnThrow( REAL32 tm, bool bFirst);

	///////////////////////////////////////////////////////////////////////
	//Processing
	void	SetMoveDir( VEC3D * pVecOut);
	// 무기 버리기 / 줍기 처리
	void	ProcessDropWeapon( REAL32 rDeltatime);
	// 앉기 / 일어서기 처리 ( return BodyChangeflag)
	bool	ProcessCrouch( REAL32 rDeltatime);
	// 이동 처리
	void	ProcessMovement( REAL32 rDeltatime, bool bChangeBody);

	// 무기 처리
	void	ProcessWeapon( REAL32 rDeltatime);
	void	ProcessDinoAttack( REAL32 rDeltatime);

	// Extension
	void	ProcessExtension( void);

	AI_STATE	ProcessAI( REAL32 rDeltatime);
	void		ProcessInput( REAL32 rDeltatime);


	// 출력정보
	const i3::rc_wstring& getNickname(void) const	{ return m_nickname;	}
	INT32		getRank(void)					{ return m_rank;	}
	INT32		getPrimaryWeaponName(void)		{ return m_primaryWeaponName;	}
	INT32		getSecondaryWeaponName(void)	{ return m_secondaryWeaponName;	}
	INT32		getMeleeWeaponName(void)		{ return m_meleeWeaponName;	}
	INT32		getThrowWeaponName(void)		{ return m_throwWeaponName;	}
	INT32		getItemWeaponName(void)			{ return m_itemWeaponName;	}
	INT32		getRedCharaName(void)			{ return m_redCharaName;	}
	INT32		getBlueCharaName(void)			{ return m_blueCharaName;	}
	INT32		getHeadCharaName(void)			{ return m_headCharaName;	}
	INT32		getBeretCharaName(void)			{ return m_beretCharaName;	}
	INT32		getHp(void)						{ return m_hp;				}
	INT32		getRespawnTime(void)			{ return m_respawnTime;		}
};

#endif
