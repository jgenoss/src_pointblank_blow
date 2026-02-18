#include "pch.h"
#include "GrenadeInfo.h"
#include "OnlyGrenadeInfo.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/algorithm/to_upper.h"

I3_CLASS_INSTANCE( CGrenadeInfo);//, CWeaponInfo);

CGrenadeInfo::CGrenadeInfo()
{
	m_pOnlyGrenadeInfo = COnlyGrenadeInfo::new_object();
}

/*virtual*/ CGrenadeInfo::~CGrenadeInfo()
{
	I3_SAFE_RELEASE(m_pOnlyGrenadeInfo);
}


void CGrenadeInfo::FillBulletCountString(i3::stack_wstring& out_wstrBulletCount)
{
	if(WEAPON::FIRE_THROWING == GetTypeFire()) {
		//i3::safe_string_copy(strBulletCount, "1/1");
		i3::sprintf(out_wstrBulletCount, L"%d/%d", GetLoadBullet(), GetMaxBullet());
	}
	else {
		I3ASSERT_0;
	}
}

void CGrenadeInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	CWeaponInfo::ReadWeaponInfo(pKey);

	I3ASSERT(m_pOnlyGrenadeInfo);

	m_pOnlyGrenadeInfo->ReadWeaponInfo(pKey);
}

/*virtual*/ void CGrenadeInfo::OnEncriptInfo( UINT8 u8Key)
{
	CWeaponInfo::OnEncriptInfo( u8Key);

	I3ASSERT( m_pOnlyGrenadeInfo);

	m_pOnlyGrenadeInfo->OnEncriptInfo( u8Key);
}

/*virtual*/INT32 CGrenadeInfo::GetFireBulletCount()	 const		
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetFireBulletCount(); 
}

/*virtual*/INT32 CGrenadeInfo::GetFireDecBulletCount() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetFireDecBulletCount();	
}

/*virtual*/INT32 CGrenadeInfo::GetLoadBullet() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetLoadBullet(); 
}

/*virtual*/INT32 CGrenadeInfo::GetReloadBulletCount() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetReloadBulletCount(); 
}

/*virtual*/INT32 CGrenadeInfo::GetMaxBullet() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetMaxBullet(); 
}

/*virtual*/REAL32 CGrenadeInfo::GetExplosion_Time() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetExplosion_Time(); 
}	

/*virtual*/REAL32 CGrenadeInfo::GetExplosion_Range() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetExplosion_Range(); 
}

/*virtual*/REAL32 CGrenadeInfo::GetExplosion_NearRange() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetExplosion_NearRange(); 
}

/*virtual*/REAL32 CGrenadeInfo::GetExplosion_EffectiveSlope() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetExplosion_EffectiveSlope(); 
}

/*virtual*/REAL32 CGrenadeInfo::GetDetected_Range() const
{
	I3ASSERT(m_pOnlyGrenadeInfo);
	return m_pOnlyGrenadeInfo->GetDetected_Range();
}

/*virtual*/REAL32 CGrenadeInfo::GetThrowSpeed() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetThrowSpeed();	
}

/*virtual*/REAL32 CGrenadeInfo::GetExThrowSpeed()
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetExThrowSpeed();	
}

/*virtual*/REAL32 CGrenadeInfo::GetEffectiveTime() const
{ 
	I3ASSERT(m_pOnlyGrenadeInfo); 
	return m_pOnlyGrenadeInfo->GetEffectiveTime(); 
}

/*virtual*/REAL32 CGrenadeInfo::GetAttachLength() const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetAttachLength();
}

/*virtual*/REAL32 CGrenadeInfo::GetDamageTerm() const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetDamageTerm();
}

/*virtual*/INT32 CGrenadeInfo::GetIncreaseCount()
{
	I3ASSERT(m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetIncreaseCount();
}

/*virtual*/REAL32 CGrenadeInfo::GetAttackTimeA( INT32 num) const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetAttackTimeA(num);
}

/*virtual*/REAL32 CGrenadeInfo::GetAttackTimeB( INT32 num) const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetAttackTimeB(num);
}

/*virtual*/void CGrenadeInfo::GetDefaultDecoySound(i3::rc_wstring& out)
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	m_pOnlyGrenadeInfo->GetDefaultDecoySound(out);
}

void CGrenadeInfo::_SetTracerType( i3RegKey* pKey )
{
	i3::rc_wstring	wstrType;

	FIND_REG_DATA( pKey, "TracerType", wstrType );	// TRACE_TYPE

	if( i3::generic_is_iequal(wstrType, "HAND_GRENADE")  )
		m_TraceType		= TRACER_TYPE_HAND_GRENADE;
	else if( i3::generic_is_iequal(wstrType, "FLASH")  )
		m_TraceType		= TRACER_TYPE_FLASH;
	else if( i3::generic_is_iequal(wstrType, "SMOKE")  )
		m_TraceType		= TRACER_TYPE_SMOKE;
}

bool CGrenadeInfo::GetDual( void) const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetDual();
}

bool CGrenadeInfo::GetUseThrowDelay( void) const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetUseThrowDelay();
}

UINT32 CGrenadeInfo::GetBombtriggerItemID() const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetBombtriggerItemID();
}
 
bool CGrenadeInfo::GetUseBombTrigger( void) const
{
	I3ASSERT( m_pOnlyGrenadeInfo != nullptr);
	return m_pOnlyGrenadeInfo->GetUseBombTrigger();
}