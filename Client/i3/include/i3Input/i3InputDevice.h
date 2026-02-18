#if !defined( I3_INPUT_DEVICE_H)
#define I3_INPUT_DEVICE_H

#include "i3InputType.h"

class I3_EXPORT_INPUT i3InputDevice : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE( i3InputDevice, i3ElementBase);
protected:
	I3I_DEVICE_TYPE			m_DeviceType = I3I_DEVICE_UNKNOWN;
	UINT32					m_DeviceID = 0;

public:

	I3I_DEVICE_TYPE			GetDeviceType(void)				{ return m_DeviceType; }

	UINT32					GetDeviceID(void)				{ return m_DeviceID; }

	virtual	void			Update( REAL32 tm) = 0;
	virtual void			Reset(void) = 0;

	virtual void			SetAccessRight(bool bGet) {}

	void					Vibration( INT32 ms);			//	by KOMET
};

#endif
