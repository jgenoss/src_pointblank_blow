#include "pch.h"

#ifdef	DOMI_ANIM_CAM

#include "OutsideEvents.h"
#include "GameCamera_Animated.h"
#include "AnimatedCamera.h"
#include "../StageBattle.h"
#include "../../GameCharaMoveContext.h"

#ifdef DINO_FULLBODY_ANIMATION
#include "../Character/Fullbody/GameCharaDomiMutantRex.h"
#include "../Character/Fullbody/FullbodyStateDomiMutantRex.h"
#endif


OutsideEventSubject::OutsideEventSubject(CGameCameraAnimated* cam)
{
	cam->GetCamera()->AttachOutsideEvent(this);
	m_camera = cam->GetCamera();
}

OutsideEventSubject::~OutsideEventSubject()
{
	m_camera->DetachOutsideEvent();
}

void OutsideEventSubject::Attach(OutsideEventObserver* obj)
{
	m_observers.push_front(obj);
}

void OutsideEventSubject::Detach(OutsideEventObserver* obj)
{
	std::list<OutsideEventObserver*>::iterator iter;
	for (iter = m_observers.begin(); iter != m_observers.end();)
	{
		if (*iter == obj)
			iter = m_observers.erase(iter);
		else
			iter++;
	}
}

void OutsideEventSubject::Notify()
{
	if (m_observers.empty()) return;

	i3Node* anim = m_camera->GetCurAnim();
	if (anim)
	{
		i3Transform* transform = static_cast<i3Transform*>(anim->GetChild(0)->GetChild(0));
		REAL32 playTime = transform->getPlayTime();
		REAL32 duration = transform->GetDuration();

		std::list<OutsideEventObserver*>::iterator iter;
		for (iter = m_observers.begin(); iter != m_observers.end(); iter++)
		{
			if (playTime > duration)
			{
				if (::strcmp((*iter)->GetAnimFileName(), m_camera->GetCurAnimName()) == 0)
				{
					if ((*iter)->IsEventDone())
					{
						(*iter)->ResetEvent();
						return;
					}
				}
				return;
			}

			if ((*iter)->GetEventTime() <= playTime && playTime <= duration)
			{
				if (::strcmp((*iter)->GetAnimFileName(), m_camera->GetCurAnimName()) == 0)
				{
					if ((*iter)->IsEventDone() == FALSE)
					{
						(*iter)->Update();
						(*iter)->SetEventDone(TRUE);
					}
				}
			}
		}
	}
}



OutsideEventObserver::OutsideEventObserver(OutsideEventSubject* subject, OutsideEventObserver* evt, const char* animName, REAL32 evtTime)
{
	m_subject = subject;
	subject->Attach(evt);
	m_event = evt;

	m_eventTime = evtTime;
	m_animFileName = animName;
	m_eventDone = FALSE;
}

OutsideEventObserver::~OutsideEventObserver()
{
	m_subject->Detach(m_event);
}




void Deadtown6DayObjColumn::Update()
{
	CStageBattle* s = reinterpret_cast<CStageBattle*>(g_pFramework->GetCurrentStage());
	size_t objCnt = s->GetStageObjectList()->size();

	for (size_t i=0; i<objCnt; i++)
	{
		i3Object* object = (*s->GetStageObjectList())[i];
		INT32 idx = object->getNetIndex();

		if (idx == deadtown6DayObjNetIdx) // Deadtown_6Day_Obj_column.i3s
		{
			INT32 stateCount = object->getStateCount();
			object->setCurState(stateCount-1);
			object->setCurrentHP(0.0f);
		}
	}

	I3TRACE("do event : Deadtown 6 Day Object Columm Camera Outside Event!\n");
	_LOG_("do event : Deadtown 6 Day Object Columm Camera Outside Event!\n");
}

void Deadtown6DayBossDeadStart::Update()
{
	
	for( INT32 i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
	{
		CGameCharaBase* pAI = g_pCharaManager->getCharaByNetIdx( i, true);
		if (pAI != NULL)
		{
			if (pAI->IsDomiMutantRex())
			{
				pAI->getMoveContext()->setRecvTheta(-I3_PI2);

				fb_fsm::StateMgr* sm = static_cast<CGameCharaFullbody*>(pAI)->GetFBStateMgr();
				sm->Transition(sm->GetState(fb_fsm::domi_mutantrex::DEATH_STAGEEND));
			}
		}
	}

	I3TRACE("do event : Deadtown 6 Day BOSS Dead Start!\n");
	_LOG_("do event : Deadtown 6 Day BOSS Dead Start!\n");
}

void Deadtown6DayBossWalkStart::Update()
{
	for( INT32 i = 0; i < MAX_COUNT_DOMI_DINO; i++ )
	{
		CGameCharaBase* pAI = g_pCharaManager->getCharaByNetIdx( i, true);
		if (pAI != NULL)
		{
			if (pAI->IsDomiMutantRex())
			{
				pAI->getSceneObject()->RemoveFlag(I3_NODEFLAG_INVISIBLE);
				pAI->getMoveContext()->setRecvTheta(I3_PI2);

				fb_fsm::StateMgr* sm = static_cast<CGameCharaFullbody*>(pAI)->GetFBStateMgr();
				sm->Transition(sm->GetState(fb_fsm::domi_mutantrex::WALK_STAGEON));
			}
		}
	}

	I3TRACE("do event : Deadtown 6 Day BOSS Walk Start!\n");
	_LOG_("do event : Deadtown 6 Day BOSS Walk Start!\n");
}

#endif