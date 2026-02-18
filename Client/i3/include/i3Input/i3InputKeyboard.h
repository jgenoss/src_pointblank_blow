#if !defined( __I3_INPUT_KEYBOARD_H)
#define __I3_INPUT_KEYBOARD_H

#include "i3InputDevice.h"

class I3_EXPORT_INPUT i3InputKeyboard : public i3InputDevice
{
	I3_EXPORT_CLASS_DEFINE( i3InputKeyboard, i3InputDevice);
protected:
	bool		m_bPressInfo[I3I_KEY_MAX] = { false };
	bool		m_bRepeatInfo[I3I_KEY_MAX] = { false };
	INT32		m_RepeatCount[I3I_KEY_MAX] = { 0 };
	bool		m_bStrokeInfo[I3I_KEY_MAX] = { false };
	REAL32	m_TimeStamp[I3I_KEY_MAX] = { 0.0f };

public:
	//
	// 만약 DirectX 라면....pPlatformData로 IDirectInputDevice8 Object Pointer가 전달된다.
	bool		Create( void * pPlatformData);
	void		ResetAll(void);

	bool		GetPressState( I3I_KEY key)			{ return m_bPressInfo[ key]; }
	bool		GetRepeatState( I3I_KEY key)			{ return m_bRepeatInfo[ key]; }
	bool		GetStrokeState( I3I_KEY key)			{ return m_bStrokeInfo[ key]; }

	void		SetPressState( I3I_KEY key, bool bPress)	{ m_bPressInfo[ key] = bPress; }
	void		SetStrokeState( I3I_KEY key, bool bStroke)	{ m_bStrokeInfo[key] = bStroke; }

	static const char *		GetKeyName( I3I_KEY key);

	virtual void	Update( REAL32 tm) override;
	virtual void	Reset(void) override;

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
