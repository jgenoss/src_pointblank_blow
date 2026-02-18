#if !defined( __I3_INPUT_CONTROLLER_H)
#define __I3_INPUT_CONTROLLER_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputController : public i3InputDevice
{
	I3_EXPORT_CLASS_DEFINE( i3InputController, i3InputDevice);
protected:
	UINT32		m_PressState = 0;
	UINT32		m_RepeatState = 0;
	UINT32		m_StrokeState = 0;
	REAL32		m_TimeStamp[I3I_BTN_MAX] = { 0.0f };
	UINT32		m_RepeatCount[I3I_BTN_MAX] = { 0 };

	bool		m_bAnalog = true;

	REAL32		m_Analog[I3I_ANALOG_DIR_MAX] = { 0.0f };

#if defined( I3G_DX)
	IDirectInputDevice8 *		m_pDevice = nullptr;
#endif

public:
	virtual ~i3InputController(void);

	void			ResetAll(void);

	UINT32			GetPressState(void)						{ return m_PressState; }
	void			SetPressState( UINT32 btn)				{ m_PressState = btn; }

	UINT32			GetRepeatState(void)					{ return m_RepeatState; }
	void			SetRepeatState( UINT32 btn)				{ m_RepeatState = btn; }

	UINT32			GetStrokeState(void)					{ return m_StrokeState; }
	void			SetStrokeState( UINT32 btn)				{ m_StrokeState = btn; }

	bool			IsAnalogStick(void)						{ return m_bAnalog; }
	REAL32			GetAnalogValue( I3I_ANALOG_DIR dir)		{ return m_Analog[ dir]; }

	bool			Create( void * pPlatformData);

	virtual	void	Update( REAL32 tm) override;
	virtual void	Reset(void) override;
	virtual void	SetAccessRight( bool bGet) override;
};

#endif
