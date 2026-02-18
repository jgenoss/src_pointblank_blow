#ifndef _LOGINWORKER_LATIN_H
#define _LOGINWORKER_LATIN_H

#include "LoginTCP.h"
#include "CTPacket.h"

#define CLIENT_SOCKET_BUFFER_SIZE	(PACKETBUFFERSIZE * 3)
#define ENCRYPT_KEY					(12280411)

#define LATIN_PACKET_ID_LOGIN_REQUEST		(1001)
#define LATIN_PACKET_ID_LOGIN_ACK			(1002)
#define LATIN_PACKET_ID_LOGOUT_REQUEST		(1003)
#define LATIN_PACKET_ID_CASH_SYNC			(1009)
#define LATIN_PACKET_ID_LOGIN_REQUEST2		(1010)

#define GAME_CODE		("PBLANK")
#define	GAME_CODE_SIZE	(6)
#define	STR_ID_SIZE		(20)
#define	STR_IP_SIZE		(15)
#define DUMMY_STR_DATA	(50)

class CLoginWorker_Latin : public CLoginTCP
{
private:
	UINT32					m_ui32ConnectTime;

	INT32					m_i32Idx;
	UINT32					m_ui32BufferMaxCount;
	UINT8					m_ui8MethodIdx;

	i3RingBuffer *			m_pOutBuffer;

protected:
	virtual BOOL			_WorkProcess( LOGIN_INPUT * pLoginIn, LOGIN_OUTPUT* pLoginOut );
	virtual	bool			_WorkProcess(CTPacket *pData);
	
public:							
	CLoginWorker_Latin();
	virtual					~CLoginWorker_Latin();

	virtual INT32			RecvMessage(char* pPacket, INT32 i32Size);
	virtual BOOL			OnRunningEx();


	virtual BOOL			OnCreate(INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx);

	INT32					PopOutBufferCount(void) { if (m_pOutBuffer) return m_pOutBuffer->GetBufferCount(); return 0; }
	CTPacket *				PopOutBufferPointer(void) { if (m_pOutBuffer) return (CTPacket *)m_pOutBuffer->Pop(); return NULL; }
	void					PopOutBufferIdx(void) { if (!m_pOutBuffer) return; ((CTPacket *)(m_pOutBuffer->Pop()))->ClearAndReAlloc(65535, 0); m_pOutBuffer->PopIdx(); }

	void					PacketParsing(CTPacket* packet, INT32 i32Size );

};

#endif // #ifndef _LOGINWORKER_LATIN_H