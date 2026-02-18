#pragma once

class CDediRoom;

class UDPParser
{
public:
	static INT32	GamePacketParsing		( CDediRoom* pRoom, RECV_PACKET_INFO* pRecvInfo, UINT32* ui32FireActionFlag );
	
private:
	static BOOL		_ProcessCharaActionKey	( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pIn);
	static BOOL		_PacketFirePacket		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire );
	static BOOL		_ParseGrenadePacket		( CDediRoom* pRoom, UINT32 ui32SlotIdx, UINT16 ui16Idx, GAMEINFO_THROW_WEAPON_NETDATA* pThorwWeapon );
	static BOOL		_ParseDropWeaponPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon);
	static BOOL		_ParseGetWeaponPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON * pWeapon);
	static BOOL		_ParseDirectPickUpPacket( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP * pWeapon);
};
