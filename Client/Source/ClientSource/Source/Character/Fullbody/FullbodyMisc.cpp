#include "pch.h"

#include "FullbodyMisc.h"
#include "GameCharaFullbody.h"
#include "../../GameCharaSoundContext.h"
#include "../../GameCharaMoveContext.h"
#include "../../GameCharaBoneContext.h"
#include "../../GameCharaUpdateContext.h"
#include "../../GameCharaCameraContext.h"
#include "../../GameCharaCollisionContext.h"
#include "../../Weapon/WeaponDino.h"


namespace fb_misc
{
	bool TimeEvent::OnUpdate(REAL32 tm)
	{
		m_current += tm;
		if (m_current >= m_target)
		{
			(*this)();
			return true;
		}

		return false;
	}

	void OneFootStepSound::operator()()
	{
		if (m_chara->isCharaStateMask(CHARA_STATE_DEATH)) return;

		m_chara->getSoundContext()->PlayFootStep(m_chara->getMoveContext()->getFloorTerrainType(), m_isLeft, false);

		if (m_isLeft == true)	m_chara->removeCharaStateMask(CHARA_STATE_FOOTSTEPRIGHT);
		else					m_chara->addCharaStateMask(CHARA_STATE_FOOTSTEPRIGHT);

		// effect
		VEC3D pos;
		CGameCharaBoneContext* boneCtx = m_chara->getBoneContext();
		if (boneCtx)
		{
			if (m_chara->IsDomiMutantRex())
			{
				MATRIX* mat = nullptr;
				if (m_isLeft)
					mat = boneCtx->getBoneMatrix(DOMI_MUTANTREX_BONE_FOOT_LEFT);
				else
					mat = boneCtx->getBoneMatrix(DOMI_MUTANTREX_BONE_FOOT_RIGHT);

				i3Vector::Copy(&pos, i3Matrix::GetPos(mat));
				setY(&pos, getY(m_chara->GetPos()));

				g_pEffectManager->AddEnvironmentEffect(ENV_EFEECT_TREX_WALK, &pos, &I3_YAXIS);
			}
			else if (m_chara->IsDomiTank())
			{
				MATRIX* mat = nullptr;
				if (m_isLeft)
					mat = boneCtx->getBoneMatrix(DOMI_RAPTOR_BONE_FOOT_LEFT);
				else
					mat = boneCtx->getBoneMatrix(DOMI_RAPTOR_BONE_FOOT_RIGHT);

				i3Vector::Copy(&pos, i3Matrix::GetPos(mat));
				setY(&pos, getY(m_chara->GetPos()));

				g_pEffectManager->AddEnvironmentEffect(ENV_EFEECT_TREX_WALK, &pos, &I3_YAXIS);
			}
		}

	}

	void OneFootStepSound::ResetCurrentTime()
	{
		m_current = 0.0f;
	}

	class None : public TimeEvent
	{
	public:
		None() : TimeEvent(0.0f) {}
		virtual ~None() {}
		virtual void operator()() {}
	}g_none;

	BipedalSound::BipedalSound(TimeEvent* left, TimeEvent* right)
	{
		TimeEvent* evt[FOOT_STEP_MAX] = {left, right};
		for (INT32 i=0; i<FOOT_STEP_MAX; i++)
		{
			m_footstep[i] = evt[i];
			m_current[i] = evt[i];
		}
	}

	BipedalSound::~BipedalSound() 
	{ 
		for (INT32 i=0; i<FOOT_STEP_MAX; i++)
		{
			m_current[i] = 0;
			I3_SAFE_DELETE( m_footstep[i]);
		}
	}

	void BipedalSound::OnUpdate(REAL32 tm)
	{
		for (INT32 i=0; i<FOOT_STEP_MAX; i++)
		{
			if (m_current[i]->OnUpdate(tm))
			{
				if (m_current[i] != &g_none)
					m_current[i] = &g_none;
			}
		}

		if (m_current[FOOT_STEP_LEFT] == &g_none &&
			m_current[FOOT_STEP_RIGHT] == &g_none)
		{
			Reset();
		}
	}

	void BipedalSound::Reset()
	{
		for (INT32 i=0; i<FOOT_STEP_MAX; i++)
		{
			m_current[i] = m_footstep[i];
			static_cast<OneFootStepSound*>(m_current[i])->ResetCurrentTime();
		}
	}

	void Breath::OnPlay(REAL32 tm)
	{
		m_tm -= tm;
		if (m_tm < 0.0f)
		{
			m_tm = BREATH_TERM;

			INT32 rand = i3Math::Rand()%3;
			g_pSndMng->StartDinoActionSound(m_snd, GTBDA_ON_BREATH_1+rand, m_chara->GetPos(), m_chara->is1PV());
		}
	}


	void Hide(CGameCharaFullbody* chara)
	{
		chara->getMoveContext()->setVelocity(0.0f, 0.0f, 0.0f);
		chara->getMoveContext()->setAccel(0.0f, 0.0f, 0.0f);
		chara->getUpdateContext()->setToHideTime(1.0f);
		chara->getUpdateContext()->setHideTimer(1.0f);

		// 컨트롤러의 컬리젼을 끕니다.
		chara->getMoveContext()->EnableController( false );
	}

	void Death(CGameCharaFullbody* chara, GSND_TYPE_BY_DINO deathSnd)
	{
		chara->addCharaStateMask(CHARA_STATE_DEATH);
		chara->setCharaInfo()->SetHP(0.0f);
		Hide(chara);

		chara->getCollisionContext()->SetContactShapeSetEnable(true);

		if( chara->getCollisionContext()->getFloorHitResult()->m_T > 0.5f)
		{
			chara->getUpdateContext()->setDeathOriginalHeight(getY( chara->GetPos()));

			REAL32 rOffset = -(chara->getCollisionContext()->getFloorHitResult()->m_T - 0.5f);
			addY(chara->GetPos(), rOffset );

			chara->getUpdateContext()->setDeathBlendHeight(getY( chara->GetPos()));
		}
		else
		{
			chara->getUpdateContext()->setDeathOriginalHeight(getY(chara->GetPos()));
			chara->getUpdateContext()->setDeathBlendHeight(getY(chara->GetPos()));
		}
	}

	void Effect(CGameCharaFullbody* chara, const char* effectName, INT32 boneIdx)
	{
		INT32 idx = g_pEffectManager->FindEffect(effectName);
		MATRIX* mat =  chara->getBoneContext()->getBoneMatrix(boneIdx);
		VEC3D pos; i3Vector::Copy(&pos, i3Matrix::GetPos(mat));
		VEC3D norm; i3Vector::Copy(&norm, i3Matrix::GetUp(mat));
		g_pEffectManager->AddEffect(idx, &pos, &norm);
	}

	void Effect(CGameCharaFullbody* chara, const char* effectName, MATRIX* mat)
	{
		INT32 idx = g_pEffectManager->FindEffect(effectName);
		VEC3D pos; i3Vector::Copy(&pos, i3Matrix::GetPos(mat));
		VEC3D norm; i3Vector::Copy(&norm, i3Matrix::GetUp(mat));
		g_pEffectManager->AddEffect(idx, &pos, &norm);
	}
}

