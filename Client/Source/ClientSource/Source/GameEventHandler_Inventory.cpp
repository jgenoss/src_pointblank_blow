#include "pch.h"
#include "GameEventHandler.h"
#include "UI/UICharaInfoMgr.h"
#include "UI/UISideMenu.h"
#include "UI_V11/UITabProfileDesignation_V11.h"

const char * CGameEventHandler::__getInventoryEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_ENTER_INVENTORY : return "EVENT_ENTER_INVENTORY";
	case EVENT_LEAVE_INVENTORY : return "EVENT_LEAVE_INVENTORY";
	case EVENT_INVEN_CHANGE_CHARA_EQUIP : return "EVENT_INVEN_CHANGE_CHARA_EQUIP";
	default :			return nullptr;
	}
}

void CGameEventHandler::__register_Inventory( void)
{
	regSendEvent( EVENT_ENTER_INVENTORY,				&CGameEventHandler::__evtInven_Enter);
	regSendEvent( EVENT_LEAVE_INVENTORY,				&CGameEventHandler::__evtInven_Leave);
	regSendEvent( EVENT_INVEN_CHANGE_CHARA_EQUIP,		&CGameEventHandler::__evtInven_ChangeEquip,				new PACKET_CHAR_CHANGE_EQUIP_REQ );
}

bool CGameEventHandler::__evtInven_Enter( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_INVENTORY_ENTER_REQ);
	return true;
}

bool CGameEventHandler::__evtInven_Leave( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol( PROTOCOL_INVENTORY_LEAVE_REQ);

	//// 쭈니형을 위한 착한 코드 ♥ -> 난 나쁜 남자라고!!!
	//PACKET_INVENTORY_LEAVE_REQ ReqPacket;
	//for( INT32 idx = 0 ; idx < TEAM_COUNT ; ++idx)
	//	ReqPacket.m_ui8TeamSlot[idx] = CCharaInfoContext::i()->GetMainCharaSlotIdx( static_cast<TEAM_TYPE>(idx));

	//ReqPacket.m_bCommonEquipFlag = false; 
	//ReqPacket.m_ui8ChangeSlotCount = 0;

	//pInfo->_SendPacket.WriteData( &ReqPacket, sizeof(PACKET_INVENTORY_LEAVE_REQ) );

	return true;
}

bool CGameEventHandler::__evtInven_ChangeEquip( GAMEEVENTINFO * pInfo)
{
	PACKET_CHAR_CHANGE_EQUIP_REQ* pSend = static_cast<PACKET_CHAR_CHANGE_EQUIP_REQ*>(pInfo->m_S2MOPacket);

	const PACKET_CHAR_CHANGE_EQUIP_REQ * pReqData = reinterpret_cast<const PACKET_CHAR_CHANGE_EQUIP_REQ*>(pInfo->_pArg);

	*pSend = *pReqData;

	return true;
}