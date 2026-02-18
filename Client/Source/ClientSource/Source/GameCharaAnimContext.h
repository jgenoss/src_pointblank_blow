/**
 * \file	GameCharaAnimContext.h
 *
 * Declares the game chara animation context class.
 */

#if !defined( __GAME_CHARA_ANIM_CONTEXT_H__)
#define __GAME_CHARA_ANIM_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaDef.h"
#include "./Character/GameCharaAnimControlDefine.h"
#include "GameCharaWeaponControlDef.h"

#define CHARA_UPPER_BONE_BODY_COUNT		5
#define CHARA_UPPER_BONE_LEFT_COUNT		23
#define CHARA_UPPER_BONE_RIGHT_COUNT	23

class CGameCharaAnimControl;
class CGameCharaAnimControl1PV;
class CGameCharaAnimControl3PV;

class CGameCharaAnimContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaAnimContext, CGameCharaContextBase);
	
	friend class Avatar;
	friend class CGameCharaBase;
	friend class CGameCharaDino;
	friend class CGameCharaDinoRaptor;
	friend class CGameCharaDinoTRex;
	friend class CGameCharaDinoSting;
	friend class CGameCharaDinoTank;
	friend class CGameCharaDinoAcid;
	friend class CGameCharaDinoElite;

	friend class CGameCharaDinoCCRaptor;
	friend class CGameCharaDinoCCSting;
	friend class CGameCharaDinoCCAcid;
	friend class CGameCharaDinoRaptorMercury;
	friend class CGameCharaDinoStingMars;
	friend class CGameCharaDinoAcidVulcan;

	friend class CGameCharaActionContext;
	friend class CGameCharaBoneContext;
	friend class CGameCharaWeaponContext;

	friend class CGameCharaAnimControl;
	friend class CGameCharaAnimControl3PV;
	
	

protected:
	CGameCharaAnimControl *			m_pCurrentAnimControl = nullptr;
	CGameCharaAnimControl1PV *		m_p1PVAnimControl = nullptr;
	CGameCharaAnimControl3PV *		m_p3PVAnimControl = nullptr;

	ID_LOWER_AI			m_nOldLowerId = ID_LOWER_UNKNOWN;
	ID_UPPER_AI			m_nOldUpperId = ID_UPPER_UNKNOWN;

	ID_UPPER_DUAL_AI	m_nOldUpperDualLeftId = ID_UPPER_DUAL_UNKNOWN;
	ID_UPPER_DUAL_AI	m_nOldUpperDualRightId = ID_UPPER_DUAL_UNKNOWN;

	// Animation index list;
	INT8				m_nAICommonId[ID_COMMON_AI_MAX] = { 0, 0 };

	

	i3AIContext *		m_pAILower = nullptr;			//	하체 AI (Anim)	nullptr일수 있습니다.
	i3AIContext *		m_pAIUpper = nullptr;			//	상체 AI (Anim)	nullptr일수 있습니다.
	i3AIContext *		m_pCommon = nullptr;			// 상하체 전체 애니메이션이 있는 context입니다.(로비 캐릭터의 ceremony에 사용)
	i3AIContext *		m_pAIUpper_Left = nullptr;	// 상체 좌측 animation context

	INT8*				m_UpperBoneList = nullptr; //[ CHARA_UPPER_BONE_COUNT ];	// Animation Scaling을 위한 본리스트
	INT8*				m_LowerBoneList = nullptr; //[ CHARA_LOWER_BONE_COUNT ];	// Animation Scaling을 위해 본리스트를 가지고 있습니다.

	REAL32				m_fUpperAniTimer = 0.0f;				// 상체 싱크 타임

	// FootStep
	REAL32				m_fLeftStepTime = 0.0f;
	REAL32				m_fRightStepTime = 0.0f;

	INT32				m_nAccessory = 0;				//액세서리 장비의 스치는 소리인덱스( 랜덤으로 하지 않도록 요청받았기때문)
	REAL32				m_fRunElapseTime = 0.0f;
	REAL32				m_fRunDuration;					//1 [initialize at constructor]

	// Run Jump
	bool				m_bRunJump = false;
	REAL32				m_rJumpAnimScaleValue = 0.0f;
	REAL32				m_rLowerJumpStepTime = 0.0f;
	REAL32				m_rJumpUpperPoseChangeTime = 0.0f;

	REAL32				m_rLowerAnimScale = 1.0f;
	REAL32				m_rUpperAnimScale = 1.0f;

	bool				m_bUpperWalk = false;

	// search를 하지 않기 위해 미리 담아둔다. 
	UINT32				m_MaxWeaponNumber;				// [initialize at constructor]
	UINT32				m_MaxDualWeaponNumber;			// [initialize at constructor]

public:
	/** \brief Lower의 AI ID를 반환합니다. */
	INT32				getAILowerID( CHARA_ACTION_BODYLOWER body, ID_LOWER_AI ai);

	#if !defined( AI_MOVE_TO_WEAPON)
	/** \brief Upper AI id를 반환합니다.
		\param[in] wepaonClass 무기 클래스
		\param[in] nWeaponNum 무기 번호
		\param[in] ai ID_UPPER_AI index */ 
	INT32				getAIUpperID( WEAPON_CLASS_TYPE weaponClass, WEAPON::RES_ID nWeaponNum, ID_UPPER_AI ai);

	/** \brief 듀얼용 Upper AI id Index를 반환합니다.
		\param[in] viewType 1/3인칭 flag
		\param[in] nWeaponNum 무기 번호
		\param[in] ai ID_UPPER_DUAL_AI index */
	INT32				getAIUpperDualIDIndex(CHARA_VIEW_TYPE viewType, WEAPON::RES_ID nWeaponNum, ID_UPPER_DUAL_AI ai)
	{
		I3_BOUNDCHK( viewType, CHARA_VIEW_MAX);
		I3ASSERT( nWeaponNum < m_MaxDualWeaponNumber);
		I3_BOUNDCHK( ai, ID_UPPER_DUAL_AI_MAX);

		return (viewType * m_MaxDualWeaponNumber * ID_UPPER_DUAL_AI_MAX) +
				(nWeaponNum * ID_UPPER_DUAL_AI_MAX) + ai;
	}

	/** \brief 듀얼용 Upper AI Id를 반환합니다.
		\param[in] viewType 1/3인칭 flag
		\param[in] nWeaponNum 무기 번호
		\param[in] ai ID_UPPER_DUAL_AI index */
	INT32				getAIUpperDualID( WEAPON::RES_ID nWeaponNum, ID_UPPER_DUAL_AI ai);
	#endif

	/** \brief 이전 lower ai id를 반환합니다. */
	ID_LOWER_AI			getOldLowerID( void);

	/** \brief 이전 lower ai id를 설정합니다. */
	void				setOldLowerID( ID_LOWER_AI id)			{ I3ASSERT( id < ID_LOWER_AI_MAX); m_nOldLowerId = id; }

	/** \brief 이전 upper ai id를 반환합니다. */
	ID_UPPER_AI			getOldUpperID( void);

	/** \brief 이전 upper ai id를 설정합니다. */
	void				setOldUpperID( ID_UPPER_AI id)			{ I3ASSERT( id < ID_UPPER_AI_MAX); m_nOldUpperId = id; }

	/** \brief Dual의 이전 Left ai id를 반환합니다. */
	ID_UPPER_DUAL_AI	getOldUpperDualLeftID( void)			{ return m_nOldUpperDualLeftId; }

	/** \brief Dual의 이전 Left ai id를 설정합니다. */
	void				setOldUpperDualLeftID( ID_UPPER_DUAL_AI id)	{ I3ASSERT( id < ID_UPPER_DUAL_AI_MAX); m_nOldUpperDualLeftId = id; }

	/** \brief Dual의 이전 right ai id를 반환합니다. */
	ID_UPPER_DUAL_AI	getOldUpperDualRightID( void)			{ return m_nOldUpperDualRightId; }

	/** \brief Dual의 이전 right ai id를 설정합니다. */
	void				setOldUpperDualRightID( ID_UPPER_DUAL_AI id) { I3ASSERT( id < ID_UPPER_DUAL_AI_MAX); m_nOldUpperDualRightId = id; }

	/** \brief Lower Ai State 설정 여부 */
	bool				isAILower( void)						{ return (m_pAILower != nullptr ? true : false);		}

	/** \brief Upper Ai State 설정 여부 */
	bool				isAIUpper( void)						{ return (m_pAIUpper != nullptr ? true : false);		}

	/** \brief Lower Ai State를 반환합니다. */
	i3AIContext *		getAILower( void)						{ return m_pAILower;		}

	/** \brief Lower AI State를 설정합니다. */
	void				setAILower( i3AIContext * pCtx)			{ m_pAILower = pCtx; }

	/** \brief Upper AI State를 반환합니다. */
	i3AIContext *		getAIUpper( void)						{ return m_pAIUpper;		}

	/** \brief Upper AI State를 설정합니다. */
	void				setAIUpper( i3AIContext * pCtx)			{ m_pAIUpper = pCtx; }

	/** \brief Dual의 Upper Left AI State를 반환합니다. */
	i3AIContext *		getAIUpper_Left( void)					{ return m_pAIUpper_Left; }

	/** \brief Dual의 Upper Left AI State를 설정합니다. */
	void				setAIUpper_Left( i3AIContext * pCtx)	{
		if( m_pAIUpper_Left != nullptr )m_pAIUpper_Left->SetExclusiveEnable( false, false);
		m_pAIUpper_Left = pCtx;
	}

	/** \brief Upper Bone ID를 반환합니다. */
	INT8				getUpperBoneID( INT32 idx)				
	{	//렙터스팅
		I3_BOUNDCHK( idx, m_pOwner->_GetUpperBoneCount());
		return m_UpperBoneList[ idx]; 
	}

	/** \brief Lower Bone ID를 반환합니다. */
	INT8				getLowerBoneID( INT32 idx)				
	{ 
		if(m_pOwner->IsDino())
		{
			I3_BOUNDCHK( idx, CHARA_LOWER_DINO_BONE_COUNT); 
		}
		else
		{
			I3_BOUNDCHK( idx, CHARA_LOWER_BONE_COUNT); 
		}

		return m_LowerBoneList[ idx]; 
	}

	/** \brief 왼발 스텝 시간을 반환합니다. */
	REAL32				getLeftStepTime( void)					{ return m_fLeftStepTime; }

	/** \brief 왼발 스텝 시간을 설정합니다. */
	void				setLeftStepTime( REAL32 tm)				{ m_fLeftStepTime = tm; }

	/** \brief 오른발 스텝 시간을 반환합니다. */
	REAL32				getRightStepTime( void)					{ return m_fRightStepTime; }

	/** \brief 왼발 스텝 시간을 설정합니다. */
	void				setRightStepTime( REAL32 tm)			{ m_fRightStepTime = tm; }

	/** \brief 뛰는 방향에 따른 Lower AI id를 반환합니다. */
	ID_LOWER_AI			getMoveIdByDir( VEC3D * vDir);

	/** \brief 걷는 방향에 따른 Lower AI id를 반환합니다. */
	ID_LOWER_AI			getWalkIdByDir( VEC3D * vDir);

	/** \brief 뛰기 Elapse 시간을 반환합니다. */
	REAL32				getRunElapseTime( void)					{ return m_fRunElapseTime; }

	/** \brief 뛰기 Elapse 시간을 설정합니다. */
	void				setRunElapseTime( REAL32 tm)			{ m_fRunElapseTime = tm; }

	/** \brief 장비 소리 인덱스를 반환합니다. */
	INT32				getAccessorySoundIdx( void)				{ return m_nAccessory; }

	/** \brief 장비 소리 인덱스를 설정합니다. */
	void				setAccessorySoundIdx( INT32 idx)		{ m_nAccessory = idx; }

	/** \brief 뛰는 시간을 반환합니다. 
		\note 방향에 따른 한 스텝의 길이입니다. */
	REAL32				getRunDuration( void)					{ return m_fRunDuration; }

	/** \brief 뛰는 시간을 설정합니다.
		\note 한 애니메이션의 총 길이가 됩니다. */
	void				setRunDuration( REAL32 tm)				{ m_fRunDuration = tm; }

	/** \brief 상체의 애니메이션 시간을 반환합니다. */
	REAL32				getUpperAniTimer( void)					{ return m_fUpperAniTimer; }

	/** \brief 상체의 애니메이션 시간을 설정합니다. */
	void				setUpperAniTimer( REAL32 tm)			{ m_fUpperAniTimer = tm; }

	/** \brief Common AI Context를 설정합니다. */
	void				setAICommon( i3AIContext * pCtx)		{ m_pCommon = pCtx; }

	/** \brief Common AI Context를 반환합니다. */
	i3AIContext *		getAICommon( void)						{ return m_pCommon; }

	/** \brief Common AI Id를 설정합니다. */
	void				setAICommonID( INT32 idx, INT32 id)
	{
		I3_BOUNDCHK( idx, ID_COMMON_AI_MAX);
		I3ASSERT( id < 128); // -1이 들어오는 경우가 있기 때문에. 2011.11.29. ugeun.ji
		m_nAICommonId[ idx] = (INT8) id;
	}

	/** \brief Common AI id를 반환합니다. */
	INT32				getAICommonID( INT32 idx )				{ I3ASSERT( idx < ID_COMMON_AI_MAX);	return (INT32) m_nAICommonId[ idx]; }

	/** \brief 이동 점프 여부를 반환합니다. */
	bool				isEnableRunJump( void)					{ return m_bRunJump; }

	/** \brief 이동 점프 여부를 설정합니다. */
	void				setEnableRunJump( bool bVal)			{ m_bRunJump = bVal; m_rJumpAnimScaleValue = -1.f;}

	/** \brief 이동 점프에 대한 애니메이션 속도를 반환합니다. */
	REAL32				getRunJumpAnimScale( void)				{ return m_rJumpAnimScaleValue; }

	/** \brief 이동 점프에 대한 애니메이션 속도를 설정합니다. */
	void				setRunJumpAnimScale( REAL32 rVal)		{ m_rJumpAnimScaleValue = rVal; }

	/** \brief 이동 점프의 스텝 시간을 반환합니다. */
	REAL32				getRunJumpStepTime( void)				{ return m_rLowerJumpStepTime; }

	/** \brief 이동 점프의 스텝 시간을 설정합니다. */
	void				setRunJumpStepTime( REAL32 rVal)		{ m_rLowerJumpStepTime = rVal; }

	/** \brief 상체의 걷기 상태인지 반환합니다. */
	bool				isUpperWalk(void)						{ return m_bUpperWalk;}

	/** \brief 상체의 걷기 상태 여부를 설정합니다. */
	void				setUpperWalk(bool bWalk)				{ m_bUpperWalk = bWalk;}

	/** \brief 상체 애니메이션의 길이를 반환합니다. */
	inline REAL32		getUpperDuration( void)
	{
		if( !isAIUpper())	return 0.0f;
		return (getAIUpper()->getCurrentAIState() != nullptr ? getAIUpper()->getCurrentAIState()->getDuration() : 0.0f);
	}

	/** \brief 상체 애니메이션의 속도를 감안한 길이를 반환합니다. */
	inline REAL32		getUpperActualDuration( void)
	{
		if( !isAIUpper())	return 0.0f;
		return (getAIUpper()->getCurrentAIState() != nullptr ? getAIUpper()->getCurrentAIState()->getActualDuration() : 0.0f);			
	}

	/** \brief 상체의 현재 애니메이션 시간을 반환합니다. */
	inline REAL32		getUpperLocalTime( void)
	{
		if( !isAIUpper())	return 0.0f;
		return getAIUpper()->getLocalTime();
	}

	/** \brief 하체의 애니메이션 길이를 반환합니다. */
	inline REAL32		getLowerDuration( void)
	{
		if( !isAILower())	return 0.0f;
		return (m_pAILower->getCurrentAIState() != nullptr ? m_pAILower->getCurrentAIState()->getDuration() : 0.0f);
	}

	/** \brief 하체 애니메이션의 속도를 감안한 길이를 반환합니다. */
	inline REAL32		getLowerActualDuration( void)
	{
		if( !isAILower())	return 0.0f;
		return (m_pAILower->getCurrentAIState() != nullptr ? m_pAILower->getCurrentAIState()->getActualDuration() : 0.0f);
	}

	/** \brief 하체의 현재 애니메이션 시간을 반환합니다. */
	inline REAL32		getLowerLocalTime( void)
	{
		if( !isAILower())	return 0.0f;
		return m_pAILower->getLocalTime();
	}

public:
	CGameCharaAnimContext();
	virtual ~CGameCharaAnimContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;

	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV() override;
	virtual void	OnChange3PV() override;

	virtual void	OnChangeLocal() override;
	virtual void	OnChangeNetwork() override;
	virtual void	OnChangeNPC() override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;
	
	/** \brief Bone List를 작성합니다.
		\note Bone에 대한 처리를 할 시 필요함. */
	virtual void	BindBoneList();

	/** \brief 상체 Animation에 대한 SyncTime을 반환합니다. */
	REAL32			GetUpperSyncTime( void);

	/** \brief 하체 Animation에 대한 SyncTime을 반환합니다. */
	REAL32			GetLowerSyncTime( void);

	/** \brief 하체에 대한 Time Scale을 반환합니다. */
	REAL32			getLowerTimeScale( void)								{ return m_rLowerAnimScale; }

	/** \brief 하체에 대한 time Scale을 설정합니다
		\note 하체 Bone에 대해서 설정 후 Update를 합니다. */
	void			SetLowerTimeScale( REAL32 scale);

	/** \brief 상체에 대한 Time Scale을 반환합니다. */
	REAL32			getUpperTimeScale( void)								{ return m_rUpperAnimScale; }

	/** \brief 상체에 대한 Time Scale을 설정합니다.
		\note 상체 Bone에 대해서 설정 후 Update를 합니다. */
	void			SetUpperTimeScale( REAL32 scale);

	/** \brief 상체의 좌측 Bone에 대해서 time scale을 설정합니다. */
	void			SetUpperLeftTimeScale( REAL32 scale);

	/** \brief 상체의 우측 bone에 대해서 time scale을 설정합니다. */
	void			SetUpperRightTimeScale( REAL32 scale);

	/** \brief 1,3인칭 Animation을 설정합니다. */
	void			ChangeAnimSet( CHARA_VIEW_TYPE type, bool bAnimSync);

	/** \brief 하체에 맞춰 Animation View를 설정합니다. */
	void			ChangeAnimSetLowerSync( CHARA_VIEW_TYPE type);

	// 애니메이션을 강제로 돌리기 위한 용도
	void			SetAIStateImmediately( REAL32 tm);

	/** \brief Animation State를 설정하고 Animation을 적용합니다. */
	void			ChangeAnimationContext( void);

	// revision 36076 C4 설치중 무기 변경 시 총 발사 가능하고 C4 설치 사운드가 계속 play되던 문제 수정
	/** \brief 현재 AI를 종료합니다.*/
	// 엔진에서 다음 AI로 변경시 사용되는 코드와 동일. 
	// 엔진쪽에서 함수를 따로 빼야 할듯하지만 일단은 클라에 추가
	// 강제로 해야될때만 사용하길 추천
	// AI에 있는 사운드 종료시 사용
	// tm 은 Finish일 때 필요없어서 default는 0.0f
	void			FinishCurrentAIUpper( REAL32 tm = 0.f);

	/************************************************************************************/
	/*							Animation Callback functions							*/
	/************************************************************************************/
	/** \brief Idle 동작중 */
	void			CallbackAnim_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 공격중 */
	void			CallbackAnim_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief 공격 중지 */
	void			CallbackAnim_AttackFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 공격 준비 동작을 중지 */
	void			CallbackAnim_AttackReadyFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 이동 동작중 */
	void			CallbackAnim_MoveRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 무기 교체중 */
	void			CallbackAnim_WeaponSwapRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 탄약 장전중 */
	void			CallbackAnim_LoadBulletRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 탄약 장전 중지 */
	void			CallbackAnim_LoadBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 볼트액션중 */
	void			CallbackAnim_FireBulletRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 볼트액션 중지 */
	void			CallbackAnim_FireBulletFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 탄창 장전중 */
	void			CallbackAnim_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 뛰기중 */
	void			CallbackAnim_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 착지중 */
	void			CallbackAnim_JumpEndRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 착지 중지 */
	void			CallbackAnim_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 피격 동작중 */
	void			CallbackAnim_DamageRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 사망 동작 중지 */
	void			CallbackAnim_DeathFinish( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief Extension 활성화 중 */
	void			CallbackAnim_ExtensionActivateRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Extension 비활성화 중 */
	void			CallbackAnim_ExtensionDeactivateRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle A 중 */
	void			CallbackAnim_Profile_IdleARun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle A 중지 */
	void			CallbackAnim_Profile_IdleAFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle B 중 */
	void			CallbackAnim_Profile_IdleBRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle B 중지 */
	void			CallbackAnim_Profile_IdleBFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Change 중 */
	void			CallbackAnim_Profile_IdleChangeRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Change 중지 */
	void			CallbackAnim_Profile_IdleChangeFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Stand 중 */
	void			CallbackAnim_Profile_IdleStandRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Profile Idle Stand 중지 */
	void			CallbackAnim_Profile_IdleStandFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief Proflie Ceremony중 */
	void			CallbackAnim_Profile_CeremonyRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 왼쪽 Idle 중 */
	void			CallbackAnim_Dual_Left_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);
	
	/** \brief 왼쪽 공격중 */
	void			CallbackAnim_Dual_Left_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 왼쪽 뛰기중 */
	void			CallbackAnim_Dual_Left_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 왼쪽 착지 중지 */
	void			CallbackAnim_Dual_Left_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 오른쪽 Idle 중 */
	void			CallbackAnim_Dual_Right_IdleRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 오른쪽 공격중 */
	void			CallbackAnim_Dual_Right_AttackRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 오른쪽 뛰기중 */
	void			CallbackAnim_Dual_Right_JumpStartRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 오른쪽 착지 중지 */
	void			CallbackAnim_Dual_Right_JumpEndFinish( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 양손 무기 교체중 */
	void			CallbackAnim_Dual_SwapWeaponRun( i3AIContext * pCtx, REAL32 rDeltatime);

	/** \brief 양손 무기 탄창 교체중 */
	void			CallbackAnim_Dual_LoadMagazineRun( i3AIContext * pCtx, REAL32 rDeltatime);

	// ShotGun3
	void			CallbackAnim_LoadMagazine_ReadyRun( i3AIContext * pCtx, REAL32 rDeltatime);

protected:
	/** \brief 상체 animation을 설정한다. 
		\param[in] aiID ID_UPPER_AI index
		\param[in] startTime 시작 시간
		\param[in] scale animation scale 값 */
	bool			_Play_Anim_Upper( ID_UPPER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief 하체 animation을 설정한다.
		\param[in] aiID ID_LOWER_AI index
		\param[in] startTime 시작 시간
		\param[in] scale animation scale 값 */
	bool			_Play_Anim_Lower( ID_LOWER_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief Dual 상체 animation을 설정한다.
		\param[in] aiID ID_UPPER_DUAL_AI index
		\param[in] startTime 시작 시간
		\param[in] scale animation scale 값 */
	bool			_Play_Anim_DualUpper( ID_UPPER_DUAL_AI aiID, REAL32 startTime = 0.f, REAL32 scale = 1.f);

	/** \brief animation을 중지한다. */
	void			_Stop_Anim( void);

	/** \brief 하체 Pose에 대해서 Start Time을 맞춘다. */
	bool			_Play_Body_Sync( void);

	/** \brief Common Animation을 한다.*/
	bool			_Play_Anim_Common( ID_COMMON_AI aiID);

	/** \brief 무기에 맞는 애니메이션을 로드한다. */
	void			_PreloadAnimation( T_ItemID itemID);
};


#endif

