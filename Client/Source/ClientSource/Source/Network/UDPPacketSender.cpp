#include "pch.h"
#include "UDPPacketSender.h"
#include "../GlobalVariables.h"
#include "ClientUDPSocket.h"

#include "../StageBattle/UserContext.h"
#include "../StageBattle/NetworkContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "GameNetworkManager.h"
#include "UserInfoContext.h"
#include "BattleSyncTimer.h"

#include "ClientStageInfoUtil.h"

//#define	UDPSEND_TRACE

#define UDPSEND_TIME		0.0666f			// 초당 15회

UDPPacketSender::UDPPacketSender()
{
	m_rSendTime = 0.f;
	m_DediSendTime = 0.f;
	i3mem::FillZero( m_SendBuffer, sizeof( m_SendBuffer));
}

bool UDPPacketSender::Create()
{
	m_rSendTime = 0.f;
	return true; 
}

void UDPPacketSender::Destroy( void )
{	
	return; 
}

UINT16 UDPPacketSender::__DedicatedService( char * pOutBuffer, UINT32 ui32BufSize, GAMEINFO_THROW_WEAPON * pThrowInfo,
										   GAMEINFO_DROPED_WEAPON * pDropedInfo )
{
	INT32 i;

	UINT16 nSendPos = 0;
	// 자신의 던지는 무기들만 보냅니다.
	for( i = 0; i < MAX_THROWWEAPON_SLOP; i++)
	{
		if( (pThrowInfo[ i]._pWeaponBase != nullptr) &&
			(pThrowInfo[ i]._bSend) )
		{
			UINT16 idx = (UINT16) ((BattleSlotContext::i()->getMySlotIdx() * MAX_THROWWEAPON_SLOP) + i);

			I3TRACE("GRENADE: %d\n", i);

			// Build Packet
			UINT16 prevPos = nSendPos;
			nSendPos += sizeof( N_GAME_SUBHEAD);

			nSendPos += pThrowInfo[i]._tNetData.Pack( &pOutBuffer[nSendPos], ui32BufSize-nSendPos );
			UDP_UTIL::BuildSubHead( &pOutBuffer[ prevPos], P2P_SUB_HEAD_GRENADE, idx, nSendPos - prevPos);

			// 후 처리
			// 현재는 MulticastSystem의 코드와 같습니다.
			// 차후에 따른 Interface로 빼는 작업이 필요합니다.
			UINT8 tState = pThrowInfo[ i]._tNetData.GetState();

			if( tState & NET_GRENADE_STATE_BOUND)
			{
				// 패킷을 만들고 다시 플라잉 상태로 변경
				tState		&= ~NET_GRENADE_STATE_BOUND;
				tState		|= NET_GRENADE_STATE_FLY;
			}

			// 터진 상태는 이제 필요가 없게 됩니다. 초기화
			// 터진 상태를 UDP로 보내기에 여러번 보냅니다.
			if( pThrowInfo[ i]._tAssistData._SendCount <= 0)
			{
				// 패킷을 만들고 슬립모드로 전환, 안그러면 계속 보내게됩니다.
				if( tState & NET_GRENADE_STATE_CLICKFIRE)
				{
					tState		&= ~NET_GRENADE_STATE_CLICKFIRE;
					tState		|= NET_GRENADE_STATE_SLEEP;
				}
				if( tState & NET_GRENADE_STATE_C5_SETTLED)
				{
					tState		&= ~NET_GRENADE_STATE_C5_SETTLED;
					tState		|= NET_GRENADE_STATE_SLEEP;
				}
				if( tState & NET_GRENADE_STATE_SETTLED )
				{
					tState		&= ~NET_GRENADE_STATE_SETTLED;
					tState		|= NET_GRENADE_STATE_SLEEP;
				}

				if( tState & NET_GRENADE_STATE_EXPLOSION )
				{
					//스모크는 수동으로 삭제한다.
					if(	!i3::kind_of<WeaponSmoke*>(pThrowInfo[ i]._pWeaponBase) )
						pThrowInfo[ i].Reset();
				}

				pThrowInfo[ i]._bSend = false;
			}
			else
			{
				pThrowInfo[ i]._tAssistData._fSendCountElapsedTime += g_pViewer->getDeltaTime();
				if( pThrowInfo[ i]._tAssistData._fSendCountElapsedTime > 0.1f)
				{
					pThrowInfo[ i]._tAssistData._SendCount--;
					pThrowInfo[ i]._tAssistData._fSendCountElapsedTime = 0.f;
				}
			}

			pThrowInfo[ i]._tNetData.SetState(tState);
		}
	}

	/// Droped Weapon
	for( i = 0; i < g_pWeaponManager->GetDropListCount(); i++)
	{
		WeaponBase * pWeapon = g_pWeaponManager->GetDropWeapon( i);

		if( pWeapon->getNetworkIdx() != -1)
		{
			I3_BOUNDCHK( pWeapon->getNetworkIdx(), MAX_DROPEDWEAPON_COUNT);

			if( pDropedInfo[ pWeapon->getNetworkIdx()]._bSend)
			{
				UINT16 prevPos = nSendPos;
				nSendPos += sizeof( N_GAME_SUBHEAD);
				nSendPos += pDropedInfo[ pWeapon->getNetworkIdx()]._tNetData.Pack( &pOutBuffer[ nSendPos], ui32BufSize-nSendPos );
				UDP_UTIL::BuildSubHead( &pOutBuffer[ prevPos], P2P_SUB_HEAD_DROPEDWEAPON, (UINT16) pWeapon->getNetworkIdx(), nSendPos - prevPos);

				// process after send
				switch( pDropedInfo[ pWeapon->getNetworkIdx()].getState() )
				{
				case  NET_WEAPONDROPED_REMOVE :
					if( CStageInfoUtil::IsDeletableDroppedWeapon(*MyRoomInfoContext::i()->getStage()))
					{// 없어지는 무기입니다.
						pDropedInfo[ pWeapon->getNetworkIdx()].Reset();
					}
					break;
				case NET_WEAPONDROPED_START :
				case NET_WEAPONDROPED_FLY :
					break;
				case NET_WEAPONDROPED_BOUND :
					// 바운드 정보를 보내고 다시 플라잉 상태로 바꿔줍니다.
					pDropedInfo[ pWeapon->getNetworkIdx()].setState( NET_WEAPONDROPED_FLY);
					break;
				case NET_WEAPONDROPED_SLEEP :
					break;
				}

				pDropedInfo[ pWeapon->getNetworkIdx()]._bSend = false;
			}
		}
	}

	return nSendPos;
}

void UDPPacketSender::WriteMyGameInfo( const GAMEINFO_CHARACTER * pGameInfo, const GAMEINFO_THROW_WEAPON * pThrowInfo,
									  const GAMEINFO_DROPED_WEAPON * pDropedInfo, REAL32 tm, UINT8 iRoundNum )
{
	bool bSend = false;

	#if defined( USE_CLIENT_SENDCOUNT)
		m_rSendTime += tm;
		if( m_rSendTime < UDPSEND_TIME)
			return;
		m_rSendTime = 0.f;
	#endif

	UINT16 nSendPos = (UINT16) sizeof( N_GAME_HEADER );

	if( pGameInfo->_iPing)
	{
		i3mem::Copy( &m_SendBuffer[ nSendPos ], &pGameInfo->_rPingSendTime, sizeof(REAL32));//추후 수정합니다. 
		nSendPos += sizeof(REAL32);

	#if defined( UDPSEND_TRACE)
		I3TRACE( "Send Ping\n");
	#endif
		
		bSend = true;
	}

	// Dedicated server code
	if( BattleServerContext::i()->IsUseDedicationMode() )
	{
		m_DediSendTime += tm;
		if( bSend ||
			(!bSend && m_DediSendTime > UDPSEND_TIME) )
		{
			UINT16 Rv = __DedicatedService( &m_SendBuffer[ nSendPos ], UDP_SEND_PACKET_SIZE-nSendPos, const_cast<GAMEINFO_THROW_WEAPON*>(pThrowInfo), const_cast<GAMEINFO_DROPED_WEAPON*>(pDropedInfo) );
			if( Rv > 0)
			{
				//I3TRACE("RV : %d, SendPos : %d, TIME: %f\n", Rv, nSendPos, UserContext::i()->fMyGameTime);
				nSendPos += Rv;
				bSend = true;

				m_DediSendTime = 0.f;
			}
		}
	}

	//1. Send할 필요가 있는지 검사합니다. 
	if( (pGameInfo->_bSendFlag) || bSend)
	{
		bSend = true;

		//2. Send해야할 데이터를 검출합니다. 
		//이후 패킷 로딩 순서 중요합니다. 

	#if defined( UDPSEND_TRACE)
		I3TRACE( "------------------- Send Packet -------------------\n");
	#endif

		UINT16 nHeadPos = nSendPos;

		// SubHead를 추가합니다.
		nSendPos += sizeof( N_GAME_SUBHEAD);
		nSendPos += const_cast<GAMEINFO_CHARACTER*>(pGameInfo)->_tNetData.Pack( &m_SendBuffer[ nSendPos], UDP_SEND_PACKET_SIZE-nSendPos );

		// Sub head 구성
		UDP_UTIL::BuildSubHead( &m_SendBuffer[ nHeadPos], P2P_SUB_HEAD_USER,
			(UINT16) BattleSlotContext::i()->getMySlotIdx(), (UINT16) (nSendPos - nHeadPos));
	}

#if defined( TRACE_UDP_COUNT)
	static UINT32 s_SendCount = 0;
	static REAL32 s_SendTime = 0.f;
#endif

	if( bSend)
	{
		// Set header
		//yjha 
		UINT8 ui8MySlotIdx = (UINT8) BattleSlotContext::i()->getMySlotIdx();
		UINT8 ui8MyRespawnCnt = (UINT8) BattleSlotContext::i()->getNetCharaInfo(ui8MySlotIdx)->getMyRespawnCount();
		UDP_UTIL::BuildHead( &m_SendBuffer[ 0], UDPPROTOCOL_CN_INFO, ui8MySlotIdx,	BattleSyncTimer::i()->GetTimeElapsed(), 
			iRoundNum, nSendPos, (bool)(pGameInfo->_iPing>0), 0, ui8MyRespawnCnt, 
			(UINT8)UserInfoContext::i()->GetMyTouchDownCnt() );

		//Send
		_SendToMainClient(nSendPos); 

		// 정보를 보내고 버퍼를 초기화,
		// 안 그러면 다음 프레임에서 계속 보내려고 할 것입니다.
		const_cast<GAMEINFO_CHARACTER*>(pGameInfo)->ResetSendCount();

#if defined( TRACE_UDP_COUNT)
		s_SendCount++;
#endif
	}

#if defined( TRACE_UDP_COUNT)
	s_SendTime += tm;
	if( s_SendTime > 1.f)
	{
		if( s_SendCount > 0)
		{
			I3TRACE( "Send Count : %d\n", s_SendCount);
			s_SendCount = 0;
		}

		s_SendTime = 0.f;
	}
#endif
}

void UDPPacketSender::SendDedication_Ping(REAL32 rClientTime)
{
	ClientUDPSocket * pUDPSocket	= CGameNetworkManager::i()->GetUDPSocket();

	N_GAME_HEADER * pHeader = (N_GAME_HEADER *)&m_SendBuffer[0]; 
	pHeader->_ProtocolID = UDPPROTOCOL_DSERVER_PING;	
	pHeader->_GameTime	= rClientTime;		
	pHeader->_Size		= sizeof(N_GAME_HEADER);
	
	pUDPSocket->SendBattleServer(m_SendBuffer, pHeader->_Size, SLOT_DEDICATION_IDX);

	return; 
}

bool UDPPacketSender::_SendToMainClient(UINT32 iSendSize)
{
	if( iSendSize >= UDP_SEND_PACKET_SIZE )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Overflow Send Packet!!!%d", iSendSize);
	}

	ClientUDPSocket * pUDPSocket	= CGameNetworkManager::i()->GetUDPSocket();
	INT32 nMainSlotIdx				= BattleSlotContext::i()->getMainSlotIdx();

	if( BattleServerContext::i()->IsUseBattleServer() )
	{
		if( UDP_STATE_DONE == NetworkContext::i()->relayServerState)
		{
            pUDPSocket->SendBattleServer(m_SendBuffer, iSendSize, (UINT8)nMainSlotIdx);
		}
	}
	else
	{
		return false;
	}

	return true;
}

