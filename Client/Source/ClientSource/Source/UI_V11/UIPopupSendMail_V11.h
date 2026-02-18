#if !defined( __UI_POPUP_USERACTIONINTERACTSENDMAIL_H__)
#define __UI_POPUP_USERACTIONINTERACTSENDMAIL_H__

#include "../UI/UIPopupInteractBase.h"

class UIPopupSendMail_V11 : public UIPopupInteractBase
{
	I3_CLASS_DEFINE( UIPopupSendMail_V11, UIPopupInteractBase);

private:
	i3UIEditBox *			m_pWho;
	i3UIEditBox *			m_pNote;
	i3UIButtonComposed3 *	m_pSearch;

	bool			m_bReceiverUIDFlag;
	INT32			m_iListIdx;				// 답장하기에 사용

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

#if defined( USE_MESS_TEST)
	INT64			_FindNoteByUid( const char* nick);
#endif
public:
	UIPopupSendMail_V11();
	virtual ~UIPopupSendMail_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg2 INT32*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnOKButton( void) override;
	virtual bool	OnKey_Enter( void) override;
	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;
	void			CheckUser( void);
};

#endif
