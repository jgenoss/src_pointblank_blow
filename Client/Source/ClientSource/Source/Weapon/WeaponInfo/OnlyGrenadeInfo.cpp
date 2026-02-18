#include "pch.h"
#include "OnlyGrenadeInfo.h"
#include "../../../../Common/CommonSource/WeaponDefine.h"

I3_CLASS_INSTANCE( COnlyGrenadeInfo);//, i3NamedElement);

void COnlyGrenadeInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	INT32 nTemp = 0;

	FIND_REG_DATA( pKey, LS_STR("FireBulletCount"),			&nTemp);				// 발사 간격
	I3_BOUNDCHK( nTemp, 255);
	m_nFireBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("FireDecBulletCount"),		&nTemp);				// 발사 간격
	I3_BOUNDCHK( nTemp, 255);
	m_nFireDecBulletCount = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("LoadBullet"),				&nTemp);				// 탄창당 최대 탄수
	I3_BOUNDCHK( nTemp, 65535);
	m_nLoadBullet = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("ReloadBulletCount"),		&nTemp);			// 탄창당 최대 탄수
	I3_BOUNDCHK( nTemp, 65535);
	m_nReloadBulletCount = (UINT16) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("MaxBullet"),				&nTemp);					// 탄창 수
	I3_BOUNDCHK( nTemp, 65535);
	m_nMaxBullet = (UINT16) nTemp;

	REAL32 rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_Time"),			&rTemp);			// 폭발 지연시간( Grenade류 등등)
	m_rExplosion_Time = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_Range"),			&rTemp);			// 폭발 유효 범위 : 일반 범위
	m_rExplosion_Range = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_NearRange"),		&rTemp);		// 폭발 유효 범위 : 주변 범위 (ex. Claymore)
	m_rExplosion_NearRange = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Explosion_EffectiveSlope"), &rTemp);	// 3차 폭발 유효 범위
	m_rExplosion_EffectiveSlope = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, "Detected_Range",			&rTemp);			// 감지 거리(Claymore)
	m_rDetected_Range = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("Throw_Speed"),				&rTemp);				// 3차 폭발 유효 범위
	m_rThrowSpeed = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("ExThrow_Speed"),			&rTemp);				// 지뢰 익스텐션 던지는 속도
	m_rExThrowSpeed = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, "EffectiveTime",			&rTemp);				// 이펙트 유효 시간
	m_rEffectiveTime = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("AttachLength"),			&rTemp);				// 벽에 붙는 거리
	m_rAttachLength = rTemp;

	rTemp = 0.0f;
	FIND_REG_DATA( pKey, LS_STR("CheckDamageTerm"),			&rTemp);
	m_rCheckDamageTerm = rTemp;

	GlobalFunc::VAR_FIND_REG_DATA( pKey, LS_STR("IncreaseCount"),			m_nIncreaseCount);

	// KEYWORD : DECOY_SOUND
	FIND_REG_DATA( pKey, "_DefaultDecoySound",		m_strDefaultDecoySound);

	nTemp = 0;
	FIND_REG_DATA( pKey, "UseBombTrigger",			&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_bUseBombTrigger = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, "Dual",					&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_bDual = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, LS_STR("UseThrowDelay"),			&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_bUseThrowDelay = (UINT8) nTemp;

	for(INT32 i=0; i<GRENADE_ATTACK_NUM_MAX; i++)
	{
		char temp[256] = "";

		i3::snprintf(temp, 256, LS_STR("AttackTimeA%d"), i+1);
		GlobalFunc::VAR_FIND_REG_DATA( pKey, temp,			m_rAttackTimeA[i]);

		i3::snprintf(temp, 256, LS_STR("AttackTimeB%d"), i+1);
		GlobalFunc::VAR_FIND_REG_DATA( pKey, temp,			m_rAttackTimeB[i]);
	}

	FIND_REG_DATA( pKey, "Bombtrigger_ITEMID", (INT32*)&m_nBombtriggerItemID);
}

/*virtual*/ void COnlyGrenadeInfo::OnEncriptInfo( UINT8 u8Key)
{
	m_u8EncryptKey = u8Key;
}

INT32 COnlyGrenadeInfo::GetFireBulletCount() const
{
	return m_nFireBulletCount;
}

INT32 COnlyGrenadeInfo::GetFireDecBulletCount() const
{
	return m_nFireDecBulletCount;
}
INT32 COnlyGrenadeInfo::GetLoadBullet() const
{
	return m_nLoadBullet;
}

INT32 COnlyGrenadeInfo::GetReloadBulletCount() const
{
	return m_nReloadBulletCount;
}

INT32 COnlyGrenadeInfo::GetMaxBullet() const
{
	return m_nMaxBullet;
}

//투척 무기류
REAL32 COnlyGrenadeInfo::GetExplosion_Time() const
{
	return m_rExplosion_Time;
}

REAL32 COnlyGrenadeInfo::GetExplosion_Range() const
{
	return m_rExplosion_Range;
}

REAL32 COnlyGrenadeInfo::GetExplosion_NearRange() const
{
	return m_rExplosion_NearRange;
}


REAL32 COnlyGrenadeInfo::GetExplosion_EffectiveSlope() const
{
	return m_rExplosion_EffectiveSlope;
}

REAL32 COnlyGrenadeInfo::GetDetected_Range() const
{
	return m_rDetected_Range;
}

REAL32 COnlyGrenadeInfo::GetThrowSpeed() const
{
	return m_rThrowSpeed;
}

REAL32 COnlyGrenadeInfo::GetExThrowSpeed()
{
	return m_rExThrowSpeed;
}

REAL32 COnlyGrenadeInfo::GetEffectiveTime() const
{
	return m_rEffectiveTime;
}

REAL32 COnlyGrenadeInfo::GetAttachLength() const
{
	return m_rAttachLength;
}

REAL32 COnlyGrenadeInfo::GetDamageTerm( void) const
{
	return m_rCheckDamageTerm;
}

INT32 COnlyGrenadeInfo::GetIncreaseCount()
{
	return m_nIncreaseCount;
}

// 왼쪽 클릭 공격시 공격 타이밍. 인자는 공격 횟수째 인덱스 (예: 첫번째 공격은 인덱스 0)
REAL32 COnlyGrenadeInfo::GetAttackTimeA( INT32 num) const
{
	REAL32 time = m_rAttackTimeA[ num];

	return ( (num < GRENADE_ATTACK_NUM_MAX) ? time : 0.f);
}

// 오른쪽 클릭 공격시 공격 타이밍.
REAL32 COnlyGrenadeInfo::GetAttackTimeB( INT32 num) const
{
	REAL32 time = m_rAttackTimeB[ num];

	return ( (num < GRENADE_ATTACK_NUM_MAX) ? time : 0.f);
}

bool COnlyGrenadeInfo::GetDual( void) const
{
	return m_bDual != 0;
}

bool COnlyGrenadeInfo::GetUseThrowDelay( void) const
{
	return m_bUseThrowDelay != 0;
}

// KEYWORD : DECOY_SOUND
void COnlyGrenadeInfo::GetDefaultDecoySound(i3::rc_wstring& out)
{
	out = m_strDefaultDecoySound;
}

UINT32 COnlyGrenadeInfo::GetBombtriggerItemID() const
{
	return m_nBombtriggerItemID;
}

bool COnlyGrenadeInfo::GetUseBombTrigger( void) const
{
	return m_bUseBombTrigger != 0;
}