#ifndef _FB_MISC_H_
#define _FB_MISC_H_

#include "FullbodyDef.h"
#include "FullbodyFSM.h"
#include "FullbodyBiStateMapFactory.h"

class CGameCharaFullbody;

namespace fb_misc
{
	class TimeEvent;
	class OneFootStepSound;
	class BipedalSound;

	class TimeEvent
	{
	public:
		TimeEvent(REAL32 target) : m_target(target) {}
		virtual ~TimeEvent() {}

		bool OnUpdate(REAL32 tm);
		virtual void operator()() = 0;

	protected:
		REAL32 m_current = 0.0f;
		REAL32 m_target;		// [initialize at constructor]
	};

	class OneFootStepSound : public TimeEvent
	{
	public:
		OneFootStepSound(REAL32 target, CGameCharaFullbody* chara, INT32 boneIdx, bool isLeft)
			: TimeEvent(target), m_chara(chara), m_boneIdx(boneIdx), m_isLeft(isLeft) {}
		virtual ~OneFootStepSound() {}

		virtual void operator()();
		void ResetCurrentTime();

	private:
		CGameCharaFullbody* m_chara;		// [initialize at constructor]
		INT32 m_boneIdx;	// [initialize at constructor]
		bool m_isLeft;		// [initialize at constructor]
	};

	enum FOOT_STEP_TYPE { FOOT_STEP_LEFT, FOOT_STEP_RIGHT, FOOT_STEP_MAX };
	class BipedalSound
	{
	public:
		BipedalSound(TimeEvent* left, TimeEvent* right);
		~BipedalSound();

		void OnUpdate(REAL32 tm);
		void Reset();

	private:
		TimeEvent* m_footstep[FOOT_STEP_MAX];	// [initialize at constructor]
		TimeEvent* m_current[FOOT_STEP_MAX];	// [initialize at constructor]
	};

#define BREATH_TERM 5.0
	class Breath
	{
	public:
		Breath(CGameCharaFullbody* chara, GSND_TYPE_BY_DINO snd) 
			: m_chara(chara), m_snd(snd) {}
		~Breath() {};

		void OnPlay(REAL32 tm);

	private:
		Breath(const Breath& lhs);
		Breath& operator=(const Breath& lhs);

	private:
		CGameCharaFullbody*	m_chara;	// [initialize at constructor]
		REAL32				m_tm = BREATH_TERM;
		GSND_TYPE_BY_DINO	m_snd;		// [initialize at constructor]
	};

	class BreathFactory
	{
	public:
		static Breath* CreateDomiAcid(CGameCharaFullbody* acid)
		{
			Breath * p = new Breath(acid, GTBD_ON_ACID);
			MEMDUMP_NEW( p, sizeof( Breath));
			return p;
		}
		static Breath* CreateDomiRaptor(CGameCharaFullbody* raptor)
		{
			Breath * p = new Breath(raptor, GTBD_ON_RAPTOR);
			MEMDUMP_NEW( p, sizeof( Breath));
			return p;
		}
		static Breath* CreateDomiSting(CGameCharaFullbody* sting)
		{
			Breath * p = new Breath(sting, GTBD_ON_STING);
			MEMDUMP_NEW( p, sizeof( Breath));
			return p;
		}
		static Breath* CreateDomiMutantRex(CGameCharaFullbody* mut)
		{
			Breath * p = new Breath(mut, GTBD_ON_TREX);
			MEMDUMP_NEW( p, sizeof( Breath));
			return p;
		}
		static Breath* CreateDomiTank(CGameCharaFullbody* tank)
		{
			Breath * p = new Breath(tank, GTBD_ON_TANK);
			MEMDUMP_NEW( p, sizeof( Breath));
			return p;
		}
	};

	void Hide(CGameCharaFullbody* chara);
	void Death(CGameCharaFullbody* chara, GSND_TYPE_BY_DINO deathSnd);
	void Effect(CGameCharaFullbody* chara, const char* effectName, INT32 boneIdx);
	void Effect(CGameCharaFullbody* chara, const char* effectName, MATRIX* mat);

	// 서버에서는 애니메이션 상태를 (main, sub) 쌍으로 주는데,
	// sub 상태가 main에 속하는지 확인해야할 경우가 있다(ex> 이동 중인지 아닌지 확인해야할 경우...)
	template <INT32 MAINSTATE>
	bool IsKindof(fb_fsm::State* state, CGameCharaFullbody* chara)
	{
		fb_fsm::BiStateKey key(chara->GetFBStateMgr()->GetBiStateMap()->Find(state->GetID()));

		return (MAINSTATE == key.state);
	}
}

#endif