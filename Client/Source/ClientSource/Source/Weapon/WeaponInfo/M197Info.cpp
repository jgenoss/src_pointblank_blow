#include "pch.h"
#include "M197Info.h"

I3_CLASS_INSTANCE( CM197Info);//, CGunInfo);

CM197Info::CM197Info()
{
	m_rContinueFireTime = 0.f;
	m_rChargeSpeedRatio = 0.f;
	m_rFirstDelayTime = 0.f;
	m_rSlope_DelayPerFire = 0.f;
	m_rFirePenaltyTime = 0.f;
	
	m_rLimitRho = 0.f;
	m_rLimitTheta = 0.f;
}

CM197Info::~CM197Info()
{
}

void CM197Info::ReadWeaponInfo( i3RegKey * pKey)
{
	CGunInfo::ReadWeaponInfo(pKey);

	FIND_REG_DATA( pKey, LS_STR("M197_ContinueFireTime"),		&m_rContinueFireTime);
	FIND_REG_DATA( pKey, LS_STR("M197_ChargeSpeedRatio"),		&m_rChargeSpeedRatio);
	FIND_REG_DATA( pKey, LS_STR("M197_FirstDelayTime"),			&m_rFirstDelayTime);
	FIND_REG_DATA( pKey, LS_STR("M197_Slope_DelayPerFire"),		&m_rSlope_DelayPerFire);
	FIND_REG_DATA( pKey, "M197_FirePenaltyTime",		&m_rFirePenaltyTime);
	FIND_REG_DATA( pKey, "M197_LimitRho",				&m_rLimitRho);
	FIND_REG_DATA( pKey, "M197_LimitTheta",				&m_rLimitTheta);
}

/*virtual*/ void CM197Info::OnEncriptInfo( UINT8 u8Key)
{
	CGunInfo::OnEncriptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue( &m_rContinueFireTime, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rChargeSpeedRatio, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rFirstDelayTime, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rSlope_DelayPerFire, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rFirePenaltyTime, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rLimitRho, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rLimitTheta, m_u8EncryptKey[VALUETYPE_REAL]);
#endif
}

REAL32 CM197Info::GetContinueFireTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rContinueFireTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rContinueFireTime;
#endif
}
REAL32 CM197Info::GetChargeSpeedRatio() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rChargeSpeedRatio, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rChargeSpeedRatio;
#endif
}
REAL32 CM197Info::GetFirstDelayTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rFirstDelayTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rFirstDelayTime;
#endif
}

REAL32 CM197Info::GetSlope_DelayPerFire() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rSlope_DelayPerFire, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rSlope_DelayPerFire;
#endif
}

REAL32 CM197Info::GetFirePenaltyTime() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rFirePenaltyTime, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rFirePenaltyTime;
#endif
}

REAL32 CM197Info::GetLimitRho() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rLimitRho, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rLimitRho;
#endif
}

REAL32 CM197Info::GetLimitTheta() const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rLimitTheta, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rLimitTheta;
#endif
}
