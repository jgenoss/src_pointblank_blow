// ============================================================================
//
// File: i3GuiWindow.h
//
// Desc: Graphic User Interface Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================
#ifndef __I3GUI_WINDOW_H__
#define __I3GUI_WINDOW_H__

#include "i3GuiDefines.h"
#include "i3GuiObjBase.h"

#define I3GUI_WINDOW_STATE_HIDE				0x00000001		// 화면에 Window가 보이지 않는 상태.
#define I3GUI_WINDOW_STATE_SHOW				0x00000002		// 화면에 Window는 보이나 아직 Focus된 상태는 아님.
#define I3GUI_WINDOW_STATE_FOCUS			0x00000004		// 화면에 Window가 보이며, 사용자의 입력을 받을수 있는 Focus상태.
#define I3GUI_WINDOW_STATE_CLOSE			0x00000008		// HIDE상태와 보이는 것은 같지만, 이것은 아예 Window가 닫힌 상태.
#define I3GUI_WINDOW_STATE_ALLWAYS_FOCUS	0x00000010		// 항상 Focus 상태를 유지 하는 Window입니다.
#define I3GUI_WINDOW_AUTO_DELETE			0x00000020		// 자동 삭제 스테이트.

class i3GuiControl;

// ============================================================================
//
// i3GuiWindow : Gui 메인 구성 요소. i3GuiControl 들을 생성및 관리
//
// ============================================================================
class I3_EXPORT_GUI i3GuiWindow : public i3GuiObjBase
{
	I3_EXPORT_CLASS_DEFINE(i3GuiWindow, i3GuiObjBase);

protected:
	INT32			m_nLayerCount = 0;
	UINT32			m_state = 0;

public:    
	virtual void 	OnCreate(void);							// 처음 생성되었을때 호출됩니다.
	virtual void 	OnActive(void);							// 활성화 될때 호출됩니다.
	virtual void 	OnDisable(void);						// 비활성화 될때 호출됩니다.
	virtual void 	OnChangeFocus(void);					// Focus를 일을때 호출됩니다.
	virtual void 	OnPushControl(void);					// 사용자의 입력을 받게 될때 호출됩니다.

	virtual	void	UserInput(REAL32 fX, REAL32 fY, UINT32 state) override;
	
	virtual void 	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	SetEnable(bool bTrue) override;
	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
		
	// Window Layer를 설정한다.
	void			setLayerCount(INT32 nCount)						{ m_nLayerCount = nCount;						}
	INT32			getLayerCount()									{ return m_nLayerCount;							}
	
	// Window Object의 상태처리
	void			setState(UINT32 state)							{ m_state = state;								}
	UINT32			getState()										{ return m_state;								}
	void			addState(UINT32 state)							{ m_state |= state;								}
	void			removeState(UINT32 state)						{ m_state &= ~state;							}

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif //__I3GUI_WINDOW_H__
