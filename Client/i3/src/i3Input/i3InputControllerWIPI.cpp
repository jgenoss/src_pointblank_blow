#include "i3InputType.h"
#include "i3InputController.h"
#include "i3InputGlobalVariable.h"

#if defined( I3_WIPI)

i3InputController::i3InputController(void)
{
	m_bAnalog = FALSE;

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

void i3InputController::Update( RT_REAL32 tm)
{
}

#endif
