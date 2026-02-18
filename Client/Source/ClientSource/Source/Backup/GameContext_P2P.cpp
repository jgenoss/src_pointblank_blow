#include "pch.h"
#include "GameContext.h"
#include "GameNetwork.h"
#include "StageBattle.h"
#include "StageObject/ObjectWithWeapon.h"
#include "WeaponOnObject.h"
#include "GameObject.h"
#include "GameCharaContexts.h"

#define	UDPCASTTIME_GRENADE			0.2f		// Grenade 초당 5회
#define UDPCASTTIME_GRENADESHELL	0.1f		// GrenadeShell 초당 10회
#define UDPCASTTIME_SLEEPGRENADE	0.5f		// Sleep된 Grenade 초당 2회
#define UDPCASTTIME_SLEEPC5			1.f			// Sleep된 상태의 C5는 초당 1회만 맞춥니다.
#define UDPCASTTIME_C5				0.1f		// C5 초당 10회

#define UDPCASTTIME_OBJECT_ANIM		0.2f		// 이 시간 간격으로 보내집니다.(최대로 많이 보내는 경우)
#define UDPCASTTIME_OBJ_SYNC_TIME	0.07f		// Object 싱크 타임 (물리 오브젝트용)
#define UDPCASTTIME_OBJ_SYNC_MINTIME	5.f		// Object 최소한 싱크를 맞춥니다.

//#define UDPTIMEOUT_CS			10.f			//UDP를 못 받으면 MS에 알린다. ( CS )
//#define UDPTIMEOUT_CN			7.f				//UDP를 못 받으면 MS에 알린다. ( CN )
#define UDPTIMEOUT_CS			20.f			//UDP를 못 받으면 MS에 알린다. ( CS )
#define UDPTIMEOUT_CN			17.f				//UDP를 못 받으면 MS에 알린다. ( CN )

#define UDPSENDTIME_SYNCPOS			0.0666f		//0.06666f	// Character Pos 초당 15회
#define UDPSENDTIME_NOMOVE			1.0f		// 최소 싱크타임 초당 1회
#define UDPSENDTIME_MISSION			0.2f		// Mission Packet 초당 5회

#define UDP_PINGSENDTIME		5.f				// CS -> MS 타임

/***********************************************************************************/

void CGameContext::WriteGameEvent_Fire( CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon)
{
	I3ASSERT( pWeapon != NULL);

	if( pChara->isCharaStateMask( CHARA_STATE_DEATH))	return;

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	pSendBuffer->_tWeaponData._nFireOrder		= pWeapon->getFireOrder();
	pSendBuffer->_tWeaponData._nWeaponClassType	= pWeaponInfo->GetTypeClass();
	pSendBuffer->_tWeaponData._nWeaponNumber	= pWeaponInfo->GetNumber();

	i3Vector::Copy( &pSendBuffer->_tFireData._vStartPos, pChara->GetPos() );
	i3Vector::Copy( &pSendBuffer->_tFireData._vTargetPos, pTargetPos );
	pSendBuffer->_tFireData._nRand		= g_pRand->GetCode();
	pSendBuffer->_tFireData._nId++;

#if defined( CHECK_LOADEDBULLET)
	pSendBuffer->_tFireData._LoadedBullet	= (UINT8) pWeapon->getLoadedBulletCount();
	pSendBuffer->_tFireData._LoadedDualBullet = (UINT8) pWeapon->getLoadedBulletDualCount();
#endif

	if( isNetThrowWeapon( &pSendBuffer->_tWeaponData))
	{
		bool bCheck = true;
		if( pSendBuffer->_tFireData._nNetSlotIdx < MAX_THROWWEAPON_SLOP * slotIdx )
			pSendBuffer->_tFireData._nNetSlotIdx = (UINT16)(MAX_THROWWEAPON_SLOP * slotIdx );

		INT32 nMaximum = pSendBuffer->_tFireData._nNetSlotIdx +1;
		
		while( bCheck )
		{
			if( nMaximum == pSendBuffer->_tFireData._nNetSlotIdx)
				return;

			pSendBuffer->_tFireData._nNetSlotIdx++;
			if( pSendBuffer->_tFireData._nNetSlotIdx >= MAX_THROWWEAPON_SLOP * (slotIdx+1) )
				pSendBuffer->_tFireData._nNetSlotIdx = (UINT16)(MAX_THROWWEAPON_SLOP * slotIdx);

			GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = getGameInfo_ThrowWeapon( pSendBuffer->_tFireData._nNetSlotIdx);
			if( pThrowWeaponInfo->_pWeaponBase != NULL)		return;		// 이미 사용중이라면(빈 버퍼가 없으면 스킵합니다.)
			else											bCheck = false;
		}

		I3TRACE( "Fire throw. %d\n", pSendBuffer->_tFireData._nNetSlotIdx);
	}

	// 여러번 보내줍니다.
	{
		if( (pWeaponInfo->GetTypeClass() == WEAPON_CLASS_KNIFE) &&
			(pWeaponInfo->GetRealNumber() == GetItemIndex("KNIFE_MINIAXE_DUMMY")) )
		{
			if( pSendBuffer->_tWeaponData._nFireOrder == WEAPON_SECONDARY_FIRE)
			{
				m_AssistInfo[ slotIdx]._nThrowState		= GRENADE_THROW_SEND;
				m_AssistInfo[ slotIdx]._nThrowSlotIdx	= pWeaponInfo->GetTypeUsage();

				if( slotIdx == m_MySlotIdx)
				{
					m_AssistInfo[ slotIdx]._uThrowCount	= ((((m_AssistInfo[ slotIdx]._uThrowCount & 0x00000F00) >> 8) + 1) << 8) | (0xFFFFF0FF & i3Math::Rand());
					if( ((m_AssistInfo[ slotIdx]._uThrowCount & 0x00000F00) >> 8) > FIREWALL_GRENADE_COUNT)
					{
						HackingDetected(410.f);
					}
				}
			}
		}
		if( pWeaponInfo->isThrowingClass() ||
			((pWeaponInfo->GetTypeClass() == WEAPON_CLASS_BOMBTRIGGER) &&
			(pSendBuffer->_tWeaponData._nFireOrder == WEAPON_SECONDARY_FIRE)))
		{	//  자신은 수류탄을 던젔음  을 체크..
			m_AssistInfo[ slotIdx]._nThrowState		= GRENADE_THROW_SEND;
			m_AssistInfo[ slotIdx]._nThrowSlotIdx		= pWeaponInfo->GetTypeUsage();

			if( slotIdx == m_MySlotIdx)
			{
				m_AssistInfo[ slotIdx]._uThrowCount	= ((((m_AssistInfo[ slotIdx]._uThrowCount & 0x00000F00) >> 8) + 1) << 8) | (0xFFFFF0FF & i3Math::Rand());
				if( ((m_AssistInfo[ slotIdx]._uThrowCount & 0x00000F00) >> 8) > FIREWALL_GRENADE_COUNT)
				{
					HackingDetected(410.f);
				}
			}
		}
	}

	pSendBuffer->_iInfoFlag		|= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_FIRE | P2P_INFO_FLAG_SYNCWEAPON);
	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
}


void CGameContext::WriteGameEvent_Hit( CGameCharaBase * pChara, CHARA_DAMAGEINFO * pDamageInfo)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig->m_bNetworking == false) return;
#endif

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index.\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	if( pDamageInfo->_HitType == GHT_CHARA)
	{
		BOUNDCHK( pDamageInfo->_nVictimIdx, SLOT_MAX_COUNT);
		BOUNDCHK( pDamageInfo->_HitPart, CHARA_HIT_MAXCOUNT);
	}

	INT32 idx = pSendBuffer->_tFireData._tHitData.getHitCount();

	pSendBuffer->_tFireData._tHitData.setHitType( idx, pDamageInfo->_HitType);
	pSendBuffer->_tFireData._tHitData.setHitWho( idx, (UINT8) pDamageInfo->_nVictimIdx);
	pSendBuffer->_tFireData._tHitData.setHitPart( idx, (UINT8) pDamageInfo->_HitPart);

	REAL32 rDamage = MINMAX( 0.f, pDamageInfo->_rDamage, 255.f);
	pSendBuffer->_tFireData._tHitData.setHitDamage( idx, (UINT8) rDamage);
	
	idx++;
	I3ASSERT( idx <= MAX_FIREHIT_COUNT);

	pSendBuffer->_tFireData._tHitData.setHitCount( idx);
	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_HIT;

	m_CharacterGameInfo[ slotIdx]._bSendFlag = TRUE;

#if defined( USE_HOST_MODULE)
	if( IsP2PHost())
	{
		CGameCharaBase * pVictimUser = g_pCharaManager->getCharaByNetIdx( pDamageInfo->_nVictimIdx);
		g_pFramework->getHostModule()->ProcessHit( pVictimUser, pDamageInfo, rDamage);
	}
#endif
}

void CGameContext::WriteGameEvent_DropWeapon( CGameCharaBase * pChara, INT32 iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet)
{
	WeaponBase * pWeapon = NULL;

	if( iWeaponSlot > 1)	return;

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;
	
	if( IsP2PHost() )
	{
		pSendBuffer->_tDropWeaponData.Reset();

		INT32 nDropWeaponIdx = getDropedWeaponCount() % MAX_DROPEDWEAPON_COUNT;
		pWeapon = g_pWeaponManager->SetDropWeapon( slotIdx, iWeaponSlot, nDropWeaponIdx, !pChara->isCharaStateMask( CHARA_STATE_DEATH));

		if( pWeapon != NULL )
		{	// 성공하면 패킷을 세팅합니다.
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
			I3ASSERT( pWeaponInfo != NULL);

			// 드랍된 무기에 대한 정보를 세팅합니다.
			{
				GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( nDropWeaponIdx );
				pDropedWeaponInfo->Bind( pWeapon, pChara);
			}
			
			// 해당 무기에 탄약 정보를 세팅
			{
				pWeapon->SetBulletCountForNetwork( iLoadedBullet, iDualLoadedBullet, iTotalBullet);

				pSendBuffer->_tDropWeaponData._nState	= 1;
				if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
				{	// Death flag 
					pSendBuffer->_tDropWeaponData._nState |= 0x02;
				}

				pSendBuffer->_tDropWeaponData._nIdx		= (UINT8) nDropWeaponIdx;
			}

			nDropWeaponIdx++;
			SetDropedWeaponCount( (UINT16) nDropWeaponIdx);
		}
		else
		{
			for( INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT; i++)
			{
				GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( i );

				if( pDropedWeaponInfo->_pWeaponRef == NULL)												continue;
				if( pDropedWeaponInfo->_pWeaponRef->getOwner() != pChara )								continue;
				if( pDropedWeaponInfo->_pWeaponRef->getWeaponInfo()->GetTypeUsage() != iWeaponSlot )	continue;
				if( pDropedWeaponInfo->_pWeaponRef->IsDroped() == true)									continue;
				
				pSendBuffer->_tDropWeaponData._nState	= 1;
				if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
				{	// Death flag
					pSendBuffer->_tDropWeaponData._nState |= 0x02;
				}
				pSendBuffer->_tDropWeaponData._nIdx		= (UINT8) i;

				pWeapon = pDropedWeaponInfo->_pWeaponRef;
				break;
			}

			if( pWeapon == NULL)	return;
		}
	}
	else
	{
		pWeapon = pChara->GetCharaWeapon( iWeaponSlot );
		if( pWeapon == NULL )									return;
		if( pWeapon->getWeaponInfo()->isEnableDrop() == false)	return;

		pSendBuffer->_tDropWeaponData._nIdx			= (UINT8) 0;
		pSendBuffer->_tDropWeaponData._nState		= 1;
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking) return;
#endif

	pSendBuffer->_tDropWeaponData._nSlot				= (UINT8) iWeaponSlot;								// slotnumber
	pSendBuffer->_tDropWeaponData._nClassType			= (UINT8) pWeapon->getWeaponInfo()->GetTypeClass();
	pSendBuffer->_tDropWeaponData._nNumber				= (UINT8) pWeapon->getWeaponInfo()->GetNumber();
	pSendBuffer->_tDropWeaponData._nLoadedBullet		= (UINT8) pWeapon->getLoadedBulletCount();
	pSendBuffer->_tDropWeaponData._nDualLoadedBullet	= (UINT8) pWeapon->getLoadedBulletDualCount();
	pSendBuffer->_tDropWeaponData._nMaxBullet			= (UINT16) pWeapon->getTotalBulletCount();
	pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_DROPWEAPON;

	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
}

void CGameContext::WriteGameEvent_PickupWeapon( CGameCharaBase * pChara, WeaponBase * pWeapon)
{
#if defined (WEAPON_TAKE_DISABLE)
	//	적군 무기 조달 캐시 아이템
	INT32 cashItemFlag = MAKE_ITEM_FLAG(ITEM_CLASS_MAINTENANCE, CASHITEM_USAGE_MAINTENANCE, CASHITEM_GROUP_GET_DROPPED_WEAPON, CASHITEM_DAY_NO);

	//	아이템 사용중이 아니면 (사용중이면 모든 무기를 주울수 있다)
	if( (pChara == g_pPlayer) && !g_pFramework->IsUsingCashItem( cashItemFlag))
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

		//	다른 무기는 주울수 없다. (RPG7은 예외 처리)
		if( !pWeaponInfo->isSame(WEAPON_CLASS_ROCKET_LAUNCHER, g_pWeaponManager->getItemIndex_RPG7_OutPost()) &&
			!pWeaponInfo->isSame(WEAPON_CLASS_ROCKET_LAUNCHER, GetItemIndex("ROCKET_LAUNCHER_RPG7")))
		{
		#if defined( WEAPON_TAKE_BYTEAM)
			if( pWeapon->getDropInfo()->_nUserTeam != getMyTeam() )
				return;
		#else
			return;	//	무기를 주울수 없다.
		#endif
		}
	}
#endif

	if( IsP2PHost() )
	{
		pChara->Cmd_PickupWeapon( pWeapon);

		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( pWeapon->getNetworkIdx() );
		pDropedWeaponInfo->Reset();
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking) return;
#endif

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	pSendBuffer->_tGetWeaponData._nState			= 1;
	pSendBuffer->_tGetWeaponData._nIdx				= (UINT8) pWeapon->getNetworkIdx();
	pSendBuffer->_tGetWeaponData._nSlot				= (UINT8) pWeapon->getWeaponInfo()->GetTypeUsage();
	pSendBuffer->_tGetWeaponData._nClassType		= (UINT8) pWeapon->getWeaponInfo()->GetTypeClass();
	pSendBuffer->_tGetWeaponData._nNumber			= (UINT8) pWeapon->getWeaponInfo()->GetNumber();
	pSendBuffer->_tGetWeaponData._nLoadedBullet		= (UINT8) pWeapon->getLoadedBulletCount();
	pSendBuffer->_tGetWeaponData._nDualLoadedBullet	= (UINT8) pWeapon->getLoadedBulletDualCount();
	pSendBuffer->_tGetWeaponData._nMaxBullet		= (UINT16) pWeapon->getTotalBulletCount();
	pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_GETWEAPON;

	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
}

void CGameContext::WriteGameEvent_Mission( CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking) return;
#endif

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	if( pSendBuffer->_tMissionData._nState != nState)
	{
		pSendBuffer->_tMissionData._nState	= (UINT8) nState;
		pSendBuffer->_tMissionData._nArea	= nArea;
		pSendBuffer->_iInfoFlag	|= P2P_INFO_FLAG_MISSION;

		m_CharacterGameInfo[ slotIdx]._rMissionSyncSendTime	= 0.f;
		m_CharacterGameInfo[ slotIdx]._bSendFlag			= TRUE;
	}
}

void CGameContext::WriteGameEvent_SucideDamage( CGameCharaBase * pChara, UINT8 nDamage, UINT8 iByObjectIdx /*= 0xFF*/)
{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//	if( !g_pConfig->m_bNetworking) return;
//#endif

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	pSendBuffer->_tSuicideDamageData._nDamage				= nDamage;
	pSendBuffer->_tSuicideDamageData._nObjectIdx			= iByObjectIdx;
	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SUICIDEDAMAGE;

	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;


#if defined( USE_HOST_MODULE)
	if( IsP2PHost())
		g_pFramework->getHostModule()->ProcessSelfDamage( pChara, nDamage, iByObjectIdx);
#endif
}

void CGameContext::WriteGameEvent_RadioChat( CGameCharaBase * pChara, UINT8 nRadio)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking) return;
#endif
	
	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	pSendBuffer->_tDefData._nRadio	= nRadio;
	pSendBuffer->_tDefData._nSectorIdx = (UINT8) pChara->getPortalID();
	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_RADIO;

	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
}

void CGameContext::WriteGameEvent_UseObject( CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey)
{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//	if( !g_pConfig->m_bNetworking) return;
//#endif

	BOUNDCHK( pObj->getNetIndex(), 256);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	pSendBuffer->_tDefData._nActionKey = nActionKey;
	pSendBuffer->_tDefData._nUseActionObjectIdx = (UINT8) pObj->getNetIndex();
	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;

	m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;

#if defined( USE_HOST_MODULE)
	if( IsP2PHost())
		g_pFramework->getHostModule()->ProcessObjectUse( pChara, pObj, true);
#endif
}

void CGameContext::WriteGameEvent_UseObjectSync( CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey)
{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//	if( !g_pConfig->m_bNetworking) return;
//#endif

	// Client는 처리하지 않습니다.
	if( !IsP2PHost())
		return;

	BOUNDCHK( pObj->getNetIndex(), 256);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	pSendBuffer->_tDefData._iSyncObjectIdx		= (UINT8) pObj->getNetIndex();
	pSendBuffer->_tDefData._nSyncObjectState	= nSyncActionKey;
	pSendBuffer->_iInfoFlag	|= P2P_INFO_FLAG_SYNCOBJECTSTATE;

	m_CharacterGameInfo[ slotIdx]._rObjectSyncSendTime	= 0.f;
	m_CharacterGameInfo[ slotIdx]._bSendFlag				= TRUE;
}

void CGameContext::WriteGameEvent_SufferingDamage( CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart)
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking) return;
#endif

	I3ASSERT( IsP2PHost() == TRUE);

	BOUNDCHK( nShooter, SLOT_MAX_COUNT);
	BOUNDCHK( nHitPart, CHARA_HIT_MAXCOUNT);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if( slotIdx == -1)
	{
		I3WARN( "invalid character index..\n");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;
	
	pSendBuffer->_tSufferData._nDeathType	= (UINT8) type;
	pSendBuffer->_tSufferData._nShooter	= (UINT8) nShooter;
	pSendBuffer->_tSufferData._nHitpart	= (UINT8) nHitPart;
	pSendBuffer->_tDefData.setHP( (UINT16) pChara->getCharaInfo()->GetHP());
	pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_CS_LIFE | P2P_INFO_FLAG_CS_SUFFERING);

	m_CharacterGameInfo[ slotIdx]._rLifePacketSendTime	= 0.f;
	m_CharacterGameInfo[ slotIdx]._bSendFlag = TRUE;
}

void CGameContext::WriteGameEvent_ThrowWeaponData( INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo )
{
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking )		return;
#endif

	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE) )
		return;

	if( idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = getGameInfo_ThrowWeapon( idx );

	// 초기화된 수류탄입니다. 보내지 않습니다. 2008.06.19 dfly79
	if( pGrenadeInfo->_pWeaponBase == NULL)
		return;

	BOOL	bCheckSend = FALSE;
	UINT8	nStateFlag = pGrenadeInfo->_tNetData._nState | pThrowWeaponInfo->_nAddStateFlag;

	pGrenadeInfo->_fSendTime += g_pViewer->getDeltaTime();

	if( !(pThrowWeaponInfo->_nAddStateFlag & NET_GRENADE_STATE_FLY) )
	{	// 현재 보내려는 타입이 싱크용이 아니면 바로 보냅니다.
		bCheckSend = TRUE;

		//if( pThrowWeaponInfo->_nAddStateFlag & NET_GRENADE_STATE_EXPLOSION)
		{	// 폭파 flag는 send count를 지정합니다.
			pGrenadeInfo->_tAssistData._SendCount = 2;	// 총 3회 보냅니다.
		}
	}
	else if( nStateFlag & NET_GRENADE_STATE_SLEEP)
	{	// 현재 보내려는 타입이 싱크용이지만 슬립된 상태라면 횟수를 조절합니다.
		if( pGrenadeInfo->_fSendTime > pGrenadeInfo->_fBroadcastCheckSleepTime)
			bCheckSend = TRUE;
	}
	else
	{	// 싱크용으로 보내는 횟수 조절
		if( pGrenadeInfo->_fSendTime > pGrenadeInfo->_fBroadcastCheckSyncTime)
			bCheckSend = TRUE;
	}

	if( bCheckSend )
	{
		pGrenadeInfo->_bSend = true;
		pGrenadeInfo->_fSendTime = 0.f;
		pGrenadeInfo->_tNetData._nState = nStateFlag;

		if( pThrowWeaponInfo->_nAddStateFlag & NET_GRENADE_STATE_BOUND || (pThrowWeaponInfo->_nAddStateFlag & NET_GRENADE_STATE_TAKEOBJECT))
		{
			pGrenadeInfo->_tNetData._nBoundType = (UINT8) pThrowWeaponInfo->_nBoundType;
		}

		if( pThrowWeaponInfo->_pPos != NULL)
		{
			i3Vector::Copy( &pGrenadeInfo->_tNetData._vPos, pThrowWeaponInfo->_pPos );
		}
		if( pThrowWeaponInfo->_pNormal != NULL)
		{
			i3Vector::Copy( &pGrenadeInfo->_tNetData._vDir,	pThrowWeaponInfo->_pNormal);
		}
	}
}


void CGameContext::WriteGameEvent_ObjectData( INT32 idx, UINT16 nHP, VEC3D * pPos, QUATERNION * pRot )
{
	BOOL bSend = FALSE;
	if( m_pMulticast == NULL )	return;

	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = getGameInfo_Object( idx );

	if( pObjInfo->_tNetData._nHP != nHP)
	{
		pObjInfo->_tNetData._nHP		= nHP;
		bSend = TRUE;
	}

	if( (pObjInfo->_pObjectRef != NULL) && (pObjInfo->_tNetData._nHP != 0) )
	{	// 마지막 스테이트 상태라면 HP를 0으로 강제합니다. 2008. 11. 28
		if( pObjInfo->_pObjectRef->getCurState() == (pObjInfo->_pObjectRef->getStateCount() - 1) )
		{
			pObjInfo->_tNetData._nHP = 0;
		}
	}

	switch( pObjInfo->_nType )
	{
	case I3_NETWORK_GAMEOBJECT_TYPE_NONE :		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_STATIC :
		pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
		if( pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME)
			bSend = TRUE;
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_MOVE :
		{
			if( pPos == NULL )
			{
				I3TRACE( "[GameContext::WriteGameEvent_ObjectData()] move position is NULL.\n" );
				return;
			}
			i3Vector::Copy( &pObjInfo->_tNetData._vPos, pPos );

			pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
			if( (pObjInfo->_pObjectRef->isEnable() && pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_TIME) ||
				(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME))
				bSend = TRUE;
		}
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC :
		{
			if( pPos == NULL )
			{
				I3TRACE( "[GameContext::WriteGameEvent_ObjectData()] dynamic position is NULL.\n" );
				return;
			}

			if( pRot == NULL )
			{
				I3TRACE( "[GameContext::WriteGameEvent_ObjectData()] dynamic rotate is NULL.\n" );
				return;
			}
			i3Vector::Copy( &pObjInfo->_tNetData._vPos, pPos );
			i3Quat::Copy( &pObjInfo->_tNetData._qRot, pRot );

			pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
			if( (pObjInfo->_pObjectRef->isEnable() && pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_TIME) ||
				(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME))
				bSend = TRUE;
		}
		break;
	default :
		I3FATAL( "[GameContext::WriteGameEvent_ObjectData()] - invalid Object type." );
		break;
	}

	if( bSend)
	{
		pObjInfo->_bBroadcast			= TRUE;
		pObjInfo->_fBroadcastTime		= 0.f;
	}
}

void CGameContext::WriteGameEvent_AnimObjectData( INT32 idx, UINT16 nHP, REAL32 rKeyFrame)
{
	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = getGameInfo_Object( idx );

	if( pObjInfo->_nType != I3_NETWORK_GAMEOBJECT_TYPE_ANIM )
	{
		I3TRACE( "[GameContext::WriteGameEvent_AnimObjectData()] - invalid object type..\n" );
		return;
	}

	{
		pObjInfo->_tNetData._nType		= 0;
		pObjInfo->_tNetData._nState		= 0;
		pObjInfo->_tNetData._nHP		= nHP;
		pObjInfo->_tNetData._fKey		= rKeyFrame;

		pObjInfo->_bBroadcast			= TRUE;
		pObjInfo->_fBroadcastTime		= 0.f;
	}
}

void CGameContext::WriteGameEvent_NPCData( INT32 idx, UINT8 nState, UINT8 nHP, VEC3D * pDir, INT16 nHitPart)
{
	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_NPC * pNPCInfo = getGameInfo_NPC( idx);

	pNPCInfo->_nState		= nState;
	pNPCInfo->_nHP			= nHP;
	pNPCInfo->_nHitUserPart = nHitPart;

	if( pDir != NULL)
	{
		i3Vector::Copy( &pNPCInfo->_vTargetPos, pDir);
	}
	pNPCInfo->_bBroadcast	= TRUE;
}

void CGameContext::WriteGameEvent_DropedWeaponData( INT32 idx, VEC3D * pPos, MATRIX * pMtxRot)
{
	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( idx);

	i3Vector::Copy( &pDropedWeaponInfo->_tNetData._vPos, pPos);
	i3Matrix::GetRotateQuat( &pDropedWeaponInfo->_tNetData._qRot, pMtxRot );

	pDropedWeaponInfo->_bSend = true;
}


void CGameContext::BindNetGrenadeInfo( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	I3ASSERT( pWeapon != NULL);
	if( idx < 0 || idx >= MAX_THROWWEAPON_COUNT )	return;
	GAMEINFO_THROW_WEAPON * pGrenadeInfo = getGameInfo_ThrowWeapon( idx);

	REAL32 syncTime;

	if( pWeapon->IsExactTypeOf( WeaponGrenadeShell::GetClassMeta()) ||
		pWeapon->IsExactTypeOf( WeaponRocketLauncherShell::GetClassMeta()) ||
		pWeapon->IsExactTypeOf( WeaponThrowKnife::GetClassMeta()) )					// 쓰로잉 나이프류
	{
		syncTime = UDPCASTTIME_GRENADESHELL;	// 초당 10회 보냅니다.
	}
	else
	{
		syncTime = UDPCASTTIME_GRENADE;			// 그 외 수류탄류는 초당 5회
	}

	pGrenadeInfo->Bind( pWeapon, UDPCASTTIME_SLEEPGRENADE, syncTime, pInitPos);

	I3TRACE( "Bind grenade info. %d %d %x\n", idx, pWeapon->getNetworkIdx(), pWeapon );
}

void CGameContext::BindNetC5Info( INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	I3ASSERT( pWeapon != NULL);
	if( idx < 0 || idx >= MAX_THROWWEAPON_COUNT )	return;
	GAMEINFO_THROW_WEAPON * pC5Info = getGameInfo_ThrowWeapon( idx);

	pC5Info->Bind( pWeapon, UDPCASTTIME_SLEEPC5, UDPCASTTIME_C5, pInitPos);

	I3TRACE( "Bind grenade info. %d %d %x\n", idx, pWeapon->getNetworkIdx(), pWeapon );
}


void CGameContext::WriteGameEvent_AnimObjectDataForOutpost( INT32 idx, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, BOOL bSend )
{
	if( g_pFramework->IsBattleStage() == FALSE) return;
	if( (((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = getGameInfo_Object( idx );

	I3ASSERT( pObjInfo->_nType == I3_NETWORK_GAMEOBJECT_TYPE_ANIM );

	pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
	if( (pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJECT_ANIM) || (pObjInfo->_tNetData._nState != nPathKey) )							// 패스 키가 바뀐경우
	{
		if( (bSend == TRUE) ||
			(pObjInfo->_tNetData._nHP != nHP) ||								// Client도 HP를 받아 게이지를 표시해줘야 합니다.
			(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME) )		// 싱크 타임 5초에 한번 싱크를 맞추도록 합니다.
		{
			pObjInfo->_tNetData._nType		= NET_ANIMOBJECT_TYPE_OUTPOST;		// object net type
			pObjInfo->_tNetData._nState		= nPathKey;
			pObjInfo->_tNetData._nHP		= nHP;
			pObjInfo->_tNetData._fKey		= rKeyFrame;
			pObjInfo->_bBroadcast			= TRUE;
			pObjInfo->_fBroadcastTime		= 0.f;
		}
	}
}



//---------------------------------------------------------------------------------------------//
// P2P Data's Receive and Send
//---------------------------------------------------------------------------------------------//
void CGameContext::_P2PUpdate( RT_REAL32 rDeltatime )
{
#if defined( DEF_OBSERVER_MODE)
	BOUNDCHK( m_MySlotIdx, SLOT_ALL_COUNT);
#else
	BOUNDCHK( m_MySlotIdx, SLOT_MAX_COUNT);
#endif

	m_fMyGameTime += rDeltatime;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Recv 받은 데이터 파싱후 처리까지 완료 합니다.
	_P2PMergeRecvData( rDeltatime );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Send 해야 하는 패킷이 있는지 확인합니다.
	INT32 iStageState = ((CStageBattle*)g_pFramework->GetCurrentStage())->getStageState();
	if((iStageState == GAMESTAGE_STATE_SHOWSCORE) || (iStageState == GAMESTAGE_STATE_ENDGAME )) return;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Send 확인하는 부분은 수정해야함.
#if defined( DEF_OSBERVER_MODE)
	// Observer가 아닌 경우에만 Send
	if( isObserverMe() == false)
#endif
		if( g_pPlayer != NULL)
		{
			_CheckGameDataToSend( g_pPlayer);
		}


	if( IsP2PHost() )
	{//방장일때 처리
		_CheckGameDataToSendForAi();		// 훈련전 AI 캐릭터

		_CheckGameDataToSendForHost();
		_CalcPing( rDeltatime );			// TCP로 게임서버에 UDP패킷의 Ping을 보낸다.
		SendDeathChara();					// TCP GameServer에 DeathEvent를 보냅니다.

		m_broadcastInfo._rDeltatime	= rDeltatime;
		m_broadcastInfo._iRoundNum = getCurrentRound();
		m_pMulticast->Broadcast( &m_broadcastInfo );	// 각 유저에게 패킷을 보낸다.
	}
	else
	{//일반 유저일때 처리
		//패킷 만들기 내부에서 패킷을 보내요.. 나중에 수정합니다.
#if defined( DEF_OBSERVER_MODE)
		// Observer가 아닌 경우에만 Send
		if( isObserverMe() == false)
#endif
			m_pUDPSender->WriteMyGameInfo( &m_CharacterGameInfo[ getMySlotIdx()],
				&m_ThrowWeaponGameInfo[ getMySlotIdx() * MAX_THROWWEAPON_SLOP],
				m_DropedWeaponGameInfo, rDeltatime, getCurrentRound());
	}
}



void CGameContext::_CalcPing( REAL32 rDeltatime)
{
	INT32 i;

	m_PingInfo.m_rPingCheckTime += rDeltatime;
	if( m_PingInfo.m_rPingCheckTime > UDP_PINGSENDTIME)
	{
		//Set Ping Data
		for( i = 0; i < SLOT_MAX_COUNT; i++ )
		{
			if( getSlotInfo( i )->_State != SLOT_STATE_BATTLE )
			{
				m_PingInfo.m_Pings[ i ] = 1;
				continue;
			}
			if( m_PingInfo.m_rPingList[ i ] < g_pConfigEx->GetNetwork().PingLevelCheckValue[0]	)
				m_PingInfo.m_Pings[ i ] = 5;
			else if( m_PingInfo.m_rPingList[ i ] < g_pConfigEx->GetNetwork().PingLevelCheckValue[1]	)
				m_PingInfo.m_Pings[ i ] = 4;
			else if( m_PingInfo.m_rPingList[ i ] < g_pConfigEx->GetNetwork().PingLevelCheckValue[2]	)
				m_PingInfo.m_Pings[ i ] = 3;
			else if( m_PingInfo.m_rPingList[ i ] < g_pConfigEx->GetNetwork().PingLevelCheckValue[3]	)
				m_PingInfo.m_Pings[ i ] = 2;
			else
				m_PingInfo.m_Pings[ i ] = 1;
		}

		//Send Data
		SetEvent( EVENT_SENDPING_N);
		m_PingInfo.m_rPingCheckTime = 0.f;
	}
}


//-------------------------------------------------------------------------------------------//
//	UDP 패킷으로 받은 데이터를 확인하고 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_P2PMergeRecvData( REAL32 rDeltatime )
{
	INT32	i, idx;
	INT32	nRecvCount;
	RECV_P2P_DATA	*			pRecvData;
	GAMEINFO_CHARACTER *		pCharaInfo;
	INT32	nMainSlotIdx = getMainSlotIdx();

	nRecvCount = m_pUDPReceiver->GetRecvCount();

	if( !IsP2PHost() && nRecvCount)
	{	// Client는 Host로부터로만 받게 되므로 받는 경우가 있으면 체크
		m_fUDPConnectTime[ nMainSlotIdx ] = 0.f;
	}

	for( i = 0; i < nRecvCount; i++ )
	{
		pRecvData	= m_pUDPReceiver->ReadGameData();
		idx			= pRecvData->Idx;

		// 2009.06.09 패킷에 라운드값 추가. dfly79
		// 라운드가 다르면 패킷을 무시합니다.
		if( pRecvData->iRoundNum == getCurrentRound() )
		{
			switch( pRecvData->Type )
			{
				// Host or Client
			case INFO_DATA_CHARACTER :
				{	//캐릭터 데이터 입니다.
					BOUNDCHK( idx, SLOT_MAX_COUNT);
					CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( idx);
					if( pChara == NULL )
					{	// 아직 캐릭터를 등록하지 않은 상태에서 받았습니다. 해당 패킷은 무시
						break;
					}

					pCharaInfo = &m_CharacterGameInfo[ idx ];

					if( IsP2PHost() )
					{ // Host는 각 Client로부터 Character 정보를 받으므로 받았다는 표시를 여기서 해줍니다.
						m_fUDPConnectTime[ idx ] = 0.f;
						// 패킷 처리
						_ProcessRecvPacket_Chara_ForHost( idx, pCharaInfo, pRecvData, pChara);

						// Recv Ping //방장이 보낸 핑타임과 확인을 해야 한다. 성백
						if( pRecvData->iPing)
						{
							m_PingInfo.RecvPing( idx, pRecvData->rPingSendTime, getMyTime());
						}
					}
					else
					{
						_ProcessRecvPacket_Chara_ForClient( idx, pCharaInfo, pRecvData, pChara );
					}
				}
				break;

			// for only client
			case INFO_DATA_GRENADE					:	_ProcessRecvPacket_Grenade( idx, pRecvData, rDeltatime);			break;
			case INFO_DATA_OBJECT_STATIC			:	_ProcessRecvPacket_Obj_Static( idx, pRecvData, rDeltatime);		break;
			case INFO_DATA_OBJECT_MOVE				:	_ProcessRecvPacket_Obj_Move( idx, pRecvData, rDeltatime);		break;
			case INFO_DATA_OBJECT_DYNAMIC			:	_ProcessRecvPacket_Obj_Dynamic( idx, pRecvData, rDeltatime);		break;
			case INFO_DATA_OBJECT_ANIM				:	_ProcessRecvPacket_Obj_Anim( idx, pRecvData, rDeltatime);		break;
			case INFO_DATA_DROPEDWEAPON				:	_ProcessRecvPacket_DropedWeapon( idx, pRecvData, rDeltatime);	break;
			case INFO_DATA_STAGEINFO_CHARA			:	_ProcessRecvPacket_StageInfo_Chara( idx, pRecvData);				break;
			case INFO_DATA_STAGEINFO_OBJECT_ANIM	:	_ProcessRecvPacket_StageInfo_Obj_Anim( idx, pRecvData);			break;
			case INFO_DATA_STAGEINFO_OBJECT_STATIC	:	_ProcessRecvPacket_StageInfo_Obj_Static( idx, pRecvData);		break;
			case INFO_DATA_STAGEINFO_OBJECT_DYNAMIC :	_ProcessRecvPacket_StageInfo_Obj_Dynamic( idx, pRecvData);		break;
			case INFO_DATA_STAGEINFO_MISSION		:	_ProcessRecvPacket_StageInfo_Mission( pRecvData);				break;
			case INFO_DATA_NPC						:	_ProcessRecvPacket_NPC( idx, pRecvData, rDeltatime);			break;
			case INFO_DATA_C5						:	_ProcessRecvPacket_C5( idx, pRecvData, rDeltatime);				break;
			case INFO_DATA_STAGEINFO_CONTROLED_OBJECT :	_ProcessRecvPacket_StageInfo_ControledObject( idx, pRecvData);	break;
			default :									{ I3TRACE( "Unknown recv data type...\n"); }				break;
			}

			// Recv Ping for Client
			if( !IsP2PHost() && pRecvData->iPing
#if defined( DEF_OBSERVER_MODE)
				// Observer는 처리하지 않는다.
				&& !isObserverMe()
#endif
				)
			{
				getGameInfo_Chara( getMySlotIdx() )->_iPing				= pRecvData->iPing;
				getGameInfo_Chara( getMySlotIdx() )->_rPingSendTime		= pRecvData->rPingSendTime;
			}
		}

		// Receive Buffer를 초기화 하고 리드 포지션을 증가시켜줍니다.
		pRecvData->Reset();
		m_pUDPReceiver->PlusReadPos();
	}

	// 받은 패킷이 없으면 서버에 알려줘야됩니다.
	_CalcUDPTimeOut( rDeltatime );

	return;
}

void CGameContext::_CalcUDPTimeOut( REAL32 rDeltatime )
{
#if !defined( NO_TIMEOUT )
	char szTrace[ 256] = {0,};

	if( (getSlotInfo( m_MySlotIdx )->_State == SLOT_STATE_BATTLE) && (m_UdpHoleState[m_MySlotIdx] == UDP_STATE_DONE) )
	{//성백 추가.. 게임시작시 네트워크에서 빠져나가는거 방지.
		if( IsP2PHost() )
		{
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);

				if( pChara == NULL)
					continue;

				if( (i != m_MainSlotIdx) && (getSlotInfo( i )->_State == SLOT_STATE_BATTLE) &&
					!pChara->isCharaStateMask( CHARA_STATE_DEATH))	// 캐릭터가 살아 있는 경우에만 체크
				{
					if( IsBombMissionMode()  || IsAnnihilationMode() || IsBossMissionMode())
					{
						if( !IsPlayingSlot(i) ) m_fUDPConnectTime[ i ] = 0.f;					// 미션 모드 일때 난입한 경우
						if( g_pCharaManager->getCharaTask( i) != CHARA_TASK_RESPAWN) m_fUDPConnectTime[ i ] = 0.f; //미션 모드 일때 게임후 죽은 경우
					}

					m_fUDPConnectTime[ i ]	+= rDeltatime;
					if( m_fUDPConnectTime[ i ] > UDPTIMEOUT_CS )
					{
						
						m_fUDPConnectTime[ i ]	= 0.f;
						sprintf( szTrace, "Client UDP timeout idx %d\n", i);
						I3NOTICE( szTrace);
					// yjha
					//	SetEvent( EVENT_M_TIMEOUTCS, &i );
					}
				}
			}
		}
		else
		{
			INT32 nMainSlotIdx = getMainSlotIdx();
			m_fUDPConnectTime[ nMainSlotIdx ]	+= rDeltatime;
			if( m_fUDPConnectTime[ nMainSlotIdx ] > UDPTIMEOUT_CN )
			{//
				
				m_fUDPConnectTime[ nMainSlotIdx ]	= 0.f;
				sprintf( szTrace, "Host UDP timeout idx %d\n", nMainSlotIdx );
				I3NOTICE( szTrace);
				//yjha
				// SetEvent( EVENT_M_TIMEOUTCN );
				return;
			}
		}
	}
#endif
}

//파싱 순서 중요합니다.
BOOL CGameContext::_ProcessRecvPacket_Chara_ForClient( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara )
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *) pRecvData->pArg;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	pCharaInfo->_bRecvFlag				= TRUE;

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		if( !pChara->isInvincible())
		{
			UINT16 hp = pRecvBuf->_tDefData.getHP();

			pCharaNetData->_tDefData.setHP( hp);

			if( (((INT32)(pChara->getCharaInfo()->GetHP())) == hp) && (hp != 0) )
				pChara->SetHelmProtectionFlag( true);
			else
				pChara->SetHelmProtectionFlag( false);

			pChara->getCharaInfo()->SetHP((REAL32) hp);
		}
	}

	if( pChara->isCharaStateMask( CHARA_STATE_DEATH) == FALSE)
	{	// 이미 죽은 상태라면 처리 하지 않는다.

		if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
			_ProcessRecvPacket_Chara_GetWeaponForClient( pRecvBuf, pChara);

		// 기본 데이터
		if( pChara->isNetwork())
		{
			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
			{
				pCharaNetData->_tDefData._bOnLoadObject			= pRecvBuf->_tDefData._bOnLoadObject;
				pCharaNetData->_tDefData._iOnLoadObjectIndex	= pRecvBuf->_tDefData._iOnLoadObjectIndex;
				//_ProcessRecvPacket_Chara_OnLoadObject( pCharaNetData, pRecvBuf, pChara);
			}

			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
			{
				pRecvBuf->_tPosRotData.CopyTo( &pCharaNetData->_tPosRotData);
				_ProcessRecvPacket_Chara_PosRotation( pRecvBuf, pChara);
			}

			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON)
			{
				pRecvBuf->_tWeaponData.CopyTo( &pCharaNetData->_tWeaponData);
				_ProcessRecvPacket_Chara_WeaponSync( pRecvBuf, pChara);
			}

			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
			{
				pCharaNetData->_tDefData._Body		= pRecvBuf->_tDefData._Body;
				pCharaNetData->_tDefData._Body2		= pRecvBuf->_tDefData._Body2;
				pCharaNetData->_tDefData._Upper		= pRecvBuf->_tDefData._Upper;
				pCharaNetData->_tDefData._Lower		= pRecvBuf->_tDefData._Lower;

				pCharaNetData->_tDefData._nFireAction		= pRecvBuf->_tDefData._nFireAction;
				pCharaNetData->_tWeaponData._nFireOrder		= pRecvBuf->_tWeaponData._nFireOrder;
				pCharaNetData->_tDefData._nHelmet			= pRecvBuf->_tDefData._nHelmet;
				pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_CHARASTATE;

				_ProcessRecvPacket_Chara_ActionState( pRecvBuf, pChara);
			}

			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
			{	
				_ProcessRecvPacket_Chara_Suicide( pCharaNetData, pRecvBuf, pChara );
			}

			if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_MISSION)
				_ProcessRecvPacket_Chara_MissionDataForClient( pRecvBuf, pChara);
		}

		if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_RADIO)
		{
			pCharaNetData->_tDefData._nRadio = pRecvBuf->_tDefData._nRadio;
			_ProcessRecvPacket_Chara_RadioChat( pRecvBuf, pChara);
		}

		if( (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE) )
		{
			// Process packet
			if( pChara->isLocal() )		_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient( pCharaNetData, pRecvBuf);		// 자신이 던진 수류탄만 처리
			else
			{
				if(pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireDataOnObject( pCharaNetData, pRecvBuf, pChara->GetOnObject());
				else						_ProcessRecvPacket_Chara_FireData( pCharaNetData, pRecvBuf, pChara);
			}
		}

		if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HIT )
		{
			if( pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireNHitDataOnObject( pRecvBuf, pChara->GetOnObject());
			else						_ProcessRecvPacket_Chara_FireNHitData( pRecvBuf, pChara);
		}
	}

	if( (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DROPWEAPON) )
		_ProcessRecvPacket_Chara_DropWeaponForClient( pRecvBuf, pChara);

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ACTIONKEY)
	{
		_ProcessRecvPacket_Chara_ActionKey( pCharaNetData, pRecvBuf, pChara);

		pCharaNetData->_tDefData._nMoveKey				= pRecvBuf->_tDefData._nMoveKey;
		pCharaNetData->_tDefData._nActionKey			= pRecvBuf->_tDefData._nActionKey;
		pCharaNetData->_tDefData._nUseActionObjectIdx	= pRecvBuf->_tDefData._nUseActionObjectIdx;
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
		_ProcessRecvPacket_Chara_DeathDataForClient( pRecvBuf, pChara);

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
		_ProcessRecvPacket_Chara_SufferDataForClient( pRecvBuf, pChara);

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
		_ProcessRecvPacket_Chara_ActionForObjectSync( pRecvBuf, pChara);

	return TRUE;
}

//파싱 순서 중요합니다.
BOOL CGameContext::_ProcessRecvPacket_Chara_ForHost( INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *) pRecvData->pArg;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	pCharaInfo->_bRecvFlag	= TRUE;
	pCharaInfo->_bSendFlag	= TRUE;			// broadcast character info

	if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
	{	// 이미 죽은 상태라면 처리 하지 않는다.
		return FALSE;
	}

	// process packet
	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		pCharaNetData->_tDefData._bOnLoadObject			= pRecvBuf->_tDefData._bOnLoadObject;
		pCharaNetData->_tDefData._iOnLoadObjectIndex	= pRecvBuf->_tDefData._iOnLoadObjectIndex;
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_ONLOADOBJECT;

		//_ProcessRecvPacket_Chara_OnLoadObject( pCharaNetData, pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
	{
		pRecvBuf->_tPosRotData.CopyTo( &pCharaNetData->_tPosRotData);
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_CHARAPOS;

		_ProcessRecvPacket_Chara_PosRotation( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		pCharaNetData->_tDefData._Body		= pRecvBuf->_tDefData._Body;
		pCharaNetData->_tDefData._Body2		= pRecvBuf->_tDefData._Body2;
		pCharaNetData->_tDefData._Upper		= pRecvBuf->_tDefData._Upper;
		pCharaNetData->_tDefData._Lower		= pRecvBuf->_tDefData._Lower;

		pCharaNetData->_tDefData._nFireAction		= pRecvBuf->_tDefData._nFireAction;
		pCharaNetData->_tWeaponData._nFireOrder		= pRecvBuf->_tWeaponData._nFireOrder;
		//pCharaNetData->_tDefData._nHelmet			= pRecvBuf->_tDefData._nHelmet;
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_CHARASTATE;

		_ProcessRecvPacket_Chara_ActionState( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_RADIO)
	{
		pCharaNetData->_tDefData._nRadio = pRecvBuf->_tDefData._nRadio;
		pCharaNetData->_tDefData._nSectorIdx = pRecvBuf->_tDefData._nSectorIdx;
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_RADIO;

		_ProcessRecvPacket_Chara_RadioChat( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON)
	{
		pRecvBuf->_tWeaponData.CopyTo( &pCharaNetData->_tWeaponData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;

		_ProcessRecvPacket_Chara_WeaponSync( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
	{	
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_SUICIDEDAMAGE;

		_ProcessRecvPacket_Chara_Suicide( pCharaNetData, pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_MISSION)
	{
		pRecvBuf->_tMissionData.CopyTo( &pCharaNetData->_tMissionData );
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_MISSION;

		_ProcessRecvPacket_Chara_MissionDataForHost( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
		_ProcessRecvPacket_Chara_GetWeaponForHost( pRecvBuf, pChara);

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE)
	{
		if(pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireDataOnObject( pCharaNetData, pRecvBuf, pChara->GetOnObject());
		else						_ProcessRecvPacket_Chara_FireData( pCharaNetData, pRecvBuf, pChara);

		pRecvBuf->_tFireData.CopyToFire( &pCharaNetData->_tFireData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_FIRE;
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HIT)
	{
		pRecvBuf->_tFireData._tHitData.CopyTo( &pCharaNetData->_tFireData._tHitData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_HIT;

		if(pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireNHitDataOnObject( pRecvBuf, pChara->GetOnObject());
		else						_ProcessRecvPacket_Chara_FireNHitData( pRecvBuf, pChara);
	}

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
		_ProcessRecvPacket_Chara_DropWeaponForHost( pRecvBuf, pChara);

	if( pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ACTIONKEY)
	{
		_ProcessRecvPacket_Chara_ActionKey( pCharaNetData, pRecvBuf, pChara);

		pCharaNetData->_tDefData._nMoveKey				= pRecvBuf->_tDefData._nMoveKey;
		pCharaNetData->_tDefData._nActionKey			= pRecvBuf->_tDefData._nActionKey;
		pCharaNetData->_tDefData._nUseActionObjectIdx	= pRecvBuf->_tDefData._nUseActionObjectIdx;
		pCharaNetData->_iInfoFlag	|= P2P_INFO_FLAG_ACTIONKEY;
	}

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_Grenade( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	if( idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return FALSE;

	GAMEINFO_THROW_WEAPON * pInfo					= getGameInfo_ThrowWeapon( idx );
	GAMEINFO_THROW_WEAPON_NETDATA * pRecvBuf		= (GAMEINFO_THROW_WEAPON_NETDATA *) pRecvData->pArg;

	if( IsDedicatedHost())
	{
		// 자신이 보낸 패킷이 온 경우는 무시
		INT32 maxLimit = getMySlotIdx() * MAX_THROWWEAPON_SLOP;
		INT32 lowLimit = idx - maxLimit;

		if( lowLimit >= 0 && lowLimit < MAX_THROWWEAPON_SLOP)
			return FALSE;
	}

	if( pInfo->_pWeaponBase != NULL )
	{
		pInfo->_tNetData._nState				= pRecvBuf->_nState;
		pInfo->_tNetData._nBoundType			= pRecvBuf->_nBoundType;
		i3Vector::Copy( &pInfo->_tNetData._vPos, &pRecvBuf->_vPos );
		i3Vector::Copy( &pInfo->_tAssistData._vPostPos, ((WeaponBase*)pInfo->_pWeaponBase)->GetPos() );
		pInfo->_tAssistData._fInterpolateTime		= 0.f;
		pInfo->_bRecv = true;

		if( IsDedicatedHost() && IsP2PHost())
		{	// Host기능이 필요한 경우(relay server에 접속된 경우)는 Broadcast를 해야한다.
			pInfo->_tAssistData._SendCount = 1;	// 방장의 경우 Recv시 바로 샌드를 해야되기때문에 정보를 초기화하지 않도록 카운트를 준다. 샌드시 카운트 감소
			pInfo->_bSend = true;
		}
	}

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_Obj_Static( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	INT32 nNetIdx = FindNetIdxForStaticObject( idx );

	if( nNetIdx == -1 )	return FALSE;

	GAMEINFO_OBJECT * pObjInfo = getGameInfo_Object( nNetIdx );
	I3ASSERT( pObjInfo != NULL);
	GAMEINFO_OBJECT_NETDATA * pRecvBuf = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;
	I3ASSERT( pRecvBuf != NULL);

	pObjInfo->_tNetData._nState	= pRecvBuf->_nState;
	pObjInfo->_tNetData._nHP		= pRecvBuf->_nHP;
	pObjInfo->_bRecv	= TRUE;

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_Obj_Move( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	INT32 nNetIdx = FindNetIdxForMoveObject( idx );

	if( nNetIdx == -1 )	return FALSE;

	GAMEINFO_OBJECT * pObjInfo	= getGameInfo_Object( nNetIdx );
	I3ASSERT( pObjInfo != NULL);
	GAMEINFO_OBJECT_NETDATA * pRecvBuf = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;
	I3ASSERT( pRecvBuf != NULL );

	if( pObjInfo->_pObjectRef == NULL)
	{
		I3FATAL( "오브젝트가 없습니다.");
		return FALSE;
	}

	pObjInfo->_tNetData._nState	= pRecvBuf->_nState;
	pObjInfo->_tNetData._nHP		= pRecvBuf->_nHP;
	i3Vector::Copy( &pObjInfo->_tNetData._vPos, &pRecvBuf->_vPos );
	pObjInfo->_tAssistData._fAssistTime	= 0.f;
	i3Vector::Copy( &pObjInfo->_tAssistData._vPostPos, ((i3Object *)pObjInfo->_pObjectRef)->GetPos() );
	pObjInfo->_bRecv = TRUE;

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_Obj_Dynamic( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	INT32 nNetIdx = FindNetIdxForDynamicObject( idx );

	if( nNetIdx == -1 )	return FALSE;

	GAMEINFO_OBJECT * pObjInfo	= getGameInfo_Object( nNetIdx );
	I3ASSERT( pObjInfo != NULL);
	GAMEINFO_OBJECT_NETDATA * pRecvBuf = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;
	I3ASSERT( pRecvBuf != NULL );

	if( pObjInfo->_pObjectRef == NULL)
	{
		I3FATAL( "오브젝트가 없습니다.");
		return FALSE;
	}

	pObjInfo->_tNetData._nState		= pRecvBuf->_nState;
	pObjInfo->_tNetData._nHP		= pRecvBuf->_nHP;

	i3Vector::Copy( &pObjInfo->_tNetData._vPos, &pRecvBuf->_vPos );
	i3Quat::Copy( &pObjInfo->_tNetData._qRot, &pRecvBuf->_qRot );
	i3Vector::Copy( &pObjInfo->_tAssistData._vPostPos, ((i3Object *)pObjInfo->_pObjectRef)->GetPos() );
	i3Matrix::GetRotateQuat( &pObjInfo->_tAssistData._qPostRot, (((i3Object*) pObjInfo->_pObjectRef)->GetMatrix()) );
	pObjInfo->_tAssistData._fAssistTime	= 0.f;
	pObjInfo->_bRecv = TRUE;

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_Obj_Anim( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	INT32 nNetIdx = FindNetIdxForAnimObject( idx );

	if( nNetIdx == -1 )	return FALSE;

	GAMEINFO_OBJECT * pObjInfo = getGameInfo_Object( nNetIdx );
	I3ASSERT( pObjInfo != NULL);
	GAMEINFO_OBJECT_NETDATA * pRecvBuf = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;
	I3ASSERT( pRecvBuf != NULL );

	pObjInfo->_tNetData._nState		= pRecvBuf->_nState;
	pObjInfo->_tNetData._nHP		= pRecvBuf->_nHP;
	pObjInfo->_tNetData._fKey		= pRecvBuf->_fKey;
	pObjInfo->_tAssistData._fAssistTime	= 0.f;
	pObjInfo->_bRecv				= TRUE;

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_DropedWeapon( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	GAMEINFO_DROPED_WEAPON * pObjInfo = getGameInfo_DropedWeapon( idx );
	I3ASSERT( pObjInfo != NULL);
	GAMEINFO_DROPEDWEAPON_NETDATA * pRecvBuf = (GAMEINFO_DROPEDWEAPON_NETDATA*) pRecvData->pArg;
	I3ASSERT( pRecvBuf != NULL );

	if( pObjInfo->_pWeaponRef == NULL )
	{// 무기가 없습니다.
		return FALSE;
	}

	if( IsDedicatedHost())
	{
		if( pObjInfo->_pWeaponRef->getOwner() != NULL)
		{	// 자신이 버린 무기는 Network처리를 하지 않는다.
			if( pObjInfo->_pWeaponRef->getOwner()->isLocal())
				return FALSE;
		}
	}

	pObjInfo->_tNetData._nState	= pRecvBuf->_nState;
	i3Vector::Copy( &pObjInfo->_tAssistData._vPostPos, pObjInfo->_pWeaponRef->GetPos() );
	i3Matrix::GetRotateQuat( &pObjInfo->_tAssistData._qPostRot, pObjInfo->_pWeaponRef->GetMatrix() );
	i3Vector::Copy( &pObjInfo->_tNetData._vPos, &pRecvBuf->_vPos );
	i3Quat::Copy( &pObjInfo->_tNetData._qRot, &pRecvBuf->_qRot );

	if( pRecvBuf->_nState == NET_WEAPONDROPED_FLY)
	{
		VEC3D vDir;
		i3Vector::Sub( &pObjInfo->_tAssistData._vDir, &pObjInfo->_tNetData._vPos, &pObjInfo->_tAssistData._vPostPos);
		i3Vector::Scale( &vDir, &pObjInfo->_tAssistData._vDir, 0.03f );
		REAL32 rLen = i3Vector::Length( &vDir);
		i3Vector::Add( &vDir, 0.f, -rLen, 0.f);
		i3Vector::Add( &pObjInfo->_tAssistData._vPostPos, &pObjInfo->_tAssistData._vPostPos, &vDir);
	}

	pObjInfo->_tAssistData._fAssistTime	= 0.f;
	pObjInfo->_bRecv = true;

	// Dedicated server로 작동하지만 방장 기능을 갖는 다면 Broadcast를 한다.
	// Client가 처리하기 때문에...
	if( IsDedicatedHost() && IsP2PHost())
	{
		pObjInfo->_bSend = true;
	}

	return TRUE;
}


BOOL CGameContext::_ProcessRecvPacket_NPC( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	VEC3D vPos, vDir;
	GAMEINFO_NPC * pNPCInfo	= getGameInfo_NPC(idx);
	GAMEINFO_NPC * pRecvBuf = (GAMEINFO_NPC*) pRecvData->pArg;

	CGameCharaBase * pNPC = g_pCharaManager->getNPCByNetIdx( idx);
	if( pNPC == NULL)
		return FALSE;

	if( pNPCInfo->_nState != pRecvBuf->_nState)
	{
		i3Vector::Copy( &pNPCInfo->_vTargetPos, &pRecvBuf->_vTargetPos);
	}

	pNPCInfo->_nState			= pRecvBuf->_nState;
	pNPC->getCharaInfo()->SetHP((REAL32)pRecvBuf->_nHP);
	pNPCInfo->_nHP				= pRecvBuf->_nHP;

	INT32 nCmd = pRecvBuf->_nState & 0x0F;
	if( nCmd == NPC_AI_STATE_HITARROW )
	{
		INT32 nTarget = (INT32)((pRecvBuf->_nState >> 4) & 0x0F);

		CGameCharaBase * pHitUser = g_pCharaManager->getCharaByNetIdx( nTarget);

		if( pHitUser != NULL)
		{
			REAL32 rDistance;

			i3Vector::Sub( &vDir, pHitUser->GetPos(), pNPC->GetPos());
			rDistance = i3Vector::Normalize( &vDir, &vDir);

			GIRAN_ARROW * pArrowInfo = g_pWeaponManager->AttachArrowToChara( pHitUser, (CHARA_HIT_PART) pRecvBuf->_nHitUserPart, &vDir);
			if( pArrowInfo != NULL && pArrowInfo->_pArrow != NULL)
			{
				WeaponArrow * pArrow = pArrowInfo->_pArrow;
				CHARA_DAMAGEINFO DamageInfo;
				DamageInfo.Reset();

				DamageInfo._IsNPC = TRUE;
				DamageInfo._FireOrder		= pArrow->getFireOrder();
				DamageInfo._IsOwner			= pNPC->isNPC();
				DamageInfo._nKillerIdx		= pNPC->getCharaNetIndex();
				DamageInfo._WallShotReduceRate = 0;
				DamageInfo._pHitResult		= NULL;
				DamageInfo._pWallHitResult	= NULL;
				DamageInfo._rDistance		= rDistance;
				DamageInfo._HitType			= GHT_CHARA;
				DamageInfo._HitPart			= (CHARA_HIT_PART) pRecvBuf->_nHitUserPart;
				DamageInfo._DeathType		= CHARA_DEATH_A;	//	CHARA_DEATH_A 는 임시이므로 다른 Death 동작이 필요하면 변경이 필요합니다.
				DamageInfo._rDamage			= pArrow->getAP();

				pArrow->getWeaponInfo()->SetDamageInfo(&DamageInfo);

				i3Vector::Copy( &vPos, pHitUser->GetPos());
				i3Vector::AddY( &vPos, 1.5f );
				DamageInfo._pVecPos		= &vPos;
				DamageInfo._pVecDir		= &vDir;

				//I3ASSERT( DamageInfo.isValid() );

				// 캐릭터의 HP 및 이펙트 처리
				pHitUser->OnHit( &DamageInfo);

				if( pHitUser->is1PV())
				{	// 1인칭은 화살을 붙이지 않습니다.
					g_pWeaponManager->PushArrow( pArrowInfo);
				}
			}
		}
	}
	else if( nCmd == NPC_AI_STATE_RESPAWN )
	{
		pNPC->getAIContext()->SetCommand( NPC_AI_STATE_RESPAWN);
	}

	return TRUE;

}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_Chara( INT32 idx, RECV_P2P_DATA * pRecvData)
{
	/*
	if( g_pCharaManager->getCharaByNetIdx( idx ) != NULL )
	{
		GAMEINFO_RECV * pRecvCharaInfo = (GAMEINFO_RECV*) pRecvData->pArg;
		for( INT32 j = 0; j < WEAPON_USAGE_COUNT; j++ )
		{
			WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)((pRecvCharaInfo->_tDefData._nWeaponInfo[j] >> 8) & 0x00FF);
			INT32 nWeaponNum = (pRecvCharaInfo->_tDefData._nWeaponInfo[j] & 0x00FF);
			g_pCharaManager->getCharaByNetIdx( idx )->getWeaponContext()->WeaponValidCheck( classType, nWeaponNum);
		}
	}
	*/

	return TRUE;
}

void CGameContext::_ProcessRecvPacket_StageInfo_Obj( i3Object * pObject, INT32 nState, REAL32 rLocalTime)
{
	I3ASSERT( pObject != NULL);
	BOOL bStateChange = FALSE;

	if( pObject->getStateCount() < nState)
		return;

	//if( pObject->getCurState() == nState)
	//	return;

	if( pObject->getCurState() != nState )
	{
		pObject->setCurState( nState );
		bStateChange = TRUE;
	}

	I3_OBJECT_STATE * pState = pObject->getState( nState );

	// 사운드 및 이팩트는 출력하지 않습니다.
	for( UINT32 k = 0; k < pState->m_pTimeEvent->getEventCount(); k++)
	{
		i3TimeEventInfo * pEventInfo = pState->m_pTimeEvent->getEvent( k);
		i3Action * pAction = pEventInfo->getAction();
		if( pAction == NULL)
			continue;

		//	|| pAction->IsExactTypeOf( i3ActionPlayAnim::GetClassMeta() )

		if( pAction->IsExactTypeOf( i3ActionPlaySound::GetClassMeta() )
			|| pAction->IsExactTypeOf( i3ActionEmitEffect::GetClassMeta() )
			|| pAction->IsExactTypeOf( i3ActionExplosion::GetClassMeta() ) )
		{	// 사운드, 이팩트, 폭발 계산을 하지 않습니다.
			continue;
		}

		//pState->m_pTimeEvent->setLocalTime( rLocalTime);

		if( rLocalTime == 0.f)
			rLocalTime = pState->m_pTimeEvent->getDuration();
		pAction->OnEvent( (i3EventReceiver*)pState->m_pTimeEvent, rLocalTime);// pState->m_pTimeEvent->getDuration());
		if( pState->m_pTimeEvent->getEventReceiver() != NULL)
		{
			pState->m_pTimeEvent->getEventReceiver()->OnEvent( I3_EVT_ACTION, pAction, pEventInfo->getParam() );

			if( pAction->IsExactTypeOf( i3ActionPlayAnim::GetClassMeta()) )
			{
				if( ((i3ActionPlayAnim*)pAction)->getAnim() != NULL)
				{
					REAL32 rAnimTime = rLocalTime - pEventInfo->getStartTime();
					if( rAnimTime > 0.f)
					{
						pObject->getSceneObject()->PlayAnim( ((i3ActionPlayAnim*)pAction)->getAnim(), I3_ANIM_BLEND_MODE_REPLACE,
							rAnimTime, 0.0f, 1.0f, ((i3ActionPlayAnim*)pAction)->getPlayMode(), I3_ANIM_PLAY_MODE_STOP);
					}
				}
			}
		}

		pState->m_pTimeEvent->setLocalTime( rLocalTime);
	}

	if( bStateChange )
		pState->m_pTimeEvent->setCurrentEventIndex( pState->m_pTimeEvent->getEventCount() );
}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_Obj_Anim( INT32 idx, RECV_P2P_DATA * pRecvData)
{
	if( m_ObjectCountInfo.m_pAnimObjectList == NULL )	return FALSE;

	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;

	INT32 nNetIdx = FindNetIdxForAnimObject( idx);
	if( nNetIdx == -1)		return FALSE;

	GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( nNetIdx );

	if( pObjectInfo->_pObjectRef == NULL )		return FALSE;
	if( pObjectInfo->_bIntrudeCheck == TRUE)	return TRUE;

	pObjectInfo->_bRecv = TRUE;
	pObjectInfo->_tNetData._nState	= pRecvObjectInfo->_nState;
	pObjectInfo->_tNetData._nHP		= pRecvObjectInfo->_nHP;
	pObjectInfo->_tNetData._fKey	= pRecvObjectInfo->_fKey;

	// 사운드 및 이팩트 등을 처리하지 않도록 세팅합니다.
	_ProcessRecvPacket_StageInfo_Obj( pObjectInfo->_pObjectRef, pRecvObjectInfo->_nState, pRecvObjectInfo->_fKey);

	pObjectInfo->_pObjectRef->setCurrentHP( pRecvObjectInfo->_nHP);

	// 특수 오브젝트입니다.
	if( pObjectInfo->_pObjectRef->IsTypeOf( CGameControledObject::GetClassMeta()))
	{
		// 난입에 대한 특수 오브젝트 처리
		((CGameControledObject*)pObjectInfo->_pObjectRef)->ProcessIntrude( pRecvObjectInfo->_nState);

		// 무기 오브젝트 처리
		if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()) ||
			pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()) )
		{
			CGameObjectWithWeapon * pWeaponObject = (CGameObjectWithWeapon*) pObjectInfo->_pObjectRef;
			if( pRecvObjectInfo->_EnterCharaIdx < SLOT_MAX_COUNT)
			{
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( pRecvObjectInfo->_EnterCharaIdx);
				if( pChara != NULL)
					pWeaponObject->EnterObject( pChara);
			}

			// 탑승 헬기에 대한 애니메이션 패스를 세팅합니다.
			pObjectInfo->_tNetData._nState = pRecvObjectInfo->_nPathKey;
		}
	}

	pObjectInfo->_bIntrudeCheck = TRUE;
	I3TRACE( "StageInfo Recv AnimObject : %d, %d, %.4f\n", nNetIdx, pObjectInfo->_tNetData._nState, pObjectInfo->_tNetData._fKey);

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_Obj_Static( INT32 idx, RECV_P2P_DATA * pRecvData)
{
	if( m_ObjectCountInfo.m_pStaticObjectList == NULL)	return FALSE;

	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;

	INT32 nNetIdx = FindNetIdxForStaticObject( idx);

	if( nNetIdx == -1)	return FALSE;

	GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( nNetIdx );

	if( pObjectInfo->_pObjectRef == NULL )	return FALSE;
	if( m_pMissionManager->isMissionObject( pObjectInfo->_pObjectRef) ) return FALSE;

	// Netstate가 아닌 Object의 현재 State 번호입니다.
	if( (INT32) pRecvObjectInfo->_nState >= pObjectInfo->_pObjectRef->getStateCount() )	return FALSE;

	if( pObjectInfo->_bIntrudeCheck == TRUE) return TRUE;

	if( pRecvObjectInfo->_nState > pObjectInfo->_pObjectRef->getCurState() )
	{
		I3_OBJECT_STATE * pState = pObjectInfo->_pObjectRef->getState( pRecvObjectInfo->_nState );
		REAL32 rDamage = (1.f - pState->m_HPRatio) * pObjectInfo->_pObjectRef->getMaxHP();

		_ProcessRecvPacket_StageInfo_Obj( pObjectInfo->_pObjectRef, pRecvObjectInfo->_nState, 0.f);

		pObjectInfo->_pObjectRef->setCurrentHP( pObjectInfo->_pObjectRef->getMaxHP() - rDamage);

		pObjectInfo->_tNetData._nHP = (UINT16) pObjectInfo->_pObjectRef->getCurrentHP();
	}

	pObjectInfo->_bIntrudeCheck = TRUE;

	I3TRACE( "StageInfo Recv StaticObject %d\n", nNetIdx);

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_Obj_Dynamic( INT32 idx, RECV_P2P_DATA * pRecvData)
{
	if( m_ObjectCountInfo.m_pDynamicObjectList == NULL)	return FALSE;

	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;

	INT32 nNetIdx = FindNetIdxForDynamicObject( idx);

	if( nNetIdx == -1)	return FALSE;

	GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( nNetIdx );

	if( pObjectInfo->_pObjectRef == NULL )	return FALSE;
	if( m_pMissionManager->isMissionObject( pObjectInfo->_pObjectRef) ) return FALSE;

	// Netstate가 아닌 Object의 현재 State 번호입니다.
	if( (INT32) pRecvObjectInfo->_nState >= pObjectInfo->_pObjectRef->getStateCount() )	return FALSE;

	if( pObjectInfo->_bIntrudeCheck == TRUE) return TRUE;

	if( pRecvObjectInfo->_nState > pObjectInfo->_pObjectRef->getCurState() )
	{
		I3_OBJECT_STATE * pState = pObjectInfo->_pObjectRef->getState( pRecvObjectInfo->_nState );
		REAL32 rDamage = (1.f - pState->m_HPRatio) * pObjectInfo->_pObjectRef->getMaxHP();

		_ProcessRecvPacket_StageInfo_Obj( pObjectInfo->_pObjectRef, pRecvObjectInfo->_nState, 0.f);

		pObjectInfo->_pObjectRef->setCurrentHP( pObjectInfo->_pObjectRef->getMaxHP() - rDamage);

		pObjectInfo->_tNetData._nHP = (UINT16) pObjectInfo->_pObjectRef->getCurrentHP();
	}

	i3Matrix::SetRotateQuat( pObjectInfo->_pObjectRef->getSceneObject()->GetMatrix(), &pRecvObjectInfo->_qRot);
	i3Matrix::PostTranslate( pObjectInfo->_pObjectRef->getSceneObject()->GetMatrix(), &pRecvObjectInfo->_vPos );

	pObjectInfo->_bIntrudeCheck = TRUE;

	I3TRACE( "StageInfo Recv DynamicObject %d\n", nNetIdx);

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_Mission( RECV_P2P_DATA * pRecvData )
{
	{
		// Mission Object
		// 설치되어 있으면 생성해서 월드에 붙여줍니다.
		MISSION_INTRUDER * pInfo = (MISSION_INTRUDER*) pRecvData->pArg;
		if( pInfo->m_MissionMode != 1)
			return FALSE;

		if( (pInfo->m_MissionInstallWho >= SLOT_MAX_COUNT) || (pInfo->m_MissionInstallWho < 0) )
			return FALSE;

		WeaponBase * pC4 = NULL;
#if defined( USE_EVENT_SHUTTLEX)
		pC4 = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, GetItemIndex("MISSION_SHUTTLEX") );
#else
		pC4 = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, g_pWeaponManager->getItemIndex_C4() );
#endif

		if( pC4 == NULL )
		{
			CGameCharaBase * pPlayer = g_pPlayer;

#if defined( DEF_OBSERVER_MODE)
			if( pPlayer == NULL)
			{
				// Observer에서는 Play중인 Character에서 처리
				INT32 i;
				for( i = 0; i < g_pCharaManager->getCharaCount(); i++)
				{
					if( g_pCharaManager->getChara( i) != NULL)
					{
						pPlayer = g_pCharaManager->getChara( i);
						break;
					}
				}
	
			}
#endif

			if( pPlayer != NULL)
			{
				// 자신이 C4가 없으면 생성해서 월드에 붙여줍니다.
				if( pPlayer->getWeaponContext()->getWeapon( WEAPON_USAGE_MISSION ) == NULL )
				{
					pPlayer->Cmd_CreateWeapon( WEAPON_USAGE_MISSION, WEAPON_CLASS_MISSION, g_pWeaponManager->getItemIndex_C4());
				}

				pPlayer->Cmd_DropWeapon( WEAPON_USAGE_MISSION, &pInfo->m_MissionInstalledPos);
			}

			I3TRACE( "Recv Mission Info. who Install : %d, pos : %.4f, %.4f, %.4f\n", pInfo->m_MissionInstallWho, getX(&pInfo->m_MissionInstalledPos), getY(&pInfo->m_MissionInstalledPos), getZ(&pInfo->m_MissionInstalledPos) );
		}
	}

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_StageInfo_ControledObject( INT32 idx, RECV_P2P_DATA * pRecvData)
{
	if( m_ObjectCountInfo.m_pControledObjectList == NULL)	return FALSE;

	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = (GAMEINFO_OBJECT_NETDATA*) pRecvData->pArg;

	INT32 nNetIdx = FindNetIdxForControledObject( idx);

	if( nNetIdx == -1)	return FALSE;

	GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( nNetIdx );

	if( pObjectInfo->_pObjectRef == NULL )	return FALSE;
	if( m_pMissionManager->isMissionObject( pObjectInfo->_pObjectRef) ) return FALSE;

	if( pObjectInfo->_bIntrudeCheck == TRUE) return TRUE;

	{	// WeaponBox의 respawn timer를 맞춘다.
		CGameObjectWeaponBox * pBox = (CGameObjectWeaponBox*) pObjectInfo->_pObjectRef;

		if( pRecvObjectInfo->_nState == 0)
			pBox->DisableObject();
		else
			pBox->EnableObject();

		pBox->setRespawnTimer( pRecvObjectInfo->_fKey);
		pBox->setMaxShellCount( pRecvObjectInfo->_nShellCount);
	}
	
	pObjectInfo->_bIntrudeCheck = TRUE;

	I3TRACE( "StageInfo Recv StaticObject %d\n", nNetIdx);

	return TRUE;
}

BOOL CGameContext::_ProcessRecvPacket_C5( INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	if( idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return FALSE;

	GAMEINFO_THROW_WEAPON * pInfo					= getGameInfo_ThrowWeapon( idx );
	GAMEINFO_THROW_WEAPON_NETDATA * pRecvBuf		= (GAMEINFO_THROW_WEAPON_NETDATA *) pRecvData->pArg;

	if( IsDedicatedHost())
	{
		// 자신이 보낸 패킷이 온 경우는 무시
		INT32 maxLimit = getMySlotIdx() * MAX_THROWWEAPON_SLOP;
		INT32 lowLimit = idx - maxLimit;

		if( lowLimit >= 0 && lowLimit < MAX_THROWWEAPON_SLOP)
			return FALSE;
	}

	if( pInfo->_pWeaponBase != NULL )
	{
		WeaponC5 * pWeaponC5 = (WeaponC5 *) pInfo->_pWeaponBase;
		if( pWeaponC5->isThrowing() )
		{	// 던져진 수류탄 
			pInfo->_tNetData._nState			= pRecvBuf->_nState;
			pInfo->_tNetData._nBoundType		= pRecvBuf->_nBoundType;
			i3Vector::Copy( &pInfo->_tNetData._vPos, &pRecvBuf->_vPos );
			i3Vector::Copy( &pInfo->_tAssistData._vPostPos, pWeaponC5->GetPos() );
			i3Vector::Copy( &pInfo->_tNetData._vDir, &pRecvBuf->_vDir );
			pInfo->_tAssistData._fInterpolateTime		= 0.f;
			pInfo->_bRecv = true;

			if( IsDedicatedHost() && IsP2PHost())
			{
				pInfo->_bSend = true; 
			}
		}
	}

	return TRUE;
}


void CGameContext::_CheckGameDataToSend( CGameCharaBase * pChara)
{
#if defined( DEF_OBSERVER_MODE)
	I3ASSERT( isObserverMe() == false);
#endif

	INT32 slotIdx = pChara->getCharaNetIndex();

	BOUNDCHK( slotIdx, SLOT_MAX_COUNT);

	GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ slotIdx]._tNetData;

	// Host는 죽어 있는 경우가 있습니다. 이 경우 HP가 0인 경우를 보내줘야 합니다. [mantis 0000081]
	// HitPoint
	pSendBuffer->_tDefData.setHP( (UINT16) pChara->getCharaInfo()->GetHP());
	
	if( pChara->IsDino() == false)
	{
		// current weapon info
		if( pSendBuffer->_tWeaponData._nWeaponSlot != (WEAPON_USAGE_TYPE) pChara->getWeaponContext()->getCurrentSlotNum())
		{
			pSendBuffer->_tWeaponData._nWeaponSlot			= (WEAPON_USAGE_TYPE) pChara->getWeaponContext()->getCurrentSlotNum();
			pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_SYNCWEAPON;
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}

		if( pChara->getWeaponContext()->getCurrent() != NULL)
		{
			if( pSendBuffer->_tWeaponData._bWeaponExtension != pChara->getWeaponContext()->getCurrent()->isExtensionActivate())
			{
				pSendBuffer->_tWeaponData._bWeaponExtension		= pChara->getWeaponContext()->getCurrent()->isExtensionActivate();
				pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_SYNCWEAPON;
				m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
			}

			// 네트워크로 동작만 보내주는 무기
			WeaponBase * pWeapon = pChara->getWeaponContext()->getCurrent()->getUsedWeapon();
			if( pWeapon != NULL)
			{
				// action state data
				if( (UINT16) pWeapon->isWeaponStateMask( WSM_NEED_SEND_FIREACTION) )
				{
					pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_CHARASTATE;
					m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
				}
			#if defined( USE_CLIENT_SENDCOUNT)
				pSendBuffer->_tDefData._nFireAction				|= (UINT16) pWeapon->isWeaponStateMask( WSM_NEED_SEND_FIREACTION);
			#else
				pSendBuffer->_tDefData._nFireAction				= (UINT16) pWeapon->isWeaponStateMask( WSM_NEED_SEND_FIREACTION);
			#endif

				pWeapon->removeWeaponStateMask( WSM_NEED_SEND_FIREACTION);

				if( pSendBuffer->_tWeaponData._nFireOrder != pWeapon->getFireOrder())
				{
					pSendBuffer->_tWeaponData._nFireOrder			= pWeapon->getFireOrder();
					pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_CHARASTATE;
					m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
				}


				if( pSendBuffer->_tWeaponData._nWeaponClassType != pWeapon->getWeaponInfo()->GetTypeClass())
				{
					pSendBuffer->_tWeaponData._nWeaponClassType		= pWeapon->getWeaponInfo()->GetTypeClass();
					pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_SYNCWEAPON;
					m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
				}

				if( pSendBuffer->_tWeaponData._nWeaponNumber != pWeapon->getWeaponInfo()->GetNumber())
				{
					pSendBuffer->_tWeaponData._nWeaponNumber		= pWeapon->getWeaponInfo()->GetNumber();
					pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_SYNCWEAPON;
					m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
				}
			}
			else
			{
				pSendBuffer->_tDefData._nFireAction = 0;
				pSendBuffer->_tWeaponData._nFireOrder = (WEAPON_FIRE_ORDER) WEAPON_PRIMARY_FIRE;
				pSendBuffer->_tWeaponData._nWeaponClassType = WEAPON_CLASS_UNKNOWN;
				pSendBuffer->_tWeaponData._nWeaponNumber = 0;
			}
			
		}
	}
	else
	{
		pSendBuffer->_tWeaponData._nWeaponSlot = ((CGameCharaDino*)pChara)->GetDinoWeaponUsageType();
		pSendBuffer->_tDefData._nFireAction = (UINT16)((CGameCharaDino*)pChara)->GetNowAttackFirstFrame();
		if(pSendBuffer->_tDefData._nFireAction)
		{
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}
		((CGameCharaDino*)pChara)->SetNowAttackFirstFrame(false);
		pSendBuffer->_tWeaponData._nFireOrder = ((CGameCharaDino*)pChara)->GetFireOrder();

		pSendBuffer->_tWeaponData._nWeaponClassType = ((CGameCharaDino*)pChara)->GetDinoWeapon()->getWeaponInfo()->GetTypeClass();
		pSendBuffer->_tWeaponData._nWeaponNumber = ((CGameCharaDino*)pChara)->GetDinoWeapon()->getWeaponInfo()->GetNumber();

		//GlobalFunc::PB_TRACE("m_CharaInfo._tNetData._tDefData._nFireAction %d _nWeaponSlot %d", m_CharaInfo._tNetData._tDefData._nFireAction, m_CharaInfo._tNetData._tWeaponData._nWeaponSlot);
	}

	// character action state
	if( pSendBuffer->_tDefData._Body != (UINT16) pChara->getActionContext()->getBodyLower())
	{
		pSendBuffer->_tDefData._Body = (UINT16) pChara->getActionContext()->getBodyLower();
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
		m_CharacterGameInfo[ slotIdx]._bSendFlag = TRUE;
	}

	UINT16 body = (UINT16) pChara->getActionContext()->getBodyUpper();
	if( pChara->isCharaStateMask( CHARA_STATE_ZOOM ) )
		body = CHARA_BODYUPPER_ZOOM;

	if( pSendBuffer->_tDefData._Body2 != body)
	{
		pSendBuffer->_tDefData._Body2 = body;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
		m_CharacterGameInfo[ slotIdx]._bSendFlag = TRUE;
	}
	
	UINT16 upper = (UINT16) pChara->getActionContext()->getUpper();
	if( pSendBuffer->_tDefData._Upper != upper)
	{
		if( (upper != CHARA_UPPER_FIRE) &&				// 공격
			(upper != CHARA_UPPER_SECONDARY_FIRE) &&		// 보조 공격
			(upper != CHARA_UPPER_DAMAGE) &&				// 피격
			(upper != CHARA_UPPER_MOVE) &&				// 이동
			(upper != CHARA_UPPER_SWAPWEAPON) )			// 무기 교체 동작은 이벤트가 발생해도 보내지 않습니다.
		{
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}
		pSendBuffer->_tDefData._Upper = upper;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
	}

	if( pSendBuffer->_tDefData._Lower != (UINT16) pChara->getActionContext()->getLower())
	{
		pSendBuffer->_tDefData._Lower = (UINT16) pChara->getActionContext()->getLower();
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
		m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
	}


	UINT8 moveKey = (UINT8)( pChara->getInputContext()->getKeyPressInput() & 0x0F);
	if( pSendBuffer->_tDefData._nMoveKey != moveKey )
	{
		pSendBuffer->_tDefData._nMoveKey				= moveKey;
		pSendBuffer->_iInfoFlag							|= P2P_INFO_FLAG_ACTIONKEY;
		m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
	}
	

	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// chara position, theta, rho
	{
		BOOL bPosChange = FALSE;
		bool bOnLoadObject = false;
		VEC3D * pPos = pChara->GetPos();

		if( pChara->isCharaStateMask( CHARA_STATE_ONOBJECT) )
		{
			bOnLoadObject = true;
			pPos = pChara->getMoveContext()->getRideObjectLocalPos();
		}

		if( !i3Vector::IsSame( &pSendBuffer->_tPosRotData._vPos, pPos ) )
		{
			i3Vector::Copy( &pSendBuffer->_tPosRotData._vPos, pPos);
			bPosChange = TRUE;
		}

		if( pSendBuffer->_tPosRotData._fTheta != pChara->getBoneContext()->getTheta() )
		{
			pSendBuffer->_tPosRotData._fTheta = pChara->getBoneContext()->getTheta();
			bPosChange = TRUE;
		}

		if( pSendBuffer->_tPosRotData._fRho != pChara->getBoneContext()->getRho() )
		{
			pSendBuffer->_tPosRotData._fRho = pChara->getBoneContext()->getRho();
			bPosChange = TRUE;
		}
		

		if( (pSendBuffer->_tDefData._bOnLoadObject != bOnLoadObject ) ||
			(pSendBuffer->_tDefData._iOnLoadObjectIndex != (UINT8) pChara->getMoveContext()->getRideObjectIndex()) )
		{
			// 일단 애니메이션 오브젝트만 체크합니다.
			pSendBuffer->_tDefData._bOnLoadObject		= bOnLoadObject;
			pSendBuffer->_tDefData._iOnLoadObjectIndex	= (UINT8) pChara->getMoveContext()->getRideObjectIndex();
			pSendBuffer->_iInfoFlag						|= P2P_INFO_FLAG_ONLOADOBJECT;
			bPosChange = TRUE;
		}

		if( bPosChange )
		{
			pSendBuffer->_iInfoFlag	|= P2P_INFO_FLAG_CHARAPOS;
			m_CharacterGameInfo[ slotIdx]._bPosSendCheck		= TRUE;
			m_CharacterGameInfo[ slotIdx]._bPosStopSendCheck	= FALSE;
		}
	}

	// Mission data
	m_CharacterGameInfo[ slotIdx]._rMissionSyncSendTime += g_pViewer->getDeltaTime();

	if( ((pSendBuffer->_tMissionData._nState == NET_MISSION_TRIGGER_ARM_START) ||
		(pSendBuffer->_tMissionData._nState == NET_MISSION_TRIGGER_DISARM_START)) &&
		(m_CharacterGameInfo[ slotIdx]._rMissionSyncSendTime > UDPSENDTIME_MISSION) )
	{
		pSendBuffer->_iInfoFlag									|= P2P_INFO_FLAG_MISSION;
		m_CharacterGameInfo[ slotIdx]._rMissionSyncSendTime	= 0.f;
		m_CharacterGameInfo[ slotIdx]._bSendFlag			= TRUE;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	//									process send sync packet								//
	//////////////////////////////////////////////////////////////////////////////////////////////
	m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime += g_pViewer->getDeltaTime();

	if( m_CharacterGameInfo[ slotIdx]._bPosSendCheck )
	{
		if( m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime > UDPSENDTIME_SYNCPOS )
		{
			// 이동 패킷을 보낸중에 싱크용으로 무기 정보를 보냅니다.
			m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime += m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime;
			if( m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime > 1.5f)
			{
				pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_SYNCWEAPON);
				m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime	= 0.f;
			}

			pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT);
			m_CharacterGameInfo[ slotIdx]._bPosSendCheck = FALSE;
			m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime = 0.f;
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}
	}
	else if( !m_CharacterGameInfo[ slotIdx]._bPosStopSendCheck)
	{	// 캐릭터 인터폴레이트로 인해 정지하고 일정시간 후에 한번 더 보내줘야 합니다.
		// 정지했다는 패킷을 받지 못하면 그 캐릭터는 계속 이동하는 모션을 취할 것입니다.
		if( m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime > UDPSENDTIME_SYNCPOS)
		{
			// 이동 패킷을 보낸중에 싱크용으로 무기 정보를 보냅니다.
			m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime += m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime;
			if( m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime > 1.5f)
			{
				pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_SYNCWEAPON);
				m_CharacterGameInfo[ slotIdx]._rWeaponSyncSendTime	= 0.f;
			}

			pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT);
			m_CharacterGameInfo[ slotIdx]._bPosStopSendCheck = TRUE;
			m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime = 0.f;
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}
	}

	if( m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime > UDPSENDTIME_NOMOVE )
	{
		pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_SYNCWEAPON );
		m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
	}

	if( m_CharacterGameInfo[ slotIdx]._bSendFlag )
	{
		m_CharacterGameInfo[ slotIdx]._rCharaPosSendTime = 0.f;
	}

	if( m_AssistInfo[ slotIdx]._nThrowState == GRENADE_THROW_SEND )
	{	// 아직 패킷을 못 받은 상태 계속 보내줍니다.
#if defined( USE_CLIENT_SENDCOUNT)
		if( m_AssistInfo[ slotIdx]._iSendGrenadeCount < 5)
#else
		if( (pChara->getActionContext()->isUpper( CHARA_UPPER_FIRE ) ||
			pChara->getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE )) &&
			(m_AssistInfo[ slotIdx]._iSendGrenadeCount < 5) )
#endif
		{
			pSendBuffer->_tFireData._nId++;
			m_AssistInfo[ slotIdx]._iSendGrenadeCount++;
			pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_FIRE | P2P_INFO_FLAG_SYNCWEAPON);
			m_CharacterGameInfo[ slotIdx]._bSendFlag	= TRUE;
		}
		else
		{
			m_AssistInfo[ slotIdx]._nThrowState = GRENADE_THROW_NONE;
			m_AssistInfo[ slotIdx]._iSendGrenadeCount = 0;
		}
	}


}

void CGameContext::_CheckGameDataToSendForAi(void)
{
	// 훈련전 전용
	if (FALSE == g_pGameContext->IsStageMaskTraining() )
	{
		return;
	}

	// NPC 시작 Idx
	INT32 beginIdx;

	if (0 == m_MySlotIdx % 2)
	{
		beginIdx = 1;
	}
	else
	{
		beginIdx = 0;
	}

	// 모든 NPC 정보를 쌓는다.
	for(INT32 i = beginIdx; (i / 2) < g_pGameContext->getEnemyAiCount() && i < SLOT_MAX_COUNT; i += 2)
	{
		CGameCharaBase* aiChara = g_pCharaManager->getCharaByNetIdx(i);

		if (aiChara != NULL)
			_CheckGameDataToSend( aiChara);
	}
}

void CGameContext::_CheckGameDataToSendForHost()
{
	INT32 i;

	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);

		if( pChara == NULL)	continue;

		GAMEINFO_CHARA_NETDATA * pSendBuffer = &m_CharacterGameInfo[ i]._tNetData;

		m_CharacterGameInfo[ i]._rObjectSyncSendTime += g_pViewer->getDeltaTime();
		if( m_CharacterGameInfo[ i]._rObjectSyncSendTime > 1.f)
		{
			pSendBuffer->_iInfoFlag	|= P2P_INFO_FLAG_SYNCOBJECTSTATE;
			m_CharacterGameInfo[ i]._rObjectSyncSendTime	= 0.f;
			m_CharacterGameInfo[ i]._bSendFlag				= TRUE;
		}

		m_CharacterGameInfo[ i]._rLifePacketSendTime += g_pViewer->getDeltaTime();
		if( m_CharacterGameInfo[ i]._rLifePacketSendTime > 1.f)
		{
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CS_LIFE;
			m_CharacterGameInfo[ i]._rLifePacketSendTime = 0.f;
			m_CharacterGameInfo[ i]._bSendFlag = TRUE;
		}
	}
}


//-------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------//
//	패킷으로 받은 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_ActionState( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	pChara->getInputContext()->ProcessActionBody( (CHARA_ACTION_BODYLOWER) pRecvBuf->_tDefData._Body);
	pChara->getInputContext()->ProcessActionBody2( (CHARA_ACTION_BODYUPPER) pRecvBuf->_tDefData._Body2);
	pChara->getInputContext()->ProcessActionUpper( (CHARA_ACTION_UPPER) pRecvBuf->_tDefData._Upper);

	// 발사 동작이 있는 경우 모션만 취합니다.
	if( !pChara->isLocal() && pRecvBuf->_tDefData._nFireAction == 1 )
	{
		VEC3D vStartPos;

		pChara->getCameraContext()->getVirtualCamPosition( &vStartPos);

		INT32 iSlot = getGameInfo_Chara( pChara->getCharaNetIndex())->_tNetData._tWeaponData._nWeaponSlot;
		pChara->Cmd_Attack( pRecvBuf->_tWeaponData._nFireOrder, &vStartPos, NULL, true, iSlot);
	}

	if( !IsP2PHost() && !pChara->isInvincible())
	{
		// 드랍된 상태라면 헬멧을 떨어뜨립니다.
		if( (pRecvBuf->_tDefData._nHelmet == NET_HELMET_STATE_DROP) &&
			pChara->getEquipContext()->isAttachOnChara( EQUIP_CLASS_TYPE_HELMET ) )
		{
			VEC3D vDir;
			i3Vector::Scale( &vDir, i3Matrix::GetAt( pChara->GetMatrix()), -1.f);
			pChara->getEquipContext()->DropToWorld( EQUIP_CLASS_TYPE_HELMET, &vDir );
		}
	}
}

void CGameContext::_ProcessRecvPacket_Chara_PosRotation( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	pChara->getInputContext()->ProcessPositionRotation( pRecvBuf);
}

void CGameContext::_ProcessRecvPacket_Chara_ActionKey( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	if( pRecvBuf->_tDefData._nUseActionObjectIdx >= OBJ_TOTAL_COUNT )
		return;

	// 오브젝트 사용
	if( (pRecvBuf->_tDefData._nActionKey & NET_ACTIONKEY_USEOBJECT) )
	{
		GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( pRecvBuf->_tDefData._nUseActionObjectIdx);

		//I3TRACE( "Object use. %d\n", pRecvBuf->_tDefData._nUseActionObjectIdx);

		if( pObjectInfo->_pObjectRef != NULL )
		{
			if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()))
			{
				if( g_pGameContext->IsP2PHost() && ((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, pCharaNetData->_tDefData._bOnLoadObject) == false )
				{
					pRecvBuf->_tDefData._nActionKey &= ~NET_ACTIONKEY_USEOBJECT;
				}
				else
				{
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnableObject();
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
				}
			}
			else if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectWeaponBox::GetClassMeta()))
			{
				CGameObjectWeaponBox * pRPG7Box = (CGameObjectWeaponBox*) pObjectInfo->_pObjectRef;
				pRPG7Box->HoldToChara( pChara);
			}
			else if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()) )
			{
				if( g_pGameContext->IsP2PHost() && ((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, pCharaNetData->_tDefData._bOnLoadObject) == false )
				{
					pRecvBuf->_tDefData._nActionKey &= ~NET_ACTIONKEY_USEOBJECT;
				}
				else
				{
					((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
				}
			}
		}
	}
	else if( (pRecvBuf->_tDefData._nActionKey & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT) )
	{
		GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( pRecvBuf->_tDefData._nUseActionObjectIdx);

		if( pObjectInfo->_pObjectRef != NULL )
		{
			if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()) )
			{
				((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject();
			}
			else if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()))
			{
				((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject();
			}
		}
	}
	else if( (pRecvBuf->_tDefData._nActionKey & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT) )
	{
		GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( pRecvBuf->_tDefData._nUseActionObjectIdx);

		if( pObjectInfo->_pObjectRef != NULL )
		{
			if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()) )
			{
				if( ((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, pCharaNetData->_tDefData._bOnLoadObject) )
				{
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
				}
			}
			else if ( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()))
			{
				if( ((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, pCharaNetData->_tDefData._bOnLoadObject) )
				{
					((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
				}
			}
		}
	}
}

void CGameContext::_ProcessRecvPacket_Chara_OnLoadObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pCharaNetData != NULL);
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);
}

// 발사 패킷 또는 무기 스왑 받으면 무기 싱크 맞추는 곳
void CGameContext::_ProcessRecvPacket_Chara_WeaponSync( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	if(pChara->IsDino())
	{
		return;
	}
	// 무기 슬롯을 먼저 변경합니다.
	if( pRecvBuf->_tWeaponData._nWeaponSlot != pChara->getWeaponContext()->getCurrentSlotNum() )
	{
		pChara->Cmd_ChangeWeapon( pRecvBuf->_tWeaponData._nWeaponSlot);
	}

	// 무기가 맞는지 체크해봐야합니다.
	if( (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE) || (m_MyIntrudeState == MY_INTRUDE_STATE_INTRUDE))
		pChara->getWeaponContext()->WeaponValidCheck( (WEAPON_CLASS_TYPE) pRecvBuf->_tWeaponData._nWeaponClassType, pRecvBuf->_tWeaponData._nWeaponNumber );

	// Weapon Extension
	if( (pChara->getWeaponContext()->getCurrent() != NULL))
	{
		if( pChara->getWeaponContext()->getCurrent()->IsExtensionWeapon())
		{
			if( pRecvBuf->_tWeaponData._bWeaponExtension && (pChara->getWeaponContext()->getCurrent()->isExtensionActivate() == FALSE) )
			{
				pChara->getWeaponContext()->getCurrent()->ExtensionActivate( TRUE, FALSE);
				pChara->getWeaponContext()->getCurrent()->OnSetVisibleExtension( TRUE);
			}
			else if( (pRecvBuf->_tWeaponData._bWeaponExtension == FALSE) && pChara->getWeaponContext()->getCurrent()->isExtensionActivate())
			{
				pChara->getWeaponContext()->getCurrent()->ExtensionActivate( FALSE, FALSE);
				pChara->getWeaponContext()->getCurrent()->OnSetVisibleExtension( FALSE);
			}
		}
	}
}

void CGameContext::_ProcessRecvPacket_Chara_RadioChat( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	char szMessage[128];
	char szSector[256];

#if defined( DEF_OBSERVER_MODE)
	// Observer는 처리하지 않는다.
	if( isObserverMe())
	{
		return;
	}
#endif

	I3ASSERT( pChara != NULL);

	// Radio Chat
	if( pRecvBuf->_tDefData._nRadio )
	{
		if( pChara->getCharaTeam() == g_pPlayer->getCharaTeam() )
		{
			RADIOCHAT_TYPE	nType = (RADIOCHAT_TYPE) (pRecvBuf->_tDefData._nRadio / 10);
			INT32	nMsgNum = (INT32)(pRecvBuf->_tDefData._nRadio) % 10;

			if( nType < RADIOCHAT_TYPE_AUTO )
			{
				const char * pszMessage = GetRadioMessage( nType, nMsgNum );

				CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
				I3ASSERT( pStage != NULL);
				i3World * pWorld = pStage->getWorld();
				I3ASSERT( pWorld != NULL);
				i3MapSectorTable * pTable = pWorld->getMapSectorTable();

				if( pTable != NULL)
				{
					INT32 idNew = pRecvBuf->_tDefData._nSectorIdx;
					I3ASSERT( (idNew >= 0) && (idNew < pTable->getCount()));
					I3_MAP_SECTOR_INFO * pSector = pTable->getMapSector( idNew);
					I3ASSERT( pSector != NULL);

					if( i3String::Length( pSector->m_szName) > 0 && idNew != 0)
					{
						sprintf( szSector, "{col:98,180,253,255}[%s]{/col}", pSector->m_szName);
						sprintf( szMessage, "[Radio] %s %s", pszMessage, szSector);
					}
					else
					{
						sprintf( szMessage, "[Radio] %s", pszMessage );
					}
				}
				else
				{
					sprintf( szMessage, "[Radio] %s", pszMessage );
				}

				g_pFramework->getChatBox()->PutUserChatting( getNickForSlot( pChara->getCharaNetIndex()), szMessage );
			}
			else if( nType == RADIOCHAT_TYPE_AUTO )
			{	// Auto message
				if( nMsgNum != 1 )
				{
					//수류탄은 메시지를 출력하지 않고 사운드만 출력합니다.
					const char * pszMessage = GetRadioMessage( nType, nMsgNum );
					sprintf( szMessage, "%s", pszMessage );
					g_pFramework->getChatBox()->PutUserChatting( getNickForSlot( pChara->getCharaNetIndex()), szMessage );
				}
			}

			PlayRadioMessageSound( nType, nMsgNum, pChara);
		}
	}
}

void CGameContext::_ProcessRecvPacket_Chara_ActionForObjectSync( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	if( (pRecvBuf->_tDefData._nSyncObjectState & NET_SYNCKEY_ON_OBJECT) )
	{
		// 아웃포스트 헬기만 해당됩니다.. 그외의 오브젝트가 세팅된다면 문제가 발생합니다.
		GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( pRecvBuf->_tDefData._iSyncObjectIdx);

		if( pObjectInfo->_pObjectRef != NULL )
		{
			if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()))
			{
				BOUNDCHK( pChara->getCharaNetIndex(), SLOT_MAX_COUNT);
				if( ((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, m_CharacterGameInfo[ pChara->getCharaNetIndex()]._tNetData._tDefData._bOnLoadObject) )
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
			}
			else if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()))
			{
				BOUNDCHK( pChara->getCharaNetIndex(), SLOT_MAX_COUNT);
				if( ((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, m_CharacterGameInfo[ pChara->getCharaNetIndex()]._tNetData._tDefData._bOnLoadObject) )
					((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
			}
		}
	}
	else if( (pRecvBuf->_tDefData._nSyncObjectState & NET_SYNCKEY_OFF_OBJECT) )
	{
		GAMEINFO_OBJECT * pObjectInfo = getGameInfo_Object( pRecvBuf->_tDefData._iSyncObjectIdx);

		if( pObjectInfo->_pObjectRef != NULL )
		{
			if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRidingHelicopter::GetClassMeta()))
				((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject( pChara->getCharaNetIndex());
			else if( pObjectInfo->_pObjectRef->IsExactTypeOf( CGameObjectRespawnHelicopter::GetClassMeta()))
				((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject( pChara->getCharaNetIndex());
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	미션용 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_MissionDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara )
{
	switch( pRecvBuf->_tMissionData._nState )
	{
	case NET_MISSION_TRIGGER_ARM_START :
		if( pChara->isNetwork() && !pChara->getActionContext()->isUpper( CHARA_UPPER_FIRE))
		{
			if( (pChara->GetCurrentCharaWeapon() != NULL) &&
				(pChara->GetCurrentCharaWeapon()->getWeaponInfo() != NULL) &&
				(pChara->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION ) )
			{	// 확인 코드. 미션용 C4만 애니메이션을 하도록 수정. 2009.06.09 dfly79
				pChara->Cmd_Attack();
				//pChara->Cmd_Only_AttackWeapon();
			}
		}
		m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, TRUE );
		break;
	case NET_MISSION_TRIGGER_ARM_CANCEL :
		if( pChara->isNetwork() && pChara->getActionContext()->isUpper( CHARA_UPPER_FIRE))
			pChara->Cmd_Idle();
		m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, FALSE );
		break;
	case NET_MISSION_TRIGGER_ARM_DONE :			break;
	case NET_MISSION_TRIGGER_ARM_FAIL :			m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, FALSE );	break;
	case NET_MISSION_TRIGGER_DISARM_START :		m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), TRUE );									break;
	case NET_MISSION_TRIGGER_DISARM_DONE :		break;
	case NET_MISSION_TRIGGER_DISARM_CANCEL :	m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), FALSE );									break;
	case NET_MISSION_TRIGGER_DISARM_FAIL :		m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), FALSE );									break;
	default : break;
	}
}

void CGameContext::_ProcessRecvPacket_Chara_MissionDataForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	// Process
	{
		switch( pRecvBuf->_tMissionData._nState )
		{
		case NET_MISSION_TRIGGER_ARM_START :
			if( pChara->isNetwork() && !pChara->getActionContext()->isUpper( CHARA_UPPER_FIRE))
			{
				if( (pChara->GetCurrentCharaWeapon() != NULL) &&
					(pChara->GetCurrentCharaWeapon()->getWeaponInfo() != NULL) &&
					(pChara->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION ) )
				{	// 확인 코드. 미션용 C4만 애니메이션을 하도록 수정. 2009.06.09 dfly79
					pChara->Cmd_Attack();
					//pChara->Cmd_Only_AttackWeapon();
				}
			}
			m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, TRUE );
			break;
		case NET_MISSION_TRIGGER_ARM_CANCEL :
			if( pChara->isNetwork() && pChara->getActionContext()->isUpper( CHARA_UPPER_FIRE))
				pChara->Cmd_Idle();
			m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, FALSE );
			break;
		case NET_MISSION_TRIGGER_ARM_DONE :			m_pMissionManager->SendInstallComplete( pChara->getCharaNetIndex() );								break;
		case NET_MISSION_TRIGGER_ARM_FAIL :			m_pMissionManager->Install( pChara->getCharaNetIndex(), pRecvBuf->_tMissionData._nArea, FALSE );	break;
		case NET_MISSION_TRIGGER_DISARM_START :		m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), TRUE );									break;
		case NET_MISSION_TRIGGER_DISARM_DONE :		m_pMissionManager->SendUninstallComplete( pChara->getCharaNetIndex() );								break;
		case NET_MISSION_TRIGGER_DISARM_CANCEL :	m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), FALSE );									break;
		case NET_MISSION_TRIGGER_DISARM_FAIL :		m_pMissionManager->Uninstall( pChara->getCharaNetIndex(), FALSE );									break;
		default : break;
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	낙하 데미지에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_Suicide( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pCharaNetData != NULL);
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	bool bFalling = false;

	pRecvBuf->_tSuicideDamageData.CopyTo( &pCharaNetData->_tSuicideDamageData);

	if( pRecvBuf->_tSuicideDamageData._nObjectIdx == 0xFF)
		bFalling = true;

	pChara->Cmd_DamageSelf( pRecvBuf->_tSuicideDamageData._nDamage, pRecvBuf->_tSuicideDamageData._nObjectIdx, bFalling);
}

//-------------------------------------------------------------------------------------------//
//	_CalcGetWeapon()
//	무기 습득 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_GetWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	if(pChara->IsDino()) {
		return;
	}

	// check valid data
	if( pRecvBuf->_tGetWeaponData._nIdx >= MAX_DROPEDWEAPON_COUNT )		return;
	if( pRecvBuf->_tGetWeaponData._nState == 0 )						return;

	GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( pRecvBuf->_tGetWeaponData._nIdx );
	WeaponBase * pWeapon = g_pWeaponManager->FindDropWeaponByIdx( pRecvBuf->_tGetWeaponData._nIdx);
	if( pWeapon != NULL )
	{
		pChara->Cmd_PickupWeapon( pWeapon);
	}
	else
	{
		// Netsync
		// 무기가 없습니다. 생성해서 붙여줍니다.
		pChara->getWeaponContext()->WeaponValidCheck( (WEAPON_CLASS_TYPE) pRecvBuf->_tGetWeaponData._nClassType, pRecvBuf->_tGetWeaponData._nNumber );
		pWeapon = pChara->getWeaponContext()->getWeapon( pRecvBuf->_tGetWeaponData._nSlot );
	}
	pWeapon->SetBulletCountForNetwork( pRecvBuf->_tGetWeaponData._nLoadedBullet, pRecvBuf->_tGetWeaponData._nDualLoadedBullet, pRecvBuf->_tGetWeaponData._nMaxBullet );
	pDropedWeaponInfo->Reset();
}

void CGameContext::_ProcessRecvPacket_Chara_GetWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	if(pChara->IsDino()) {
		return;
	}

	// check valid data
	if( pRecvBuf->_tGetWeaponData._nIdx >= MAX_DROPEDWEAPON_COUNT )		return;

	// Process
	{
		BOOL	bTakeWeapon = FALSE;
		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( pRecvBuf->_tGetWeaponData._nIdx );
		WeaponBase * pWeapon = g_pWeaponManager->FindDropWeaponByIdx( pRecvBuf->_tGetWeaponData._nIdx );
		if( pWeapon != NULL )
		{
			// 무기를 들게 합니다.
			pChara->Cmd_PickupWeapon( pWeapon);
			bTakeWeapon = TRUE;
		}
		else
		{
			// 그 캐릭터가 들고 있는지 체크합니다
			// 해당 캐릭터가 이미 들고 있지만 확인 메세지를 못 받은 경우 일 수 있습니다. 해당 무기를 들도록 합니다.
			if( pChara->getWeaponContext()->getWeapon(0) != NULL &&
				(pChara->getWeaponContext()->getWeapon(0)->getNetworkIdx() == pRecvBuf->_tGetWeaponData._nIdx) )
			{
				bTakeWeapon = TRUE;
				pWeapon = pChara->getWeaponContext()->getWeapon(0);
			}
			else if( pChara->getWeaponContext()->getWeapon(1) != NULL &&
				(pChara->getWeaponContext()->getWeapon(1)->getNetworkIdx() == pRecvBuf->_tGetWeaponData._nIdx))
			{
				bTakeWeapon = TRUE;
				pWeapon = pChara->getWeaponContext()->getWeapon(1);
			}
		}

		if( bTakeWeapon )
		{
			// Set network info
			// 드랍된 무기 정보 초기화
			pDropedWeaponInfo->_fDropedWeaponLifeTime	= 0.f;
			pDropedWeaponInfo->setState( NET_WEAPONDROPED_GET);

			// broadcast info set
			WriteGameEvent_PickupWeapon( pChara, pWeapon);
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	_CalcDropWeapon()
//	무기 버리기 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_DropWeaponForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	if( pRecvBuf->_tDropWeaponData._nState == 0 )	return;

	BOOL bDeathFlag = ((pRecvBuf->_tDropWeaponData._nState >> 1) & 0x01) ? FALSE : TRUE;

	WeaponBase * pWeapon = g_pWeaponManager->SetDropWeapon( pChara->getCharaNetIndex(), pRecvBuf->_tDropWeaponData._nSlot, pRecvBuf->_tDropWeaponData._nIdx, bDeathFlag );

	if( pWeapon != NULL )
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT( pWeaponInfo != NULL);

		// 드랍된 무기에 대한 정보 세팅
		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = getGameInfo_DropedWeapon( pRecvBuf->_tDropWeaponData._nIdx );

		pDropedWeaponInfo->Bind( pWeapon, pChara);

		m_nDropedWeaponCount = pRecvBuf->_tDropWeaponData._nIdx + 1;

		pWeapon->SetBulletCountForNetwork( pRecvBuf->_tDropWeaponData._nLoadedBullet, pRecvBuf->_tDropWeaponData._nDualLoadedBullet, pRecvBuf->_tDropWeaponData._nMaxBullet );
	}
}

void CGameContext::_ProcessRecvPacket_Chara_DropWeaponForHost( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	WriteGameEvent_DropWeapon( pChara, pRecvBuf->_tDropWeaponData._nSlot, pRecvBuf->_tDropWeaponData._nLoadedBullet, pRecvBuf->_tDropWeaponData._nDualLoadedBullet, pRecvBuf->_tDropWeaponData._nMaxBullet);
}

//-------------------------------------------------------------------------------------------//
//	발사 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_FireDataOnObject( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj)
{
	I3ASSERT(pObj);
	I3ASSERT(pObj->GetWeaponOnObject());

	pObj->GetWeaponOnObject()->FireWeaponOnObject();
	pObj->GetWeaponOnObject()->FireCollision( pObj->GetWeaponOnObject()->GetStartPos(), pObj->GetWeaponOnObject()->GetTargetPos());
}

//-------------------------------------------------------------------------------------------//
//	발사 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
bool CGameContext::_ProcessRecvPacket_Chara_FireData( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT( pCharaNetData != NULL);
	I3ASSERT( pRecvBuf != NULL);
	I3ASSERT( pChara != NULL);

	if( isNetThrowWeapon( &pRecvBuf->_tWeaponData))
	{
		if( pRecvBuf->_tFireData.isValidThrow( pChara->getCharaNetIndex() ) == false )	return false;

		pCharaNetData->_tFireData._nNetSlotIdx = pRecvBuf->_tFireData._nNetSlotIdx;

		GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = getGameInfo_ThrowWeapon( pRecvBuf->_tFireData._nNetSlotIdx);
		if( pThrowWeaponInfo->_pWeaponBase != NULL)
		{
			I3TRACE( "이미 사용중인 수류탄..\n");
			return false;
		}
	}
	else
	{
		// 같은 패킷이거나 늦게 날아온 패킷입니다.
		if( pCharaNetData->_tFireData._nId >=  pRecvBuf->_tFireData._nId )	return false;
		pCharaNetData->_tFireData._nId = pRecvBuf->_tFireData._nId;
	}
	
	pChara->getInputContext()->ProcessNetFire( pRecvBuf);
	return true;
}

void CGameContext::_ProcessRecvPacket_Chara_FireNHitDataOnObject( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj)
{
	INT32 i;

	// Process
	VEC3D	vInvDir;
	CHARA_DAMAGEINFO DamageInfo;
	DamageInfo.Reset();

	WeaponOnObject* pWeapon = pObj->GetWeaponOnObject();
	I3ASSERT(pWeapon);

	CWeaponInfo * pWeaponInfo	= pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	CGameCharaBase * pHitUser = NULL;

	INT32 hitCount = pRecvBuf->_tFireData._tHitData.getHitCount();

	if( hitCount > MAX_FIREHIT_COUNT )
	{
		hitCount = MAX_FIREHIT_COUNT;		// 문제가 있을 수 있습니다.
		pRecvBuf->_tFireData._tHitData.setHitCount( hitCount);
	}

	for( i = 0; i < hitCount; i++ )
	{
		UINT8 hitPart = pRecvBuf->_tFireData._tHitData.getHitPart( i);
		UINT8 hitWho = pRecvBuf->_tFireData._tHitData.getHitWho( i);
		GAMEINFO_HIT_TYPE hitType = pRecvBuf->_tFireData._tHitData.getHitType( i);

		if( hitPart == 0 )					continue;
		if( ((hitWho >> 7) & 0x01) != 0)	continue;

		// 캐릭터 히트
		INT32 nHitIndex = (hitWho & 0x0F);

		pHitUser = g_pCharaManager->getCharaByNetIdx( nHitIndex );

		// 맞는것과 동시에 캐릭터가 나간경우는 예외 처리가 되야합니다.. - 정순구
		if( pHitUser == NULL )		continue;

		i3Vector::Sub( &vInvDir, pHitUser->GetPos(), pWeapon->GetMuzzlePos());
		REAL32 _dist = i3Vector::Normalize( &vInvDir, &vInvDir );

		pWeaponInfo->SetDamageInfo(&DamageInfo);

		DamageInfo._FireOrder		= pRecvBuf->_tWeaponData._nFireOrder;
		DamageInfo._rDamage			= (REAL32) pRecvBuf->_tFireData._tHitData.getHitDamage( i);
		DamageInfo._pVecDir			= &vInvDir;
		DamageInfo._IsOwner			= false;
		DamageInfo._nKillerIdx		= pObj->GetEnterCharaIdx();

		DamageInfo._pHitResult		= NULL;
		DamageInfo._pWeapon			= pWeapon;
		DamageInfo._pWeaponInfo		= pWeaponInfo;
		// 월샷 데미지 감소량
		DamageInfo._WallShotReduceRate = 0;
		DamageInfo._pWallHitResult	= NULL;

		DamageInfo._HitType			= hitType;
		DamageInfo._HitPart			= (CHARA_HIT_PART) hitPart;
		DamageInfo._rDistance		= _dist;

		//	헤드샷 죽음
		if( (CHARA_HIT_PART) hitPart == CHARA_HIT_HEAD)
			DamageInfo._DeathType = CHARA_DEATH_H;
		//	보통 죽음
		else
			DamageInfo._DeathType = CHARA_DEATH_A;	//	CHARA_DEATH_A 는 임시이므로 다른 Death 동작이 필요하면 변경이 필요합니다.

		DamageInfo._pVecPos		= pWeapon->GetMuzzlePos();

		DamageInfo._IsNPC = FALSE;

		I3ASSERT( DamageInfo.isValid() );

		if( hitType == GHT_CHARA)
		{
			if( IsP2PHost() )
			{	// CS만 데미지 계산
	#if defined( USE_HOST_MODULE)
				g_pFramework->getHostModule()->ProcessHit( pHitUser, &DamageInfo, DamageInfo._rDamage);
	#else
				pHitUser->NET_HitForHost( &DamageInfo, DamageInfo._rDamage);
	#endif
			}
			else
			{
				//	데미지 동작
				if( pHitUser->isCharaStateMask( CHARA_STATE_DEATH) == FALSE)
				{
					if( DamageInfo._DeathType == CHARA_DEATH_H )
						pHitUser->getEquipContext()->setHelmetProtect( TRUE);

					pHitUser->Cmd_Damage( &DamageInfo);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	_ProcessRecvPacket_Chara_FireNHitData()
//	발사 및 히트에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_FireNHitData( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase* pChara )
{
	INT32	i;
	CHARA_DAMAGEINFO DamageInfo;
	VEC3D	vInvDir, vPos;
	WeaponBase * pWeapon = NULL;

	// 무기를 확인합니다.
	pWeapon = pChara->GetCharaWeapon(pRecvBuf->_tWeaponData._nWeaponSlot);

	if( pWeapon == NULL )		return;

	// KEYWORD : WEAPON_DUALHANDGUN
	if( pWeapon->IsExactTypeOf( WeaponDualHandGun::GetClassMeta()))
	{
		// 데미지 계산은 왼손 무기로..
		// UI 표시는 듀얼 무기로 처리해야 합니다.
		WeaponBase * pLeftWeapon = ((WeaponDualHandGun*)pWeapon)->getLeftWeapon();
		if( pLeftWeapon->getWeaponInfo()->GetNumber() == pRecvBuf->_tWeaponData._nWeaponNumber)
			pWeapon = pLeftWeapon;
	}
	else if( pWeapon->IsTypeOf( WeaponDualSMG::GetClassMeta()))
	{
		WeaponBase * pLeftWeapon = ((WeaponDualSMG*)pWeapon)->getLeftWeapon();
		if( pLeftWeapon->getWeaponInfo()->GetNumber() == pRecvBuf->_tWeaponData._nWeaponNumber)
			pWeapon = pLeftWeapon;
	}
	else if( pWeapon->IsTypeOf( WeaponDualCIC::GetClassMeta()))
	{	// KEYWORD : WEAPON_CIC
		if( pRecvBuf->_tWeaponData._nFireOrder == WEAPON_SECONDARY_FIRE)
			pWeapon = ((WeaponDualCIC*)pWeapon)->getLeftWeapon();
	}

	CWeaponInfo * pWeaponInfo	= pWeapon->getRealWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);

	INT32 hitCount = pRecvBuf->_tFireData._tHitData.getHitCount();

	if( hitCount > MAX_FIREHIT_COUNT )
	{
		hitCount = MAX_FIREHIT_COUNT;		// 문제가 있을 수 있습니다.
		pRecvBuf->_tFireData._tHitData.setHitCount( hitCount);
	}

	for( i = 0; i < hitCount; i++ )
	{
		CHARA_HIT_PART hitPart = (CHARA_HIT_PART) pRecvBuf->_tFireData._tHitData.getHitPart( i);
		UINT8 hitWho = pRecvBuf->_tFireData._tHitData.getHitWho( i);
		GAMEINFO_HIT_TYPE hitType = pRecvBuf->_tFireData._tHitData.getHitType( i);

		switch( hitType)
		{ 
		case GHT_CHARA :// 캐릭터 히트
			{
				if( hitPart == CHARA_HIT_UNKNOWN )			break;
				if( ((hitWho >> 7) & 0x01) != 0)			break;
				
				INT32 nHitIndex = (hitWho & 0x0F);

				CGameCharaBase * pHitUser = g_pCharaManager->getCharaByNetIdx( nHitIndex );

				// 맞는것과 동시에 캐릭터가 나간경우는 예외 처리가 되야합니다.. - 정순구
				if( pHitUser == NULL )		break;

				DamageInfo.Reset();

				i3Vector::Sub( &vInvDir, pHitUser->GetPos(), pChara->GetPos());
				REAL32 _dist = i3Vector::Normalize( &vInvDir, &vInvDir );

				pWeaponInfo->SetDamageInfo(&DamageInfo);

				DamageInfo._FireOrder		= pRecvBuf->_tWeaponData._nFireOrder;
				DamageInfo._rDamage			= (REAL32) pRecvBuf->_tFireData._tHitData.getHitDamage( i);

				// GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_FireNHitData _rDamage %f \n", DamageInfo._rDamage);

				DamageInfo._pVecDir			= &vInvDir;
				DamageInfo._IsOwner			= pChara->isLocal();
				DamageInfo._nKillerIdx		= pChara->getCharaNetIndex();

				DamageInfo._pHitResult		= NULL;

				DamageInfo._pWeapon			= pWeapon;
				DamageInfo._pWeaponInfo		= pWeaponInfo;

				// 월샷 데미지 감소량
				DamageInfo._WallShotReduceRate = 0;
				DamageInfo._pWallHitResult	= NULL;

				DamageInfo._HitType			= hitType;
				DamageInfo._HitPart			= hitPart;
				DamageInfo._rDistance		= _dist;

				if( (pWeaponInfo->GetTypeClass() == WEAPON_CLASS_THROWING) ||
					(pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION))
				{	//	폭탄등에 죽음
					DamageInfo._DeathType	= CHARA_DEATH_B;
					DamageInfo._pVecPos		= pWeapon->GetPos();
				}
				else
				{
					if ( pWeaponInfo->GetTypeClass() == WEAPON_CLASS_DINO && 
						g_pCharaManager->IsAssist_Kill(DamageInfo._nKillerIdx,pHitUser->getCharaNetIndex()))
					{
						DamageInfo._DeathType = CHARA_DEATH_ASSIST;	//	CHARA_DEATH_A 는 임시이므로 다른 Death 동작이 필요하면 변경이 필요합니다.	
					}
					else
					{
						DamageInfo.SetDeathTypeByHitPart( hitPart);
					}

					i3Vector::Set( &vPos, getX( pChara->GetPos() ), getY( pChara->GetPos() ) + pChara->getCameraContext()->getIKHeightForCam(), getZ( pChara->GetPos() ) );
					DamageInfo._pVecPos		= &vPos;
				}

				DamageInfo._IsNPC = FALSE;

				I3ASSERT( DamageInfo.isValid() );

				if( IsP2PHost() )
				{	// CS만 데미지 계산
					//	네트워크 데미지 정보 처리
			#if defined( USE_HOST_MODULE)
					g_pFramework->getHostModule()->ProcessHit( pHitUser, &DamageInfo, DamageInfo._rDamage);
			#else
					pHitUser->NET_HitForHost( &DamageInfo, DamageInfo._rDamage);
			#endif
				}
				else
				{
					//	데미지 동작
					if( pHitUser->isCharaStateMask( CHARA_STATE_DEATH) == FALSE)
					{
						if( DamageInfo._DeathType == CHARA_DEATH_H && pHitUser->IsDino() == false)
							pHitUser->getEquipContext()->setHelmetProtect( TRUE);

						pHitUser->Cmd_Damage( &DamageInfo);
					}
				}
			}
			break;
		case GHT_ITEM_C5 :
			{
				WeaponBase * pHitedWeapon = g_pWeaponManager->FindThrowWeaponByNetIndex( GetItemIndex("THROWING_C5"), hitWho);
				if( pHitedWeapon != NULL)
				{
					if( pHitedWeapon->IsExactTypeOf( WeaponC5::GetClassMeta()))
					{
						((WeaponC5*)pHitedWeapon)->Explosion();
					}
				}
			}
			break; 
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	발사 및 히트에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameContext::_ProcessRecvPacket_Chara_DeathDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara )
{
	VEC3D vInvDir;
	VEC3D vPos;

	// Check valid data
	if( pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_UN )	return;
	if( pRecvBuf->_tSufferData._nShooter >= SLOT_MAX_COUNT )	return;

	// 쏜 유저입니다.
	CGameCharaBase * pKiller = g_pCharaManager->getCharaByNetIdx( pRecvBuf->_tSufferData._nShooter);
	if( pKiller == NULL )								return;

	CHARA_DAMAGEINFO DamageInfo;
	DamageInfo.Reset();

	CWeaponInfo * pWeaponInfo = NULL;
	WeaponBase* pWeapon = NULL;

	DamageInfo._rDamage	= 50.f;
	DamageInfo._FireOrder		= WEAPON_PRIMARY_FIRE;

	pWeapon = pKiller->GetCurrentCharaWeapon();

	if( pWeapon != NULL )
	{
		pWeaponInfo = pWeapon->getWeaponInfo();
		DamageInfo._FireOrder		= pWeapon->getFireOrder();
		DamageInfo._rDamage			= pWeapon->getAP();
	}
	else
	{
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_M7"));
		DamageInfo._rDamage			= (REAL32) pWeaponInfo->GetDamage();
	}

	if( (pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_B) ||
		(pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_O) )
	{
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_THROWING, GetItemIndex("THROWING_K400"));
		DamageInfo._rDamage			= (REAL32) pWeaponInfo->GetDamage();

		DamageInfo._pVecPos		= pKiller->GetPos();
		i3Vector::Scale( &vInvDir, &pRecvBuf->_tSufferData._vHitDir, -1.f);
	}
	else
	{

		i3Vector::Set( &vPos, getX( pKiller->GetPos() ), getY( pKiller->GetPos() ) + pKiller->getCameraContext()->getIKHeightForCam(), getZ( pKiller->GetPos() ) );
		DamageInfo._pVecPos		= &vPos;
		i3Vector::Sub( &vInvDir, pChara->GetPos(), pKiller->GetPos());
	}

	DamageInfo._rDistance = i3Vector::Normalize( &vInvDir, &vInvDir );
	DamageInfo._pWeapon = pWeapon;
	DamageInfo._pWeaponInfo = pWeaponInfo;

	I3ASSERT( pWeaponInfo != NULL);
	pWeaponInfo->SetDamageInfo(&DamageInfo);

	DamageInfo._pVecDir			= &vInvDir;
	DamageInfo._IsOwner			= pKiller->isLocal();
	DamageInfo._nKillerIdx		= pKiller->getCharaNetIndex();

	DamageInfo._pHitResult		= NULL;
	DamageInfo._WallShotReduceRate = 0;
	DamageInfo._pWallHitResult	= NULL;
	DamageInfo._DeathType		= (CHARA_DEATH_TYPE) pRecvBuf->_tSufferData._nDeathType;

	DamageInfo._HitPart			= (CHARA_HIT_PART) pRecvBuf->_tSufferData._nHitpart;

	//	임시...나이프로 보조(찌르기) 공격을 하면 데미지 1.5배가 된다. komet
	//if( DamageInfo._WeaponType == WEAPON_CLASS_KNIFE && DamageInfo._FireOrder == WEAPON_SECONDARY_FIRE)
	//	DamageInfo._rDamage		= DamageInfo._rDamage * 1.5f;

	I3ASSERT( DamageInfo.isValid() );

	// 캐릭터의 죽음에 대한 설정
	g_pCharaManager->DeathChara( pChara->getCharaNetIndex(), pRecvBuf->_tSufferData._nShooter, &vInvDir,
						(CHARA_DEATH_TYPE) pRecvBuf->_tSufferData._nDeathType, (CHARA_HIT_PART) pRecvBuf->_tSufferData._nHitpart );

	// 해당 캐릭터의 히트 등을 세팅(죽은 것과 달리 히트 정보는 따로 호출됩니다)
	pChara->Cmd_HitEffect( &DamageInfo );

	// 해당 캐릭터가 죽었음을 체크
	setUserDeath( pChara->getCharaNetIndex(), TRUE );
}

void CGameContext::_ProcessRecvPacket_Chara_SufferDataForClient( GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	VEC3D vShootPos, vDir;
	CHARA_DAMAGEINFO DamageInfo;
	WeaponBase* pWeapon = NULL;

	if( pRecvBuf->_tSufferData._nShooter >= SLOT_MAX_COUNT)
		return;

	DamageInfo.Reset();
	DamageInfo._HitPart		= (CHARA_HIT_PART) pRecvBuf->_tSufferData._nHitpart;
	DamageInfo._pVecDir		= NULL;
	DamageInfo._WeaponType	= WEAPON_CLASS_THROWING;
	DamageInfo._nKillerIdx	= pRecvBuf->_tSufferData._nShooter;
	DamageInfo._DeathType	= (CHARA_DEATH_TYPE) pRecvBuf->_tSufferData._nDeathType;

	CGameCharaBase * pShooter = g_pCharaManager->getCharaByNetIdx( pRecvBuf->_tSufferData._nShooter);
	if( pShooter != NULL)
	{
		if( ((pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_A) ||
				(pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_H)) )
		{
			if( pShooter->isLocal() )
			{
				// 맞힌 유저가 자신이면 스킵
				return;
			}
			else
			{
				// 일반적인 무기로 맞은 경우
				pWeapon = pShooter->GetCurrentCharaWeapon();
			}

			if( (pRecvBuf->_tSufferData._nDeathType == CHARA_DEATH_H) && (pChara->IsDino() == false) )
			{
				pChara->getEquipContext()->setHelmetProtect( TRUE);
			}
		}

		i3Vector::Copy( &vShootPos, pShooter->GetPos());

		if( (pWeapon != NULL) &&
			(pWeapon->getWeaponInfo() != NULL) )
		{
			DamageInfo._WeaponType		= pWeapon->getWeaponInfo()->GetTypeClass();
			DamageInfo._nWeaponNumber	= pWeapon->getWeaponInfo()->GetNumber();
		}


		if( pShooter->IsOnObject() )
		{
			DamageInfo._WeaponType		= WEAPON_CLASS_OBJECT_GUN;
		}

		i3Vector::Sub( &vDir, pChara->GetPos(), &vShootPos);
		i3Vector::Normalize( &vDir, &vDir);
		DamageInfo._pVecDir = &vDir;
	}
	DamageInfo._pVecPos		= &vShootPos;

	pChara->Cmd_Damage( &DamageInfo);

	//Hit된경우 이동속도 느려지는 효과
	if( pChara->isLocal())
	{
		pChara->Cmd_SpeedReduction( (CHARA_HIT_PART) pRecvBuf->_tSufferData._nHitpart, pChara->getCurHP(), pRecvBuf->_tDefData.getHP());
	}

	return;
}

void CGameContext::_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient( GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf)
{
	VEC3D	vPos, normal, vDir;

	GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient start");

	I3ASSERT( g_pPlayer != NULL);

	if( isNetThrowWeapon( &pRecvBuf->_tWeaponData) == false)	return;
	if( pRecvBuf->_tFireData.isValidThrow( g_pPlayer->getCharaNetIndex()) == false )				return;

	GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = getGameInfo_ThrowWeapon( pRecvBuf->_tFireData._nNetSlotIdx);
	if( pThrowWeaponInfo->_pWeaponBase != NULL)
	{
		I3TRACE( "이미 사용중인 수류탄..\n");
		return;
	}

	g_pPlayer->getCameraContext()->getVirtualCamPosition( &vPos );
	i3Vector::Sub( &vPos, &vPos, g_pPlayer->GetPos());

	if( pCharaNetData->_tDefData._bOnLoadObject )
	{
		// local to world matrix
		INT32 nAnimObjectIdx = FindNetIdxForAnimObject( pCharaNetData->_tDefData._iOnLoadObjectIndex);
		if( nAnimObjectIdx != -1)
		{
			GAMEINFO_OBJECT * pObjInfo = getGameInfo_Object( nAnimObjectIdx);
			if( pObjInfo->_pObjectRef != NULL)
			{
				i3Object * pObject = pObjInfo->_pObjectRef;
				i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObject->getSceneObject() );
				if( pTrans != NULL)
				{
					i3Vector::TransformCoord( &pRecvBuf->_tFireData._vStartPos, &pRecvBuf->_tFireData._vStartPos, pTrans->GetCacheMatrix() );
				}
			}
		}
	}

	i3Vector::Add( &vPos, &vPos, &pCharaNetData->_tPosRotData._vPos);
	i3Vector::Sub( &vDir, &pRecvBuf->_tFireData._vTargetPos, &vPos );
	i3Vector::Normalize( &normal, &vDir );

	WeaponBase * pWeapon = g_pPlayer->getWeaponContext()->getWeapon( m_AssistInfo[ m_MySlotIdx]._nThrowSlotIdx);
	I3ASSERT( pWeapon != NULL);

	pWeapon->NET_FirstThrow( &vPos, &normal, pRecvBuf->_tFireData._nNetSlotIdx );

	GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient end");
}








