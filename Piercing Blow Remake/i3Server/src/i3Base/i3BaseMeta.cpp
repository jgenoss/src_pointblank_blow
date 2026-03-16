#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3NamedElement.h"
#include "i3PersistantElement.h"
#include "i3BinList.h"
#include "i3List.h"
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
#include "i3RefStack.h"
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

#include "i3NetworkClientSocket.h"
#include "i3NetworkClientSocket2.h"

#include "inst/detail/inst_memory_mgr.h"
#include "tls/detail/tls_mgr.h"

static BOOL		s_bBaseMetaRegistered = FALSE;

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
I3_EXPORT_BASE HINSTANCE g_hInstBase;
#endif

I3_EXPORT_BASE void i3BaseRegisterMetas(void)
{
	if( s_bBaseMetaRegistered == FALSE)
	{
		i3MemoryInit();

		i3ElementBase::RegisterMeta();
		i3PersistantElement::RegisterMeta();
		i3NamedElement::RegisterMeta();
		i3InstanceProxy::RegisterMeta();
		i3List::RegisterMeta();
		i3BinList::RegisterMeta();
		i3StringList::RegisterMeta();
		i3Stream::RegisterMeta();
		i3MemStream::RegisterMeta();
		i3FileStream::RegisterMeta();
		i3NullStream::RegisterMeta();
		i3AsyncFileStream::RegisterMeta();
		i3LineLexer::RegisterMeta();
		i3SimpleLineLexer::RegisterMeta();
		i3IniParser::RegisterMeta();
		i3Thread::RegisterMeta();
		i3ResourceFile::RegisterMeta();
		i3Timer::RegisterMeta();
		i3ObjectPool::RegisterMeta();
		i3MemoryBuffer::RegisterMeta();
		i3EventReceiver::RegisterMeta();
		i3StringTable::RegisterMeta();
		i3ToolProgressCtrl::RegisterMeta();
		i3Mutex::RegisterMeta(); 
		i3Signal::RegisterMeta();
		i3ResourceObject::RegisterMeta();
		i3TreeNode::RegisterMeta();
		i3RefStack::RegisterMeta();
		i3PackNode::RegisterMeta();
		i3PackVolume::RegisterMeta();
		i3PointerStack::RegisterMeta();

		#if !defined( I3_NO_PROFILE)
		i3RealTimeProfileNode::RegisterMeta();
		i3RealTimeProfileSample::RegisterMeta();
		#endif

		i3Action::RegisterMeta();
		i3TimeEventInfo::RegisterMeta();
		i3TimeEventGen::RegisterMeta();
		i3NamedResourceManager::RegisterMeta();
		i3StackMemoryPool::RegisterMeta();
		i3LRUQueue::RegisterMeta();

		i3NetworkClientSocket::RegisterMeta();
		i3NetworkClientSocket2::RegisterMeta();
		
		s_bBaseMetaRegistered = TRUE;
	}
}

#if (defined( I3_WINDOWS) || defined( I3_WINCE) ) && defined( I3_DLL )
BOOL WINAPI DllMain( HANDLE hInst, DWORD dwReason, LPVOID)
{
	switch( dwReason)
	{
		case DLL_PROCESS_ATTACH :	//i3BaseRegisterMetas();
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

	return TRUE;
}

#endif

