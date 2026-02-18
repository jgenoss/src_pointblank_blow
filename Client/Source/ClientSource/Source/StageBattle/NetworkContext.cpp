#include "pch.h"
#include "NetworkContext.h"

#include "../Network/GameNetworkManager.h"


void NetworkContext::RelayHolePunch( UINT32 wIP, UINT16 sPort, UINT32 wGroupId, UINT32 wKey )
{
	relayServerIp		= wIP;
	relayServerPort		= sPort;
	relayGroupId		= wGroupId;
	RelayEncryptKey		= wKey;
}

void NetworkContext::RendezvousHolePunch()
{
	//	m_pUDPHolePuncher->setServerUDPState( SERVER_UDP_STATE_RENDEZVOUS );
	//#ifdef JUST_RENDEZVOUS_SERVER
	//	m_RendezvousState = UDP_STATE_SENDRECV;
	//#else
	//
	//	switch( getMyNetInfo()->m_nNATState )
	//	{
	//	// Full ÀÏ¶§´Â Rendezvous ¿Í È¦ÆÝÄª ÇÊ¿ä°¡ ¾ø´Ù.
	//	// Symmetric ÀÏ¶§´Â È¦ÆÝÄªÀÌ ÀÇ¹Ì°¡ ¾ø´Ù.
	//	case NAT_STATE_FULL_CONE_NAT:
	//	case NAT_STATE_SYMMETRIC_NAT:
	//		m_RendezvousState = UDP_STATE_RECV_OK;
	//		break;
	//	case NAT_STATE_RESTRICTED_CONE_NAT:
	//	case NAT_STATE_PORT_RESTRICTED_CONE_NAT:
	//	case NAT_STATE_WITHOUT:
	//		m_RendezvousState = UDP_STATE_SENDRECV;
	//		break;
	//	}
	//#endif
}

void NetworkContext::InitP2P( void)
{
	for( UINT32 i = 0; i < USER_SLOT_COUNT; i++)
	{	
		NetInfo[i].m_nLocalIP		= 0;
		NetInfo[i].m_nLocalPort	= 0;
		NetInfo[i].m_nRealIP		= 0;
		NetInfo[i].m_nRealPort		= 0;
		
		UdpHoleState[i] = UDP_STATE_NONE;
		UdpHoleType[i] = UDP_TYPE_NONE;
	}
}

void NetworkContext::GiveupBattle( INT32 idx)
{
	I3_BOUNDCHK( idx, USER_SLOT_COUNT);

	// UDP Á¤º¸ ¸®¼Â
	NetInfo[idx].Reset();
	UdpHoleState[idx] = UDP_STATE_NONE;
	UdpHoleType[idx] = UDP_TYPE_NONE;
}

void NetworkContext::EndBattle( void)
{
	for( INT32 i = 0; i < USER_SLOT_COUNT; i++)
	{
		GiveupBattle( i);
	}
}

INT32 NetworkContext::GetSessionIndex( void )
{
	return CGameNetworkManager::i()->GetSessionIndex();
}

UINT16 NetworkContext::GetGamePort( void )
{
	return CGameNetworkManager::i()->GetGamePort();
}

void NetworkContext::SetGamePort( UINT16 ui16Port )
{
	CGameNetworkManager::i()->SetGamePort( ui16Port);
}

bool NetworkContext::GetIsRealIp( void )
{
	return CGameNetworkManager::i()->IsRealIP();
}

bool NetworkContext::isRealIp( void )
{
	return CGameNetworkManager::i()->IsRealIP();
}

const NET_UDP_INFO* NetworkContext::getMyNetInfo( void ) const 
{
	return CGameNetworkManager::i()->GetMyNetInfo();
}

NET_UDP_INFO* NetworkContext::setMyNetInfo( void )
{
	return CGameNetworkManager::i()->SetMyNetInfo();
}

void NetworkContext::SetEnteredServer( bool Entered )
{
	CGameNetworkManager::i()->SetEnteredServer( Entered);
}

bool NetworkContext::GetEnteredServer()
{
	return CGameNetworkManager::i()->GetEnteredServer();

	/*I3ASSERT(m_pTCPClientSocket != nullptr);
	if (m_pTCPClientSocket)
	return m_pTCPClientSocket->GetEnteredServer();
	return false; */
}

void NetworkContext::ChangeSocket(void)
{
	CGameNetworkManager::i()->ChangeSocket();
}
