#ifndef __ROOM_IN_DESTROY_H__
#define __ROOM_IN_DESTROY_H__

#include "RoomBase.h"

class CRoomInDestroy : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInDestroy );

	UINT8					m_ui8BonusDestroyExp;					// 파괴 보너스 경험치

	UINT8					m_ui8BonusDamageMax;
	UINT8					m_ui8rBonusDamageScore;
	UINT8					m_ui8BonusDestroyMax;
	UINT8					m_ui8BonusDestroyScore;

	DESTRUCTION_INFO		m_MiDestructionInfo;					// 파괴 미션 오브젝트 정보
	UINT16					m_ui16MiObjectMaxHp_Red; 
	UINT16					m_ui16MiObjectMaxHp_Blue;

protected:
	UINT8					m_ui8AccMissionDestroy[SLOT_MAX_COUNT];	// 파괴 수행 횟수
	UINT16					m_ui16AccMissionDamageScore[SLOT_MAX_COUNT];

public:
	CRoomInDestroy();
	virtual ~CRoomInDestroy();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32 ui32StageID );
	virtual BOOL			UserGiveUpBattle( INT32 i32SlotIdx );

	virtual void			SetMatchStart();
	virtual void			SetRoundStart();

	virtual BOOL			CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16			GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );

	virtual INT16			GetMissionExp( INT32 i32SlotIdx );

	virtual void			AddStartBattlePacket( PACKET_BATTLE_STARTBATTLE_ACK* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );

	// Mission Message
	virtual BOOL			MissionObjectDestroy( DESTRUCTION_INFO* pInfo );
	virtual void			GetRoomNowScore( ROOM_NOW_SCORE* pRoomNowScore, UINT32 ui32RTServer );

	// Result Person
	virtual REAL32			GetPercentOfAsymmetryUserExp(INT32 i32Team, INT32 i32AllCount, INT32 i32TeamCount);
	virtual REAL32			GetPercentOfAsymmetryUserPoint(INT32 i32Team, INT32 i32AllCount, INT32 i32TeamCount);
};

#endif