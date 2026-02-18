#include "pch.h"
#include "GrenadeGunInfo.h"
#include "OnlyGrenadeInfo.h"

I3_CLASS_INSTANCE( CGrenadeGunInfo);//, CGrenadeInfo);

CGrenadeGunInfo::CGrenadeGunInfo()
{
	m_pOnlyGreandeInfo = COnlyGrenadeInfo::new_object();

	m_Homing = 0.0f;

	//OnEncriptInfo(0);
}

/*virtual*/ CGrenadeGunInfo::~CGrenadeGunInfo()
{
	I3_SAFE_RELEASE(m_pOnlyGreandeInfo);
}

void CGrenadeGunInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	CGrenadeInfo::ReadWeaponInfo(pKey);

	I3ASSERT(m_pOnlyGreandeInfo);

	m_pOnlyGreandeInfo->ReadWeaponInfo(pKey);

	FIND_REG_DATA( pKey, LS_STR("Homing"),		&m_Homing);				// 유도정도
//	FIND_REG_DATA( pKey, "ShellType",	m_wstrShellType);
}

/*virtual*/ void CGrenadeGunInfo::OnEncriptInfo( UINT8 u8Key)
{
	CGrenadeInfo::OnEncriptInfo( u8Key);

	m_pOnlyGreandeInfo->OnEncriptInfo( u8Key);

#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue( &m_Homing, m_u8EncryptKey[VALUETYPE_REAL]);
#endif

}

REAL32 CGrenadeGunInfo::getHoming(void) const
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_Homing, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_Homing;
#endif
}

/*virtual*/INT32 CGrenadeGunInfo::GetFireBulletCount() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetFireBulletCount(); 
}

/*virtual*/INT32 CGrenadeGunInfo::GetFireDecBulletCount() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetFireDecBulletCount();	
}

/*virtual*/INT32 CGrenadeGunInfo::GetLoadBullet() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetLoadBullet(); 
}

/*virtual*/INT32 CGrenadeGunInfo::GetReloadBulletCount() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetReloadBulletCount(); 
}

/*virtual*/INT32 CGrenadeGunInfo::GetMaxBullet() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetMaxBullet(); 
}

/*virtual*/REAL32 CGrenadeGunInfo::GetExplosion_Time() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_Time(); 
}	

/*virtual*/REAL32 CGrenadeGunInfo::GetExplosion_Range() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_Range(); 
}

/*virtual*/REAL32 CGrenadeGunInfo::GetExplosion_EffectiveSlope() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetExplosion_EffectiveSlope(); 
}

/*virtual*/REAL32 CGrenadeGunInfo::GetThrowSpeed() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetThrowSpeed();	
}

/*virtual*/REAL32 CGrenadeGunInfo::GetEffectiveTime() const
{ 
	I3ASSERT(m_pOnlyGreandeInfo); 
	return m_pOnlyGreandeInfo->GetEffectiveTime(); 
}

/*virtual*/REAL32 CGrenadeGunInfo::GetAttachLength() const
{
	I3ASSERT( m_pOnlyGreandeInfo != nullptr);
	return m_pOnlyGreandeInfo->GetAttachLength();
}

/*virtual*/REAL32 CGrenadeGunInfo::GetDamageTerm() const
{
	I3ASSERT( m_pOnlyGreandeInfo != nullptr );
	return m_pOnlyGreandeInfo->GetDamageTerm();
}
