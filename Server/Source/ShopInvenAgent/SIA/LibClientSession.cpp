#include "pch.h"
#include "LibClientSession.h"

#define WZ_BEGIN		INT32 i32Index = 0
#define WZ_WRITE(a, b)	i3mem::Copy( &Dat.m_Data[i32Index], &a, sizeof(b) );i32Index += sizeof(b)

#ifdef BUILDMODE_KOREA

CLibClientSession::CLibClientSession(void)
{
}

CLibClientSession::~CLibClientSession(void)
{
}

void CLibClientSession::OnNetConnect( bool bSuccess, DWORD dwError )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_NETCONNECT );
	WZ_BEGIN;
	WZ_WRITE( bSuccess,			bool );
	WZ_WRITE( dwError,			DWORD );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnNetSend( INT32 i32Size )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_NETSEND );
	WZ_BEGIN;
	WZ_WRITE( i32Size,			INT32 );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnNetClose( DWORD dwError )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_NETCLOSE );
	WZ_BEGIN;
	WZ_WRITE( dwError,			DWORD );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnUserStatus( DWORD	dwAccountGUID, long	dwBillingGUID, DWORD dwRealEndDate, DWORD dwEndDate, REAL64 r64RestPoint, REAL64 r64RestTime, INT16 i16DeductType, INT16 i16AccessCheck, INT16 i16ResultCode )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_USERSTATUS );
	WZ_BEGIN;
	WZ_WRITE( dwAccountGUID,	DWORD );
	WZ_WRITE( dwBillingGUID,	long );
	WZ_WRITE( dwRealEndDate,	DWORD );
	WZ_WRITE( dwEndDate,		DWORD );
	WZ_WRITE( r64RestPoint,		REAL64 );
	WZ_WRITE( r64RestTime,		REAL64 );
	WZ_WRITE( i16DeductType,	INT16 );
	WZ_WRITE( i16AccessCheck,	INT16 );
	WZ_WRITE( i16ResultCode,	INT16 );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnInquireMultiUser( long AccountID, long RoomGUID, long	Result )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_INQUIREMULTIUSER );
	WZ_BEGIN;
	WZ_WRITE( AccountID,		long );
	WZ_WRITE( RoomGUID,			long );
	WZ_WRITE( Result,			long );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnInquirePCRoomPoint( long AccountID, long	RoomGUID, long GameCode, long ResultCode )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_INQUIREPCROOMPOINT );
	WZ_BEGIN;
	WZ_WRITE( AccountID,		long );
	WZ_WRITE( RoomGUID,			long );
	WZ_WRITE( GameCode,			long );
	WZ_WRITE( ResultCode,		long );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnCheckLoginUser( long AccountID, long	BillingGUID )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_CHECKLOGINUSER );
	WZ_BEGIN;
	WZ_WRITE( AccountID,		long );
	WZ_WRITE( BillingGUID,		long );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnCheckPCRoomIPAddr( DWORD dwIPAddress, DWORD dwAccountGUID, DWORD dwRoomGUID, INT16 i16ResultCode, DWORD dwReserved1, DWORD dwReserved2, DWORD dwReserved3 )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_CHECKPCROOMIPADDR );
	WZ_BEGIN;
	WZ_WRITE( dwIPAddress,		DWORD );
	WZ_WRITE( dwAccountGUID,	DWORD );
	WZ_WRITE( dwRoomGUID,		DWORD );
	WZ_WRITE( i16ResultCode,	INT16 );
	WZ_WRITE( dwReserved1,		DWORD );
	WZ_WRITE( dwReserved2,		DWORD );
	WZ_WRITE( dwReserved3,		DWORD );
	m_pBuffer->Push( &Dat );
}

void CLibClientSession::OnPCRoomDeductUpdate( long RoomGUID, long GameCode )
{
	PCCAFE_CALLBACK_RESULT Dat( WEBZEN_CALLBACK_PCROOMDEDUCTUPDATE );
	WZ_BEGIN;
	WZ_WRITE( RoomGUID,			long );
	WZ_WRITE( GameCode,			long );
	m_pBuffer->Push( &Dat );
}

#endif