#include "pch.h"
#include "UtilBattle.h"

#include "BattleHUD_Scope.h"
#include "GameCharaBoneContext.h"
#include "ObserverBattle.h"

#include "HackDefense.h"

#include "Camera/GameCamera_OnObject.h"

#include "GameMap.h"

#include "UI/UIUtil.h"
#include "CrosshairMgr.h"
#include "Crosshair.h"

#include "./StageBattle/AIModeContext.h"
#include "./StageBattle/DinoModeContext.h"
#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "GameCharaCameraContext.h"


#if ENABLE_UNIT_TEST
#include "UnitTest/GameUnitTest.h"
#endif

#define OBSERVER_CAMERA_INTERPOLATE_TIME 0.1f

static CGameCharaBase * getCurrentChara( void)
{
	return g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
}

static void sAfterLoadChara_ForAI( void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind)
{
	GameEventSender::i()->SetEventOnce(EVENT_RESPAWN_BATTLE_FOR_AI, &idx );

#if ENABLE_UNIT_TEST
	if( !g_pConfig->m_bNetworking || UnitTest::get_unit_test() != nullptr )
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE_FOR_AI, idx);
	}
#endif
}

static void sAfterLoadCharaForDinoMode( void * pAvatar, INT32 idx,AI_BIND_DATA * pAIBind)
{
	BattleServerContext::i()->sAfterLoadChara( pAvatar, idx);

	if( pAvatar != nullptr && UserContext::i()->IsPlayingSlot(idx))
	{
		((CGameCharaBase*)pAvatar)->SetVisible(true);
	}
}



namespace
{

	bool	g_bCurrCameraIsFPS = true;
	VEC3D	g_v3TopViewCameraPos;				// 전역인 경우 0초기화를 기대할수 있음..
}

namespace util_battle
{
	
	bool is_live_character(INT32 idx)
	{
		// SlotState가 SLOT_STATE_BATTLE 일것
		// 이번 Round에 Play하고 있을 것 IsPlayingSlot()
		// g_pCharaManager->m_pChara[idx]가 NULL이 아닐것
		// hp가 0보다 많을 것

		const SLOT_INFO* pInfo = BattleSlotContext::i()->getSlotInfo(idx);

		if (BattleSlotContext::i()->IsGMObserverForClient(idx)) return false; //@eunil. PBBUG-14920 GM옵저버도 죽은거랑 같이 취급

		if( SLOT_STATE_BATTLE != pInfo->_State) return false;
		if( !UserContext::i()->IsPlayingSlot(idx)) return false;
		if( UserContext::i()->Death[idx])	return false;

		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
		if( pChara != nullptr && pChara->isCharaStateMask( CHARA_STATE_DEATH))
			return false;
		else if( pChara == nullptr)
			return false;

		return true;
	}
	
	bool is_current_toggle_camera_fps()
	{
		return g_bCurrCameraIsFPS;
	}
	
	void toggle_3p_camera()
	{
		// 미션모드가 아니라도 자신의 캐릭터에 한해 3인칭 카메라로 변경
		if( i3::same_of<CGameCamera_Observer*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER2);
			g_bCurrCameraIsFPS = false;
		}
		else if( i3::same_of<CGameCamera_Observer2*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
			g_bCurrCameraIsFPS = true;
		}
		else
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
			g_bCurrCameraIsFPS = false;
		}

		if( g_pCamera->getCurTarget() )
		{
			CHUD_Scope::instance()->SetEnableSinperScope(false);
			g_pCamera->getCurTarget()->Cmd_Observer_Zoom();

			CHARA_VIEW_TYPE type = CHARA_VIEW_3PV;
			if( g_bCurrCameraIsFPS )
			{
				type = CHARA_VIEW_1PV;
			}

			g_pCamera->getCurTarget()->Cmd_ChangeView( type);
		}
	}
	
	void GetRotation_QuaternionToRadian(VEC3D& out, const MATRIX& m)
	{
		QUATERNION q;
		i3Matrix::GetRotateQuat(&q, &m);
		REAL32 sqw = q.w * q.w;
		REAL32 sqx = q.x * q.x;
		REAL32 sqy = q.y * q.y;
		REAL32 sqz = q.z * q.z;

		out.x = asinf(2.0f * (q.w * q.x - q.y * q.z));	// pitch
		out.y = atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw));	// yaw
		out.z = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw));	// roll
	}

	void toggle_flying_camera_mode(void)
	{
		CGameCharaBase * pPlayer = g_pCamera->getCurTarget();
		if (pPlayer == 0) return;

		if( i3::same_of<CGameCamera_FLY*>(g_pCamera))
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
		else
		{
			MATRIX* camMatrix = g_pCamera->getCamMatrix();
			CGameCharaBase* chara = g_pCamera->getCurTarget();

			// 위치 : 캐릭터 뒤.
			if (i3::same_of<CGameCamera_FPS*>(g_pCamera))
			{
				VEC3D basePos;
				basePos.x = 0.0f; basePos.y = 1.5f; basePos.z = 2.0f; 
				i3Vector::AddY(&basePos, -chara->getCameraContext()->getIKHeightForCam());

				VEC3D pos;
				i3Vector::TransformCoord(&pos, &basePos, camMatrix);
				i3Matrix::SetPos(camMatrix, &pos);
			}

			// FLY 카메라로 변경.
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FLY);

			CGameCamera_FLY* flyCam = static_cast<CGameCamera_FLY*>(g_pCamera);
			if (flyCam)
			{
				// 회전
				VEC3D v;
				GetRotation_QuaternionToRadian(v, *camMatrix);

				if (i3::same_of<CGameCamera_FPS*>(g_pCamera) == false)
				{
					VEC3D* mouseDelta = flyCam->GetMouseDelta();
					i3Vector::Zero(mouseDelta);

					const REAL32 sensitivity = 2.0f;
					mouseDelta->y = I3_RAD2DEG(v.x) * sensitivity;
				}

				i3Matrix::Copy(flyCam->getCamMatrix(), camMatrix);

				flyCam->SetBaseRotationY(v.y);
				flyCam->SetPreRotateY(0.0f);
				flyCam->SetRotateY(0.0f);
			}
		}

		CHUD_Scope::instance()->SetEnableSinperScope(false);
		pPlayer->Cmd_Observer_Zoom();
	}

	void toggle_normal_observe_mode(void)
	{
		hd::check_observer::i()->check();

		UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();

		BOOL bGMObserver = UserInfoContext::i()->IsGM_Observer();

		//@eunil. 3인칭 시점 제한의 경우 3인칭 회전은 사용할 수 없다. GM은 제외한다
		BOOL bObserverCamera1PVOnly = bGMObserver ? false : TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
		BOOL bObserverCameraChase = bGMObserver ? true : ( !bObserverCamera1PVOnly && TESTBIT(LockObserver, LOCK_OBSERVER_CHASE) );
		BOOL bObserverCameraFree = bGMObserver ? true : TESTBIT( LockObserver, LOCK_OBSERVER_FREE);

		if( i3::same_of<CGameCamera_Observer*>(g_pCamera))
		{
			if	    (bObserverCamera1PVOnly)		CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);
			else if ( bObserverCameraChase )		CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER2);
			else
			{
				if( bObserverCameraFree )	
					toggle_flying_camera_mode();
				else						
					CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
			}
		}
		else if( i3::same_of<CGameCamera_Observer2*>(g_pCamera))
		{
			if( bObserverCameraFree )	
				toggle_flying_camera_mode();
			else						
				CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
		}
		else if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
		{
			if( !bObserverCamera1PVOnly ) 
				CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
			else if (bObserverCameraFree)
				toggle_flying_camera_mode();
			else
				return;
		}
		else if( i3::same_of<CGameCamera_FLY*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);
		}
		else
			return;

		if( g_pCamera->getCurTarget() )
		{
			CHUD_Scope::instance()->SetEnableSinperScope(false);
			g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
		}
				
	}

	void toggle_GM_observe_mode(void)
	{
		if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
		}
		else if( i3::same_of<CGameCamera_Observer*>(g_pCamera))
		{
			VEC3D v3Temp0;
			VEC3D v3Temp1;
			i3Vector::Copy(&v3Temp0, i3Matrix::GetAt(g_pCamera->getCamMatrix()) );
			v3Temp0.y = 0;
			i3Vector::Normalize(&v3Temp0, &v3Temp0);

			i3Vector::Set(&v3Temp1, 0.f, 0.f, -1.f);
			//REAL32 rAngle0 = i3Math::acos(i3Vector::Dot(&v3Temp1, &v3Temp0));
			if(g_pCamera->getCurTarget() == nullptr) {
				return;
			}

			REAL32 rAngle0 = g_pCamera->getCurTarget()->getBoneContext()->getTheta();

			i3Vector::Copy(&v3Temp0, i3Matrix::GetAt(g_pCamera->getCamMatrix()) );
			v3Temp0.x = 0;
			i3Vector::Normalize(&v3Temp0, &v3Temp0);

			//REAL32 rAngle1 = g_pCamera->getCurTarget()->getBoneContext()->getRho();
			//REAL32 rAngle1 = i3Math::acos(i3Vector::Dot(&v3Temp1, &v3Temp0));

			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FLY);

			i3Vector::Set(&v3Temp0, I3_RAD2DEG(rAngle0)*2.f, 0.f, 0.f);
			((CGameCamera_FLY*)g_pCamera)->SetMouseDelta(v3Temp0);
			((CGameCamera_FLY*)g_pCamera)->SetRotateY(0);
		}
		else if( i3::same_of<CGameCamera_FLY*>(g_pCamera))
		{
			if(CGameCameraManager::i()->getTargetCharacter() ==  nullptr) 
			{
				//observer_battle::i()->init_observer(0);//, true);	
				observer::Context::i()->ChangeViewTarget(BattleSlotContext::i()->FindAnyoneAlive());
			}
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);
		}
		else 
		{
			I3ASSERT_0;
		}

		if( g_pCamera->getCurTarget() )
		{
			CHUD_Scope::instance()->SetEnableSinperScope(false);
			g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
		}
	}

	void set_topview_camera_pos(const VEC3D& p)
	{
		g_v3TopViewCameraPos = p;
	}

	void toggle_topview_camera(void)
	{
		if( !i3::same_of<CGameCamera_FLY*>(g_pCamera))
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FLY);

		g_pCamera->SetCameraPos(g_v3TopViewCameraPos);
		((CGameCamera_FLY*)g_pCamera)->InitTopViewCamera();

		if (CHUD_Scope::instance()->IsEnabledSniperScope())
			CHUD_Scope::instance()->SetEnableSinperScope(false);
	}

	void toggle_observer_fps_camera()
	{
		// 미션모드가 아니라도 자신의 캐릭터에 한해 3인칭 카메라로 변경
		if( i3::same_of<CGameCamera_Observer*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
			g_bCurrCameraIsFPS = true;
		}
		else
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
			g_bCurrCameraIsFPS = false;
		}

		if( g_pCamera->getCurTarget() )
		{
			CHUD_Scope::instance()->SetEnableSinperScope(false);
			g_pCamera->getCurTarget()->Cmd_Observer_Zoom();

			CHARA_VIEW_TYPE type = CHARA_VIEW_3PV;
			if( g_bCurrCameraIsFPS )
			{
				type = CHARA_VIEW_1PV;
			}

			g_pCamera->getCurTarget()->Cmd_ChangeView( type);
		}
	}

}


#include "AIFunc.h"

namespace
{
	void _SetAiEquipment( NET_CHARA_INFO* pNetCharaInfo, AI_ENTRY* pAiEntry, AI_BIND_DATA * pBindData, INT32 difficultyLevel)
	{
		if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE))
		{
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, pBindData->_primaryWeaponName);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, pBindData->_secondaryWeaponName);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, pBindData->_meleeWeaponName);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, pBindData->_throwWeaponName);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, pBindData->_itemWeaponName);

			// Respawn
			pAiEntry->_respawnTime = (REAL32)pBindData->_respawnTime;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD) )
		{
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, GetAIWeapon_Melee(difficultyLevel));
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, 0);

			// Respawn
			pAiEntry->_respawnTime = g_pTempConfig->m_RespawnTime;
		}
		else if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) )
		{
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, pBindData->_meleeWeaponName);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, 0);
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, 0);

			// Respawn
			pAiEntry->_respawnTime = g_pTempConfig->m_RespawnTime;
		}
	}

	void _SetDefaultAiDataWithCheckResource( AI_BIND_DATA * pAiData)
	{
		I3ASSERT( g_pCharaManager != nullptr);
		I3ASSERT( g_pWeaponManager != nullptr);

		CHARA_PARTS_INFO temp;

		// vv3 - check
		for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; i++)
		{
			temp.SetPartsItemID( (CHAR_EQUIPMENT_PARTS) i, pAiData->_Parts.GetPartsItemID( (CHAR_EQUIPMENT_PARTS) i));
		}

		pAiData->_Parts.Reset();
	
		for( INT32 i = 0; i < CHAR_EQUIPMENT_PARTS_COUNT; i++)
		{
			T_ItemID itemID = temp.GetPartsItemID( (CHAR_EQUIPMENT_PARTS) i);
			if( itemID != 0)
			{
				// vv3 - check
				const CEquipInfo * pInfo = g_pCharaInfoDataBase->GetInfo( itemID);
				if( pInfo != nullptr)
				{
					pAiData->_Parts.SetPartsItemID( (CHAR_EQUIPMENT_PARTS) i, itemID);
				}
			}
		}

		T_ItemID * weaponID[CHAR_EQUIPMENT_WEAPON_COUNT] = {
			&pAiData->_primaryWeaponName,
			&pAiData->_secondaryWeaponName,
			&pAiData->_meleeWeaponName,
			&pAiData->_throwWeaponName,
			&pAiData->_itemWeaponName,
		};

		//// 지정된 AI의 무기 리소스(I3CHR)가 로딩되어 있지 않으면 WeaponBase::Create 할때 크래시 위험 때문에 디폴트로 대체한다. komet
		for (INT32 i=0; i<CHAR_EQUIPMENT_WEAPON_COUNT; i++)
		{
			WEAPON_SLOT_TYPE usageType = GetWeaponSlot_WeaponItemIDFunc( *weaponID[i]);	
			WEAPON_CLASS_TYPE classType = WEAPON::ItemID2Class( *weaponID[i]);

			if( usageType != i)
			{
				*weaponID[i] = GetDefaultWeapon( (WEAPON_SLOT_TYPE) i, classType);
			}

			// 지연 로딩하기 위해 주석처리합니다.
			/*WEAPON::RES_ID number = WEAPON::ItemID2ResID( *weaponID[i]);	
			g_pWeaponManager->PreloadWeaponResource( classType, number);*/
		}
	}


	bool _IsAiUsableWeapon(INT32 weaponID)
	{
		// 사용 가능 무기 확인에 사용할 테이블 결정
		i3::vector<INT32>* weaponTable;
		INT32 weaponCount;

		// weaponFlag가 0일 경우는 무기가 설정되지 않은것. 즉! 무기를 가지고 있지 않은것으로 합니다.(잘못된것이 아님)
		if( weaponID == 0) return true;

		WEAPON_SLOT_TYPE weaponSlot = GetWeaponSlot_WeaponItemIDFunc( weaponID);

		switch( weaponSlot)
		{
		case WEAPON_SLOT_PRIMARY:
			weaponTable = &g_vecAI_UsablePrimaryWeaponFlag;
			weaponCount = ::GetAiUsablePrimaryWeaponCount();
			break;
		case WEAPON_SLOT_SECONDARY:
			weaponTable = &g_vecAI_UsableSecondaryWeaponFlag;
			weaponCount = ::GetAiUsableSecondaryWeaponCount();
			break;
		case WEAPON_SLOT_MELEE:
			weaponTable = &g_vecAI_UsableMeleeWeaponFlag;
			weaponCount = ::GetAiUsableMeleeWeaponCount();
			break;
		case WEAPON_SLOT_THROWING1:
			weaponTable = &g_vecAI_UsableThrowingWeaponFlag;
			weaponCount = ::GetAiUsableThrowingWeaponCount();
			break;
		case WEAPON_SLOT_THROWING2:
			weaponTable = &g_vecAI_UsableItemWeaponFlag;
			weaponCount = ::GetAiUsableItemWeaponCount();
			break;
		default:
			weaponTable = nullptr;
			weaponCount = 0;
			break;
		}

		// 사용 가능 무기로 등록되어 있는지 확인한다.
		for(INT32 i = 0; i < weaponCount; i++)
		{
			if (weaponID == (*weaponTable)[i])
			{
				return true;
			}
		}

		return false;
	}
}
	
namespace util_battle
{
	void AddAiChara(INT32 slotIdx)
	{
		AI_BIND_DATA * pBindData = _SetAiInfo( slotIdx, AIModeContext::i()->CurrentLevel);

		if( pBindData != nullptr)
		{
			NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( slotIdx);

			BattleSlotContext::i()->setCharaType( slotIdx, CHARA::TYPE_AI);

#if defined( I3_DEBUG)
	
			static const bool is_random_equip = false;

			if( is_random_equip )
			{
				// vv3 - check
				T_ItemID chara_item_id = pBindData->_Parts.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

				for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx)
				{
					if( idx == CHAR_EQUIPMENT_PARTS_CHARA) 	continue;
					if( idx == CHAR_EQUIPMENT_PARTS_BERET )	continue;	// 임시 예외처리.

					EQUIP::ePART parts_type = EQUIP::CommonParts2eParts( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );
					T_ItemID item_id = GlobalFunc::GetRandParts( parts_type, chara_item_id);

					if( idx == CHAR_EQUIPMENT_PARTS_SKIN && (i3Math::Rand() % 100 > 10) )
					{
						item_id = 0;
					}

					pBindData->_Parts.SetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx), item_id );
				}


				static WEAPON_CLASS_TYPE s_PriClassTable[] =
				{
					WEAPON_CLASS_ASSAULT,
					WEAPON_CLASS_SMG,
					WEAPON_CLASS_DUALSMG,
					WEAPON_CLASS_SNIPER,
					WEAPON_CLASS_SHOTGUN,
					WEAPON_CLASS_MG,
				};

				pBindData->_primaryWeaponName = WEAPON::MakeRandomWeaponItem( s_PriClassTable, GET_ARRAY_COUNT( s_PriClassTable));

				static WEAPON_CLASS_TYPE s_SecClassTable[] =
				{
					WEAPON_CLASS_HANDGUN,
					WEAPON_CLASS_DUALHANDGUN,
					WEAPON_CLASS_CIC,
				};

				pBindData->_secondaryWeaponName = WEAPON::MakeRandomWeaponItem( s_SecClassTable, GET_ARRAY_COUNT( s_SecClassTable));

				static WEAPON_CLASS_TYPE s_MeleeClassTable[] =
				{
					WEAPON_CLASS_KNIFE,
					WEAPON_CLASS_DUALKNIFE,
				};

				pBindData->_meleeWeaponName = WEAPON::MakeRandomWeaponItem( s_MeleeClassTable, GET_ARRAY_COUNT( s_MeleeClassTable));

				static WEAPON_CLASS_TYPE s_ItemClassTable[] =
				{
					WEAPON_CLASS_THROWING_CONTAIN,
					WEAPON_CLASS_THROWING_ATTACK,
					WEAPON_CLASS_THROWING_HEAL,
				};

				pBindData->_itemWeaponName = WEAPON::MakeRandomWeaponItem( s_ItemClassTable, GET_ARRAY_COUNT( s_ItemClassTable));

				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, pBindData->_primaryWeaponName);
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, pBindData->_secondaryWeaponName);
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, pBindData->_meleeWeaponName);
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, pBindData->_throwWeaponName);
				pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, pBindData->_itemWeaponName);
			}
#endif


			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( slotIdx);
			pNetCharaInfo->setCharaParts( &pBindData->_Parts);
			if( pNetCharaInfo->getCharaParts()->IsValid() == false)
			{
				// vv3 - check
				pNetCharaInfo->setCharaParts()->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, CDI_RED_BULLS);
			}

			if( pChara == nullptr || !pChara->getCharaInfo()->GetParts()->IsSameBody( pNetCharaInfo->getCharaParts()))
			{
#if defined( I3_DEBUG)
				if( i3Math::Rand() % 2 == 0 && is_random_equip )
					g_pCharaManager->ChangeTeamAvatarAI((AvatarLoadedCallback)sAfterLoadChara_ForAI, slotIdx, CHARA::TYPE_AI, pBindData);
				else
#endif
				{
					// 2ms
					g_pCharaManager->RemoveChara( slotIdx);

					g_pCharaManager->CreateAvatarAI( (AvatarLoadedCallback) sAfterLoadChara_ForAI, slotIdx, pNetCharaInfo, CHARA::TYPE_AI, pBindData);
				}
			}
			else
			{
				GameEventSender::i()->SetEventOnce( EVENT_RESPAWN_BATTLE_FOR_AI, &slotIdx);
			}
		}
	}

	void	CheckAIData( AI_BIND_DATA * pAiData)
	{
		_SetDefaultAiDataWithCheckResource( pAiData);
	}

	void AddAiCharacter(void)
	{
		INT32 remainAlliedAiCount = AIModeContext::i()->FriendCount;
		INT32 remainEnemyAiCount = AIModeContext::i()->EnemyCount;
		INT32 mySlotIdx = BattleSlotContext::i()->getMySlotIdx();

		for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			// 자기 슬롯은 넘어간다.
			if (i == mySlotIdx)
			{
				continue;
			}

			// 아군인가?
			if( (i % 2) == (BattleSlotContext::i()->getMySlotIdx() % 2) )
			{
				if (0 < remainAlliedAiCount)
				{
					remainAlliedAiCount--;
					util_battle::AddAiChara(i);
				}
			}
			else
			{
				if (0 < remainEnemyAiCount)
				{
					remainEnemyAiCount--;
					util_battle::AddAiChara(i);
				}
			}
		}
	}


	void RespawnAiCharacter( INT32 slotIdx)
	{
		g_pCharaManager->RespawnCharaForAI(slotIdx);
	}

	void ChangeAiCharacter( void)
	{
		INT32 difficultyLevel = AIModeContext::i()->CurrentLevel;

		// 가드
		if (difficultyLevel == 0)
		{
			difficultyLevel = AIModeContext::i()->StartLevel;
			I3PRINTLOG(I3LOG_NOTICE,"[ERROR] TrainingMode: difficultyLevel is zero.");
		}

		// 상대편 Net 캐릭터를 Ai 캐릭터로 변경
		INT32 beginAiIdx;

		if (0 == BattleSlotContext::i()->getMainSlotIdx() % 2)	beginAiIdx = 1;
		else											beginAiIdx = 0;

		for( INT32 i = beginAiIdx; (i / 2) < AIModeContext::i()->EnemyCount; i += 2)
		{
			if( (i % 2) != (BattleSlotContext::i()->getMySlotIdx() % 2) )
			{
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);
				if( pChara == nullptr) continue;

				VEC3D pos;
				
				i3Vector::Copy(&pos, pChara->GetPos());

				AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[i];

				pAiEntry->_level = difficultyLevel;
				pAiEntry->_number = i / TEAM_COUNT;

				// 리소스 매니져에서 Bind할 Ai를 로드합니다.
				g_pCharaManager->LoadAiBindData(pAiEntry->_level, pAiEntry->_number, static_cast<TEAM_TYPE>(i % TEAM_COUNT));
				AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData(pAiEntry->_number);

			

				if( pBindData != nullptr)
				{
					// 닉네임
					BattleSlotContext::i()->setNickForSlot(i, pBindData->_wstr_nickname);
					
					// 계급
					BattleSlotContext::i()->setSlotRank(i, (UINT8)pBindData->_rank);

					// 무기
					NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo(i);
					pNetCharaInfo->setRespawnIndex((UINT32)i);
					pAiEntry->_hp = (INT8)pBindData->_hp;
					
					pChara->setCharaInfo()->SetType(CHARA::TYPE_AI);
					
					// Change AI
					pChara->ChangeControl_AI();
					I3TRACE( "Change AI Character. %d\n", i);

					// AI 연결
					pChara->BindAI(pBindData);

					INT8 hp = BattleSlotContext::i()->getNetCharaInfo(i)->getHP();
					g_pCharaManager->RespawnCharaForAI(i);
					BattleSlotContext::i()->setNetCharaInfo(i)->setHP( hp);

					if (0 < hp)
					{
						pChara->forceSetPos(&pos);
						pChara->setInvincibleTime(0.f);
					}

					g_pCharaManager->setCharaInfo(i)->SetHP((REAL32)hp);
				}
			}
		}
	}

	void StartAiForClient( INT32 slotIdx)
	{
		if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) &&
			(BattleServerContext::i()->IsP2PHost() == false) &&
			(slotIdx == BattleSlotContext::i()->getMySlotIdx()) )
		{
			INT32 beginAiIdx = 0;
			if (0 == BattleSlotContext::i()->getMainSlotIdx() % 2)	beginAiIdx = 1;
			else											beginAiIdx = 0;

			for(INT32 i = beginAiIdx; (i / 2) < AIModeContext::i()->EnemyCount; i += 2)
			{
				if( (i % 2) != (BattleSlotContext::i()->getMySlotIdx() % 2) )
					_SetAiInfo( i, AIModeContext::i()->CurrentAILevel[i]);
			}
		}
	}

	//탈출 미션, Cross Counter 모드(Round Pre Start)
	void AddDinoChara()
	{
		INT32 i;
		DINO_ROOM_INFO* pDinoRoomInfo = &DinoModeContext::i()->DinoSlot;
		I3ASSERT(pDinoRoomInfo);

		//	I3ASSERT(!(pDinoRoomInfo->_uiTRexSlotIdx == WRONG_SLOT_INDEX));

#ifdef RAPTOR_ENABLE
		//static int iii = 0;
		pDinoRoomInfo->_uiRaptorSlotIdx[0] = 0;
		pDinoRoomInfo->_uiTRexSlotIdx = WRONG_SLOT_INDEX;
		//iii++;
		//GlobalFunc::PB_TRACE("pDinoRoomInfo->_uiRaptorSlotIdx[0] %d", pDinoRoomInfo->_uiRaptorSlotIdx[0]);
#endif

#ifdef I3_DEBUG
		GlobalFunc::PB_TRACE("AddDinoChara");
		GlobalFunc::PB_TRACE("_uiTRexSlotIdx %d", DinoModeContext::i()->DinoSlot._uiTRexSlotIdx);
		for( i = 0; i<RAPTOR_TOTAL_CNT; i++) {
			GlobalFunc::PB_TRACE("_uiRaptorSlotIdx %d %d", i, DinoModeContext::i()->DinoSlot._uiRaptorSlotIdx[i]);
		}
#endif

		// 캐릭터와 공룡 모두 삭제
		//g_pCharaManager->RemoveAllChara();
		
		UINT8	DinoMap[ SLOT_MAX_COUNT];

		i3mem::FillZero( DinoMap, sizeof( DinoMap));

		// Trex 설정
		g_pCharaManager->RemoveCharaAddDinoChara( pDinoRoomInfo->_uiTRexSlotIdx, CHARA_RES_ID_DINO_TREX);

		if( pDinoRoomInfo->_uiTRexSlotIdx != 0xFF)
		{
			DinoMap[ pDinoRoomInfo->_uiTRexSlotIdx] = 1;
		}

		I3TRACE("_uiTRexSlotIdx %d ",pDinoRoomInfo->_uiTRexSlotIdx);
		GlobalFunc::PB_TRACE("_uiTRexSlotIdx %d ", DinoModeContext::i()->DinoSlot._uiTRexSlotIdx);
		

		// 렙터 설정
		for( i=0; i<RAPTOR_TOTAL_CNT; i++) {
			if(pDinoRoomInfo->_uiRaptorSlotIdx[i] == WRONG_SLOT_INDEX)
			{
				continue;
			}

			const COMMON_PARTS_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( pDinoRoomInfo->_uiRaptorSlotIdx[i] )->getCommonParts();
			// vv3 - check
			T_ItemID dino_id = pInfo->m_Info[CHAR_EQUIPMENT_COMMON_DINO].m_TItemID;

			const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( dino_id);
			I3ASSERT(pCharaDBInfo != nullptr);
			g_pCharaManager->RemoveCharaAddDinoChara(pDinoRoomInfo->_uiRaptorSlotIdx[i], (CHARA_RES_ID) pCharaDBInfo->GetResID() );

			DinoMap[ pDinoRoomInfo->_uiRaptorSlotIdx[i]] = 1;
		}

		// 일반 캐릭터
		for( i = 0; i<SLOT_MAX_COUNT;i++)
		{
			if( BattleSlotContext::i()->getSlotInfo( i)->_State >= SLOT_STATE_LOAD)			
			{
				//if( g_pCharaManager->getCharaInfo( i)->IsDino() == false)
				if( DinoMap[i] == 0)
				{
					CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);
					if( (pChara != nullptr) && i3::same_of<Avatar*>(pChara) == false )
					{
						g_pCharaManager->RemoveChara( i);	// 공룡이면 삭제
						g_pCharaManager->CreateAvatarPlayer( (AvatarLoadedCallback)sAfterLoadCharaForDinoMode, i,
							BattleSlotContext::i()->setNetCharaInfo(i), (i==BattleSlotContext::i()->getMySlotIdx())?CHARA::TYPE_PLAYER:CHARA::TYPE_PLAYER_NET);
					}
				}
			}
		}

		g_pCharaManager->SetVisibleAtPlayingChara();
	}
}

#include "GameCharaEquipContext.h"

namespace util_battle 
{
	void ApplySkillsystem(INT32 idx)
	{
		INT32 i= 0;

		const NET_CHARA_INFO * pInfo = BattleSlotContext::i()->getNetCharaInfo( idx);
		
		if (pInfo->getSkillsystem(0) != nullptr) 
		{
			for( i=0; i<CATEGORYLIST_MAX; i++)
			{
				const SKILLSYSTEM_APPLY_INFO * pSkillsystem = pInfo->getSkillsystem(i);

				const_cast<NET_CHARA_INFO*>(pInfo)->ResetSkill(i);

				if((pSkillsystem == nullptr) || (pSkillsystem->getLevel() < 1))
					continue;

				if( pSkillsystem->getActivate() == false)
					continue;

				if( pSkillsystem->getActivate() == true)
				{

					SetSkillsystemValue(idx, i);
				}

			}
		}

//		g_pFramework->getCommandSet()->SkillDebugView();
		// 당최 이해가 안되는 호출이라서 바로 주석걸음...(2015.05.21.수빈)
	}

	// 스킬 시스템 능력치 적용
	void	SetSkillsystemValue(INT32 CharaSlotIdx, INT32 SkillSlotIdx )		// 이것은 CommandSet에서 직접 호출
	{
		NET_CHARA_INFO * pInfo = BattleSlotContext::i()->setNetCharaInfo( CharaSlotIdx);
		SKILLSYSTEM_APPLY_INFO * pSkillsystem = pInfo->setSkillsystem(SkillSlotIdx);
		NCHARA_SKILL * pSkillSet = g_pSkillSystemValueSet->getSkillsystemSet( (SKILL_CATEGORYLIST)SkillSlotIdx, pSkillsystem->getIndex());
		if( pSkillSet != nullptr)
		{
			SKILL_TYPE skilltype = pSkillSet->getApplySkill();
			SKILL_WEAPONTYPE weapontype = pSkillSet->getApplyWeaponType();
			REAL32 rVal = 0.f;

			rVal = pSkillSet->getApplyValue(pSkillsystem->getLevel());
			pSkillsystem->setWeaponType( weapontype);	
			pSkillsystem->setSkillType( skilltype);

			switch(skilltype)
			{
			case SKILLLIST_RUN:		
				pSkillsystem->setCharaMoveSpeed(rVal);
				break;
			case SKILLLIST_WALK:	
				pSkillsystem->setCharaWalkSpeed(rVal);
				break;
			case SKILLLIST_DEFENSE:	
				pSkillsystem->setCharaDefense(rVal);
				break;
			case SKILLLIST_HELMETDEFENSE:	
				pSkillsystem->setCharaHelmetDefense(rVal);
				break;
			case SKILLLIST_JUMP:
				pSkillsystem->setCharaJump(rVal);
				break;
			case SKILLLIST_FIRESPEED:
				pSkillsystem->setWeaponFireSpeed(rVal);
				break;
			case SKILLLIST_DAMAGE:
				pSkillsystem->setWeaponDamage(rVal);
				break;
			case SKILLLIST_ACCURACY:
				pSkillsystem->setWeaponDeclination(rVal);
				break;
			case SKILLLIST_RANGE:
				pSkillsystem->setWeaponAttackRange(rVal);
				break;
			case SKILLLIST_PENETRATE:
				pSkillsystem->setWeaponPenetrate(rVal);
				break;
			case SKILLLIST_RELOAD:
				pSkillsystem->setWeaponReloadTime(rVal);
				break;
			case SKILLLIST_CHANGE:
				pSkillsystem->setWeaponChange(rVal);
				break;
			case SKILLLIST_BOOMRANGE:
				pSkillsystem->setWeaponBombRange(rVal);
				break;
			case SKILLLIST_BOOMLENGTH:
				pSkillsystem->setWeaponBombLength(rVal);
				break;
			case SKILLLIST_BOOMTIME:
				pSkillsystem->setWeaponBombTime(rVal);
				break;
			default:
				break;
			}
		}


	}
	
	void	ApplyLRHandFromEnvSet()
	{
		CGameCharaBase * pPlayer = getCurrentChara();
		if (pPlayer != nullptr)
		{
			if( pPlayer->is1PV() && pPlayer->isLocal() && !pPlayer->IsDino())
			{
				switch( g_pEnvSet->m_nHandType)
				{
				case 0:	pPlayer->setLeftHand( false);	break;
				case 1:	pPlayer->setLeftHand( true);	break;
				default:pPlayer->setLeftHand( false);	break;
				}
			}
			else
			{
				pPlayer->setLeftHand( false);
			}
		}
	}

	void	SetToOnObjectCamera(i3Body* pBody, INT32 idx, i3Object* pObj, WeaponBase * pWeapon)
	{
		// revision 45988 아웃포스트 GM 자유시점시 적군 헬기무기탑승자에게 시점이 고정됨 - hansoft 1980
		if( UserInfoContext::i()->IsGM_Observer() && i3::same_of<CGameCamera_FLY*>(g_pCamera) )
			return;

		if( !i3::same_of<CGameCamera_OnObject*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_ONOBJECT, OBSERVER_CAMERA_INTERPOLATE_TIME);
			g_pCamera->SetCameraFromTrans(pBody, idx, OBSERVER_CAMERA_INTERPOLATE_TIME, pObj);
			// g_pCamera->SetInterPolate( 0.f);
			CHUD_Scope::instance()->SetEnableSinperScope(false);
			CHUD_Scope::instance()->SetSniperScope( true, pWeapon->getWeaponInfo(), pWeapon);	// revision 64440

			// 오브젝트 카메라로 변경될때 크로스헤어도 변경
			if( getCurrentChara())
				getCurrentChara()->Cmd_ApplyCrosshair();

			gmap::i()->Process_SetToOnObjectCamera();
		}
	}

	void	SetToOldCameraIfOnObjectCamera()
	{
		if( i3::same_of<CGameCamera_OnObject*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeToOldCamera();

			g_pCamera->SetInterPolate( 0.f);
			if( g_pCamera->getCurTarget() )
			{
				CHUD_Scope::instance()->SetEnableSinperScope(false);
				g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
			}
		}
	}

	void	SetToObserveCamera()
	{
		if( !i3::same_of<CGameCamera_Observer*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);

			if( g_pCamera->getCurTarget() )
			{
				CHUD_Scope::instance()->SetEnableSinperScope(false);
				g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
			}
		}
	}



}


AI_BIND_DATA * _SetAiInfo( INT32 slotIdx, INT32 difficultyLevel)
{
	AI_BIND_DATA * pBindData = nullptr;
	AI_ENTRY* pAiEntry = &AIModeContext::i()->AIEntry[slotIdx];

	// 가드
	if (difficultyLevel == 0)
	{
		difficultyLevel = AIModeContext::i()->StartLevel;
		I3PRINTLOG(I3LOG_WARN, "[ERROR] TrainingMode: difficultyLevel is zero.");
	}

	// 난이도 상승
	//if (pAiEntry->_level != difficultyLevel)
	{
		pAiEntry->_level = difficultyLevel;
		pAiEntry->_number = slotIdx / TEAM_COUNT;

		// 리소스 매니져에서 Bind할 Ai를 로드합니다.
		g_pCharaManager->LoadAiBindData(pAiEntry->_level, pAiEntry->_number, static_cast<TEAM_TYPE>(slotIdx % TEAM_COUNT));
		pBindData = g_pCharaManager->getAiBindData(pAiEntry->_number);

		// 계급
		BattleSlotContext::i()->setSlotRank( slotIdx, (UINT8) pBindData->_rank);

		// 닉네임

		BattleSlotContext::i()->setNickForSlot(slotIdx, pBindData->_wstr_nickname);

		// 무기
		NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo(slotIdx);
		pNetCharaInfo->setRespawnIndex( slotIdx);
		pNetCharaInfo->setHP( (UINT8) pBindData->_hp);
		pAiEntry->_hp				= (INT8) pBindData->_hp;

		// 지정된 AI의 무기와 캐릭터 리소스(I3CHR)가 로딩되어 있지 않으면 AddChara 할때 크래시 위험 때문에 디폴트로 대체한다. komet
		_SetDefaultAiDataWithCheckResource( pBindData);

		_SetAiEquipment(pNetCharaInfo, pAiEntry, pBindData, difficultyLevel);

		pNetCharaInfo->setCharaParts( &pBindData->_Parts);

#if (defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION))
		for( INT32 j = 0; j < 5; j++)
		{
			if(!(MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_MADNESS) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_DIEHARD)))
			{	
				// 사용 가능한 무기인지 확인
				if( _IsAiUsableWeapon( pNetCharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)j)) == false)
				{
					I3PRINTLOG(I3LOG_FATAL,  "AI 캐릭터( %s)가 사용할 수 없는 무기를 장비하였습니다.", BattleSlotContext::i()->getNickForSlot(slotIdx) );
				}
			}
		}
#endif

	}

	return pBindData;
}
