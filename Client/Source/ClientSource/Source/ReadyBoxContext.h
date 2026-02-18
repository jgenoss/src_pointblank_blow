#pragma once
//----------------------------------------------------------------------------------//
//										ReadyBox														//
//----------------------------------------------------------------------------------//

class ReadyBoxContext : public i3::shared_ginst<ReadyBoxContext>
{
	//작성자 : 이정우

public:
	//게임 종료후에 대기 방에서 ReadyBox 획득 한 사람 정보
	//채팅창에 알릴때 사용 됨
	void SetReadyBoxMsg(UINT8 Type, UINT32 ServerIdx, UINT32 ChannelNum,
		UINT32 RoomIdx, T_ItemID ItemID, const char* NickName);

	bool EmptyReadyBoxMsg() const;
	size_t GetReadyBoxMsg_Size() const;

//	SUPPLY_BOX_TYPE GetReadyBoxMsg_Type(size_t index) const;				// 서버에서 안쓴다고 지움 (2014.07.21.수빈)
	UINT32 GetReadyBoxMsg_ServerInx(size_t index) const;
	UINT32 GetReadyBoxMsg_ChannelNum(size_t index) const;
	UINT32 GetReadyBoxMsg_RoomIdx(size_t index) const;
	UINT32 GetReadyBoxMsg_ItemID(size_t index) const;
	const i3::rc_string & GetReadyBoxMsg_Nick(size_t index) const;

	//ReadyBox를 초기화 한다.
	void ClearReadyBoxMsg();

	//게임 종료 후 결과 창에 표시하기 위한 함수
	void SetReadyBoxPresented(UINT8 * PresentedSupplyBox);
	const UINT8* GetReadyBoxPresented() const;

	//보상창에 사용됨.
	void	SetReadyBoxReward(UINT8 Type, T_ItemID ItemID);

//	SUPPLY_BOX_TYPE GetReadyBoxReward_Type() const;						// 서버에서 안쓴다고 지움 (2014.07.21.수빈)
	UINT32	GetReadyBoxReward_ItemID() const;

	//ReadyBox를 초기화 한다.
	void	ClearReadyBoxReward();


private:
	//채팅창에 알릴때 사용 됨
	struct MSG_CONTEXT
	{
		UINT8	 Type = 0;
		UINT32 ServerIdx = 0;
		UINT32 ChannelNum = 0;
		UINT32 RoomIdx = 0;
		T_ItemID ItemID = 0;
		i3::rc_string UserNick;
	};

//	typedef i3::vector< MSG_CONTEXT > Msg_List;
	i3::vector< MSG_CONTEXT > m_Msg_List;

	UINT8 m_PresentedReadyBox[SLOT_MAX_COUNT];

	//보상창에 사용됨.
	UINT8	 m_RewardReadyBoxType;
	UINT32 m_RewardReadyBoxItemID;
};
