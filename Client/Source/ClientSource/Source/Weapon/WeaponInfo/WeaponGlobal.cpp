#include "pch.h"
#include "WeaponGlobal.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE( CWeaponGlobal);//, i3NamedElement);
I3_CLASS_INSTANCE( CUITemplateInfo_Weapon);

CWeaponGlobal::CWeaponGlobal( void)
{
	m_rTracerBulletLength		= 0.f;
	m_rTracerBulletThickness	= 0.f;
	m_rTracerBulletSpeed		= 0.f;
	m_iTracerBulletRate			= 0;	//	총알 발사 확률 ( Max: 100%)

	m_rTracerHandGrenadeThickness = 1.f;
	m_rTracerShellGrenadeThickness = 1.f;
	m_rTracerArrowThickness = 1.f;
	m_rTracerSmokeThickness = 1.f;	
	m_rTracerFlashThickness = 1.f;
	m_rTracerHandGrenadePieceLifeTime = 1.f;	
	m_rTracerShellGrenadePieceLifeTime = 1.f;	
	m_rTracerArrowPieceLifeTime = 1.f;	
	m_rTracerSmokePieceLifeTime = 1.f;	
	m_rTracerFlashPieceLifeTime = 1.f;	
	m_rTracerHandGrenadePieceCreateTime = 1.f;
	m_rTracerShellGrenadePieceCreateTime = 1.f;
	m_rTracerArrowPieceCreateTime = 1.f;
	m_rTracerSmokePieceCreateTime = 1.f;	
	m_rTracerFlashPieceCreateTime = 1.f;	
	i3Color::Set(&m_TracerHandGrenadeColor, (UINT8) 255, 255, 255, 255);
	i3Color::Set(&m_TracerShellGrenadeColor, (UINT8) 255, 255, 255, 255);
	i3Color::Set(&m_TracerArrowColor, (UINT8) 255, 255, 255, 255);
	i3Color::Set(&m_TracerSmokeColor, (UINT8) 255, 255, 255, 255);
	i3Color::Set(&m_TracerFlashColor, (UINT8) 255, 255, 255, 255);	
	
	i3Color::Set( &m_DamageEffectColor, (UINT8) 0, 0, 0, 0);
	m_DamageEffectBlurEnd = 0.f;
	m_DamageEffectBlurStart = 0.f;
}

CWeaponGlobal::~CWeaponGlobal( void)
{
}

void CWeaponGlobal::ReadInfo( i3RegKey * pKey)
{
	I3ASSERT( pKey != nullptr);

	char szName[128] = {0,};
	INT32 len = i3::generic_string_size( pKey->GetName());

	if( len > 0)
	{
		i3::string_ncopy_nullpad( szName, pKey->GetName(), len);

		GlobalFunc::EncryptValue( szName, len, ENC_SHIFT_WEAPONINFO_RESNAME);

		SetName( szName);
	}

	//요거 중요한 넘입니다. 지우지 마세요( 케릭터 타입을 문자열이아닌 인덱스로 찾고자하는경우 쓰입니다.
	//내부의 데이터가 정렬이되는경우 Child의 인덱스로 찾기를 시도하는경우 오류가 생길수 있으므로 내부데이터를 하나둡니다.
	FIND_REG_DATA( pKey, "TracerBulletLength", &m_rTracerBulletLength);
	FIND_REG_DATA( pKey, "TracerBulletThickness", &m_rTracerBulletThickness);
	FIND_REG_DATA( pKey, "TracerBulletSpeed", &m_rTracerBulletSpeed);	
	FIND_REG_DATA( pKey, "TracerBulletRate", &m_iTracerBulletRate);	
	FIND_REG_DATA( pKey, "TracerBulletEffect", m_strTracerBulletEffectFile );	

	FIND_REG_DATA( pKey, "TracerHandGrenadeThickness", &m_rTracerHandGrenadeThickness);
	FIND_REG_DATA( pKey, "TracerShellGrenadeThickness", &m_rTracerShellGrenadeThickness);
	FIND_REG_DATA( pKey, "TracerArrowThickness", &m_rTracerArrowThickness);
	FIND_REG_DATA( pKey, "TracerSmokeThickness", &m_rTracerSmokeThickness);
	FIND_REG_DATA( pKey, "TracerFlashThickness", &m_rTracerFlashThickness);

	FIND_REG_DATA( pKey, "TracerHandGrenadePieceLifeTime", &m_rTracerHandGrenadePieceLifeTime);
	FIND_REG_DATA( pKey, "TracerShellGrenadePieceLifeTime", &m_rTracerShellGrenadePieceLifeTime);
	FIND_REG_DATA( pKey, "TracerArrowPieceLifeTime", &m_rTracerArrowPieceLifeTime);
	FIND_REG_DATA( pKey, "TracerSmokePieceLifeTime", &m_rTracerSmokePieceLifeTime);
	FIND_REG_DATA( pKey, "TracerFlashPieceLifeTime", &m_rTracerFlashPieceLifeTime);

	FIND_REG_DATA( pKey, "TracerHandGrenadePieceCreateTime", &m_rTracerHandGrenadePieceCreateTime);
	FIND_REG_DATA( pKey, "TracerShellGrenadePieceCreateTime", &m_rTracerShellGrenadePieceCreateTime);
	FIND_REG_DATA( pKey, "TracerArrowPieceCreateTime", &m_rTracerArrowPieceCreateTime);
	FIND_REG_DATA( pKey, "TracerSmokePieceCreateTime", &m_rTracerSmokePieceCreateTime);
	FIND_REG_DATA( pKey, "TracerFlashPieceCreateTime", &m_rTracerFlashPieceCreateTime);

	FIND_REG_DATA( pKey, "TracerHandGrenadeColor", &m_TracerHandGrenadeColor);
	FIND_REG_DATA( pKey, "TracerShellGrenadeColor", &m_TracerShellGrenadeColor);
	FIND_REG_DATA( pKey, "TracerArrowColor", &m_TracerArrowColor);
	FIND_REG_DATA( pKey, "TracerSmokeColor", &m_TracerSmokeColor);
	FIND_REG_DATA( pKey, "TracerFlashColor", &m_TracerFlashColor);

	FIND_REG_DATA( pKey, "TracerHandGrenadeEffectFile", m_strTracerHandGrenadeEffectFile );
	FIND_REG_DATA( pKey, "TracerShellGrenadeEffectFile", m_strTracerShellGrenadeEffectFile );
	FIND_REG_DATA( pKey, "TracerArrowEffectFile", m_strTracerArrowEffectFile );
	FIND_REG_DATA( pKey, "TracerSmokeEffectFile", m_strTracerSmokeEffectFile );
	FIND_REG_DATA( pKey, "TracerFlashEffectFile", m_strTracerFlashEffectFile );

	FIND_REG_DATA( pKey, "DamageEffectColor", &m_DamageEffectColor);	
	FIND_REG_DATA( pKey, "DamageEffectBlurStart", &m_DamageEffectBlurStart);	
	FIND_REG_DATA( pKey, "DamageEffectBlurEnd", &m_DamageEffectBlurEnd);
}

CUITemplateInfo_Weapon::CUITemplateInfo_Weapon()
{

}

CUITemplateInfo_Weapon::~CUITemplateInfo_Weapon()
{

}

void CUITemplateInfo_Weapon::ReadInfo(i3RegKey* rk)
{
	FIND_REG_DATA(rk, "Name", m_name);
}

const i3::rc_wstring& CUITemplateInfo_Weapon::GetName() const
{
	return m_name;
}