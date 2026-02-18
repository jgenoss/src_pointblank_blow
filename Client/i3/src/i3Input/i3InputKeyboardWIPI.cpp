#include "i3InputType.h"
#include "i3InputKeyboard.h"
#include "i3InputGlobalVariable.h"

#if defined( I3_WIPI)

//	폰일경우 방향키 재정의
#if 0
	#define EVENT_KEY_UP					MH_KEY_LEFT							//	방향키
	#define EVENT_KEY_DOWN					MH_KEY_RIGHT
	#define EVENT_KEY_LEFT					MH_KEY_DOWN
	#define EVENT_KEY_RIGHT					MH_KEY_UP
#else	
//	에뮬일경우 방향키 재정의
	#ifdef I3_WIPI_KTF_G1000
		#define EVENT_KEY_UP					MH_KEY_LEFT							//	방향키
		#define EVENT_KEY_DOWN					MH_KEY_RIGHT
		#define EVENT_KEY_LEFT					MH_KEY_DOWN
		#define EVENT_KEY_RIGHT					MH_KEY_UP
	#else
		#define EVENT_KEY_UP					MH_KEY_UP							//	방향키
		#define EVENT_KEY_DOWN					MH_KEY_DOWN
		#define EVENT_KEY_LEFT					MH_KEY_LEFT
		#define EVENT_KEY_RIGHT					MH_KEY_RIGHT
	#endif
#endif

#if defined( I3_WIPI_SKT_SV360)
#define EVENT_KEY_A							MH_KEY_5						//	A 버튼 (G 버튼)
#define EVENT_KEY_B							MH_KEY_3						//	B 버튼 (X 버튼)
#define EVENT_KEY_X							MH_KEY_0						//	C 버튼 (OK 버튼)
#define EVENT_KEY_Y							MH_KEY_1						//	D 버튼 (G> 버튼)
#elif defined( I3_WIPI_SKT_IM8300)
#define EVENT_KEY_A							MH_KEY_3						//	A 버튼
#define EVENT_KEY_B							MH_KEY_5						//	B 버튼
#elif defined( I3_WIPI_SKT_MS550)
#define EVENT_KEY_A							MH_KEY_5						//	A 버튼 (HOLD 버튼)
#define EVENT_KEY_B							MH_KEY_3						//	B 버튼 (> 버튼)
#define EVENT_KEY_C							MH_KEY_1						//	C 버튼 (< 버튼)
//#define EVENT_KEY_D							MH_KEY_0						//	D 버튼 (Z 버튼)
#elif defined( I3_WIPI_KTF_G1000)
#define EVENT_KEY_A							MH_KEY_GAME_A					//	A 버튼
#define EVENT_KEY_B							MH_KEY_GAME_B					//	B 버튼
#define EVENT_KEY_C							MH_KEY_GAME_C					//	C 버튼
#define EVENT_KEY_L							MH_KEY_GAME_SIDELEFT			//	L 버튼
#define EVENT_KEY_R							MH_KEY_GAME_SIDERIGHT			//	R 버튼
#elif defined( I3_WIPI_KTF_KV3600)
#define EVENT_KEY_A							MH_KEY_GAME_A					//	A 버튼
#define EVENT_KEY_B							MH_KEY_GAME_B					//	B 버튼
#define EVENT_KEY_X							MH_KEY_GAME_X					//	X 버튼
#define EVENT_KEY_Y							MH_KEY_GAME_Y					//	Y 버튼
#endif

#define EVENT_KEY_SELECT					MH_KEY_SELECT				//	멀티팩
#define EVENT_KEY_SEND						MH_KEY_SEND					//	전화걸기
#define EVENT_KEY_END						MH_KEY_END					//	종료
#define EVENT_KEY_0							MH_KEY_0					//	멀티팩

#if !defined( I3_WIPI_SKT_IM8300)
#define EVENT_KEY_1							MH_KEY_1					//	A 버튼
#endif

#define EVENT_KEY_3							MH_KEY_3					//	C 버튼
#define EVENT_KEY_5							MH_KEY_5					//	전화걸기
#define EVENT_KEY_2							MH_KEY_2					//	B 버튼
#define EVENT_KEY_4							MH_KEY_4					//	멀티팩
#define EVENT_KEY_6							MH_KEY_6					//	종료
#define EVENT_KEY_7							MH_KEY_7					//	A 버튼
#define EVENT_KEY_8							MH_KEY_8					//	B 버튼
#define EVENT_KEY_9							MH_KEY_9					//	C 버튼
#define EVENT_KEY_ASTERISK					MH_KEY_ASTERISK				//	전화걸기

#if defined( I3_WIPI_SKT_IM8300)
#define EVENT_KEY_POUND						MH_KEY_1					//	MP3 버튼 (메뉴 호출)
#elif defined( I3_WIPI_SKT_MS550)
#define EVENT_KEY_POUND						MH_KEY_0					//	Z 버튼 (메뉴 호출)
#else
#define EVENT_KEY_POUND						MH_KEY_POUND				//	# 버튼 (메뉴 호출)
#endif


//////////////////////////////////////////////////////////////////////////
//	키코드 정의 (32개까지 키정의 가능)
#define VK_KEY_UP				0x00000001				//	방향키
#define VK_KEY_DOWN				0x00000002
#define VK_KEY_LEFT				0x00000004
#define VK_KEY_RIGHT			0x00000008
#define VK_KEY_UPPER_RIGHT		0x00000010
#define VK_KEY_UPPER_LEFT		0x00000020
#define VK_KEY_LOWER_RIGHT		0x00000040
#define VK_KEY_LOWER_LEFT		0x00000080
#define VK_KEY_A				0x00000100				//	A 버튼
#define VK_KEY_B				0x00000200				//	B 버튼

// Archlord 에 맞춰 키를 바꿨습니다. by dfly79
#if defined( I3_WIPI_KTF_G1000) || defined( I3_WIPI_SKT_MS550)
#define VK_KEY_C				0x00000400				//	C 버튼
#endif

#if defined( I3_WIPI_KTF_G1000)
#define VK_KEY_SIDERIGHT		0x00000800				//	R	버튼
#define VK_KEY_SIDELEFT			0x00001000				//	L	버튼
#endif

#define VK_KEY_SELECT			0x00002000				//	멀티팩
#define VK_KEY_MENU				0x00004000				//	메뉴
#define VK_KEY_OK				0x00008000				//	확인
#define VK_KEY_SEND				0x00010000				//	전화걸기
#define VK_KEY_END				0x00020000				//	종료
#define	VK_KEY_1				0x00040000
#define	VK_KEY_2				0x00080000
#define	VK_KEY_3				0x00100000
#define	VK_KEY_4				0x00200000
#define	VK_KEY_5				0x00400000
#define	VK_KEY_6				0x00800000
#define	VK_KEY_7				0x01000000
#define	VK_KEY_8				0x02000000
#define	VK_KEY_9				0x04000000
#define	VK_KEY_0				0x08000000
#define	VK_KEY_ASTERISK			0x10000000
#define	VK_KEY_POUND			0x20000000

#if defined( I3_WIPI_KTF_KV3600) || defined( I3_WIPI_SKT_SV360)
#define VK_KEY_X				0x40000000	 			
#define VK_KEY_Y				0x80000000
#endif

static i3InputKeyboard *		s_pKeyboard = NULL;
static UINT32					s_PressState = 0, s_StrokeState = 0;

i3InputKeyboard::i3InputKeyboard(void)
{
	s_pKeyboard = this;
}

i3InputKeyboard::~i3InputKeyboard(void)
{

}

BOOL i3InputKeyboard::Create( void * pPlatformData)
{
	Reset();

	return TRUE;
}

void i3InputKeyboard::Reset(void)
{
}

static INT32 s_KeyMap[] =
{
	I3I_KEY_UP,				// VK_KEY_UP			0x00000001				//	방향키
	I3I_KEY_DOWN,			// VK_KEY_DOWN			0x00000002
	I3I_KEY_LEFT,			// VK_KEY_LEFT			0x00000004
	I3I_KEY_RIGHT,			// VK_KEY_RIGHT			0x00000008
	0xFF,					// VK_KEY_UPPER_RIGHT	0x00000010
	0xFF,					// VK_KEY_UPPER_LEFT	0x00000020
	0xFF,					// VK_KEY_LOWER_RIGHT	0x00000040
	0xFF,					// VK_KEY_LOWER_LEFT	0x00000080
	I3I_KEY_Z,				// VK_KEY_A				0x00000100				//	A 버튼
	I3I_KEY_X,				// VK_KEY_B				0x00000200				//	B 버튼
	I3I_KEY_C,				// VK_KEY_C				0x00000400				//	C 버튼
	I3I_KEY_Q,				// VK_KEY_SIDERIGHT		0x00000800				//	R	버튼
	I3I_KEY_W,				// VK_KEY_SIDELEFT		0x00001000				//	L	버튼
	I3I_KEY_NUMPADDEL,		// VK_KEY_SELECT		0x00002000				//	멀티팩
	I3I_KEY_NUMPADMINUS,	// VK_KEY_MENU			0x00004000				//	메뉴
	I3I_KEY_NUMPADPLUS,		// VK_KEY_OK			0x00008000				//	확인
	I3I_KEY_NUMPADENTER,	// VK_KEY_SEND			0x00010000				//	전화걸기
	I3I_KEY_ESC,			// VK_KEY_END			0x00020000				//	종료
	I3I_KEY_NUMPAD1,		// VK_KEY_1				0x00040000
	I3I_KEY_NUMPAD2,		// VK_KEY_2				0x00080000
	I3I_KEY_NUMPAD3,		// VK_KEY_3				0x00100000
	I3I_KEY_NUMPAD4,		// VK_KEY_4				0x00200000
	I3I_KEY_NUMPAD5,		// VK_KEY_5				0x00400000
	I3I_KEY_NUMPAD6,		// VK_KEY_6				0x00800000
	I3I_KEY_NUMPAD7,		// VK_KEY_7				0x01000000
	I3I_KEY_NUMPAD8,		// VK_KEY_8				0x02000000
	I3I_KEY_NUMPAD9,		// VK_KEY_9				0x04000000
	I3I_KEY_NUMPAD0,		// VK_KEY_0				0x08000000
	I3I_KEY_NUMPADMULTIPLY,	// VK_KEY_ASTERISK		0x10000000				// *
	I3I_KEY_NUMPADDIVIDE,	// VK_KEY_POUND			0x20000000				// #
	I3I_KEY_A,				// VK_KEY_X				0x40000000
	I3I_KEY_S				// VK_KEY_Y				0x80000000
};

void i3InputKeyboard::Update( RT_REAL32 tm)
{
	// Clear Stroke Infos
	memset( m_bStrokeInfo, 0, sizeof(m_bStrokeInfo));

	INT32 i3idx, i;
	UINT32 mask;

	for( i = 0, mask = 1; i < 32; i++, mask = mask << 1)
	{
		i3idx = s_KeyMap[ i];

		if( i3idx == 0xFF)
		{
			// Unmapped Key...
			continue;
		}

		// Stroke State
		s_pKeyboard->m_bStrokeInfo[ i3idx] = ((s_StrokeState & mask) != 0);

		// Press State
		s_pKeyboard->m_bPressInfo[ i3idx] = ((s_PressState & mask) != 0);
	}

	s_StrokeState = 0;
}

#define		SET_KEYBIT( a)		s_PressState |= (a);	s_StrokeState |= (a)

void i3InputKeyboard::UpdateCletEvent( int type, int param1, int param2)
{
	switch(type) 
	{
		//	키 입력
		case MH_KEY_PRESSEVENT:	
			//I3TRACE1( "Key : %d\n", param1);

		switch(param1) 
		{
			case EVENT_KEY_UP:			SET_KEYBIT( VK_KEY_UP);	break;
			case EVENT_KEY_DOWN:		SET_KEYBIT( VK_KEY_DOWN);	break;
			case EVENT_KEY_LEFT:		SET_KEYBIT( VK_KEY_LEFT);	break;
			case EVENT_KEY_RIGHT:		SET_KEYBIT( VK_KEY_RIGHT);	break;
			case EVENT_KEY_A:			SET_KEYBIT( VK_KEY_A);	break;
			case EVENT_KEY_B:			SET_KEYBIT( VK_KEY_B);	break;
#if defined( I3_WIPI_KTF_G1000) || defined( I3_WIPI_SKT_MS550)
			case EVENT_KEY_C:			SET_KEYBIT( VK_KEY_C);	break;
#endif
#if !defined( I3_WIPI_SKT_IM8300)
			case EVENT_KEY_SELECT:		SET_KEYBIT( VK_KEY_SELECT);	break;
#endif
			case EVENT_KEY_SEND:		SET_KEYBIT( VK_KEY_SEND);	break;
			case EVENT_KEY_END:			SET_KEYBIT( VK_KEY_END);	break;
			case EVENT_KEY_2:			SET_KEYBIT( VK_KEY_2);	break;			
			case EVENT_KEY_4:			SET_KEYBIT( VK_KEY_4);	break;			
			case EVENT_KEY_6:			SET_KEYBIT( VK_KEY_6);	break;
			case EVENT_KEY_7:			SET_KEYBIT( VK_KEY_7);	break;
			case EVENT_KEY_8:			SET_KEYBIT( VK_KEY_8);	break;
			case EVENT_KEY_9:			SET_KEYBIT( VK_KEY_9);	break;			
			case EVENT_KEY_ASTERISK:	SET_KEYBIT( VK_KEY_ASTERISK);	break;
			case EVENT_KEY_POUND:		SET_KEYBIT( VK_KEY_POUND);	break;

			#if defined( I3_WIPI_GIGA)
				#if defined( I3_WIPI_SKT_IM8300)
					case EVENT_KEY_0:			SET_KEYBIT( VK_KEY_0);	break;			
				#elif defined( I3_WIPI_SKT_SV360)
					case EVENT_KEY_X:			SET_KEYBIT( VK_KEY_X);	break;
					case EVENT_KEY_Y:			SET_KEYBIT( VK_KEY_Y);	break;
				#endif
			#else
				#if defined( I3_WIPI_KTF_G1000)
					case EVENT_KEY_L:			SET_KEYBIT( VK_KEY_SIDELEFT); break;
					case EVENT_KEY_R:			SET_KEYBIT( VK_KEY_SIDERIGHT); break;
				#elif defined( I3_WIPI_KTF_KV3600)
					case EVENT_KEY_X:			SET_KEYBIT( VK_KEY_X); break;
					case EVENT_KEY_Y:			SET_KEYBIT( VK_KEY_Y); break;
				#endif

				case EVENT_KEY_0:			SET_KEYBIT( VK_KEY_0);	break;
				case EVENT_KEY_1:			SET_KEYBIT( VK_KEY_1);	break;
				case EVENT_KEY_3:			SET_KEYBIT( VK_KEY_3);	break;
				case EVENT_KEY_5:			SET_KEYBIT( VK_KEY_5);	break;
			#endif
		}
		break;	

	case MH_KEY_REPEATEVENT:		
		break;

		//	키 해제
	case MH_KEY_RELEASEEVENT:
		switch(param1) 
		{
			case EVENT_KEY_UP:				s_PressState &= ~VK_KEY_UP;				break;
			case EVENT_KEY_DOWN:			s_PressState &= ~VK_KEY_DOWN;			break;
			case EVENT_KEY_LEFT:			s_PressState &= ~VK_KEY_LEFT;			break;
			case EVENT_KEY_RIGHT:			s_PressState &= ~VK_KEY_RIGHT;			break;
			case EVENT_KEY_A:				s_PressState &= ~VK_KEY_A;				break;
			case EVENT_KEY_B:				s_PressState &= ~VK_KEY_B;				break;
#if defined( I3_WIPI_KTF_G1000) || defined( I3_WIPI_SKT_MS550)
			case EVENT_KEY_C:				s_PressState &= ~VK_KEY_C;				break;
#endif
#if !defined( I3_WIPI_SKT_IM8300)
			case EVENT_KEY_SELECT:			s_PressState &= ~VK_KEY_SELECT;			break;
#endif
			case EVENT_KEY_SEND:			s_PressState &= ~VK_KEY_SEND;			break;
			case EVENT_KEY_END:				s_PressState &= ~VK_KEY_END;			break;			
			case EVENT_KEY_2:				s_PressState &= ~VK_KEY_2;				break;
			case EVENT_KEY_4:				s_PressState &= ~VK_KEY_4;				break;
			case EVENT_KEY_6:				s_PressState &= ~VK_KEY_6;				break;
			case EVENT_KEY_7:				s_PressState &= ~VK_KEY_7;				break;
			case EVENT_KEY_8:				s_PressState &= ~VK_KEY_8;				break;
			case EVENT_KEY_9:				s_PressState &= ~VK_KEY_9;				break;
			case EVENT_KEY_ASTERISK:		s_PressState &= ~VK_KEY_ASTERISK;		break;
			case EVENT_KEY_POUND:			s_PressState &= ~VK_KEY_POUND;			break;

			#if defined( I3_WIPI_GIGA)
				#if defined( I3_WIPI_SKT_IM8300)
					case EVENT_KEY_0:				s_PressState &= ~VK_KEY_0;		break;
				#elif defined( I3_WIPI_SKT_SV360)
					case EVENT_KEY_X:				s_PressState &= ~VK_KEY_X;				break;
					case EVENT_KEY_Y:				s_PressState &= ~VK_KEY_Y;				break;
				#endif
			#else
				#if defined( I3_WIPI_KTF_G1000)
					case EVENT_KEY_L:				s_PressState &= ~VK_KEY_SIDELEFT; break;
					case EVENT_KEY_R:				s_PressState &= ~VK_KEY_SIDERIGHT; break;
				#elif defined( I3_WIPI_KTF_KV3600)
					case EVENT_KEY_X:				s_PressState &= ~VK_KEY_X;		break;
					case EVENT_KEY_Y:				s_PressState &= ~VK_KEY_Y;		break;
				#endif

				case EVENT_KEY_0:				s_PressState &= ~VK_KEY_0;		break;
				case EVENT_KEY_1:				s_PressState &= ~VK_KEY_1;		break;
				case EVENT_KEY_3:				s_PressState &= ~VK_KEY_3;		break;
				case EVENT_KEY_5:				s_PressState &= ~VK_KEY_5;		break;
			#endif
		}
		break;
	}
}

#endif
