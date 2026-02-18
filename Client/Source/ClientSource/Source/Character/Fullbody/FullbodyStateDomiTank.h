#ifndef _FULLBODY_STATE_DOMI_TANK_H_
#define _FULLBODY_STATE_DOMI_TANK_H_

#include "FullbodyFSM.h"
#include "FullbodyMisc.h"

namespace fb_fsm
{
	namespace domi_tank
	{
		//----------------------------------------------
		// States
		//----------------------------------------------
		class Idle : public AnimationBase
		{	
		public:
			Idle(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Idle();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			fb_misc::Breath* m_breath;
		};

		class Walk : public AnimationBase
		{	
		public:
			Walk(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Walk();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			fb_misc::BipedalSound* m_walkSound;
		};

		class Run : public AnimationBase
		{	
		public:
			Run(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Run();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			fb_misc::BipedalSound* m_runSound;
		};

		class DeathByGrenade : public AnimationBase
		{	
		public:
			DeathByGrenade(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~DeathByGrenade();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class DeathByHeadShot : public AnimationBase
		{	
		public:
			DeathByHeadShot(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~DeathByHeadShot();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class DeathMid1 : public AnimationBase
		{	
		public:
			DeathMid1(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~DeathMid1();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class DeathMid2 : public AnimationBase
		{	
		public:
			DeathMid2(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~DeathMid2();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class DeathMid3 : public AnimationBase
		{	
		public:
			DeathMid3(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~DeathMid3();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class Rush : public AnimationBase
		{	
		public:
			Rush(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Rush();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			fb_misc::BipedalSound* m_rushSound;
		};

		class Bite : public AnimationBase
		{	
		public:
			Bite(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Bite();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			bool m_bite;
			MATRIX* m_matBitePoint;
		};

		//----------------------------------------------
		// Helper Functions
		//----------------------------------------------
		AnimationBase*	CreateAnimation(INT32 id, CGameCharaFullbody* chara);
		void	DestroyAnimation(AnimationBase*& lt);
		INT32	GetAnimationCount();
	}
}

#endif