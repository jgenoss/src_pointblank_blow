#include "pch.h"

#include "FullbodyStateDomiAcid.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaBoneContext.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaMaterialContext.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

using namespace fb_fsm;
using namespace fb_fsm::domi_acid;

namespace
{
	#define ACID_WALK_LEFT_STEP_TIME		0.933f
	#define ACID_WALK_RIGHT_STEP_TIME		0.333f 
	#define ACID_RUN_LEFT_STEP_TIME			0.666f
	#define ACID_RUN_RIGHT_STEP_TIME		0.266f
	#define ACID_EXPLOSION_ANIM_TIME		0.150f
	#define ACID_BOMB_DAMAGE_RANGE			10.0f
}


namespace fb_fsm
{
	namespace domi_acid
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
				m_animations[IDLE]					= &static_creator<AnimationBase, Idle,				IDLE, REPEAT>::call;
				m_animations[LOOKAROUND]			= &static_creator<AnimationBase, AnimationBase,		LOOKAROUND, REPEAT>::call;
				m_animations[WALK]					= &static_creator<AnimationBase, Walk,				WALK, REPEAT>::call;
				m_animations[RUN]					= &static_creator<AnimationBase, Run,				RUN, REPEAT>::call;
				m_animations[DAMAGE_MID]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_MID, IDLE>::call;
				m_animations[DAMAGE_HIGH]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_HIGH, IDLE>::call;
				m_animations[DAMAGE_LOW]			= &static_creator<AnimationBase, AnimationBase,		DAMAGE_LOW, IDLE>::call;
				m_animations[KNOCKDOWN]				= &static_creator<AnimationBase, AnimationBase,		KNOCKDOWN, STANDUP>::call;
				m_animations[STANDUP]				= &static_creator<AnimationBase, AnimationBase,		STANDUP, IDLE>::call;
				m_animations[BITE]					= &static_creator<AnimationBase, AnimationBase,		BITE, IDLE>::call;
				m_animations[SUICIDE_ATTACK_READY]	= &static_creator<AnimationBase, SuicideAttackReady,SUICIDE_ATTACK_READY, REPEAT>::call;
				m_animations[SUICIDE_ATTACK]		= &static_creator<AnimationBase, SuicideAttack,		SUICIDE_ATTACK, NONE>::call;
				m_animations[JUMPSTART]				= &static_creator<AnimationBase, JumpStart,			JUMPSTART, JUMPING>::call;
				m_animations[JUMPING]				= &static_creator<AnimationBase, AnimationBase,		JUMPING, REPEAT>::call;
				m_animations[JUMPEND]				= &static_creator<AnimationBase, JumpEnd,			JUMPEND, IDLE>::call;
				m_animations[WARNKILL]				= &static_creator<AnimationBase, AnimationBase,		WARNKILL, IDLE>::call;
				m_animations[ROAR]					= &static_creator<AnimationBase, AnimationBase,		ROAR, IDLE>::call;
				m_animations[GROGGY]				= &static_creator<AnimationBase, AnimationBase,		GROGGY, REPEAT>::call;
				m_animations[BOOMING]				= &static_creator<AnimationBase, SuicideAttackReady,BOOMING, REPEAT>::call; // 이펙트는 SuicideAttackReady 클래스에서 처리.

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
			FuncPtr	m_animations[AI_MAX];			// [initialize at constructor]
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
	m_breath = fb_misc::BreathFactory::CreateDomiAcid(chara);
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
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(ACID_WALK_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(ACID_WALK_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);

	m_walkSound = new fb_misc::BipedalSound( pFootStepLeft, pFootStepRight );

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
	fb_misc::OneFootStepSound * pFootStepLeft = new fb_misc::OneFootStepSound(ACID_RUN_LEFT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_LEFT, true);
	fb_misc::OneFootStepSound * pFootStepRight = new fb_misc::OneFootStepSound(ACID_RUN_RIGHT_STEP_TIME, chara, DOMI_RAPTOR_BONE_FOOT_RIGHT, false);
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
// SUICIDE_ATTACK_READY (반복)
//----------------------------------------------
SuicideAttackReady::SuicideAttackReady(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

SuicideAttackReady::~SuicideAttackReady() {}

void SuicideAttackReady::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void SuicideAttackReady::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
void SuicideAttackReady::OnAnimStart() {}
void SuicideAttackReady::OnAnimRun(REAL32 tm)
{
	// 스킨 색을 바꾼다.
	COLORREAL mtrl;
	mtrl.r = 0.0f;
	mtrl.g = 0.0f;
	mtrl.b = 0.0f;
	mtrl.a = 0.0f;

	m_mtrlRed += m_mtrlRedIncrFactor;
	if (m_mtrlRed < 0.0f || 1.0f < m_mtrlRed)
		m_mtrlRedIncrFactor *= -1.0f;

	mtrl.r = m_mtrlRed;
	m_chara->getMaterialContext()->SetMaterialColor(&mtrl, &mtrl);

	fb_misc::Effect(m_chara, "Dino_Acid_Fire_B2", DOMI_RAPTOR_BONE_ROOT);
}
void SuicideAttackReady::OnAnimEnd() { AnimationBase::OnAnimEnd(); }

//----------------------------------------------
// SUICIDE_ATTACK
//----------------------------------------------
SuicideAttack::SuicideAttack(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

SuicideAttack::~SuicideAttack() {}

void SuicideAttack::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void SuicideAttack::OnEvent(fsm::IMachine* m, fsm::IEvent* e) {}
void SuicideAttack::OnAnimStart() { m_explosed = false; }
void SuicideAttack::OnAnimRun(REAL32 tm)
{
	if (m_explosed == false)
	{
		if (m_chara->GetAiContext()->getLocalTime()+tm > ACID_EXPLOSION_ANIM_TIME)
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

	fb_misc::Effect(m_chara, "HMK_Dino_Fire_Attack_B", DOMI_RAPTOR_BONE_ROOT);

	m_chara->DisableScene();

	if (g_pCamera)
	{
		MATRIX* cam = g_pCamera->getCamMatrix();
		MATRIX* mat = m_chara->getBoneContext()->getBoneMatrix(DOMI_RAPTOR_BONE_ROOT);

		REAL32 length = ACID_BOMB_DAMAGE_RANGE;
		VEC3D temp;
		i3Vector::Sub(&temp, i3Matrix::GetPos(mat), i3Matrix::GetPos(cam));
		length = i3Vector::Length(&temp);

		REAL32 maxLength = ACID_BOMB_DAMAGE_RANGE * 3;
		if (length < maxLength)
		{
			REAL32 trembAmpRate = 1.0f - (length/maxLength);
			g_pCamera->SetTremble( 5.0f * trembAmpRate * trembAmpRate, 7.0f * trembAmpRate, 0.1f, 2.f * trembAmpRate);
		}
	}	
}

//----------------------------------------------
// JUMPSTART
//----------------------------------------------
JumpStart::JumpStart(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID) 
	: AnimationBase(chara, id, startAnimID, nextAnimID) {}

JumpStart::~JumpStart() {}

void JumpStart::OnEntry(fsm::IMachine* m, fsm::IState* s) { AnimationBase::OnEntry(m, s); }
void JumpStart::OnEvent(fsm::IMachine* m, fsm::IEvent* e) { AnimationBase::OnEvent(m, e); }
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

