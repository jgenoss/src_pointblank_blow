#include "pch.h"
#include "S2MO.h"
#include "S2MOBase.h"

S2MOBase* g_pS2MOBase = NULL;

BOOL S2MOCreate()
{
	g_pS2MOBase = new S2MOBase;
	if( NULL == g_pS2MOBase )					return FALSE;
	if( FALSE == g_pS2MOBase->Create() )		return FALSE;

	return TRUE;
}

void S2MODestory()
{
	if( g_pS2MOBase )
	{
		delete g_pS2MOBase;
		g_pS2MOBase = NULL;
	}
}

BOOL S2MOMakePacket( S2MOPacketBase* pPacket, char* pBuffer )
{
	pPacket->Reset();
	if( FALSE == pPacket->Packing_T( pBuffer ) )			return FALSE;

	return TRUE;
}

BOOL S2MORecvPacket( S2MOPacketBase* pPacket, char* pBuffer )
{
	pPacket->Reset();
	if( FALSE == pPacket->UnPacking_T( pBuffer ) )			return FALSE;
	
	return TRUE;
}

void S2MOSetPacketRandSeed( INT16 i16RandSeed )
{
	g_pS2MOBase->SetPacketRandSeed( i16RandSeed );
}

INT16 S2MOGetPacketRandSeed()
{
	return g_pS2MOBase->GetPacketRandSeed();
}

BOOL S2MOCreatePairKey()
{
	return g_pS2MOBase->RSACreatePairKey();
}

BOOL S2MOWritePublicKey( char* pBuffer, INT32& i32Size )
{
	return g_pS2MOBase->RSAWritePublicKey( pBuffer, i32Size );
}

BOOL S2MOReadPublicKey( char* pBuffer, INT32& i32Size )
{
	return g_pS2MOBase->RSAReadPublicKey( pBuffer, i32Size );
}

BOOL S2MOPacketEncrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
{
	return g_pS2MOBase->RSAEncrypt( pOut, i32OutSize, pIn, i32InSize );
}

BOOL S2MOPacketDecrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize )
{
	return g_pS2MOBase->RSADecrypt( pOut, i32OutSize, pIn, i32InSize );
}