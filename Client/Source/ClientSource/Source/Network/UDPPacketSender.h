#ifndef __UDPPACKETSENDER_H__
#define __UDPPACKETSENDER_H__

#include "Network/ClientP2PDef.h"

#define UDP_SEND_PACKET_SIZE		2048

class UDPPacketSender : public i3::shared_ginst<UDPPacketSender>
{
protected:	
	// Send Packet buffer
	char	m_SendBuffer[ UDP_SEND_PACKET_SIZE ];

	REAL32	m_rSendTime;
	REAL32	m_DediSendTime;	// Dedicated Mode에서 캐릭터 이외의 정보를 보내는 경우 체크

private:
	UINT16	__DedicatedService( char * pOutBuffer, UINT32 ui32BufSize, GAMEINFO_THROW_WEAPON * pThrowInfo, GAMEINFO_DROPED_WEAPON * pDropedInfo);

protected:	
	bool	_SendToMainClient( UINT32 nSendSize );

public:
	UDPPacketSender();

	bool	Create(void);
	void	Destroy( void );
	void	WriteMyGameInfo( const GAMEINFO_CHARACTER * pGameInfo, const GAMEINFO_THROW_WEAPON * pThrowInfo,
		const GAMEINFO_DROPED_WEAPON * pDropedInfo, REAL32 tm, UINT8 iRoundNum );

	void	SendDedication_Ping(REAL32 rClientTime); 

};

#endif	// __UDPPACKETSENDER_H__
