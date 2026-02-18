#ifndef _FULLBODY_STATE_DOMI_ACID_VULCAN_H_
#define _FULLBODY_STATE_DOMI_ACID_VULCAN_H_

#include "FullbodyFSM.h"

namespace fb_fsm
{
	namespace domi_acid_vulcan
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