#include "pch.h"
#include "MainWeapon_Knife.h"
#include "MainWeapon_KnifeGender.h"
#include "WeaponFunction.h"

#include "i3Base/string/ext/utf16_to_mb.h"

i3::string s_strAIName[KNIFE_AI_COUNT] =
{
	"LoadMag3PV",
	"LoadMag1PV",
	"Idle",
	"Fire",
	"SecondaryFire"
};

i3::string s_strGenderType[KNIFE_GENDER_COUNT] =
{
	"Male",
	"Female"
};

MainWeapon_KnifeGender::MainWeapon_KnifeGender(WeaponBase* p) : MainWeapon_Knife(p)
{
	i3mem::Fill(m_i8AIIndex, -1, sizeof(m_i8AIIndex) );
}

void	MainWeapon_KnifeGender::OnBindResource()
{
	MainWeapon_Knife::OnBindResource();

	if( m_pWeaponBase->GetAIContext() == nullptr)
		return;

	// Animation 설정
	char szAnimTemp[ MAX_STRING_COUNT];
	for( INT32 type = 0 ; type < KNIFE_GENDER_COUNT ; ++type )
	{
		for( INT32 idx = 0 ; idx < KNIFE_AI_COUNT ; ++idx )
		{
			if( type != 0 )
				sprintf_s( szAnimTemp, "%s_%s", s_strAIName[idx].c_str(), s_strGenderType[type].c_str());
			else
				sprintf_s( szAnimTemp, "%s", s_strAIName[idx].c_str());

			m_i8AIIndex[type][idx] = static_cast<INT8>( m_pWeaponBase->GetAIContext()->FindAIStateByName( szAnimTemp ) );
		}	
	}
}

void MainWeapon_KnifeGender::SetOwner( CGameCharaBase * pOwner)
{
	_BindAnimationIndex(pOwner);
	MainWeapon_Knife::SetOwner(pOwner);
}

void MainWeapon_KnifeGender::_BindAnimationIndex(CGameCharaBase * pOwner)
{
	if( m_pWeaponBase->getOwner() == nullptr )
		return;

	INT32 type = m_pWeaponBase->getOwner()->isFemale() ? 1 : 0 ;	// 남 : 0 , 여 : 1;
	
	m_pWeaponBase->setAIIndex( WEAPON::AI_LOADMAGAZINE_3PV,			m_i8AIIndex[type][0]	);
	m_pWeaponBase->setAIIndex( WEAPON::AI_LOADMAGAZINE_1PV,			m_i8AIIndex[type][1]	);
	m_pWeaponBase->setAIIndex( WEAPON::AI_IDLE,						m_i8AIIndex[type][2]	);
	m_pWeaponBase->setAIIndex( WEAPON::AI_FIRE,						m_i8AIIndex[type][3]	);
	m_pWeaponBase->setAIIndex( WEAPON::AI_SECFIRE,					m_i8AIIndex[type][4]	);
}