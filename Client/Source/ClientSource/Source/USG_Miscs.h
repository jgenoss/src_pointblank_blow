#ifndef _ULSAN_GUARD_MISCS_
#define _ULSAN_GUARD_MISCS_

namespace usg_misc
{
	struct HackToolDetectionCtx : i3::shared_ginst<HackToolDetectionCtx>
	{
		bool isHackDetected = false;

		static void OnMsgboxOKButtonClicked(void* pThis, INT32 nParam);
	};
}


#endif
