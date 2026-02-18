#include "pch.h"
#include "GameEventHandler.h"
#include "Shop.h"
#include "UI/CBFuction.h"

void CGameEventHandler::__register_Gacha( void)
{
	regSendEvent( EVENT_ENTER_GACHA, &CGameEventHandler::__evtGacha_Enter);
	regSendEvent( EVENT_LEAVE_GACHA, &CGameEventHandler::__evtGacha_Leave);
	regSendEvent( EVENT_ENTER_GACHA_INFO, &CGameEventHandler::__evtGacha_ItemInfo);
	regSendEvent( EVENT_START_GACHA, &CGameEventHandler::__evtGacha_Start);
}

bool CGameEventHandler::__evtGacha_ItemInfo( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_ITEM_INFO_REQ);
	return true;
}

bool CGameEventHandler::__evtGacha_Enter( GAMEEVENTINFO * pInfo)
{
	//INT32 version = g_pGameContext->GetShop()->GetVersion();
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_ENTER_REQ);

	return true;
}

bool CGameEventHandler::__evtGacha_Leave( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_LEAVE_REQ);
	return true;
}

bool CGameEventHandler::__evtGacha_Start( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_GACHA_PURCHASE_REQ);
	pInfo->_SendPacket.WriteData(pInfo->_pArg, sizeof(PACKET_H_GACHA_PURCHASE_REQ));
	return true;
}