#include "pch.h"
#include "GunInfo.h"
#include "GameCharaBoneContext.h"
#include "GameCharaBase.h"
#include "TracerBullet.h"
#include "../../../../Common/CommonSource/QuestDef.h"
#include "GameCharaDef.h"
#include "GameCharaActionContext.h"
#include "WeaponBase.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CGunInfo);//, CWeaponInfo);

void CGunInfo::_ReadCrosshairKey( i3RegKey * pKey)
{
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Expand"),		m_rCrossHairExpand);				// 흔들린 크로스 헤어가 복구되는데 걸리는 시간.
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Min"),			m_rCrossHairMin);					// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Max"),			m_rCrossHairMax);					// CorssHair가 최대로 클 때의 크기 (Clipping-Space에서의 크기)
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Damage"),	m_rCrossHairSizeDamage);			// 표준상태에서의 AR크기
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Jump"),		m_rCrossHairSizeJump);				// 점프시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Move"),		m_rCrossHairSizeMove);				// 조준시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_MoveSit"),	m_rCrossHairSizeMoveSit);			// 앉아서 조준시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Walk"),		m_rCrossHairSizeWalk);				// 조준시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_WalkSit"),	m_rCrossHairSizeWalkSit);			// 앉아서 조준시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Stand"),	m_rCrossHairSizeStand);				// 대미지시 "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Sit"),		m_rCrossHairSizeSit);				// 대미지시 "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_Zoom"),		m_rCrossHairSizeZoom);				// 대미지시 "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Size_NoZoom"),	m_rCrossHairSizeNoZoom);			// 대미지시 "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_Type"),			m_CrossHairInvisible);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_SpeedFactor"),	m_CrossHairSpeedFactor);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_ExpandTime"),	m_CrossHairExpandTime);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("CrossHair_ContractTime"),	m_CrossHairContractTime);
}

/*virtual*/ WEAPON::ZOOM_TYPE CGunInfo::GetTypeZoom() const
{
	return m_TypeZoom;
}

/*virtual*/ REAL32 CGunInfo::GetThrowSpeed() const
{
	return m_fThrowSpeed;
}

// 발사체 개수
/*virtual*/ INT32 CGunInfo::GetFireBulletCount() const
{
	return m_nFireBulletCount;
}

// 발사했을 때, 소모하는 총알 개수
/*virtual*/ INT32 CGunInfo::GetFireDecBulletCount() const
{
	return m_nFireDecBulletCount;
}

// 발사 형식 (0:연발, 1:단발, 2, 3, 4...:점사)	
/*virtual*/ INT32 CGunInfo::GetAutoFireCount() const
{
	return m_nAutoFireCount;
}

// 점사 시의 발사 간격
/*virtual*/ REAL32 CGunInfo::GetAutoFireDelay() const
{
	return m_rAutoFireDelay;
}

BOOL CGunInfo::GetAutoPointShotEnable() const
{
	return m_nAutoPointShotEnable;
}

// 장전 손잡이를 당기는 시간
/*virtual*/ REAL32 CGunInfo::GetLoadBulletTime() const
{
	return m_rLoadBulletTime;
}
// 재장전에 걸리는 시간
/*virtual*/ REAL32 CGunInfo::GetReloadTime() const
{
	return m_rReloadTime;
}

// 장전 가능 탄알 수
/*virtual*/ INT32 CGunInfo::GetLoadBullet() const
{
	return m_nLoadBullet;
}
// 재장전 타입
/*virtual*/ INT32 CGunInfo::GetLoadBulletType() const
{
	return m_nLoadBulletType;
}

// Gauge Reload 타입
/*virtual*/ INT32 CGunInfo::GetGaugeReload() const
{
	return m_nGaugeReload;
}
// 1번 장전에 채워지는 총알 개수 (보통은 Load Bullet 개수)
/*virtual*/ INT32 CGunInfo::GetReloadBulletCount() const
{
	return m_nReloadBulletCount;
}
// 최대 소유 탄알 수
/*virtual*/ INT32 CGunInfo::GetMaxBullet() const
{
	return m_nMaxBullet;
}

// Load Magazine 후, Load Bullet을 해야하는가?
/*virtual*/ INT32 CGunInfo::GetNeedToLoadBullet() const
{
	return (_getAttribute() & WEAPON_ATTRIBUTE_NEEDTOLOADBULLET) == WEAPON_ATTRIBUTE_NEEDTOLOADBULLET;
}

//Move( %)
// "Movement" field값 load
/*virtual*/ void CGunInfo::_ReadMovementKey( i3RegKey * pKey)
{
	CWeaponInfo::_ReadMovementKey( pKey);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, "MoveNormal_Zoom",			m_rMoveNormalZoom);				// 무기 망원경 조준시 이동속도
}

// 견착 상태에서의 이동속도 감속 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetMoveNormalZoom() const
{
	return m_rMoveNormalZoom;
}

// Crosshair
// Crosshiar가 넓어질 때, 길이가 길어지는 비율
/*virtual*/ REAL32 CGunInfo::GetCrossHairExpand() const
{
	return m_rCrossHairExpand;
}
// CorssHair가 최소로 작을 때의 크기 (Clipping-Space에서의 크기)
/*virtual*/ REAL32 CGunInfo::GetCrossHairMin() const
{
	return m_rCrossHairMin;
}

/*virtual*/ REAL32 CGunInfo::GetCrossHairMax() const
{
	return m_rCrossHairMax;
}
// Damage를 입었을 때, CrossHair가 넒어지는 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeDamage() const
{
	return m_rCrossHairSizeDamage;
}
// Jump 동작 시에 CrossHair가 넓어지는 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeJump() const
{
	return m_rCrossHairSizeJump;
}

// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeMove() const
{
	return m_rCrossHairSizeMove;
}

/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeMoveSit() const
{
	return m_rCrossHairSizeMoveSit;
}
// 이동 동작 시에 CrossHair가 넒어지는 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeWalk() const
{
	return m_rCrossHairSizeWalk;
}
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeWalkSit() const
{
	return m_rCrossHairSizeWalkSit;
}
// Idle 동작 시에 CrossHair가 넒어지는 비율 (Factor)
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeStand() const
{
	return m_rCrossHairSizeStand;
}

/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeSit() const
{
	return m_rCrossHairSizeSit;
}

// 견착 시에 CrossHair가 넓어지는 비율 (Factor) ?
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeZoom() const
{
	return m_rCrossHairSizeZoom;
}
/*virtual*/ REAL32 CGunInfo::GetCrossHairSizeNoZoom(void) const
{
	return m_rCrossHairSizeNoZoom;
}
// 0 : All Visible 2:Scope Invisible
/*virtual*/ INT32 CGunInfo::GetCrossHairInvisible() const
{
	return m_CrossHairInvisible;
}

// 1발당 수직 반동각
/*virtual*/ REAL32 CGunInfo::GetRecoilVertAngle() const
{
	return m_RecoilVertAngle;
}
// 최대 누적 반동각
/*virtual*/ REAL32 CGunInfo::GetRecoilVertMax() const
{
	return m_RecoilVertMax;
}

INT32 CGunInfo::GetRecoilVertCount() const
{
	return m_RecoilVertCount;
}

REAL32 CGunInfo::GetRecoilVertAngle_Recovertime() const
{
	return m_RecoilVertAngle_Recovertime;
}

REAL32 CGunInfo::GetRecoilHorzAngle_Recovertime() const
{
	return m_RecoilHorzAngle_Recovertime;
}

// 좌우 흔들림 각
/*virtual*/ REAL32 CGunInfo::GetRecoilHorzAngle() const
{
	return m_RecoilHorzAngle;
}
// 최대 누적 좌우 흔들림 각
/*virtual*/ REAL32 CGunInfo::GetRecoilHorzMax() const
{
	return m_RecoilHorzMax;
}

REAL32 CGunInfo::GetCrossHairExpandTime() const
{
	return m_CrossHairExpandTime;
}

REAL32 CGunInfo::GetCrossHairContractTime() const
{
	return m_CrossHairContractTime;
}

REAL32 CGunInfo::GetCrossHairSpeedFactor() const
{
	return m_CrossHairSpeedFactor;
}

// 수평 반동이 적용되기 시작하는 연사 총알 개수
/*virtual*/ INT32 CGunInfo::GetRecoilHorzCount() const
{
	return m_RecoilHorzCount;
}

/*virtual*/ INT32 CGunInfo::GetOneHorzEnable() const
{
	return m_OneHorzEnable;
}

INT32 CGunInfo::GetOneVertEnable() const
{
	return m_OneVertEnable;
}

// 편차각
/*virtual*/ REAL32 CGunInfo::GetDeviation() const
{
	return m_Deviation;
}
// 최대 누적 편차각
/*virtual*/ REAL32 CGunInfo::GetDeviationMax() const
{
	return m_DeviationMax;
}

// 인벤토리와 상점에 표기되는 Hit Rate 값으로 기본 0 이상값이 설정되어 있는 경우만 사용한다. 
// GUI에서 이값이 설정되어 있지 않으면 GetDeviation() 함수를 사용함.
/*virtual*/ REAL32 CGunInfo::GetDeviationOnlyUI() const
{
	return m_DeviationOnlyUI;
}

INT32 CGunInfo::GetFireDelayOnlyUI() const
{
	return m_FireDelayOnlyUI;
}

INT32 CGunInfo::GetRecoilVertOnlyUI() const
{
	return m_RecoilVertOnlyUI;
}

INT32 CGunInfo::GetRecoilHorzOnlyUI() const
{
	return m_RecoilHorzOnlyUI;
}

// 0:총기반동에 무반응   1:완전반응
/*virtual*/ REAL32 CGunInfo::GetCamSyncRate() const
{
	return m_CamSyncRate;
}
/*virtual*/ REAL32 CGunInfo::GetDPD_Rate() const
{
	return m_DPD_Rate;
}

void CGunInfo::_ReadBulletTracerKey( i3RegKey * pKey)
{
	FIND_REG_DATA( pKey, "BulletSmokeColor",			&m_cBulletSmokeColor);
	FIND_REG_DATA( pKey, "BulletSmokeRepeatTexSize",	&m_fBulletSmokeRepeatTexSize);
	FIND_REG_DATA( pKey, "BulletSmokeGrowThickness",	&m_fBulletSmokeGrowThickness);	
	FIND_REG_DATA( pKey, "BulletSmokeLifeTime",			&m_fBulletSmokeLifeTime);
	FIND_REG_DATA( pKey, "BulletSmokeThickness",		&m_fBulletSmokeThickness);
	FIND_REG_DATA( pKey, "BulletSmokeStartSpeed",		&m_fBulletSmokeStartSpeed);
	FIND_REG_DATA( pKey, "BulletSmokeEndSpeed",			&m_fBulletSmokeEndSpeed);
}

//	총알 궤적 연기 색상	
/*virtual*/ void CGunInfo::GetBulletSmokeColor( I3COLOR * pOut) const
{
	i3Color::Copy( pOut, const_cast<I3COLOR*>(&m_cBulletSmokeColor));
}	

//	총알 궤적 연기 이미지가 타일링되는 길이 (단위: m)
/*virtual*/ REAL32 CGunInfo::GetBulletSmokeRepeatTexSize() const
{
	return m_fBulletSmokeRepeatTexSize;
}
/*virtual*/ REAL32 CGunInfo::GetBulletSmokeGrowThickness() const
{
	return m_fBulletSmokeGrowThickness;
}
/*virtual*/ REAL32 CGunInfo::GetBulletSmokeLifeTime() const
{
	return m_fBulletSmokeLifeTime;
}
/*virtual*/ REAL32 CGunInfo::GetBulletSmokeThickness() const
{
	return m_fBulletSmokeThickness;
}

/*virtual*/ REAL32 CGunInfo::GetBulletSmokeStartSpeed() const
{
	return m_fBulletSmokeStartSpeed;
}
/*virtual*/ REAL32 CGunInfo::GetBulletSmokeEndSpeed() const
{
	return m_fBulletSmokeEndSpeed;
}

void CGunInfo::_ReadDamageKey( i3RegKey * pKey)
{
	CWeaponInfo::_ReadDamageKey( pKey);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("DPD_Rate"),				m_DPD_Rate);						// 무기별 데미지 값 (DistanceDamageSlope 대신...)
	GlobalFunc::VAR_FIND_REG_DATA( pKey, "BulletWeight",			m_rBulletWeight);
}

REAL32 CGunInfo::GetBulletWeight() const
{
	return m_rBulletWeight;
}

void CGunInfo::_ReadAccuracyKey( i3RegKey * pKey)
{
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilHorzAngle"),			m_RecoilHorzAngle);				// 조준시   "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilHorzMax"),			m_RecoilHorzMax);					// 발사시   "

	INT32 nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("RecoilHorzCount"),			&nTemp);				// 발사시   "
	I3_BOUNDCHK( nTemp, 65535);
	m_RecoilHorzCount = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA(pKey, LS_STR("RecoilVertCount"), &nTemp);							// 발사시   "
	I3_BOUNDCHK(nTemp, 65535);
	m_RecoilVertCount = (UINT16)nTemp;

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilVertAngle"),			m_RecoilVertAngle);				// 대미지시 "
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilVertMax"),			m_RecoilVertMax);					// 대미지시 "

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("Deviation"),				m_Deviation);						// 편차각
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("Deviation_Max"),			m_DeviationMax);					// 최대 누적 편차각
	FIND_REG_DATA( pKey, "Deviation_OnlyUI",		&m_DeviationOnlyUI);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("OneHorzEnable"),			m_OneHorzEnable);		//수평반동 개산 방식.(구/신 : 0/1)
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("OneVertEnable"),			m_OneVertEnable);		//수평반동 개산 방식.(구/신 : 0/1)
	FIND_REG_DATA( pKey, "RecoilVert_OnlyUI",		&m_RecoilVertOnlyUI);
	FIND_REG_DATA( pKey, "RecoilHorz_OnlyUI",		&m_RecoilHorzOnlyUI);

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilVertAngle_Recovertime"),		m_RecoilVertAngle_Recovertime);
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("RecoilHorzAngle_Recovertime"),		m_RecoilHorzAngle_Recovertime);
}

/*virtual*/void CGunInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	INT32 nTemp =0;

	CWeaponInfo::ReadWeaponInfo(pKey);

	FIND_REG_DATA( pKey, LS_STR("_ZoomType"), &nTemp);				//	줌 방식
	m_TypeZoom = (WEAPON::ZOOM_TYPE)nTemp;

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("Throw_Speed"),				m_fThrowSpeed);				// RPG-7에서 사용됩니다.
	
	
	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireBulletCount"),			&nTemp);				// 발사 간격
	I3_BOUNDCHK( nTemp, 255);
	m_nFireBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireDecBulletCount"),		&nTemp);			// 발사 간격
	I3_BOUNDCHK( nTemp, 255);
	m_nFireDecBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("AutoFireCount"),			&nTemp);					// 발사 간격
	I3_BOUNDCHK( nTemp, 255);
	m_nAutoFireCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA(pKey, "AutoPointShotEnable",		&nTemp);					// 신규 점사 방식 작동.
	m_nAutoPointShotEnable = (UINT8)nTemp;

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("AutoFireDelay"),			m_rAutoFireDelay);			// 발사 간격

	FIND_REG_DATA( pKey, "FireDelay_OnlyUI",		&m_FireDelayOnlyUI);		// UI 전용 연사력

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("LoadBullet"),				&nTemp);					// 탄창당 최대 탄수
	//I3_BOUNDCHK( nTemp, 65535);
	m_nLoadBullet = (UINT32) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, "LoadBulletType",				&nTemp);					// 탄의 정보
	//I3_BOUNDCHK( nTemp, 65535);
	m_nLoadBulletType = (UINT32) nTemp;

	nTemp = 0;
	FIND_REG_DATA (pKey, LS_STR("GaugeReload"),					&nTemp);					// Gauge Reload
	m_nGaugeReload = (UINT32)nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("ReloadBulletCount"),		&nTemp);				// 탄창당 최대 탄수
	I3_BOUNDCHK( nTemp, 65535);
	m_nReloadBulletCount = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("MaxBullet"),				&nTemp);						// 탄창 수
	//I3_BOUNDCHK( nTemp, 65535);
	m_nMaxBullet = (UINT32) nTemp;

	_FindAttributeKey( pKey, "PumpAction");
	_FindAttributeKey( pKey, "ShotgunMasterC");
	_FindAttributeKey( pKey, "FastReloadAnimation");
	_FindAttributeKey( pKey, "LoadBulletLeftBarrel");
	_FindAttributeKey( pKey, "LoadMagazineLeftBarrel");
	_FindAttributeKey( pKey, "LoadMagazineReady");
	_FindAttributeKey( pKey, "LoadMagToLoadBullet");
	_FindAttributeKey( pKey, "ReloadLoopAnimation");

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("LoadBulletTime"),			m_rLoadBulletTime);				// 장전 손잡이를 당기는 시간
	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("ReloadTime"),				m_rReloadTime);					// 재장전에 걸리는 시간
	_FindAttributeKey( pKey, "NeedToLoadBullet");

	GlobalFunc::VAR_FIND_REG_DATA( pKey, "CamSyncRate",				m_CamSyncRate);

	i3RegKey * pChildKey = Config::FindKey(LS_STR("Crosshair"), pKey);
	if( pChildKey != nullptr)
		_ReadCrosshairKey( pChildKey);

	pChildKey = Config::FindKey( "BulletTracer", pKey);
	if( pChildKey != nullptr)
		_ReadBulletTracerKey( pChildKey);

	pChildKey = Config::FindKey(LS_STR("Accuracy"), pKey);
	if( pChildKey != nullptr)
		_ReadAccuracyKey( pChildKey);
}

/*virtual*/ void CGunInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponInfo::OnEncriptInfo( u8Key);
}

void CGunInfo::FillBulletCountString(i3::stack_wstring& out_wstrBulletCount)
{
	if(WEAPON::FIRE_RANGE == GetTypeFire()) {
		i3::sprintf(out_wstrBulletCount, L"%d/%d", GetLoadBullet(), GetMaxBullet());
	}
	else {
		out_wstrBulletCount = L"-"; 
	}
}

/*virtual*/REAL32 CGunInfo::CalcDeviationByAction(CGameCharaBase* pOwner, WeaponBase * pWeapon, REAL32 fAddDeviation)
{
	if ( pWeapon->getOwner() == nullptr)
		return 0.f;

	REAL32 base = pWeapon->GetCrossHairMin();
	REAL32 newDeviation = 0.f;

	newDeviation = base;
	
	if( pOwner->getActionContext()->isBody( CHARA_BODYLOWER_STAND))
	{
		// Stand
		REAL32 crossHairSizeStand = GetCrossHairSizeStand();
		
		if ( pOwner->isCharaStateMask(CHARA_STATE_CLIMB) )
			crossHairSizeStand *= g_pConfigEx->GetLadderPenaltyRate_CrossHairSize();

		newDeviation += (base * crossHairSizeStand);
	}

	// xgkick
	if( pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
	{
		newDeviation += (base * GetCrossHairSizeZoom());
	}
	else
	{
		// 만약 Sniper 총이라면...
		if( pWeapon->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_SNIPER)
		{
			newDeviation += GetCrossHairSizeNoZoom();
		}
	}

	switch( pOwner->getActionContext()->getLower())
	{
		case CHARA_LOWER_IDLE:
			if( pOwner->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
				newDeviation += (base * GetCrossHairSizeSit());	
			break;
		case CHARA_LOWER_RUN :	
			if( pOwner->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
				newDeviation += (base * GetCrossHairSizeMoveSit());	
			else
				newDeviation += (base * GetCrossHairSizeMove());	
			break;

		case CHARA_LOWER_WALK :	
			if( pOwner->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
				newDeviation += (base * GetCrossHairSizeWalkSit());	
			else
				newDeviation += (base * GetCrossHairSizeWalk());	
			break;

		case CHARA_LOWER_JUMP : newDeviation += (base * GetCrossHairSizeJump());	break;
	}

	if( pOwner->getActionContext()->isUpper( CHARA_UPPER_DAMAGE ))
	{
		newDeviation += (base * GetCrossHairSizeDamage());
	}

	// 호칭등 추가 요소에 의한 변동값
	newDeviation += fAddDeviation;

	if( newDeviation < 0.0f)
		newDeviation = 0.0f;

	REAL32 rMax = GetCrossHairMax();
	if( newDeviation > rMax)
		newDeviation = rMax;
	
	return newDeviation;
}

void CGunInfo::SetRecoilVert(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase * pWeapon, REAL32 fRate)
{
	REAL32 rVal = GetRecoilVertMax() + ( GetRecoilVertMax() * fRate);

	INT32 limitCount = pWeapon->GetRecoilVertCount(this, bApplyExt);
	bool bDoRecoilVert = false;

	if (limitCount != 0)
	{
		if (pWeapon->GetOneVertEnable(this, bApplyExt))
			bDoRecoilVert = (limitCount <= pWeapon->getAutoFireCount());	//신 적용 방식 1발째부터 검사가능.
		else
			bDoRecoilVert = (limitCount < pWeapon->getAutoFireCount());		//구 적용 방식 1발은 검사가 불가능 2발 이상부터 검사가능.
	}
	else // Vertical은 LimitCount가 0일때는 무조건 돌아야 합니다.
		bDoRecoilVert = true;

	if (bDoRecoilVert)
	{
		REAL32 factor = 1.0f;

		if (bApplyExt)		factor = GetExtRecoilVertFactor();

		recoilData.m_RecoilAngle = recoilData.m_RecoilAngle + pWeapon->GetRecoilVertAngle() * factor;
		if (recoilData.m_RecoilAngle > rVal)
			recoilData.m_RecoilAngle = rVal;

		recoilData.m_RecoilVertAngle_Recovertime = GetRecoilVertAngle_Recovertime();

		recoilData.m_RecoilTargetAngle = recoilData.m_RecoilAngle;
		recoilData.m_RecoilTargetTime = log10f(recoilData.m_RecoilAngle / recoilData.m_RecoilVertAngle_Recovertime);
		recoilData.m_RecoilTime = 0.0f;
	}
}

void CGunInfo::SetRecoilHorz(bool bApplyExt, CHARAPLAYER_RECOIL& recoilData, WeaponBase* pWeapon, REAL32 rTheta, REAL32 fRate)
{
	REAL32 rVal = GetRecoilHorzMax() + ( GetRecoilHorzMax() * fRate);

	INT32 limitCount = pWeapon->GetRecoilHorzCount(this, bApplyExt);

	// 지정 개수 이하의 연사에서는 수평 반동을 적용하지 않는다.
	
	bool bDoRecoilHorz = false;
	if( pWeapon->GetOneHorzEnable(this, bApplyExt) ) 
		bDoRecoilHorz = (limitCount <= pWeapon->getAutoFireCount());	//신 적용 방식 1발째부터 검사가능.
	else
		bDoRecoilHorz = (limitCount < pWeapon->getAutoFireCount());		//구 적용 방식 1발은 검사가 불가능 2발 이상부터 검사가능.
	
	if( bDoRecoilHorz )
	{
		REAL32 factor = 1.0f;

		if( bApplyExt)	factor = GetExtRecoilHorzFactor();

		recoilData.m_RecoilHorzAngle = i3Math::Randf2() * pWeapon->GetRecoilHorzAngle() * factor;

		// 만약 변경된 Theta의 값이 지정된 범위를 벗어난다면
		// 반대로 반향으로 적용되도록 한다.
		{
			REAL32 diff = i3Math::abs( (I3_RAD2DEG( rTheta) + recoilData.m_RecoilHorzAngle) - I3_RAD2DEG(recoilData.m_HorzStartAngle));

			if( diff > rVal)
			{
				//I3TRACE( "Turn Angle : %f, %f, %f\n", I3_RAD2DEG(m_Theta), m_RecoilData.m_RecoilHorzAngle, I3_RAD2DEG( m_RecoilData.m_HorzStartAngle));

				recoilData.m_RecoilHorzAngle = -recoilData.m_RecoilHorzAngle;		// 방향 뒤집기
			}
		}

		recoilData.m_RecoilHorzTime = 0.0f;
		recoilData.m_RecoilHorzTimeTarget = MIN( pWeapon->GetFireDelay(), 0.5f);

		if( recoilData.m_RecoilHorzTimeTarget > I3_EPS )
			recoilData.m_rRecoilHorzTimeTarget = 1.0f / recoilData.m_RecoilHorzTimeTarget;
		else
			recoilData.m_rRecoilHorzTimeTarget = 0.0f;

		recoilData.m_RecoilHorzAngle_Recovertime = GetRecoilHorzAngle_Recovertime();
	}
}


/*virtual*/INT32 CGunInfo::GetPercentage( WEAPON::INFO_TYPE nWeaponInfoType) const
{
	// 고정값 반환
	switch(nWeaponInfoType)
	{
	case WEAPON::INFO_DAMAGE:	
		if (WEAPON_CLASS_THROWING_CONTAIN == GetTypeClass())	return -1;
		break;
	case WEAPON::INFO_RAPID:
		if( WEAPON_CLASS_SNIPER == GetTypeClass() )
		{	
			if( GetNeedToLoadBullet() )	return 0;
		}
		if( WEAPON_CLASS_HANDGUN == GetTypeClass() )
		{	
			if( GetNeedToLoadBullet() )	return 0;
		}

		{
			WEAPON::FIRE_TYPE fireType = GetTypeFire();

			if( WEAPON::FIRE_THROWING == fireType )		return -1;
			if( WEAPON::FIRE_MELEE == fireType ) 		return -1;
		}
		break;
	case WEAPON::INFO_HIT_RATE:
		{
			WEAPON::FIRE_TYPE fireType = GetTypeFire();

			if( WEAPON::FIRE_MELEE == fireType )		return 100;
			if( WEAPON::FIRE_THROWING == fireType )		return -1;
		}
		break;
	case WEAPON::INFO_VERT_RECOIL:
	case WEAPON::INFO_HORZ_RECOIL:	
		{
			WEAPON::FIRE_TYPE fireType = GetTypeFire();

			if( WEAPON::FIRE_THROWING == fireType )		return -1;
			if( WEAPON::FIRE_MELEE == fireType )		return -1;
		}
		break;
	case WEAPON::INFO_SPEED:
		break;
	default:
		I3ASSERT_0;
		break;
	}
	
	// 계산식	
	INT32 nPercentage = 0;
	
	switch(nWeaponInfoType)
	{
	case WEAPON::INFO_DAMAGE:
		{
			WEAPON_CLASS_TYPE classType = GetTypeClass();
			
			/*  더 아랫쪽코드에서 nPercentage가 통으로 덮어씌어지므로 걍 안쓴다고 가정하고 주석칩니다. 16.08.05.수빈.
			switch(classType)
			{
			case WEAPON_CLASS_SHOTGUN		:
			case WEAPON_SUBCLASS_SHOTGUN	:
			case WEAPON_CLASS_DUALSHOTGUN	:	nPercentage = GetDamage() * GetFireBulletCount();	break;
			default							:	nPercentage = GetDamage();							break;
			}
			*/
			if( (classType == WEAPON_CLASS_SHOTGUN ) || (classType == WEAPON_SUBCLASS_SHOTGUN))
			{
				nPercentage = GetDamage() * GetFireBulletCount();
			}
			else
			{
				nPercentage = GetDamage();
			}

			if( GetDamageOnlyUI() > 0)
				nPercentage = (INT32)GetDamageOnlyUI();

		}
		break;
	case WEAPON::INFO_RAPID:		
		{
			nPercentage = INT32(100.0f - (GetFireDelay() * 100.0f));	

			if( GetFireDelayOnlyUI() > 0)		// 0 이상 지정된 값이 있다면 이것을 우선적으로 사용 (기본 0)
				nPercentage = GetFireDelayOnlyUI();		//	FireDelay 값 계산이 0으로 계산되어 GUI에 0으로 표기되는 일부 무기 때문에 임의로 표시해주는 값을 사용
		}
		break;
	case WEAPON::INFO_HIT_RATE:
		{
			if (WEAPON_CLASS_SNIPER == GetTypeClass())
			{
				nPercentage = INT32(100.0f - (GetDeviation() * GetDeviationMax() * 0.1f));
			}
			else
			{
				nPercentage = INT32(100.0f - (GetDeviation() * GetDeviationMax()));
			}

			if( GetDeviationOnlyUI() > 0)		// 0 이상 지정된 값이 있다면 이것을 우선적으로 사용 (기본 0)
				nPercentage = static_cast<INT32>(GetDeviationOnlyUI());		//	Deviation 값 계산이 0으로 계산되어 GUI에 0으로 표기되는 일부 무기 때문에 임의로 표시해주는 값을 사용
		}
		break;
	case WEAPON::INFO_VERT_RECOIL:
		{
			nPercentage = INT32( GetRecoilVertAngle() * 30.0f);	

			if( GetRecoilVertOnlyUI() > 0)		// 0 이상 지정된 값이 있다면 이것을 우선적으로 사용 (기본 0)
				nPercentage = GetRecoilVertOnlyUI();		//	RecoilVert 값 계산이 0으로 계산되어 GUI에 0으로 표기되는 일부 무기 때문에 임의로 표시해주는 값을 사용
		}
		break;
	case WEAPON::INFO_HORZ_RECOIL:
		{
			nPercentage = INT32(GetRecoilHorzAngle() * 30.0f);

			if( GetRecoilHorzOnlyUI() > 0)		// 0 이상 지정된 값이 있다면 이것을 우선적으로 사용 (기본 0)
				nPercentage = GetRecoilHorzOnlyUI();		//	RecoilHorz 값 계산이 0으로 계산되어 GUI에 0으로 표기되는 일부 무기 때문에 임의로 표시해주는 값을 사용
		}
		break;
	case WEAPON::INFO_SPEED:
		{
			nPercentage = INT32(GetMoveNormalStand());
		}
		break;
	default:
		I3ASSERT_0;
		break;
	}

	// 범위 제한 0 ~ 100
	if( 100 < nPercentage ) nPercentage = 100;
	if( 0 > nPercentage ) nPercentage = 0;

	return nPercentage;
}

///*virtual*/ void CGunInfo::SetDropWeaponInfo(N_PCINFO_DROPWEAPON* pIn)
//{
//	I3ASSERT(pIn);
//
//	INT32 loadBullet = GetLoadBullet();
//	INT32 maxBullet = GetMaxBullet();
//
//	if( pIn->GetLoadedBullet() > loadBullet )
//		pIn->SetLoadedBullet( (UINT8)loadBullet );
//
//	if( pIn->GetDualLoadedBullet() > loadBullet )
//		pIn->SetDualLoadedBullet( (UINT8) loadBullet );
//	
//	if( pIn->GetMaxBullet() > maxBullet )
//		pIn->SetMaxBullet( (UINT16) maxBullet );
//}

/*virtual*/void CGunInfo::SetBulletSmokeInfo(TracerBulletInfo* pBulletInfo)
{
	I3ASSERT(pBulletInfo);

	pBulletInfo->_LifeTime = GetBulletSmokeLifeTime();
	pBulletInfo->_Thickness = GetBulletSmokeThickness();
	pBulletInfo->_GrowThickness = GetBulletSmokeGrowThickness();
	pBulletInfo->_StartSpeed = GetBulletSmokeStartSpeed();
	pBulletInfo->_EndSpeed = GetBulletSmokeEndSpeed();
	pBulletInfo->_RepeatTexSize = GetBulletSmokeRepeatTexSize();

	GetBulletSmokeColor( &pBulletInfo->_color);
}

const char* CGunInfo::GetStringExtensionType(WEAPON::EXT_TYPE type) const
{ 
	switch( type)
	{
	case WEAPON::EXT_NONE:				return "WEAPON::EXT_NONE";
	case WEAPON::EXT_DUAL_MAG:			return "WEAPON::EXT_DUAL_MAG";
	case WEAPON::EXT_SILENCER:			return "WEAPON::EXT_SILENCER";
	case WEAPON::EXT_ZOOM:				return "WEAPON::EXT_ZOOM";
	case WEAPON::EXT_DOT_SIGHT:			return "WEAPON::EXT_DOT_SIGHT";
	case WEAPON::EXT_STEADY_AIM:			return "WEAPON::EXT_STEADY_AIM";
	case WEAPON::EXT_SCOPE:				return "WEAPON::EXT_SCOPE";
	case WEAPON::EXT_GLAUNCHER:			return "WEAPON::EXT_GLAUNCHER";
	case WEAPON::EXT_SHOTGUN_LAUNCHER:	return "WEAPON::EXT_SHOTGUN_LAUNCHER";
	case WEAPON::EXT_BAYONET:			return "WEAPON::EXT_BAYONET";	
	default: return "unknown EXT";
	}
}
