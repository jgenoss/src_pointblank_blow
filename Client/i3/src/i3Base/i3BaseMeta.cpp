#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3NamedElement.h"
#include "i3PersistantElement.h"
#include "i3Stream.h"
#include "i3StreamMem.h"
#include "i3StreamNull.h"
#include "i3StreamFile.h"
#include "i3StreamFileAsync.h"
#include "i3IniParser.h"
#include "i3Lexer.h"
#include "i3Thread.h"
#include "i3Timer.h"
#include "i3ObjectPool.h"
#include "i3MemoryBuffer.h"
#include "i3EventReceiver.h"
#include "i3Macro.h"
#include "i3StringTable.h"
#include "i3BaseUtil.h"
#include "i3Mutex.h"
#include "i3Signal.h"
#include "i3ResourceObject.h"
#include "i3TreeNode.h"
#include "i3RealTimeProfile.h"
#include "i3Action.h"
#include "i3TimeEventGen.h"
#include "i3NamedResourceManager.h"
#include "i3InstanceProxy.h"
#include "i3StackMemoryPool.h"
#include "i3PackNode.h"
#include "i3PackVolume.h"
#include "i3PackFileSystem.h"
#include "i3PointerStack.h"
#include "i3LRUQueue.h"
#include "i3AnyData.h"
#include "i3System.h"

#include "tls/detail/tls_mgr.h"

static bool		s_bBaseMetaRegistered = false;

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
I3_EXPORT_BASE HINSTANCE g_hInstBase;

#endif



I3_EXPORT_BASE void i3BaseRegisterMetas(void)
{
	if( s_bBaseMetaRegistered == false)
	{
		i3MemoryInit();
		s_bBaseMetaRegistered = true;
	}
}

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
	case DLL_PROCESS_ATTACH :	//i3BaseRegisterMetas();
		i3System::HoldDllModule((HMODULE)hInst);
		g_hInstBase = (HINSTANCE) hInst;
		i3::detail_tls::g_TlsManager.OnProcessAttach();
		break;
	case DLL_THREAD_ATTACH:
		i3::detail_tls::g_TlsManager.OnThreadAttach();
		break;
	case DLL_THREAD_DETACH:
		i3::detail_tls::g_TlsManager.OnThreadDetach();	
		break;
	case DLL_PROCESS_DETACH:
		i3::detail_tls::g_TlsManager.OnProcessDetach();
		default:
		break;
	}

	return true;
}

#endif

