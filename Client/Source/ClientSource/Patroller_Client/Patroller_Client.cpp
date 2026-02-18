#include "stdafx.h"

#include "Patroller_Thread.h"
#include "Patroller_Client.h"
#include "Patroller_Shared_Struct.h"

Patroller_Thread	PThread;
ZGuard_CallbackProc	ZG_cbProc;


namespace ZGuard
{
	bool Patroller_Start_Cli(ZGuard_CallbackProc cbProc)
	{
		ZG_cbProc = cbProc;
		bool ret = true;
		ret &= PThread.CreateModule();
		return ret;
	}

	INT32 Patroller_End_Cli(void)
	{
		PThread.SetThreadExit(true);
		return 0;
	}

	void Patroller_UpdateDLLLIst(void)
	{
		PThread.SetProtocolOnce(PAT::PROTOCOL_DLL_LIST);
	}


	void Patroller_SendInfo(UINT64 nUID)
	{
		PThread.SetUID(nUID);
		PThread.SetProtocolOnce(PAT::PROTOCOL_ACCOUNT_INFO);
	}

}
