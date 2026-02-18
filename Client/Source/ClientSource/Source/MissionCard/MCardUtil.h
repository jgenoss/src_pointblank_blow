#pragma once

enum BADGE_TYPE;
class MCardSet;
class MCard;
struct MCardSetInfo;
struct MCardInfo;
class MCardCallback;

namespace MCardUtil
{

	i3::rc_wstring GetPrizeDescription(BADGE_TYPE t);

	i3::rc_wstring	GetCurrHUDDesc(INT32 idxQuest);
	i3::rc_wstring	FindMCardActionName(ACTION_ID actionID);

	INT32		GetMCardSetCount_Server() ;
	INT32		GetMCardSetType_Server(INT32 idx) ;		// 인수가 셋타입이 아니고 리턴값이 셋타입임..
	bool		IsExistMCardSet_Server(INT32 MCardSetType) ;
	INT32		GetServerIndexWithMCardSetType(INT32 MCardSetType);


	MCardSet*	GetSelectedMCardSet();
	MCard*		GetSelectedMCard();
	INT32		GetSelectedMCardSetIndex();

	INT32		FindCardSetSlotWithCardSetType(INT32 cardSetType);
	INT32		FindCardSetSlotWithServerListIdx(INT32 serverIdx);
	
	bool		IsServerListIdxCurrentSelectedSet(INT32 serverIdx);

	const MCardSetInfo*	FindMCardSetInfoBySetType( INT32 setType );

	MCardSet*		GetMCardSet(INT32 slotIdx);
	bool			HasMCardSet(INT32 cardSetType);

	void		PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, 
		bool& isCardRewardExist, bool& isFinalCard);
	

//
	INT32		FindNextAvailableCardSetSlotIndex();
	MCardSet*	FindNextAvailableCardSet();

	bool		SelectActiveCardSetToServerWithSetType( INT32 setType);
	void		SelectActiveCardSetToServer(INT32 selSetIdx);	
	void		SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx);
	void		SelectActiveCardToServer(INT32 selCardIdx);


/////
	bool		LoadPef(const i3::rc_string& szFileName, const i3::rc_string& szTextFileTitle);

	void Update_Recv_PROTOCOL_BASE_GET_SYSTEM_INFO_ACK(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT]);
	void Update_Recv_PROTOCOL_BASE_GET_USER_INFO_ACK( const QUESTING_INFO* questingInfo, 
		const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ]);
	void Update_Recv_PROTOCOL_BASE_QUEST_GET_ACK(INT32 activeSetIdx, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx);
	void Update_Recv_PROTOCOL_BASE_QUEST_GET_ITEM_ACK(INT32 activeSetIdx, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo);
	void Update_Recv_PROTOCOL_BASE_QUEST_GET_INFO_ACK(const QUESTING_INFO* questingInfo, INT32 setSlot);
	void Update_Recv_PROTOCOL_BASE_QUEST_BUY_CARD_SET_ACK(const QUESTING_INFO* questingInfo, INT32 setSlot);
	void Update_Recv_PROTOCOL_BASE_QUEST_CHANGE_ACK(UINT8 ui8FinishFlagAndProcessedQuestIdx, UINT8 ui8ProcessedTaskCount);
	void Update_Recv_PROTOCOL_BASE_QUEST_DELETE_CARD_SET_ACK(const QUESTING_INFO* questingInfo);
	void Update_Recv_PROTOCOL_BASE_QUEST_ACTIVE_IDX_CHANGE_ACK(bool isFinalCardCompleted);
	void Update_Recv_PROTOCOL_BATTLE_ENDBATTLE_ACK();
	void Update_Recv_PROTOCOL_BATTLE_STARTBATTLE_ACK();
	void Update_Recv_PROTOCOL_BATTLE_MISSION_ROUND_START_ACK();
//////
	void Update_Process_GiveupBattle();
	bool RemoveSelectedCardSetAfterFinalPrize();
	bool RemoveCardSetAfterFinalPrize(INT32 cardSetSlot);
//////	
	void SetMCardCallback(MCardCallback* callback);


	UINT32  ConvertItemID_V10_TO_V20( UINT32 itemID_V10_V20 );

}
