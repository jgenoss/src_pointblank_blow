#include "pch.h"
#include "FullbodyStateDomiAcidItem.h"
#include "FullbodyStateDomiAcid.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../UI/hud/UIHUD_Domination_Manager.h"

namespace
{
	#define ACID_ITEM_BOMB_TREMBLE_RANGE		1000.0f
	#define ACID_ITEM_EXPLOSION_ANIM_TIME		0.150f
}

using namespace fb_fsm;
using namespace fb_fsm::domi_acid_item;

namespace fb_fsm
{
	namespace domi_acid_item
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

				m_animations[NONE]				= &static_creator<AnimationBase, AnimationBase,		NONE, NONE>::call;
				m_animations[IDLE]				= &static_creator<AnimationBase, domi_acid::Idle,	IDLE, REPEAT>::call;
				m_animations[WALK]				= &static_creator<AnimationBase, domi_acid::Walk,	WALK, REPEAT>::call;
				m_animations[RUN]				= &static_creator<AnimationBase, domi_acid::Run,	RUN, REPEAT>::call;
				m_animations[SUICIDE_ATTACK]	= &static_creator<AnimationBase, SuicideAttack,		SUICIDE_ATTACK, NONE>::call;	// acid item 전용
				m_animations[JUMPSTART]			= &static_creator<AnimationBase, domi_acid::JumpStart,	JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]			= &static_creator<AnimationBase, AnimationBase,			JUMPING, REPEAT>::call;
				m_animations[JUMPEND]			= &static_creator<AnimationBase, domi_acid::JumpEnd,	JUMPING, IDLE>::call;

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
// SUICIDE_ATTACK
//----------------------------------------------
SuicideAttack::SuicideAttack(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID)
	: AnimationBase(chara, id, startAnimID, nextAnimID), m_explosed(false) {}

SuicideAttack::~SuicideAttack() {}

void SuicideAttack::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void SuicideAttack::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void SuicideAttack::OnAnimStart() { m_explosed = false; }
void SuicideAttack::OnAnimRun(REAL32 tm)
{
	if (m_explosed == false)
	{
		if (m_chara->GetAiContext()->getLocalTime()+tm > ACID_ITEM_EXPLOSION_ANIM_TIME)
		{
			Explosion();
			m_explosed = true;
		}
	}	
}
void SuicideAttack::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

void SuicideAttack::Explosion()
{
	fb_misc::Death(m_chara, GTBD_ON_ACID);

	UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_INDICATOR, HUD_DMN_EVT_DELETE_CHARACTER_OBJECT, m_chara->getCharaNetIndex());
	INT8 count = --domi_state_info::i()->etc_count;
	if (count <= 0)
		UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_INVISIBLE, dui::etc);

	fb_misc::Effect(m_chara, "HMK_Dino_Fire_Attack_B", DOMI_RAPTOR_BONE_ROOT);

	m_chara->DisableScene();

	if (g_pCamera)
	{
		MATRIX* cam = g_pCamera->getCamMatrix();
		MATRIX* mat = m_chara->getBoneContext()->getBoneMatrix(DOMI_RAPTOR_BONE_ROOT);

		REAL32 length = 0.0f;
		VEC3D temp;
		i3Vector::Sub(&temp, i3Matrix::GetPos(mat), i3Matrix::GetPos(cam));
		length = i3Vector::Length(&temp);

		REAL32 maxLength = ACID_ITEM_BOMB_TREMBLE_RANGE;
		if (length < maxLength)
		{
			REAL32 trembAmpRate = 1.0f - (length/maxLength);
			g_pCamera->SetTremble( 5.0f * trembAmpRate * trembAmpRate, 7.0f * trembAmpRate, 0.1f, 2.f * trembAmpRate);
		}
	}	
}