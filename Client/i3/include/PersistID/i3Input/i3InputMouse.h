#if !defined( __I3_INPUT_MOUSE_H)
#define __I3_INPUT_MOUSE_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputMouse : public i3InputDevice
{
	I3_CLASS_DEFINE( i3InputMouse);
protected:
	REAL32		m_X;
	REAL32		m_Y;
	REAL32		m_Z;
	UINT32		m_PressButton;
	UINT32		m_StrokeButton;

	REAL32		m_DeltaX;
	REAL32		m_DeltaY;
	REAL32		m_DeltaZ;
#if defined( I3G_DX)
	IDirectInputDevice8 *		m_pDevice;
#endif

public:
	i3InputMouse(void);
	virtual ~i3InputMouse(void);

	REAL32		GetX(void)								{ return m_X; }
	REAL32		GetY(void)								{ return m_Y; }
	REAL32		GetZ(void)								{ return m_Z; }

	REAL32		GetDeltaX(void)							{ return m_DeltaX; }
	REAL32		GetDeltaY(void)							{ return m_DeltaY; }
	REAL32		GetDeltaZ(void)							{ return m_DeltaZ; }

	void		SetMousePos( REAL32 fX, REAL32 fY, REAL32 fZ);
	UINT32		GetButtonState(void)					{ return m_PressButton; }
	UINT32		GetStrokeButtonState(void)				{ return m_StrokeButton; }
	
	BOOL		Create( void * pPlatformData);

	virtual void	Update( RT_REAL32 tm);
	virtual void	Reset(void);

#if defined( I3_WINDOWS)
	void		OnMouseMove( UINT nFlag, POINT pt);
#endif
};

#endif
