#if !defined( __UI_POPUP_RECEIVEMAIL_V11_H__)
#define __UI_POPUP_RECEIVEMAIL_V11_H__

#include "UI/UIPopupBase.h"
#include "UIPopupMailBox_V11.h"

class UIPopupReceiveMail_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupReceiveMail_V11, UIPopupBase);

private:
	MessageTab		m_CurrentTab;
	INT32			m_SelectedIndex = 0;	// 쪽지함 리스트의 인덱스
	UINT32			m_PackageGoodsID;	// 아이템 ID

	INT32			m_nTypeMessage[ NOTE_MESSAGE_MAX ];
	GIFT_BUFFER*	m_pGiftInfo;

protected:
	void			_GetPresentMessage( i3::rc_wstring* pwstrOut, const size_t sizeOutMax, NOTE_RECEIVE* pNote);
	void			_SetData( void);

public:
	UIPopupReceiveMail_V11();
	virtual ~UIPopupReceiveMail_V11();
	
	void			Accpet_Join_Clan();
	void			Deny_Join_Clan();
	void			Reply();
	void			ReceiveGift();
		
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 (MessageTab)(INT32)
	param[in] pArg2 INT32
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnCloseButton( void) override;

	virtual void	ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData) override;
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

	void			SetSelectedNote( NOTE_RECEIVE* pNote );
	void			ReceivePresent( NOTE_RECEIVE* pNote );

	void			OnNoteAccept(bool bVal);
};

#endif
