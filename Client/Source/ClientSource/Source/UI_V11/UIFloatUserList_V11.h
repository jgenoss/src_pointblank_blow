/**
 * \file	UI\UIFloatUserList_V11.h
 *
 * Declares the user interface float user list class.
 */


#if !defined( __UI_FLOAT_USERLIST_H__)
#define __UI_FLOAT_USERLIST_H__

#include "../UI/UISubBase.h"
#include "../UI/UIUtil.h"

#include "../CommunityContext.h"

#define MAX_NATION_MARK 5

class UIMainFrame;
class UIFloatChat_V11;

class UIFloatUserList_V11 : public UISubBase
{
	I3_CLASS_DEFINE( UIFloatUserList_V11, UISubBase);

private:

	struct USERINFO_V11
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

		UINT8			_ui8NationCode;

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

			_ui8NationCode = 0;
		}

		void	CopyTo(USERINFO_V11 * pDest)
		{
			pDest->_Session = _Session;
			pDest->_ClanMark = _ClanMark;

			i3::safe_string_copy(pDest->_Clan, _Clan, NET_CLAN_NAME_SIZE);
			pDest->_Rank = _Rank;
			pDest->_eSportID = _eSportID;

			i3::safe_string_copy(pDest->_Nick, _Nick, NET_NICK_NAME_SIZE);

			pDest->_NickColor = _NickColor;
			pDest->_friendState = _friendState;

			pDest->_szPosition = _szPosition;
			pDest->_BlockReason = _BlockReason;

			pDest->_PosColor = _PosColor;

			pDest->_ui8NationCode = _ui8NationCode;
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

			_ui8NationCode = pSrc->_ui8NationalCode;
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

			pDest->_ui8NationalCode = _ui8NationCode;
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

		bool	IsSameUser(USERINFO_V11 * pDest)
		{
			if( _Session != pDest->_Session)	return false;
			if( i3::generic_compare(_Nick, pDest->_Nick) != 0) return false;
			
			return true;
		}
	};

	i3::vector<USERINFO_V11*>	m_UserInfoList;
	INT32			m_RSelectedIndex;					// 우클릭 선택만 해당된다.
	USERINFO_V11		m_SelectedUserInfo;		// 이부분 LobbyUserInfoList로 되있어서 UserInfo로 바꿉니다. (2012.07.10.수빈)

	USERLIST_TAB	m_CurrentUserTab;
	USERLIST_TAB	m_TabState[ USERLIST_TAB_MAX];		// 총 4개의 State를 설정하기 위한 용도

	bool			m_bReqUserInfo;	/// 요청상태이면 true, 서버에서 받을때까지 기다리고 받으면 출력을 시작

	i3UIImageBoxEx *	m_pClanImg[ 6];

	i3UIFrameWnd *	m_pUserListMainBox;
	i3UIListView_Box *m_pUserListBox;

	i3UIEditBox	*	m_pEditSearch;

	bool			m_bReqClanMemberList;		///<< 서버에 요청 여부를 확인한다.
	INT32			m_SelectedIndex;
	bool			m_bUserUpdate;

	UIFloatChat_V11 *	GetChat( void);

protected:
	void			_ClearUserList( void);
	void			_SetUserInfo( INT32 idx, const USERINFO_V11 * pInfo);
	bool			_SetSelectedUser( INT32 slot);

	bool			_GetUserLocationInfo( CUserLocationInfo & Info);

	void			_UpdateCurrentList( void);
	void			_UpdateFriendList( void);
	void			_UpdateClanMemberList( void);
	void			_UpdateBlockList( void);

	void			_UpdateUserList( void);

	INT32			_FindMyDataIndex( void);
	bool			_IsMyClanMember(INT64);

	/** \brief 자기 자신은 Highlight시킨다*/
	void			_UpdateMyHighlight( void);

	void			_AddFriend(INT32 arg);

public:
	USERINFO_V11 *		getUserInfo( INT32 idx)			{ I3_BOUNDCHK( idx, (INT32)m_UserInfoList.size()); return m_UserInfoList[ idx]; }
	
	USERLIST_TAB	getCurrentUserTab( void)		{ return m_CurrentUserTab; }

public:
	UIFloatUserList_V11();
	virtual ~UIFloatUserList_V11();

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	/** \brief Bind Scene
		\desc 특정 Control Pointer를 접근하거나 Lua Glue Function을 등록합니다. */
	virtual void	BindScene( i3UIScene * pScene) override;

	/** \brief Scene 해제시 호출되어 멤버를 초기화해준다. */
	virtual void	UnBindScene( void) override;

	void			OnMinimizeStart( void);
	void			OnMinimizeEnd( void);

	void			OnMaximizeStart( void);
	void			OnMaximizeEnd( void);

	/** \brief Phase에 따라 Tab의 구성을 업데이트합니다 */
	virtual void	OnMovePhase( UIPHASE phase) override;

	void			SelectUserTab( INT32 tab);
	void			OpenUserPopup( INT32 slot);

	void			SetUserInfo( INT32 slot, LOBBY_USER_INFO_LIST * pInfo);

	void			InputSearchEditBox(UINT32 Param);
	void			SearchUser( void);
	
	void UpdateUserList()	{	_UpdateUserList();	}	

	i3UIEditBox *	GetSerchEditBox(void) { return m_pEditSearch ; }

private:
	GameUI::UserUI_NationMarkData	m_UserUI_NationMarkData;
	GameUI::NationMarkTemplate		m_NationMark[MAX_NATION_MARK];

private:
	void			_UpdateNationMarkAndEffect();
};

#endif
