#if !defined( __I3_INPUT_DEVICE_MANAGER_H)
#define __I3_INPUT_DEVICE_MANAGER_H

#include "i3Base.h"
#include "i3InputType.h"
#include "i3InputDefine.h"
#include "i3InputDevice.h"
#include "i3InputController.h"

#ifndef I3_WINCE
#include "i3InputKeyboard.h"
#include "i3InputMouse.h"
#endif

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct I3I_MAP_ANALOG
{
	REAL32			rValue = 0.0f;
	I3I_ANALOG_DIR	nDir = I3I_ANALOG_DIR_UNKNOWN;
	I3I_KEY			nMinusKey = I3I_KEY_UNKNOWN;
	I3I_KEY			nPlusKey = I3I_KEY_UNKNOWN;

	INT32	nMappedMinusButton = -1;
	INT32	nMappedPlusButton = -1;

	void Init()
	{
		rValue = 0.0f;
		nDir = I3I_ANALOG_DIR_UNKNOWN;
		nMinusKey = I3I_KEY_UNKNOWN;
		nPlusKey = I3I_KEY_UNKNOWN;
		nMappedMinusButton = -1;
		nMappedPlusButton = -1;
	}

	void Reset()
	{
		rValue = 0.0f;
	}
} ;

struct I3I_MAP_BUTTON
{
	bool	bPress = false;
	bool	bStroke = false;
	I3I_BUTTON	nButton = I3I_BTN_UNKNOWN;
	I3I_KEY		nKey = I3I_KEY_UNKNOWN;

	void Init()
	{
		bPress = false;
		bStroke = false;
		nKey = I3I_KEY_UNKNOWN;
		nButton = I3I_BTN_UNKNOWN;
	}

	void Reset()
	{
		bPress = false;
		bStroke = false;
	}

} ;

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

class I3_EXPORT_INPUT i3InputDeviceManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3InputDeviceManager, i3ElementBase);
protected:

#ifndef I3_WINCE
	i3InputKeyboard*		m_pKeyboard = nullptr;
	i3InputMouse *			m_pMouse = nullptr;
#endif

	i3InputController *		m_pController[I3I_CONTROLLER_MAX] = { nullptr, nullptr, nullptr, nullptr };
	INT32					m_ControllerCount = 0;

	i3Timer *				m_pTimer = nullptr;

#if defined( I3G_DX) || defined( I3G_XBOX)
	IDirectInput8	*		m_pInputObj = nullptr;
#endif

	INT32					m_SkipUpdateCount = 0;

	// 입력 데이터를 매핑했는지 여부
	bool					m_bMapped = false;

	I3I_MAP_ANALOG			*m_pAnalogMap = nullptr;
	I3I_MAP_BUTTON			*m_pButtonMap = nullptr;

	INT32					m_nAnalogMapCount = 0;
	INT32					m_nButtonMapCount = 0;

	INT32					m_nJoyMapIndex = 0;

	void					InitCommon();
	void					DestroyCommon();

public:
	virtual ~i3InputDeviceManager(void);


#ifndef I3_WINCE
	void				SetKeyboard( i3InputKeyboard * pKeyboard);
	void				SetMouse( i3InputMouse * pMouse);	

	i3InputKeyboard *	GetKeyboard(void)						{ return m_pKeyboard; }
	i3InputMouse *		GetMouse(void)							{ return m_pMouse; }
#endif

	void				SetController( INT32 idx, i3InputController * pController);

	i3InputController *	GetController( INT32 idx)				{ return m_pController[ idx]; }
	INT32				GetControllerCount(void)				{ return m_ControllerCount; }

	//i3InputDevice *		GetDevice( I3I_DEVICE_TYPE type, INT32 idx = 0);

	bool		Create( void * pPlatformData, UINT32 DeviceMask = I3I_DEVICE_MASK_KEYBOARD | I3I_DEVICE_MASK_MOUSE | I3I_DEVICE_MASK_CONTROLLER);

	void		Update(void);
	void		Reset(void);

	// 지정된 회수만큼 업데이트를 하지 않음.
	void		SkipUpdate( INT32 nCount );

	bool		IsMapped(){	return m_bMapped; }

	// Mapped Input Management
	bool		CreateMap( INT32 nAnalogCount, INT32 nButtonCount, INT32 nJoyIndex = 0 );

	void		ResetMapData();
	void		UpdateMapData();

	void		MapAnalog( INT32 nIndex, I3I_ANALOG_DIR nDir, I3I_KEY nMinusKey, I3I_KEY nPlusKey );
	void		MapButton( INT32 nIndex, I3I_BUTTON nButton, I3I_KEY nKey );

	void		MapAnalogByButton( INT32 nIndex, INT32 nMappedMinusButton, INT32 nMappedPlusButton );

	INT32		GetAnalogMapCount(){ return m_nAnalogMapCount; }
	INT32		GetButtonMapCount(){ return m_nButtonMapCount;	}

	REAL32		GetAnalogValue( INT32 nIndex ){	return m_pAnalogMap[ nIndex ].rValue; }
	bool		IsButtonPress( INT32 nIndex ){ return m_pButtonMap[ nIndex ].bPress; }
	bool		IsButtonStroke( INT32 nIndex ){ return m_pButtonMap[ nIndex ].bStroke; }

	void		SetAccessRight( bool bGet);

#if defined( I3G_DX)
	virtual void OnLButtonDown( UINT nFlag, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMButtonDown( UINT nFlag, POINT point);
	virtual void OnMButtonUp( UINT nFlag, POINT point);
	virtual void OnRButtonDown( UINT nFlag, POINT point);
	virtual void OnRButtonUp( UINT nFlag, POINT point);
	virtual void OnMouseMove( UINT nFlag, POINT point);
	virtual void OnMouseWheel( UINT nFlag, short zDelta, POINT point);
	virtual void OnRawInput( WPARAM wParam, LPARAM lParam);
	virtual void OnKeyDown( UINT32 nKey);
	virtual void OnKeyUp( UINT32 nKey);
#endif
};

#endif
