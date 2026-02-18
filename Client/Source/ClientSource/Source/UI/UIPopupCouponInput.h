#if !defined( __UI_POPUP_COUPONINPUT_H__)
#define __UI_POPUP_COUPONINPUT_H__

#include "UIPopupBase.h"

#define COUPON_SLOT	4

// Letter Count가 0일때 삭제 시도, 혹은 5일때 입력 시도시
// OnEvent 자체가 들어오지 않기 때문에
// 쿠폰 입력 팝업에 한하여, 코드에서 강제로 제어해준다. (2011.12.21 양승천)
#define COUPON_MAX_LETTER_COUNT 5	

class UIPopupCouponInput : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupCouponInput, UIPopupBase);

public:
	UIPopupCouponInput();
	virtual ~UIPopupCouponInput();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;
public:
	void			Confirm();
	void			NotifyFail(INT32);
	void			NotifySuccess();
	void			Cancel();
	void			AutoFocusing(UINT32 nEditState);
	virtual bool	OnKey_Enter() override { Confirm(); return true; }
	virtual bool	OnKey_Escape() override { Cancel(); return true;  }

private:
	bool			m_bWaitingConfirmSubscripting;
	INT32			m_nOldLength;
	WCHAR			m_szCurEditText[COUPON_MAX_LETTER_COUNT];	

	i3UIEditBox *	m_pCouponBox;

	void			ReceiveGameEvent_Coupon_Award( INT32 arg, const i3::user_data&);
	
};

#endif