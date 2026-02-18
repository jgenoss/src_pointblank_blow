#include "i3InputType.h"
#include "i3InputKeyboard.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_DX)
#define I3I_BUFFER_SIZE		8  // arbitrary number of buffer elements

bool i3InputKeyboard::Create( void * pPlatformData)
{
	ResetAll();

	return true;
}

void i3InputKeyboard::ResetAll(void)
{
	i3mem::Fill( m_bPressInfo, 0, sizeof(m_bPressInfo));
	i3mem::Fill( m_bRepeatInfo, 0, sizeof(m_bRepeatInfo));
	i3mem::Fill( m_RepeatCount, 0, sizeof(m_RepeatCount));
	i3mem::Fill( m_bStrokeInfo, 0, sizeof(m_bStrokeInfo));
	i3mem::Fill( m_TimeStamp, 0, sizeof(m_TimeStamp));
}

static INT32		s_KeyMap[256] =
{
	-1,						// 0x00
	-1,						// #define VK_LBUTTON        0x01
	-1,						// #define VK_RBUTTON        0x02
	-1,						// #define VK_CANCEL         0x03
	-1,						// #define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

							// #if(_WIN32_WINNT >= 0x0500)
	-1,						// #define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
	-1,						// #define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
							// #endif /* _WIN32_WINNT >= 0x0500 */

							// /*
	-1,						// * 0x07 : unassigned
							//  */

	I3I_KEY_BACK,			// #define VK_BACK           0x08
	I3I_KEY_TAB,			// #define VK_TAB            0x09

							// /*
 	-1,						// * 0x0A - 0x0B : reserved
	-1,						// 
 							// */

 	-1,						// #define VK_CLEAR          0x0C
	I3I_KEY_ENTER,			// #define VK_RETURN         0x0D
	-1,						// 0x0E
	-1,						// 0x0F

	I3I_KEY_LSHIFT,			// #define VK_SHIFT          0x10
	I3I_KEY_LCTRL,			// #define VK_CONTROL        0x11
	I3I_KEY_LALT,			// #define VK_MENU           0x12
	I3I_KEY_BREAK,			// #define VK_PAUSE          0x13
	I3I_KEY_CAPSLOCK,		// #define VK_CAPITAL        0x14

	I3I_KEY_RALT,			// #define VK_HANGUL         0x15
	-1,						// 0x16
	-1,						// #define VK_JUNJA          0x17
	-1,						// #define VK_FINAL          0x18
	I3I_KEY_RCTRL,			// #define VK_HANJA          0x19
	-1,						// 0x1A

	I3I_KEY_ESC,			// #define VK_ESCAPE         0x1B

	-1,						// #define VK_CONVERT        0x1C
	-1,						// #define VK_NONCONVERT     0x1D
	-1,						// #define VK_ACCEPT         0x1E
	-1,						// #define VK_MODECHANGE     0x1F

	I3I_KEY_SPACE,			// #define VK_SPACE          0x20
	I3I_KEY_PGUP,			// #define VK_PRIOR          0x21
	I3I_KEY_PGDN,			// #define VK_NEXT           0x22
	I3I_KEY_END,			// #define VK_END            0x23
	I3I_KEY_HOME,			// #define VK_HOME           0x24
	I3I_KEY_LEFT,			// #define VK_LEFT           0x25
	I3I_KEY_UP,				// #define VK_UP             0x26
	I3I_KEY_RIGHT,			// #define VK_RIGHT          0x27
	I3I_KEY_DOWN,			// #define VK_DOWN           0x28
	-1,						// #define VK_SELECT         0x29
	I3I_KEY_PRTSC,			// #define VK_PRINT          0x2A
	-1,						// #define VK_EXECUTE        0x2B
	-1,						// #define VK_SNAPSHOT       0x2C
	I3I_KEY_INS,			// #define VK_INSERT         0x2D
	I3I_KEY_DEL,			// #define VK_DELETE         0x2E
	-1,						// #define VK_HELP           0x2F

/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

 	I3I_KEY_0,				// 0x30		'0'
 	I3I_KEY_1,				// 0x31		'1'
 	I3I_KEY_2,				// 0x32		'2'
 	I3I_KEY_3,				// 0x33		'3'
 	I3I_KEY_4,				// 0x34		'4'
	I3I_KEY_5,				// 0x35		'5'
	I3I_KEY_6,				// 0x36		'6'
	I3I_KEY_7,				// 0x37		'7'
	I3I_KEY_8,				// 0x38		'8'
	I3I_KEY_9,				// 0x39		'9'

	-1,						// 0x3A
	-1,						// 0x3B
	-1,						// 0x3C
	-1,						// 0x3D
	-1,						// 0x3E
	-1,						// 0x3F

	-1,						// 0x40
	I3I_KEY_A,				// 0x41		'A'
	I3I_KEY_B,				// 0x42		'B'
	I3I_KEY_C,				// 0x43		'C'
	I3I_KEY_D,				// 0x44		'D'
	I3I_KEY_E,				// 0x45		'E'
	I3I_KEY_F,				// 0x46		'F'
	I3I_KEY_G,				// 0x47		'G'
	I3I_KEY_H,				// 0x48		'H'
	I3I_KEY_I,				// 0x49		'I'
	I3I_KEY_J,				// 0x4A		'J'
	I3I_KEY_K,				// 0x4B		'K'
	I3I_KEY_L,				// 0x4C		'L'
	I3I_KEY_M,				// 0x4D		'M'
	I3I_KEY_N,				// 0x4E		'N'
	I3I_KEY_O,				// 0x4F		'O'
	I3I_KEY_P,				// 0x50		'P'
	I3I_KEY_Q,				// 0x51		'Q'
	I3I_KEY_R,				// 0x52		'R'
	I3I_KEY_S,				// 0x53		'S'
	I3I_KEY_T,				// 0x54		'T'
	I3I_KEY_U,				// 0x55		'U'
	I3I_KEY_V,				// 0x56		'V'
	I3I_KEY_W,				// 0x57		'W'
	I3I_KEY_X,				// 0x58		'X'
	I3I_KEY_Y,				// 0x59		'Y'
	I3I_KEY_Z,				// 0x5A		'Z'

	I3I_KEY_WIN,			// #define VK_LWIN           0x5B
	I3I_KEY_WIN,			// #define VK_RWIN           0x5C
	-1,						// #define VK_APPS           0x5D

							// /*
	-1,						// * 0x5E : reserved
							// */

	-1,						// #define VK_SLEEP          0x5F

	I3I_KEY_NUMPAD0,		// #define VK_NUMPAD0        0x60
	I3I_KEY_NUMPAD1,		// #define VK_NUMPAD1        0x61
	I3I_KEY_NUMPAD2,		// #define VK_NUMPAD2        0x62
	I3I_KEY_NUMPAD3,		// #define VK_NUMPAD3        0x63
	I3I_KEY_NUMPAD4,		// #define VK_NUMPAD4        0x64
	I3I_KEY_NUMPAD5,		// #define VK_NUMPAD5        0x65
	I3I_KEY_NUMPAD6,		// #define VK_NUMPAD6        0x66
	I3I_KEY_NUMPAD7,		// #define VK_NUMPAD7        0x67
	I3I_KEY_NUMPAD8,		// #define VK_NUMPAD8        0x68
	I3I_KEY_NUMPAD9,		// #define VK_NUMPAD9        0x69
	I3I_KEY_NUMPADMULTIPLY,	// #define VK_MULTIPLY       0x6A
	I3I_KEY_NUMPADPLUS,		// #define VK_ADD            0x6B
	-1,						// #define VK_SEPARATOR      0x6C
	I3I_KEY_NUMPADMINUS,	// #define VK_SUBTRACT       0x6D
	-1,						// #define VK_DECIMAL        0x6E
	I3I_KEY_NUMPADDIVIDE,	// #define VK_DIVIDE         0x6F
	I3I_KEY_F1,				// #define VK_F1             0x70
	I3I_KEY_F2,				// #define VK_F2             0x71
	I3I_KEY_F3,				// #define VK_F3             0x72
	I3I_KEY_F4,				// #define VK_F4             0x73
	I3I_KEY_F5,				// #define VK_F5             0x74
	I3I_KEY_F6,				// #define VK_F6             0x75
	I3I_KEY_F7,				// #define VK_F7             0x76
	I3I_KEY_F8,				// #define VK_F8             0x77
	I3I_KEY_F9,				// #define VK_F9             0x78
	I3I_KEY_F10,			// #define VK_F10            0x79
	I3I_KEY_F11,			// #define VK_F11            0x7A
	I3I_KEY_F12,			// #define VK_F12            0x7B
	-1,						// #define VK_F13            0x7C
	-1,						// #define VK_F14            0x7D
	-1,						// #define VK_F15            0x7E
	-1,						// #define VK_F16            0x7F
	-1,						// #define VK_F17            0x80
	-1,						// #define VK_F18            0x81
	-1,						// #define VK_F19            0x82
	-1,						// #define VK_F20            0x83
	-1,						// #define VK_F21            0x84
	-1,						// #define VK_F22            0x85
	-1,						// #define VK_F23            0x86
	-1,						// #define VK_F24            0x87

	/*
	* 0x88 - 0x8F : unassigned
	*/
	-1,						// 0x88
	-1,						// 0x89
	-1,						// 0x8A
	-1,						// 0x8B
	-1,						// 0x8C
	-1,						// 0x8D
	-1,						// 0x8E
	-1,						// 0x8F

	I3I_KEY_NUMLOCK,		// #define VK_NUMLOCK        0x90
	I3I_KEY_SCROLLLOCK,		// #define VK_SCROLL         0x91

	-1,						// 0x92
	-1,						// 0x93
	-1,						// 0x94
	-1,						// 0x95
	-1,						// 0x96

/*
 * 0x97 - 0x9F : unassigned
 */
	-1,						// 0x97
	-1,						// 0x98
	-1,						// 0x99
	-1,						// 0x9A
	-1,						// 0x9B
	-1,						// 0x9C
	-1,						// 0x9D
	-1,						// 0x9E
	-1,						// 0x9F

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
	I3I_KEY_LSHIFT,			// #define VK_LSHIFT         0xA0
	I3I_KEY_RSHIFT,			// #define VK_RSHIFT         0xA1
	I3I_KEY_LCTRL,			// #define VK_LCONTROL       0xA2
	I3I_KEY_RCTRL,			// #define VK_RCONTROL       0xA3
	I3I_KEY_LALT,			// #define VK_LMENU          0xA4
	I3I_KEY_RALT,			// #define VK_RMENU          0xA5

	-1,						// #define VK_BROWSER_BACK        0xA6
	-1,						// #define VK_BROWSER_FORWARD     0xA7
	-1,						// #define VK_BROWSER_REFRESH     0xA8
	-1,						// #define VK_BROWSER_STOP        0xA9
	-1,						// #define VK_BROWSER_SEARCH      0xAA
	-1,						// #define VK_BROWSER_FAVORITES   0xAB
	-1,						// #define VK_BROWSER_HOME        0xAC

	-1,						// #define VK_VOLUME_MUTE         0xAD
	-1,						// #define VK_VOLUME_DOWN         0xAE
	-1,						// #define VK_VOLUME_UP           0xAF
	-1,						// #define VK_MEDIA_NEXT_TRACK    0xB0
	-1,						// #define VK_MEDIA_PREV_TRACK    0xB1
	-1,						// #define VK_MEDIA_STOP          0xB2
	-1,						// #define VK_MEDIA_PLAY_PAUSE    0xB3
	-1,						// #define VK_LAUNCH_MAIL         0xB4
	-1,						// #define VK_LAUNCH_MEDIA_SELECT 0xB5
	-1,						// #define VK_LAUNCH_APP1         0xB6
	-1,						// #define VK_LAUNCH_APP2         0xB7

/*
 * 0xB8 - 0xB9 : reserved
 */
 	-1,						// 0xB8
	-1,						// 0xB9

	I3I_KEY_SEMICOLON,		// #define VK_OEM_1          0xBA   // ';:' for US
	I3I_KEY_EQUAL,			// #define VK_OEM_PLUS       0xBB   // '+' any country
	I3I_KEY_COMMA,			// #define VK_OEM_COMMA      0xBC   // ',' any country
	I3I_KEY_MINUS,			// #define VK_OEM_MINUS      0xBD   // '-' any country
	I3I_KEY_PERIOD,			// #define VK_OEM_PERIOD     0xBE   // '.' any country
	I3I_KEY_SLASH,			// #define VK_OEM_2          0xBF   // '/?' for US
	I3I_KEY_GRAVE,			// #define VK_OEM_3          0xC0   // '`~' for US

/*
 * 0xC1 - 0xD7 : reserved
 */
	-1,						// 0xC1
	-1,						// 0xC2
	-1,						// 0xC3
	-1,						// 0xC4
	-1,						// 0xC5
	-1,						// 0xC6
	-1,						// 0xC7
	-1,						// 0xC8
	-1,						// 0xC9
	-1,						// 0xCA
	-1,						// 0xCB
	-1,						// 0xCC
	-1,						// 0xCD
	-1,						// 0xCE
	-1,						// 0xCF
	-1,						// 0xD0
	-1,						// 0xD1
	-1,						// 0xD2
	-1,						// 0xD3
	-1,						// 0xD4
	-1,						// 0xD5
	-1,						// 0xD6
	-1,						// 0xD7

/*
 * 0xD8 - 0xDA : unassigned
 */
	-1,						// 0xD8
	-1,						// 0xD9
	-1,						// 0xDA

	I3I_KEY_LBRACKET,		// #define VK_OEM_4          0xDB  //  '[{' for US
	I3I_KEY_BACKSLASH,		// #define VK_OEM_5          0xDC  //  '\|' for US
	I3I_KEY_RBRACKET,		// #define VK_OEM_6          0xDD  //  ']}' for US
	I3I_KEY_APOSTROPHE,						// #define VK_OEM_7          0xDE  //  ''"' for US
	-1,						// #define VK_OEM_8          0xDF

/*
 * 0xE0 : reserved
 */
 	-1,						// 0xE0

/*
 * Various extended or enhanced keyboards
 */
	-1,						// #define VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
	-1,						// #define VK_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
	-1,						// #define VK_ICO_HELP       0xE3  //  Help key on ICO
	-1,						// #define VK_ICO_00         0xE4  //  00 key on ICO

	-1,						// #define VK_PROCESSKEY     0xE5

	-1,						// #define VK_ICO_CLEAR      0xE6

	-1,						// #define VK_PACKET         0xE7

/*
 * 0xE8 : unassigned
 */
 	-1,						// 0xE8

/*
 * Nokia/Ericsson definitions
 */
	-1,						// #define VK_OEM_RESET      0xE9
	-1,						// #define VK_OEM_JUMP       0xEA
	-1,						// #define VK_OEM_PA1        0xEB
	-1,						// #define VK_OEM_PA2        0xEC
	-1,						// #define VK_OEM_PA3        0xED
	-1,						// #define VK_OEM_WSCTRL     0xEE
	-1,						// #define VK_OEM_CUSEL      0xEF
	-1,						// #define VK_OEM_ATTN       0xF0
	-1,						// #define VK_OEM_FINISH     0xF1
	-1,						// #define VK_OEM_COPY       0xF2
	-1,						// #define VK_OEM_AUTO       0xF3
	-1,						// #define VK_OEM_ENLW       0xF4
	-1,						// #define VK_OEM_BACKTAB    0xF5

	-1,						// #define VK_ATTN           0xF6
	-1,						// #define VK_CRSEL          0xF7
	-1,						// #define VK_EXSEL          0xF8
	-1,						// #define VK_EREOF          0xF9
	-1,						// #define VK_PLAY           0xFA
	-1,						// #define VK_ZOOM           0xFB
	-1,						// #define VK_NONAME         0xFC
	-1,						// #define VK_PA1            0xFD
	-1,						// #define VK_OEM_CLEAR      0xFE
	-1						// 0xFF
};

static UINT32 s_UpdateKeyTable[][2] =
{
	{	VK_LMENU,		I3I_KEY_LALT },
	{	VK_RMENU,		I3I_KEY_RALT },
	{	VK_LCONTROL,	I3I_KEY_LCTRL },
	{	VK_RCONTROL,	I3I_KEY_RCTRL },
	{	VK_LSHIFT,		I3I_KEY_LSHIFT },
	{	VK_RSHIFT,		I3I_KEY_RSHIFT },
	{	0,				0	}
};

void i3InputKeyboard::Update( REAL32 time)
{
	/*
	// ALT키 특별 취급
	for( i = 0; s_UpdateKeyTable[i][0] > 0; i++)
	{
		SHORT keyAlt = GetAsyncKeyState( s_UpdateKeyTable[i][0]);
		INT32 idxKey = s_UpdateKeyTable[i][1];

		if( keyAlt & 0x8000)
		{
			m_bStrokeInfo[ idxKey] = !m_bPressInfo[ idxKey];
			m_bPressInfo[ idxKey] = true;
		}
		else
		{
			m_bPressInfo[ idxKey] = false;
		}
	}
	*/

	for( INT32 i = 0; i < I3I_KEY_MAX; i++)
	{
		if( m_bPressInfo[i])
		{
			UINT32 vKey = FindVKByI3IKey( (I3I_KEY) i);

			if( (GetAsyncKeyState( vKey) & 0x8000) == 0)
			{
				//
				m_bPressInfo[ i] = false;
				m_bStrokeInfo[ i] = false;
			}
		}
	}

	/*
	아래의 코드는 매우 심각한 누적 증상을 가져온다.
	- TCP의 속도가 느려짐.
	- 회를 거듭할수록 전반적인 속도가 느려짐.
	MSG msg;

	while( ::PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
	{
		::TranslateMessage( &msg );
        ::DispatchMessage( &msg );
	}
	*/
}

void i3InputKeyboard::Reset(void)
{
	// Stroke State들은 모두 false로 되돌려준다.
	INT32 i;

	for( i = 0; i < I3I_KEY_MAX; i++)
	{
		m_bStrokeInfo[ i] = false;
	}
}

void i3InputKeyboard::OnKeyDown( UINT32 nKey)
{
	INT32 key = s_KeyMap[ nKey & 0xFF];

	if( key != -1)
	{
		m_bStrokeInfo[ key] = !m_bPressInfo[ key];
		m_bPressInfo[ key] = true;
	}
}

void i3InputKeyboard::OnKeyUp( UINT32 nKey)
{
	INT32 key = s_KeyMap[ nKey & 0xFF];

	if( key != -1)
	{
		m_bPressInfo[ key] = false;
		m_bStrokeInfo[ key] = false;
	}
}

INT32 i3InputKeyboard::FindVKByI3IKey(I3I_KEY key)
{
	for(UINT32 i = 0; i < 256; i++)
	{
        if (s_KeyMap[i] == key)
		{
			return i;
		}
	}	

	return -1;
}

#endif
