#if !defined( _UI_POPUP_ATTENDANCE_BASE_H_)
#define _UI_POPUP_ATTENDANCE_BASE_H_

#include "UIPopupBase.h"

class UIImgSetCtrl;

class UIPopupAttendanceBase : public UIPopupBase
{
	I3_CLASS_DEFINE(UIPopupAttendanceBase, UIPopupBase);
public:
	UIPopupAttendanceBase();
	virtual ~UIPopupAttendanceBase();

public:
	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

protected:
	// 이벤트 기간을 파싱하여 각각의 스트링에 맞게 세팅.
	void				SetPeriod(i3::rc_wstring& out, UINT32 ui32StartDate, UINT32 ui32EndDate, bool IsNext = false);

	// i3UiStaticText 형식의 UI Control 을 찾아 텍스트를 세팅
	void				SetStaticText(i3::string findname, i3::rc_wstring& text);
	void				SetErrorMsg(INT32 arg);

protected:
	virtual	void		ReceiveGameEvent_Attendance_Check(INT32 arg, const i3::user_data &UserData) = 0;

protected:
	enum { MONTH_MAX_COUNT = 31, };
	enum
	{
		CLEAR_ATTENDANCE = 0,
		TODAY_ATTENDANCE,
		NEXT_ATTENDANCE,

		MAX_ATTENDANCE_COUNT,
	};

};

#endif