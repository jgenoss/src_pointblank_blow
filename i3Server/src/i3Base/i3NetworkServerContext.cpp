#include "i3CommonType.h"
#include "i3NetworkServerContext.h"
#include "i3NetworkAcceptor.h" 
#include "i3NetworkCompletionHandler.h"
#include "i3NetworkSessionManager.h"

I3_CLASS_INSTANCE(i3NetworkServerContext, i3ElementBase); 

i3NetworkServerContext::i3NetworkServerContext()
{	
	m_pAcceptor				= NULL; 
	m_pCompletionHandler	= NULL; 
	m_pSessionManager		= NULL; 
}

i3NetworkServerContext::~i3NetworkServerContext()
{
	OnDestroy(); 
}

BOOL i3NetworkServerContext::OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager)
{
	//// Session Manager
	if(pSessionManager == NULL)
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error SessionManager Is Null \n"); 
		return FALSE; 
	}
	m_pSessionManager = pSessionManager;

	//// Create Iocp
	m_pCompletionHandler = i3NetworkCompletionHandler::NewObject();
	if( !m_pCompletionHandler->OnCreate( WorkCount, m_pSessionManager) )
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error IOCP Create Fail \n"); 
		return FALSE; 
	}
	

	//// Create Acceptor
	m_pAcceptor = i3NetworkAcceptor::NewObject(); 
	if( !m_pAcceptor->OnCreate(SocketCount, pAddress, pPort, pTimeOut, m_pSessionManager, m_pCompletionHandler))
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error Acceptor Create Fail \n"); 
		return FALSE; 
	}

	return TRUE; 
}

BOOL i3NetworkServerContext::OnDestroy(void)
{
	I3_SAFE_RELEASE( m_pAcceptor ); 	
	I3_SAFE_RELEASE( m_pCompletionHandler );	
	m_pSessionManager = NULL;
	return TRUE; 
}

INT32 i3NetworkServerContext::GetSessionCount(void)
{	
	return m_pSessionManager->GetSessionCount();
}

void i3NetworkServerContext::GetPacketInfo(UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize)
{
	m_pCompletionHandler->GetPacketInfo(pi64Count, pi64Size, pi64SendCount, pi64SendSize);
}

void i3NetworkServerContext::GetPacketInfoThread(INT32 i32ThreadIdx, UINT64* pi64Count, UINT64* pi64Size, UINT64* pi64SendCount, UINT64* pi64SendSize)
{
	m_pCompletionHandler->GetPacketInfoThread(i32ThreadIdx, pi64Count, pi64Size, pi64SendCount, pi64SendSize);
}
