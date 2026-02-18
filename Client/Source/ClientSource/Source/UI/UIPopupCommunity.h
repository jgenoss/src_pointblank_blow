#if !defined( __UI_POPUP_COMMUNITY_H__)
#define __UI_POPUP_COMMUNITY_H__

#include "UIPopupBase.h"
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

class UIPopupCommunity : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupCommunity);

public:
	UIPopupCommunity();
	virtual ~UIPopupCommunity();

	virtual void	OnCreate( i3GameNode * pParent);

	//--------------------------------------------------------------------------//
	//									Note									//
	//--------------------------------------------------------------------------//
private:
	NOTE_TYPE		m_nNoteType;
	REAL32			m_fNoteEmptyRate;
	bool			m_bNoteCheck[MAX_NOTE_COUNT];
	bool			m_bAlarmCommunity;
	REAL32			m_rAlarmTerm;
	BOOL			m_bAlarmFlag;
	static BOOL		m_bWarningNoteCount;

	FRIEND_NODE			m_SelectedFriendNode;
	CLAN_MEMBER_NODE	m_SelectedClanMemberNode;

	INT32			m_PackageGoodsID;

	INT32			m_nTypeMessage[ NOTE_MESSAGE_MAX ];


	INT64			m_CurrentUserInfoUID;
	COMMUNITY_TAB	m_CurrentUserInfoType;

protected:
	void			_UpdateAlarmNote(REAL32 rDeltaSeconds);

public:
	/** \brief 깜박이기 */
	void			StartAlarmNote( void);
	void 			EndAlarmNote(void);
	void 			ReceiveAllNote( INT32 offset = 0);
	void 			SetNoteCheck( INT32 idx);
	void 			SetNoteType( NOTE_TYPE type);
	void 			SetSelectedNote( INT32 idx);
	void 			ResetNote(void);

	INT64			GetCurrentUserInfoUID() const		{ return m_CurrentUserInfoUID; }
	void			SetCurrentUserInfoUID(INT64 val)	{ m_CurrentUserInfoUID = val; }

	COMMUNITY_TAB	GetCurrentUserInfoType() const		{ return m_CurrentUserInfoType; }
	void			SetCurrentUserInfoType(COMMUNITY_TAB val) { m_CurrentUserInfoType = val; }
};

#endif
