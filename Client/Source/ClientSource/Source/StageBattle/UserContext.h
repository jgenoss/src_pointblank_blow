#if !defined( __USER_CONTEXT_H)
#define __USER_CONTEXT_H

#include "InBattleDef.h"

class UserContext : public i3::shared_ginst<UserContext>
{
public:
	/** \brief Game 남은시간 */
	REAL32 fRemainingBattleTime; // fBattleTime에서 네이밍 변경

	/** \brief Battle, Mission에서 게임중인지를 체크합니다.  */
	UINT16 PlayingSlotFlag;
	
	/** \brief 옵저버 유저 판별용(GM옵저버) 입니다. ( 옵저버가 아닐 때 플래그가 켜짐 )*/
	UINT16 BattleUserFlag;

	/** \brief 죽은 유저 검사용 flag */
	bool Death[SLOT_MAX_COUNT];		
	
	/** \bref ??? */
	bool bBowFireOrder;

	/** \bref 이벤트 아이템 지급 관련 팝업 처리 */
	bool ReceivedEvent;

	/** \brief Only For Host */
	/** \brief Kill 정보를 서버에 보내기 위한 버퍼	*/
	KILLING_INFO KillInfo[SLOT_MAX_COUNT];

public:
	UserContext();
	
	bool		IsPlayingSlot(INT32 idxSlot) const;
	bool		IsBattleSlot(INT32 idxSlot) const;

	void		QueDeathChara( INT32 idx, UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx);
	void		SendDeathChara();

	void		CreateBattle( void);
	void		InitRoomData( void);

	INT32		GetIndexedRankderIdx(TEAM_TYPE team, INT32 idx);
	INT32		GetTopRankerIdx(TEAM_TYPE team);

protected:
	bool		_IsLiveCharacter(TEAM_TYPE team, INT32 idx);
};

#endif