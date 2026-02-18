//**
// * \file	UI\UIPopupMailBox_V11.h
// *
// * Declares the user interface popup mail box class.
// */


#if !defined( __UI_POPUP_MAILBOX_V11_H__)
#define __UI_POPUP_MAILBOX_V11_H__

#include "UI/UIPopupBase.h"
#include "MailBoxContext.h"
#include "../CommunityContext.h"

#define MAX_SLOT_FOR_CONTACT_LIST	8
#define MAX_NOTE_SLOT				10
#define MAX_NOTE_COUNT				100

enum COMMUNITY_TAB
{
	COMMUNITY_FRIEND_TAB = 0,	//	친구 
	COMMUNITY_BLOCK_TAB,		//	차단
	COMMUNITY_CLAN_TAB,			//	클랜
	COMMUNITY_TAB_INVALID,
};

enum NickPopupType
{
	NPT_ADD_FRIEND = 0,
	NPT_FIND_USER,
	NPT_ADD_BLOCK,
};

enum NOTE_TYPE
{
	NOTE_TYPE_SEND = 0,
	NOTE_TYPE_REPLY,
	NOTE_TYPE_GIFT,
};

enum MessageTab
{
	MessageTab_None = -1,
	MessageTab_Receive,
	MessageTab_Send,
	MessageTab_ReceivePresent,
	MessageTab_SendPresent,
};

class UITabMailBoxSend;
class UITabMailBoxReceive;

class UIPopupMailBox_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMailBox_V11, UIPopupBase);

private:
	MessageTab				m_CurrentTab;
	
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	void			_InitTab( void);

public:
	char					m_szNote[MAX_STRING_COUNT];
	INT32					m_SelectedListIdx;

	UIPopupMailBox_V11();
	virtual ~UIPopupMailBox_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;

	virtual void	ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	
	void			ChangeTab( MessageTab tab);
	
	//--------------------------------------------------------------------------//
	//									Note									//
	//--------------------------------------------------------------------------//
private:
	NOTE_TYPE		m_nNoteType;
	bool			m_bNoteCheck[MAX_NOTE_COUNT];
	bool			m_bAlarmCommunity;
	REAL32			m_rAlarmTerm;
	bool			m_bAlarmFlag;
	static bool		m_bWarningNoteCount;

	FRIEND_NODE			m_SelectedFriendNode;
	CLAN_MEMBER_NODE	m_SelectedClanMemberNode;

	INT32			m_PackageGoodsID;
	INT32			m_nDelMessage;		// 삭제 Row

	INT32			m_nTypeMessage[ NOTE_MESSAGE_MAX ];


	INT64			m_CurrentUserInfoUID;
	COMMUNITY_TAB	m_CurrentUserInfoType;

	HDC				m_hdc;					// 최적화를 위해서 GetTextExtentPoint32의 재료가 되는 dc를 미리 구합니다..(11.10.25.수빈)
	INT32			m_subject_width;		// 뷰박스 제목칸의 너비를 미리 구함.
	INT32			m_ellipse_width;		// ellipse의 너비를 미리 구함..
	INT32			m_scroll_idx;	
	INT32			m_view_item_count;	
	bool			m_abLoadedItem[256];
	bool			m_checkBoxState[256];

	i3UIListView_Box*	m_pListBox;
	
protected:
	void			_UpdateAlarmNote(REAL32 rDeltaSeconds);

public:
	/** \brief 깜박이기 */
	void			StartAlarmNote( void);
	void 			EndAlarmNote(void);

	void 			ReceiveAllNote();			// 함수의 기능을 둘로 쪼갠다.. ( 사이즈 조절 + 리스트 갱신)

	//void			ReceiveAllNote_Size();
	void			ReceiveAllNote_Update();
	void			ReceiveAllNote_Size( const INT32 nCount );
	void			AllNote_Update();
	void			ReceiveAllNote_Update( const INT32 nCount);


	void 			SetNoteCheck( INT32 idx);
	void 			SetNoteType( NOTE_TYPE type);
	void 			SetSelectedNote( INT32 idx);
	void 			ResetNote(void);
	void			MailDelete(INT32 row , INT32 col);
	void			SendMail();
	void			DeleteAll();
	void			TakeSelectedGift();
	void			SendEventTakeGift(UINT8 giftCount, T_GiftDBIdx* giftlist);
	void			OnDeleteNote(void);
	void			OnDeleteNoteAll(void);
	void			_UpdateNote(void);
	void			UpdateTakeButtonState(void);
	
	INT64			GetCurrentUserInfoUID() const		{ return m_CurrentUserInfoUID; }
	void			SetCurrentUserInfoUID(INT64 val)	{ m_CurrentUserInfoUID = val; }

	COMMUNITY_TAB	GetCurrentUserInfoType() const		{ return m_CurrentUserInfoType; }
	void			SetCurrentUserInfoType(COMMUNITY_TAB val) { m_CurrentUserInfoType = val; }

	static void		CbDeleteNote( void * pThis, INT32 nParam);
	static void		CbDeleteNoteAll( void * pThis, INT32 nParam);

	void			GetCheckBoxState(INT32 sel_idx);
	void			OnSelItems(INT32 sel_idx);
	void			OnOpenItem(INT32 sel_idx);
	void			OnScroll(INT32 first_idx);
};

#endif
