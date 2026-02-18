//#include "stdafx.h"
#include "pch.h"
#include "CTPacket.h"

CTPacket::CTPacket( WORD nId , DWORD dwBufferSize )
{
	m_nBufferSize = (WORD)dwBufferSize;
	m_bIsEncrypted = false;
	m_bIsDecrypted = false;

	if( m_nBufferSize )
	{
		m_pBuffer = new char[ m_nBufferSize + 10 ];
		ZeroMemory( m_pBuffer, m_nBufferSize + 10 );

		m_Header.m_nSize = (unsigned short int *)&m_pBuffer[0];
		m_Header.m_nID = (unsigned short *)&m_pBuffer[2];

		Initialize();

		if( nId != 0xFFFF ) *( m_Header.m_nID ) = nId;
		*( m_Header.m_nSize ) = DF_PACKET_HEADER_SIZE;
	}
	else
		m_pBuffer = NULL;
}

CTPacket::~CTPacket(void)
{
	Release();
}

CTPacket *	CTPacket::CloneDup()
{
	CTPacket * pPacket = new CTPacket(GetID(), 0);
	pPacket->SetBuffer(m_pBuffer);

	pPacket->m_Header.m_nSize = ( WORD * )&m_pBuffer[0];
	pPacket->m_Header.m_nID = ( WORD * )&m_pBuffer[2];
	m_pBuffer = NULL;

	pPacket->m_nBufferSize = GetSize();

	pPacket->InitializeBufferPosition();
	
	return pPacket;
}

void CTPacket::Clone( CTPacket *pPacket )
{
	SetBuffer( pPacket->m_pBuffer, pPacket->GetSize() );
	m_nBufferSize = pPacket->GetSize();

	m_Header.m_nSize = ( WORD * )&m_pBuffer[0];
	m_Header.m_nID = ( WORD * )&m_pBuffer[2];

	InitializeBufferPosition();
}

void CTPacket::Initialize( WORD nSize  )
{
	m_bIsEncrypted = false;
	m_bIsDecrypted = false;

	if( nSize > m_nBufferSize )
	{
		char *oldBuffer = m_pBuffer;
		int oldPacketSize = m_nBufferSize;

		m_nBufferSize = nSize;

		m_pBuffer = new char[ m_nBufferSize + 10 ];
		ZeroMemory( m_pBuffer, m_nBufferSize + 10 );

		i3mem::Copy( m_pBuffer, oldBuffer, oldPacketSize );
		SAFE_DELETE_ARRAY( oldBuffer );
	}

	m_Header.m_nSize = ( WORD * )&m_pBuffer[0];
	m_Header.m_nID = ( WORD * )&m_pBuffer[2];

	InitializeBufferPosition();
}

void CTPacket::ClearAndReAlloc( WORD nId , DWORD dwBufferSize )
{
	// 기존데이타 클리어
	Release();
	
	// 새롭게 할당
	m_nBufferSize = (WORD)dwBufferSize;
	m_bIsEncrypted = false;
	m_bIsDecrypted = false;

	if( m_nBufferSize )
	{
		m_pBuffer = new char[ m_nBufferSize + 10 ];
		ZeroMemory( m_pBuffer, m_nBufferSize + 10 );

		m_Header.m_nSize = (unsigned short int *)&m_pBuffer[0];
		m_Header.m_nID = (unsigned short *)&m_pBuffer[2];

		Initialize();

		if( nId != 0xFFFF ) *( m_Header.m_nID ) = nId;
		*( m_Header.m_nSize ) = DF_PACKET_HEADER_SIZE;
	}
	else
		m_pBuffer = NULL;
}

void CTPacket::Release()
{
	SAFE_DELETE_ARRAY( m_pBuffer );
	m_bIsEncrypted = false;
	m_bIsDecrypted = false;
}

CTPacket &CTPacket::ID( WORD nID )
{
	*( m_Header.m_nSize ) = DF_PACKET_HEADER_SIZE;
	*( m_Header.m_nID ) = nID;
	InitializeBufferPosition();
	
	return ( *this );
}

void CTPacket::Encrypt( DWORD dwEncryptVariable )
{
	if( m_bIsEncrypted )				return;

	m_bIsEncrypted = true;
	DWORD dwEncryptVar = dwEncryptVariable?dwEncryptVariable:GetSize();

	for( WORD i = 0; i < ( GetSize() - DF_PACKET_HEADER_SIZE ); ++i )
	{
		*( m_pBuffer + DF_PACKET_HEADER_SIZE + i ) ^= dwEncryptVar;
	}
}

void CTPacket::Decrypt( DWORD dwDecryptVariable )
{
	if( m_bIsDecrypted )				return;

	m_bIsDecrypted = true;

	DWORD dwDecryptVar = dwDecryptVariable?dwDecryptVariable:GetSize();

	for( WORD i = 0; i < ( GetSize() - DF_PACKET_HEADER_SIZE ); i++ )
	{
		*( m_pBuffer + DF_PACKET_HEADER_SIZE + i ) ^= dwDecryptVar;
	}
}

void CTPacket::SendType( WORD nId  )
{
	Initialize( m_nBufferSize );
	if( nId != 0xFFFF )	*( m_Header.m_nID ) = nId;
	*( m_Header.m_nSize ) = DF_PACKET_HEADER_SIZE;
}

void CTPacket::RecvType()
{
	m_Header.m_nSize = ( WORD* )&m_pBuffer[0];
	m_Header.m_nID = ( WORD* )&m_pBuffer[2];
}

bool CTPacket::ReadData( void *pData, WORD nSize )
{
	if ( m_pCurrentPointOfBuffer + nSize > m_pBuffer + GetSize() )
		return ( false );

	i3mem::Copy( pData, m_pCurrentPointOfBuffer, nSize );
	m_pCurrentPointOfBuffer += nSize;

	return ( true );
}

void CTPacket::WriteData( void *pData, WORD nSize )
{
	// TODO : 예외처리
	if( GetSize() + nSize >= m_nBufferSize )		return;

	if ( m_pCurrentPointOfBuffer + nSize >= m_pBuffer + m_nBufferSize )
	{
		WORD packetBufferSize = *( WORD *)( (WORD)( m_nBufferSize + nSize + DF_DEFAULT_BUFFER_SIZE ) );		// TODO : 

		Initialize( packetBufferSize );

		m_pCurrentPointOfBuffer += *( m_Header.m_nSize );
	}

	i3mem::Copy( m_pCurrentPointOfBuffer, pData, nSize );
	m_pCurrentPointOfBuffer += nSize;
	*( m_Header.m_nSize ) += ( unsigned short )nSize;
}

bool CTPacket::ReadStream( void *pData, WORD &nSize )
{
	if( !ReadData( &nSize, sizeof( WORD ) ) )		return ( false );

	return( ReadData( pData, nSize ) );
}

void CTPacket::WriteStream( void *pData, WORD &nSize )
{
	WriteData( &nSize, sizeof( WORD ) );
	WriteData( pData, nSize );
}

WORD CTPacket::GetID()
{
	if ( ( m_Header.m_nID ) == NULL ) return ( 0 );
	return ( WORD )*( m_Header.m_nID ) ;
}

WORD CTPacket::GetSize()
{
	if ( ( m_Header.m_nSize ) == NULL ) return ( 0 );
	return ( WORD )*( m_Header.m_nSize ) ;
}

CTPacket &CTPacket::ReplaceID( WORD nID )
{
	*( m_Header.m_nID ) = nID;
	return ( *this );
}

void CTPacket::SetBuffer( const char *pBuffer, WORD nSize )
{
	m_bIsEncrypted = false;
	m_bIsDecrypted = false;
	Initialize( nSize + 10 );
	i3mem::Copy( m_pBuffer, pBuffer, nSize );
}

CTPacket& CTPacket::operator << ( bool in )
{
	WriteData( &in, sizeof( bool ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( char cIn )
{
	WriteData( &cIn, sizeof( char ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( BYTE nIn )
{
	WriteData( &nIn, sizeof( BYTE ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( short iIn )
{
	WriteData( &iIn, sizeof( short ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( WORD nIn )
{
	WriteData( &nIn, sizeof( WORD ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( int iIn )
{
	WriteData( &iIn, sizeof( int ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( DWORD dwIn )
{
	WriteData( &dwIn, sizeof( DWORD ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( float fIn )
{
	WriteData( &fIn, sizeof( float ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( double fIn )
{
	WriteData( &fIn, sizeof( double ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( __int64 i64In )
{
	WriteData( &i64In, sizeof( __int64) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( char *strIn )
{
	if ( strIn == NULL ) return ( *this );
	if ( strlen ( strIn ) >= DF_MAX_STRING_SIZE )
	{
		return ( *this );
	}
	WriteData( (void *)strIn, ( WORD ) ( strlen(strIn) + 1 ) );
	return ( *this );
}

CTPacket& CTPacket::operator << ( const char *strIn )
{
	if ( strIn == NULL ) return ( *this );
	if ( strlen ( strIn ) >= DF_MAX_STRING_SIZE )
	{
		return ( *this );
	}
	WriteData( (void *)strIn, ( WORD ) ( strlen(strIn) + 1 ) );
	return ( *this );
}

//CTPacket& CTPacket::operator << ( time_t in )
//{
//	WriteData( ( void * )&in, sizeof( time_t ) );
//
//	return ( *this );
//}

CTPacket& CTPacket::operator << ( struct tm in )
{
	WriteData( ( void * )&in, sizeof( struct tm ) );

	return ( *this );
}

CTPacket& CTPacket::operator << ( struct sockaddr_in *in )
{
	*this << in->sin_addr.S_un.S_addr << in->sin_port;
	return ( *this );
}

CTPacket& CTPacket::operator >> ( bool &out )
{
	ReadData( &out, sizeof( bool ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( char &out )
{
	ReadData( &out, sizeof( char ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( BYTE &out )
{
	ReadData( &out, sizeof( BYTE ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( short &out )
{
	ReadData( &out, sizeof( short ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( WORD &out )
{
	ReadData( &out, sizeof( WORD ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( int &out )
{
	ReadData( &out, sizeof( int ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( DWORD &out )
{
	ReadData( &out, sizeof( DWORD ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( float &out )
{
	ReadData( &out, sizeof( float ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( double &out )
{
	ReadData( &out, sizeof( double ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( __int64 &out )
{
	ReadData( &out, sizeof( __int64 ) );

	return ( *this );
}

CTPacket& CTPacket::operator >> ( char *out )
{
	if ( out == NULL ) return ( *this );

	int length = (int)strlen( (const char *)m_pCurrentPointOfBuffer ) + 1;
	if ( length >= DF_MAX_STRING_SIZE )
	{
		m_pCurrentPointOfBuffer += length;
		return ( *this );
	}

	ReadData( out, ( WORD )( length ) );

	return ( *this );
}

//CTPacket& CTPacket::operator >> ( time_t &out )
//{
//	ReadData( &out, sizeof( time_t ) );
//	return ( *this );
//}

CTPacket& CTPacket::operator >> ( struct tm &out )
{
	ReadData( &out, sizeof( struct tm ) );
	return ( *this );
}

CTPacket& CTPacket::operator >> ( struct sockaddr_in &out )
{
	*this >> out.sin_addr.S_un.S_addr >> out.sin_port;
	return ( *this );
}