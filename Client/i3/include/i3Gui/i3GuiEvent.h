#if !defined( __I3_GUI_EVENT_H)
#define __I3_GUI_EVENT_H

namespace i3GUI
{
	enum EVENT
	{
		// 마우스가 해당 Control의 영역 안으로 들어왔을 때 전달된다.
		// param1 : i3InputMouse *
		// param2 : unused
		EVT_ENTERMOUSE = 1,

		// 마우스가 Control의 영역 안에 있다가 바깥으로 나가는 경우 전달된다.
		// param1 : i3InputMouse *
		// param2 : unused
		EVT_LEAVEMOUSE,

		EVT_SETFOCUS,
		EVT_KILLFOCUS,
	};
};

#endif
