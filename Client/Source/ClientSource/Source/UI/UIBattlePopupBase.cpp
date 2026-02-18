#include "pch.h"
#include "UIBattlePopupBase.h"

#include "UIBattleFrame.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIBattlePopup_OK( LuaState * L)
	{
		UIBattleFrame * pFrame = UIBattleFrame::i();
		I3ASSERT( pFrame);

		INT32 idx = pFrame->FindPopup( L);
		I3ASSERT_RETURN( idx >= 0, 0);
		
		UIPopupBase * pPopup = pFrame->GetPopup( static_cast<UIBATTLE_POPUP>(idx));
		I3ASSERT( pPopup);
		if( pPopup->OnOKButton())
			pFrame->TogglePopup( static_cast<UIBATTLE_POPUP>(idx));

		return 0;
	}

	int UIBattlePopup_Close( LuaState * L)
	{
		UIBattleFrame * pFrame = UIBattleFrame::i();
		I3ASSERT( pFrame);

		INT32 idx = pFrame->FindPopup( L);
		I3ASSERT_RETURN(idx >= 0, 0);

		UIPopupBase * pPopup = pFrame->GetPopup( static_cast<UIBATTLE_POPUP>(idx));
		I3ASSERT( pPopup);
		if( pPopup->OnCloseButton())
		{
			pFrame->TogglePopup( static_cast<UIBATTLE_POPUP>(idx));
		}

		return 0;
	}
}

LuaFuncReg UIBattlePopup_Glue[] = {
	{"OK",					UIBattlePopup_OK	},
	{"Close",				UIBattlePopup_Close},			// 창 닫기
	{nullptr,					nullptr}
};

I3_CLASS_INSTANCE( UIBattlePopupBase);//, UIPopupBase);

UIBattlePopupBase::UIBattlePopupBase()
{
}

UIBattlePopupBase::~UIBattlePopupBase()
{
}

void	UIBattlePopupBase::_InitializeAtLoad( i3UIScene * pScene)
{
	INT32 i;

	UIBase::_InitializeAtLoad( pScene);

	// 여기에 걸리는 것은 중복으로 Scene을 load하는 경우입니다.
	// 다른 scene을 추가하려면 UIBase::_LoadScene()을 호출하십시오.. 순구
	//I3ASSERT( m_pScene == nullptr);
	
	if (m_pScene == nullptr)
	{
		m_pScene = pScene;

		LuaState * L = m_pScene->getLuaContext();
		if( L != nullptr)
		{
			for( i = 0; UIBattlePopup_Glue[i].name; i++)
			{
				i3Lua::RegisterFunc( L, UIBattlePopup_Glue[ i].name, UIBattlePopup_Glue[i].func);
			}
		}

		m_pBGFrame = (i3UIFrameWnd*) pScene->FindChildByName( "bg");
		if( m_pBGFrame == nullptr)
			m_pBGFrame = (i3UIFrameWnd*) pScene->FindChildByName( "BG");
		_ApplyBGSize();
 
	//	m_slide.ClearFrameWndList();
	//	m_slide.SetTargetTime(false, 0.f);
	//	m_slide.SetTargetTime(true, 0.f);
	}
}

void UIBattlePopupBase::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();
	
	i3UIScene* scn = GetScene(0);
	if (scn)
	{
		i3UIFrameWnd* frame = (i3UIFrameWnd*) scn->FindChildByName( "main");

		if( frame != nullptr)
		{
			m_ChildList.clear();
			frame->GetChildControls( m_ChildList, false);

			i3Vector::Copy( &m_vOriginalSize, frame->getSize());

			m_slide.ClearFrameWndList();
			m_slide.AddFrameWnd(frame, UISlide::POPUP);

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

//		m_slide.AddFrameWnds( scn );
	}
}

void UIBattlePopupBase::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	m_ChildList.clear();
}

UIBattleFrame *	UIBattlePopupBase::GetUIBattleFrame( void)
{
	UIBattleFrame * pFrame = UIBattleFrame::i();
	I3ASSERT( pFrame);

	return pFrame;
}

void UIBattlePopupBase::_OnClose( void)
{
	if( GetUIBattleFrame() != nullptr)
	{
		UIBATTLE_POPUP type = GetUIBattleFrame()->FindPopupByPointer( this);
		GetUIBattleFrame()->ClosePopup( type);
	}
}

void	UIBattlePopupBase::SetEnable( bool bTrue , REAL32 tm )
{
	bool enable_old = UIBattlePopupBase::isEnable();
	UIPopupBase::SetEnable(bTrue, tm);
	if (enable_old != bTrue)
	{
		UIBattleFrame::i()->AddPopupCount( INT32(bTrue) * 2 - 1);
	}
}
