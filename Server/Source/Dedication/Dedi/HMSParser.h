#ifndef _HMSPARSER_H
#define _HMSPARSER_H

class CDediRoom;
class CCharacter;
class CHMS;

class HMSParser
{
public:
	static INT32 GamePacketParsing( CDediRoom* pRoom, RECV_PACKET_INFO* pInfo );
	
private:
	static BOOL	_ParseCharaState			( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_CHARA_STATE* pState);
	static BOOL _ParseCharaPosRotPacket		( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot, REAL32 r32PacketTime, UINT32 ui32RespawnCount );
	//static BOOL _ParseCharaActionKey		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_ACTIONKEY* pActionKey );
	static BOOL	_ParseCharaTakingObject		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_TAKING_OBJECT*  pOnLoad );
	static BOOL _ParseCharaWeaponParam		( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pParam, REAL32 r32PacketTime );
	static BOOL _ParseMissionPacket			( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_MISSION* pMissionInfo, REAL32 r32PacketTime );
	static BOOL _ParseDropWeaponPacket		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DROPWEAPON* pWeapon );
	static BOOL _ParseGetWeaponPacket		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_GETWEAPON* pWeapon );
	static BOOL _ParseDirectPickUpPacket	( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_DIRECTPICKUP* pWeapon);
	static BOOL	_ParseBulletPacket			( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire, REAL32 r32PacketTime );
	static BOOL	_ParseHitPacketByChara		( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pArrHit, UINT32 ui32Count );
	static BOOL	_ParseHitPacketByExplosion	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYEXPLOSION* pArrHit, UINT32 ui32Count );
	static BOOL	_ParseHitPacketByObject		( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_BYOBJECT* pArrHit, UINT32 ui32Count );
	static BOOL	_ParseTouchDown				( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_TOUCHDOWN* pTouchDown );

	static void _RollBack ( DS_PACKET_ROLLBACK* pRollback ); 
	static void _SaveData ( DS_PACKET_ROLLBACK* pRollback );

public:
	// 사전 핵체크 기능 // HMSPasrser_HackCheck.cpp
	static DS_HACK_TYPE 	_CheckWeaponRange	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_HIT_DEDICATED2* pHitInfo );
	static DS_HACK_TYPE 	_CheckFireSpeed		( CDediRoom* pRoom, UINT32 ui32SlotIdx, GAMEINFO_FIRE* pFire, REAL32 r32PacketTime );
	static DS_HACK_TYPE 	_CheckWeaponSync	( CDediRoom* pRoom, UINT32 ui32SlotIdx, const GAMEINFO_CHARA_WEAPON* pWeapon );
	static DS_HACK_TYPE 	_CheckWeaponSync	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON* pWeapon );
	static DS_HACK_TYPE 	_CheckWeaponParam	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_PCINFO_WEAPON_PARAM* pWeaponParam );
	static BOOL				_CheckRecallHack	( CDediRoom* pRoom, UINT32 ui32SlotIdx, N_POS_R* pDeathPos0, N_POS* pDeathPos1, N_PCINFO_HIT_WEAPON* pHitWeapon, BOOL bAttacker );
	static DS_HACK_TYPE		_CheckNaturalRPGGet	( CDediRoom* pRoom, UINT32 ui32SlotIdx, UINT32 ui32Class, UINT32 ui32Number);
	static BOOL			 	_CheckRPG			( CDediRoom* pRoom, T_ItemID TItemID );
	static void			 	_CheckGhost			( CDediRoom* pRoom, CCharacter * pChara, REAL32 r32PacketTime, UINT32 ui32SlotIdx, N_PCINFO_POSROTATION* pPosRot);

	static DS_HACK_TYPE		_CheckUseWeapon		( CDediRoom* pRoom, UINT32 ui32SlotIdx, T_ItemID TItemID );

	static BOOL				_IsInvincibleTime	( CDediRoom* pRoom, N_PCINFO_HIT_DEFAULT* pDefault );
};

#endif //_HMSPARSER_H