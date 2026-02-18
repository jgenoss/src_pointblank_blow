#include "pch.h"
#include "TutorialContext.h"
#include "UserInfoContext.h"

TutorialContext::TutorialContext() : m_uiRoundPlayTime(0), m_ui8OldTutorialIng(0)
{
	m_WasTutorial = false;
	m_bShowMissionButtonEffect = true; 
	m_bShowedMessage = false;
}

TutorialContext::~TutorialContext()
{


}

void TutorialContext::DeActiveMissionButtonEffect()
{
	if(IsFirstFinishTutorial())
	{
		m_bShowMissionButtonEffect = false;
	}
}

bool TutorialContext::IsFinishTutorial() const
{ 
	USER_INFO_BASIC	MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	return MyUserInfoBasic.m_ui8TutorialIng == 0x01; 
}

bool TutorialContext::IsFirstFinishTutorial() const
{ 
	return m_ui8OldTutorialIng == 0x0 && IsFinishTutorial(); 
	//return true;
}
void TutorialContext::SetOldTutorialIng()
{
	USER_INFO_BASIC	MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
	m_ui8OldTutorialIng = MyUserInfoBasic.m_ui8TutorialIng;
}



