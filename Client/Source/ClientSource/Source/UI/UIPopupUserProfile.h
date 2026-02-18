/**
 * \file	UI\UIPopupUserProfile.h
 *
 * Declares the user interface popup user profile class.
 */


#if !defined( __UI_POPUP_USERPROFILE_H__)
#define __UI_POPUP_USERPROFILE_H__

#include "UIPopupBase.h"
#include "UIUtil.h"
#include "CommunityContext.h"

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
	UIP_BTN_STATE_BLOCK,		// 차단
	UIP_BTN_STATE_INVITEMERC,	// 용병 초대

	UIP_BTN_STATE_MAX
};

// 대상의 상태
enum TARGET_STATE
{
	// 유저타입 TSU(TARGET_STATE_USER)
	TSU_TYPE_MYSELF	= 1<<1,		//나자신
	TSU_TYPE_FRIEND	= 1<<2,		//친구
	TSU_TYPE_HOLD	= 1<<3,		//친구대기
	TSU_TYPE_NORMAL	= 1<<4,		//일반

	// 클랜타입 TSC(TARGET_STATE_CLAN)
	TSC_TYPE_NONE		= 1<<5, //클랜이 없다.
	TSC_TYPE_SAMECLAN	= 1<<6,	//같은 클랜
	TSC_TYPE_OTHERCLAN	= 1<<7, //다른 클랜
	TSC_ENABLE_INVITE   = 1<<8, //초대권한

	// 블럭상태 TSB(TARGET_STATE_BLOCK)
	TSB_STATE_ENABLE	= 1<<9, //블럭 버튼 사용
	TSB_STATE_BLOCK		= 1<<10, //블럭 사용자

	// 위치 TSL(TARGET_STATE_LOCATION)
	TSL_USER_ONLINE		= 1 << 11,	//채널
	TSL_USER_LOBBY		= 1<<12,	//로비
	TSL_USER_READY_ROOM	= 1<<13,	//레디룸
	TSL_TARGET_ONLINE	= 1 << 14,	//대상 로비
	TSL_TARGET_LOBBY	= 1<<15,	//대상 로비
	TSL_TARGET_READY_ROOM = 1<<16,	//대상 레디룸
	TSL_TARGET_OFFLINE	= 1<<17,	//대상이 오프라인
	TSL_TARGET_CLAN_CHENNEL = 1<<18,	// 대상이 클랜채널에 있음
	TSL_TARGET_CLAN_MATCH = 1<<19,	// 대상이 클랜 매치 대기방에 있음.
	TSL_SAME_LOCATION	= 1<<20,	//대상과 같은위치
};

namespace GameUnitTest{ namespace Popup{ class UserProFile;}};
class UIPopupUserProfile : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupUserProfile, UIPopupBase);

	friend class GameUnitTest::Popup::UserProFile;

private:
	UIP_BTN_STATE	m_BtnState[3];
	USERLIST_TAB	m_UserTab;
	INT32			m_i32SelIdx, m_i32SelSlotIdx;

	i3UIImageBoxEx * m_pClanMark;

	i3::rc_wstring	m_wstrBlockReason;
	i3::rc_wstring	m_wstrUserNickName;		/// [NET_NICK_NAME_SIZE];
	bool	m_bClanMercenary;

protected:

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	/** \brief 상대의 상태에 따른 버튼 활성화 처리 */
	void			_SetButtonState(void);
	// 버튼의 기능별로 나눕니다.
	void			_SetUserInfoButton(void);	// 정보 관련 버튼(정보 보기, 귓속말 ..)
	void			_SetCommunityButton(void);	// 커뮤니티 버튼 (친구 추가, 차단, 따라가기 등..)
	void			_SetDefaultButton(void);	// 기본 버튼 (방장 넘기기, 닫기, GM 옵션 등..)

	/** \brief 버튼 입력에 대한 행동을 처리합니다. */
	void			_ProcessClickButton( UIP_BTN_STATE state);

	void			_SetUserInfo(void);
	void			_SetMerceInfo(void);

	void			_SetNickAndKillDeathRate(i3::stack_wstring& out, INT32 Kill, INT32 Death);
	void			_SetUserLocation(i3::stack_wstring& out, const USER_SIMPLE_INFO * pInfo);
	void			_SetClanMark(UINT32 ui32ClanMark);

	void			_SetMercenaryRecord();

	USERLIST_TAB	_FindRelationTab(void);

private:
	void ClickUserInfo_in_none() {}
	void ClickUserInfo_in_room();
	void ClickUserInfo_in_lobby();
	void (UIPopupUserProfile::*fn_click_user_info)();

public:
	UIPopupUserProfile();
	virtual ~UIPopupUserProfile();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 i3::rc_wstring*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd(void) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;
	virtual bool	OnCloseButton( void) override;

	void			UpdateUserInfo( void);

	void			ClickButton( INT32 idx);
	void			ClickWhisper(void);
	void			ClickDetailInfo(void);
	void			ClickUserInfo(void);
	void			ClickGMButton(void);

	void			AddFriend( void);

	void			DoInviteClan( void);
	void			ChangeHost( void);

	//------------------------------------------------------------------------------//
	//								Callback procedure								//
	static void		CbInviteClan(void* pThis, INT32 nParam);
	static void		CbDeleteFriend(void* pThis,INT32 nParam);

protected:
	bool			_IsUserState( UINT32 _type );		// 대상의 상태확인
	void			_AddUserState( UINT32 _type );		// 대상의 상태추가
	void			_UpdateUserState();					// 대상의 상태 업데이트
	void			_UpdateMerceState();
	void			_ResetUserState();					// 대상의 상태 초기화

	UINT32			 m_nUTD;
};

#endif
