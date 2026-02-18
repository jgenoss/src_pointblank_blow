// ============================================================================
//
// File: i3Gui.h
//
// Desc: i3Gui Main Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================

#ifndef __I3GUIROOT_H__
#define __I3GUIROOT_H__

#include "i3GuiDefines.h"
#include "i3GuiWindow.h"
#include "i3GuiControl.h"

#define I3GUI_MAX_NOTIFY		16

enum I3GUI_TEXT_USAGE_TYPE
{
	I3GUI_TEXT_USAGE_TYPE_TEXTNODEDX = 0,		// D3DXFont사용 (Window에서 사용가능( DirectX사용가능한...)
	I3GUI_TEXT_USAGE_TYPE_TEXTNODE,				// 이전 버전의 폰트 사용( 어느곳에서나 가능)
};

class I3_EXPORT_GUI i3GuiRoot : public i3GameNode
{
	I3_EXPORT_CLASS_DEFINE(i3GuiRoot, i3GameNode);
private:
	static REAL32					m_sfGuiResolutionWidth;
	static REAL32					m_sfGuiResolutionHeight;

	static REAL32					m_sfGuiResolutionWidthInGame;
	static REAL32					m_sfGuiResolutionHeightInGame;

	static i3GuiObjBase*			m_spCapturedMouseControl;
	static i3GuiObjBase*			m_spFocusControl;

	static I3GUI_TEXT_USAGE_TYPE	m_sTextUsageType;

	static UINT32					m_sMouseButtonState;

	static bool						m_sbDisableInput;

	static i3GuiObjBase *			m_pOldOnMouse;

protected:
	i3AttrSet*				m_pGuiAttrSet = nullptr;			
	i3Node*					m_pNode = nullptr;				// SceneGraph용 Node
	i3::vector<i3GuiControl*> m_ControlBackUpList;

	i3GuiWindow*			m_pOldFocusWindow = nullptr;		// 최상위 Window중에서 이전에 Focus를 가지고 있던 Window입니다.	

	INT32					m_nWidth = 0;
	INT32					m_nHeight = 0;

	// Control notify handler functions
	I3GUI_CONTROL_NOTIFY	m_NotifyQueue[I3GUI_MAX_NOTIFY];
	INT32					m_idxStart = 0;
	INT32					m_idxLast = 0;
	bool					m_bFulled = false;
	bool					m_bManualDispatchNotify = false;

	// Default 인자
	i3TextFontSetInfo		m_FontSetInfo;
	INT32					m_nDefaultTextCount = 256;
	INT32					m_nDefaultTextSize = 8;
	INT32					m_nDefaultTextSpaceX = 0;
	INT32					m_nDefaultTextSpaceY = 0;
	REAL32					m_fDefaultTextOffsetX = 0.0f;
	REAL32					m_fDefaultTextOffsetY = 0.0f;


protected:
	void					_Rec_BuildSceneGraphByWindow( i3GuiObjBase * pObj);
	void					_Rec_BuildSceneGraphByControl( i3GuiControl * pObj);
	i3GuiControl *			_Rec_FindChildControlByName( i3GuiControl * pCtrl, const char * pszName);

	i3EventReceiver *		_Find_EventReceiver( i3GuiObjBase * pLeaf);

public:
	i3GuiRoot(void);
	virtual					~i3GuiRoot( void);

	virtual void			OnUpdate( REAL32 rDeltaSeconds) override;
	virtual bool			OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	bool					Create( i3Node *pParentRoot, const char *pFontName = "돋음", UINT32 CharSet = DEFAULT_CHARSET, INT32 nTextMaxCount = 256, INT32 nTextSize = 9);
	
	// 우선 이것으로 대체합니다!
	i3GuiWindow*			ADDWindowByGUI(const char * szPath);
	i3GuiWindow*			ADDControlByGUI(const char * szPath, bool bAddToTail = true);

	i3Node*					getSceneGraphNode()								{ return m_pNode;					}

	void					SetViewer(i3Viewer* pViewer, bool bNoResize = true);

	void					AddChild(i3GuiWindow* pWindow);
	void					RemoveChild(i3GuiWindow* pWindow);
	void					RemoveAllChild();
	i3GuiControl *			FindChildControlByName( const char * pszName);

	// Index의 Window의 활성화 비활성화를 설정합니다.
	void					SetEnableWindowByIndex(INT32 nIndex, bool bTrue = true);

	// 현재 Mouse가 올려져 있는 Window를 반환합니다.
	i3GuiWindow*			GetOnMouseWindow(i3InputMouse* pMouse);

	// 해당 요소를 가장 상위의 Layer로 설정합니다.
	void					SetTopLayer(i3GuiWindow* pWindow);	

	// 
	void					setDefaultTextSpace( INT32 nX, INT32 nY)		{ m_nDefaultTextSpaceX = nX;	m_nDefaultTextSpaceY = nY;	}
	void					setDefaultTextOffset( REAL32 fX, REAL32 fY)		{ m_fDefaultTextOffsetX = fX;	m_fDefaultTextOffsetY = fY;	}

	void					setWidth( INT32 nWidth)							{ m_nWidth = nWidth;				}
	INT32					getWidth()										{ return m_nWidth;					}

	void					setHeight( INT32 nHeight)						{ m_nHeight = nHeight;				}
	INT32					getHeight()										{ return m_nHeight;					}
	
	UINT32					getMouseState(void)								{ return m_msMouseState;			} // I3GUI_MOUSE_STATE
	
	void					ClearMouseState(void);

	virtual void			SetEnable( bool bEnable);

	void					OnInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds);
	UINT32					OnMouseInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds);	
	void					SetDoubleClickTime(REAL32 rSeconds = 0.5f);	// default 500ms

	// Control notify handler functions
	void					PushControlNotify(INT32 nID,INT32 nEvent,INT32 nPriParam,INT32 nSecParam, void * pPointer = nullptr);
	bool					PopControlNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	void					ResetControlNotify(void);
	
	bool					isManualNotifyDispatch(void)					{ return m_bManualDispatchNotify; }
	void					setManualNotifyDispatch( bool bState)			{ m_bManualDispatchNotify = bState; }

	void					BuildSceneGraph(void);

	// 
	i3GuiControl*			GetControlByName(const char* pszName, i3EventReceiver * pReceiver = nullptr); 
	

	//
	static bool				isControlOnceUpdate(void);
	static i3Viewer*		getViewer(void);	
	static REAL32			getGuiResolutionWidth(void)						{ return m_sfGuiResolutionWidth;	}
	static REAL32			getGuiResolutionHeight(void)					{ return m_sfGuiResolutionHeight;	}
	
	static REAL32			getHeightRatioBy768(void) { return m_sfGuiResolutionHeightInGame; }
	static REAL32			getWidthRatioBy768(void) { return m_sfGuiResolutionWidthInGame; }

	static void				ChangeResolution(i3Viewer* pViewer);
	static bool				isChangeResolution(void);

	static i3GuiObjBase*	getCapturedMouseControl(void)					{ return m_spCapturedMouseControl;	}
	static void				setCapturedMouseControl( i3GuiObjBase* pCtrl)	{ m_spCapturedMouseControl = pCtrl;	}

	static i3GuiObjBase*	getFocusControl()								{ return m_spFocusControl;			}
	static void				setFocusControl( i3GuiObjBase* pCtrl)			{ m_spFocusControl = pCtrl;			}

	static UINT32			getMouseButtonState()							{ return m_sMouseButtonState;		}

	static void				setGuiDisableInput( bool bTrue)					{ m_sbDisableInput = bTrue;			}
	static bool				getGuiDisableInput(void)						{ return m_sbDisableInput;			}	

	// TextNode의 사용방법을 결정합니다.
	static void				setTextNodeUsage( I3GUI_TEXT_USAGE_TYPE type)	{ m_sTextUsageType = type;			}
	static I3GUI_TEXT_USAGE_TYPE	getTextNodeUsage(void)					{ return m_sTextUsageType;			}

	void					_Rec_ChangeUpdate( i3GuiObjBase * pBase);

	static void				NoticeGuiObjBaseDestroy(i3GuiObjBase * pObj);

	// 마우스 STATE 관련
protected:
	UINT32					m_mkPress = 0;				// I3I_MOUSE
	UINT32					m_mkStroke = 0;				// I3I_MOUSE
	UINT32					m_msMouseState = 0;			// I3GUI_MOUSE_STATE
	UINT32					m_mkLastStroke = 0;			// I3I_MOUSE
	REAL32					m_rClickTimeStamp = 0.0f;
	REAL32					m_rDoubleClickTime = 0.0f;
	REAL32					m_rRepeatTimeStamp[3] = { 0.0f, 0.0f, 0.0f };	// LBUTTON, RBUTTON, MBUTTON
public:
	void					SET_MOUSE_STATE(UINT32 idx, UINT32 key, UINT32 down, UINT32 up, UINT32 dclick, UINT32 repeat, UINT32 mkOldPress);
};

#endif // __I3GUIROOT_H__
