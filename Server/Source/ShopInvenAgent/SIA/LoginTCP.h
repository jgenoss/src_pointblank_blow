#ifndef __LOGIN_TCP_H__
#define __LOGIN_TCP_H__

#include "LoginBase.h"

class CLoginTCP : public CLoginBase
{
protected:
	i3NetworkClientSocket2*	m_pSocket;

	UINT32					m_ui32IP;
	UINT16					m_ui16Port;

	UINT32					m_ui32ReconnectTime;
public:
	CLoginTCP();
	virtual~CLoginTCP();

	virtual BOOL			OnCreate(INT32 i32idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx);
	virtual void			OnDestroy(void);
	
	virtual BOOL			OnRunningEX();

	virtual INT32			SendMessage(char* pPacket, INT32 i32Size);
	virtual INT32			RecvMessage(char* pPacket, INT32 i32Size);
};

#endif