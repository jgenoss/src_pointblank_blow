#include "pch.h"
#include "FullbodyStateDomiAcidNotBomb.h"
#include "FullbodyStateDomiAcid.h"
#include "GameCharaFullbody.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_acid_not_bomb;

namespace fb_fsm
{
	namespace domi_acid_not_bomb
	{
		//----------------------------------------------
		// State »ý¼º ÆÑÅä¸®
		//----------------------------------------------
		class AnimationFactory
		{
		public:
			AnimationFactory()
			{
				ZeroMemory(m_animations, sizeof(m_animations));

				m_animations[NONE]					= &static_creator<AnimationBase, AnimationBase,		NONE, NONE>::call;
				m_animations[IDLE]					= &static_creator<AnimationBase, domi_acid::Idle,	IDLE, REPEAT>::call;
				m_animations[LOOKAROUND]			= &static_creator<AnimationBase, AnimationBase,		LOOKAROUND, REPEAT>::call;
				m_animations[WALK]					= &static_creator<AnimationBase, domi_acid::Walk,	WALK, REPEAT>::call;
				m_animations[RUN]					= &static_creator<AnimationBase, domi_acid::Run,	RUN, REPEAT>::call;
				m_animations[DAMAGE_MID]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_MID, IDLE>::call;
				m_animations[DAMAGE_HIGH]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_HIGH, IDLE>::call;
				m_animations[DAMAGE_LOW]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_LOW, IDLE>::call;
				m_animations[KNOCKDOWN]				= &static_creator<AnimationBase, AnimationBase,		KNOCKDOWN, STANDUP>::call;
				m_animations[STANDUP]				= &static_creator<AnimationBase, AnimationBase,		STANDUP, IDLE>::call;
				m_animations[DEATH_BY_GRENADE]		= &static_creator<AnimationBase, DeathGrenade,		DEATH_BY_GRENADE, NONE>::call;
				m_animations[DEATH_BY_HEADSHOT]		= &static_creator<AnimationBase, DeathHead,			DEATH_BY_HEADSHOT, NONE>::call;
				m_animations[DEATH_FRONT]			= &static_creator<AnimationBase, DeathFront,		DEATH_FRONT, NONE>::call;
				m_animations[BITE]					= &static_creator<AnimationBase, AnimationBase,		BITE, IDLE>::call;
				m_animations[JUMPSTART]				= &static_creator<AnimationBase, domi_acid::JumpStart,	JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]				= &static_creator<AnimationBase, AnimationBase,			JUMPING, REPEAT>::call;
				m_animations[JUMPEND]				= &static_creator<AnimationBase, domi_acid::JumpEnd,	JUMPEND, IDLE>::call;
				m_animations[WARNKILL]				= &static_creator<AnimationBase, AnimationBase,		WARNKILL, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase,		ROAR, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase,		GROGGY, REPEAT>::call;

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
// DEATH_BY_GRENADE
//----------------------------------------------
DeathGrenade::DeathGrenade(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathGrenade::~DeathGrenade() {}

void DeathGrenade::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathGrenade::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathGrenade::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_ACID); }
void DeathGrenade::OnAnimRun(REAL32 tm) {}
void DeathGrenade::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_BY_HEADSHOT
//----------------------------------------------
DeathHead::DeathHead(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathHead::~DeathHead() {}

void DeathHead::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathHead::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathHead::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_ACID); }
void DeathHead::OnAnimRun(REAL32 tm) {}
void DeathHead::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// DEATH_FRONT
//----------------------------------------------
DeathFront::DeathFront(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

DeathFront::~DeathFront() {}

void DeathFront::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void DeathFront::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void DeathFront::OnAnimStart() { fb_misc::Death(m_chara, GTBD_ON_ACID); }
void DeathFront::OnAnimRun(REAL32 tm) {}
void DeathFront::OnAnimEnd() { AnimationBase::OnAnimEnd(); }