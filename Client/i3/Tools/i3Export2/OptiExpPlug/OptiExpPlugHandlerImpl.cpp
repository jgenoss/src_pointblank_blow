
#include "stdafx.h"
#include "i3TDKAgent.h"
#include "OptiExpDlg.h"
#include "IOptiExpSettings.h"

#include "OptiExpPlugHandlerImpl.h"

static IOptiExpPlugHandler* gs_handler = NULL;


OptiExpPlugHandlerImpl::OptiExpPlugHandlerImpl(OptiExpPlugCallBack* callBack)
{
	m_callBack = callBack;
	m_expSettings = NULL;
}

OptiExpPlugHandlerImpl::~OptiExpPlugHandlerImpl()
{
	gs_handler = NULL;
}

bool OptiExpPlugHandlerImpl::Init()
{
	IOptiExpSettings::CreateExpSettings();
	return true;
}

void OptiExpPlugHandlerImpl::Finalize()
{
	IOptiExpSettings::DestroyExpSettings();
}

void OptiExpPlugHandlerImpl::Destroy()
{
	Finalize();
	delete this;
}

bool OptiExpPlugHandlerImpl::ShowExportDlg(HWND hParent)
{
	if(!DialogBoxParam(OptiExpPlug_HInstance, MAKEINTRESOURCE(IDD_DLG_OPTIEXP),
		hParent, _OptiExpDialogProc, (LPARAM)this))
	{
		return false;
	}

	return true;
}


//
// OptiExpPlug Entry
//

static bool gs_initialized = false;
I3OPTIEXPPLUG_API HINSTANCE OptiExpPlug_HInstance = NULL;

I3OPTIEXPPLUG_API HINSTANCE OptiExpPlug_GetHInstance() 
{
	return OptiExpPlug_HInstance; 
}

static bool InitI3EngineStuff()
{
	// dx render context
	InitCommonControls();

	//i3MemoryInit();
	//i3Error::Init();

	//i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	//i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG);
	//i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG | I3ERR_CHN_CALLBACK);
	//i3Error::SetChannelName( I3LOG_NOTICE, "i3OptiExpPlug.log");
	//i3Error::SetChannelName( I3LOG_FATAL, "i3OptiExpPlug.log");
	//i3Error::SetChannelName( I3LOG_WARN, "i3OptiExpPlug.log");

	//i3SceneRegisterMetas();
	
	i3TDK_Init(OptiExpPlug_HInstance); // init mfc

	return true;
}

static void FinalizeI3EngineStuff()
{
	i3TDK_Finalize();
}


I3OPTIEXPPLUG_API IOptiExpPlugHandler* OptiExpPlug_Init(OptiExpPlugCallBack* callBack)
{
	if(gs_initialized)
		return gs_handler;

	if(!callBack)
		return NULL;

	::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | 
		GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPCSTR) OptiExpPlug_Init, (HMODULE*)&OptiExpPlug_HInstance);

	I3ASSERT(OptiExpPlug_HInstance);
	if(!OptiExpPlug_HInstance)
	{
		OptiExpPlug_SetLastErr(900);
		return NULL;
	}
	
	InitI3EngineStuff();

	OptiExpPlugHandlerImpl* handler = new OptiExpPlugHandlerImpl(callBack);
	if(!handler->Init())
	{
		handler->Destroy();
		handler = NULL;

		OptiExpPlug_SetLastErr(100);

		gs_initialized = true; // OptiExpPlug_Finalize() 실행을위해
		OptiExpPlug_Finalize();

		return NULL;
	}

	gs_handler = handler;
	gs_initialized = true;

	return gs_handler;
}

I3OPTIEXPPLUG_API void OptiExpPlug_Finalize()
{
	if(!gs_initialized)
		return;

	if(gs_handler)
	{
		gs_handler->Destroy();
		gs_handler = NULL;
	}

	FinalizeI3EngineStuff();
	gs_initialized = false;
}

I3OPTIEXPPLUG_API IOptiExpPlugHandler* OptiExpPlug_GetHdr()
{
	return gs_handler;
}
