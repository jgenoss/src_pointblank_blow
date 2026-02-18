// ============================================================================
//
// File: i3UIFrameWnd.h
//
// Desc: Graphic User Interface Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================
#ifndef __I3UI_FRAME_WINDOW_H__
#define __I3UI_FRAME_WINDOW_H__

#include "i3UIDefines.h"
#include "i3UICaptionControl.h"
#include "i3UIScrollBar.h"

// ============================================================================
//
// i3UIFrameWnd : Gui 메인 구성 요소. i3UICaptionControl 들을 생성및 관리
//
// ============================================================================

class i3UIManager;

class I3_EXPORT_FRAMEWORK i3UIFrameWnd : public i3UICaptionControl
{
	I3_EXPORT_CLASS_DEFINE(i3UIFrameWnd, i3UICaptionControl);

	friend class i3UIManager;
protected:
	i3UIControl *	m_pFocusCtrl = nullptr;

	i3UIControl *	m_pPreFocusCtrl = nullptr;	///<< Focus 현재 프레임에서 설정되지만 FrameWork에서 처리시 알 수 없어서 이 변수로 대체합니다.
	
	//윈도우의 크기(클리핑된 영역 포함)
	INT32			m_nMaxWidth = 0;
	INT32			m_nMaxHeight = 0;

	INT32			m_nPosX = 0;
	INT32			m_nPosY = 0;

	//ScrollBar
	i3UIScrollBar*	m_pUIVScrollBar = nullptr;
	i3UIScrollBar*	m_pUIHScrollBar = nullptr;

	// FrameWnd 가 setEnable 되었을 때 Focus를 자동으로 가져올지 여부
	bool			m_bUpdateFocusWhenEnable = true;
	bool			m_bTopFrameWnd = false;
	bool			m_bLoaded = false;		// 한번 로딩되었으면 true 이후부터는 Reordering을 하도록
	
	
	INT32			_getVScrollPos( void);
	INT32			_getHScrollPos( void);
	
	void			_OnScroll( INT32 hPos, INT32 vPos);
	void			_UpdateScrollBarValue( void);
	virtual void	_UpdateShapeState(void) override;
public:
	i3UIFrameWnd();
	virtual			~i3UIFrameWnd();

	virtual bool	isFrameWnd(void)						{ return true;}

	virtual bool	isTopFrameWnd(void)						{ return m_bTopFrameWnd;}
	virtual void	setTopFrameWndState(bool bTop)			{ m_bTopFrameWnd = bTop;}

	void			SetFocusCtrlNULL( void)					{	m_pFocusCtrl = nullptr;	}
	void			SetFocusCtrl( i3UIControl * pCtrl);
	i3UIControl*	getFocusCtrl(void)						{	return m_pFocusCtrl;}
	virtual i3UIControl *	OnQueryFocusControl(void);

	void			SetPreFocusCtrl( i3UIControl * pCtrl)	{ m_pPreFocusCtrl = pCtrl; }
	i3UIControl *	GetPreFocusCtrl( void)					{ return m_pPreFocusCtrl; }

	void			SetUpdateFocusWhenEnable( bool bEnable) { m_bUpdateFocusWhenEnable = bEnable; }

	virtual void	EnableCtrl(bool bTrue, bool bRecursive = false) override;
			void	EnableExclusively(void);
	
	virtual bool	OnKillFocus( i3UIControl * pByCtrl = nullptr) override;
	virtual bool	OnSetFocus(void) override;
	virtual void	OnSize( REAL32 cx, REAL32 cy) override;

	virtual void	OnDraw( i3UIRenderer * pRenderer) override;

	virtual void	OnLButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnLButtonDblClk( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonDown( UINT32 nFlag, POINT point) override;
	virtual void	OnRButtonUp( UINT32 nFlag, POINT point) override;

	virtual void	OnMouseMove( UINT32 nFlag, POINT point) override;
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point) override;

	virtual void	OnEnabled( bool bEnable) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;

	virtual	void	OnKeyDown( UINT32 nKey) override;
	virtual void	OnKeyUp( UINT32 nKey) override;

	virtual bool	isOnMouse(  I3UI_OVERLAP_INFO * pInfo) override;
	
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;

	virtual void	OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2) override;

	virtual void	OnBindTemplate(void) override;

	static	void	RegisterLuaFunction(LuaState * pState);
};

#endif //__I3UI_WINDOW_H__
