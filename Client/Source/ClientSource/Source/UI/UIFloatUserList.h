/**
 * \file	UI\UIFloatUserList.h
 *
 * Declares the user interface float user list class.
 */


#if !defined( __UI_FLOAT_USERLIST_H__)
#define __UI_FLOAT_USERLIST_H__

#include "UIFloatingBase.h"
#include "UIUtil.h"

#include "../CommunityContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

class UIMainFrame;
class UIFramework;

class UIFloatUserList : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatUserList, UIFloatingBase);

	friend class UIPopupUserProfile;

private:



private:
	enum{ UserListMaxFrm = 4 }; //PointBlankRe_UserList 에 사용되는 모든 FrameWnd 수
	i3UIFrameWnd *	m_pMainWnd[UserListMaxFrm];

	i3::vector<USERINFO*>	m_UserInfoList;
	INT32					m_RSelectedIndex;					// 우클릭 선택만 해당된다.

	USERLIST_TAB			m_CurrentUserTab;

	bool					m_bReqUserInfo;	/// 요청상태이면 true, 서버에서 받을때까지 기다리고 받으면 출력을 시작

	bool					m_bReqClanMemberList;		///<< 서버에 요청 여부를 확인한다.
	INT32					m_SelectedIndex;

	i3UIEditBox *			m_pSearchEdit;
	bool					m_bUseSearchFrame;

	bool					m_bFriendAlarm;
	bool					m_bAlarmFlag;
	REAL32					m_rAlarmTerm;

	bool					m_bUserListFrame;
	REAL32					m_rUpdateUserListTime;

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	void			_ClearUserList( void);

	void			_SetUserSlot(void);

	void			_SetActiveFrame(bool bUserlist);
	void			_SetUserListFrame(bool bView);
	void			_SetSearchFrame(bool bView);

	// 탭을 활성화 시킬지 여부 반환
	bool			_CheckActiveTab(INT32 tab);
	
	void			_SetUserInfo( INT32 idx, const USERINFO * pInfo);
	bool			_SetSelectedUser( INT32 slot);

	void			__UpdateLobbyListByLobby(void);

	void			_UpdateLobbyList( void);
	void			_UpdateFriendList( void);
	void			_UpdateClanMemberList( void);

	void			_UpdateUserList( void);

	void			_StartFriendAlarm( void);
	void			_EndFriendAlarm (void);
	void			_UpdateFriendAlarm( REAL32 rDeltaSeconds);

	INT32			_FindMyDataIndex( void);
	bool			_IsMyClanMember(INT64);

	/** \brief 자기 자신은 Highlight시킨다*/
	void			_UpdateMyHighlight( void);

	void			_RemoveOnMouseState(void);

	void			_ClearSelectTab(void);

	void			_SendUserGuideCtrl(INT32 idx);

	void			_ListView_ScrollTop();

public:
	USERINFO *		getUserInfo( INT32 idx)			{ if( idx >= 0 || (size_t)idx < m_UserInfoList.size()) { return m_UserInfoList[ idx];} return nullptr; }
	
	USERLIST_TAB	getCurrentUserTab( void)		{ return m_CurrentUserTab; }
	INT32			getSelectIdx( void)				{ return m_RSelectedIndex; }
	bool			getUseSearchFrame(void)			{ return m_bUseSearchFrame; }

	void			ResetUserTab(void)				{ m_CurrentUserTab = USERLIST_TAB_NONE; }
public:
	UIFloatUserList();
	virtual ~UIFloatUserList();

	virtual void	OnCreate(i3GameNode * pParent) override;

	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;

	virtual bool	OnExitStart(void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	virtual	bool	OnKey_Enter( void) override;
	virtual bool	OnKey_Escape(void) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	void			SelectUserTab( INT32 tab);
	void			OpenUserPopup( INT32 slot);
	void			SelectWhisper(INT32 slot);
	void			SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo);
	void			CloseCurrentFrame(i3UIControl * pCtrl, bool bUserList);	

	void			SearchUser( void);

private:

	typedef void	(UIFloatUserList::*call_func)(INT32, const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func > m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, call_func f);
	
	void			ReceiveGameEvent_Insert_Friend( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Clan_MemberList( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Update_UserList( INT32, const i3::user_data&);
	void			ReceiveGameEvent_GetLobbyUser( INT32, const i3::user_data&);
	void			ReceiveGameEvent_Get_UserInfo_Session( INT32 arg, const i3::user_data&);
	

};

#endif
