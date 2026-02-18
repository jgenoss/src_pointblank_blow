#include "pch.h"
#include "CharaAIIndexTable.h"

#include "i3Base/string/ext/generic_string_cat.h"

//#include "GameCharaAnimControlDefine.h"

CCharaAIIndexTable::CCharaAIIndexTable(void)
{
	i3mem::Fill( m_IndexTable, 0xFF, sizeof(m_IndexTable));
	i3mem::Fill( m_DualIndexTable, 0xFF, sizeof(m_DualIndexTable));
}

void CCharaAIIndexTable::Init( WeaponBase * pWeapon)
{
	char szAIName[256] = "";
	char * pszAnimSet = "";
	char * pszBody2Name = "";

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	//	독립될 수 없는 LSS와 같은 익스텐션 전용 무기는 주무기 자체에 애니메이션이 있기 때문에 스킵한다.
	if( pWeaponInfo->AttachedSubWeapon() == true)	
		return;

	//	AnimSet
	for( INT32 nAnimCnt = 0; nAnimCnt < CHARA_VIEW_MAX; nAnimCnt++)
	{
		switch( (CHARA_VIEW_TYPE) nAnimCnt)
		{
			case CHARA_VIEW_1PV:			pszAnimSet = "1PV:";		break;
			case CHARA_VIEW_3PV:			pszAnimSet = "3PV:";		break;
		//	default:		continue;
		}

		//	Body2
		for( INT32 nBodyState = 0; nBodyState < CHARA_BODYUPPER_MAXCOUNT; nBodyState++)
		{
			switch( (CHARA_ACTION_BODYUPPER) nBodyState)
			{
			case CHARA_BODYUPPER_ATTACKIDLE:		pszBody2Name = "AttackIdle";	break;
			case CHARA_BODYUPPER_ZOOM:
				{
					//	Zoom 가능한 무기만 찾도록 한다.
					if( pWeaponInfo->isAnimZoomType() ) 
						pszBody2Name = "Zoom";
					else
						continue;
				}
				break;
			case CHARA_BODYUPPER_GRENADE :
				{
					if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_GLAUNCHER ||
						pWeaponInfo->getExtensionType(1) == WEAPON::EXT_GLAUNCHER)
					{
						pszBody2Name = "Grenade";		break;
					}
					else
						continue;
				}
				break;
			case CHARA_BODYUPPER_SHOTGUN:
				{
					if( pWeaponInfo->getExtensionType(0) == WEAPON::EXT_SHOTGUN_LAUNCHER ||
						pWeaponInfo->getExtensionType(1) == WEAPON::EXT_SHOTGUN_LAUNCHER)
					{
						pszBody2Name = "ShotGun";		break;
					}
					else
						continue;
				}
				break;
			default:		continue;
			}

			i3::safe_string_copy( szAIName, "CharaAI:", 256 );
			i3::generic_string_cat( szAIName, pszAnimSet);
			i3::generic_string_cat( szAIName, pszBody2Name);

			//	AI Context
			i3AIContext * pAICtx = pWeapon->FindAI( szAIName);
			if( pAICtx == nullptr)		continue;		//	Zoom이 없는 무기도 있으므로

			//	AI Id
			for( INT32 id = 0; id < ID_UPPER_AI_MAX; id++)
			{	
				INT8 nRet = (INT8) pAICtx->FindAIStateByName( g_szNameUpperAI[ id] );

				setIndex( (CHARA_VIEW_TYPE) nAnimCnt, (CHARA_ACTION_BODYUPPER)nBodyState, (ID_UPPER_AI)id, nRet);
			}
		}
	}

	// Dual Weapon인 경우에 대한 처리...
	if( pWeaponInfo->GetLeftHandWeaponItemID() > 0)
	{
		_InitForDualWeapon( pWeapon, "AttackIdle");
		_InitForDualWeapon( pWeapon, "AttackIdle_Left");
	}
}

void CCharaAIIndexTable::_InitForDualWeapon( WeaponBase * pWeapon, const char * pszAIName)
{
	char szAIName[256];
	char * pszAnimSet = nullptr;
	INT32 nAnimCnt;

	//	AnimSet
	for( nAnimCnt = 0; nAnimCnt < CHARA_VIEW_MAX; nAnimCnt++)
	{
		switch( (CHARA_VIEW_TYPE) nAnimCnt)
		{
			case CHARA_VIEW_1PV:			pszAnimSet = "1PV:";		break;
			case CHARA_VIEW_3PV:			pszAnimSet = "3PV:";		break;
		//	default:		continue;
		}

		i3::safe_string_copy( szAIName, "CharaAI:", 256 );
		i3::generic_string_cat( szAIName, pszAnimSet);
		i3::generic_string_cat( szAIName, pszAIName);

		//	AI Context
		i3AIContext * pAICtx = pWeapon->FindAI( szAIName);
		if( pAICtx == nullptr)		continue;		//	Zoom이 없는 무기도 있으므로

		//	AI Id
		for( INT32 id = 0; id < ID_UPPER_DUAL_AI_MAX; id++)
		{	
			INT8 nRet = (INT8) pAICtx->FindAIStateByName( g_szNameUpperDualAI[ id] );

			setDualIndex( (CHARA_VIEW_TYPE) nAnimCnt, (ID_UPPER_DUAL_AI) id, nRet);
		}
	}
}
