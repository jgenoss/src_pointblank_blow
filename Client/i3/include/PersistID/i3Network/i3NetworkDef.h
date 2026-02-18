#ifndef _I3_NETWORKDEF_H
#define _I3_NETWORKDEF_H

#include "i3Base.h"

typedef		UINT16	PROTOCOL;

//////////////////////////////////////////////////////////////////////////
//Define 
#define		PACKETBUFFERSIZE			8912
#define		I3ACCEPTOR_RECV_TIME		5
#define		PACKETHEADERSIZE			4

//////////////////////////////////////////////////////////////////////////
// Unknown addr
#ifndef		INADDR_ANY
#define		INADDR_ANY				0x00000000
#endif

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3_NETWORK_BUILD )
		#define I3_EXPORT_NETWORK		__declspec( dllexport)
		#define I3_EXTERN_NETWORK		extern
	#else
		#define I3_EXPORT_NETWORK		__declspec( dllimport)
		#define I3_EXTERN_NETWORK		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_NETWORK )
		#define	I3_EXPORT_NETWORK
		#define I3_EXTERN_NETWORK		extern
	#endif
#endif

#ifndef TPACKETHEADER_LEN
	#define	TPACKETHEADER_LEN			12		// 패킷 해더의 길이
#endif

//Set Socket Structure 
#if defined( I3_WINDOWS) || defined( I3_XBOX)
	#include <wininet.h>
	#include <Winsock2.h>
	typedef struct sockaddr_in I3NETSOCKETADDR;
#endif

#endif // _I3_NETWORKDEF_H
