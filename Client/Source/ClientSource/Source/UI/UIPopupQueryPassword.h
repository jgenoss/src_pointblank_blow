#if !defined( __UI_POPUP_QUERYPASSWORD_H__)
#define __UI_POPUP_QUERYPASSWORD_H__

#include "UIPopupBase.h"

typedef void (*PQPCallBack)(bool OK, const i3::string & Password, void * pUserContext);

struct QPParam
{
	i3::rc_wstring	wstrMsg;
	bool			OnlyNumber;
	bool			ShowPassword;
	INT32			LimitCount;
	PQPCallBack		Callback;
	void *			pUserContext;

	QPParam()
	{
		OnlyNumber		= false;
		ShowPassword	= false;
		LimitCount		= 0;
		Callback		= nullptr;
		pUserContext	= nullptr;
	}
};

class UIPopupQueryPassword : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupQueryPassword, UIPopupBase);

private:
	PQPCallBack		m_pCallback;
	void *			m_pUserContext;

	i3UIEditBox *	m_pED;

	bool			m_bSendEnterRoom;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);

	void			_SendEnterRoom(void);
public:
	UIPopupQueryPassword();
	virtual ~UIPopupQueryPassword();

	virtual void	OnCreate( i3GameNode * pParent) override;

	/*
	param[in] pArg1 const QPParam *
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;

	virtual void	OnExitEnd(void) override;

	virtual bool	OnOKButton( void) override;

};

#endif
