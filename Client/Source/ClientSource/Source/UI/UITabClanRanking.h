/**
 * \file	UI\UITabClanRanking.h
 *
 * Declares the user interface tab clan list class.
 */


#if !defined( __UI_TAB_CLAN_RANKING_H__)
#define __UI_TAB_CLAN_RANKING_H__

#include "UITabClan.h"
#include "UIClanRankSlot.h"

#define CLAN_RANK_COUNT 8

class UITabClanRanking : public UITabClan_Normal
{
	I3_CLASS_DEFINE( UITabClanRanking, UITabClan_Normal);
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	/** \brief Clan List를 업데이트합니다. */
	void			_UpdateClanList( void);

	bool			_SearchClanListView( const i3::wliteral_range& wKeywordRng);

	void			_InitClanSortType(void);
	void			_InitRadioToolTip(void);

	void			_SetScoreLabel(INT32 sort_idx);

	void			_SelectedSortType(INT32 idx);
	void			AllUnselectedClanList();

public:
	UITabClanRanking();
	~UITabClanRanking();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual bool	OnKey_Enter() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	void			SearchClan( const i3::wliteral_range& wKeywordRng);
	void			VisitClan(INT32 i32Slot);
	void			SortClanList(INT32 i32SortType);
	void			ChangeSlot(bool bAfter);
	void			ChangeSlotToEnd(bool bAfter);
	void			SelectedClan(INT32 idx);

private:
	bool			m_bNeedUpdateClanList;
	INT32			m_iClanListidx;
	INT32			m_iClanListSortType;

	UINT32			m_iBottomIdx;
	UIClanRankSlot	m_ClanSlot[CLAN_RANK_COUNT];

};

#endif // __UI_TAB_CLAN_RANKING_H__