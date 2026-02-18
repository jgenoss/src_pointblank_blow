#include "pch.h"
#include "UIPhaseSubBase.h"
#include "UIMainFrame.h"
#include "UITopMenu.h"

#include "UIUtil.h"

#include "../CommunityContext.h"

//I3_ABSTRACT_CLASS_INSTANCE( UIPhaseSubBase, UIPhaseBase);
I3_CLASS_INSTANCE( UIPhaseSubBase);

UIPhaseSubBase::UIPhaseSubBase()
{
}

UIPhaseSubBase::~UIPhaseSubBase()
{
}

void UIPhaseSubBase::OnEscapeKey(void)
{
	GetMainFrame()->GetTopMenu()->ClickBackButton();

	//연속적인 Esc 키 입력을 방지하고자 넣는다.
	i3InputKeyboard * pKeyboard = g_pFramework->getKeyboard();
	if( pKeyboard->GetStrokeState(I3I_KEY_ESC) == true) 
		pKeyboard->SetStrokeState(I3I_KEY_ESC, false);
}

// 메인 페이즈의 일부 함수는 업데이트하도록 추가 실시간 업데이트 함수를 따로 구성한다..
void	UIPhaseSubBase::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate(rDeltaSeconds);
	
	UIPhaseBase* main_phase = GetMainFrame()->GetCurrentPhase();	
	
	if (main_phase != nullptr)
	{
		main_phase->OnUpdate_NoneRsc(rDeltaSeconds);
	}
}

