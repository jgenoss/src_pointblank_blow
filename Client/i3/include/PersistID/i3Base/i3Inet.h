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
#else
	typedef INT32 I3SOCKETADDR;		//아직 미정입니다. 
#endif

namespace i3Net
{
	I3_EXPORT_BASE BOOL Init(void);				//이거 이름이 이상해서 사용하지 말아주세요.	추후 삭제하겠습니다.Create를 사용해 주세요. 

	I3_EXPORT_BASE BOOL Create(void);
	I3_EXPORT_BASE void Destroy(void);			

	I3_EXPORT_BASE BOOL GetMyAddress( char * pAdress );
	I3_EXPORT_BASE void GetName( INT32 Socket, SOCKADDR_IN * plocaladdr ); 
	//I3_EXPORT_BASE INT32 GetMACAddress(char* pMAC,INT32 nNICCount = 1);
	I3_EXPORT_BASE INT32 GetMACAddress(UINT64 * pMAC,INT32 nNICCount = 1);
	I3_EXPORT_BASE INT32 GetError( char * pString );

#if defined( I3_DEBUG)
	I3_EXPORT_BASE void DumpMACAddress(void);
#endif
}; 


class I3_EXPORT_BASE  i3Udp : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Udp );
protected: 
	INT32	m_Socket;

public:
	i3Udp(); 
	virtual ~i3Udp(); 

	INT32	CreateSocket(void); 
	void	DeleteSocket(void);

	void	BindAny(UINT16 Port);
	void	Bind(UINT32 Ip, UINT16 Port); 
	void	Send(const char * pBuffer, INT32 Byte, UINT32 Ip, UINT16 Port, INT32 flag = 0);
	INT32	Recv(char * pBuffer, INT32 Byte, INT32 flags = 0, I3SOCKETADDR * pFrom = NULL, INT32 * pFromlen = NULL); 

	INT32	GetOption( INT32 nlevel, INT32 nOption, char * pOut, INT32 * pReturnSize );
	INT32	SetOption( INT32 nlevel, INT32 nOption, const char * pszValue, INT32 nSize );
	void	GetName( I3SOCKETADDR * pLocal );

	INT32	getSocketHandle( void )				{ return m_Socket; }
}; 

class I3_EXPORT_BASE  i3Tcp : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Tcp );
protected: 
	INT32	m_Socket;

	INT32	CreateSocket(void); 
	void	DeleteSocket(void);
public:
	i3Tcp(); 
	virtual ~i3Tcp(); 
	BOOL	Connect(const char * pAddress, UINT16 PortNo); 
	BOOL	Connect(UINT32 Address, UINT16 PortNo); 
	void	Send(const char * pBuffer, INT32 Byte, INT32 flag = 0);
	INT32	Recv(char * pBuffer, INT32 Byte, INT32 flags = 0);

	void	GetName( I3SOCKETADDR * pLocal );
}; 


#endif
