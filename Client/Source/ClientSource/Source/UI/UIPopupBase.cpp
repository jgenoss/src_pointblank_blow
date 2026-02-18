#include "pch.h"
#include "UIPopupBase.h"

#include "UIFramework.h"
#include "UIMainFrame.h"
#include "UIBattleFrame.h"
#include "UIUtil.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopup_OK( LuaState * L)
	{
		bool bNoFind = false;
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		if( pFrame != nullptr)
		{
			INT32 idx = pFrame->FindPopup( L);
			if( idx != -1)
			{
				if( pFrame->IsOpenedPopup(static_cast<UIPOPUPWIN>(idx)))
				{
					UIPopupBase * pPopup = pFrame->GetPopup( static_cast<UIPOPUPWIN>(idx));
					I3ASSERT( pPopup);

					if( pPopup->OnOKButton())
					{
						//pFrame->TogglePopup( static_cast<UIPOPUPWIN>(idx), false);
						pFrame->ClosePopup(static_cast<UIPOPUPWIN>(idx));
					}
				}
			}
			else
				bNoFind = true;
		}
		else
			bNoFind = true;

		if( bNoFind)
		{
			UIPopupBase * pMsgBox = (UIPopupBase*) g_pFramework->GetMessageBoxManager()->FindMsgBox( L);
			if( pMsgBox)
			{
				if( pMsgBox->OnOKButton())
					pMsgBox->OnExitStart();
			}
		}

		return 0;
	}

	int UIPopup_Close( LuaState * L)
	{
		bool bNoFind = false;
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
		if( pFrame != nullptr)
		{
			INT32 idx = pFrame->FindPopup( L);
			if( idx != -1)
			{
				UIPopupBase * pPopup = pFrame->GetPopup( static_cast<UIPOPUPWIN>(idx));
				I3ASSERT( pPopup);
				if(	pPopup->OnCloseButton())
				{
					pFrame->TogglePopup( static_cast<UIPOPUPWIN>(idx), false);
				}
			}
			else
				bNoFind = true;
		}
		else
			bNoFind = true;

		if( bNoFind)
		{
			UIPopupBase * pMsgBox = (UIPopupBase*) g_pFramework->GetMessageBoxManager()->FindMsgBox( L);
			if( pMsgBox)
			{
				if( pMsgBox->OnCloseButton())
					pMsgBox->OnExitStart();
			}
		}

		return 0;
	}
}

LuaFuncReg UIPopup_Glue[] = {
	{"OK",					UIPopup_OK	},
	{"Close",				UIPopup_Close},			// 창 닫기
	{nullptr,					nullptr}
};

//------------------------------------------------------------------------------//

//I3_ABSTRACT_CLASS_INSTANCE( UIPopupBase, UIBase);
I3_CLASS_INSTANCE( UIPopupBase);

UIPopupBase::UIPopupBase()
{
	m_pScene = nullptr;
	m_pPopupFrame = nullptr;

	m_bScaleAnimation = false;
	m_slide.SetTargetTime(false, 0.f );
	m_slide.SetTargetTime(true, 0.f);

	m_pBGFrame = nullptr;
}

UIPopupBase::~UIPopupBase()
{
}

/*virtual*/ void UIPopupBase::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBase::_InitializeAtLoad( pScene);

	if( m_pScene == nullptr)
	{
		m_pScene = pScene;

		LuaState * L = m_pScene->getLuaContext();

		if( L != nullptr)
		{
			INT32 i;
			for( i = 0; UIPopup_Glue[i].name; i++)
			{
				i3Lua::RegisterFunc( L, UIPopup_Glue[ i].name, UIPopup_Glue[i].func);
			}
		}

		m_pPopupFrame = (i3UIFrameWnd*) pScene->FindChildByName( "main");
		if( m_pPopupFrame != nullptr)
		{
			m_ChildList.clear();
			m_pPopupFrame->GetChildControls( m_ChildList, false);

			i3Vector::Copy( &m_vOriginalSize, m_pPopupFrame->getSize());

			m_slide.ClearFrameWndList();
			m_slide.AddFrameWnd(m_pPopupFrame, UISlide::POPUP);

			if( m_bScaleAnimation)
			{
				m_slide.SetTargetTime(false, 1.f / 10.f );
				m_slide.SetTargetTime(true, 1.f / 5.f);
			}
			else
			{
				m_slide.SetTargetTime(false, 0.f );
				m_slide.SetTargetTime(true, 0.f);
			}
		}
		else
		{
			m_slide.SetTargetTime(false, 0.f);
			m_slide.SetTargetTime(true, 0.f);
		}

		m_pBGFrame = (i3UIFrameWnd*) pScene->FindChildByName( "bg");
		if( m_pBGFrame == nullptr)
			m_pBGFrame = (i3UIFrameWnd*) pScene->FindChildByName( "BG");
		_ApplyBGSize();

		// 이 경우에는 팝업스타일로 정할 것..
		
	}
}

/*virtual*/ void UIPopupBase::_ClearAtUnload( i3UIScene * pScene)
{
	UIBase::_ClearAtUnload( pScene);

	m_pScene = nullptr;
	m_pPopupFrame = nullptr;
	m_pBGFrame = nullptr;
	m_ChildList.clear();
}

void UIPopupBase::_ApplyBGSize( void)
{
	enum { Add_Width = 10 };

	if( m_pBGFrame != nullptr)
	{
		m_pBGFrame->setSize( (REAL32) g_pViewer->GetViewWidth(), (REAL32)(g_pViewer->GetViewHeight() + Add_Width) );

		REAL32 pos_offset_x = -( (REAL32)i3UI::getManager()->getDisplayOffsetX() * i3UI::getManager()->getScreenStretchRatioX() );
		REAL32 pos_offset_y = -( (REAL32)i3UI::getManager()->getDisplayOffsetY() * i3UI::getManager()->getScreenStretchRatioY() );

		m_pBGFrame->setPos(pos_offset_x, pos_offset_y);

		i3UIImageBox * pImgBox = (i3UIImageBox*) m_pBGFrame->FindChildByName( "i3UIImageBox");
		if( pImgBox != nullptr)
		{
			pImgBox->setSize( (REAL32) g_pViewer->GetViewWidth(), (REAL32)(g_pViewer->GetViewHeight() + Add_Width) );
		}
	}
}

void UIPopupBase::_ReSizeBG()
{
	if(m_pBGFrame == nullptr)
	{
		m_pBGFrame = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "bg");
		if( m_pBGFrame == nullptr)
			m_pBGFrame = (i3UIFrameWnd*) GetScene(0)->FindChildByName( "BG");
		_ApplyBGSize();
	}
}

void UIPopupBase::_OnClose( void)
{
	if( GetMainFrame() != nullptr)
	{
		UIPOPUPWIN type = GetMainFrame()->FindPopupByPointer( this);
		GetMainFrame()->ClosePopup( type);
	}
}

void UIPopupBase::OnUpdate( REAL32 rDeltaSeconds)
{

	UIBase::OnUpdate(rDeltaSeconds);


}
/*virtual*/ bool UIPopupBase::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		if( i3::same_of<i3UIManager*>(pObj))
		{
			I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

			if( (pNotify->m_nEvent == I3UI_EVT_CLICKED) || 
				(pNotify->m_nEvent == I3UI_EVT_R_CLICKED) ||
				(pNotify->m_nEvent == I3UI_EVT_M_CLICKED) ||
				(pNotify->m_nEvent == I3UI_EVT_SELITEM) )
			{
				if( pNotify->m_pFrameWnd != m_pFrameWnd)
				{
					if( isNoCloseButton())
					{
						if( GetMainFrame() != nullptr)
						{
							UIPOPUPWIN type = GetMainFrame()->FindPopupByPointer( this);
							GetMainFrame()->ClosePopup( type);
							return true;
						}
					}
				}
			}
		}
	}

	return UIBase::OnEvent( event, pObj, param2, code);
}

/*virtual*/ bool UIPopupBase::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	if(i3UI::getGlobalIme()->isFocused())
	{
		i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

		// EditBox에 Focus가 있어도, ESC 키는 동작합니다.
		if(pKeyboard->GetStrokeState( I3I_KEY_ESC) == false && 
			pKeyboard->GetStrokeState( I3I_KEY_ENTER) == false)	// UIBase에 엔터키에 대한 동작이 구현되어 있으므로 추가함.
		{
			return false;
		}
	}

	return UIBase::OnKeyInput(pInputMng);
}

/*virtual*/ bool UIPopupBase::OnKey_Enter( void)
{
	if( isIncludeOKButton())
	{
		if( OnOKButton())
		{
			_OnClose();
			return true;
		}
	}

	return false;
}

/*virtual*/ bool UIPopupBase::OnKey_Escape( void)
{
	// 기본적으로 닫기
	if( OnCloseButton())
	{
		_OnClose();
		return true;
	}

	return false;
}

/*virtual*/ bool UIPopupBase::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	_ReSizeBG();
	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3UIControl * pChild = m_ChildList[ i];
		pChild->SetVisible( false);
	}

	PlaySound( UISND_POPUP_OPEN);

	return true;
}

/*virtual*/ void UIPopupBase::OnEntranceEnd( void)
{
	UIBase::OnEntranceEnd();

	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3UIControl * pChild = m_ChildList[ i];
		pChild->SetVisible( true);
	}
	
	i3UIScene* scn = GetScene(0);
	if ( scn )
	{
		i3UIControl* ctrl = reinterpret_cast<i3UIControl*>(scn->getFirstChild());
		i3UI::SetFocusControl(ctrl, true);
	}
		
}

/*virtual*/ bool UIPopupBase::OnExitStart( void)
{
	if( UIBase::OnExitStart() == false)
		return false;

	
	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		i3UIControl * pChild = m_ChildList[ i];
		pChild->EnableCtrl( false, true);
	}
	
	return true;
}

UIMainFrame * UIPopupBase::GetMainFrame( void)
{
	if( i3::same_of<UIFramework*>(m_pParent))
		return ((UIFramework*) m_pParent)->GetUIMainframe();

	return nullptr;
}

bool UIPopupBase::OnRevive( i3RenderContext * pCtx)
{
	if( UIBase::OnRevive( pCtx) == false)
		return false;

	_ApplyBGSize();
	return true;
}

