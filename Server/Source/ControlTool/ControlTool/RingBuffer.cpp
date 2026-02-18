
#include "pch.h"
#include "RingBuffer.h"

CRingBuffer::CRingBuffer(void)
{
	m_i32PushIndex		= 0;
	m_i32PopIndex		= 0;
	ZeroMemory(m_packBuffer, MAX_BUFFER_SIZE);
}





CRingBuffer::~CRingBuffer(void)
{
	ZeroMemory(m_packBuffer, MAX_BUFFER_SIZE);
}





INT32 CRingBuffer::GetBufferCount(void)
{
	INT32 i32Rv = 0;

	if(m_i32PopIndex > m_i32PushIndex)
	{
		if(m_i32PopIndex >= 10)
		{
			m_i32PopIndex = m_i32PushIndex;
			return 0;
		}
	}
	else
		i32Rv = m_i32PushIndex - m_i32PopIndex;

	if(i32Rv < 0)
		i32Rv += 10;

	return i32Rv;
}



BOOL CRingBuffer::IsEmpty(void)
{
	INT32 tempValue = m_i32PushIndex - m_i32PopIndex;

	if(tempValue != 0)
		return TRUE;
	else
		return FALSE;
}





BOOL CRingBuffer::PushData(const char * buffer, INT32 Size, INT32 index)
{
	m_packBuffer[m_i32PushIndex].CopyToBuffer(buffer, Size, index);
	
	m_i32PushIndex++;

	if(MAX_BUFFER_SIZE <= m_i32PushIndex)
		m_i32PushIndex = 0;

	return TRUE;
}




CPacket* CRingBuffer::PopData(void)
{
	CPacket* Rv = NULL;

	Rv = &(m_packBuffer[m_i32PopIndex]);
	
	if(Rv->GetReceivedSize() == 0)
		return NULL;

	m_i32PopIndex++;

	if(MAX_BUFFER_SIZE <= m_i32PopIndex)
		m_i32PopIndex = 0;

	return Rv;
}