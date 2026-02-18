#include "pch.h"
#include "UIHUDUsurpation.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIMath.h"

I3_CLASS_INSTANCE( UIHudUsurpation); //, i3GameObjBase);

UIHudUsurpation::UIHudUsurpation(  ) : close_popup(false), enable_progressbar(true),
										open_time(0.0f), target_time(4.8f)
{

}

UIHudUsurpation::~UIHudUsurpation( void )
{

}

void UIHudUsurpation::OnCreate( i3GameNode * pParent )
{
	UIBattlePopupBase::OnCreate( pParent);

	AddScene( "Scene/HUD/PBRe_HUD_watermelon.i3UIs" );
}

bool UIHudUsurpation::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	
	pProgressbar = (i3UIProgressBar*)GetScene(0)->FindChildByName( "i3UIProgressBar" );
	I3ASSERT(pProgressbar != nullptr);

	open_time = 0.0f;
	close_popup = false;
	enable_progressbar = true;

	SetInfo();

	GetSlide()->ForceEnd();

	return true;
}

bool UIHudUsurpation::OnExitStart( void )
{
	if( UIBattlePopupBase::OnExitStart() == false)
		return false;

	g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);

	return true;
}

bool UIHudUsurpation::OnKeyInput( i3InputDeviceManager * pInputMng )
{
	if( UIBattlePopupBase::OnKeyInput(pInputMng) == true) return true;
	
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	
	if( pKeyboard->GetStrokeState(I3I_KEY_SPACE))
	{
		close_popup = true;
		enable_progressbar = false;
	}

	return false;
}

void UIHudUsurpation::SetInfo()
{

}

void UIHudUsurpation::OnUpdate( REAL32 rDeltaSeconds )
{
	UIBattlePopupBase::OnUpdate(rDeltaSeconds);
	
	if(enable_progressbar)
	{
		REAL32 factor = uiMath::inverse_interpolate(open_time, 0.f, target_time);
		factor = uiMath::limited(factor, 0.f, 1.f);
		if(pProgressbar != nullptr)
		{
			pProgressbar->setProgress(factor);
		}
		
		open_time += rDeltaSeconds;
		if( open_time >= target_time) open_time = target_time;

		if(factor >= 1.0f)
		{
			close_popup = true;
			enable_progressbar = false;
		}
	}

	if(close_popup)
	{
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);

		GetUIBattleFrame()->ClosePopup(UBP_HUD_USURPATION);
		GetUIBattleFrame()->ClosePopup(UBP_HUD_ESCAPE);
	}
}

bool UIHudUsurpation::OnKey_Escape()
{
	// ESC 키를 눌렀을 경우 EXIT 메뉴 팝업이 나와야 함
	g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);

	UIBattleFrame::i()->OpenPopup( UBP_EXIT);
	
	return true;
}