#if !defined( __UI_POPUP_GIFT_SEND__)
#define __UI_POPUP_GIFT_SEND__

#include "UIPopupBase.h"

struct CLAN_MEMBER_NODE;
struct FRIEND_NODE;
class UIImgSetCtrl;

class UIPopupGiftSend : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupGiftSend, UIPopupBase);

private:
	enum { TAB_FRIEND = 0, TAB_CLAN = 1, LISTVIEW_MAX = 6, };

	struct GiftTargetUser
	{
		INT64			_i64UID;
		UINT8			_Rank;
		i3::rc_wstring	_wstrNick;
		FRIEND_STATE	_FriendState;
		I3COLOR			_NickColor;

		GiftTargetUser(void)
		{
			_i64UID			= 0;
			_Rank			= 0;
			_FriendState	= FRIEND_STATE_REQUEST;
			i3Color::Set( &_NickColor, (UINT8) 255, 255, 255, 255);
		}

		void	CopyFrom( const CLAN_MEMBER_NODE * pSrc);
		void	CopyFrom( const FRIEND_NODE * pSrc);

		void	SetNickColot(void);
	};

private:
	i3::vector<GiftTargetUser>	m_TargetUserList;

	INT32			m_i32Tab;	
	INT32			m_i32SelectIdx;

	bool			m_isDefaultMsg;
	
	UIImgSetCtrl	*		m_pItemImgSet;
	i3UIEditBox		*		m_pEditBox;
	i3UIButtonComposed3	*	m_pOKButton;

private:
	void		_InitGiftPopup(void);
	void		_SetGiftItemInfo(void);
	void		_SetClanTabActive(void);

	void		_UpdateUserList(void);
	
	void		_BuildFriendList(void);
	void		_BuildClanList(void);

	void		_SetUserSlot(void);
	void		_SetUserData(INT32 idx, GiftTargetUser * pInfo);

	bool		_IsFilter(const i3::wliteral_range& wMessageRng);

protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

public:
	UIPopupGiftSend(void);
	virtual ~UIPopupGiftSend(void);

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */) override;
	virtual void	OnEntranceEnd(void) override;

public:
	void			ClickTab(INT32 i32Tab);
	void			SetEditFocus(bool isSetFocus);
	void			SetSelectIdx(INT32 i32SelectIdx);

	void			SendGift(void);
	void			OnClose(void);
};

#endif // __UI_POPUP_GIFT_SEND__
