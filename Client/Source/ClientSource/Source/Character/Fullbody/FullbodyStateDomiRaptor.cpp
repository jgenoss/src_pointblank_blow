#include "pch.h"

#include "FullbodyStateDomiRaptor.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../GameCharaMoveContext.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_raptor;

namespace
{
	#define RAPTOR_WALK_LEFT_STEP_TIME		0.933f
	#define RAPTOR_WALK_RIGHT_STEP_TIME		0.333f 
	#define RAPTOR_RUN_LEFT_STEP_TIME		0.666f
	#define RAPTOR_RUN_RIGHT_STEP_TIME		0.266f
	#define	RAPTOR_LEFT_SCRATCH_TIME		0.380f
	#define RAPTOR_RIGHT_SCRATCH_TIME		0.600f
	#define RAPTOR_BITE_TIME				0.400f	
}

namespace fb_fsm
{
	namespace domi_raptor
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
				m_animations[FORAGE]				= &static_creator<AnimationBase, AnimationBase, FORAGE, REPEAT>::call;
				m_animations[WALK]					= &static_creator<AnimationBase, Walk,			WALK, REPEAT>::call;
				m_animations[RUN]					= &static_creator<AnimationBase, Run,			RUN, REPEAT>::call;
				m_animations[DAMAGE_MID]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_MID, IDLE>::call;
				m_animations[DAMAGE_HIGH]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_HIGH, IDLE>::call;
				m_animations[DAMAGE_LOW]			= &static_creator<AnimationBase, AnimationBase, DAMAGE_LOW, IDLE>::call;
				m_animations[KNOCKDOWN]				= &static_creator<AnimationBase, AnimationBase, KNOCKDOWN, STANDUP>::call;
				m_animations[STANDUP]				= &static_creator<AnimationBase, AnimationBase, STANDUP, IDLE>::call;
				m_animations[DEATH_BY_GRENADE]		= &static_creator<AnimationBase, DeathByGrenade, DEATH_BY_GRENADE, NONE>::call;
				m_animations[DEATH_BY_HEADSHOT]		= &static_creator<AnimationBase, DeathByHeadShot, DEATH_BY_HEADSHOT, NONE>::call;
				m_animations[DEATH_MID1]			= &static_creator<AnimationBase, DeathMid1,		DEATH_MID1, NONE>::call;
				m_animations[DEATH_MID2]			= &static_creator<AnimationBase, DeathMid2,		DEATH_MID2, NONE>::call;
				m_animations[DEATH_MID3]			= &static_creator<AnimationBase, DeathMid3,		DEATH_MID3, NONE>::call;
				m_animations[BITE]					= &static_creator<AnimationBase, AnimationBase, BITE, IDLE>::call;
				m_animations[JUMPATTACK]			= &static_creator<AnimationBase, AnimationBase, JUMPATTACK, IDLE>::call;
				m_animations[JUMPSTART]				= &static_creator<AnimationBase, JumpStart,		JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]				= &static_creator<AnimationBase, AnimationBase, JUMPING, REPEAT>::call;
				m_animations[JUMPEND]				= &static_creator<AnimationBase, JumpEnd,		JUMPEND, IDLE>::call;
				m_animations[WARNKILL]				= &static_creator<AnimationBase, AnimationBase, WARNKILL, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase, ROAR, IDLE>::call;
				m_animations[LEAP_READY_BEND_OVER]	= &static_creator<AnimationBase, AnimationBase, LEAP_READY_BEND_OVER, LEAP_READY_QUAKING>::call;
				m_animations[LEAP_READY_QUAKING]	= &static_creator<AnimationBase, LeapReadyQuaking, LEAP_READY_QUAKING, REPEAT>::call;
				m_animations[LEAP_START]			= &static_creator<AnimationBase, AnimationBase, LEAP_START, LEAP>::call;
				m_animations[LEAP]					= &static_creator<AnimationBase, Leap,			LEAP, REPEAT>::call;
				m_animations[LEAP_ATTACK]			= &static_creator<AnimationBase, AnimationBase, LEAP_ATTACK, IDLE>::call;
				m_animations[LEAP_FAIL]				= &static_creator<AnimationBase, AnimationBase, LEAP_FAIL, IDLE>::call;
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
	m_breath = fb_misc::BreathFactory::CreateDomiRaptor(chara);
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
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(RAPTOR_WALK_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(RAPTOR_WALK_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
	m_walkSound = new fb_misc::BipedalSound( pFootStepLeft,	pFootStepRight);

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
void Walk::OnAnimRun(REAL32 tm) { m_walkSound->OnUpdate(tm); }
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
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(RAPTOR_RUN_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight =new fb_misc::OneFootStepSound(RAPTOR_RUN_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
	m_runSound = new fb_misc::BipedalSound( pFootStepLeft, pFootStepRight);

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
void DeathByGrenade::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_RAPTOR); }
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
void DeathByHeadShot::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_RAPTOR); }
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
void DeathMid1::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_RAPTOR); }
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
void DeathMid2::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_RAPTOR); }
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
void DeathMid3::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_RAPTOR); }
void DeathMid3::OnAnimRun(REAL32 tm) {}
void DeathMid3::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// JUMPSTART
//----------------------------------------------
JumpStart::JumpStart(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

JumpStart::~JumpStart() {}

void JumpStart::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void JumpStart::OnEvent(fsm::IMachine* m, fsm::IEvent* e)
{
	StateMgr* sm	= static_cast<StateMgr*>(m);
	Event* evt		= static_cast<Event*>(e);

	// JUMPSTART UDP는 서버가 1번만 보냄, UDP가 소실될 경우를 대비.
	fsm::IState* prevSt = m->GetPrevState();
	if (prevSt && (static_cast<State*>(prevSt)->GetID() != JUMPSTART))
	{
		Transition(sm, sm->GetState(JUMPSTART));
		return;
	}

	Transition(sm, sm->GetState(evt->id));
}
void JumpStart::OnAnimStart()
{
	g_pSndMng->ReserveSoundPlay(IT_TERRAIN_TYPE, 
		m_chara->getMoveContext()->getFloorTerrainType(), GTBT_JUMP_START, m_chara->GetPos(), false);
}
void JumpStart::OnAnimRun(REAL32 tm) {}
void JumpStart::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// JUMPEND
//----------------------------------------------
JumpEnd::JumpEnd(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

JumpEnd::~JumpEnd() {}

void JumpEnd::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void JumpEnd::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void JumpEnd::OnAnimStart()
{
	g_pSndMng->ReserveSoundPlay(IT_TERRAIN_TYPE, 
		m_chara->getMoveContext()->getFloorTerrainType(), GTBT_JUMP_END, m_chara->GetPos(), false);
}
void JumpEnd::OnAnimRun(REAL32 tm) {}
void JumpEnd::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// LEAP READY QUAKING (반복)
//----------------------------------------------
LeapReadyQuaking::LeapReadyQuaking(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

LeapReadyQuaking::~LeapReadyQuaking() {}

void LeapReadyQuaking::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void LeapReadyQuaking::OnEvent(fsm::IMachine* m, fsm::IEvent* e)
{
	StateMgr* sm	= static_cast<StateMgr*>(m);
	Event* evt		= static_cast<Event*>(e);

	if (evt->id == LEAP_READY_BEND_OVER) return;

	Transition(sm, sm->GetState(evt->id)); 
}
void LeapReadyQuaking::OnAnimStart() {}
void LeapReadyQuaking::OnAnimRun(REAL32 tm) {}
void LeapReadyQuaking::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// LEAP (반복)
//----------------------------------------------
Leap::Leap(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

Leap::~Leap() {}

void Leap::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Leap::OnEvent(fsm::IMachine* m, fsm::IEvent* e)
{
	StateMgr* sm	= static_cast<StateMgr*>(m);
	Event* evt		= static_cast<Event*>(e);

	if (evt->id == LEAP_START) return;

	Transition(sm, sm->GetState(evt->id));
}
void Leap::OnAnimStart() {}
void Leap::OnAnimRun(REAL32 tm) {}
void Leap::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

