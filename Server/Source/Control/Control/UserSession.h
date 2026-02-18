#ifndef __SESSION_H
#define __SESSION_H

#define IS_GAME		1
#define IS_BATTLE	2
class CSession : public i3NetworkClientSocket
{
	I3_CLASS_DEFINE( CSession ); 
private:	
	
protected: 	
	wchar_t					m_strId[NET_ID_SIZE];
	wchar_t					m_strPass[NET_PW_SIZE];
public:
	INT32					m_i32ConnectType;
	INT32					m_i32ServerType;
	INT32					m_i32ServerIdx;

	UINT32					m_ui32IsKillST;
	UINT32					m_ui32LastRecvST;

	//관제툴 일때 
	INT8					m_i8Auth_Service;

	// 최근 업데이트 된 MD5
	UINT32					m_ui32LastMD5;

	UINT8					m_ui8RSItemListCount;

	BOOL					m_bIsNormalDisconnect;	// 정상 종료 여부

protected:
	BOOL					_CheckAdminAuth();
	void					_KillUser();

	void					_PacketConfirmation( i3NetworkPacket* pPacket );

	void					_SendMapDataPacket();
	void					_SendRandomMapDataPacket();
	void					_SendBannerURLPacket();
public:
	INT32					m_i32Idx;
	UINT32					m_ui32ConIp;
	char					m_strIpString[SERVER_IP_SIZE];
	wchar_t					m_wstrIpString[SERVER_IP_SIZE];

	CSession();
	virtual ~CSession();

	void					OnCreate(SOCKET Socket, UINT32 ui32ConIp, char * pstrIpString);
	virtual void			OnDestroy(void); 

	virtual void			DisConnect();


	virtual void			PacketParsing(i3NetworkPacket * pPacket);

	BOOL					GetIsActive(void)		
	{
		if( GetSocket() == INVALID_SOCKET ) return FALSE; 
		return TRUE; 
	}


};

#endif
