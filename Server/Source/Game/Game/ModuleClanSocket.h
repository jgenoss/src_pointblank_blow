#ifndef	__MODULE_SOCKET_CLAN_H__
#define __MODULE_SOCKET_CLAN_H__

class CModuleClanSocket : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CModuleClanSocket ); 
private: 	
	BOOL	m_bConnect; 	
	char	m_strRoomNotice[NOTICE_STRING_MAX_COUNT]; 

	//RecvBuffer에서 임시로 사용합니다. 
	CLAN_MEMBER_BASIC_INFO		m_aTempMemberBasicInfo[200];		//맥스로 200개를 잡음: 임시용 버퍼 메모리 생성을 하지 않기 위해서 만듬
	CLAN_REQUEST_BASIC_INFO		m_aTempRequestBasicInfo[200];		//맥스로 200개를 잡음: 임시용 버퍼 메모리 생성을 하지 않기 위해서 만듬

protected: 

public : 
	BOOL			m_bNewClanCreate; 
	UINT32			m_ui32TempUserItemCharaCount;
	UINT32			m_ui32TempUserItemWeaponCount;

	UINT32			m_ui32LastSendTime;
	UINT32			m_ui32LastRecvTime;

public : 
	CModuleClanSocket(void); 
	virtual ~CModuleClanSocket(void); 
	virtual void PacketParsing(i3NetworkPacket  * pPacket); 
	virtual BOOL OnCreate( UINT32 ui32IP, UINT16 ui16PortNo);
	virtual void OnDestroy(void); 

	virtual void DisConnect();
		
	UINT32	GetLastRecvTime(void)	{ return m_ui32LastRecvTime; }

	BOOL	IsActive(void)		{ return m_bConnect; }
	BOOL	IsConnect(void)	
	{
		if(GetSocket() == INVALID_SOCKET)return FALSE;
		return TRUE; 
	}	
};

#endif