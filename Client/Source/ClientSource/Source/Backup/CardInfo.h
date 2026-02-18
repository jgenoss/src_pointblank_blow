#if !defined( __CARDINFO_H__)
#define __CARDINFO_H__


class CCardInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( CCardInfo, i3ElementBase);

protected:
	INT32				m_MyIndex;
	UINT16				m_ActiveIndex;						//카드 인덱스 
	QUEST_ITEM_INFO_NEW m_QuestItemInfo;					//카드 완료 보상 
	QUEST_CTX			m_QuestData[MAX_QUEST_PER_CARD_NEW];
	CQuestBase	*		m_pQuest[MAX_QUEST_PER_CARD_NEW];		//Quest Pointer 

public:
	CCardInfo( void);
	virtual ~CCardInfo( void);

	bool			Reset( INT32 MyIndex );
	void			RestoreQuest();
	void			ApplyQuest(INT32 idx);

public:
	UINT32			getActiveQuestIndex()					{ return m_ActiveIndex;} //0xFFFF
	UINT32			getActiveQuestIndex( UINT32 idx )		{ return ((m_ActiveIndex & (0xF << (idx * MAX_QUEST_PER_CARD_NEW))) >> (idx * MAX_QUEST_PER_CARD_NEW));}	
	CQuestBase	*	getActiveQuest( UINT32 nRow)			{ return m_pQuest[ getActiveQuestIndex( nRow) + (nRow * MAX_QUEST_COL_PER_CARD)];}		//위에거 때문에 있는거. 
	CQuestBase	*	getQuest( UINT32 idx)					{ I3ASSERT( idx < MAX_QUEST_PER_CARD_NEW); return m_pQuest[idx];}	

	UINT32			GetPrize(void);
	UINT32			GetPoint(void);
	UINT32			GetExp(void);

	//CTX 의 순서 번호 입니다. 
	INT32			GetIndx()	{return m_MyIndex;}
	void			SetActiveIndex(UINT16 Index)	{m_ActiveIndex = Index;}
	void			GetQuestData(INT32 Index, QUEST_CTX & Ctx);
	void			SetQuestData(INT32 Index, QUEST_CTX Ctx);
	void			SetQuestCompalte(INT32 Index, UINT8 Value);
	void			SetQuestItemData(const QUEST_ITEM_INFO_NEW & ItemInfo);

public:
	bool			CompleteQuest( UINT32 nRow);								//해당 열의 active quest를 완료시킨다.
	void			SaveQuest( UINT32 nIdx);
	bool			IsCompletedCard();

protected:
	void			SetActiveQuestIndex( UINT32 nRow, UINT32 idx);
};

#endif // __CARDINFO_H__
