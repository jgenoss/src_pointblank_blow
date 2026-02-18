#ifndef _FULLBODY_STATE_DOMI_ACID_ITEM_H
#define _FULLBODY_STATE_DOMI_ACID_ITEM_H

#include "FullbodyFSM.h"

namespace fb_fsm
{
	namespace domi_acid_item
	{
		//----------------------------------------------
		// States
		//----------------------------------------------
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
			bool m_explosed;
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