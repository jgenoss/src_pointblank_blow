#if !defined( __UI_POPUP_RECEIVEMAIL_H__)
#define __UI_POPUP_RECEIVEMAIL_H__

#include "UIPopupBase.h"
#include "UIPopupMailBox.h"

class UIPopupReceiveMail : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupReceiveMail, UIPopupBase );

public:
	enum MAILTYPE
	{
		SEND = 0,
		READ,
		GIFT,
		REPLY,			// SEND 에서만 변경 됨
		CLAN_SEND,		// CLAN 전체 쪽지 메세지 
		CLAN_READ,		// CLAN 메세지 Read
		CLAN_READ_ASK,	// CLAN 초대 쪽지 Read

		MAX_MAILTYPE,
	};

	struct OPEN_DATA
	{
		MAILTYPE		mailType;

		int				noteIdx;
		int				noteAppendData;
		
		i3::rc_wstring	wstrNickName;
		i3::rc_wstring	wstrMessage;
		i3::rc_wstring	wstrAttachedItem;

		OPEN_DATA() : noteIdx( -1 ), mailType( SEND ), noteAppendData(0)
		{
		}
	};

private:
	OPEN_DATA	m_data;
	
	bool		m_bReOpen;
	bool		m_bCheckedID;

private:
	i3UIEditBox*		m_pEditNickName;
	i3UIEditBox*		m_pEditMessage;
	UIMessageBox*		m_pMsgboxWait;

public:
	void		CheckID();
	void		SendClanJoinAsk(bool bAccept);

private:
	bool		_SendMessage();
	bool		_SendClanMessage();

	bool		_SendGiftMessage(UINT8 ui8Type);

	void		_UpdateOpenData();
	void		_ProcessGameEvent_CheckID( INT32 arg );

public:
	virtual void		OnCreate( i3GameNode* pParent) override;
	
	/*
	param[in] pArg1 OPEN_DATA*
	*/
	virtual bool		OnEntranceStart( void* pArg1 = nullptr, void* pArg2 = nullptr ) override;
	virtual void		OnEntranceEnd() override;

	virtual void		OnExitEnd() override;

	virtual bool		OnKeyInput( i3InputDeviceManager * pInputMng ) override;
	virtual bool		OnOKButton() override;
	virtual bool		OnCloseButton() override;

	virtual bool		OnKey_Enter() override;
	virtual bool		OnKey_Escape() override;

	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code ) override;

public:
	virtual void		ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData) override;

	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;

protected:
	virtual void		_InitializeAtLoad( i3UIScene* pScene ) override;
	virtual void		_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIPopupReceiveMail();
	virtual ~UIPopupReceiveMail();
};

#endif