/**
 * \file	UI\UIPopupBase.h
 *
 * Declares the user interface popup base class.
 */


#if !defined( __UI_POPUP_BASE_H__)
#define __UI_POPUP_BASE_H__

#include "UIBase.h"

class UIPopupBase : public UIBase
{
	I3_ABSTRACT_CLASS_DEFINE( UIPopupBase, UIBase);

protected:
	i3UIScene *		m_pScene;
	i3UIFrameWnd *	m_pPopupFrame;
	i3UIFrameWnd *	m_pBGFrame;

	VEC2D			m_vOriginalSize;
	REAL32			m_SlideScale;
	bool			m_bScaleAnimation;
	i3::vector<i3UIControl*> m_ChildList;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	void			_ApplyBGSize();
	void			_ReSizeBG();

	/** \brief 자신을 닫는다. */
	virtual void	_OnClose( void);

public:
	UIPopupBase();
	virtual ~UIPopupBase();
	
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override {}

	virtual bool	OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;

	virtual bool	OnKeyInput(i3InputDeviceManager * pInputMng) override;
	
	virtual bool	OnKey_Enter( void) override;

	virtual bool	OnKey_Escape( void) override;
	
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual void	OnEntranceEnd( void) override;

	virtual bool	OnExitStart( void) override;

	UIMainFrame *	GetMainFrame( void);

	/** \brief OK 버튼에 대한 처리
		\desc Lua에서 버튼 입력에 대한 경우 true 반환시 자동으로 닫습니다. */
	virtual bool	OnOKButton( void) { return true;}

	/** \brief Close 버튼에 대한 처리
		\desc Lua에서 버튼 입력을 받으면 호출되며 true를 반환하면 창을 닫습니다. */
	virtual bool	OnCloseButton( void) { return true;}

	//--------------------------------------------------------------------------//
	//							Old GUI를 사용하는 Function입니다.				//
	virtual bool	OnLinkControl( const i3::vector_set<struct STRCINFO*>* pList, i3GameNode * pControl) { return true;}
	virtual void	OnInitControl( void) {}
	virtual void	OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify) override {}


	//delay pop을 사용할 경우에 재정의 하자.
	virtual void	Enter( void) {}
	virtual void	OnEnter( void) {}
	virtual void	Leave( void) {}
	virtual void	OnLeave( void) {}

	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif
