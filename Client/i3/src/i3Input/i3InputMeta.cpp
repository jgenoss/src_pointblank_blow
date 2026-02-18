#include "i3InputType.h"
#include "i3InputDevice.h"
#include "i3InputController.h"

#ifndef I3_WINCE
#include "i3InputKeyboard.h"
#include "i3InputMouse.h"
#endif

#include "i3InputDeviceManager.h"

#if defined( I3_WINDOWS)
#include "i3InputIMEContext.h"
#include "i3InputIME.h"
#include "i3InputMethodContext.h"
//#include "i3InputMethodEditor.h"
#endif

#pragma optimize ("g",off)

static bool AIP_Mark = false;

I3_EXPORT_INPUT
void AIP_Marker_Input(void) { AIP_Mark = true; }

I3_EXPORT_INPUT
void i3InputRegisterMetas(void)
{
	if ( AIP_Mark == false)
	{
		AIP_Marker_Input();
	}
}

#pragma optimize ("g",on)