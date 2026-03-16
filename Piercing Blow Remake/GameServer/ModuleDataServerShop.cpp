#include "pch.h"
#include "ModuleDataServer.h"
#include "ShopManager.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "GameServerContext.h"

// ============================================================================
// Request operations - Shop
// ============================================================================

void ModuleDataServer::RequestShopList()
{
	IS_SHOP_LIST_REQ req;
	req.i32SessionIdx = 0;
	SendRequest(PROTOCOL_IS_SHOP_LIST_REQ, &req, sizeof(req));
}

// ============================================================================
// Response handlers - Shop
// ============================================================================

void ModuleDataServer::OnShopListAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_SHOP_LIST_ACK))
		return;

	IS_SHOP_LIST_ACK* pAck = (IS_SHOP_LIST_ACK*)pData;

	printf("[ModuleDataServer] Shop list received - Count=%d, Result=%d\n",
		pAck->i32ItemCount, pAck->i32Result);

	if (pAck->i32Result != 0 || pAck->i32ItemCount <= 0)
		return;

	IS_SHOP_ITEM_ENTRY* pEntries = (IS_SHOP_ITEM_ENTRY*)(pData + sizeof(IS_SHOP_LIST_ACK));

	// Load into ShopManager for use by GameSessionShop
	if (g_pShopManager)
		g_pShopManager->LoadFromDataServer(pEntries, pAck->i32ItemCount);
}

// ============================================================================
// Request operations - Shop Buy
// ============================================================================

void ModuleDataServer::RequestShopBuy(int64_t i64UID, int i32SessionIdx, uint32_t ui32GoodsId,
									   uint32_t ui32ItemId, uint8_t ui8PayType, int i32Price)
{
	IS_SHOP_BUY_REQ req;
	req.i64UID = i64UID;
	req.i32SessionIdx = i32SessionIdx;
	req.ui32GoodsId = ui32GoodsId;
	req.ui32ItemId = ui32ItemId;
	req.ui8PayType = ui8PayType;
	req.i32Price = i32Price;
	SendRequest(PROTOCOL_IS_SHOP_BUY_REQ, &req, sizeof(req));
}

void ModuleDataServer::OnShopBuyAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_SHOP_BUY_ACK))
		return;

	IS_SHOP_BUY_ACK* pAck = (IS_SHOP_BUY_ACK*)pData;

	printf("[ModuleDataServer] Shop buy result - UID=%lld, Item=0x%08X, Result=%d\n",
		pAck->i64UID, pAck->ui32ItemId, pAck->i32Result);

	if (!g_pGameServerContext)
		return;

	GameSessionManager* pMgr = g_pGameServerContext->GetSessionManager();
	if (!pMgr)
		return;

	GameSession* pSession = pMgr->GetSession(pAck->i32SessionIdx);
	if (pSession && pSession->GetUID() == pAck->i64UID)
	{
		pSession->OnShopBuyResult(pAck->ui32ItemId, pAck->i32Result,
								  pAck->i32RemainingGP, pAck->i32RemainingCash);
	}
}

// ============================================================================
// Request operations - Inventory Update
// ============================================================================

void ModuleDataServer::RequestInventoryUpdate(int64_t i64UID, uint8_t ui8Operation, uint32_t ui32ItemId,
											   int i32ItemCount, int i32SlotIdx, uint8_t ui8IsEquipped)
{
	IS_INVEN_UPDATE_REQ req;
	req.i64UID = i64UID;
	req.ui8Operation = ui8Operation;
	req.ui32ItemId = ui32ItemId;
	req.i32ItemCount = i32ItemCount;
	req.i32SlotIdx = i32SlotIdx;
	req.ui8IsEquipped = ui8IsEquipped;
	SendRequest(PROTOCOL_IS_INVEN_UPDATE_REQ, &req, sizeof(req));
}

void ModuleDataServer::OnInvenUpdateAck(char* pData, int i32Size)
{
	if (i32Size < (int)sizeof(IS_INVEN_UPDATE_ACK))
		return;

	IS_INVEN_UPDATE_ACK* pAck = (IS_INVEN_UPDATE_ACK*)pData;

	if (pAck->i32Result != 0)
		printf("[ModuleDataServer] Inventory update failed - UID=%lld, Item=0x%08X, Result=%d\n",
			pAck->i64UID, pAck->ui32ItemId, pAck->i32Result);
}
