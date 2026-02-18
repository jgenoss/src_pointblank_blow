#include "pch.h"

#include "ReadyBoxContext.h"

//----------------------------------------------------------------------------------//
//										ReadyBox														//
//----------------------------------------------------------------------------------//
//작성자 : 이정우 

/***************//***************/
//Message
void	ReadyBoxContext::SetReadyBoxMsg(UINT8 Type, UINT32 ServerIdx, UINT32 ChannelNum,
									 UINT32 RoomIdx, T_ItemID ItemID, const char* NickName)
{
	MSG_CONTEXT Context;

	Context.Type = Type;
	Context.ServerIdx = ServerIdx;
	Context.ChannelNum = ChannelNum;
	Context.RoomIdx = RoomIdx;
	Context.ItemID = ItemID;
	Context.UserNick = NickName;

	m_Msg_List.push_back( Context );
}
/*
SUPPLY_BOX_TYPE ReadyBoxContext::GetReadyBoxMsg_Type(size_t index) const
{
	return (SUPPLY_BOX_TYPE)m_Msg_List[index].Type;
}
*/

UINT32 ReadyBoxContext::GetReadyBoxMsg_ServerInx(size_t index) const
{
	return m_Msg_List[index].ServerIdx;
}

UINT32 ReadyBoxContext::GetReadyBoxMsg_ChannelNum(size_t index) const
{
	return m_Msg_List[index].ChannelNum;
}

UINT32 ReadyBoxContext::GetReadyBoxMsg_RoomIdx(size_t index) const
{
	return m_Msg_List[index].RoomIdx;
}

UINT32 ReadyBoxContext::GetReadyBoxMsg_ItemID(size_t index) const
{
	return m_Msg_List[index].ItemID;
}

const i3::rc_string & ReadyBoxContext::GetReadyBoxMsg_Nick(size_t index) const
{
	return m_Msg_List[index].UserNick;
}

void	ReadyBoxContext::ClearReadyBoxMsg()
{
	m_Msg_List.clear();
}


bool ReadyBoxContext::EmptyReadyBoxMsg() const
{
	return m_Msg_List.empty();
}

size_t ReadyBoxContext::GetReadyBoxMsg_Size() const
{
	return m_Msg_List.size();
}

/***************//***************/
//Message
void	ReadyBoxContext::SetReadyBoxPresented(UINT8 * PresentedSupplyBox)
{
	for( int i = 0; i < SLOT_MAX_COUNT; ++i)
	{
		m_PresentedReadyBox[i] = PresentedSupplyBox[i];
	}
}

const UINT8* ReadyBoxContext::GetReadyBoxPresented() const
{
	return m_PresentedReadyBox;
}


/***************//***************/
//Reward Window
void	ReadyBoxContext::SetReadyBoxReward(UINT8 Type, T_ItemID ItemID)
{
	m_RewardReadyBoxType = Type;
	m_RewardReadyBoxItemID = ItemID;
}

/*
SUPPLY_BOX_TYPE ReadyBoxContext::GetReadyBoxReward_Type() const
{
	return (SUPPLY_BOX_TYPE)m_RewardReadyBoxType;
}
*/

UINT32 ReadyBoxContext::GetReadyBoxReward_ItemID() const
{
	return m_RewardReadyBoxItemID;
}

void	ReadyBoxContext::ClearReadyBoxReward()
{
	m_RewardReadyBoxType = 0;
}
