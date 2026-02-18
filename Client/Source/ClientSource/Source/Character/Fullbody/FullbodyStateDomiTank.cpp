#include "pch.h"

#include "FullbodyStateDomiTank.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../GameCharaMoveContext.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_tank;

namespace
{
#define WALK_LEFT_STEP_TIME		1.580f
#define WALK_RIGHT_STEP_TIME	0.600f 
#define RUN_LEFT_STEP_TIME		0.920f
#define RUN_RIGHT_STEP_TIME		0.430f
#define RUSH_LEFT_STEP_TIME		0.510f
#define RUSH_RIGHT_STEP_TIME	0.222f
#define BITE_TIME				0.400f	
#define BITE_TREMBLE_RANGE		3.000f;
}

namespace fb_fsm
{
	namespace domi_tank
	{
		//----------------------------------------------
		// State 생성 팩토리
		//----------------------------------------------
		class AnimationFactory
		{
		public:
			AnimationFactory()
			{
				ZeroMemory(m_animations, sizeof(m_animations));

				m_animations[NONE]					= &static_creator<AnimationBase, AnimationBase, NONE, NONE>::call;
				m_animations[IDLE]					= &static_creator<AnimationBase, Idle,			IDLE, REPEAT>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase, ROAR, IDLE>::call;
				m_animations[WALK]					= &static_creator<AnimationBase, Walk,			WALK, REPEAT>::call;
				m_animations[RUN]					= &static_creator<AnimationBase, Run,			RUN, REPEAT>::call;
				m_animations[DAMAGE_MID]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_MID,  IDLE>::call;
				m_animations[DAMAGE_HIGH]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_HIGH, IDLE>::call;
				m_animations[DAMAGE_LOW]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_LOW,  IDLE>::call;
				m_animations[KNOCKDOWN]				= &static_creator<AnimationBase, AnimationBase, KNOCKDOWN, STANDUP>::call;
				m_animations[STANDUP]				= &static_creator<AnimationBase, AnimationBase, STANDUP,   IDLE>::call;
				m_animations[DEATH_BY_GRENADE]		= &static_creator<AnimationBase, DeathByGrenade,  DEATH_BY_GRENADE,  NONE>::call;
				m_animations[DEATH_BY_HEADSHOT]		= &static_creator<AnimationBase, DeathByHeadShot, DEATH_BY_HEADSHOT, NONE>::call;
				m_animations[DEATH_MID1]			= &static_creator<AnimationBase, DeathMid1,		DEATH_MID1, NONE>::call;
				m_animations[DEATH_MID2]			= &static_creator<AnimationBase, DeathMid2,		DEATH_MID2, NONE>::call;
				m_animations[DEATH_MID3]			= &static_creator<AnimationBase, DeathMid3,		DEATH_MID3, NONE>::call;
				m_animations[BITE]					= &static_creator<AnimationBase, Bite,			BITE, IDLE>::call;
				m_animations[HOWL]					= &static_creator<AnimationBase, AnimationBase, HOWL, IDLE>::call;
				m_animations[RUSH_READY_BEND_OVER]	= &static_creator<AnimationBase, AnimationBase, RUSH_READY_BEND_OVER, RUSH_READY_QUAKING>::call;
				m_animations[RUSH_READY_QUAKING]	= &static_creator<AnimationBase, AnimationBase, RUSH_READY_QUAKING,   REPEAT>::call;
				m_animations[RUSH]					= &static_creator<AnimationBase, Rush,			RUSH, REPEAT>::call;
				m_animations[RUSH_FINISH]			= &static_creator<AnimationBase, AnimationBase, RUSH_FINISH, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase, GROGGY, REPEAT>::call;

				for (INT32 i=0; i<AI_MAX; i++)
				{
					if (m_animations[i] == nullptr)
						m_animations[i] = &static_creator<AnimationBase, AnimationBase, NONE, NONE>::call;
				}
			}

			typedef AnimationBase* (*FuncPtr)(CGameCharaFullbody* chara, INT32 id);

			FuncPtr	GetFunction(INT32 id) const { return m_animations[id];			}
			INT32	GetAnimationCount() const	{ return _countof(m_animations);	}

		private:
			FuncPtr	m_animations[AI_MAX];
		} g_animationFactory;


		AnimationBase* CreateAnimation(INT32 id, CGameCharaFullbody* chara)
		{
			return (*g_animationFactory.GetFunction(id))(chara, id);
		}

		void DestroyAnimation(AnimationBase*& lt)
		{
			I3_SAFE_DELETE( lt);
		}

		INT32 GetAnimationCount()
		{
			return g_animationFactory.GetAnimationCount();
		}
	}
}



//----------------------------------------------
// IDLE (반복)
//----------------------------------------------
Idle::Idle(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID)
	: AnimationBase(chara, id, startAnimID, nextAnimID)
{
	m_breath = fb_misc::BreathFactory::CreateDomiTank(chara);
}

Idle::~Idle()
{
	I3_SAFE_DELETE( m_breath);
}

void Idle::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Idle::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Idle::OnAnimStart() {}
void Idle::OnAnimRun(REAL32 tm) { m_breath->OnPlay(tm); }
void Idle::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// WALK (반복)
//----------------------------------------------
Walk::Walk(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID)
{
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(WALK_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(WALK_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
	m_walkSound = new fb_misc::BipedalSound( pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_walkSound, sizeof(fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof(fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof(fb_misc::OneFootStepSound));
}

Walk::~Walk()
{
	I3_SAFE_DELETE( m_walkSound);
}

void Walk::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Walk::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Walk::OnAnimStart() {}
void Walk::OnAnimRun(REAL32 tm){ m_walkSound->OnUpdate(tm); }
void Walk::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd();
	m_walkSound->Reset();
}

//----------------------------------------------
// RUN (반복)
//----------------------------------------------
Run::Run(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID)
{
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(RUN_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(RUN_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
	m_runSound = new fb_misc::BipedalSound(pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_runSound, sizeof(fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof(fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof(fb_misc::OneFootStepSound));
}

Run::~Run()
{
	I3_SAFE_DELETE( m_runSound);
}

void Run::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Run::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Run::OnAnimStart() {}
void Run::OnAnimRun(REAL32 tm) { m_runSound->OnUpdate(tm); }
void Run::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd();
	m_runSound->Reset();
}

//----------------------------------------------
// DEATH_BY_GRENADE
//----------------------------------------------
DeathByGrenade::DeathByGrenade(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathByGrenade::~DeathByGrenade() {}

void DeathByGrenade::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathByGrenade::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathByGrenade::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_TANK); }
void DeathByGrenade::OnAnimRun(REAL32 tm) {}
void DeathByGrenade::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_BY_HEADSHOT
//----------------------------------------------
DeathByHeadShot::DeathByHeadShot(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathByHeadShot::~DeathByHeadShot() {}

void DeathByHeadShot::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathByHeadShot::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathByHeadShot::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_TANK); }
void DeathByHeadShot::OnAnimRun(REAL32 tm) {}
void DeathByHeadShot::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_MID1
//----------------------------------------------
DeathMid1::DeathMid1(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathMid1::~DeathMid1() {}

void DeathMid1::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathMid1::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathMid1::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_TANK); }
void DeathMid1::OnAnimRun(REAL32 tm) {}
void DeathMid1::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_MID2
//----------------------------------------------
DeathMid2::DeathMid2(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathMid2::~DeathMid2() {}

void DeathMid2::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathMid2::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathMid2::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_TANK); }
void DeathMid2::OnAnimRun(REAL32 tm) {}
void DeathMid2::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_MID3
//----------------------------------------------
DeathMid3::DeathMid3(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathMid3::~DeathMid3() {}

void DeathMid3::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathMid3::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathMid3::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_TANK); }
void DeathMid3::OnAnimRun(REAL32 tm) {}
void DeathMid3::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// RUSH (반복)
//----------------------------------------------
Rush::Rush(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID)
: AnimationBase(chara, id, startAnimID, nextAnimID)
{
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(RUSH_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(RUSH_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
	m_rushSound = new fb_misc::BipedalSound(pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_rushSound, sizeof(fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof(fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof(fb_misc::OneFootStepSound));
}

Rush::~Rush()
{
	I3_SAFE_DELETE( m_rushSound);
}

void Rush::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Rush::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Rush::OnAnimStart() {}
void Rush::OnAnimRun(REAL32 tm) 
{
	fb_misc::Effect(m_chara, "TANK_ATTACK_B", DOMI_RAPTOR_BONE_IK);

	m_rushSound->OnUpdate(tm); 
}
void Rush::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd();
	m_rushSound->Reset();
}

//----------------------------------------------
// BITE
//----------------------------------------------
Bite::Bite(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID)
	: AnimationBase(chara, id, startAnimID, nextAnimID), m_bite(false) 
{
	i3Transform* fxDummy = nullptr;
	i3Node* node = i3Scene::FindNodeByName(m_chara->getSceneObject()->GetBody(), "FXDummy");

	if (node)
	{
		if (i3::kind_of<i3BoneRef*>(node))
		{
			fxDummy = (i3Transform*)i3Scene::FindNodeByExactType(node, i3Transform::static_meta());
			I3ASSERT(fxDummy);
		}
		else if (i3::kind_of<i3Transform*>(node))
			fxDummy =(i3Transform*)node;

		m_matBitePoint = fxDummy->GetCacheMatrix();
	}
}

Bite::~Bite() { m_matBitePoint = nullptr; }

void Bite::OnEntry(fsm::IMachine* m, fsm::IState* s) 
{
	m_bite = false;
	AnimationBase::OnEntry(m, s); 
}
void Bite::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Bite::OnAnimStart() {}
void Bite::OnAnimRun(REAL32 tm) 
{
	if (m_bite == false)
	{
		if (m_chara->GetAiContext()->getLocalTime()+tm > BITE_TIME)
		{
			fb_misc::Effect(m_chara, "TANK_ATTACK_A", m_matBitePoint);

			if (g_pCamera)
			{
				MATRIX* cam = g_pCamera->getCamMatrix();

				VEC3D temp;
				i3Vector::Sub(&temp, i3Matrix::GetPos(m_matBitePoint), i3Matrix::GetPos(cam));
				REAL32 length = i3Vector::Length(&temp);

				REAL32 maxLength = length * 3.0f;
				if (length < maxLength)
				{
					REAL32 trembAmpRate = 1.0f - (length/maxLength);
					g_pCamera->SetTremble( 5.0f * trembAmpRate * trembAmpRate, 7.0f * trembAmpRate, 0.1f, 2.f * trembAmpRate);
				}
			}

			m_bite = true;
		}
	}
}
void Bite::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd();
}
