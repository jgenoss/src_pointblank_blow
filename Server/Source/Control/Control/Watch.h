#ifndef _WATCH_H__
#define _WATCH_H__

#include "WatchUDPSocket.h"


class CWatch :	public i3ElementBase
{
	I3_CLASS_DEFINE( CWatch );

private :	
	CWatchUDPSocket*		m_pUDPClientSocket;		// UDPSocket
	BOOL					m_bUseInterNet;			// 인터넷 연결 유무

public:	


	//////////////////////////////////////////////////////////////////////////

private:


public:
	CWatch(void);
	~CWatch(void);

	BOOL	Create(void);
	BOOL	Destroy(void);

	void	DestroyUDPSocket( void);
	void	CheckServerIPUDPSocket( UINT32 ui32IP );

	//** xxx.xxx.xxx.xxx형식의 IP 인지 확인 합니다.
	bool				_IsDottedAddressIP( const char* pAddr ) const;
	//** 도메인 네임을 IPstr로 변환 합니다. 
	const char*			_ConvertDomainName2IP( const char* pAddr );

};

extern CWatch * g_pWatch;

#endif //_WATCH_H__