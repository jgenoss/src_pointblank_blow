#ifndef _FULLBODY_STATE_DOMI_STING_MARS_H_
#define _FULLBODY_STATE_DOMI_STING_MARS_H_

#include "FullbodyFSM.h"

namespace fb_fsm
{
	namespace domi_sting_mars
	{
		//----------------------------------------------
		// States
		//----------------------------------------------

		//----------------------------------------------
		// Helper Functions
		//----------------------------------------------
		AnimationBase*	CreateAnimation(INT32 id, CGameCharaFullbody* chara);
		void	DestroyAnimation(AnimationBase*& lt);
		INT32	GetAnimationCount();
	}
}

#endif