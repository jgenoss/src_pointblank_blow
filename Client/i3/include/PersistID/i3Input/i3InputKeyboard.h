#if !defined( __I3_INPUT_KEYBOARD_H)
#define __I3_INPUT_KEYBOARD_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputKeyboard : public i3InputDevice
{
	I3_CLASS_DEFINE( i3InputKeyboard);
protected:
	BOOL		m_bPressInfo[ I3I_KEY_MAX];
	BOOL		m_bRepeatInfo[ I3I_KEY_MAX];
	INT32		m_RepeatCount[ I3I_KEY_MAX];
	BOOL		m_bStrokeInfo[ I3I_KEY_MAX];
	RT_REAL32	m_TimeStamp[ I3I_KEY_MAX];

public:
	i3InputKeyboard(void);
	virtual ~i3InputKeyboard(void);

	//
	// 만약 DirectX 라면....pPlatformData로 IDirectInputDevice8 Object Pointer가 전달된다.
	BOOL		Create( void * pPlatformData);
	void		ResetAll(void);

	BOOL		GetPressState( I3I_KEY key)			{ return m_bPressInfo[ key]; }
	BOOL		GetRepeatState( I3I_KEY key)			{ return m_bRepeatInfo[ key]; }
	BOOL		GetStrokeState( I3I_KEY key)			{ return m_bStrokeInfo[ key]; }

	void		SetPressState( I3I_KEY key, BOOL bPress)	{ m_bPressInfo[ key] = bPress; }
	void		SetStrokeState( I3I_KEY key, BOOL bStroke)	{ m_bStrokeInfo[key] = bStroke; }

	virtual void	Update( RT_REAL32 tm);
	virtual void	Reset(void);

#if defined( I3_WIPI)
	static	void UpdateCletEvent( int type, int param1, int param2);
#endif

#if defined( I3_WINDOWS)
	void		OnKeyDown( UINT32 nKey);
	void		OnKeyUp( UINT32 key);

	INT32		FindVKByI3IKey(I3I_KEY key);
#endif
};

#endif
