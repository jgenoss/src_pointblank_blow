// ============================================================================
//
// File: i3UIFrameWnd.cpp
//
// Desc: Graphic User Interface Components
//
// Copyright (c) Zepetto Inc. All rights reserved.
//
// ============================================================================
#include "i3FrameworkPCH.h"
#include "i3UICaptionControl.h"
#include "i3UIFrameWnd.h"
#include "i3UIManager.h"
#include "i3UITemplate_FrameWnd.h"
#include "i3UIEditBox.h"
#include "i3UIRenderer.h"

#include "i3UIComboBase.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

// ============================================================================
//
// i3UIFrameWnd
//
// ============================================================================

I3_CLASS_INSTANCE( i3UIFrameWnd);

i3UIFrameWnd::i3UIFrameWnd()
{
	// default caption
	if( i3UI::getToolDevMode())
		SetCaption( L"Frame Window");
}

i3UIFrameWnd::~i3UIFrameWnd()
{
	I3_SAFE_RELEASE( m_pUIVScrollBar);
	I3_SAFE_RELEASE( m_pUIHScrollBar);
}

void i3UIFrameWnd::_UpdateShapeState(void)
{
	if( m_pTemplate == nullptr)
		return;

	INT32 i;

	for( i = 0; i < I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT; ++i)
	{
		setShapeVisible( i, true);
	}

	if( !isActivated())
	{
		setShapeVisible( I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT, true);
		setShapeVisible( I3UI_FRAMEWND_SHAPE_CLIENT, false);
	}
	else
	{
		setShapeVisible( I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT, false);
	}

	i3UICaptionControl::_UpdateShapeState();
}

void i3UIFrameWnd::_UpdateScrollBarValue( void)
{
	if( m_pUIVScrollBar == nullptr || m_pUIHScrollBar == nullptr)
		return;

	INT32 nSize = 20;
	
	// Vertical
	m_pUIVScrollBar->setSize( (REAL32)nSize, getHeight()-nSize);
	m_pUIVScrollBar->setPos( getWidth()-nSize, 0.f);

	m_pUIVScrollBar->setScrollRange( 0, m_nMaxHeight);
	m_pUIVScrollBar->setPageSize( (INT32)(m_pRects[I3UI_FRAMEWND_SHAPE_CLIENT].bottom));
	m_pUIVScrollBar->setScrollPos( m_nPosY);

	if( !m_pUIVScrollBar->isActivated())
	{
		m_pUIVScrollBar->EnableCtrl( false);
	}
	else if( m_pUIVScrollBar->isActivated())
	{
		m_pUIVScrollBar->EnableCtrl( true);
	}

	// Horizontal
	m_pUIHScrollBar->setSize( getWidth()-nSize, (REAL32)nSize);
	m_pUIHScrollBar->setPos( 0.f, getHeight()-nSize);

	m_pUIHScrollBar->setScrollRange( 0, m_nMaxWidth);
	m_pUIHScrollBar->setPageSize( (INT32)(m_pRects[I3UI_FRAMEWND_SHAPE_CLIENT].right));
	m_pUIHScrollBar->setScrollPos( m_nPosX);

	if( !m_pUIHScrollBar->isActivated())
	{
		m_pUIHScrollBar->EnableCtrl( false);
	}
	else if( m_pUIHScrollBar->isActivated())
	{
		m_pUIHScrollBar->EnableCtrl( true);
	}
}

void i3UIFrameWnd::EnableCtrl(bool bTrue, bool bRecursive)
{
	if( bTrue == false)
	{
		// 이 경우 Memory에서 해제될 수 있다.
		// Child가 먼저 해제되므로 FocusCtrl을 초기화해야함.
		m_pFocusCtrl = nullptr;
	}

	i3UICaptionControl::EnableCtrl( bTrue, bRecursive);

	// Texture Loading이 완료되기까지 Display를 보류한다.
	if( bTrue && isTopFrameWnd())
	{
		if( m_bLoaded)
		{
			addState( I3UI_STATE_REORDERING);
		}
		else
		{
			addState( I3UI_STATE_WAITLOAD);
			HideShapes( true);
		}
	}

	{
		i3UIManager * pManager = i3UI::getManager();
		if( pManager != nullptr)
			pManager->EnableWindow( this, bTrue, false, m_bUpdateFocusWhenEnable);
	}
}

void i3UIFrameWnd::EnableExclusively(void)
{
	i3UICaptionControl::EnableCtrl( true, true); 

	// Texture Loading이 완료되기까지 Display를 보류한다.
	addState( I3UI_STATE_WAITLOAD);
	HideShapes( true);

	{
		i3UIManager * pManager = i3UI::getManager();

		pManager->EnableWindow( this, true, true);
	}
}

void i3UIFrameWnd::OnUpdate( REAL32 tm)
{
	if( isState( I3UI_STATE_WAITLOAD))
	{
		if( isTopFrameWnd() && _PrepareShow())
		{
			// Texture의 Loading이 모두 완료되었다!
			OnCompletedLoading();

			if( m_hSprites != nullptr && m_nShapeCount > 0)
			{
				AdjustSpriteOrder( m_hSprites[m_nShapeCount-1], true);
			}

			HideShapes( false);

			m_bLoaded = true;

			//g_pUIRenderer->SaveVRAMCache( "D:\\cache.png");
		}
		else
		{
			HideShapes( true);
		}
	}
	else if( isState( I3UI_STATE_REORDERING))
	{
		//HideShapes( false);

		if( isTopFrameWnd() && _PrepareShow())
		{
			// Texture의 Loading이 모두 완료되었다!
			OnCompletedLoading();

			if( m_hSprites != nullptr && m_nShapeCount > 0)
			{
				AdjustSpriteOrder( m_hSprites[m_nShapeCount-1], true);
			}

			removeState( I3UI_STATE_REORDERING);
		}
	}
	
	i3UICaptionControl::OnUpdate( tm);
}

void i3UIFrameWnd::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);

	if( bEnable)
	{
		_UpdateScrollBarValue();
	}
	else
	{
		if( m_pUIHScrollBar != nullptr)	m_pUIHScrollBar->EnableCtrl( bEnable);
		if( m_pUIVScrollBar != nullptr)	m_pUIVScrollBar->EnableCtrl( bEnable);
	}
}


i3UIControl * i3UIFrameWnd::OnQueryFocusControl(void)
{
	if( getFirstChild() == nullptr)
		return this;

	i3UIControl * pChild = (i3UIControl *) getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::kind_of<i3UIEditBox*>(pChild))
		{
			i3UIEditBox * pEditBox = (i3UIEditBox *) pChild;

			if( pEditBox->isReadOnly() == false)
				return pEditBox;
		}

		pChild = (i3UIControl *) pChild->getNext();
	}

	return (i3UIControl *) getFirstChild();
}

void i3UIFrameWnd::SetFocusCtrl( i3UIControl * pCtrl)
{
	if( m_pFocusCtrl == pCtrl)
		return;

	if( m_pFocusCtrl != nullptr)
	{
		m_pFocusCtrl->OnKillFocus( pCtrl);
	}

	m_pFocusCtrl = pCtrl;

	if( m_pFocusCtrl != nullptr)
	{
		bool bOK = false;
		i3GameNode * pParent = pCtrl;
		while( pParent && !i3::same_of<i3UIScene*>(pParent))
		{
			if( pParent == this)
			{
				bOK = true;
				break;
			}

			pParent = pParent->getParent();
		}

		if( bOK == false)
		{
			I3PRINTLOG(I3LOG_FATAL,  "SetFocusCtrl is invalid. pCtrl is no child member..");
		}
			
		//I3TRACE( "i3UIFrameWnd::SetFocusCtrl : %s\n", m_pFocusCtrl->GetName() );
		m_pFocusCtrl->OnSetFocus();
	}
	else
	{
		//I3TRACE( "i3UIFrameWnd::SetFocusCtrl : nullptr\n");
	}
}

bool i3UIFrameWnd::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	if( m_pFocusCtrl != nullptr)
	{
		if( m_pFocusCtrl != this)
			m_pFocusCtrl->OnKillFocus( pByCtrl);
		else
			i3UICaptionControl::OnKillFocus( pByCtrl);
	}

	return true;
}

bool i3UIFrameWnd::OnSetFocus(void)
{
	if( m_pFocusCtrl != nullptr)
	{
		if( m_pFocusCtrl != this)
			m_pFocusCtrl->OnSetFocus();
		else
			i3UICaptionControl::OnSetFocus();
	}

	return true;
}

void i3UIFrameWnd::OnSize( REAL32 cx, REAL32 cy)
{
	/*if( getResizeStyle() == I3UI_CTRL_RESIZE_FIXED_PIXEL)
	{
		ResizeShapeForFixedPixel( cx, cy,
					I3UI_FRAMEWND_SHAPE_TOPLEFT, I3UI_FRAMEWND_SHAPE_TOP, I3UI_FRAMEWND_SHAPE_TOPRIGHT,
					I3UI_FRAMEWND_SHAPE_LEFT, I3UI_FRAMEWND_SHAPE_CLIENT, I3UI_FRAMEWND_SHAPE_RIGHT,
					I3UI_FRAMEWND_SHAPE_BOTTOMLEFT, I3UI_FRAMEWND_SHAPE_BOTTOM, I3UI_FRAMEWND_SHAPE_BOTTOMRIGHT,
					I3UI_FRAMEWND_SHAPE_DISABLE_CLIENT);
	}*/

	m_nPosX = m_nPosY = 0;

	i3UICaptionControl::OnSize( cx, cy);

	_UpdateScrollBarValue();
}

void i3UIFrameWnd::OnDraw( i3UIRenderer * pRenderer)
{
	// 자식에 들어가기전 Depth
	pRenderer->DepthPlus();
	i3UICaptionControl::OnDraw( pRenderer);
}

void	i3UIFrameWnd::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if( i3UI::getDisableUIInput())
		return;

	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnLButtonDown( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnLButtonDown( nFlag, point);
}

void	i3UIFrameWnd::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnLButtonUp( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnLButtonUp( nFlag, point);
}

void	i3UIFrameWnd::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	if( i3UI::getDisableUIInput())
		return;

	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnLButtonDblClk( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnLButtonDblClk( nFlag, point);
}

void	i3UIFrameWnd::OnMButtonDown( UINT32 nFlag, POINT point)
{
	if( i3UI::getDisableUIInput())
		return;

	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnMButtonDown( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnMButtonDown( nFlag, point);
}

void	i3UIFrameWnd::OnMButtonUp( UINT32 nFlag, POINT point)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnMButtonUp( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnMButtonUp( nFlag, point);
}

void	i3UIFrameWnd::OnRButtonDown( UINT32 nFlag, POINT point)
{
	if( i3UI::getDisableUIInput())
		return;

	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnRButtonDown( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnRButtonDown( nFlag, point);
}

void	i3UIFrameWnd::OnRButtonUp( UINT32 nFlag, POINT point)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnRButtonUp( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnRButtonUp( nFlag, point);
}

void	i3UIFrameWnd::OnMouseMove( UINT32 nFlag, POINT point)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnMouseMove( nFlag, point);
		return;
	}
	
	i3UICaptionControl::OnMouseMove( nFlag, point);
}

void	i3UIFrameWnd::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnMouseWheel( nFlag, zDelta, point);
		return;
	}
	
	i3UICaptionControl::OnMouseWheel( nFlag, zDelta, point);
}

void	i3UIFrameWnd::OnKeyDown( UINT32 nKey)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnKeyDown( nKey);
		return;
	}
	
	i3UICaptionControl::OnKeyDown( nKey);
}

void	i3UIFrameWnd::OnKeyUp( UINT32 nKey)
{
	if((m_pFocusCtrl != nullptr) && (m_pFocusCtrl != this))
	{
		m_pFocusCtrl->OnKeyUp( nKey);
		return;
	}
	
	i3UICaptionControl::OnKeyUp( nKey);
}

static bool sRec_OnMouseComboBox( i3UIControl * pCtrl, I3UI_OVERLAP_INFO * pInfo)
{
	i3UIControl * pChild;

	if( i3::kind_of<i3UIComboBase*>(pCtrl) ) 
	{
		if( pCtrl->isOnMouse( pInfo))
			return true;
	}

	pChild = (i3UIControl*) pCtrl->getFirstChild();

	while( pChild)
	{
		if( sRec_OnMouseComboBox( pChild, pInfo))
			return true;

		pChild = (i3UIControl*) pChild->getNext();
	}

	return false;
}

bool i3UIFrameWnd::isOnMouse( I3UI_OVERLAP_INFO * pInfo)
{
	if( i3UIControl::isOnMouse( pInfo) == false)
	{
		// ComboBox는 화면을 벗어날 수 있다.
		// ComboBox가 있다면 체크
		return sRec_OnMouseComboBox( this, pInfo);
	}

	return true;
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4	UI_FRAMEWND
	{
		UINT8		m_ID[4] = { 'U', 'I', 'F', '1' };
		INT32		m_Reserved[20] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32	i3UIFrameWnd::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	pack::UI_FRAMEWND	data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3UIFrameWnd::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32	Rc, Result = 0;
	pack::UI_FRAMEWND data;

	i3GameNode::SetEnable(false);

	Result = i3UICaptionControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	OnCreate();

	return Result;
}

bool i3UIFrameWnd::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	return true;
}

bool i3UIFrameWnd::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	OnCreate();

	return true;
}

void i3UIFrameWnd::OnNotify( i3UIControl * pCtrl, I3UI_EVT evt, INT32 param1, INT32 param2)
{
	switch( evt)
	{
	case I3UI_EVT_SCROLL:
		{
			if( pCtrl == m_pUIVScrollBar)
				_OnScroll( _getHScrollPos(), param1 );
			else if( pCtrl == m_pUIHScrollBar)
				_OnScroll( param1, _getVScrollPos() );
		}
		break;
	default:
		break;
	}

	i3UICaptionControl::OnNotify( pCtrl, evt, param1, param2);
}

void i3UIFrameWnd::_OnScroll( INT32 hPos, INT32 vPos)
{
	m_nPosX = hPos;
	m_nPosY = vPos;

	//I3TRACE("hPos:%d, vPos:%d\n", m_nPosX, m_nPosY);

	//_UpdateScrollBarValue();
}

INT32 i3UIFrameWnd::_getVScrollPos( void)
{
	INT32 nPos = 0;

	if( m_pUIVScrollBar != nullptr)	// 또는 invisible인경우...는? 
	{
		nPos = m_pUIVScrollBar->getScrollPos();
	}
	
	return nPos;
}

INT32 i3UIFrameWnd::_getHScrollPos( void)
{
	INT32 nPos = 0;

	if( m_pUIHScrollBar != nullptr)	// 또는 invisible인경우...는? 
	{
		nPos = m_pUIHScrollBar->getScrollPos();
	}
	
	return nPos;
}

void i3UIFrameWnd::OnBindTemplate(void)
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	//V ScrollBar
	if( m_pUIVScrollBar == nullptr)
	{
		m_pUIVScrollBar = i3UIScrollBar::new_object();
		AddControl( m_pUIVScrollBar);
		m_pUIVScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pUIVScrollBar->addStyle( I3UI_STYLE_SLAVE);
	}

	//H ScrollBar
	if( m_pUIHScrollBar == nullptr)
	{
		m_pUIHScrollBar = i3UIScrollBar::new_object();
		AddControl( m_pUIHScrollBar);
		m_pUIHScrollBar->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pUIHScrollBar->addStyle( I3UI_STYLE_SLAVE);
	}

	//Set Template
	m_pUIVScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_FRAMEWND_SUBTEMPLATE_VSCROLL));
	m_pUIHScrollBar->SetTemplate( m_pTemplate->getSubTemplate( I3UI_FRAMEWND_SUBTEMPLATE_HSCROLL));
}



//////////////////////////////////////////////////////////////////////////
// Lua glue

extern "C" int i3UIFrameWnd_setUpdateFocusWhenEnable( lua_State* L)
{
	i3UIFrameWnd* pCtrl = (i3UIFrameWnd*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		bool	bEnable = i3Lua::GetBooleanArg( L, 2, 0);
		pCtrl->SetUpdateFocusWhenEnable( bEnable);
	}

	return 1;
}

LuaFuncReg i3UIFrameWndGlue[] = 
{
	{	"uiFrameWnd_setUpdateFocusWhenEnable",		i3UIFrameWnd_setUpdateFocusWhenEnable},

	{nullptr, nullptr}
};

void i3UIFrameWnd::RegisterLuaFunction( LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIFrameWndGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIFrameWndGlue[i].name, i3UIFrameWndGlue[i].func);
	}
}

