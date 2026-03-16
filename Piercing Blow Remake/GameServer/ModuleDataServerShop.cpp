#include "pch.h"
#include "ModuleDataServer.h"
#include "ShopManager.h"

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
