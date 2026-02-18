#pragma once

class CDediRoom;

class UDPParser2
{
public:
	static INT32	GamePacketParsing		( CDediRoom* pRoom, RECV_PACKET_INFO* pRecvInfo, UINT32* ui32FireActionFlag );
	
private:
	static BOOL		_ProcessCharaActionKey	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_ACTIONKEY * pIn);
	static BOOL		_ParseDropWeaponPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_DROPWEAPON* pWeapon);
	static BOOL		_ParseGetWeaponPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_GETWEAPON * pWeapon);
};
