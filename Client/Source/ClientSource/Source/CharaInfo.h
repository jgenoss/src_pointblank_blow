#if !defined ( _CHARA_INFO_H__)
#define _CHARA_INFO_H__

////////////////////////////////////////////////////////////////////////
// 각 케릭터 유형에 따른 정보를 관장하기 위한 클래스입니다.
// i3Registry에서 케릭터 형태에 따른 정보를 얻어올 예정입니다. - raja -
// 이값은 게임에서 속성 형태로 읽혀질 값입니다. 런타임시의 값은
// 다른곳에서 사용하시길 바랍니다.
////////////////////////////////////////////////////////////////////////
#include "EquipInfo.h"
#include "GameCharaDef.h"
#include "GameCharaEquipDef.h"

#define		DINO_ATTACK_DAMAGE_TYPE_COUNT	6

namespace CHARA {
	enum eDAMAGEPART
	{
		eDAMAGEPART_ARM,
		eDAMAGEPART_BODY,
		eDAMAGEPART_FOOT,
		eDAMAGEPART_HAND,
		eDAMAGEPART_HEAD,
		eDAMAGEPART_LEG,
		eDAMAGEPART_NECK,
		eDAMAGEPART_OTHER,
		eDAMAGEPART_VITAL,

		eDAMAGEPART_COUNT
	};

	inline
	eDAMAGEPART		GetDamagePartByHit( CHARA_HIT_PART part)
	{
		switch( part)
		{
		case CHARA_HIT_JAW:
		case CHARA_HIT_HEAD:		return eDAMAGEPART_HEAD;	// 머리
		case CHARA_HIT_NECK	:
		case CHARA_HIT_LCLAVICLE :
		case CHARA_HIT_RCLAVICLE :
		case CHARA_HIT_BODY1 :
		case CHARA_HIT_BODY2 :
		case CHARA_HIT_BODY3 :
		case CHARA_HIT_ROOT	:		
		case CHARA_HIT_DEATH_TRAMPLE:
		case CHARA_HIT_DEATH_HOWL:	return eDAMAGEPART_BODY;	// 몸통
		case CHARA_HIT_RUARM:
		case CHARA_HIT_RDARM:
		case CHARA_HIT_LUARM:
		case CHARA_HIT_LDARM:		return eDAMAGEPART_ARM;		// 팔
		case CHARA_HIT_LHAND:
		case CHARA_HIT_RHAND:		return eDAMAGEPART_HAND;	// 손
		case CHARA_HIT_LULEG:
		case CHARA_HIT_LDLEG:
		case CHARA_HIT_RULEG:
		case CHARA_HIT_RDLEG:		return eDAMAGEPART_LEG;		// 다리
		case CHARA_HIT_LFOOT:
		case CHARA_HIT_RFOOT:		return eDAMAGEPART_FOOT;	// 발
		case CHARA_HIT_NECK_LOWER:	return eDAMAGEPART_NECK;	// 목	// 공룡용
		case CHARA_HIT_VITAL_EYE:	return eDAMAGEPART_VITAL;	// 급소
		}

		return eDAMAGEPART_OTHER;
	}

	enum eSEX_TYPE {
		eSEX_TYPE_MALE = 0,
		eSEX_TYPE_FEMALE,
		eSEX_TYPE_DINO,
		eSEX_TYPE_SKINY_MALE,	// 기획상 남자, 컬리젼과 본은 여자

		eSEX_TYPE_MAX
	};
};

////////////////////////////////////////////////////////////////////////////////////
//	공용 캐릭터 정보
////////////////////////////////////////////////////////////////////////////////////
class CCharaGlobal : public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaGlobal, i3NamedElement);

private:
	REAL32		m_rInvincibleTime = 0.0f;	//	무적 시간
	REAL32		m_rInvincibleTime_4vs4 = 0.0f;//4vs4모드에서의 무적시간
	REAL32		m_rAimingAngleMax = 0.0f;	//	최대 조준 각도
	REAL32		m_rAimingAngleMin = 0.0f;	//	최소 조준 각도
	REAL32		m_rSpeedReductionTime = 0.0f;		//피격된경우 이동시간 감소의 지속시간
	REAL32		m_rSpeedReductionMaxRate = 0.0f;	//피격된경우 데미지에따른 최대 이동량 감소 비율

	REAL32		m_rPartDamageRateInSuddenDeath[CHARA::eDAMAGEPART_COUNT] = { 0.0f, };	//	경우에 따른 데미지 비율 (%)
	REAL32		m_rMinDamage = 0.0f;

protected:
	UINT8 		m_u8EncryptKey = 0;

public:
	REAL32		GetInvincibleTime() const;
	void		SetInvincibleTime(REAL32 val);
	REAL32		GetInvincibleTime_4vs4() const;
	void		SetInvincibleTime_4vs4(REAL32 val);
	REAL32		GetAimingAngleMax() const;
	void		SetAimingAngleMax(REAL32 val);
	REAL32		GetAimingAngleMin() const;
	void		SetAimingAngleMin(REAL32 val);
	REAL32		GetSpeedReductionTime() const;
	void		SetSpeedReductionTime(REAL32 val);
	REAL32		GetSpeedReductionMaxRate() const;
	void		SetSpeedReductionMaxRate(REAL32 val);
	REAL32		GetInvincibleTime( STAGE_MODE type) const;

public:
	void	ReadInfo( i3RegKey * pKey);
	
	void	OnEncryptInfo( UINT8 u8Key);

	void		ReadDamageByModeInfo( i3RegKey * pKey);
	REAL32		GetDamageRateByMode( CHARA::eDAMAGEPART eParam) const;
	void		CalcDamageByPart(REAL32& rDamage);

	// 낙하 데미지 관련.
private:
	INT32		m_nFallMinDamage = 0;			//	낙하할 경우 받는 최소 데미지 량
	REAL32		m_rFallDamageMinHeight = 0.0f;		//	낙하할 경우 데미지 받는 최소 높이
	REAL32		m_rFallDamageSlope = 0.0f;			//	낙하

public:
	INT32	GetFallMinDamage() const;
	void	SetFallMinDamage(INT32 val);
	REAL32	GetFallDamageMinHeight() const;
	void	SetFallDamageMinHeight(REAL32 val);
	REAL32	GetFallDamageSlope() const;
	void	SetFallDamageSlope(REAL32 val);
};

class CCharaGlobal2 : public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaGlobal2, i3NamedElement);

private:
	REAL32		m_rInvincibleTime = 0.0f;	//	무적 시간
	REAL32		m_rAimingAngleMax = 0.0f;	//	최대 조준 각도
	REAL32		m_rInvincibleTime_4vs4 = 0.0f;//4vs4모드에서의 무적시간
	REAL32		m_rAimingAngleMin = 0.0f;	//	최소 조준 각도
	REAL32		m_rSpeedReductionMaxRate = 0.0f;	//피격된경우 데미지에따른 최대 이동량 감소 비율
	REAL32		m_rSpeedReductionTime = 0.0f;		//피격된경우 이동시간 감소의 지속시간
	
protected:
	UINT8		m_u8EncryptKey = 0;

public:
	REAL32		GetInvincibleTime() const;
	void		SetInvincibleTime(REAL32 val);
	REAL32		GetAimingAngleMax() const;
	void		SetAimingAngleMax(REAL32 val);
	REAL32		GetInvincibleTime_4vs4() const;
	void		SetInvincibleTime_4vs4(REAL32 val);
	REAL32		GetAimingAngleMin() const;
	void		SetAimingAngleMin(REAL32 val);

	REAL32		GetSpeedReductionMaxRate() const;
	void		SetSpeedReductionMaxRate(REAL32 val);
	REAL32		GetSpeedReductionTime() const;
	void		SetSpeedReductionTime(REAL32 val);

public:
	void		ReadInfo( i3RegKey * pKey);
	void		OnEncryptInfo( UINT8 u8Key);

	// 낙하 데미지 관련.
private:
	INT32		m_nFallMinDamage = 0;			//	낙하할 경우 받는 최소 데미지 량
	REAL32		m_rFallDamageSlope = 0.0f;			//	낙하
	REAL32		m_rFallDamageMinHeight = 0.0f;		//	낙하할 경우 데미지 받는 최소 높이

public:
	INT32 GetFallMinDamage() const;
	void SetFallMinDamage(INT32 val);
	REAL32 GetFallDamageMinHeight() const;
	void SetFallDamageMinHeight(REAL32 val);
	REAL32 GetFallDamageSlope() const;
	void SetFallDamageSlope(REAL32 val);
};


class CSpeedReduction
{
public:
	REAL32 GetRate() const;
	void SetRate(REAL32 val);
	REAL32 GetTime() const;
	void SetTime(REAL32 val);
private:
	REAL32 m_rRate = 0.0f;
	REAL32 m_rTime = 0.0f;
};

////////////////////////////////////////////////////////////////////////////////////
//	개인 캐릭터 정보
////////////////////////////////////////////////////////////////////////////////////
class CCharaInfo :	public CEquipInfo
{
	I3_CLASS_DEFINE( CCharaInfo, CEquipInfo);

private:
	REAL32	m_arMoveSpeed[MMT_COUNT][MST_COUNT][MDT_COUNT] = { 0.0f, };
	REAL32	m_rJumpDelay = 0.0f;			// 다음 점프가 가능한 시간
	REAL32  m_rAccelerationTime = 0.3f;	// 최고속도까지 도달하는데 걸리는 시간
	INT32	m_iResParts[EQUIP::ePART_COUNT] = { 0 };

	T_ItemID	m_RepresentID = 0;			// Resource를 공유하는 원본 ID

	T_ItemID	m_TeamBlue_ItemID = 0;		// Blue팀의 ItemID
	T_ItemID	m_TeamRed_ItemID = 0;		// Red팀의 ItemID

public:
	REAL32	GetJumpDelay() const;
	void	SetJumpDelay(REAL32 val);
	REAL32	GetAccelerationTime() const;
	void	SetAccelerationTime(REAL32 val);
	REAL32	GetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT = MMT_NORMAL ) const;
	void	SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT = MMT_NORMAL);
	T_ItemID	GetPartsItemID( EQUIP::ePART parts) const;

	T_ItemID	GetRepresentID( void) const;

	T_ItemID	GetTeamBlue_ItemID( void) const		{ return m_TeamBlue_ItemID; }
	T_ItemID	GetTeamRed_ItemID( void) const		{ return m_TeamRed_ItemID; }

private:
	INT32	m_nHP = 0;					// 캐릭터 기본 체력
	//Jump
	REAL32	m_rNewTempJumpHeight = 0.0f;	
	REAL32  m_rNewTempClunchJumpHeight = 0.0f;	
	REAL32	m_rJumpHeight = 0.0f;			// 점프 가능한 높이
	REAL32	m_rJumpTime = 0.0f;			// 점프시 체공시간( 점프한 위치와 같은 높이가 되는데 걸리는 시간?)
	REAL32	m_rJumpPenaltyTime = 0.0f;		// 점프한후 이시간이 경과되지 않은경우엔 점프시 페널티를 받음
	REAL32	m_rJumpPenaltyRate = 0.0f;		// 페널티가 적용될때 단계별로 누적될 비율의 감소치
	//Weapon
	REAL32	m_rWeaponSwap = 0.0f;			// 무기 교체 시간( 무기에 따라 틀려지지 않을라나?)
	REAL32	m_rScopeWaveVelocity = 0.0f;	// 흔들림 속도
	REAL32	m_rScopeWaveRange = 0.0f;		// 흔들림 범위 (%)
	REAL32	m_rReloadSpeed = 0.0f;			// 장전속도 (%)
	REAL32  m_rShoulderSpeed = 0.0f;		// 견착 속도

	//관성치 관련
	REAL32	m_rAccelRateBackVsFront = 0.0f;// 전진가속도에대한 후진가속도의 비율

	//Height
	VEC4D	m_vCamIKHeight;			//자세에따른 카메라의 허리높이x:stand y:sitmove z:sit
	//FPS Camera의 Position
	VEC3D	m_vFPSCamPos;
	//	Damage Rate by body parts	// 캐릭터 부위별 데미지 비율
	REAL32		m_rPartDamageRate[CHARA::eDAMAGEPART_COUNT] = { 0.0f };

	INT32		m_nCharaSexType = 0;

	//////////////////////////////////////////////////////////////////////////
	//	Add NPC info
	REAL32		m_rNPCAttackDelay = 0.0f;
	REAL32		m_rNPCInvincibleTime = 0.0f;
	REAL32		m_rNPCRespawnTime = 0.0f;
	REAL32		m_rNPCSearchRandomValue = 0.0f;

	// Clunch점프 관련
	REAL32		m_rClunchJumpHeight = 0.0f;
	REAL32		m_rClunchJumpCheckTime = 0.0f;

public:
	INT32	GetHP() const;
	CHARA::eSEX_TYPE	GetCharaSexType() const;
	CHARA::eSEX_TYPE	GetCharaSexTypePHYSIX() const;
	CHARA::eSEX_TYPE	GetCharaSexTypeDISPLAY() const;
	void	SetHP(INT32 val);
	REAL32	GetJumpHeight() const;
	void	SetJumpHeight(REAL32 val);
	
	REAL32	GetNewTempJumpHeight() const;
	void	SetNewTempJumpHeight(REAL32 val);
	REAL32	GetNewTempClunchJumpHeight() const;
	void	SetNewTempClunchJumpHeight(REAL32 val);

	REAL32	GetJumpTime() const;
	void	SetJumpTime(REAL32 val);

	REAL32	GetJumpPenaltyTime() const;
	void	SetJumpPenaltyTime(REAL32 val);
	REAL32	GetJumpPenaltyRate() const;
	void	SetJumpPenaltyRate(REAL32 val);
	REAL32	GetWeaponSwap() const;
	void	SetWeaponSwap(REAL32 val);
	REAL32	GetScopeWaveVelocity() const;
	void	SetScopeWaveVelocity(REAL32 val);
	REAL32	GetScopeWaveRange() const;
	void	SetScopeWaveRange(REAL32 val);
	REAL32	GetReloadSpeed() const;
	void	SetReloadSpeed(REAL32 val);
	REAL32	GetShoulderSpeed() const;
	void	SetShoulderSpeed(REAL32 val);
	REAL32	GetAccelRateBackVsFront() const;
	void	SetAccelRateBackVsFront(REAL32 val);
	void	GetCamIKHeight( VEC4D * pOut) const;
	void	SetCamIKHeight(const VEC4D & val);
	void	GetFPSCamPos( VEC3D * pOut) const;
	void	SetFPSCamPos(const VEC3D & val);

	REAL32	GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const;
	void	SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val);

	REAL32	GetNPCAttackDelay() const;
	void	SetNPCAttackDelay(REAL32 val);

	REAL32 GetNPCInvincibleTime() const;
	void SetNPCInvincibleTime(REAL32 val);
	REAL32 GetNPCRespawnTime() const;
	void SetNPCRespawnTime(REAL32 val);
	REAL32 GetNPCSearchRandomValue() const;
	void SetNPCSearchRandomValue(REAL32 val);
	REAL32 GetClunchJumpHeight() const;
	void SetClunchJumpHeight(REAL32 val);
	REAL32 GetClunchJumpCheckTime() const;
	void SetClunchJumpCheckTime(REAL32 val);


	//등등
public:
	virtual void	ReadInfo( i3RegKey * pKey) override;
	virtual void	OnEncryptInfo( UINT8 u8Key) override;

	void	ReadDamageRateInfo( i3RegKey * pKey);

	// 부위에 따른 이동 속도 감소 비율 및 시간
private:
	CSpeedReduction  m_rSpeedReduction[CHPS_COUNT];
public:
	const CSpeedReduction* GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const;

#if defined( AI_MOVE_TO_WEAPON)
	////////////////////////////////
	// AI State를 무기로 이전함에 따라 필요해진 함수 및 멤버들.
	// 
protected:
	char		m_szAnimName[64] = { 0 };

public:
	const char *	getAnimationName(void)			{ return (const char *) m_szAnimName; }
#endif

	virtual UINT32		GetDinoWeaponID(INT32 idx)	const				{ return 0; }
	virtual void		SetDinoWeaponID(INT32 idx, T_ItemID itemID)		{}

	// 공룡 관련
	virtual REAL32		GetDinoHP_Ratio(INT32 idx) const				{ return 0.f; }
	virtual void		SetDinoHP_Ratio(INT32 idx, REAL32 val)			{}

	virtual REAL32		GetDinoCCRadius() const							{ return 0.f; }
	virtual REAL32		GetDinoCCTall() const							{ return 0.f; }
	virtual REAL32		GetDinoCCStepOffset() const						{ return 0.f; }

	virtual void 		SetDinoCCTall(REAL32 val)						{}
	virtual void 		SetDinoCCStepOffset(REAL32 val)					{}
	virtual void 		SetDinoCCRadius(REAL32 val)						{}

	virtual void 		SetCharaSexType(INT32 val)						{ m_nCharaSexType = val; }

	virtual REAL32		getFootStepTrembleAmp(void) const				{ return 0.f; }
	virtual void		SetFootStepTrembleAmp(REAL32 val)				{}

	virtual INT32		getShapeIndexCCSkill( UINT8 idx) const			{ return 0; }
	virtual void		getCCSkillDescription( i3::rc_wstring& out, UINT8 idx) const	{}

	// 사다리 이동값.
private:
	REAL32 m_rLadderWalkBackSpeed = 0.0f;
	REAL32 m_rLadderWalkFrontSpeed = 0.0f;
	REAL32 m_rLadderWalkSideSpeed = 0.0f;
	REAL32 m_rLadderWalkDiagonalSpeed = 0.0f;
	REAL32 m_rLadderRunBackSpeed = 0.0f;
	REAL32 m_rLadderRunFrontSpeed = 0.0f;
	REAL32 m_rLadderRunSideSpeed = 0.0f;
	REAL32 m_rLadderRunDiagonalSpeed = 0.0f;

public:
	REAL32 GetLadderWalkBackSpeed() const;
	REAL32 GetLadderWalkFrontSpeed() const;
	REAL32 GetLadderWalkSideSpeed() const;
	REAL32 GetLadderWalkDiagonalSpeed() const;
	REAL32 GetLadderRunBackSpeed() const;
	REAL32 GetLadderRunFrontSpeed() const;
	REAL32 GetLadderRunSideSpeed() const;
	REAL32 GetLadderRunDiagonalSpeed() const;
};


// 치팅 방지를 위해 CCharaInfo와 같은 값을 하나 더 복사해서 
// 일정 시간마다 검사한다.

class CCharaInfo2 :	public i3NamedElement
{
	I3_CLASS_DEFINE( CCharaInfo2, i3NamedElement);

private:
	REAL32  m_rAccelerationTime = 0.3f;	// 최고속도까지 도달하는데 걸리는 시간
	REAL32	m_rJumpDelay = 0.0f;			// 다음 점프가 가능한 시간
	REAL32	m_arMoveSpeed[MMT_COUNT][MST_COUNT][MDT_COUNT] = { 0.0f, };
	INT32	m_iResParts[EQUIP::ePART_COUNT] = { 0 };

public:
	REAL32	GetAccelerationTime() const;
	void	SetAccelerationTime(REAL32 val);

	REAL32	GetJumpDelay() const;
	void	SetJumpDelay(REAL32 val);

	REAL32	GetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, EMoveModeType eMMT = MMT_NORMAL ) const;
	void	SetMoveSpeed(EMoveSpeedType eMST, EMoveDirectionType eMDT, REAL32 Value, EMoveModeType eMMT = MMT_NORMAL );
	T_ItemID	GetPartsItemID( EQUIP::ePART parts);

private:
	//Jump
	REAL32	m_rJumpHeight = 0.0f;			// 점프 가능한 높이
	INT32	m_nHP = 0;					// 캐릭터 기본 체력
	REAL32	m_rJumpTime = 0.0f;			// 점프시 체공시간( 점프한 위치와 같은 높이가 되는데 걸리는 시간?)
	REAL32	m_rJumpPenaltyTime = 0.0f;		// 점프한후 이시간이 경과되지 않은경우엔 점프시 페널티를 받음
	INT32	m_nCharaSexType = 0;
public:
	REAL32	GetJumpHeight() const;
	void	SetJumpHeight(REAL32 val);
	INT32	GetHP() const;
	void	SetHP(INT32 val);
	REAL32	GetJumpTime() const;
	void	SetJumpTime(REAL32 val);
	REAL32	GetJumpPenaltyTime() const;
	void	SetJumpPenaltyTime(REAL32 val);

	// 부위에 따른 이동 속도 감소 비율 및 시간
private:
	CSpeedReduction  m_rSpeedReduction[CHPS_COUNT];
public:
	const CSpeedReduction* GetSpeedReduction(CHARA_HIT_PART_SIMPLE ePart) const;

private:
	REAL32	m_rJumpPenaltyRate = 0.0f;		// 페널티가 적용될때 단계별로 누적될 비율의 감소치
	//Weapon
	REAL32	m_rWeaponSwap = 0.0f;			// 무기 교체 시간( 무기에 따라 틀려지지 않을라나?)

	REAL32	m_rScopeWaveRange = 0.0f;		// 흔들림 범위 (%)

	REAL32  m_rShoulderSpeed = 0.0f;		// 견착 속도
	//Height
	VEC4D	m_vCamIKHeight;			//자세에따른 카메라의 허리높이x:stand y:sitmove z:sit
	REAL32	m_rReloadSpeed = 0.0f;			// 장전속도 (%)
	//관성치 관련
	REAL32	m_rAccelRateBackVsFront = 0.0f;// 전진가속도에대한 후진가속도의 비율
	REAL32	m_rScopeWaveVelocity = 0.0f;	// 흔들림 속도
	//FPS Camera의 Position
	VEC3D	m_vFPSCamPos;
	//	Damage Rate
	REAL32	m_rPartDamageRate[CHARA::eDAMAGEPART_COUNT] = { 0.0f };

	//////////////////////////////////////////////////////////////////////////
	//	Add NPC info
	REAL32		m_rNPCAttackDelay = 0.0f;

	REAL32		m_rNPCRespawnTime = 0.0f;
	REAL32		m_rNPCInvincibleTime = 0.0f;
	REAL32		m_rClunchJumpHeight = 0.0f;
	REAL32		m_rNPCSearchRandomValue = 0.0f;
	// Clunch점프 관련
	
	REAL32		m_rClunchJumpCheckTime = 0.0f;

	T_ItemID	m_ITEMID = 0;

protected:
	UINT8 		m_u8EncryptKey = 0;

public:


	REAL32	GetJumpPenaltyRate() const;
	void	SetJumpPenaltyRate(REAL32 val);

	REAL32	GetWeaponSwap() const;
	void	SetWeaponSwap(REAL32 val);

	REAL32	GetScopeWaveRange() const;
	void	SetScopeWaveRange(REAL32 val);

	REAL32	GetShoulderSpeed() const;
	void	SetShoulderSpeed(REAL32 val);

	void	GetCamIKHeight( VEC4D * pOut) const;
	void	SetCamIKHeight(const VEC4D & val);

	REAL32	GetReloadSpeed() const;
	void	SetReloadSpeed(REAL32 val);

	REAL32	GetAccelRateBackVsFront() const;
	void	SetAccelRateBackVsFront(REAL32 val);

	REAL32	GetScopeWaveVelocity() const;
	void	SetScopeWaveVelocity(REAL32 val);

	void	GetFPSCamPos( VEC3D * pOut) const;
	void	SetFPSCamPos(const VEC3D & val);

	REAL32	GetPartDamageRate( CHARA::eDAMAGEPART damagePart) const;
	void	SetPartDamageRate( CHARA::eDAMAGEPART damagePart, REAL32 val);

	REAL32	GetNPCAttackDelay() const;
	void	SetNPCAttackDelay(REAL32 val);

	REAL32	GetNPCRespawnTime() const;
	void	SetNPCRespawnTime(REAL32 val);

	REAL32	GetNPCInvincibleTime() const;
	void	SetNPCInvincibleTime(REAL32 val);

	REAL32	GetClunchJumpHeight() const;
	void	SetClunchJumpHeight(REAL32 val);

	REAL32	GetNPCSearchRandomValue() const;
	void	SetNPCSearchRandomValue(REAL32 val);

	REAL32	GetClunchJumpCheckTime() const;
	void	SetClunchJumpCheckTime(REAL32 val);


	void		SetITEMID( T_ItemID val);
	T_ItemID	GetITEMID( void);

	//등등
public:
	void	ReadInfo( i3RegKey * pKey);

	void	ReadDamageRateInfo( i3RegKey * pKey);
	void	OnEncryptInfo( UINT8 u8Key);
	
	// 사다리 이동값.
private:
	REAL32 m_rLadderWalkBackSpeed = 0.0f;
	REAL32 m_rLadderWalkFrontSpeed = 0.0f;
	REAL32 m_rLadderWalkSideSpeed = 0.0f;
	REAL32 m_rLadderWalkDiagonalSpeed = 0.0f;
	REAL32 m_rLadderRunBackSpeed = 0.0f;
	REAL32 m_rLadderRunFrontSpeed = 0.0f;
	REAL32 m_rLadderRunSideSpeed = 0.0f;
	REAL32 m_rLadderRunDiagonalSpeed = 0.0f;

public:
	REAL32 GetLadderWalkBackSpeed() const;
	REAL32 GetLadderWalkFrontSpeed() const;
	REAL32 GetLadderWalkSideSpeed() const;
	REAL32 GetLadderWalkDiagonalSpeed() const;
	REAL32 GetLadderRunBackSpeed() const;
	REAL32 GetLadderRunFrontSpeed() const;
	REAL32 GetLadderRunSideSpeed() const;
	REAL32 GetLadderRunDiagonalSpeed() const;
};


class CUITemplateInfo_Chara : public i3NamedElement
{
	I3_CLASS_DEFINE(CUITemplateInfo_Chara, i3NamedElement);

public:
	void ReadInfo(i3RegKey* rk);
	INT32 GetType() const;
	INT32 GetLoopLoad() const;
	
	const i3::string& GetName() const;

private:
	INT32 m_type = -1;
	INT32 m_LoopLoad = 0;
	i3::string m_name;		
};

#endif