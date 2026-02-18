#include "pch.h"
#include "RespawnMgr.h"


CRespawnMgr::CRespawnMgr()
{
	Reset();
}

void CRespawnMgr::Reset()
{
	m_ui32RedCount		= 0;
	m_ui32BlueCount		= 0;
	m_ui32RedNpcCount	= 0;
	m_ui32BlueNpcCount	= 0;
}

BOOL CRespawnMgr::AddRespawnPosition( UINT8 ui8TeamType, NxVec3& pos )
{
	BOOL bRet = TRUE;
	
	switch ( ui8TeamType )
	{
	case 0: //CHARACTER_TEAM_RED: 
		{
			if( m_ui32RedCount < DS_MAX_RESPAWN_PER_TEAM )
			{
				m_avRed[ m_ui32RedCount ] = pos;
				m_ui32RedCount++;
			}
			else
			{
				bRet = FALSE;
			}
		}
		break;
	case 1: //CHARACTER_TEAM_BLUE:
		{
			if( m_ui32BlueCount < DS_MAX_RESPAWN_PER_TEAM )
			{
				m_avBlue[ m_ui32BlueCount ] = pos;
				m_ui32BlueCount++;
			}
			else
			{
				bRet = FALSE;
			}
		}
		break;
	case 2: //NPC_TEAM_RED:
		{
			if( m_ui32RedNpcCount < DS_MAX_RESPAWN_PER_TEAM )
			{
				m_avBlue[ m_ui32RedNpcCount ] = pos;
				m_ui32RedNpcCount++;
			}
			else
			{
				bRet = FALSE;
			}
		}
		break;
	case 3: //NPC_TEAM_BLUE:
		{
			if( m_ui32BlueNpcCount < DS_MAX_RESPAWN_PER_TEAM )
			{
				m_avBlue[ m_ui32BlueNpcCount ] = pos;
				m_ui32BlueNpcCount++;
			}
			else
			{
				bRet = FALSE;
			}
		}
		break;
	default:
		I3ASSERT( 0 );
		break;
	}

	if(!bRet) DSERROR("AddRespawnPosition(): Count Error\n");

	return bRet;
}