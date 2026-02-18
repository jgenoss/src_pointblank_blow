#if !defined( __I3_INET_H)
#define __I3_INET_H

#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#if defined( I3_WINDOWS ) || defined( I3_WINCE )
	#include <Winsock2.h>
	#include <ws2tcpip.h>
	#include <Iphlpapi.h>					// MAC adress 얻기
	typedef struct sockaddr I3SOCKETADDR;
	
	#define		NET_MTU_SIZE					1400	// (Maximum Segment Size) 1448 이지만 여유롭게..
#else
	typedef INT32 I3SOCKETADDR;		//아직 미정입니다. 
#endif

namespace i3Net
{
	I3_EXPORT_BASE BOOL Init(void);				//이거 이름이 이상해서 사용하지 말아주세요.	추후 삭제하겠습니다.Create를 사용해 주세요. 

	I3_EXPORT_BASE BOOL Create(void);
	I3_EXPORT_BASE void Destroy(void);			

	I3_EXPORT_BASE BOOL GetMyAddress( char * pAdress );

	I3_EXPORT_BASE void DestroySocket( SOCKET hSocket );

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// \fn	I3_EXPORT_BASE char * GetMyAddressList( void);
	///
	/// \brief	Gets my address list.
	///
	/// \author	swoongoo.jung
	/// \date	2011-12-26
	/// 	
	///
	/// \return	null if it fails, else my address list. 새로 생성된 4byte의 data 배열. 사용한 후에는 해제해주세요. (I3MEM_SAFE_FREE)
	////////////////////////////////////////////////////////////////////////////////////////////////////

	I3_EXPORT_BASE char*		GetMyAddressList( INT32 * pOutCount);
	I3_EXPORT_BASE void			GetName( INT32 Socket, SOCKADDR_IN * plocaladdr ); 
	//I3_EXPORT_BASE INT32		GetMACAddress(char* pMAC,INT32 nNICCount = 1);
	I3_EXPORT_BASE INT32		GetMACAddress(UINT64 * pMAC,INT32 nNICCount = 1);
	I3_EXPORT_BASE INT32		GetError( char * pString );
	I3_EXPORT_BASE char*		GetIPToLongA( UINT32 ui32IP, char* strIP );
	I3_EXPORT_BASE wchar_t*		GetIPToLongW( UINT32 ui32IP, wchar_t* wstrIP );
	I3_EXPORT_BASE char*		GetIPToSockA( sockaddr_in* pSockAddr, char* strIP );
	I3_EXPORT_BASE wchar_t*		GetIPToSockW( sockaddr_in* pSockAddr, wchar_t* wstrIP );

#if defined( I3_DEBUG)
	I3_EXPORT_BASE void DumpMACAddress(void);
#endif
}; 


class I3_EXPORT_BASE  i3Udp : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Udp );
protected: 
	SOCKET	m_Socket;

public:
	i3Udp(); 
	virtual ~i3Udp(); 

	SOCKET	CreateSocket(void); 
	void	DeleteSocket(void);

	void	BindAny(UINT16 Port);
	void	Bind(UINT32 Ip, UINT16 Port); 
	void	Send(const char * pBuffer, INT32 Byte, UINT32 Ip, UINT16 Port, INT32 flag = 0);
	INT32	Recv(char * pBuffer, INT32 Byte, INT32 flags = 0, I3SOCKETADDR * pFrom = NULL, INT32 * pFromlen = NULL); 

	INT32	GetOption( INT32 nlevel, INT32 nOption, char * pOut, INT32 * pReturnSize );
	INT32	SetOption( INT32 nlevel, INT32 nOption, const char * pszValue, INT32 nSize );
	void	GetName( I3SOCKETADDR * pLocal );

	SOCKET	getSocketHandle( void )				{ return m_Socket; }
}; 

class I3_EXPORT_BASE  i3Tcp : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Tcp );
protected: 
	SOCKET	m_Socket;

	SOCKET	CreateSocket(void); 
	void	DeleteSocket(void);
public:
	i3Tcp(); 
	virtual ~i3Tcp(); 
	BOOL	Connect(const char * pAddress, UINT16 PortNo); 
	BOOL	Connect(UINT32 Address, UINT16 PortNo); 
	INT32	Send(const char * pBuffer, INT32 Byte, INT32 flag = 0);
	INT32	Recv(char * pBuffer, INT32 Byte, INT32 flags = 0);

	void	GetName( I3SOCKETADDR * pLocal );
}; 


#endif
