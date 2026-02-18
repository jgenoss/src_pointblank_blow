#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"
#include "StageBattle.h"
#include "GameCamera_FPS.h"

/*
void CGameCharaLocal::_ProcessExtension( REAL32 tm)
{
	
	UINT64 InputKeyStroke = getKeyStrokeInput();
	
	WeaponBase * pCurWeapon = WEAPON_getCurrent();
	I3ASSERT( pCurWeapon != NULL);

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	if( STATE_IsUpper(CHARA_UPPER_LOADMAGAZINE) || STATE_IsUpper(CHARA_UPPER_LOADBULLET)
		|| STATE_IsUpper(CHARA_UPPER_SWAPWEAPON) || STATE_IsUpper(CHARA_UPPER_EXTENSION_ACTIVATE) || STATE_IsUpper(CHARA_UPPER_EXTENSION_DEACTIVATE) )
		return;

	switch( pCurWeaponInfo->getExtensionType())
	{
		default :
			// Extension이 없는 무기 모두...
			if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				switch( pCurWeaponInfo->m_TypeZoom)
				{
					case WEAPON_ZOOM_SCOPE :	ACTION_ToggleZoom();		break;		// 스코프 기능 사용
				}
			}
			break;

		case WEAPON_EXT_SCOPE :
			// M4A1
			if( InputKeyStroke & GAME_KEY_ACTION_EXTENSION_ACTIVE )
			{
				ACTION_ToggleExtensionWeapon();		//	무기 Extension 활성화 토글
			}
			else if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				if( pCurWeapon->isExtensionActivate() )
					ACTION_ToggleZoom();				// 스코프 기능 사용
			}
			break;

		case WEAPON_EXT_SCOPE2 :
		case WEAPON_EXT_ZOOM :					// 견착
			// K2, K1
			if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				ACTION_ToggleZoom();				// 스코프 기능 사용
			}
			break;

		case WEAPON_EXT_STEADY_AIM :
			// Spectre
			if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				ACTION_ToggleZoom();				// 스코프 기능 사용
			}
			break;

			// Activate 단계 없이 사용하는 Extension 무기들
		case WEAPON_EXT_DUAL_MAG :
			// AK47
		case WEAPON_EXT_SILENCER :		// 사용과 동시에 Activate
			// MP5K	// MK23
			if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				ACTION_ToggleExtensionWeapon();
			}
			break;
		case WEAPON_EXT_DOT_SIGHT :
			if( InputKeyStroke & GAME_KEY_ACTION_SUBFUNC )
			{
				ACTION_ToggleZoom();				// 스코프 기능 사용
			}
			break;
	}
	
}


*/


