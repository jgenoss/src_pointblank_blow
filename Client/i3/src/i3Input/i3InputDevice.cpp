#include "i3InputType.h"
#include "i3InputDevice.h"

I3_CLASS_INSTANCE( i3InputDevice);


void i3InputDevice::Vibration( INT32 ms)
{
#if defined( I3_WIPI)	
	//	시간(ms)동안 진동을 울립니다. 
	MC_mdaVibrator( 1, ms);
#endif
}

