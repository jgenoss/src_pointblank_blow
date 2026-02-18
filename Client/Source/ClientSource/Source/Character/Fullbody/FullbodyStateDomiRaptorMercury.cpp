#include "pch.h"

#include "FullbodyStateDomiRaptorMercury.h"
#include "FullbodyStateDomiRaptor.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../GameCharaMoveContext.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_raptor_mercury;

namespace fb_fsm
{
	namespace domi_raptor_mercury
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
				m_animations[IDLE]					= &static_creator<AnimationBase, domi_raptor::Idle, IDLE, REPEAT>::call;
				m_animations[FORAGE]				= &static_creator<AnimationBase, AnimationBase,		FORAGE, REPEAT>::call;
				m_animations[WALK]					= &static_creator<AnimationBase, domi_raptor::Walk, WALK, REPEAT>::call;
				m_animations[RUN]					= &static_creator<AnimationBase, domi_raptor::Run,	RUN, REPEAT>::call;
				m_animations[DAMAGE_MID]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_MID, IDLE>::call;
				m_animations[DAMAGE_HIGH]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_HIGH, IDLE>::call;
				m_animations[DAMAGE_LOW]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_LOW, IDLE>::call;
				m_animations[KNOCKDOWN]				= &static_creator<AnimationBase, AnimationBase,		KNOCKDOWN, STANDUP>::call;
				m_animations[STANDUP]				= &static_creator<AnimationBase, AnimationBase,		STANDUP, IDLE>::call;
				m_animations[DEATH_BY_GRENADE]		= &static_creator<AnimationBase, domi_raptor::DeathByGrenade,	DEATH_BY_GRENADE, NONE>::call;
				m_animations[DEATH_BY_HEADSHOT]		= &static_creator<AnimationBase, domi_raptor::DeathByHeadShot,	DEATH_BY_HEADSHOT, NONE>::call;
				m_animations[DEATH_MID1]			= &static_creator<AnimationBase, domi_raptor::DeathMid1,		DEATH_MID1, NONE>::call;
				m_animations[DEATH_MID2]			= &static_creator<AnimationBase, domi_raptor::DeathMid2,		DEATH_MID2, NONE>::call;
				m_animations[DEATH_MID3]			= &static_creator<AnimationBase, domi_raptor::DeathMid3,		DEATH_MID3, NONE>::call;
				m_animations[BITE]					= &static_creator<AnimationBase, AnimationBase,		BITE, IDLE>::call;
				m_animations[JUMPATTACK]			= &static_creator<AnimationBase, AnimationBase,		JUMPATTACK, IDLE>::call;
				m_animations[JUMPSTART]				= &static_creator<AnimationBase, domi_raptor::JumpStart, JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]				= &static_creator<AnimationBase, AnimationBase,			 JUMPING, REPEAT>::call;
				m_animations[JUMPEND]				= &static_creator<AnimationBase, domi_raptor::JumpEnd,   JUMPEND, IDLE>::call;
				m_animations[WARNKILL]				= &static_creator<AnimationBase, AnimationBase,		WARNKILL, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase,		ROAR, IDLE>::call;
				m_animations[LEAP_READY_BEND_OVER]	= &static_creator<AnimationBase, AnimationBase,		LEAP_READY_BEND_OVER, LEAP_READY_QUAKING>::call;
				m_animations[LEAP_READY_QUAKING]	= &static_creator<AnimationBase, domi_raptor::LeapReadyQuaking, LEAP_READY_QUAKING, REPEAT>::call;
				m_animations[LEAP_START]			= &static_creator<AnimationBase, AnimationBase,		LEAP_START, LEAP>::call;
				m_animations[LEAP]					= &static_creator<AnimationBase, domi_raptor::Leap, LEAP, REPEAT>::call;
				m_animations[LEAP_ATTACK]			= &static_creator<AnimationBase, AnimationBase,		LEAP_ATTACK, IDLE>::call;
				m_animations[LEAP_FAIL]				= &static_creator<AnimationBase, AnimationBase,		LEAP_FAIL, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase,		GROGGY, REPEAT>::call;

				for (INT32 i=0; i<AI_MAX; i++)
				{
					if (m_animations[i] == nullptr)
						m_animations[i] = &static_creator<AnimationBase, AnimationBase, NONE, NONE>::call;
				}
			}

			typedef AnimationBase* (*FuncPtr)(CGameCharaFullbody* chara, INT32 id);

			FuncPtr	GetFunction(INT32 id) const { return m_animations[id];		}
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
