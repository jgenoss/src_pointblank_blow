#ifndef __ROOM_IN_TUTORIAL_H__
#define __ROOM_IN_TUTORIAL_H__

#include "RoomBase.h"

// 튜토리얼 모드 구현 관련하여 미정인 사항이 많아 임시로 구현
class CRoomInTutorial : public CRoomBase
{
	I3_CLASS_DEFINE( CRoomInTutorial );

	INT32					m_i32BombInstallSlotIdx;		// 폭탄설치 인덱스	
	INT32					m_i32BombUnInstallSlotIdx;		// 폭탄해체 인덱스
	UINT32					m_ui32RTBombInstall;			// 폭탄설치 타임
	UINT16					m_ui32BombTime;					// 폭탄 터지는 시간 추후 맵마다 틀림 

	BOOL					m_bRoundEnd;					// 라운드 종료 플래그
	BOOL					m_bMatchEnd;					// 배틀 종료 플래그

	BOOL					m_bIsFirst;						// 처음 플레이인지 확인

protected:

public:
	CRoomInTutorial();
	virtual ~CRoomInTutorial();

	virtual BOOL			OnCreate( TYPE_ROOM_INFO* pRoomInfo );
	void					OnDestroy();

	virtual BOOL			SetRoomOption( UINT32 ui32StageID );
	
	virtual BOOL			CheckRoundEnd( UINT32 ui32RTServer );
	virtual BOOL			CheckMatchEnd();

	virtual INT32			GetWinTeam();

	virtual void			GetBasicExpPoint( INT32 i32Win, TYPE_ROOM_GET_EXPPOINT_V1* pstGetExpPoint );

	virtual REAL32			GetSubPercentOfUnusualEnd( INT32 i32Win );

	virtual	UINT16			GetMatchEndExp(INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccExp, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle);
	virtual	UINT16			GetMatchEndPoint( INT32 i32Win, INT32 i32SlotIdx, UINT16 ui16AccPoint, REAL32 r32AdjustPercent, REAL32 r32AsymPercent, BOOL bNormalEnd, UINT32 ui32RTBattle );

	// Mission Message
	virtual BOOL			MissionBombInstall( INT32 i32SlotIdx, UINT8 ui8BombPosition, REAL32 * pr32BombVecPos, UINT32 ui32RTBattle );
	virtual BOOL			MissionBombUnInstall( INT32 i32SlotIdx );

	virtual BOOL			MissionTutorialRoundEnd( CUserSession* pSession);
};

#endif