
#ifndef _I3OPTIEXPPLUG_H_
#define _I3OPTIEXPPLUG_H_


//#if defined(I3OPTIEXPPLUG_EXPORTS)
//	#define I3OPTIEXPPLUG_API	// __declspec( dllexport)
//#else
//	#define I3OPTIEXPPLUG_API	// __declspec( dllimport)
//#endif


#define I3OPTIEXPPLUG_API

#include "../OptionInfo.h"


class I3OPTIEXPPLUG_API IOptiExpPlugHandler
{
public:
	virtual void Destroy() = 0;

	virtual bool ShowExportDlg(HWND hParent) = 0;
};

class I3OPTIEXPPLUG_API OptiExpPlugCallBack
{
public:
	virtual HINSTANCE OEPC_GetOwnerModuleInst() = 0;
	virtual void OEPC_ClearLog() = 0;

	virtual bool OEPC_DoExport(const char* path, const char* filename, 
		i3Node** rstSceneNode) = 0;

	virtual void* OEPC_GetExporter() = 0; // i3Export* g_export;
	virtual bool OEPC_DoExporterOptDialog() = 0;

	virtual void OEPC_GetExporterFilePathName(char* path, char* name) = 0;
	virtual void OEPC_SetExporterFilePathName(const char* path, const char* name) = 0;
	
	virtual i3tool::exporter::pack::OPTION_INFO* OEPC_GetExporterOpt() = 0;

	virtual void OEPC_Log(char* msg) = 0;

	virtual INT32 OEPC_SetPhysXSettingToNode(INT32 dynType, bool includeOnly=false,
		const char* includeNode=NULL, const char* excludeNode=NULL) = 0;

	virtual void OEPC_GetAniTimeRange(INT32& startTime, INT32& endTime) = 0;
	virtual void OEPC_SetAniTimeRange(INT32 startTime, INT32 endTime) = 0;

	virtual void OEPC_Changei3DxResourceClassMode(bool toMem) = 0;

	virtual bool OEPC_CheckDupplicatedName(void** rst) = 0;
	virtual bool OEPC_CheckCollsionMesh(DWORD logFlag=0) = 0;

	virtual void OEPC_SetLoggingToFile(bool logging, bool* prevStatus=NULL) = 0;
	virtual bool OEPC_GetPhysXCookingLibInfo(DWORD* ver=NULL) = 0;
};


I3OPTIEXPPLUG_API IOptiExpPlugHandler* OptiExpPlug_Init(OptiExpPlugCallBack* callBack);
I3OPTIEXPPLUG_API IOptiExpPlugHandler* OptiExpPlug_GetHdr();
I3OPTIEXPPLUG_API void OptiExpPlug_Finalize();

I3OPTIEXPPLUG_API INT32 OptiExpPlug_GetLastErr();
I3OPTIEXPPLUG_API void OptiExpPlug_SetLastErr(INT32 errVal);

I3OPTIEXPPLUG_API HINSTANCE OptiExpPlug_GetHInstance();
I3OPTIEXPPLUG_API extern HINSTANCE OptiExpPlug_HInstance;

I3OPTIEXPPLUG_API void OptiExpPlug_CenterWindow(HWND hWnd, HWND hWndParent);


#endif
