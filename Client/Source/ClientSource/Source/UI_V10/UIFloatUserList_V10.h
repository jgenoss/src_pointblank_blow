/**
 * \file	UI\UIFloatUserList_V10.h
 *
 * Declares the user interface float user list class.
 */


#if !defined( __UI_FLOAT_USERLIST_H__)
#define __UI_FLOAT_USERLIST_H__

#include "../UI/UIFloatingBase.h"
#include "../UI/UIUtil.h"

#include "../CommunityContext.h"


class UIMainFrame;
class UIFloatChat_V10;

class UIFloatUserList_V10 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatUserList_V10, UIFloatingBase);

private:

	struct USERINFO
	{
		INT32			_Session;					// DB ID
		UINT32			_ClanMark;						// Clan Mark
		TTCHAR			_Clan[ NET_CLAN_NAME_SIZE];		// Clan Name
		UINT8			_Rank;							// 랭킹
		UINT8			_eSportID;						// E-Sport ID
		TTCHAR			_Nick[ NET_NICK_NAME_SIZE];	// 닉네임
		I3COLOR			_NickColor;
		FRIEND_STATE	_friendState;
		i3::wstring		_szPosition;
		i3::wstring		_BlockReason;
		I3COLOR			_PosColor;

		void	Reset( void)
		{
			_Session	= -1;
			_ClanMark	= 0xFFFFFFFF;
			_Clan[0]	= 0;
			_Rank		= 0;
			_eSportID	= 0xFF;
			_Nick[0]	= 0;
			_BlockReason.clear();
			i3Color::Set( &_NickColor, (UINT8) 255, 255, 255, 255);
			_friendState	= FRIEND_STATE_REQUEST;
			_szPosition.clear();
			i3Color::Set( &_PosColor, (UINT8) 255, 255, 255, 255);
		}

		void	CopyFromLobbyUser( const LOBBY_USER_INFO_LIST * pSrc)
		{
			_Session = pSrc->_Session;
			_ClanMark = pSrc->_ClanMark;
			if( pSrc->_ClanName[0] != 0)
			{
				i3::safe_string_copy(_Clan,  pSrc->_ClanName, NET_CLAN_NAME_SIZE);
				//i3String::Copy( _Clan, pSrc->_ClanName, NET_CLAN_NAME_SIZE);
			}

			_Rank = pSrc->_Rank;
			_eSportID = pSrc->_eSportID;
			i3::safe_string_copy(_Nick, pSrc->_Nick, NET_NICK_NAME_SIZE);
			//i3String::Copy( _Nick, pSrc->_Nick, NET_NICK_NAME_SIZE);
		#ifdef USE_COLOR_NICK
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( pSrc->_NickColor);
			if( pNickColor != nullptr)
				i3mem::Copy( &_NickColor, pNickColor, sizeof( _NickColor) );
		#endif
		}

		void	CopyToLobbyUser( LOBBY_USER_INFO_LIST * pDest)
		{
			pDest->_Session = _Session;
			pDest->_ClanMark = _ClanMark;
			i3::safe_string_copy(pDest->_ClanName, _Clan, NET_CLAN_NAME_SIZE);
			//i3String::Copy( pDest->_ClanName, _Clan, NET_CLAN_NAME_SIZE);
			pDest->_Rank = _Rank;
			pDest->_eSportID = _eSportID;

			i3::safe_string_copy(pDest->_Nick, _Nick, NET_NICK_NAME_SIZE);
			//i3String::Copy(	pDest->_Nick, _Nick, NET_NICK_NAME_SIZE);
		#ifdef USE_COLOR_NICK
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( pDest->_NickColor);
			if( pNickColor != nullptr)
				i3mem::Copy( &_NickColor, pNickColor, sizeof( _NickColor) );
		#endif
		}

		void	CopyFromClanUser( const CLAN_MEMBER_NODE * pSrc)
		{
			i3::safe_string_copy(_Nick, pSrc->_wstr_nick, NET_NICK_NAME_SIZE);
			//i3String::Copy( _Nick, pSrc->_nick, NET_NICK_NAME_SIZE);
			_Rank = pSrc->_rank;
			_friendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);
		}

		void	CopyFromFriend( const FRIEND_NODE * pSrc)
		{
			i3::safe_string_copy(_Nick, pSrc->_wstr_nick, NET_NICK_NAME_SIZE);
			//i3String::Copy( _Nick, pSrc->_nick, NET_NICK_NAME_SIZE);
			_Rank = pSrc->_rank;
			
			_friendState = (FRIEND_STATE)GET_FRIEND_STATE(pSrc->_state);
		}
		void	CopyFromBlock( const BLOCK_USER * pSrc)
		{
			i3::safe_string_copy(_Nick, pSrc->_wstrNick, NET_NICK_NAME_SIZE);
			i3::safe_string_copy(_BlockReason, pSrc->_wstr_comment, BLOCK_COMMENT_MAX);
			//i3String::Copy( _Nick, pSrc->_nick, NET_NICK_NAME_SIZE);
			//i3String::Copy( _BlockReason, pSrc->_comment, BLOCK_COMMENT_MAX);
			_Rank = pSrc->_blockInfo._rank;
		}

		bool	IsSameUser( USERINFO * pDest)
		{
			if( _Session != pDest->_Session)	return false;
			if( i3::generic_compare(_Nick, pDest->_Nick) != 0) return false;
			
			return true;
		}
	};

	i3::vector<USERINFO*>	m_UserInfoList;
	INT32			m_RSelectedIndex;					// 우클릭 선택만 해당된다.
	USERINFO		m_SelectedUserInfo;		// 이부분 LobbyUserInfoList로 되있어서 UserInfo로 바꿉니다. (2012.07.10.수빈)

	bool			m_bReqUserInfo;	/// 요청상태이면 true, 서버에서 받을때까지 기다리고 받으면 출력을 시작

	i3UIFrameWnd*	m_pMainFmw;

	i3UIImageBoxEx *	m_pClanImg[ 6];

	i3UIListView_Box *	m_pUserListBox;

	bool			m_bReqClanMemberList;		///<< 서버에 요청 여부를 확인한다.
	INT32			m_SelectedIndex;
	bool			m_bUserUpdate;

	UIFloatChat_V10 *	GetChat( void);

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	void			_ClearUserList( void);
	void			_SetUserInfo( INT32 idx, const USERINFO * pInfo);
	bool			_SetSelectedUser( INT32 slot);

	bool			_GetUserLocationInfo( CUserLocationInfo & Info);

	void			_UpdateCurrentList( void);
	void			_UpdateUserList( void);

	INT32			_FindMyDataIndex( void);
	bool			_IsMyClanMember(INT64);

	/** \brief 자기 자신은 Highlight시킨다*/
	void			_UpdateMyHighlight( void);

	void			_AddFriend(INT32 arg);

	void			_SetPhaseAtPos();

public:
	USERINFO *		getUserInfo( INT32 idx)			{ I3_BOUNDCHK( idx, (INT32)m_UserInfoList.size()); return m_UserInfoList[ idx]; }
	INT32			GetSlotIdxbyName(i3::rc_wstring _name);
public:
	UIFloatUserList_V10();
	virtual ~UIFloatUserList_V10();

	virtual bool	OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */) override;

	virtual bool	OnExitStart( void) override;

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;
	
	virtual bool	OnRevive(i3RenderContext * pCtx) override;

	void			OpenUserPopup( INT32 slot);
	void			OpenUserProfile(INT32 slot);

	void			SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo);

	void			InputSearchEditBox(UINT32 Param);
	void			SearchUser( void);
	
	void UpdateUserList()	{	_UpdateUserList();	}	

};

#endif
