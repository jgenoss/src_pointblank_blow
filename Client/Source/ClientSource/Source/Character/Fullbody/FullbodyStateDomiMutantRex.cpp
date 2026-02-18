#include "pch.h"

#include "FullbodyStateDomiMutantRex.h"
#include "GameCharaDomiMutantRex.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../Mode/Domination//Domination_Defines.h"
#include "../../GameCharaHUDContext.h"
#include "../../GameCharaMoveContext.h"
#include "../../UI/HUD/UIHUD_Domination_Manager.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_mutantrex;

namespace
{
	#define MUTANTREX_WALK_LEFT_STEP_TIME			1.666f
	#define MUTANTREX_WALK_RIGHT_STEP_TIME			0.666f 
	#define MUTANTREX_RUN_LEFT_STEP_TIME			0.950f
	#define MUTANTREX_RUN_RIGHT_STEP_TIME			0.430f
	#define MUTANTREX_WALK_DAMAGED_LEFT_STEP_TIME	2.133f
	#define MUTANTREX_WALK_DAMAGED_RIGHT_STEP_TIME  0.800f
	#define MUTANTREX_STOMP_ATTACK_ANIM_TIME		0.730f
	#define MUTANTREX_STOMP_DAMAGE_RANGE			35.0f

	void EraseBossHP(INT32 idx)
	{
		domi_state_info* info = domi_state_info::i();
		i3::vector<DOMI_BOSS_HP>::iterator itr;
		for( itr = info->boss_list.begin(); itr != info->boss_list.end(); ++itr)
		{
			if( itr->boss->getCharaNetIndex() == idx)
			{
				info->boss_list.erase( itr);

				break;
			}
		}
	}
}

namespace fb_fsm
{
	namespace domi_mutantrex
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
				m_animations[WALK]					= &static_creator<AnimationBase, Walk,			WALK, REPEAT>::call;
				m_animations[WALK_WITH_LIMP]		= &static_creator<AnimationBase, WalkWithLimp,	WALK_WITH_LIMP, REPEAT>::call;
				m_animations[DAMAGE]				= &static_creator<AnimationBase, AnimationBase, DAMAGE, IDLE>::call;
				m_animations[DAMAGED_BY_DEATHBLOW]	= &static_creator<AnimationBase, AnimationBase, DAMAGED_BY_DEATHBLOW, IDLE>::call;
				m_animations[HOWLING_HITTED]		= &static_creator<AnimationBase, AnimationBase, HOWLING_HITTED, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase, GROGGY, REPEAT>::call;
				m_animations[KNOCKBACK]				= &static_creator<AnimationBase, AnimationBase, KNOCKBACK, IDLE>::call;
				m_animations[DEATH_STAGEEND]		= &static_creator<AnimationBase, DeathStageEnd, DEATH_STAGEEND, NONE>::call;
				m_animations[ATTACK_BOTTOM]			= &static_creator<AnimationBase, AnimationBase, ATTACK_BOTTOM, IDLE>::call;
				m_animations[ATTACK_TOP]			= &static_creator<AnimationBase, AnimationBase, ATTACK_TOP, IDLE>::call;
				m_animations[RUNATTACK_READY]		= &static_creator<AnimationBase, RunAttackReady, RUNATTACK_READY, REPEAT>::call;
				m_animations[RUNATTACK]				= &static_creator<AnimationBase, RunAttack,		RUNATTACK, REPEAT>::call;
				m_animations[STOMP]					= &static_creator<AnimationBase, Stomp,			STOMP, IDLE>::call;
				m_animations[HOWLING]				= &static_creator<AnimationBase, Howling,		HOWLING, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase, ROAR, IDLE>::call;

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
	m_breath = fb_misc::BreathFactory::CreateDomiMutantRex(chara);
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
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(MUTANTREX_WALK_LEFT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(MUTANTREX_WALK_RIGHT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_RIGHT, false);
	m_walkSound = new fb_misc::BipedalSound( pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_walkSound, sizeof( fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof( fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof( fb_misc::OneFootStepSound));
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
// WALK_WITH_LIMP (반복)
//----------------------------------------------
WalkWithLimp::WalkWithLimp(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID)
{
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(MUTANTREX_WALK_DAMAGED_LEFT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(MUTANTREX_WALK_DAMAGED_RIGHT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_RIGHT, false);
	m_walkLimpSound = new fb_misc::BipedalSound(pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_walkLimpSound, sizeof(fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof(fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof(fb_misc::OneFootStepSound));
}

WalkWithLimp::~WalkWithLimp()
{
	I3_SAFE_DELETE( m_walkLimpSound);
}

void WalkWithLimp::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void WalkWithLimp::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void WalkWithLimp::OnAnimStart() {}
void WalkWithLimp::OnAnimRun(REAL32 tm) { m_walkLimpSound->OnUpdate(tm); }
void WalkWithLimp::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd(); 
	m_walkLimpSound->Reset(); 
}

//----------------------------------------------
// DEATH_STAGEEND
//----------------------------------------------
DeathStageEnd::DeathStageEnd(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathStageEnd::~DeathStageEnd() {}

void DeathStageEnd::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathStageEnd::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathStageEnd::OnAnimStart() 
{ 
	fb_misc::Death(m_chara, GTBD_ON_TREX);
	EraseBossHP(m_chara->getCharaNetIndex());
}
void DeathStageEnd::OnAnimRun(REAL32 tm) {}
void DeathStageEnd::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// RUNATTACK_READY (반복)
//----------------------------------------------
RunAttackReady::RunAttackReady(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

RunAttackReady::~RunAttackReady() {}

void RunAttackReady::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void RunAttackReady::OnEvent(fsm::IMachine* m, fsm::IEvent* e) 
{ 
	AnimationBase::OnEvent(m, e); 
}

void RunAttackReady::OnAnimStart() 
{
	// 캐릭터 툴 사용해서 반복을 설정했으므로 이 곳은 한 번만 들어옴.
}
void RunAttackReady::OnAnimRun(REAL32 tm) { }
void RunAttackReady::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// RUNATTACK (반복)
//----------------------------------------------
RunAttack::RunAttack(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) 
{
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(MUTANTREX_RUN_LEFT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(MUTANTREX_RUN_RIGHT_STEP_TIME, chara, DOMI_MUTANTREX_BONE_FOOT_RIGHT, false);
	m_runSound = new fb_misc::BipedalSound( pFootStepLeft, pFootStepRight);

	MEMDUMP_NEW( m_runSound, sizeof(fb_misc::BipedalSound));
	MEMDUMP_NEW( pFootStepLeft, sizeof(fb_misc::OneFootStepSound));
	MEMDUMP_NEW( pFootStepRight, sizeof(fb_misc::OneFootStepSound));
}

RunAttack::~RunAttack() 
{
	I3_SAFE_DELETE( m_runSound);
}

void RunAttack::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void RunAttack::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void RunAttack::OnAnimStart() {}
void RunAttack::OnAnimRun(REAL32 tm) { m_runSound->OnUpdate(tm); }
void RunAttack::OnAnimEnd() 
{ 
	AnimationBase::OnAnimEnd(); 
	m_runSound->Reset();
}

//----------------------------------------------
// STOMP
//----------------------------------------------
Stomp::Stomp(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

Stomp::~Stomp() {}

void Stomp::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Stomp::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void Stomp::OnAnimStart() { m_stomped = false; }
void Stomp::OnAnimRun(REAL32 tm)
{
	if (m_stomped == false)
	{
		if (m_chara->GetAiContext()->getLocalTime()+tm >= MUTANTREX_STOMP_ATTACK_ANIM_TIME)
		{
			StompingOn();
			m_stomped = true;
		}
	}	
}
void Stomp::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

void Stomp::StompingOn()
{
	fb_misc::Effect(m_chara, "Exp_C5_01", DOMI_MUTANTREX_BONE_FOOT_LEFT);

	if (g_pCamera)
	{
		MATRIX* cam = g_pCamera->getCamMatrix();

		VEC3D temp;
		MATRIX* matLeftFoot = m_chara->getBoneContext()->getBoneMatrix(DOMI_MUTANTREX_BONE_FOOT_LEFT);
		i3Vector::Sub(&temp, i3Matrix::GetPos(matLeftFoot), i3Matrix::GetPos(cam));
		REAL32 length = i3Vector::Length(&temp);

		REAL32 maxLength = MUTANTREX_STOMP_DAMAGE_RANGE * 3;
		if (length < maxLength)
		{
			REAL32 trembAmpRate = 1.0f - (length/maxLength);
			g_pCamera->SetTremble( 5.0f * trembAmpRate * trembAmpRate, 7.0f * trembAmpRate, 0.1f, 2.f * trembAmpRate);
		}
	}
}

//----------------------------------------------
// HOWLING
//----------------------------------------------
Howling::Howling(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID), m_tm(0.0f) {}

Howling::~Howling() {}

void Howling::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void Howling::OnEvent(fsm::IMachine* m, fsm::IEvent* e) 
{ 
	if (DAMAGED_BY_DEATHBLOW == static_cast<fb_fsm::Event*>(e)->id)
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_BOSS_HOWL_CANCELED); // "하울링 중지" 메시지 출력

	AnimationBase::OnEvent(m, e);
}

void Howling::OnAnimStart() 
{ 
	m_tm = 0.0f; 
	domi_state_info::i()->boss_cur_hp_at_howl_start = m_chara->getCharaInfo()->GetHP();
}
void Howling::OnAnimRun(REAL32 tm) 
{
	m_tm += tm;

	if (m_tm >= 1.0f)
	{
		domi_state_info::i()->boss_howling_time -= 1;
		if (domi_state_info::i()->boss_howling_time < 0) 
			domi_state_info::i()->boss_howling_time = 0;
		UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_BOSS_HOWLING);

		m_tm = 0.0f;
	}

	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_BOSS_HOWL_HP);
}
void Howling::OnAnimEnd() 
{ 
	domi_state_info::i()->boss_howling_time = 6;
	domi_state_info::i()->boss_cur_hp_at_howl_start = 0.0f;
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::boss_howl_hp);
	UIHUDDominationMgr::i()->SetHudEvent( HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::boss_howling);

	AnimationBase::OnAnimEnd(); 
}

