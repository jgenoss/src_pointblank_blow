/**
 * \file	TutorialMode.h
 *
 * Declares the tutorial mode class.
 */

#ifndef __TUTORIAL_MODE_H__
#define __TUTORIAL_MODE_H__

enum TUTORIAL_STATE
{
	TS_NOT_SET = -1,				///< 초기화된 상태. 
	TS_START = 0,					///< 출발선상에서 움직이지 못하는 상태
	TS_MOVE,						///< 움직이기 시작해서 처음 화살표를 먹기전 상태
	TS_AVOID_HURDLE,				///< 처음 화살표를 먹고 장애물을 피하는 구간
	TS_JUMP,						///< 점프 구간
	TS_PREPARE_SHOOT,				///< 점프 구간을 통과하고 사격장 앞 화살표를 먹기 전 상태
	TS_SHOOT,						///< 실제 사격을 할 수 있는 상태. 
	TS_PREPARE_SHOOT_EXT,			///< 첫번째 사격장의 타겟을 모두 쓰러뜨린 후. 
	TS_TARGET_LOCKON,				///< 익스텐션 사격장 앞 화살표를 먹고 난 후 사격이 가능하나 조준경을 off한 상태
	TS_SHOOT_EXT,					///< 익스텐션 사격장 앞 화살표를 먹고 난 후 실제로 익스텐션 사격이 가능한 상태. 
	TS_PREPARE_GRENADE,				///< 타겟이 모두 쓰러진 후 다음 화살표를 먹기 전 상태. 
	TS_SWAP_GRENADE,				///< 수류탄으로 타겟을 쓰러뜨릴 수 있으나 주무기가 수류탄이 아닐경우
	TS_THROW_GRENADE,				///< 투척 사격장 앞 화살표 먹고 난 후 실제로 수류탄으로 타겟을 쓰러뜨릴 수 있는 상태. 
	TS_THROW_SMOKE,					///< 수류탄 투척이 성공후 다음 상태 (지금은 중국의 요청으로 이 상태가 제외되어 있다. 
	TS_PREPARE_GRENADE_LAUNCHER,	///< 수류탄 투척후 유탄 준비 상태 설명 문구를 위해 있는 상태 3초후에 다음 상태로 바뀜. 
	TS_SHOOT_GRENADE_LAUNCHER,		///< 유탄을 쏴서 타겟을 쓰러뜨릴 수 있는 상태. 
	TS_PREPARE_C4,					///< 유탄으로 모든 타겟을 넘어뜨렸다면 이 상태가 된다. 
	TS_SWAP_C4,						///< 다른 무기를 들고있을시 C4로 교체하라는 메세지
	TS_INSTALL_C4,					///< 폭탄 설치 지역에 있는 화살표를 먹으면 이 상태가 된다. 
	TS_REMOVE_C4,					///< 폭탄 설치가 완료되면 이 상태가 된다. 
	TS_PREPARE_MELEE,				///< 근접 무기 훈련 통로로 이동하라는 메세지
	TS_SWAP_MELEE,					///< 현재 무기가 보조무기가 아닐경우 메세지
	TS_MELEE,						///< 폭탄 설치를 완료하고 칼로 타겟을 넘어뜨릴 수 있는 상태. 
	TS_MOVE_TO_HELICOPTER,			///< 칼로 제거할 수 있는 타겟이 모두 쓰려지면 되는 상태. 
	TS_BOARD_HELICOPTER,			///< 헬기 위 화살표를 먹으면 이 상태가 된다. 
	TS_SWAP_M187,					///< 헬기위에서 M187을 장착하고 있지 않을시 이상태가 된다.
	TS_SHOOT_M187,					///< E키를 눌러 헬기에 탑승하면 이 상태가 된다. 
	TS_RELEASE_M187,				///< M197 사격 종료후 E키를 눌러 해제하라는 메세지.
	TS_MOVE_TO_EXIT,				///< 해제하고 난 후 도착지점으로 이동하라는 메세지. 
	TS_END,							///< 마지막 화살표를 먹고 난 후 상태 (처음 튜토리얼을 하는 거라면). 
	TS_END_TIME_ATTACK, ///< 마지막 화살표를 먹고 난 후 상태 (이미 한바퀴를 돈 후라면). 
	TS_COUNT,
};

enum TUTORIAL_TARGET_AREA_TYPE
{
	TTAT_SHOOT, ///< 첫번째 주무기로 공격 할 수 있는 타겟 지역. 
	TTAT_SHOOT_EXT, ///< 두번째 사격장 주무기의 익스텐션 기능으로 공격 할 수 있는 지역. 
	TTAT_GRENADE,   ///< 수류탄 투척으로 타겟을 제거하는 지역. 
	TTAT_MELEE, ///< 칼로 타겟을 제거하는 지역. 

	TTAT_COUNT
};

class CTargetInTutorial;
class UI_HUD_Tutorial;
class CHud;

/**
 * \class	CTutorialMode
 *
 * \brief	Tutorial mode. 
 */

class CTutorialMode: public i3ElementBase
{
	I3_CLASS_DEFINE(CTutorialMode, i3ElementBase);

private:
	TUTORIAL_STATE m_eTutorialState;	///< State of the tutorial
	REAL32 m_rStateTime;	///< Time of the state
	USER_INFO_EQUIPMENT_WEAPON m_InfoEquipWeaponBackup; ///< The information equip weapon backup
	i3::vector<CTargetInTutorial*> m_vecTarget[TTAT_COUNT];	///< The vector target
	i3::vector<i3Object*> m_vecDrumInTutorial; ///< The vector drum in tutorial
	UI_HUD_Tutorial* m_pHudTutorial;	///< The pointer hud tutorial
	INT32 m_iEndCountInTutorial;	///< Zero-based index of the end count in tutorial
	bool m_bShowKeyDesc;	///< show key description
	bool m_bNeedReloadState;	///< need reload state
	REAL32 m_rNeedReloadStateTime;  ///< Time of the need reload state
	INT32 m_iAppearChallengeClearEffectCount;   ///< Number of appear challenge clear effects
	i3SoundPlayInfo* m_pTutorialSound[2];   ///< The pointer tutorial sound
	bool	m_bOnceFlag;
	bool	m_bOnceFlag2;
	bool	m_bEndByTimeOut;		// 스테이지클래스에 있던 것을 여기로 옮기는게 좋음.(2011.01.12.수빈)

public:

	/**
	 * \fn	CTutorialMode::CTutorialMode();
	 *
	 * \brief	Default constructor.
	 */

	CTutorialMode();

	/**
	 * \fn	virtual CTutorialMode::~CTutorialMode();
	 *
	 * \brief	Finaliser.
	 */

	virtual ~CTutorialMode();

	/**
	 * \fn	void CTutorialMode::OnCreate(i3GameNode * pParent);
	 *
	 * \brief	StageBattle OnCreate에서 해당 함수를 호출함
	 *
	 * \param [in,out]	pParent	If non-null, the parent.
	 */

	void OnCreate(i3GameNode * pParent);

	/**
	 * \fn	void CTutorialMode::OnFinish();
	 *
	 * \brief	StageBattle OnFinish에서 해당 함수를 호출함
	 */

	void OnFinish();

	/**
	 * \fn	void CTutorialMode::UpdateTutorialState(REAL32 rDeltaTime);
	 *
	 * \brief	튜토리얼 모드라면 StageBattle일 때 항상 호출되는 함수.
	 *
	 * \param	rDeltaTime	Time of the delta.
	 */

	void UpdateTutorialState(REAL32 rDeltaTime);

	/**
	 * \fn	void CTutorialMode::SetTutorialState(TUTORIAL_STATE eState);
	 *
	 * \brief	Sets a tutorial state.
	 *
	 * \param	eState	The state.
	 */

	void SetTutorialState(TUTORIAL_STATE eState);

	/**
	 * \fn	void CTutorialMode::FirstTickInTutorialState();
	 *
	 * \brief	TUTORIAL_STATE 가 바뀔 때 처음 한번만 호출 되는 함수.
	 */

	void FirstTickInTutorialState();

	/**
	 * \fn	bool CTutorialMode::ChangeNextStateByArrowInTutorial(INT32 iIndex);
	 *
	 * \brief	"BN_arrow_Green" 이름을 가진 i3Object와 총돌이 되면 함수 내부에서 m_eTutorialState를 변화시킨다.
	 *
	 * \param	iIndex	Zero-based index of the.
	 *
	 * \return	true if it succeeds, false if it fails.
	 */

	bool ChangeNextStateByArrowInTutorial(INT32 iIndex);

	/**
	 * \fn	void CTutorialMode::CheckTargetState();
	 *
	 * \brief	매틱마다 타겟의 상태를 체크함
	 */

	void CheckTargetState();

	/**
	 * \fn	bool CTutorialMode::IsAllTargetDown(TUTORIAL_TARGET_AREA_TYPE eType);
	 *
	 * \brief	eType이 지정한 지역의 타겟이 모두 쓰러져 있다면 true를 리턴함 
	 *
	 * \param	eType	The type.
	 *
	 * \return	true if all target down, false if not.
	 */

	bool IsAllTargetDown(TUTORIAL_TARGET_AREA_TYPE eType);

	/**
	 * \fn	bool CTutorialMode::IsAllTargetNotIdle(TUTORIAL_TARGET_AREA_TYPE eType);
	 *
	 * \brief	eType이 지정한 지역의 타겟이 모두 idle 상태라면 false를 리턴함 
	 *
	 * \param	eType	The type.
	 *
	 * \return	true if all target not idle, false if not.
	 */

	bool IsAllTargetNotIdle(TUTORIAL_TARGET_AREA_TYPE eType);

	/**
	 * \fn	TUTORIAL_STATE CTutorialMode::GetTutorialState()
	 *
	 * \brief	Gets the tutorial state.
	 *
	 * \return	The tutorial state.
	 */

	TUTORIAL_STATE GetTutorialState() { return m_eTutorialState; }

	/**
	 * \fn	void CTutorialMode::LoadTutorialSound();
	 *
	 * \brief	Loads the tutorial sound.
	 */

	void LoadTutorialSound();

	/**
	 * \fn	void CTutorialMode::ReleaseTutorialSound();
	 *
	 * \brief	Releases the tutorial sound.
	 */

	void ReleaseTutorialSound();

	/**
	 * \fn	void CTutorialMode::ChangeNextState(TUTORIAL_STATE eState);
	 *
	 * \brief	다음 상태로 변경 순서에 따라 변경되는 것을 보장함
	 *
	 * \param	eState	The state.
	 */

	void ChangeNextState(TUTORIAL_STATE eState);

	/**
	 * \fn	void CTutorialMode::TurnOffKeyDescription();
	 *
	 * \brief	Turn off key description.
	 */

	void TurnOffKeyDescription();

	/**
	 * \fn	void CTutorialMode::TurnOnKeyDescription(bool bForceTurnOn = false);
	 *
	 * \brief	Turn on key description.
	 *
	 * \param	bForceTurnOn	(optional) the force turn on.
	 */

	void TurnOnKeyDescription(bool bForceTurnOn = false);

	/**
	 * \fn	void CTutorialMode::AddTargetInTutorial(CTargetInTutorial* pTarget);
	 *
	 * \brief	CTargetInTutorial 객체가 Reset 될 때 해당 함수를 호출. 어떤 지역의 타겟인지 구별한 다음 지역에 해당하는 벡터에 push를 함
	 *
	 * \param [in,out]	pTarget	If non-null, target for the.
	 */

	void AddTargetInTutorial(CTargetInTutorial* pTarget);

	/**
	 * \fn	INT32 CTutorialMode::GetMissionCompleteCount(INT32 idx);
	 *
	 * \brief	이동훈련, \n
	 * 			사격 및 투척 훈련,\n
	 * 			근접 및 특수 훈련,\n
	 * 			타임어택 훈련,\n
	 * 			파라메터인 idx와 m_eTutorialState 에 따라 훈련에서 몇번을 수행했는지 리턴함
	 *
	 * \param	idx	0은 이동훈련, 1은 사격 및 투척 훈련...
	 *
	 * \return	The mission complete count.
	 */

	INT32 GetMissionCompleteCount(INT32 idx);

	/**
	 * \fn	INT32 CTutorialMode::GetMissionMaxCount(INT32 idx);
	 *
	 * \brief	idx 에 따른 해당 훈련 맥스 카운트를 리턴함
	 *
	 * \param	idx	Zero-based index of the.
	 *
	 * \return	The mission maximum count.
	 */

	INT32 GetMissionMaxCount(INT32 idx);

	/**
	 * \fn	ChallengeBulletType CTutorialMode::IsMissionComplete(INT32 idx);
	 *
	 * \brief	해당 훈련이 완료인지 아닌지 판단
	 *
	 * \param	idx	Zero-based index of the.
	 *
	 * \return	.
	 */

	ChallengeBulletType IsMissionComplete(INT32 idx);

	/**
	 * \fn	void CTutorialMode::CheckGrenadeInTutorial(VEC3D& v3GrenadePos, INT32 iWeaponNum);
	 *
	 * \brief	유탄이나 수류탄을 투척했을 때 해당 함수가 호출되며, 타겟이 모두 데미지를 받았다면 다음 스테이트로 변경하고
	 * 			그렇지 않으면 수류탄과 유탄를 리필한다.
	 *
	 * \param [in,out]	v3GrenadePos	The v 3 grenade position.
	 * \param	iWeaponNum				The weapon number.
	 */

	void CheckGrenadeInTutorial(VEC3D& v3GrenadePos, INT32 iWeaponNum);

	/**
	 * \fn	void CTutorialMode::RefillAmmo();
	 *
	 * \brief	Refill ammo.
	 */

	void RefillAmmo();

	/**
	 * \fn	bool CTutorialMode::IsAvailableTargetOnTutorialState(TUTORIAL_TARGET_AREA_TYPE eType,
	 * 		INT32 idx);
	 *
	 * \brief	Query if 'eType' is available target on tutorial state.
	 *
	 * \param	eType	The type.
	 * \param	idx  	Zero-based index of the.
	 *
	 * \return	true if available target on tutorial state, false if not.
	 */

	bool IsAvailableTargetOnTutorialState(TUTORIAL_TARGET_AREA_TYPE eType, INT32 idx);

	/**
	 * \fn	bool CTutorialMode::IsAvailableTargetOnTutorialStateOnWeapon(INT32 idx, i3GameObj* pObj);
	 *
	 * \brief	해당 타겟에 데미지를 주는 무기가 타당한지 아닌지 체크
	 * 			TTAT_SHOOT 은 주무기로만 데미지를 줄 수 있고,
	 * 			TTAT_MELEE 은 칼로만 데미지를 줄 수 있다.
	 *
	 * \param	idx				Zero-based index of the.
	 * \param [in,out]	pObj	If non-null, the object.
	 *
	 * \return	true if available target on tutorial state on weapon, false if not.
	 */

	bool IsAvailableTargetOnTutorialStateOnWeapon(INT32 idx, i3GameObj* pObj);

	/**
	 * \fn	bool CTutorialMode::IsTutorialPrimaryWeapon(WeaponBase* pObj);
	 *
	 * \brief	튜토리얼에서 쓸 수 있는 무기는 정해져 있다. 그 무기인지 확인
	 *
	 * \param [in,out]	pObj	If non-null, the object.
	 *
	 * \return	true if tutorial primary weapon, false if not.
	 */

	bool IsTutorialPrimaryWeapon(WeaponBase* pObj);

	/**
	 * \fn	void CTutorialMode::PushDrum(i3Object* pObj);
	 *
	 * \brief	튜토리얼에서 쓰는 드럼통 오브젝트를 벡터에 넣는다
	 *
	 * \param [in,out]	pObj	If non-null, the object.
	 */

	void PushDrum(i3Object* pObj);

	/**
	 * \fn	void CTutorialMode::PlusOneEndCount()
	 *
	 * \brief	Plus one end count.
	 */

	void PlusOneEndCount() { m_iEndCountInTutorial++; }

	/**
	 * \fn	void CTutorialMode::SetOneEndCount()
	 *
	 * \brief	Sets the one end count.
	 */

	void SetOneEndCount() { m_iEndCountInTutorial = 1; }

	/**
	 * \fn	UI_HUD_Tutorial* CTutorialMode::GetHudTutorial()
	 *
	 * \brief	Gets the hud tutorial.
	 *
	 * \return	null if it fails, else the hud tutorial.
	 */

	UI_HUD_Tutorial* GetHudTutorial() { return m_pHudTutorial; }

	/**
	 * \fn	void CTutorialMode::SetHud(CHud* pHud)
	 *
	 * \brief	Sets a hud.
	 *
	 * \param [in,out]	pHud	If non-null, the hud.
	 */

	void SetEndByTimeOut() { m_bEndByTimeOut = true; }
	bool IsEndByTimeOut() const { return m_bEndByTimeOut; }
};

#endif // __TUTORIAL_MODE_H__