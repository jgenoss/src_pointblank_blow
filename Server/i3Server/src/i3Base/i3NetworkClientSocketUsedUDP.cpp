#include "i3NetworkDef.h"
#include "i3NetworkClientSocket.h"
#include "i3NetworkClientSocketUsedUDP.h"
#include "i3NetworkPacket.h"

I3_CLASS_INSTANCE( i3NetworkClientSocketUsedUDP, i3NetworkClientSocket );

i3NetworkClientSocketUsedUDP::i3NetworkClientSocketUsedUDP()
{
	m_nAssignedSession		= 0xFFFFFFFF;

	m_nRealUDPIP			= 0;
	m_nRealUDPPort			= 0;

	// 사설 IP는 ANY_ADDR 이다..( default 0 )
	m_nLocalUDPPort			= 0;

	m_pNetInfo				= NULL;
}

i3NetworkClientSocketUsedUDP::~i3NetworkClientSocketUsedUDP()
{
}

BOOL i3NetworkClientSocketUsedUDP::OnCreate(char * Address, UINT16 PortNo, CallbackRoutine CallBack)
{
	return i3NetworkClientSocket::OnCreate( Address, PortNo, CallBack); 
}

BOOL i3NetworkClientSocketUsedUDP::OnDestroy(void)
{
	return i3NetworkClientSocket::OnDestroy(); 
}

