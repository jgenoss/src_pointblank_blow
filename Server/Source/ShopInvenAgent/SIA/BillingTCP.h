#ifndef __BILLING_TCP_H__
#define __BILLING_TCP_H__

#include "BillingBase.h"

class CBillingTCP	: public CBillingBase
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	i3NetworkClientSocket2*	m_pSocket;

	UINT32					m_ui32ReconnectTime;

public:
	CBillingTCP();
	virtual ~CBillingTCP();

	virtual BOOL			OnCreate( INT32 i32Idx, UINT8 ui8MethodIdx );
	virtual void			OnDestroy(void); 

	virtual BOOL			OnRunning();

	virtual INT32			SendMessage( char* pPacket, INT32 i32Size );
	virtual INT32			RecvMessage( char* pPacket, INT32 i32Size );
};

#endif