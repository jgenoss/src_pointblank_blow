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
		m_KillBuffer[ i] = 0;
		m_KillPositionBuf[ i][0] = 0.0f;
		m_KillPositionBuf[ i][1] = 0.0f;
		m_KillPositionBuf[ i][2] = 0.0f;
	}
	m_KillObjectIdx = -1;
}

void KILLING_INFO::QueKillingInfo( UINT8 ui8Weapon, UINT32 nDeathPacket, INT32 nType, UINT32 WeaponInfo, VEC3D * pDeathPos, INT32 nObjectIdx )
{	
	INT32 iAgainCount = m_KillerAgainCount;
	I3ASSERT( iAgainCount < MAX_KILLUSER_COUNT );

	if( iAgainCount > 0 )
	{
		switch( nType )
		{
		case CHARA_DEATH_A:
		case CHARA_DEATH_H:
			{
				WEAPON_CLASS_TYPE weaponClass = WEAPON::ItemID2Class( WeaponInfo);

				switch( weaponClass )
				{
				case WEAPON_CLASS_SHOTGUN		:
				case WEAPON_SUBCLASS_SHOTGUN	:
				case WEAPON_CLASS_DUALSHOTGUN	:	m_KillType = CHARA_KILL_MASS;		break;
				default							:	m_KillType = CHARA_KILL_PIERCING;	break;
				}
			}
			break;
		case CHARA_DEATH_B :
		case CHARA_DEATH_O :
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
				//I3TRACE( "이상한 킬입니다.\n" );
				I3PRINTLOG(I3LOG_WARN,  "이상한 킬입니다." );
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
	m_ui8Weapon[ iAgainCount ]		= ui8Weapon;
	m_KillBuffer[ iAgainCount ]		= nDeathPacket;

	// 죽은 유저의 위치
	m_KillPositionBuf[ iAgainCount ][0] = getX( pDeathPos );
	m_KillPositionBuf[ iAgainCount ][1] = getY( pDeathPos );
	m_KillPositionBuf[ iAgainCount ][2] = getZ( pDeathPos );

	m_KillObjectIdx = nObjectIdx;

	INT32 tmpKillerAginCount = m_KillerAgainCount;
	m_KillerAgainCount = tmpKillerAginCount + 1;

	I3ASSERT( m_KillerAgainCount <= MAX_KILLUSER_COUNT );
}


