#include "pch.h"
#include "ScopeGunInfo.h"

I3_CLASS_INSTANCE( CScopeGunInfo);//, CGunInfo);

CScopeGunInfo::CScopeGunInfo()
{
	m_nMaxScopeLevel = 0;								// 최대 스코프 단계

	m_rScopeFOV		= 0.0f;									// Scope시의 FOV값
	m_rScopeFOV2	= 0.0f;

	m_fThrowSpeed	= 0.f;

	m_nScopeType	= UINT8(-1);

	//OnEncriptInfo(0);
}

/*virtual*/ CScopeGunInfo::~CScopeGunInfo()
{
}

/*virtual*/ void CScopeGunInfo::_ReadExtensionKey( i3RegKey * pKey, INT32 idx)
{
	INT32 nTemp = 0;
	CGunInfo::_ReadExtensionKey( pKey, idx);

	FIND_REG_DATA( pKey, "ScopeType",				&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nScopeType = (UINT8) nTemp;

	nTemp = 0;
	FIND_REG_DATA( pKey, "MaxScopeLevel",			&nTemp);
	I3_BOUNDCHK( nTemp, 255);
	m_nMaxScopeLevel = (UINT8) nTemp;

	FIND_REG_DATA( pKey, "FovOnScope",				&m_rScopeFOV);					// 1단 Scope시의 FOV값
	FIND_REG_DATA( pKey, "FovOnScope2",				&m_rScopeFOV2);					// 2단 Scope시의 FOV값
}

void CScopeGunInfo::ReadWeaponInfo( i3RegKey * pKey)
{
	CGunInfo::ReadWeaponInfo(pKey);

	FIND_REG_DATA( pKey, "Throw_Speed",				&m_fThrowSpeed);				// RPG-7에서 사용됩니다.
}

/*virtual*/ void CScopeGunInfo::OnEncriptInfo( UINT8 u8Key)
{
	CGunInfo::OnEncriptInfo( u8Key);
	
#if defined( MEM_ENCRYPTION_LV1)
	GlobalFunc::EncryptValue( &m_nMaxScopeLevel, m_u8EncryptKey[VALUETYPE_INT]);
	GlobalFunc::EncryptValue( &m_rScopeFOV, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_rScopeFOV2, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_fThrowSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
	GlobalFunc::EncryptValue( &m_nScopeType, m_u8EncryptKey[VALUETYPE_INT]);
#endif
}

/*virtual*/ INT32 CScopeGunInfo::GetMaxScopeLevel()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_nMaxScopeLevel, m_u8EncryptKey[VALUETYPE_INT]);
#else
	return m_nMaxScopeLevel;
#endif
}

/*virtual*/ REAL32 CScopeGunInfo::GetScopeFOV()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rScopeFOV, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rScopeFOV;
#endif
}
/*virtual*/ REAL32 CScopeGunInfo::GetScopeFOV2()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_rScopeFOV2, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_rScopeFOV2;
#endif
}

/*virtual*/ REAL32 CScopeGunInfo::GetThrowSpeed()
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_fThrowSpeed, m_u8EncryptKey[VALUETYPE_REAL]);
#else
	return m_fThrowSpeed;
#endif
}

/*virtual*/ INT32 CScopeGunInfo::GetScopeType( void)
{
#if defined( MEM_ENCRYPTION_LV1)
	return GlobalFunc::DecryptValue( &m_nScopeType, m_u8EncryptKey[VALUETYPE_INT]);
#else
	return m_nScopeType;
#endif
}
