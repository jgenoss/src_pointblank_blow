#if !defined(_UI_POPUP_MONTH_ATTENDANCE_H_)
#define _UI_POPUP_MONTH_ATTENDANCE_H_

#include "UIPopupBase.h"
#include "UIPopupAttendanceBase.h"

class UIImgSetCtrl;

class UIPopupMonthAttendance : public UIPopupAttendanceBase
{
	I3_CLASS_DEFINE( UIPopupMonthAttendance, UIPopupAttendanceBase);

public:
	UIPopupMonthAttendance();
	virtual ~UIPopupMonthAttendance();

public:
	virtual void OnCreate ( i3GameNode * pParent) override;
	virtual bool OnEntranceStart(void * pArg1 = nullptr  , void * pArg2 = nullptr ) override;
	virtual void ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData);

public:
	void		 ClickButton();
	void		 SelectReward(INT32 idx);

protected:
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

private:
	UIImgSetCtrl *			m_pImgCtrl[2];
	i3UIStaticText *		m_pRewardText[2];
	i3UIButtonImageSet *	m_pRewardIcon[2];

	i3::vector<i3UIButtonImageSet *> m_vMonthItemBG;

	INT32					m_RewardItemIdx;
	INT32					m_TodayAttendance;
	INT32					m_ViewItemIdx;
	UINT8					m_EventState;

protected:
	void			_SetEventData();

	void			_SetStaticText(i3UIScene * pScene);

	void			_SetViewMonthItem();
	void			_SetViewItemBG(INT32 idx, INT32 shape_type);
	void			_SetViewTodayItem();
	void			__SetTodayItemIcon(ATTENDANCE_CHOICE_ITEM& item_, bool IsviewOnly);

	void			_SetSuccessUI();
	void			_SetSuccessEvent();

};

#endif