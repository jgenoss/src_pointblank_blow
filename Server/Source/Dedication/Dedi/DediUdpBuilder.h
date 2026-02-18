#if !defined( __DediUdpBuilder_H )
#define __DediUdpBuilder_H

class CDediRoom;
class CUdpBuffer;
class CStageInfoTimer;

class UDPCSBuilder
{
public:
	// 방장 패킷 만드는 인터페이스
	static UINT32  PacketPackingEvent( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom );

	static UINT32  PacketPackingSync( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom, INT32 i32SlotIdx );

	static UINT32  PacketPackingHead( char* pOutBuf, UINT32 ui32SendPos, DS_BROADCASTINFOEX* pBroadcastInfo );
	
protected:
	// 게임 데이터를 버퍼에 저장합니다.
	static UINT32	_WriteGameInfo_Chara		( char* pOutBuf, UINT32 ui32BufLen,	UINT32 ui32SlotIdx,	DS_BROADCASTINFOEX	* pBroadcastInfo, CDediRoom* pDediRoom);

	//static UINT32 	_WriteGameInfo_Grenade		( char* pOutBuf, UINT32 ui32BufLen,	INT32 i32Idx,		DS_BROADCASTINFOEX	* pBroadcastInfo);
	
	// --------------------------------------------------------------------
	// _MergeStageInfoAsPossible에서 호출되는 함수들
	static UINT32	_MakeStageInfo_Chara			( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo );
	static UINT32	_MakeStageInfo_Mission			( char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo );

	static UINT32	_MakeStageInfo_Object			( UINT32 ui32CastFlag, char* pOutBuf, UINT32 ui32BufLen, DS_BROADCASTINFOEX* pBroadcastInfo, CDediRoom* pDediRoom, INT32 i32SlotIdx );
	// counter 리셋( 다시 처음부터 stage info 채우기 시작)
	static VOID		_ResetStageInfoIndices			( DS_BROADCASTINFOEX* pBroadcastInfo );
};

#endif	// __DediUdpBuilder_H
