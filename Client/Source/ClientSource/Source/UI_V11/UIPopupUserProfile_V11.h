/**
 * \file	UI\UIPopupUserProfile_V11.h
 *
 * Declares the user interface popup user profile class.
 */


#if !defined( __UI_POPUP_USERPROFILE_H__)
#define __UI_POPUP_USERPROFILE_H__

#include "../UI/UIPopupBase.h"
#include "../UI/UIUtil.h"

enum UIP_BTN_STATE
{
	UIP_BTN_STATE_NONE = -1,
	UIP_BTN_STATE_TRACKING,		// 찾아가기
	UIP_BTN_STATE_INVITE,		// 초대하기
	UIP_BTN_STATE_ADDFRIEND,	// 친구추가
	UIP_BTN_STATE_ACCEPTFRIEND,	// 친구수락
	UIP_BTN_STATE_REFUSEFRIEND,	// 친구거절
	UIP_BTN_STATE_REMOVEFRIEND,	// 친구삭제
	UIP_BTN_STATE_INVITECLAN,	// 클랜초대
	UIP_BTN_STATE_INVITEMERC,	// 용병 초대

	UIP_BTN_STATE_MAX
};

enum UIP_TAB
{
	UIP_TAB_RECORD = 0,	// 전적
	UIP_TAB_EQUIPMENT,	// 장비
};

class UIPopupUserProfile_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserProfile_V11, UIPopupBase);

private:
	bool			m_bUserDetailInfoFlag;
	INT32			m_SelectedIndex;		// 친구,클랜의 선택된 유저 인덱스
	INT32			m_ReadyRoomSlotIndex;	// 대기방에서의 Slot Index

	UIP_BTN_STATE	m_BtnState1;
	UIP_BTN_STATE	m_BtnState2;
	UIP_BTN_STATE	m_BtnState3;
	UIP_BTN_STATE	m_BtnState4;

	USERLIST_TAB	m_UserTab;

	LOBBY_USER_INFO_LIST 	m_CurrentUser;	// 로비의 일반 유저 정보

	bool			m_bReceivedUserInfo;
	bool			m_bWaitReceive;
	bool			m_bCreateChara;
	bool			m_bClanMercenary; 

	UIP_TAB			m_CurrentTab;

	i3UIFrameWnd *	m_pCharaViewFrame;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	/** \brief 상대의 상태에 따른 버튼 활성화 처리 */
	void			_SetButtonState( bool bClan = false);

	/** \brief UI 버튼 초기화 */
	void			_InitializeUI( void);

	// 유저 정보 초기화
	bool			_InitInfoForUserList(void * pArg);
	bool			_InitinfoForSearchUser(void);

	/** \brief 기본 정보 설정 */
	void			_SetDefault( void);

	/** \brief 전적 설정 */
	void				_SetRecord( void);
	USER_INFO_RECORD *	_GetUserRecordInfo(void);

	/** \brief Item 정보를 설정 */
	void			_SetEquipment( void);
	void 			_GetUserEquipWeapon(USER_INFO_EQUIPMENT_WEAPON & outInfo);
	
	void			_SetCharacter();

	/** \brief 버튼 입력에 대한 행동을 처리합니다. */
	void			_ProcessClickButton( UIP_BTN_STATE state);


public:
	UIPopupUserProfile_V11();
	virtual ~UIPopupUserProfile_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

	virtual void	OnDragStart( i3UIControl * pControl, INT32 x, INT32 y) override;
	virtual void	OnDragging( i3UIControl * pControl, INT32 x, INT32 y) override;

	virtual bool	OnCloseButton(void) override;

	void			UpdateUserInfo( void);

	void			SetUserInfo( void);
	void			SetFriendInfo( void);
	void			SetClanMemberInfo( void);
	void			SetBlockUserInfo( void);

	bool			GetCurrentSelectedUserLocation( CUserLocationInfo & UserLocInfo);

	void			ChangeTab( UIP_TAB tab);
	void			ClickButton( INT32 idx);
	void			AddFriend( void);
	void			BuyViewItem( void);
	void			CharacterChange( void);

	void			DoInviteClan( void);
	void			SetReadyroomIdx(INT32 RoomIdx);

	LOBBY_USER_INFO_LIST *	m_Info;
	LOBBY_USER_INFO_LIST * GetUserInfo(){ return m_Info; };

	//------------------------------------------------------------------------------//
	//								Callback procedure								//
	static void		CbInviteClan(void* pThis, INT32 nParam);
};

#endif
