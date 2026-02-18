#ifndef __PCCAFE_TCP_H__
#define __PCCAFE_TCP_H__

#include "PCCafeBase.h"

class CPCCafeTCP	: public CPCCafeBase
{
	UINT32					m_ui32IP;
	UINT16					m_ui16Port;
	i3NetworkClientSocket2*	m_pSocket;

	UINT32					m_ui32ReconnectTime;

public:
	CPCCafeTCP();
	virtual ~CPCCafeTCP();

	virtual BOOL			OnCreate( INT32 i32Idx );
	virtual void			OnDestroy(void); 

	virtual BOOL			OnRunning();

	virtual INT32			SendMessage( char* pPacket, INT32 i32Size );
	virtual INT32			RecvMessage( char* pPacket, INT32 i32Size );
};

#endif