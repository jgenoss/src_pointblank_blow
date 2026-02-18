#include "pch.h"
#include "GameP2PDef.h"

UINT16 GAMEINFO_NORMAL::getHP( void)
{
#if defined( MEM_ENCRYPTION)
	return GlobalFunc::DecryptValue( &_nCharaHP, ENC_SHIFT_CHARA_HP);
#else
	return _nCharaHP;
#endif
}

void GAMEINFO_NORMAL::setHP( UINT16 hp)
{
#if defined( MEM_ENCRYPTION)
	GlobalFunc::EncryptValue( &hp, sizeof( hp), ENC_SHIFT_CHARA_HP);
#endif

	_nCharaHP = hp;
}
