// i3Export2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Resource.h"
#include "Util.h"
#include "i3EXPClassDesc.h"
#include "i3ImageClassDesc.h"
#include "i3Image.h"

static int s_bInitControl = false;


#ifdef BUG_TRAP
	#include "BugTrap.h"
	#pragma comment(lib, "BugTrap.lib") // Link to ANSI DLL
	// #pragma comment(lib, "BugTrapU.lib") // Link to Unicode DLL

	static void SetupExceptionHandler()
	{
		BT_InstallSehFilter();
		BT_SetAppName(_T("i3MaxExporter"));
		// BT_SetSupportEMail(_T("wishmaster@zepetto.com"));

		UINT32 BT_Flag = 0;
		{
			BT_Flag = BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_LISTPROCESSES |  BTF_SHOWADVANCEDUI;
		}

		BT_SetFlags(BT_Flag);
		BT_SetReportFormat(BTRF_TEXT);
		BT_SetSupportServer(_T("122.199.144.237"), 9999);
		BT_SetDumpType(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpFilterMemory);
		BT_SetAppVersion("1.0");

  //MiniDumpFilterModulePaths = 0x0080, 
  //MiniDumpWithProcessThreadData = 0x0100, 
  //MiniDumpWithPrivateReadWriteMemory = 0x0200

		// BT_SetSupportURL(_T("http://www.zepetto.com"));

		/*g_iBugTrapLogHandle = BT_OpenLogFile(NULL, BTLF_TEXT);
		BT_SetLogSizeInBytes(g_iBugTrapLogHandle, 10000);
		BT_SetLogFlags(g_iBugTrapLogHandle, BTLF_SHOWTIMESTAMP);
		BT_SetLogEchoMode(g_iBugTrapLogHandle, BTLE_STDERR | BTLE_DBGOUT);*/

		/*PCTSTR pszLogFileName = BT_GetLogFileName(g_iBugTrapLogHandle);
		BT_AddLogFile(pszLogFileName);*/
	}
#endif

/*
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch( ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH :
			hInstance = (HINSTANCE) hModule;

			DisableThreadLibraryCalls( hInstance);
			break;
	}

    return TRUE;
}
*/

I3_EXPORT_EXP int LibInitialize(void)
{
	// Initialize the custom controls. This should be done only once.
	if (!s_bInitControl) 
	{
		s_bInitControl = true;

		(HINSTANCE) ::GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
											GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
											(LPCSTR) LibInitialize,
											(HMODULE *) &hInstance);

		InitCustomControls( hInstance);
		InitCommonControls();

		i3MemoryInit();
		i3Error::Init();

		i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK);
		i3Error::SetChannelName( I3LOG_NOTICE, "i3Export2.log");
		i3Error::SetChannelName( I3LOG_FATAL, "i3Export2.log");
		i3Error::SetChannelName( I3LOG_WARN, "i3Export2.log");

		{
			i3SceneRegisterMetas();

		/*	
			i3Texture::SetConcreteClass( i3TextureMem::static_meta());
			i3TextureCube::SetConcreteClass( i3TextureCubeMem::static_meta());
			i3VertexArray::SetConcreteClass( i3VertexArrayMem::static_meta());
			i3IndexArray::SetConcreteClass( i3IndexArrayMem::static_meta());
		*/
			i3TextureMem::static_meta()->set_class_meta_style_concrete(i3Texture::static_meta());
			i3TextureCubeMem::static_meta()->set_class_meta_style_concrete(i3TextureCube::static_meta());
			i3VertexArrayMem::static_meta()->set_class_meta_style_concrete(i3VertexArray::static_meta());
			i3IndexArrayMem::static_meta()->set_class_meta_style_concrete(i3IndexArray::static_meta());
		}
	}

#ifdef BUG_TRAP
	SetupExceptionHandler();
	BT_SetTerminate();
#endif

	return TRUE;
}

I3_EXPORT_EXP int LibShutdown(void)
{
	i3System::HoldDllModule((HMODULE)hInstance);
	return TRUE;
}

I3_EXPORT_EXP const TCHAR* LibDescription(void) 
{	
	return GetString(IDS_LIBDESC);
}

I3_EXPORT_EXP int LibNumberClasses(void)
{
	/// MUST CHANGE THIS NUMBER WHEN ADD NEW CLASS 
	return 2;
}


I3_EXPORT_EXP ClassDesc * LibClassDesc(int i) 
{
	switch(i) 
	{
		case 0: 
			return GetExportedClassDesc();
			break;

		case 1 :
			return GetImageClassDesc();
			break;

		default: 
			return 0;
	}

	assert(0); // NOT REACHED
}

I3_EXPORT_EXP ULONG LibVersion(void) 
{
	return VERSION_3DSMAX;
}

I3_EXPORT_EXP ULONG CanAutoDefer(void)
{
	return 0;
}