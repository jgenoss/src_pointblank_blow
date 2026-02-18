#ifndef _FULLBODY_STATE_DOMI_ACID_H_
#define _FULLBODY_STATE_DOMI_ACID_H_

#include "FullbodyFSM.h"
#include "FullbodyMisc.h"

namespace fb_fsm
{
	namespace domi_acid
	{
		//----------------------------------------------
		// Animations
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
			fb_misc::Breath* m_breath;		// [initialize at constructor]
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
			fb_misc::BipedalSound* m_walkSound;		// [initialize at constructor]
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
			fb_misc::BipedalSound* m_runSound;			// [initialize at constructor]
		};

		class SuicideAttackReady : public AnimationBase
		{	
		public:
			SuicideAttackReady(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~SuicideAttackReady();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			REAL32 m_mtrlRedIncrFactor = 0.5f;
			REAL32 m_mtrlRed = 0.0f;
		};

		class SuicideAttack : public AnimationBase
		{	
		public:
			SuicideAttack(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~SuicideAttack();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;

		private:
			void Explosion();

		private:
			bool m_explosed = false;
		};

		class JumpStart : public AnimationBase
		{	
		public:
			JumpStart(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~JumpStart();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
		};

		class JumpEnd : public AnimationBase
		{	
		public:
			JumpEnd(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~JumpEnd();

			virtual void OnEntry(fsm::IMachine* m, fsm::IState* s) override;
			virtual void OnEvent(fsm::IMachine* m, fsm::IEvent* e) override;
			virtual void OnAnimStart() override;
			virtual void OnAnimRun(REAL32 tm) override;
			virtual void OnAnimEnd() override;
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