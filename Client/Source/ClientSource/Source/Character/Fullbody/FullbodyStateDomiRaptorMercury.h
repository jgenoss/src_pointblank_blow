#ifndef _FULLBODY_STATE_DOMI_RAPTOR_MERCURY_H_
#define _FULLBODY_STATE_DOMI_RAPTOR_MERCURY_H_

#include "FullbodyFSM.h"

namespace fb_fsm
{
	namespace domi_raptor_mercury
	{
		//----------------------------------------------
		// States
		//----------------------------------------------
		class Jumping : public AnimationBase
		{	
		public:
			Jumping(CGameCharaFullbody* chara, INT32 id, INT32 startAnimID, INT32 nextAnimID);
			virtual ~Jumping();

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