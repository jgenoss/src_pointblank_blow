#include "pch.h"

#if defined( TEST_AUTOMATION)

namespace CaseRun
{
	
	const wchar_t *	s_KeyName[] =
	{
		L"KEY_0",		// I3I_KEY_0				= 0,
		L"KEY_1",		// I3I_KEY_1,
		L"KEY_2",		// I3I_KEY_2,
		L"KEY_3",		// I3I_KEY_3,
		L"KEY_4",		// I3I_KEY_4,
		L"KEY_5",		// I3I_KEY_5,
		L"KEY_6",		// I3I_KEY_6,
		L"KEY_7",		// I3I_KEY_7,
		L"KEY_8",		// I3I_KEY_8,
		L"KEY_9",		// I3I_KEY_9,

		L"KEY_A",		// I3I_KEY_A,
		L"KEY_B",		// I3I_KEY_B,
		L"KEY_C",		// I3I_KEY_C,
		L"KEY_D",		// I3I_KEY_D,
		L"KEY_E",		// I3I_KEY_E,
		L"KEY_F",		// I3I_KEY_F,
		L"KEY_G",		// I3I_KEY_G,
		L"KEY_H",		// I3I_KEY_H,
		L"KEY_I",		// I3I_KEY_I,
		L"KEY_J",		// I3I_KEY_J,
		L"KEY_K",		// I3I_KEY_K,
		L"KEY_L",		// I3I_KEY_L,
		L"KEY_M",		// I3I_KEY_M,
		L"KEY_N",		// I3I_KEY_N,
		L"KEY_O",		// I3I_KEY_O,
		L"KEY_P",		// I3I_KEY_P,
		L"KEY_Q",		// I3I_KEY_Q,
		L"KEY_R",		// I3I_KEY_R,
		L"KEY_S",		// I3I_KEY_S,
		L"KEY_T",		// I3I_KEY_T,
		L"KEY_U",		// I3I_KEY_U,
		L"KEY_V",		// I3I_KEY_V,
		L"KEY_W",		// I3I_KEY_W,
		L"KEY_X",		// I3I_KEY_X,
		L"KEY_Y",		// I3I_KEY_Y,
		L"KEY_Z",		// I3I_KEY_Z,

		L"KEY_ESC",		// I3I_KEY_ESC,
		L"KEY_Enter",	// I3I_KEY_ENTER,
		L"KEY_LCtrl",	// I3I_KEY_LCTRL,
		L"KEY_RCtrl",	// I3I_KEY_RCTRL,
		L"KEY_LShift",	// I3I_KEY_LSHIFT,
		L"KEY_RShift",	// I3I_KEY_RSHIFT,
		L"KEY_LAlt",	// I3I_KEY_LALT,
		L"KEY_RAlt",	// I3I_KEY_RALT,
		L"KEY_Space",	// I3I_KEY_SPACE,
		L"KEY_Left",	// I3I_KEY_LEFT,
		L"KEY_Right",	// I3I_KEY_RIGHT,
		L"KEY_Up",		// I3I_KEY_UP,
		L"KEY_Down",	// I3I_KEY_DOWN,
		L"KEY_PaUp",	// I3I_KEY_PGUP,
		L"KEY_PaDn",// I3I_KEY_PGDN,
		L"KEY_Home",	// I3I_KEY_HOME,
		L"KEY_End",		// I3I_KEY_END,
		L"KEY_Del",	// I3I_KEY_DEL,
		L"KEY_Ins",	// I3I_KEY_INS,
		L"KEY_Tab",		// I3I_KEY_TAB,
		L"KEY_NumLock",	// I3I_KEY_NUMLOCK,
		L"KEY_CapsLock",	// I3I_KEY_CAPSLOCK,
		L"KEY_ScrollLock",	// I3I_KEY_SCROLLLOCK,
		L"KEY_Break",	// I3I_KEY_BREAK,
		L"KEY_PrtSc",	// I3I_KEY_PRTSC,
		L"KEY_Win",		// I3I_KEY_WIN,
		L"KEY_Back",	// I3I_KEY_BACK,

		L"KEY_F1",		// I3I_KEY_F1,
		L"KEY_F2",		// I3I_KEY_F2,
		L"KEY_F3",		// I3I_KEY_F3,
		L"KEY_F4",		// I3I_KEY_F4,
		L"KEY_F5",		// I3I_KEY_F5,
		L"KEY_F6",		// I3I_KEY_F6,
		L"KEY_F7",		// I3I_KEY_F7,
		L"KEY_F8",		// I3I_KEY_F8,
		L"KEY_F9",		// I3I_KEY_F9,
		L"KEY_F10",		// I3I_KEY_F10,
		L"KEY_F11",		// I3I_KEY_F11,
		L"KEY_F12",		// I3I_KEY_F12,

		L"KEY_Numpad0",	// I3I_KEY_NUMPAD0,
		L"KEY_Numpad1",	// I3I_KEY_NUMPAD1,
		L"KEY_Numpad2",	// I3I_KEY_NUMPAD2,
		L"KEY_Numpad3",	// I3I_KEY_NUMPAD3,
		L"KEY_Numpad4",	// I3I_KEY_NUMPAD4,
		L"KEY_Numpad5",	// I3I_KEY_NUMPAD5,
		L"KEY_Numpad6",	// I3I_KEY_NUMPAD6,
		L"KEY_Numpad7",	// I3I_KEY_NUMPAD7,
		L"KEY_Numpad8",	// I3I_KEY_NUMPAD8,
		L"KEY_Numpad9",	// I3I_KEY_NUMPAD9,
		L"KEY_NumpadDivide",	// I3I_KEY_NUMPADDIVIDE,
		L"KEY_NumpadMultiply",	// I3I_KEY_NUMPADMULTIPLY,
		L"KEY_NumpadMinus",	// I3I_KEY_NUMPADMINUS,
		L"KEY_NumpadPlus",	// I3I_KEY_NUMPADPLUS,
		L"KEY_NumpadEnter",	// I3I_KEY_NUMPADENTER,
		L"KEY_NumpadDel",	// I3I_KEY_NUMPADDEL,

		L"KEY_Minus",		// I3I_KEY_MINUS,
		L"KEY_Equal",		// I3I_KEY_EQUAL,
		L"KEY_BackSlash",		// I3I_KEY_BACKSLASH,
		L"KEY_LBracket",		// I3I_KEY_LBRACKET,
		L"KEY_RBracket",		// I3I_KEY_RBRACKET,
		L"KEY_Semicolon",		// I3I_KEY_SEMICOLON,
		L"KEY_Apostrophe",		// I3I_KEY_APOSTROPHE,
		L"KEY_Comma",		// I3I_KEY_COMMA,
		L"KEY_Period",		// I3I_KEY_PERIOD,
		L"KEY_Slash",		// I3I_KEY_SLASH,
		L"KEY_Grave",		// I3I_KEY_GRAVE,
	};

	I3I_KEY GetKey( const wchar_t * pszName)
	{
		for (INT32 i = 0; i < I3I_KEY_MAX; i++)
		{
			if (wcsicmp(s_KeyName[i], pszName) == 0)
				return (I3I_KEY)i;
		}

		return I3I_KEY_UNKNOWN;
	}

	const wchar_t * GetKeyName(I3I_KEY key)
	{
		if ((key < 0) || (key >= I3I_KEY_MAX))
			return nullptr;
		
		return s_KeyName[key];
	}
}
#endif
