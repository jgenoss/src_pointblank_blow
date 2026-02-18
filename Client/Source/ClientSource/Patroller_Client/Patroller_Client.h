#pragma once

typedef		void (*ZGuard_CallbackProc)(unsigned int nErrorCode);
extern		ZGuard_CallbackProc	ZG_cbProc;


namespace ZGuard
{
	enum ERROR_CODE
	{
		CREATE_HANDLE_FAIL		= 0x00F10000,
		LAUNCHER_DISCONNECTED	= 0x00F20000,
	};

	extern bool Patroller_Start_Cli(ZGuard_CallbackProc cbProc);
	extern INT32 Patroller_End_Cli(void);
	extern void Patroller_SendInfo(UINT64 nUID);
	extern void Patroller_UpdateDLLLIst(void);
}
