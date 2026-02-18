#include "pch.h"
#include "UIHudC4Indicator.h"

#include "i3Base/profile/profile.h"

UIHudC4Indicator::UIHudC4Indicator(UIHudManager * p) : iHudBase(p), m_pC4AvailablePosImage(nullptr)
{
	
}

UIHudC4Indicator::~UIHudC4Indicator()
{

}

void UIHudC4Indicator::InitializeAtLoad(i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "c4");

	//1.0과 1.5의 컨트롤 이름이 달라서 생기는 삽질
	if(g_pEnvSet->m_nGameInterface == HUD::HS_1_5)
	{
		m_pC4AvailablePosImage = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImageBox"));
		m_pC4InstalledImage = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImageBox1"));
		m_pC4Count = static_cast<i3UIStaticText*>(pScene->FindChildByName("i3UIStaticText4"));
		m_pC4Progress =	static_cast<i3UIProgressBar*>(pScene->FindChildByName("i3UIProgressBar0"));
		m_pC4ProgressBG = static_cast<i3UIFrameWnd*>(pScene->FindChildByName("i3UIFrameWnd1"));
	}
	else if(g_pEnvSet->m_nGameInterface == HUD::HS_1_0)
	{
		m_pC4AvailablePosImage = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImageBox1"));
		m_pC4InstalledImage = static_cast<i3UIImageBox*>(pScene->FindChildByName("i3UIImageBox2"));
		m_pC4Count = static_cast<i3UIStaticText*>(pScene->FindChildByName("i3UIStaticText1"));
		m_pC4Progress =	static_cast<i3UIProgressBar*>(pScene->FindChildByName("i3UIProgressBar"));
		m_pC4ProgressBG = static_cast<i3UIFrameWnd*>(pScene->FindChildByName("i3UIFrameWnd0"));
	}
	
}

void UIHudC4Indicator::Update(REAL32 rDeltaSeconds)
{
	
}

void UIHudC4Indicator::SetParameter(INT32 arg1, INT32 arg2)
{

}

void UIHudC4Indicator::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();
	iHudBase::SetEnable(true);

	m_pC4AvailablePosImage->EnableCtrl(false);
	m_pC4InstalledImage->EnableCtrl(false);
	m_pC4Count->EnableCtrl(false);
	m_pC4ProgressBG->EnableCtrl(false);
	m_pC4Progress->EnableCtrl(false);
}

void UIHudC4Indicator::SetEnableAvailablePositionImage(bool isEnable)
{
	if(m_pC4AvailablePosImage != nullptr)
		m_pC4AvailablePosImage->EnableCtrl(isEnable);
}

void UIHudC4Indicator::SetEnableInstalledImage(bool isEnable)
{
	if ( m_pC4InstalledImage != nullptr)
		m_pC4InstalledImage->EnableCtrl(isEnable);

	if ( m_pC4Count != nullptr)
		m_pC4Count->EnableCtrl(isEnable);
}

void UIHudC4Indicator::SetEnableC4Progress(bool isEnable)
{
	m_pC4ProgressBG->EnableCtrl(isEnable);
	m_pC4Progress->EnableCtrl(isEnable);
}




