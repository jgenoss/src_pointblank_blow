#if !defined( __I3_INPUT_CONTROLLER_H)
#define __I3_INPUT_CONTROLLER_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputController : public i3InputDevice
{
	I3_CLASS_DEFINE( i3InputController);
protected:
	UINT32		m_PressState;
	UINT32		m_RepeatState;
	UINT32		m_StrokeState;
	RT_REAL32	m_TimeStamp[ I3I_BTN_MAX];
	UINT32		m_RepeatCount[ I3I_BTN_MAX];

	BOOL		m_bAnalog;

	REAL32		m_Analog[ I3I_ANALOG_DIR_MAX];

#if defined( I3G_DX)
	IDirectInputDevice8 *		m_pDevice;
#endif

public:
	i3InputController(void);
	virtual ~i3InputController(void);

	void			ResetAll(void);

	UINT32			GetPressState(void)						{ return m_PressState; }
	void			SetPressState( UINT32 btn)				{ m_PressState = btn; }

	UINT32			GetRepeatState(void)					{ return m_RepeatState; }
	void			SetRepeatState( UINT32 btn)				{ m_RepeatState = btn; }

	UINT32			GetStrokeState(void)					{ return m_StrokeState; }
	void			SetStrokeState( UINT32 btn)				{ m_StrokeState = btn; }

	BOOL			IsAnalogStick(void)						{ return m_bAnalog; }
	REAL32			GetAnalogValue( I3I_ANALOG_DIR dir)		{ return m_Analog[ dir]; }

	BOOL			Create( void * pPlatformData);

	virtual	void	Update( RT_REAL32 tm);
	virtual void	Reset(void);
};

#endif
