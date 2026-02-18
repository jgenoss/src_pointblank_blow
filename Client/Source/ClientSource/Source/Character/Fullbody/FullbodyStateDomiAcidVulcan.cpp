#include "pch.h"

#include "FullbodyStateDomiAcidVulcan.h"
#include "FullbodyStateDomiAcid.h"
#include "GameCharaFullbody.h"

using namespace fb_fsm;
using namespace fb_fsm::domi_acid_vulcan;

namespace fb_fsm
{
	namespace domi_acid_vulcan
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
				m_animations[BITE]					= &static_creator<AnimationBase, AnimationBase,		BITE, IDLE>::call;
				m_animations[SUICIDE_ATTACK_READY]	= &static_creator<AnimationBase, domi_acid::SuicideAttackReady,	SUICIDE_ATTACK_READY, REPEAT>::call;
				m_animations[SUICIDE_ATTACK]		= &static_creator<AnimationBase, domi_acid::SuicideAttack,		SUICIDE_ATTACK, NONE>::call;
				m_animations[JUMPSTART]				= &static_creator<AnimationBase, domi_acid::JumpStart,			JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]				= &static_creator<AnimationBase, AnimationBase,		JUMPING, REPEAT>::call;
				m_animations[JUMPEND]				= &static_creator<AnimationBase, domi_acid::JumpEnd,JUMPEND, IDLE>::call;
				m_animations[WARNKILL]				= &static_creator<AnimationBase, AnimationBase,		WARNKILL, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase,		ROAR, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase,		GROGGY, REPEAT>::call;
				m_animations[BOOMING]				= &static_creator<AnimationBase, domi_acid::SuicideAttackReady,BOOMING, REPEAT>::call; // 이펙트는 SuicideAttackReady 클래스에서 처리.

				for (INT32 i=0; i<AI_MAX; i++)
				{
					if (m_animations[i] == nullptr)
						m_animations[i] = &static_creator<AnimationBase, AnimationBase, NONE, NONE>::call;
				}
			}

			typedef AnimationBase* (*FuncPtr)(CGameCharaFullbody* chara, INT32 id);

			FuncPtr	GetFunction(INT32 id) const { return m_animations[id];			}
			INT32	GetAnimationCount() const		{ return _countof(m_animations);	}

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

