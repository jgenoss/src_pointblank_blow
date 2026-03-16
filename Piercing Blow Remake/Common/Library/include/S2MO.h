#ifndef __S2MO__
#define __S2MO__

#define S2MO_VER_MAJOR			0
#define S2MO_VER_MINOR			5

#ifdef S2MO_EXPORTS
	#define	S2MO_EXPORT_BASE	__declspec( dllexport)
#else
	#define	S2MO_EXPORT_BASE	__declspec( dllimport)
#endif

typedef		INT16		T_PACKET_SIZE;

#define NET_MD5_KEY_SIZE			32			// MD5 KEY SIZE
#define NET_MD5_HASH_SIZE			16			// MD5 HASH SIZE. byte

#include "ProtocolDef.h"
#include "ErrorCode.h"
#include "S2MOStruct.h"
#include "S2MOPacketClass.h"
#include "S2MODatFile.h"

BOOL 	S2MO_EXPORT_BASE	S2MOCreate();
void 	S2MO_EXPORT_BASE	S2MODestory();

BOOL 	S2MO_EXPORT_BASE	S2MOMakePacket( S2MOPacketBase* pPacket, char* pBuffer );
BOOL 	S2MO_EXPORT_BASE	S2MORecvPacket( S2MOPacketBase* pPacket, char* pBuffer );

void	S2MO_EXPORT_BASE	S2MOSetPacketRandSeed( INT16 i16RandSeed );
INT16	S2MO_EXPORT_BASE	S2MOGetPacketRandSeed();

BOOL 	S2MO_EXPORT_BASE	S2MOCreatePairKey();
BOOL 	S2MO_EXPORT_BASE	S2MOWritePublicKey( char* pBuffer, int& i32Size );
BOOL 	S2MO_EXPORT_BASE	S2MOReadPublicKey( char* pBuffer, int& i32Size );

BOOL 	S2MO_EXPORT_BASE	S2MOPacketEncrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize );
BOOL 	S2MO_EXPORT_BASE	S2MOPacketDecrypt( void* pOut, INT32& i32OutSize, void* pIn, INT32 i32InSize );

#endif