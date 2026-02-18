#include "i3InputType.h"
#include "i3InputKeyboard.h"

//I3_CLASS_INSTANCE( i3InputKeyboard, i3InputDevice);
I3_CLASS_INSTANCE( i3InputKeyboard);

static const char *	s_KeyName[] =
{
	"0",		// I3I_KEY_0				= 0,
	"1",		// I3I_KEY_1,
	"2",		// I3I_KEY_2,
	"3",		// I3I_KEY_3,
	"4",		// I3I_KEY_4,
	"5",		// I3I_KEY_5,
	"6",		// I3I_KEY_6,
	"7",		// I3I_KEY_7,
	"8",		// I3I_KEY_8,
	"9",		// I3I_KEY_9,

	"A",		// I3I_KEY_A,
	"B",		// I3I_KEY_B,
	"C",		// I3I_KEY_C,
	"D",		// I3I_KEY_D,
	"E",		// I3I_KEY_E,
	"F",		// I3I_KEY_F,
	"G",		// I3I_KEY_G,
	"H",		// I3I_KEY_H,
	"I",		// I3I_KEY_I,
	"J",		// I3I_KEY_J,
	"K",		// I3I_KEY_K,
	"L",		// I3I_KEY_L,
	"M",		// I3I_KEY_M,
	"N",		// I3I_KEY_N,
	"O",		// I3I_KEY_O,
	"P",		// I3I_KEY_P,
	"Q",		// I3I_KEY_Q,
	"R",		// I3I_KEY_R,
	"S",		// I3I_KEY_S,
	"T",		// I3I_KEY_T,
	"U",		// I3I_KEY_U,
	"V",		// I3I_KEY_V,
	"W",		// I3I_KEY_W,
	"X",		// I3I_KEY_X,
	"Y",		// I3I_KEY_Y,
	"Z",		// I3I_KEY_Z,

	"ESC",		// I3I_KEY_ESC,
	"Enter",	// I3I_KEY_ENTER,
	"LCtrl",	// I3I_KEY_LCTRL,
	"RCtrl",	// I3I_KEY_RCTRL,
	"LShift",	// I3I_KEY_LSHIFT,
	"RShift",	// I3I_KEY_RSHIFT,
	"LAlt",	// I3I_KEY_LALT,
	"RAlt",	// I3I_KEY_RALT,
	"Space",	// I3I_KEY_SPACE,
	"Left",	// I3I_KEY_LEFT,
	"Right",	// I3I_KEY_RIGHT,
	"Up",		// I3I_KEY_UP,
	"Down",	// I3I_KEY_DOWN,
	"Page Up",	// I3I_KEY_PGUP,
	"Page Down",// I3I_KEY_PGDN,
	"Home",	// I3I_KEY_HOME,
	"End",		// I3I_KEY_END,
	"Delete",	// I3I_KEY_DEL,
	"Insert",	// I3I_KEY_INS,
	"Tab",		// I3I_KEY_TAB,
	"Number Lock",	// I3I_KEY_NUMLOCK,
	"Caps Lock",	// I3I_KEY_CAPSLOCK,
	"Scroll Lock",	// I3I_KEY_SCROLLLOCK,
	"Break",	// I3I_KEY_BREAK,
	"Prt Sc",	// I3I_KEY_PRTSC,
	"Win",		// I3I_KEY_WIN,
	"Back",	// I3I_KEY_BACK,

	"F1",		// I3I_KEY_F1,
	"F2",		// I3I_KEY_F2,
	"F3",		// I3I_KEY_F3,
	"F4",		// I3I_KEY_F4,
	"F5",		// I3I_KEY_F5,
	"F6",		// I3I_KEY_F6,
	"F7",		// I3I_KEY_F7,
	"F8",		// I3I_KEY_F8,
	"F9",		// I3I_KEY_F9,
	"F10",		// I3I_KEY_F10,
	"F11",		// I3I_KEY_F11,
	"F12",		// I3I_KEY_F12,

	"Numpad 0",	// I3I_KEY_NUMPAD0,
	"Numpad 1",	// I3I_KEY_NUMPAD1,
	"Numpad 2",	// I3I_KEY_NUMPAD2,
	"Numpad 3",	// I3I_KEY_NUMPAD3,
	"Numpad 4",	// I3I_KEY_NUMPAD4,
	"Numpad 5",	// I3I_KEY_NUMPAD5,
	"Numpad 6",	// I3I_KEY_NUMPAD6,
	"Numpad 7",	// I3I_KEY_NUMPAD7,
	"Numpad 8",	// I3I_KEY_NUMPAD8,
	"Numpad 9",	// I3I_KEY_NUMPAD9,
	"Numpad /",	// I3I_KEY_NUMPADDIVIDE,
	"Numpad *",	// I3I_KEY_NUMPADMULTIPLY,
	"Numpad -",	// I3I_KEY_NUMPADMINUS,
	"Numpad +",	// I3I_KEY_NUMPADPLUS,
	"Numpad Enter",	// I3I_KEY_NUMPADENTER,
	"Numpad Delete",	// I3I_KEY_NUMPADDEL,

	"-",		// I3I_KEY_MINUS,
	"=",		// I3I_KEY_EQUAL,
	"\\",		// I3I_KEY_BACKSLASH,
	"[",		// I3I_KEY_LBRACKET,
	"]",		// I3I_KEY_RBRACKET,
	";",		// I3I_KEY_SEMICOLON,
	"'",		// I3I_KEY_APOSTROPHE,
	",",		// I3I_KEY_COMMA,
	".",		// I3I_KEY_PERIOD,
	"/",		// I3I_KEY_SLASH,
	"`",		// I3I_KEY_GRAVE,
};

const char * i3InputKeyboard::GetKeyName( I3I_KEY key)
{
	if((key >= 0) && (key < I3I_KEY_MAX))
	{
		return s_KeyName[ key];
	}
	else
	{
		return "N/A";
	}
}