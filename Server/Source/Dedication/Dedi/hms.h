#ifndef _HMS_H
#define _HMS_H

#include "weaponTable.h"

// 탄창 증가 최대 증가율
#define MAX_HACK_BULLET_INC_RATE	1.5f			// 탄창 증가 최대 비율. 아이템 40% + 벨트 2% + 홀스터 3.5% + 여유 3.5%

// ------------------------------------------------
// max counters : 이 숫자보다 크면 해킹으로 판단
// ------------------------------------------------
#define MAX_HACK_GRENADE_COUNT				9		// 캐릭터 하나당 가능한 투척무기 수
#define MAX_HACK_BULLET_COUNT				200		// weapon info에서 찾을 수 없는 경우(리로드 탄약수 * 1.5배)



#ifdef  _DEBUG

// 내부망에서 오탐을 확인하기 위한 테스트 용도
#define MAX_HACK_WALLSHOT_COUNT				0		// 5이상이면 해킹으로 판정
#define MAX_HACK_SPEED_COUNT				0		// 연속 3이상이면 해킹으로 판정(정상일경우 리셋됨)
#define MAX_HACK_RECALL_COUNT				0		// 연속 3이상이면 해킹으로 판정(정상일경우 리셋됨)
#define MAX_HACK_RESPAWN_POS_COUNT			0		// 3이상이면 해킹으로 판정
#define MAX_HACK_RESPAWN_TIME_COUNT			0		// 3이상이면 해킹으로 판정

#else
#define MAX_HACK_WALLSHOT_COUNT				0		// 5이상이면 해킹으로 판정
#define MAX_HACK_SPEED_COUNT				0		// 연속 3이상이면 해킹으로 판정(정상일경우 리셋됨)
#define MAX_HACK_RECALL_COUNT				2		// 연속 3이상이면 해킹으로 판정(정상일경우 리셋됨)
#define MAX_HACK_RESPAWN_POS_COUNT			0		// 3이상이면 해킹으로 판정
#define MAX_HACK_RESPAWN_TIME_COUNT			2		// 3이상이면 해킹으로 판정

#endif

// -----------------------------------------------
// 기준값 
// ------------------------------------------------
#define BOME_DISTANCE_SQUARE				(25.f)		// C4 Installation (시간 검사 삭제:2012-03-21)
#define BOME_INTSALL_MIN_TIME				(5.0f)
#define BOME_UINSTALL_MIN_TIME				(7.0f)
#define RESPAWN_MIN_TIME					(3.0f)			// Respawn: Death에서 Respawn 8초 내외. 100% 아이템 사용시 3.5초
#define INVINCIBILITY_MIN_TIME				(3.0f)			// Invincible time: 아이템을 착용하지 않았을 경우 로컬에서 12-13초임
#define INVINCIBILITY_MIN_TIME_CHAOS_MODE	(6.0f)			// 카오스 모드 무적 시간
//#define MELEE_WEAPON_RANGE_SQUARE			(8.3f)			// range: 근접 무기1.5 내외(찌르기 공격으로 5.0m로 늘림: 2.23m)
#define GRENADE_RANGE_SQUARE				(200.0f)		// grenade range: 정지점에서10m 이내(5-7m) 10m에서 14m로 늘림.

// **** 60에서 30으로 강화 2012-05-11
// speed hacktest 
#define MAX_SPEED					(15.0f)			// 평균 속도


#define MAX_FIRE_COUNT				(30)			// 고속총 발사 카운터
#define RECALL_DISTANCE_SQURE		(25.0f)			// |b - b'|^2
#define ATTACKER_DISTANCE_SQUARE	(4.84f)			// |a - a'|^2


// ------------------------------------------------
// 캐시 아이템
// ------------------------------------------------
#define VALUE_CASH_ITEM_MEGA_HP10							(10)		//	HP +10%
#define VALUE_CASH_ITEM_MEGA_HP5							(5)			//	HP +5%

#define VALUE_CASH_ITEM_INCREASE_INVINCIBLE_TIME			(1)			//	무적시간 +1sec

#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_DAMAGE			(-5)		//	Bullet Proof Vest : 받는 데미지 -5%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_PLUS_DAMAGE		(-10)		//	Bullet Proof Vest Plus : 받는 데미지 -10%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_GM_DAMAGE			(-90)		//	Bullet Proof Vest GM : 받는 데미지 -90%
#define VALUE_CASH_ITEM_BULLET_PROOF_VEST_METAL_DAMAGE		(-20)		//	Bullet Proof Vest Metal : 받는 데미지 -20%

#define VALUE_CASH_ITEM_FMJ_AMMO_DAMAGE						(10)		//	Full Metal Jacket Ammo : 총알 데미지 +10%		31
#define VALUE_CASH_ITEM_FMJ_AMMO_ACCURACY					(-10)		//	Full Metal Jacket Ammo : 총알 정확도 -10%
#define VALUE_CASH_ITEM_HP_AMMO_DAMAGE						(-10)		//	Hollow Point Ammo : 총알 데미지 -10%			32
#define VALUE_CASH_ITEM_HP_AMMO_ACCURACY					(10)		//	Hollow Point Ammo : 총알 정확도 +10%
#define VALUE_CASH_ITEM_HP_AMMO_PLUS_DAMAGE					(10)		//	Hollow Point Ammo Plus : 총알 데미지 +10%		78
#define VALUE_CASH_ITEM_HP_AMMO_PLUS_ACCURACY				(10)		//	Hollow Point Ammo Plus : 총알 정확도 +10%
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_DAMAGE				(5)			//	Jacketed Hollow Point Ammo : 총알 데미지 +5%		36
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_ACCURACY			(5)			//	Jacketed Hollow Point Ammo : 총알 정확도 +5%
#define VALUE_CASH_ITEM_JACKETED_HP_AMMO_MOVEMENT			(-10)		//	Jacketed Hollow Point Ammo : 이동속도 -10%

#define VALUE_CASH_ITEM_C4SPEEDKIT_INSTALL					(-2)		//	C4 Speed Kit : C4 설치 시간 감소 -2sec
#define VALUE_CASH_ITEM_C4SPEEDKIT_DISARM					(-2)		//	C4 Speed Kit : C4 해체 시간 감소 -2sec

#define VALUE_CASH_ITEM_QUICK_CHANGE_WEAPON					(-50)		//	Quick change weapon increase percentage (-50%)
#define VALUE_CASH_ITEM_QUICK_CHANGE_MAGAZINE				(-25)		//	Quick change magezine increase percentage (-25%)

#define VALUE_CASH_ITEM_INCREASE_GRENADE_SLOT				(1)			//	more grenade
#define VALUE_CASH_ITEM_INCREASE_DUAL_GRENADE_SLOT			(2)			//	more dual grenade

#define VALUE_CASH_ITEM_ANTI_FLASHBANG_MASK					(-50)		//	Anti FlashBang Mask : 플래시뱅 효과 -50%

#define VALUE_CASH_ITEM_KETUPAT_HP10						(10)		//	HP +10%
#define VALUE_CASH_ITEM_VALENTINE_CHOCOLATE					(10)		//  HP + 10 (퍼센트 아님)

#define VALUE_CASH_ITEM_SHORT_RESPAWN_20					(0.2f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_20
#define VALUE_CASH_ITEM_SHORT_RESPAWN_30					(0.3f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_30
#define VALUE_CASH_ITEM_SHORT_RESPAWN_50					(0.5f)		// 리스폰 단축	: CASHITEM_GROUP_SHORT_RESPAWN_50

// ------------------------------------------------
// 
// ------------------------------------------------

class CHMS
{
public:
	//-----------------------------------------------------------
	// hack count 
	//	: 오탐 가능성 있는 검사는 카운터 유지
	//	: 핵탐지시 호출되며 값을 얻으면 1증가 된다
	//-----------------------------------------------------------
	
	UINT8	GetHackCount( UINT32 ui32SlotIdx, DS_HACK_TYPE hack )
	{
		I3ASSERT( ui32SlotIdx < SLOT_MAX_COUNT );
		if( ui32SlotIdx < SLOT_MAX_COUNT )
		{
			switch( hack )
			{
			case HACK_TYPE_SPEED:				return (++m_aui8SpeedHackCount			[ui32SlotIdx]);
			case HACK_TYPE_WALLSHOT:			return (++m_aui8WallShotCount			[ui32SlotIdx]);
			case HACK_TYPE_RECALL:				return (++m_aui8RecallHackCount			[ui32SlotIdx]);
			case HACK_TYPE_RESPAWN_TIME:		return (++m_aui8RespawnTimeHackCount	[ui32SlotIdx]);
			case HACK_TYPE_RESPAWN_POSITION:	return (++m_aui8RespawnPosHackCount		[ui32SlotIdx]);
			default:
				I3ASSERT(0);
				return 0;
			}
		}
		return 0;
	}

	//-------------------------------------
	// reset utilities
	//-------------------------------------
	void ResetOnRoomCreate	( UINT32 ui32StageID, UINT8 ui8AvailableWeapon );
	void ResetOnEnter		( UINT32 ui32SlotIdx ); 
	void ResetOnRespawn		( UINT32 ui32SlotIdx, UINT32* pCurrentWeapons );
	void ResetOnLeave		( UINT32 ui32SlotIdx );
	
	//---------------------------------------------------------------------------
	// stage data
	//---------------------------------------------------------------------------
	UINT32	GetStageID() const				{	
												I3ASSERT(m_ui32StageID != 0); 
												return m_ui32StageID;			
											}
	UINT8	GetAvailableWeapon() const		{	
												I3ASSERT(m_ui8AvailableWeapon != 0); 
												return m_ui8AvailableWeapon;	
											}
	CHMS();
	~CHMS();
private:
	//-------------------------------------
	// reset utilities
	//-------------------------------------
	void	_ResetHackData();
	void	_ResetHackData( UINT32 ui32SlotIdx );

	//---------------------------------------------------------------------------
	// stage data
	//---------------------------------------------------------------------------
	UINT32	m_ui32StageID;
	UINT8	m_ui8Mode;						// 공룡모드 구분용(m_ui8StageID로부터 계산)
	UINT8	m_ui8AvailableWeapon;			// stage 별로 사용가능한 무기 검사용

	// Note. count는 캐릭터 초기화시 max bullet으로 reset하고 fire가 발생할 때마다 minus 하는 방식으로 변경(2012-11-09)
	// throw weapon count, primary fire count... 등도 아래 배열 이용
	// 
	UINT16	m_aaui16RemainBullets				[ SLOT_MAX_COUNT ][WEAPON_SLOT_COUNT];

	//---------------------------------------------------------------------------
	// hacking data
	//---------------------------------------------------------------------------
	UINT8	m_aui8WallShotCount					[ SLOT_MAX_COUNT ];
	UINT8	m_aui8SpeedHackCount				[ SLOT_MAX_COUNT ];
	UINT8	m_aui8RecallHackCount				[ SLOT_MAX_COUNT ];
	UINT8	m_aui8RespawnPosHackCount			[ SLOT_MAX_COUNT ];
	UINT8	m_aui8RespawnTimeHackCount			[ SLOT_MAX_COUNT ];
};

extern NxBounds3 g_StageBounds[ STAGE_UID_MAX ];
void g_CommonGetMapBounds( STAGE_UID id, REAL32* pBound );

////////////////////////////////////////////////////////////////////////////////////
// Hacking Checker
////////////////////////////////////////////////////////////////////////////////////

class HACK_LOG 
{
// purpose: 핵로그 출력 형식을 맞춤
public:
	void AddInfo( const char *fmt, ... ) 
	{
		INT32 i32Len = sizeof( m_szAddInfo );
		DS_MAKE_VAR_STRING( m_szAddInfo, i32Len, fmt );

		if( i32Len > 0 )
			m_i32AddInfoLen = i32Len;
	}
	
	char* MakeLogString( CDediRoom* pRoom, UINT32 ui32SlotIdx, UINT8 ui8HackType );
	
	HACK_LOG()		{ 
						Reset(); 
					}
	HACK_LOG( const char *fmt, ... ) { 
						m_i32AddInfoLen = 0;
						INT32 i32Len = sizeof( m_szAddInfo );
						DS_MAKE_VAR_STRING( m_szAddInfo, i32Len, fmt );

						if( i32Len > 0 )
							m_i32AddInfoLen = i32Len;
					}
	void Reset()	{ 
						m_szAddInfo[0]  = m_szOutputBuffer[0] = '\0'; 
						m_i32AddInfoLen = 0;
					}

private:
	char  m_szAddInfo	  [512];
	char  m_szOutputBuffer[512];
	INT32 m_i32AddInfoLen;
};

BOOL IsMedicalKit( UINT32 ui32Class);
BOOL IsWPSmoke(UINT32 ui32Class);
BOOL IsThrowKnifeOnRMB(UINT32 ui32WeaponID, WEAPON::ATTACK_TYPE eAttackType);
#endif