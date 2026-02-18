#include "i3InputType.h"
#include "i3InputMouse.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_PSP)

i3InputMouse::i3InputMouse(void)
{
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	m_Button = 0;
}

i3InputMouse::~i3InputMouse(void)
{
}

BOOL i3InputMouse::Create( void * pPlatformData)
{

	return TRUE;
}

void i3InputMouse::Update( REAL32 tm)
{

}
#endif
