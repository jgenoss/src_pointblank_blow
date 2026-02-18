#ifndef __ROOM_IN_DESTROY_H__
#define __ROOM_IN_DESTROY_H__

#include "RoomBase.h"

class CRoomInDestroy : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInDestroy );

#ifndef NEW_EXPPOINT
	UINT8					m_ui8BonusDestroyExp;					// 파괴 보너스 경험치
#else
	UINT8					m_ui8BonusDamageMax;
	UINT8					m_ui8rBonusDamageScore;
	UINT8					m_ui8BonusDestroyMax;
	UINT8					m_ui8BonusDestroyScore;
#endif

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

	virtual BOOL			CheckRoundEnd( UINT32 ui32ServerTime );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

#ifndef NEW_EXPPOINT
	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual UINT16			GetMatchAccExp( INT32 i32SlotIdx, UINT16 ui16AccExp );
#else
	virtual INT16			GetMissionExp( INT32 i32SlotIdx );
#endif

	virtual void			AddStartBattlePacket( i3NetworkPacket* pPacket, CUserSession * pSession, UINT32 ui32RTBattle );

	// Mission Message
	virtual BOOL			MissionObjectDestroy( DESTRUCTION_INFO* pInfo );
};

#endif