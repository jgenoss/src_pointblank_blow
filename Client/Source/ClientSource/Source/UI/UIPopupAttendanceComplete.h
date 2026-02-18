#if !defined( __UI_POPUP_ATTENDANCECOMPLETE_H__)
#define __UI_POPUP_ATTENDANCECOMPLETE_H__

#include "UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupAttendanceComplete : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupAttendanceComplete, UIPopupBase);

public:
	UIPopupAttendanceComplete();
	virtual ~UIPopupAttendanceComplete();

	virtual void OnCreate ( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes(void) override;
	virtual void OnUnloadAllScenes(void) override;
	virtual void OnEntranceEnd(void) override;
	virtual void OnExitEnd(void) override;

	virtual bool OnEntranceStart( void * pArg1  = nullptr , void * pArg2  = nullptr ) override;

	virtual bool OnKey_Enter() override {return false;}
	virtual bool OnKey_Escape() override {return false;}

public:
	void ReceiveItem(void);

protected:
	void _SetEventData(void);
	void _SetItemInfo(i3::rc_wstring& out, UIImgSetCtrl * pCtrl, INT32 nGoodsID);

private:
	INT32 m_nItemCount;
	UIImgSetCtrl * m_pImgCtrl[2];

	bool m_bRes;
	void _set_default_selection();
};

#endif