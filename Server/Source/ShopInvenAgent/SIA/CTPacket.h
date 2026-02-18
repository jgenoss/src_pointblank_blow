#pragma once

#include <time.h>
#include <windows.h>
//************************************************************************
//  CLASS:      - CTPacket
//  Date:       - 20 Jul, 2004
//	COMMENTS:	- Network transfer Packet class
//	Usage:		- Registed Interface for easy Data I/O
//
//			|  |  |  |  |  |  |  |
//          Size(2)ID(2) Data(x)		* size = 2 + x
//************************************************************************

#define DF_DEFAULT_BUFFER_SIZE		8192		// 기본 버퍼 사이즈
#define DF_PACKET_HEADER_SIZE		4			// 패킷의 헤더 사이즈
#define DF_MAX_STRING_SIZE			8192		// String형의 최대 크기 - 넘으면 무시함

#define	NP_ALIVE					( 1 )
#define	NP_SPEEDHACK_REQ			( 2 )
#define	NP_MACADDRESS_REQ			( 3 )
#define	NP_FILETRANMIT_REQ			( 4 )
#define	NP_UDPINFORMATION_REQ		( 5 )
#define	NP_UDPINFORMATION_ACK		( 6 )
#define	NP_SERVERINFO_REQ			( 7 )
#define	NP_SERVERINFO_ACK			( 8 )

#define DF_PACKET_ADD_SIZE			0

#define SAFE_ALLOC(n)			( void *)GlobalAlloc( GPTR, n )
#define SAFE_FREE(p)			{ if ( p ) { GlobalFree ( ( HGLOBAL )p ); ( p ) = NULL; } }
#define SAFE_RELEASE(p)			{ if ( p ) { p->Release();	p = NULL; } }
#define SAFE_DELETE(p)			{ if ( p ) { delete (p);    (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)	{ if ( p ) { delete []p;	p = NULL; } }

class CTPacket
{
protected:
	typedef struct _PACKET_HEADER
	{
		WORD		*m_nSize;			// Packet Size
		WORD		*m_nID;				// Packet ID
	} PACKET_HEADER;

private:
	void		Initialize( WORD nSize = 0 );
	void		Release( void );			

public:
	void		InitializeBufferPosition()		{ m_pCurrentPointOfBuffer = &m_pBuffer[ DF_PACKET_HEADER_SIZE ]; }

public:
	CTPacket	&ID( WORD nID );
	void		Encrypt( DWORD dwEncryptVariable = 0 );
	void		Decrypt( DWORD dwDecryptVariable = 0 );
	void		SendType( WORD nid = 0xFFFF );
	void		RecvType();

	void		ClearAndReAlloc( WORD nId = 0xFFFF, DWORD dwBufferSize = DF_DEFAULT_BUFFER_SIZE );

public:			// For I/O Function
	bool		ReadData( void *pData, WORD nSize );
	void		WriteData( void *pData, WORD nSize );

	bool		ReadStream( void *pData, WORD &nSize );
	void		WriteStream( void *pData, WORD &nSize);

public:			// For I/O Interface
	WORD		GetID();
	WORD		GetSize();
	WORD		GetBufferSize()						{ return (m_nBufferSize); }
	CTPacket	&ReplaceID( WORD nID ) ;

	const char	*GetBuffer()						{ return ( m_pBuffer ); }
	void		SetBuffer( const char *pBuffer )	{ m_pBuffer = ( char * )pBuffer; }

	void		SetBuffer( const char *pBuffer, WORD nSize );

	INT			GetReadedSize( VOID ) { return INT( m_pCurrentPointOfBuffer - m_pBuffer - DF_PACKET_HEADER_SIZE );	} // K.Y.H 

public:			// For I/O Interface
	CTPacket&	operator << ( bool in );
	CTPacket&	operator << ( char cIn );
	CTPacket&	operator << ( BYTE nIn );
	CTPacket&	operator << ( short iIn );
	CTPacket&	operator << ( WORD nIn );
	CTPacket&	operator << ( int iIn );
	CTPacket&	operator << ( DWORD dwIn );
	CTPacket&	operator << ( float fIn );
	CTPacket&	operator << ( double fIn );
	CTPacket&	operator << ( __int64 i64In );
	CTPacket&	operator << ( char *strIn );
	CTPacket&	operator << ( const char *strIn );
	CTPacket&	operator << ( struct tm in );
	CTPacket&	operator << ( struct sockaddr_in *in );

	CTPacket&	operator >> ( bool &out );
	CTPacket&	operator >> ( char &cOut );
	CTPacket&	operator >> ( BYTE &nOut );
	CTPacket&	operator >> ( short &iOut );
	CTPacket&	operator >> ( WORD &nOut );
	CTPacket&	operator >> ( int &iOut );
	CTPacket&	operator >> ( DWORD &dwOut );
	CTPacket&	operator >> ( float &fOut );
	CTPacket&	operator >> ( double &fOut );
	CTPacket&	operator >> ( __int64 &i64Out );
	CTPacket&	operator >> ( char *strOut );
	CTPacket&	operator >> ( struct tm &out );
	CTPacket&	operator >> ( struct sockaddr_in &out );

public:
	CTPacket( WORD nId = 0xFFFF, DWORD dwBufferSize = DF_DEFAULT_BUFFER_SIZE );
	virtual ~CTPacket(void);

	CTPacket *	CloneDup();							// 메모리 생성하여 복사
	void		Clone( CTPacket *pPacket );			// 메모리 생성하지 않고 자신을 인자와 같이 복사

private:
	PACKET_HEADER		m_Header;
	char				*m_pBuffer;
	WORD				m_nBufferSize;
	char				*m_pCurrentPointOfBuffer;
	bool				m_bIsEncrypted;
	bool				m_bIsDecrypted;
};
