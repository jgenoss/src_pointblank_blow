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
	I3_CLASS_DEFINE(i3GuiRoot);

protected:
	i3AttrSet*				m_pGuiAttrSet;			
	i3Node*					m_pNode;				// SceneGraph용 Node

	i3GuiWindow*			m_pOldFocusWindow;		// 최상위 Window중에서 이전에 Focus를 가지고 있던 Window입니다.

	INT32					m_nWidth;
	INT32					m_nHeight;
	BOOL					m_bDisableInput;
	UINT32					m_MouseButtonState;

	i3GuiObjBase*			m_pFocusControl;
	i3GuiObjBase*			m_pCapturedMouseControl;

	UINT32					m_mkPress;				// I3I_MOUSE
	UINT32					m_mkStroke;				// I3I_MOUSE
	UINT32					m_msMouseState;			// I3GUI_MOUSE_STATE
	UINT32					m_mkLastStroke;			// I3I_MOUSE
	REAL32					m_rClickTimeStamp;
	REAL32					m_rDoubleClickTime;
	REAL32					m_rRepeatTimeStamp[3];	// LBUTTON, RBUTTON, MBUTTON

	I3GUI_TEXT_USAGE_TYPE	m_TextUsageType;

	// Control notify handler functions
	I3GUI_CONTROL_NOTIFY	m_NotifyQueue[I3GUI_MAX_NOTIFY];
	INT32					m_idxStart;
	INT32					m_idxLast;
	BOOL					m_bFulled;

	//
	BOOL					m_bChangeResolution;

protected:
	void					_Rec_BuildSceneGraph( i3GuiObjBase * pObj);
	i3GuiControl *			_Rec_FindChildControlByName( i3GuiControl * pCtrl, const char * pszName);

public:
	i3GuiRoot(void);
	virtual					~i3GuiRoot( void);

	virtual void			OnUpdate( REAL32 rDeltaSeconds);
	virtual BOOL			OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE);

	BOOL					Create( i3Node *pParentRoot );	

	i3Node*					getSceneGraphNode()								{ return m_pNode;					}

	void					SetViewer(i3Viewer* pViewer, BOOL bNoResize = TRUE);

	void					AddChild(i3GuiWindow* pWindow);
	void					RemoveChild(i3GuiWindow* pWindow);
	void					RemoveAllChild();
	i3GuiControl *			FindChildControlByName( const char * pszName);

	// TextNode의 사용방법을 결정합니다.
	void					setTextNodeUsage( I3GUI_TEXT_USAGE_TYPE type)	{ m_TextUsageType = type;			}
	I3GUI_TEXT_USAGE_TYPE	getTextNodeUsage(void)							{ return m_TextUsageType;			}

	// Index의 Window의 활성화 비활성화를 설정합니다.
	void					SetEnableWindowByIndex(INT32 nIndex, BOOL bTrue = TRUE);

	// 현재 Mouse가 올려져 있는 Window를 반환합니다.
	i3GuiWindow*			GetOnMouseWindow(i3InputMouse* pMouse);

	// 해당 요소를 가장 상위의 Layer로 설정합니다.
	void					SetTopLayer(i3GuiWindow* pWindow);	

	// 
	void					setWidth( INT32 nWidth)							{ m_nWidth = nWidth;				}
	INT32					getWidth()										{ return m_nWidth;					}

	void					setHeight( INT32 nHeight)						{ m_nHeight = nHeight;				}
	INT32					getHeight()										{ return m_nHeight;					}

	void					setGuiDisableInput( BOOL bTrue)					{ m_bDisableInput = bTrue;			}
	BOOL					getGuiDisableInput(void)						{ return m_bDisableInput;			}
	UINT32					getMouseButtonState()							{ return m_MouseButtonState;		}
	UINT32					getMouseState(void)								{ return m_msMouseState;			} // I3GUI_MOUSE_STATE
	
	i3GuiObjBase*			getFocusControl()								{ return m_pFocusControl;			}
	void					setFocusControl( i3GuiObjBase* pCtrl)			{ m_pFocusControl = pCtrl;			}

	i3GuiObjBase*			getCapturedMouseControl(void)					{ return m_pCapturedMouseControl; }
	void					setCapturedMouseControl( i3GuiObjBase* pCtrl)	{ m_pCapturedMouseControl = pCtrl;}

	void					ClearMouseState(void);

	virtual void			SetEnable( BOOL bEnable);

	void					OnInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds);
	UINT32					OnMouseInput(i3InputDeviceManager* pMgr,REAL32 rDeltaSeconds);	
	void					SetDoubleClickTime(REAL32 rSeconds = __RT(0.5f));	// default 500ms

	// Control notify handler functions
	void					PushControlNotify(INT32 nID,INT32 nEvent,INT32 nPriParam,INT32 nSecParam);
	BOOL					PopControlNotify(I3GUI_CONTROL_NOTIFY* pNotify);
	void					ResetControlNotify(void);

	void					BuildSceneGraph(void);

	BOOL					isChangeResolution(void)						{ return m_bChangeResolution;		}

	//
	static BOOL				isControlOnceUpdate(void);
	static i3Viewer*		getViewer(void);	
	static REAL32			getGuiResolutionWidth(void);
	static REAL32			getGuiResolutionHeight(void);
	static void				ChangeResolution(i3Viewer* pViewer);

	void					_Rec_ChangeUpdate( i3GuiObjBase * pBase);
};

#endif // __I3GUIROOT_H__
