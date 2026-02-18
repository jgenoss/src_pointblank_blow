/**
 * \file	InBattleDef.h
 *
 * Declares the in battle def class.
 */


#if !defined( __IN_BATTLE_DEF_H__)
#define __IN_BATTLE_DEF_H__

struct KILLING_INFO
{
	// 죽임 종류
	VAR_LR(UINT16,85)	m_KillType;
	// 무기 정보
	VAR_LR(UINT32,86)	m_KillWeaponInfo;	//0x00 F(ExtState) F(EventType) FF(ItemType) FF(ClassType) FF(Number)
	// 연속 킬 카운트
	VAR_LR(INT32,87)	m_KillerAgainCount;
	// 죽은사람이 들고있던 무기
	VAR_LR(UINT8,88)	m_ui8Weapon[ MAX_KILLUSER_COUNT ];
	// 메시지 data
	VAR_LR(UINT32,89)	m_KillBuffer[ MAX_KILLUSER_COUNT ];	// 0x000 F(DefenceSystem) FF(Message) F(DeathType) F(Index)
	// 죽인 위치
	VAR_LR(REAL32,90)	m_KillPositionBuf[ MAX_KILLUSER_COUNT ][3];
	// 오브젝트 사망에 의한 오브젝트 인덱스
	VAR_LR(INT32,91)	m_KillObjectIdx;

	void	Reset( void);
	void	QueKillingInfo( UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx );
};







#endif
