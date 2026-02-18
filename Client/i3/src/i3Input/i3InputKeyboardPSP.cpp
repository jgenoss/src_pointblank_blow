#include "i3InputType.h"
#include "i3InputKeyboard.h"
#include "i3InputGlobalVariable.h"

#if defined( I3G_PSP)

i3InputKeyboard::i3InputKeyboard(void)
{
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

void i3InputKeyboard::Update( REAL32 tm)
{

}

#endif
