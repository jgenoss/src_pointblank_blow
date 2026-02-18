#include "pch.h"
#include "UIPopupAttendanceBase.h"

#include "UIUtil.h"
#include "ItemConsumer.h"

I3_CLASS_INSTANCE(UIPopupAttendanceBase);

UIPopupAttendanceBase::UIPopupAttendanceBase()
{

}

UIPopupAttendanceBase::~UIPopupAttendanceBase()
{

}

void UIPopupAttendanceBase::SetPeriod(i3::rc_wstring& out, UINT32 ui32StartDate, UINT32 ui32EndDate, bool IsNext/*= false*/)
{
	if (ui32StartDate == 0 || ui32EndDate == 0)
	{
		out = L"";
		return;
	}

	i3::stack_wstring wstrStartMonth;
	i3::stack_wstring wstrStartDay;
	i3::stack_wstring wstrEndMonth;
	i3::stack_wstring wstrEndDay;

	// 시작날짜
	{
		INT32 i32Month, i32Day;

		i32Month = ui32StartDate / 10000;
		i32Month = i32Month % 10000;
		i32Day = i32Month % 100;
		i32Month = i32Month / 100;

		i3::sprintf(wstrStartMonth, L"%d", (INT32)i32Month);
		i3::sprintf(wstrStartDay, L"%d", (INT32)i32Day);
	}

	// 종료날짜
	{
		INT32 i32Month, i32Day;

		i32Month = ui32EndDate / 10000;
		i32Month = i32Month % 10000;
		i32Day = i32Month % 100;
		i32Month = i32Month / 100;

		i3::sprintf(wstrEndMonth, L"%d", (INT32)i32Month);
		i3::sprintf(wstrEndDay, L"%d", (INT32)i32Day);
	}

	if (IsNext)
	{
		i3::sprintf(out, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_NEXT_ATTENDANCE"),
			wstrStartMonth,
			wstrStartDay,
			wstrEndMonth,
			wstrEndDay);

	}
	else
	{
		i3::sprintf(out, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_PERIOD_SCREEN"),
			wstrStartMonth,
			wstrStartDay,
			wstrEndMonth,
			wstrEndDay);
	}

}

void UIPopupAttendanceBase::SetStaticText(i3::string findname, i3::rc_wstring& text)
{
	i3UIScene * pScene = GetScene(0);
	if (pScene == nullptr) return;

	i3UIStaticText * UIText = (i3UIStaticText *)pScene->FindChildByName(findname.c_str());
	if (UIText == nullptr) return;

	UIText->SetText(text);
}

void UIPopupAttendanceBase::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupBase::ProcessGameEvent(evt, arg, UserData);

	if (evt == EVENT_ATTENDANCE_CHECK)
	{
		ReceiveGameEvent_Attendance_Check(arg, UserData);
	}
}

void UIPopupAttendanceBase::SetErrorMsg(INT32 arg)
{
	i3::rc_wstring wstrString;
	switch (arg)
	{
	case EVENT_ERROR_ATTENDANCE_PARAMETER_USER:
		wstrString = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_PARAMETER_USER");
		break;
	case EVENT_ERROR_ATTENDANCE_NOT_ENOUGH:
		wstrString = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_NOT_ENOUGH");
		break;
	case EVENT_ERROR_ATTENDANCE_ALREADY_COMPLETED:
		wstrString = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_ALREADY_CHECK");
		break;
	case EVENT_ERROR_ATTENDANCE_WRONG_VERSION:
		wstrString = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_WRONG_VERSION");
		break;		
	case EVENT_ERROR_ATTENDANCE_UNKWON:
	default:
		wstrString = GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_FAIL_UNKNOWN");
		break;
	}

	GameUI::MsgBox(MSG_TYPE_ERROR, wstrString);
}