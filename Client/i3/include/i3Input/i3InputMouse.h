#if !defined( __I3_INPUT_MOUSE_H)
#define __I3_INPUT_MOUSE_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputMouse : public i3InputDevice
{
	I3_EXPORT_CLASS_DEFINE( i3InputMouse, i3InputDevice);
protected:
	REAL32		m_X = 0.0f;
	REAL32		m_Y = 0.0f;
	REAL32		m_Z = 0.0f;
	UINT32		m_PressButton = 0;
	UINT32		m_StrokeButton = 0;

	REAL32		m_DeltaX = 0.0f;
	REAL32		m_DeltaY = 0.0f;
	REAL32		m_DeltaZ = 0.0f;
#if defined( I3G_DX)
	IDirectInputDevice8 *		m_pDevice = nullptr;
#endif

public:
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

	void		SetButtonState(UINT32 mask, bool bPress);
	void		SetStrokeButtonState(UINT32 mask, bool bPress);
	
	bool		Create( void * pPlatformData);

	virtual void	Update( REAL32 tm) override;
	virtual void	Reset(void) override;
	virtual void	SetAccessRight( bool bGet) override;

#if defined( I3_WINDOWS)
	void		OnMouseMove( UINT nFlag, POINT pt);
#endif

	// 왼손 마우스 관련
private:
	bool m_bUseLeftMouse = false;
public:
	void SetUseLeftMouse(bool bFlag) { m_bUseLeftMouse = bFlag; }
};

#endif
