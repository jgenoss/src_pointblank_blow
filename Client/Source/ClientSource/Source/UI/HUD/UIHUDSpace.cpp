#include "pch.h"
#include "UIHUDSpace.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIMath.h"

I3_CLASS_INSTANCE(UIHudSpace); //, i3GameObjBase);

UIHudSpace::UIHudSpace() : close_popup(false), enable_progressbar(true),
open_time(0.0f), target_time(4.8f)
{

}

UIHudSpace::~UIHudSpace(void)
{

}

void UIHudSpace::OnCreate(i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate(pParent);

	AddScene("Scene/HUD/PBRe_HUD_SpaceMode_01.i3UIs");
}

bool UIHudSpace::OnEntranceStart(void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if (UIBattlePopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	pProgressbar = (i3UIProgressBar*)GetScene(0)->FindChildByName("i3UIProgressBar");
	I3ASSERT(pProgressbar != nullptr);

	open_time = 0.0f;
	close_popup = false;
	enable_progressbar = true;

	SetInfo();

	GetSlide()->ForceEnd();

	return true;
}

bool UIHudSpace::OnExitStart(void)
{
	if (UIBattlePopupBase::OnExitStart() == false)
		return false;

	g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);

	return true;
}

bool UIHudSpace::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	if (UIBattlePopupBase::OnKeyInput(pInputMng) == true) return true;

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

	if (pKeyboard->GetStrokeState(I3I_KEY_SPACE))
	{
		close_popup = true;
		enable_progressbar = false;
	}

	return false;
}

void UIHudSpace::SetInfo()
{

}

void UIHudSpace::OnUpdate(REAL32 rDeltaSeconds)
{
	UIBattlePopupBase::OnUpdate(rDeltaSeconds);

	if (enable_progressbar)
	{
		REAL32 factor = uiMath::inverse_interpolate(open_time, 0.f, target_time);
		factor = uiMath::limited(factor, 0.f, 1.f);
		if (pProgressbar != nullptr)
		{
			pProgressbar->setProgress(factor);
		}

		open_time += rDeltaSeconds;
		if (open_time >= target_time) open_time = target_time;

		if (factor >= 1.0f)
		{
			close_popup = true;
			enable_progressbar = false;
		}
	}

	if (close_popup)
	{
		g_pFramework->SetGameKeyMode(GAMEKEY_MODE_GAME);

		GetUIBattleFrame()->ClosePopup(UBP_HUD_SPACE);
	}
}

bool UIHudSpace::OnKey_Escape()
{
	// ESC 키를 눌렀을 경우 EXIT 메뉴 팝업이 나와야 함
	g_pFramework->SetGameKeyMode(GAMEKEY_MODE_MENU);

	UIBattleFrame::i()->OpenPopup(UBP_EXIT);

	return true;
}