#include "pch.h"
#include "InBattleDef.h"

void KILLING_INFO::Reset( void)
{
	INT32 i;

	m_KillType = 0;
	m_KillWeaponInfo = 0;
	m_KillerAgainCount = 0;
	for( i = 0; i < MAX_KILLUSER_COUNT; i++ )
	{
		m_ui8Weapon[ i] = 0;
		m_KillBuffer[ i] = 0;
		m_KillPositionBuf[ i][0] = 0;
		m_KillPositionBuf[ i][1] = 0;
		m_KillPositionBuf[ i][2] = 0;
		m_ui8AssistIdx[i]		 = 0;
	}
	m_KillObjectIdx = -1;
}

void KILLING_INFO::QueKillingInfo(UINT8 ui8DeathWeapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx, INT32 AssistIdx)
{	
	UINT32 iAgainCount = m_KillerAgainCount;
	if( iAgainCount >= MAX_KILLUSER_COUNT )
	{
		DSERROR("iAgainCount >= MAX_KILLUSER_COUNT\n");
		return;
	}

	if( iAgainCount > 0 )
	{
		switch( nType )
		{
		case CHARA_DEATH_A:
		case CHARA_DEATH_H:
			{
				switch (GET_ITEM_SUBTYPE(WeaponInfo))
				{
				case WEAPON_CLASS_SHOTGUN:
				case WEAPON_CLASS_DUALSHOTGUN:
					m_KillType = CHARA_KILL_MASS;
					break;
				default:
					m_KillType = CHARA_KILL_PIERCING;	// piercing
					break;
				}
			}
			break;
		case CHARA_DEATH_O :
			{
				WeaponInfo	= m_KillWeaponInfo;
			}
		case CHARA_DEATH_B :
		case CHARA_DEATH_F :
			{
				m_KillType = CHARA_KILL_MASS;			// MassKill
			}
			break;
		case CHARA_DEATH_TRAMPLED:
		case CHARA_DEATH_HOWL:
		case CHARA_DEATH_POISON:
			{
				m_KillType = CHARA_KILL_MASS;			// MassKill
			}
			break;
		default :
			{
				DSWARNING( "이상한 킬입니다.\n" );
				return;
			}
			break;
		}
	}
	else
	{
		m_KillType = CHARA_KILL_DEFAULT;
	}

	m_KillWeaponInfo				= WeaponInfo;
	m_ui8Weapon[ iAgainCount ]		= ui8DeathWeapon;
	m_KillBuffer[ iAgainCount ]		= nDeathPacket;
	m_ui8AssistIdx[iAgainCount]		= AssistIdx;
	

	// 죽은 유저의 위치
	m_KillPositionBuf[ iAgainCount ][0] = getX( pDeathPos );
	m_KillPositionBuf[ iAgainCount ][1] = getY( pDeathPos );
	m_KillPositionBuf[ iAgainCount ][2] = getZ( pDeathPos );

	m_KillObjectIdx = nObjectIdx;

	m_KillerAgainCount++;
	I3ASSERT( m_KillerAgainCount <= MAX_KILLUSER_COUNT );
}

void PING_INFO::Init( void)
{
	m_fPingSendTimer	= 0.0f;
	m_rPingCheckTime	= 0.0f;
	m_rPingSendTime		= 0.0f;
}

void PING_INFO::Reset( void)
{
	INT32 i;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_Pings[ i ]				= 1;
		m_rPingList[i]				= 0.0f;
	}
}

void PING_INFO::RecvPing( INT32 userIdx, REAL32 sendTime, REAL32 elapsedTime)
{
	//1. m_rPingSendTime 과 같은 핑이 왔는지 확인 아니라면
	if( m_rPingSendTime == sendTime)
	{	// 경과 시간을 더하고 평균을 계산
		m_rPingList[ userIdx] = (m_rPingList[ userIdx] + (elapsedTime - m_rPingSendTime)) * 0.5f;
	}
	else
	{	// 0.4초 delay를 더하고 평균을 구한다.
		m_rPingList[ userIdx] = (m_rPingList[ userIdx] + 0.4f) * 0.5f;
	}
}

void BATTLE_RESULT_INFO::Init( void)
{
	m_pBattleEventExp = 0;
	m_pBattleEventPoint = 0;
	m_pBattleResultIcon = 0;
	m_BattleExp = 0;
	m_BattlePoint = 0;
	m_BattleScore = 0;

	m_AccMissionCount = 0;
}

void ASSIST_INFO::Reset(void)
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_DetailInfo[i].Reset();
	}
	m_ui16AssistOrder = 0;

}

void ASSIST_INFO::SlotReset(UINT32 ui32Slotidx)
{
	m_DetailInfo[ui32Slotidx].Reset();
}

void ASSIST_INFO::SetAssist(UINT8 ui8ShooterIdx, INT32 i32Damage)
{
	if (m_ui16AssistOrder >= INIT_UINT16)	OrderReset();

	m_ui16AssistOrder++;
	m_DetailInfo[ui8ShooterIdx].SetAssistInfo(i32Damage, m_ui16AssistOrder);

}

void ASSIST_INFO::OrderReset(void)				// AssistOrder 리셋
{
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
		m_DetailInfo[i].m_ui16AttackTurn = 0;	//타격 순서 초기화.
}

INT8 ASSIST_INFO::GetAssistidx(UINT32 ui32KillIdx) //어시스트 획득 유저 판별
{
	INT32	i32MaxDamage = 0;
	UINT16	ui6AttackTurn = 0;
	INT8	i8AssistIdx = 0;

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		if (MIN_ASSIST_DAMAGE > m_DetailInfo[i].m_i32TotalDamage) continue;
		if (i32MaxDamage > m_DetailInfo[i].m_i32TotalDamage) continue;
		if (i == ui32KillIdx) continue;			//Kill한 유저는 제외

		if (i32MaxDamage < m_DetailInfo[i].m_i32TotalDamage)
		{
			i8AssistIdx = i;
			i32MaxDamage = m_DetailInfo[i].m_i32TotalDamage;
			ui6AttackTurn = m_DetailInfo[i].m_ui16AttackTurn;
		}
		else // 총데미지가 같은 경우 공격 순서를 비교해서 최근 타격한 유저Idx로 변경
		{
			if (ui6AttackTurn < m_DetailInfo[i].m_ui16AttackTurn)
			{
				i8AssistIdx = i;
				ui6AttackTurn = m_DetailInfo[i].m_ui16AttackTurn;
			}
		}
	}
	//
	if (i32MaxDamage < MIN_ASSIST_DAMAGE) return ui32KillIdx;
	return i8AssistIdx;
}

void ASSIST_DETAIL_INFO::Reset(void)
{
	m_i32TotalDamage = 0;
	m_ui16AttackTurn = 0;

}

void ASSIST_DETAIL_INFO::SetAssistInfo(INT32 i32Damage, UINT16 ui16AttackTurn) //데미지를 입었을 시 어시스트 정보 설정
{
	m_ui16AttackTurn = ui16AttackTurn;
	if (i32Damage > 0)
		m_i32TotalDamage += i32Damage;
}


