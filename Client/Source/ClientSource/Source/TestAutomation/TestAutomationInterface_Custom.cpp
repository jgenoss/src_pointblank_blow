#include "pch.h"
#include "TestAutomationInterface.h"

#if defined( TEST_AUTOMATION)

#include "UI/UIDefine.h"
#include "UI/UIMainFrame.h"

#include "UI/UIShopInfoMgr.h"

#include "IGShopContext.h"
#include "ShopContext.h"
#include "BattleSlotContext.h"
#include "GameCharaBoneContext.h"
#include "MyRoomInfoContext.h"
#include "GameMissionManager.h"
#include "GameMode.h"
#include "UI/UIShopInfoMgr.h"
#include "UI/PurchaseMgr.h"
#include "UI/BuyUtil.h"
#include "UI/UISignal.h"
#include "i3Base/string/ext/contain_string_head.h"


TESTRESULT	TestAutomationInterface::OnCustom( float tmLocal, const wchar_t * pszFunction, const wchar_t * param1, const wchar_t * param2)
{
	if( _wcsicmp( pszFunction, L"MoveToLobbyScreen") == 0)
	{
		// MoveToLobby()
		return MoveToLobbyScreen();
	}
	else if( _wcsicmp( pszFunction, L"IsAtLobbyScreen") == 0)
	{
		// 로비에 있는지 확인.
		return IsAtLobbyScreen();
	}
	else if( _wcsicmp( pszFunction, L"IsAtLoginScreen") == 0)
	{
		return IsAtLoginScreen();
	}
	else if( _wcsicmp( pszFunction, L"IsAtCharaCreateScreen") == 0)
	{
		return IsAtCharaCreateScreen();
	}
	else if( _wcsicmp( pszFunction, L"IsAtServerSelectScreen") == 0)
	{
		return IsAtServerSelectScreen();
	}
	else if (_wcsicmp(pszFunction, L"WaitPlayerRespawn") == 0)
	{
		return WaitPlayerRespawn();
	}
	else if (_wcsicmp(pszFunction, L"IsAtGiftScreen") == 0)
	{
		return IsAtGiftScreen();
	}
	else if (_wcsicmp(pszFunction, L"IsAtShopScreen") == 0)
	{
		return IsAtShopScreen();
	}
	else if (_wcsicmp(pszFunction, L"IsAtWaitRoomScreen") == 0)
	{
		return IsAtWaitRoomScreen();
	}
	else if (_wcsicmp(pszFunction, L"IsAtClanLobbyScreen") == 0)
	{
		return IsAtClanLobbyScreen();
	}
	else if (_wcsicmp(pszFunction, L"IsInBattle") == 0)
	{
		return IsInBattle();
	}
	else if (_wcsicmp(pszFunction, L"IsBombInstalled") == 0)
	{
		return IsBombInstalled();
	}
	else if (_wcsicmp(pszFunction, L"IsBombUninstalled") == 0)
	{
		return IsBombUninstalled();
	}
	else if (_wcsicmp(pszFunction, L"IsBombExploded") == 0)
	{
		return IsBombExploded();
	}
	else if (_wcsicmp(pszFunction, L"CloseAllPopup") == 0)
	{
		return CloseAllPopup();
	}
	else if (_wcsicmp(pszFunction, L"BuyGoods") == 0)
	{
		return BuyGoods(param1);
	}


	return TESTRESULT_ERROR;
}

TESTRESULT	TestAutomationInterface::MoveToLobbyScreen(void)
{
	// 일단 모든 팝업창을 닫는다.
	UIMainFrame * pMainFrm = g_pFramework->GetUIMainframe();
	if( pMainFrm == nullptr)
		return TESTRESULT_FAIL;

	CMessageBoxManager * pMsgManager = g_pFramework->GetMessageBoxManager();
	if( pMsgManager == nullptr)
		return TESTRESULT_FAIL;

	pMainFrm->CloseAllPopup();
	pMsgManager->CloseAllMessageBox();
	pMainFrm->MovePhase( UIPHASE_LOBBY);

	return TESTRESULT_PASS;
}

TESTRESULT TestAutomationInterface::IsAtLobbyScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	// 일단 Lobby Phase 인지 확인...
	if( (pUIFrame == nullptr) || (pUIFrame->GetCurrentPhaseType() != UIPHASE_LOBBY))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT TestAutomationInterface::IsAtLoginScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	// 일단 Lobby Phase 인지 확인...
	if( pUIFrame != nullptr)
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT TestAutomationInterface::IsAtCharaCreateScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	// 일단 Lobby Phase 인지 확인...
	if( (pUIFrame == nullptr) || (pUIFrame->GetCurrentPhaseType() != UIPHASE_CREATE_CHARA))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT TestAutomationInterface::IsAtServerSelectScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	// 일단 Lobby Phase 인지 확인...
	if( (pUIFrame == nullptr) || (pUIFrame->GetCurrentPhaseType() != UIPHASE_SERVER_CHANNEL_LIST))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT TestAutomationInterface::WaitPlayerRespawn(void)
{
	if (g_pFramework->IsBattleStage())
	{
		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

		if((pPlayer->isCharaStateMask(CHARA_STATE_DEATH) == false)
			&& (pPlayer->isInvincible() == false)
			)
			return TESTRESULT_PASS;
	}

	return TESTRESULT_CONTINUE;
}

TESTRESULT	TestAutomationInterface::IsAtGiftScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	if ((pUIFrame == nullptr) || (pUIFrame->GetCurrentSubPhaseType() != USP_GIFTSHOP))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT	TestAutomationInterface::IsAtShopScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	if ((pUIFrame == nullptr) || (pUIFrame->GetCurrentSubPhaseType() != USP_SHOP))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT	TestAutomationInterface::IsAtWaitRoomScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	if ((pUIFrame == nullptr) || (pUIFrame->GetCurrentPhaseType() != UIPHASE_READY_ROOM))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT	TestAutomationInterface::IsAtClanLobbyScreen(void)
{
	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();

	if (g_pFramework->IsBattleStage())
		return TESTRESULT_FAIL;

	if ((pUIFrame == nullptr) || (pUIFrame->GetCurrentPhaseType() != UIPHASE_CLANLOBBY))
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

TESTRESULT	TestAutomationInterface::IsInBattle(void)
{
	if (g_pFramework->IsBattleStage() == false)
		return TESTRESULT_FAIL;

	return TESTRESULT_PASS;
}

GAME_MISSION_STATE	TestAutomationInterface::getMissionState(void)
{
	if (g_pFramework->IsBattleStage() == false)
		return GAME_MISSION_STATE_NONE;

	{
		// 미션전 또는 튜토리얼인지 확인
		MyRoomInfoContext * pRoomCtx = MyRoomInfoContext::i();

		if (pRoomCtx == nullptr)
			return GAME_MISSION_STATE_NONE;

		//MODE 리뉴얼 작업으로 인한 함수 삭제로 해당 함수로 대체 합니다.
		if (!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_BOMB) && 
			!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			return GAME_MISSION_STATE_NONE;
	}

	CGameMissionManager * pMissionMng = CGameMissionManager::i();

	return pMissionMng->getMissionState();
}

///
// 폭탄이 설치되었는가를 확인한다.
TESTRESULT	TestAutomationInterface::IsBombInstalled(void)
{
	if (getMissionState() == GAME_MISSION_STATE_INSTALLED)
		return TESTRESULT_PASS;

	return TESTRESULT_FAIL;
}

TESTRESULT	TestAutomationInterface::IsBombUninstalled(void)
{
	if (g_pFramework->IsBattleStage() == false)
		return TESTRESULT_FAIL;

	{
		// 미션전 또는 튜토리얼인지 확인
		MyRoomInfoContext * pRoomCtx = MyRoomInfoContext::i();

		if (pRoomCtx == nullptr)
			return TESTRESULT_FAIL;

		//MODE 리뉴얼 작업으로 인한 함수 삭제로 해당 함수로 대체 합니다.
		if (!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&
			!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
			return TESTRESULT_FAIL;
	}

	if (gmode::i() == nullptr)
		return TESTRESULT_FAIL;

	CGameMissionManager * pMnger = CGameMissionManager::i();

	if (pMnger == nullptr)
		return TESTRESULT_FAIL;

	if (pMnger->getLastRoundWinType() == MISSION_END_UNINSTALL)
		return TESTRESULT_PASS;

	return TESTRESULT_FAIL;
}


TESTRESULT	TestAutomationInterface::IsBombExploded(void)
{
	if (g_pFramework->IsBattleStage() == false)
		return TESTRESULT_FAIL;

	{
		// 미션전 또는 튜토리얼인지 확인
		MyRoomInfoContext * pRoomCtx = MyRoomInfoContext::i();

		if (pRoomCtx == nullptr)
			return TESTRESULT_FAIL;

		//MODE 리뉴얼 작업으로 인한 함수 삭제로 해당 함수로 대체 합니다.
		if (!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_BOMB) &&
			!pRoomCtx->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
			return TESTRESULT_FAIL;
	}

	if (gmode::i() == nullptr)
		return TESTRESULT_FAIL;

	/*
	WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon(nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));

	if (pWeapon != nullptr)
	{
		I3ASSERT(i3::same_of<WeaponC4*>(pWeapon));
		WeaponC4 * pBomb = (WeaponC4 *)pWeapon;

		if (pBomb->getC4State() == C4_STATE_EXPLODED)
			return TESTRESULT_PASS;
	}
	*/

	CGameMissionManager * pMnger = CGameMissionManager::i();

	if (pMnger == nullptr)
		return TESTRESULT_FAIL;

	if (pMnger->getLastRoundWinType() == MISSION_END_BOMBFIRE)
		return TESTRESULT_PASS;

	return TESTRESULT_FAIL;
}

TESTRESULT TestAutomationInterface::CloseAllPopup(void)
{
	// 일단 모든 팝업창을 닫는다.
	UIMainFrame * pMainFrm = g_pFramework->GetUIMainframe();
	if (pMainFrm == nullptr)
		return TESTRESULT_FAIL;

	CMessageBoxManager * pMsgManager = g_pFramework->GetMessageBoxManager();
	if (pMsgManager == nullptr)
		return TESTRESULT_FAIL;

	pMainFrm->CloseAllPopup();
	pMsgManager->CloseAllMessageBox();

	return TESTRESULT_PASS;
}

bool GetShopItemName_Weapon(i3::rc_wstring& outName, T_ItemID itemID);
void	GetShopItemName_Char(i3::rc_wstring& outName, T_ItemID itemID);
void	GetShopItemName_Item(i3::rc_wstring& outName, T_ItemID itemID);

bool TestAutomationInterface::FindGoodsByName(const wchar_t * pszName, const SHOP_GOODS ** ppGoods, const SHOP_GOODS_PACK ** ppPack)
{
	i3::rc_wstring name;

	CShopContext * pCtx = CShopContext::i();

	for (size_t ctg = SHOP_WEAPON_GOODS; ctg < SHOP_GOODS_MAX; ++ctg)
	{
		SHOP_GOODS_TYPE shop_type = (SHOP_GOODS_TYPE)ctg;

		INT32 cntPack = pCtx->GetGoodsPackCount(shop_type);
		for (INT32 i = 0; i < cntPack; i++)
		{
			const SHOP_GOODS_PACK * pPack = pCtx->GetGoodsPack(shop_type, i);
			if (pPack == nullptr) continue;

			SHOP_GOODS * pGoods = pPack->GetGoods(0);
			if (pGoods == nullptr) continue;

			// 뽑기상점에서 이벤트 아이템을 지정 가능합니다.
			// EVENT 아이템 자체도 GoodsPack에 포함되어 있습니다.
			// 다만 모든 아이템이 EVENT이면 상점에서 제외 합니다.
			for (INT32 j = 0; j < pPack->GetGoodsCount(); j++)
			{
				pGoods = pPack->GetGoods(j);

				UINT32 item_id = pGoods->_ItemID;

				usf::GetShopItemName(name, item_id);

				if (name == pszName)
				{
					*ppGoods = pGoods;
					*ppPack = pPack;
					return true;
				}
			}
		}
	}

	ppGoods = nullptr;
	ppPack = nullptr;

	return false;
}

TESTRESULT TestAutomationInterface::BuyGoods(const wchar_t * goods_id)
{
	const SHOP_GOODS* goods;
	const SHOP_GOODS_PACK * pack;

	if (FindGoodsByName(goods_id, &goods, &pack) == false)
	{
		return TESTRESULT_ERROR;
	}

	if (usf::CheckValidGoods( pack, false) == false)
		return TESTRESULT_FAIL;
	
	buy::PushImmediate(pack);

	UIMainFrame * pUIFrame = g_pFramework->GetUIMainframe();
	pUIFrame->OpenPopup(UPW_BUYITEM, (void*)buy::PurchaseRoute_Immediate, &UISignal::FinishBuyPopup); // "finish_buy_popup");	//즉시구매

	return TESTRESULT_PASS;
}

#endif