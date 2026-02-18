#if !defined( __CARDSET_H__)
#define __CARDSET_H__



typedef		UINT8			CARDSET_ID;

class CCardInfo;

class CCardSet : public i3ElementBase
{
	I3_CLASS_DEFINE( CCardSet, i3ElementBase);

protected:
	INT32				m_MyIndex;
	i3::vector<CCardInfo*>	m_CardInfo;

	CARDSET_ID			m_ID;	
	UINT8				m_ActiveIndex;

public:
	CCardSet( void);
	virtual ~CCardSet( void);

	bool	Reset( INT32 MyIndex );

public:
	INT32				GetIndx()	{return m_MyIndex;}
	CARDSET_ID			GetID(void) const			{   return m_ID; }
	CCardInfo	*		getActiveCard(void)			{	return getCard(m_ActiveIndex);}
	UINT8				getActiveCardIndex(void)	{	return m_ActiveIndex;}
	
	CCardInfo	*		getCard( UINT32 nidx);

	bool				IsComplate();

	void		SetActiveCardIndex(UINT8 Index)	{m_ActiveIndex = Index;}
	void		SetID(CARDSET_ID ID)		{m_ID = ID;}
	void		SetCardQuestItemInfo(INT32 CardIndex, const QUEST_ITEM_INFO_NEW & ItemInfo);

	bool		ChangeActiveCard( UINT32 idx);
};


#endif // __CARDSET_H__