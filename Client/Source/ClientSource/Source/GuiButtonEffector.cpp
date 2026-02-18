#include "pch.h"
#if legacy_gui_function
#include "GuiButtonEffector.h"




CGuiButtonEffector::CGuiButtonEffector(REAL32 EffectTime)
{
	m_EffectTime	= EffectTime;
	m_pTargetButton	= nullptr;

	Reset();
}


void	CGuiButtonEffector::Set(i3GuiButton * pButton)
{
	m_pTargetButton = pButton;
	Reset();
}


void	CGuiButtonEffector::Reset()
{
	m_ActiveBtnTime	= 0.0f;
	m_ReverseEffect	= false;
}


void	CGuiButtonEffector::Update(bool Update, REAL32 DeltaSeconds)
{
	if ( ! m_pTargetButton )
		return;

	// 버튼이 올라가 있지 않아야한다.
	if ( Update && m_pTargetButton->getControlState() != I3GUI_CONTROL_STATE_ONMOUSE)
	{
		//	버튼 깜빡임 타이머
		m_ActiveBtnTime += DeltaSeconds;

		if( m_ActiveBtnTime > m_EffectTime)
		{
			m_ActiveBtnTime		= 0.0f;
			m_ReverseEffect = !m_ReverseEffect;
		}

		ButtonEffect(m_ReverseEffect);
	}
	else
	{
		ButtonEffect(false);
	}
}


void	CGuiButtonEffector::ButtonEffect(bool Reverse)
{
	if( Reverse )
	{
		m_pTargetButton->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET + 1);
		m_pTargetButton->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);
	}
	else
	{
		// 원상복귀
		m_pTargetButton->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
		m_pTargetButton->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
	}
}

#endif