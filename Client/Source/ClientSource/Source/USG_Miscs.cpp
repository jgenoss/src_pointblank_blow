#include "pch.h"

#include "USG_Miscs.h"

namespace usg_misc
{
	void HackToolDetectionCtx::OnMsgboxOKButtonClicked(void* pThis, INT32 nParam)
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_ERROR, 0);
	}

}