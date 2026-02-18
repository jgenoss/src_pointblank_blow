#include "pch.h"
#include "GameEventHandler.h"
#include "QuickJoinContext.h"

const char * CGameEventHandler::__getQuickjoinEventName( GAME_EVENT evt) const
{
	switch( evt)
	{
	case EVENT_QUICKJOIN_START : return "EVENT_QUICKJOIN_START";
	case EVENT_QUICKJOIN_SECONDBEST : return "EVENT_QUICKJOIN_SECONDBEST";
	default :					return nullptr;
	}
}

void CGameEventHandler::__register_QuickJoin( void)
{
	regSendEvent( EVENT_QUICKJOIN_START,		&CGameEventHandler::__evtQuickJoin_Start_Request);
	regSendEvent( EVENT_QUICKJOIN_SECONDBEST,	&CGameEventHandler::__evtQuickJoin_SecondBest_Request);
}

bool CGameEventHandler::__evtQuickJoin_Start_Request( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_QUICKJOIN_START_REQ);

	PACKET_QUICKJOIN_START_REQ data;
	i3mem::FillZero(&data, sizeof(PACKET_QUICKJOIN_START_REQ));

	data.m_ui8QuickJoinIdx = QuickJoinContext::i()->getQuickJoinIndex();

	for (INT32 i=0; i<MAX_QUICK_JOIN_INFO_COUNT; i++)
	{
		data.m_aQuickJoinInfo[i].m_ui32StageID = QuickJoinContext::i()->getQuickJoinInfo(i)->m_ui32StageID;
	}

	pInfo->_SendPacket.WriteData(&data, sizeof(PACKET_QUICKJOIN_START_REQ));

	return true;
}

bool CGameEventHandler::__evtQuickJoin_SecondBest_Request( GAMEEVENTINFO * pInfo)
{
	pInfo->_SendPacket.SetProtocol(PROTOCOL_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ);

	PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ data;
	i3mem::FillZero(&data, sizeof(PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ));

	data.m_ui32ChannelIdx = QuickJoinContext::i()->getChannelIndex();
	data.m_ui32RoomIdx = QuickJoinContext::i()->getRoomIndex();

	pInfo->_SendPacket.WriteData(&data, sizeof(PACKET_QUICKJOIN_ENTER_2ND_BEST_ROOM_REQ));
	return true;
}