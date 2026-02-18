#if !defined( __UI_POPUP_COMMUNITY_H__)
#define __UI_POPUP_COMMUNITY_H__

#include "UI/UIFloatingBase.h"
#include  "UI/UIFloatUserList.h"
#include "UI/UIDefine.h"
#include "UI/UIUtil.h"

#include "../CommunityContext.h"
#include "MailBoxContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"


enum {
	BUTTON_NON = -1,
	BUTTON_EXIT,
	BUTTON_SEND,
	BUTTON_DELETE,
};

enum MAINTAB
{
	MAINTAB_USERLIST = 0,
	MAINTAB_MESSAGEBOX,
};

enum SUBTAB
{
	SUBTAB_FRIEND,
	SUBTAB_CLAN,
};

enum LISTMOUSE
{
	LIST_LCLICK = 0,
	LIST_RCLICK,
};

class UIFloatCommunity : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatCommunity, UIFloatingBase);

private:
	MAINTAB					m_MainTab;
	USERLIST_TAB			m_CurrentUserTab;
	i3::vector<USERINFO*>	m_UserInfoList;
	bool					m_bReqClanMemberList;		///<< 서버에 요청 여부를 확인한다.
	i3UIListView_Box*		m_pItemListBox;
	i3UIListView_Box*		m_pItemListNote;
	i3UIScrollBar *			m_pScrollRoomList;
	i3UITab*				m_pUserTab;
	i3UITab*				m_pMsgTab;
	
	bool					m_bAlarmCommunity;
	REAL32					m_rAlarmTerm;
	bool					m_bAlarmFlag;
	bool					m_bOnLoad;
	

private:
	MAIL_SORT_TYPE			m_eSortType;
	i3::vector< _Note >		m_NoteList;
	INT32					m_i32SelectIdx;

public:
	void			SelectTab(int idx);
	void			SelectSubTab(int idx);
	void			Click(int idx);
	void			ListClick(int idx, int listidx);
	void			SwitchClick();
	void			ReadMail(INT32 i32Idx);
	void			SendMail(void);
	void			DeleteMail(void);
	void			FindUserButton();
	void			AddFriendButton();

public:
	UIFloatCommunity();
	virtual ~UIFloatCommunity();

	virtual void 	OnCreate( i3GameNode * pParent) override;
	virtual void 	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual bool 	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;
	virtual	void 	OnLoadAllScenes() override;
	
	virtual bool	OnExitStart(void) override;
	virtual void	OnUnloadAllScenes() override;

	virtual void 	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool 	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	virtual bool 	OnKey_Escape() override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	void			SelectUserTab( INT32 tab);

	i3::rc_wstring  GetNickForUserIdx(INT32 idx);
	USERLIST_TAB	GetCurrentTab() { return m_CurrentUserTab; }

protected:
	void			__BuildNoteDataList(void);
	INT32			_GetMailType(NOTE_RECEIVE & Note);
	void			_ClearUserList( void);
	void			_UpdateFriendList( void);
	void			_UpdateClanMemberList( void);
	void			_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring & wstrItemInfo);

	void			_UpdateUserList( void);
	void			_UpdateNoteList( void);

	void			_SetUserInfo( INT32 idx, const USERINFO * pInfo);
	void			_SetUserSlot(void);
	USERINFO *		getUserInfo( INT32 idx)			{ if( idx >= 0 && (size_t)idx < m_UserInfoList.size()) { return m_UserInfoList[ idx];} return nullptr; }
	bool					_IsSafeNoteIdx( INT32 idx ) const
	{
		return 0 <= idx && idx < static_cast<INT32>( m_NoteList.size() );
	}

	void			_SetNoteSlot(void);

	inline INT32	GetNoteCount(void)			{ return static_cast<INT32>( m_NoteList.size()); }

	void			CheckUnReadNote();
	void			UpdateAlarmNote(REAL32 rDeltaSeconds);
	void			StartHighlightNote( void);
	void			EndHighlightNote( void);


	//////////////////////////////////////////////////////////////////////////
	//     UserAction 부분
	//////////////////////////////////////////////////////////////////////////
	void			_OpenUserAction(INT32 idx);
	void			_FriendUserAction(const USERINFO * pInfo);
	void			_ClanUserAction(const USERINFO * pInfo);

	void			_OpenUserInfo(INT32 idx);
	bool			_IsMyClanMember( INT64 uid);
	bool			_IsMyFriend( INT64 uid);
	const CLAN_MEMBER_NODE * _GetClanMemberNode(INT64 uid);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

private:
	typedef void	(UIFloatCommunity::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void			ReceiveGameEvent_Update_UserList( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_DeleteNote( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Update_GiftGoods( INT32, const i3::user_data&);


};

#endif
