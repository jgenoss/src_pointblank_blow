#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "Behaviors.h"
#include "AniCamRegisterByMode.h"
#include "OutsideEvents.h"

namespace
{
	const char* g_animName[] = {"", "SceneA_Mutant_Walk_StageOn_Camera.i3s", "SceneB_Mutant_Death_Camera.i3s"}; 
}

AnimCamRegisterDomi::AnimCamRegisterDomi() : m_evtSubject(0)
{
}

AnimCamRegisterDomi::~AnimCamRegisterDomi()
{
	i3::vector<OutsideEventObserver*>::iterator iter;
	for (iter = m_evtObservers.begin(); iter != m_evtObservers.end(); iter++)
		delete *iter;
	m_evtObservers.clear();

	delete m_evtSubject; m_evtSubject = 0;
}

void AnimCamRegisterDomi::Register()
{
	CGameCameraAnimated* animCamera = CGameCameraManager::i()->GetAnimationCamera();
	animCamera->ChangeMode(AnimCamModeStuff::SetDominationMode);

	animCamera->AddScene("SceneB_Mutant_Death_Camera.i3s", "Object/domination/Camera/SceneB_Mutant_Death_Camera.i3s");
	animCamera->AddScene("SceneA_Mutant_Walk_StageOn_Camera.i3s", "Object/domination/Camera/SceneA_Mutant_Walk_StageOn_Camera.i3s");

	m_evtSubject = new OutsideEventSubject(animCamera);
	m_evtObservers.push_back(new Deadtown6DayObjColumn(m_evtSubject));
	m_evtObservers.push_back(new Deadtown6DayBossDeadStart(m_evtSubject));
	m_evtObservers.push_back(new Deadtown6DayBossWalkStart(m_evtSubject));
}

const char* AnimCamRegisterDomi::GetAnimKey(INT32 idx) const
{
	return g_animName[idx - ANIM_END];		// [idx-500];		ANIM_END = 500
}

#endif