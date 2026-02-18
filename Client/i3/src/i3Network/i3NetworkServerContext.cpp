#include "i3NetworkDef.h"
#include "i3NetworkServerContext.h"
#include "i3NetworkAcceptor.h" 
#include "i3NetworkCompletionHandler.h"
#include "i3NetworkSessionManager.h"

I3_CLASS_INSTANCE(i3NetworkServerContext);

i3NetworkServerContext::~i3NetworkServerContext()
{
	OnDestroy(); 
}

bool i3NetworkServerContext::OnCreate(UINT8 SocketCount, UINT32 * pAddress, UINT16 * pPort, UINT8 * pTimeOut, INT32 WorkCount, i3NetworkSessionManager * pSessionManager)
{
	//// Session Manager
	if(pSessionManager == nullptr)
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error SessionManager Is Null \n"); 
		return false;
	}
	m_pSessionManager = pSessionManager;

	//// Create Iocp
	m_pCompletionHandler = i3NetworkCompletionHandler::new_object();
	if( !m_pCompletionHandler->OnCreate( WorkCount, m_pSessionManager) )
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error IOCP Create Fail \n"); 
		return false;
	}
	

	//// Create Acceptor
	m_pAcceptor = i3NetworkAcceptor::new_object(); 
	if( !m_pAcceptor->OnCreate(SocketCount, pAddress, pPort, pTimeOut, m_pSessionManager, m_pCompletionHandler))
	{
		I3TRACE("[i3NetworkServerContext::OnCreate]Error Acceptor Create Fail \n"); 
		return false;
	}

	return true;
}

bool i3NetworkServerContext::OnDestroy(void)
{
	I3_SAFE_RELEASE( m_pAcceptor ); 	
	I3_SAFE_RELEASE( m_pCompletionHandler );	
	m_pSessionManager = nullptr;
	return true;
}

INT32 i3NetworkServerContext::GetSessionCount(void)
{
	return m_pSessionManager->GetSessionCount(); 
}
