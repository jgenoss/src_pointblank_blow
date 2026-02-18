#ifndef __ROOM_IN_BOMB_H__
#define __ROOM_IN_BOMB_H__

#include "RoomBase.h"

class CRoomInBomb : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInBomb );

#ifndef NEW_EXPPOINT
	UINT8					m_ui8BonusBombInstall;			// ÆøÅº ¼³Ä¡ º¸³Ê½º °æÇèÄ¡/Æ÷ÀÎÆ®
	UINT8					m_ui8BonusBombUninstall;		// ÆøÅº ÇØÃ¼ º¸³Ê½º °æÇèÄ¡/Æ÷ÀÎÆ®
#else
	UINT8					m_ui8BonusBombMax;				// ÆøÅº ¹Ì¼Ç ¼öÇà ÃÖ´ë È¸¼ö
	UINT8					m_ui8BonusBombExp;				// ÆøÅº ¹Ì¼Ç ¼öÇà Á¡¼ö
#endif

	INT32					m_i32BombInstallSlotIdx;		// ÆøÅº¼³Ä¡ ÀÎµ¦½º	
	INT32					m_i32BombUnInstallSlotIdx;		// ÆøÅºÇØÃ¼ ÀÎµ¦½º
	UINT32					m_ui32BombInstallTime;			// ÆøÅº¼³Ä¡ Å¸ÀÓ
	UINT16					m_ui16BombTime;					// ÆøÅº ÅÍÁö´Â ½Ã°£ ÃßÈÄ ¸Ê¸¶´Ù Æ²¸² 

	UINT8					m_aui8AccMissionBombInstallCount[SLOT_MAX_COUNT];	// ÆøÅº ¼³Ä¡ È½¼ö
	UINT8					m_aui8AccMissionBombUninstallCount[SLOT_MAX_COUNT];	// ÆøÅº ÇØÁ¦ È½¼ö

protected:

public:
	CRoomInBomb();
	virtual ~CRoomInBomb();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32	ui32StageID );
	
	virtual void			SetMatchStart();
	virtual void			SetRoundStart();

	virtual bool			RoundStart( UINT32 ui32RTBattle );

	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

#ifdef NEW_EXPPOINT
	virtual INT16			GetMissionExp( INT32 i32SlotIdx );
#endif

#ifndef NEW_EXPPOINT
	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16			GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
	virtual UINT16			GetMatchAccPoint( INT32 i32SlotIdx, UINT16 ui16AccPoint );
#endif

	virtual void			AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );

	// Mission Message
	virtual BOOL			MissionBombInstall( INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos, UINT32 ui32RTBattle );
	virtual BOOL			MissionBombUnInstall( INT32 i32SlotIdx );
};

#endif