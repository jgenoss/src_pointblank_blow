#ifndef _UDP_CHECKER_H
#define _UDP_CHECKER_H

#include "DediRoom.h"
// 데디케이션 서버에서 사용

class UDPChecker
{
public:
	static INT32 GamePacketParsing( CDediRoom* pRoom, const char* pPacket, INT32 nSize, RECV_PACKET_INFO* pRecvInfo );	
	
private:
	static BOOL				_ParseCharaState			( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_CHARA_STATE* pState );	
	static BOOL				_ParseCharaPosRotPacket		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot );
	//static PACKET_CHECK		_ParseCharaActionKey		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pActionKey );
	static BOOL				_ParseCharaTakingObject		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_TAKING_OBJECT* pObject );
	static BOOL				_ParseCharaSyncObj			( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_SYNC_OBJECT* pSyncObj );
	static BOOL				_ParseCharaRadioChat		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_RADIO_CHAT* pRadioChat );
	static BOOL				_ParseCharaWeaponInfo		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_WEAPON*  pWeapon );
	static BOOL				_ParseCharaWeaponParam		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pParam );
	static BOOL				_ParseCharaSuicideDmg		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_SUICIDEDAMAGE* pSuicideDmg );
	static BOOL				_ParseMissionPacket			( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_MISSION* pMissionInfo );
	static BOOL				_ParseDropWeaponPacket		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pDropWeapon );
	static PACKET_CHECK		_ParseGetWeaponPacket		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON* pWeapon );
	static BOOL				_ParseDirectPickUpPacket	( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP* pWeapon);
	static BOOL				_ParseFirePacket			( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire );
	//static BOOL				_ParseGrenadePacket			( CDediRoom* pRoom,  UINT32 ui32SlotIdx, UINT16 ui16GrenadeIdx, GAMEINFO_THROW_WEAPON_NETDATA* pThorwWeapon );
	static BOOL				_ParseDroppedWeaponPacket	( CDediRoom* pRoom,  UINT32 ui32SlotIdx, GAMEINFO_DROPEDWEAPON_NETDATA* pDropInfo );
	static BOOL				_ParseHitPacketByChara		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pArrHit, UINT32 ui32Count ); 
	static BOOL				_ParseHitPacketByExplosion	( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_HIT_BYEXPLOSION* pArrHit, UINT32 ui32Count );
	static BOOL				_ParseHitPacketByObject		( CDediRoom* pRoom,  UINT32 ui32SlotIdx, N_PCINFO_HIT_BYOBJECT* pArrHit, UINT32 ui32Count );
};

#endif