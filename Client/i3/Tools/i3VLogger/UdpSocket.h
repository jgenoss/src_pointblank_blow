#pragma once

#include "i3VLoggerFramework.h"

#define			BUFF_SZ		0xFFFF


// CUdpSocket command target

class CUdpSocket : public CAsyncSocket
{
protected:
	i3VLoggerFramework *	m_pFramework;
	char					m_Buff[ BUFF_SZ];
	INT32					m_Index;
	INT32					m_idxRead;

protected:
	INT32				_FindPattern( INT32 idxStart, char * pszPattern, INT32 idxMax);
	void				ParseQue(void);

	char *				_getReadData( INT32 sz)
	{
		char * pRv = m_Buff + m_idxRead;

		m_idxRead += sz;

		return pRv;
	}

public:
	CUdpSocket();
	virtual ~CUdpSocket();

	char *	getQueBuffer(void)				{ return &m_Buff[ m_Index]; }
	INT32	getQueSize(void)				{ return BUFF_SZ - m_Index - 1; }

	void	setFramework( i3VLoggerFramework * pFramework)		{ m_pFramework = pFramework; }
	virtual void OnReceive(int nErrorCode) override;
};


