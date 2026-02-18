#include "pch.h"
#include "V3FireWall.h"

I3_CLASS_INSTANCE( CV3FireWall);//, i3Thread );

CV3FireWall::CV3FireWall() : m_MyIp(0)
							, m_State(0), m_CurrentIPIdx(0), m_IPSendCount(0)
{	
}

CV3FireWall::~CV3FireWall()
{

}

void CV3FireWall::Create( INT32 nThreadLevel )
{
	m_State = V3_FIREWALL_WAIT; 

	m_DummyUdpSocket.CreateSocket(); 
	m_DummyUdpSocket.BindAny(29890); 

	//i3Net::GetMyAddress((char*)&m_MyIp); 
	INT32 count = 0;
	char * pIPList = i3Net::GetMyAddressList( &count);

	if( pIPList != nullptr && count > 0)
	{
		m_IPList.resize( count);
		
		UINT32 IP;

		for(INT32 i = 0; i < count; i++)
		{
			IP = *(UINT32*)(&pIPList[ i*4]);
			m_IPList[i] = IP;
		}

		I3MEM_SAFE_FREE( pIPList);
	}

	m_CurrentIPIdx = 0;
	m_IPSendCount = 0;

	i3Thread::Create( "V3FireWall", 0, 4096, 0, nThreadLevel );
	return; 
}

void CV3FireWall::OnDestroy()
{
	m_DummyUdpSocket.DeleteSocket(); 
	
	i3Thread::Terminate();
}

void CV3FireWall::Send()
{
	char TempBuffer[128];

	if( m_IPList.size() == 0)
		return;

	UINT32 curIP = m_IPList[ m_CurrentIPIdx];
	m_DummyUdpSocket.Send(TempBuffer, 128, curIP, 29890);

	m_IPSendCount++;
	if( m_IPSendCount > 10 / (INT32)m_IPList.size() &&
		(INT32)m_IPList.size() > m_CurrentIPIdx+1)
	{
		m_CurrentIPIdx++;
		m_IPSendCount = 0;
	}

	//m_DummyUdpSocket.Send(TempBuffer, 128, m_MyIp, 29890);
}

UINT32 CV3FireWall::OnRunning( void * pUserData)
{
	char TempBuffer[128]; 

	while( m_State == V3_FIREWALL_WAIT )		
	{
		m_DummyUdpSocket.Recv(TempBuffer, 128); 
		m_State = V3_FIREWALL_OK; 
	}
	
	return 0;
}
