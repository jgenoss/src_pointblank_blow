
#pragma once

#include "Packet.h"

#define MAX_BUFFER_SIZE		30				//패킷을 저장할 공간 갯수

class CRingBuffer
{
protected:
	INT32		m_i32PushIndex;
	INT32		m_i32PopIndex;

	CPacket		m_packBuffer[MAX_BUFFER_SIZE];

public:
	CRingBuffer(void);
	~CRingBuffer(void);

	INT32		GetBufferCount(void);

	BOOL		IsEmpty(void);
	
	BOOL		PushData(const char * buffer, INT32 Size, INT32 index);
	CPacket*	PopData(void);
};