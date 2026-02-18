#pragma once

#include "StepProfileDef.h"

struct MCardSetInfo;
struct MCardInfo;
struct MCardActionInfo;

class MCardSet;
class MCard;
class MCardCallback;

class MCardMgr : public i3::shared_ginst<MCardMgr>
{
public:
	
	MCardMgr();
	~MCardMgr();

	void		SetCallback(MCardCallback* callback);

	INT32				GetMCardSetCountBySetType() const;
	const MCardSetInfo*	FindMCardSetInfoBySetType( INT32 setType ) const;


	i3::rc_wstring GetPrizeDescription(BADGE_TYPE t) const; 

	MCardSet*		GetSelectedMCardSet() const;
	MCard*			GetSelectedMCard() const;
	INT32			GetSelectedMCardSetIndex() const;
	MCardSet*		GetMCardSet(INT32 slotIdx) const;
	bool			HasMCardSet(INT32 cardSetType) const;

	i3::rc_wstring GetCurrHUDDesc(INT32 idxQuest) const;
		

	INT32		FindCardSetSlotWithCardSetType(INT32 cardSetType) const;

	void		SelectActiveCardSetToServer(INT32 selSetIdx);	
	void		SelectActiveCardSetToServer(INT32 selSetIdx, INT32 selCardIdx);
	void		SelectActiveCardToServer(INT32 selCardIdx);

	INT32		FindNextAvailableCardSetSlotIndex() const;

	bool		LoadRegistryFile(const i3::rc_string& strFileName, const i3::rc_string& scriptTextFileTitle);	

	void		UpdateSystemInfoFromServer(UINT8 ui8ActiveQuestCount, UINT32* pui32ActiveQuest, const UINT32 (&ui32QuestCardGoodsID)[QUEST_CARDSET_TYPE_COUNT]);

	void		UpdateUserMCardInfoFromServer(const QUESTING_INFO* questingInfo, const UINT8 (&aQuestCtxActive)[ MAX_QUEST_ALL_USE_NEW ]);
	void		ModifyQuestFromServer(INT32 activeSetIndex, INT32 cardIdx, INT32 questIdx, const QUEST_CTX& ctx);
	void		ModifyCardPrizeFromServer(INT32 activeSetIndex, INT32 cardIdx, const QUEST_ITEM_INFO_NEW& questItemInfo);
	void		ActivateCardSetSlotFromServer(const QUESTING_INFO* questingInfo, INT32 setSlot);
	void		UpdateCardSetSlotsFromServer(const QUESTING_INFO* questingInfo);

	void		UpdateSelectedCardQuestFromServer_OnBattle(INT32 questIdx, INT32 completeCountOfQuest, bool isQuestComplete);

	void		ProcessGiveupBattle();
	void		RequestSelectActiveCardSetToServer(UINT32 activeCardsetIdx);		// 안전하게 되려면 서버로부터 받고 처리하는게 좋겠지 싶은데..
	
	void		ReceiveSelectAndResultFromServer(bool isFinalCardCompleted, bool isEndProtocal = false );		// 클라송수신의 수신으로도 쓰이고, 배틀 안전 종료 후에는 일방적으로 받기도 하는 것 같다..
	void		SendClientSelectStateToServer();
	void		UpdateAllHUDCallbackOnBattle();
	
	bool		RemoveSelectedCardSetAfterFinalPrize();
	bool		RemoveCardSetAfterFinalPrize(INT32 cardSetSlot);

	const MCardActionInfo*	GetMCardActionInfoByID( ACTION_ID actionID) const;
	size_t					GetMCardActionInfoCountByID() const;

	// 서버로부터 받은 목록은 _Server로 표기..
	INT32		GetMCardSetCount_Server() const;
	INT32		GetMCardSetType_Server(INT32 idx) const;		// 인수가 셋타입이 아니고 리턴값이 셋타입임..
	bool		IsExistMCardSet_Server(INT32 MCardSetType) const;

	INT32		GetServerIndexWithMCardSetType(INT32 MCardSetType) const;

	void		PopRewardMark(const MCardInfo*& outInfo, bool& isAnyQuestCompleted, bool& isCardRewardExist, bool& isFinalCard);

private:	
	i3::svector<MCardSetInfo*>		m_vecCardSetInfoByID;
	i3::svector<MCardActionInfo>	m_vecActionByID;
	
	i3::svector<INT32>				m_vecCardSetTypeList_Server;
	i3::svector<INT32>				m_vecServerListIdx_BySetType;		// 서버로부터 받는 것 확인용..

	i3::rc_wstring				m_awstrMedalDescription[BADGE_TYPE_COUNT];
	INT32						m_selCardSetIndex;		// -1, 0,1,2,3 --> 없거나, 4개까지..
	MCardSet*					m_apMCardSet[ MAX_CARDSET_PER_USER ];		// 서버에 의해 4개로 고정된것 같다..
	MCardCallback*				m_pCallback;

	const MCardInfo*			m_pCardInfoForReward;			// 보상표시를 위해 잠시 포인터 기록..
	bool						m_bIsAnyQuestCompleted;		// 카드 중 퀘스트 수행완료가 하나라도 있으면 기록..
	bool						m_bIsCardRewardExist;
	bool						m_bIsCardRewardFinal;		// 마지막 카드 보상이면 최종보상을 추가 표시해야됨.
};																				

