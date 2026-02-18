#include "i3InputType.h"
#include "i3InputController.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_PSP)

#define ANALOG_AVG		127.5f
#define ANALOG_INVAVG	0.0078431372549019607843137254901961f

i3InputController::i3InputController(void)
{
	m_bAnalog = TRUE;

	Reset();
}

i3InputController::~i3InputController(void)
{
}

void i3InputController::Reset(void)
{
	m_PressState = 0;
	m_RepeatState = 0;	
	m_StrokeState = 0;

	memset( m_RepeatCount, 0, sizeof(m_RepeatCount));
	memset( m_TimeStamp, 0, sizeof( m_TimeStamp));
	memset( m_Analog, 0, sizeof(m_Analog));
}

BOOL i3InputController::Create( void * pPlatformData)
{
	return TRUE;
}

static UINT32 s_btn_pattern[ ] =
{
	SCE_CTRL_Lleft,
	SCE_CTRL_Lright,
	SCE_CTRL_Lup,
	SCE_CTRL_Ldown,
	SCE_CTRL_Rleft,
	SCE_CTRL_Rright,
	SCE_CTRL_Rup,
	SCE_CTRL_Rdown,
	SCE_CTRL_START,
	SCE_CTRL_SELECT,
	SCE_CTRL_L1,
	SCE_CTRL_R1,
	0,
	0,
	0,
	0,
	0
};

void i3InputController::Update( REAL32 tm)
{
	UINT32	NewState = 0, pad;
	UINT32	i;
	REAL32	timeGap;
	UINT32 mask = I3I_BTN_LLEFT;
	SceCtrlData		Data;
	int		rv, Count = 0;

	pad = 0;

	//do
	{
		// sce ÇÔ¼ö
		rv = sceCtrlReadBufferPositive( &Data, 1);

		if( rv > 0)
		{
			pad |= Data.Buttons;
			Count++;
		}
	} //while( rv > 0);

	//I3TRACE( "Analog(X,Y) = %6d,%6d\n", Data.Lx, Data.Ly );

	if(m_bAnalog)
	{
		if( Count > 0)
		{
			m_Analog[ I3I_ANALOG_DIR_X] = MIN( MAX( -1.0f, ( ( Data.Lx - ANALOG_AVG ) * ANALOG_INVAVG ) ), 1.0f );
			m_Analog[ I3I_ANALOG_DIR_Y] = MIN( MAX( -1.0f, ( ( Data.Ly - ANALOG_AVG ) * ANALOG_INVAVG ) ), 1.0f );

			// Remove Dead Zone
			for( i = 0; i < I3I_ANALOG_DIR_MAX; i++ )
			{
				if( ( I3I_ANALOG_DEAD_MIN <= m_Analog[i] ) && ( m_Analog[i] <= I3I_ANALOG_DEAD_MAX ) )
				{
					m_Analog[i] = 0.0f;
				}
			}
		}
		else
		{
			m_Analog[ I3I_ANALOG_DIR_X] = 0.0f;
			m_Analog[ I3I_ANALOG_DIR_Y] = 0.0f;
		}
	}

	for( i = 0; s_btn_pattern[i] > 0; i++, mask = mask << 1)
	{
		if( pad & s_btn_pattern[i])
			NewState |= mask;
	}

	// Stroke State
	m_StrokeState = (m_PressState ^ NewState) & NewState;

	// Repeat State
	for( i = 0, mask = 1; mask != 0; mask = mask << 1, i++)
	{
		if( NewState & mask)
		{
			if( m_RepeatCount[i] == 0)
			{
				m_TimeStamp[i] = 0.0f;
				m_RepeatState |= mask;
				m_RepeatCount[i] = 1;
			}
			else
			{
				if( m_RepeatCount[i] == 1)
					timeGap = 0.250f;
				else
					timeGap = 0.033f;

				m_TimeStamp[i] += tm;

				if(m_TimeStamp[i] > timeGap)
				{
					m_RepeatCount[i] ++;
					m_RepeatState |= mask;
					m_TimeStamp[i] = 0.0f;
				}
				else
				{
					m_RepeatState &= ~mask;
				}
			}
		}
		else
		{
			m_RepeatCount[i] = 0;
			m_TimeStamp[i] = 0;
			m_RepeatState &= ~mask;
		}
	}

	// Press State
	m_PressState = NewState;
}

#endif
