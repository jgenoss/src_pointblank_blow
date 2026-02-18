#ifndef __GUIOBJBASE_H__
#define __GUIOBJBASE_H__

#include "i3GuiDefines.h"

class I3_EXPORT_GUI i3GuiObjBase : public i3GameObjBase
{
	I3_CLASS_DEFINE(i3GuiObjBase);

protected:	
	REAL32						m_fStartX;
	REAL32						m_fStartY;
	REAL32						m_fEventScaleX;
	REAL32						m_fEventScaleY;
	REAL32						m_fRotation;
	INT32						m_nScaleDir;
	INT32						m_nWidth;
	INT32						m_nHeight; 
	INT32						m_nOriginalWidth;
	INT32						m_nOriginalHeight; 

	// Persistent Data
	I3GUI_EVENT_DATA	*		m_pInfo_EventData[MAX_I3GUI_CONTROL_STATE];

	// Event에 사용되는 변수입니다
	UINT32						m_nEventFlag;
	I3GUI_EVENT_MOVE_DATA		m_Event_Move;
	I3GUI_EVENT_ROTATE_DATA		m_Event_Rotate;
	I3GUI_EVENT_SCALE_DATA		m_Event_Scale;
	
	i3SoundNode				*	m_pSoundNode;
	// 내부에서 계층구조의 계산에 좀더 유용하게 사용하려고 하는 변수입니다.
	i3List						m_List;

	//
	I3GUI_INPUT_MODE			m_InputMode;

	
	BOOL						m_bMouseUsed;

	//
	GUI_CONTROL_STATE			m_ControlState;
	GUI_CONTROL_STATE			m_OldControlState;
	
	UINT32						m_LastEvent;

	INT32						m_nControlID;	// ID for Control Notify
	
	BOOL						m_bExclusiveEnableControl;

public:
	UINT32						m_OldInputState;
	BOOL						m_bIsFocus;
	BOOL						m_bDisable;
	BOOL						m_bClickAction;
	BOOL						m_bFocusAction;
	INT32						m_UserDefineStateIndex[MAX_I3GUI_CONTROL_STATE];

	BOOL						m_bInputDisable;
	BOOL						m_bSoundSkip;

	//
	BOOL						m_bOnceUpdate;
    
public:
	i3GuiObjBase(void);
	virtual ~i3GuiObjBase(void);

	virtual void	AddChild(i3GameNode* pObj);
	virtual void	RemoveChild(i3GameNode* pObj);
	virtual void	RemoveAllChild();
	virtual void	OnUpdate( REAL32 rDeltaSeconds );
	virtual	void	UserInput(REAL32 fX, REAL32 fY, UINT32 state);
	virtual void	OnKillFocus(void);

	BOOL			Create();

	BOOL			getMouseUsed()							{ return m_bMouseUsed;							}
	void			setMouseUsed( BOOL bUsed)				{ m_bMouseUsed = bUsed;							}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UINT32			GetOldInputState(void) const { return m_OldInputState;}
	void			SetOldInputState(UINT32 state) { m_OldInputState = state;}
	BOOL			getIsFocus()							{ return m_bIsFocus;							}
	void			SetFocus( BOOL bTrue);
		
	void			setFocusAction( BOOL bTrue)				{ m_bFocusAction = bTrue;						}
	void			setClickAction( BOOL bTrue)				{ m_bClickAction = bTrue;						}
	void			SetUserDefine( GUI_CONTROL_STATE state, INT32 index);
	void			setInputDisable( BOOL bTrue)			{ m_bInputDisable = bTrue;						}
	BOOL			getInputDisable()						{ return m_bInputDisable;						}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Control State를 설정합니다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UINT32			getLastEvent(void)	{ return m_LastEvent;}
	void			setLastEvent(UINT32 LastEvent) { m_LastEvent = LastEvent; }

	GUI_CONTROL_STATE	getControlState()						{ return m_ControlState;						}
	void			setControlState(GUI_CONTROL_STATE state);

	void				setControlDisable( BOOL bTrue);
	BOOL				getControlDisable()						{ return m_bDisable;							}

	UINT32			GetEventStyle( GUI_CONTROL_STATE state)	;
	I3GUI_EVENT_DATA * getEventData( GUI_CONTROL_STATE state)	{ return m_pInfo_EventData[state];}

	void			SetEventData( GUI_CONTROL_STATE state, I3GUI_EVENT_DATA * pData);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Input Mode를 설정합니다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void				setInputMode(I3GUI_INPUT_MODE mode)	{ m_InputMode = mode;							}
	I3GUI_INPUT_MODE	getInputMode()						{ return m_InputMode;							}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Control 크기를 재설정합니다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	virtual void		SetControlResizeWidth( REAL32 fWidth);
	virtual void		SetControlResizeHeight( REAL32 fHeight);
	void				setControlResize( REAL32 fWidth, REAL32 fHeight)	{ SetControlResizeWidth( fWidth); SetControlResizeHeight(fHeight);}


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 위치를 설정한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setPositionX(REAL32 fX)					{ m_fStartX = fX;								}
	void			setPositionY(REAL32 fY)					{ m_fStartY = fY;								}
	void			setPosition(REAL32 fX, REAL32 fY)		{ m_fStartX = fX;		m_fStartY = fY;			}
	void			getPosition(VEC2D* pVec)				{ i3Vector::Set(pVec, m_fStartX, m_fStartY);	}
	REAL32			getPositionX()							{ return m_fStartX;								}
	REAL32			getPositionY()							{ return m_fStartY;								}
	REAL32			getTotalPosX();
	REAL32			getTotalPosY();
	BOOL			IsOnMouse(REAL32 fX,REAL32 fY);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 크기를 설정한다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setSize(INT32 width, INT32 height)		{ m_nWidth = width;	m_nHeight = height;			}
	INT32			getWidth()								{ return m_nWidth;								}
	INT32			getHeight()								{ return m_nHeight;								}
	void			setOriginalControlSize(INT32 width, INT32 height)	{ m_nOriginalWidth = width;	m_nOriginalHeight = height;}
	INT32			getOriginalWidth()						{ return m_nOriginalWidth;						}
	INT32			getOriginalHeight()						{ return m_nOriginalHeight;						}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Scale을 설정한다
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setEventScale(REAL32 sX, REAL32 sY)			{ m_fEventScaleX = sX;	m_fEventScaleY = sY;}
	void			setEventScaleX(REAL32 sX)					{ m_fEventScaleX = sX;						}
	void			setEventScaleY(REAL32 sY)					{ m_fEventScaleY = sY;						}
	REAL32			getEventScaleX()							{ return m_fEventScaleX;					}
	REAL32			getEventScaleY()							{ return m_fEventScaleY;	 				}
	INT32			getEventScaleDir()							{ return m_nScaleDir;						}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Rotation을 설정한다
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void			setRotate(REAL32 fRotate)				{ m_fRotation = fRotate;						}
	REAL32			getRotate()								{ return m_fRotation;							}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Event관련 함수들입니다.
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Event함수중에서 사용자가 호출하는 함수입니다.
	void			setEvent(UINT32 flag)					{ m_nEventFlag = flag;							}
	void			addEvent(UINT32 flag)					{ m_nEventFlag |= flag;							}
	void			removeEvent(UINT32 flag)				{ m_nEventFlag &= ~flag;						}
    UINT32			getEvent()								{ return m_nEventFlag;							}
	BOOL			isEvent(UINT32 flag)					{ return (m_nEventFlag & flag) != 0;			}	
	void			ResetEvent();
	void			RemoveAllChildEvents( void);
	virtual void	MoveTo(REAL32 fX, REAL32 fY, REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	RotateTo(INT32 Direction, BOOL bLoop = FALSE, REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	ScaleTo(REAL32 sX, REAL32 sY, BOOL bLoop = FALSE,REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	ScaleToRight(REAL32 scale,  BOOL bLoop = FALSE,REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	ScaleToLeft(REAL32 scale, BOOL bLoop = FALSE,REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	ScaleToUp(REAL32 scale, BOOL bLoop = FALSE,REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);
	virtual void	ScaleToDown(REAL32 scale, BOOL bLoop = FALSE,REAL32 fTime = 1.0f, INT32 nFactorType = I3GUI_EVENT_FACTOR_TYPE_NORMAL);

	void			PlaySound( i3SoundNode * pNode);
	// Event함수중에서 클래스 내부에서만 사용되는 함수입니다.
	void			_OnSetControlState( GUI_CONTROL_STATE nState);

	virtual void	_UpdateEvent( REAL32 rDeltaSeconds );	
	BOOL			_ProcessMoveTo( REAL32 rDeltaSeconds );
	BOOL			_ProcessRotateTo( REAL32 rDeltaSeconds );
	BOOL			_ProcessScaleTo( REAL32 rDeltaSeconds );
	BOOL			_ProcessSound( REAL32 rDeltaSeconds);
	
	
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method );
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
	virtual void	SetEnable( BOOL bTrue);

	void SetControlID(INT32 nControlID);	// ID for Control Notify

	void PushControlNotify(INT32 nEvent,INT32 nPriParam,INT32 nSecParam);

	void SetExclusiveEnableControl(BOOL bEnable)			{ m_bExclusiveEnableControl = bEnable;	}
	BOOL IsComboButton(void);
};

#endif // __GUIOBJBASE_H__
