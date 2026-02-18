#ifndef _QuestTextDictionary_h
#define _QuestTextDictionary_h

// 임무카드, 보상 최대 ID 갯수
#define MAX_QUEST_ID_COUNT		256

// 최대 하나의 임무카드당 최종 보상 아이템 갯수
#define MAX_QUEST_REWARD_ITEM_COUNT		4

// 카드셋 용 문자열 구조체
struct SCardSetText
{
	char _title[MAX_STRING_COUNT];
	char _name[MAX_STRING_COUNT];
	char _character[MAX_STRING_COUNT];
	char _FileName[MAX_STRING_COUNT];
	INT32 _totalExp;
	INT32 _totalPoint;
	INT32 _bonusExp;
	INT32 _bonusPoint;
	INT32 _bonusBadgeMaster;		// 최종 지급 마스터 훈장 개수
	UINT32 _bonusItemID[MAX_QUEST_REWARD_ITEM_COUNT];	// 최종 지급 아이템

	std::string _bonusItemTime[MAX_QUEST_REWARD_ITEM_COUNT];

	typedef i3::vector<i3::string> vBonusItemTimeCon;
	vBonusItemTimeCon _vbonusItemTime;

	INT32 _bonusItemCount;
};

// 보상(약장, 휘장, 훈장) 용 문자열 구조체
struct SPrizeText
{
	char _name[MAX_STRING_COUNT];
	char _description[MAX_STRING_COUNT];
	char _gainWay[MAX_STRING_COUNT];
};


// 임무시스템 관련 문자열 관리 클래스
class CQuestTextDictionary: public i3ElementBase
{
	I3_CLASS_DEFINE(CQuestTextDictionary, i3ElementBase);
public:
	CQuestTextDictionary();
	virtual ~CQuestTextDictionary();

	void Create(void);
	
	void GetCardSetTitleString(char* string, const size_t sizeStrMax, UINT8 cardSetId) const;
	void GetCardSetNameString(char* string, const size_t sizeStrMax, UINT8 cardSetId) const;

	INT32 GetCardSetTotalExp(UINT8 cardSetId) const;
	INT32 GetCardSetTotalPoint(UINT8 cardSetId) const;
	INT32 GetCardSetBonusExp(UINT8 cardSetId) const;
	INT32 GetCardSetBonusPoint(UINT8 cardSetId) const;
	INT32 GetCardSetBonusMasterBadgeCount(UINT8 cardSetId) const;
	UINT32 GetCardSetBonusItemID(UINT8 cardSetId, INT32 cnt = 0) const;
	INT32 GetCardSetBonusItemIDCount(UINT8 cardSetId) const;

	i3::string GetCardSetBonusItemTime(UINT8 cardSetId, INT32 cnt = 0) const;
	//이벤트 임무카드 기간 표시
	const char* GetEventCardPerido() const	{	return m_EventCardPerido;	}

	void GetRibbonNameString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetRibbonDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetRibbonGainWayString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;	
	void GetEnsignNameString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetEnsignDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetEnsignGainWayString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;	
	void GetMedalNameString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetMedalDescriptionString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;
	void GetMedalGainWayString(char* string, const size_t sizeStrMax, UINT8 prizeId) const;

	const char *	GetQuestFileName(ENUM_QUEST_CARDSET_TYPE CardType) const;
	UINT32			GetQuestCardPrize(UINT32 cardSetID, UINT32 CardIndex);
    
private:
	SPrizeText m_ribbonText[MAX_QUEST_ID_COUNT];
	SPrizeText m_ensignText[MAX_QUEST_ID_COUNT];
	SPrizeText m_medalText[MAX_QUEST_ID_COUNT];

	SCardSetText m_cardSetText[QUEST_CARDSET_TYPE_COUNT];
	UINT32 m_cardPrizeItem[QUEST_CARDSET_TYPE_COUNT][MAX_CARD_PER_CARDSET];

	//이벤트 임무카드 기간 표시
	char m_EventCardPerido[MAX_STRING_COUNT];
};


extern CQuestTextDictionary* g_pQuestTextDictionary;

#endif
