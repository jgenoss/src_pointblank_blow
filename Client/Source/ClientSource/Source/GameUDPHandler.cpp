/**
* \file	GameUDPHandler.cpp
*
* Implements the game udp handler class.
*/


#include "pch.h"
#include "GameUDPHandler.h"
#include "GameNetwork.h"
#include "StageBattle.h"
#include "StageObject/ObjectWithWeapon.h"
#include "StageObject/ObjectEvent.h"
#include "WeaponOnObject.h"
#include "GameObject.h"
#include "GameCharaContexts.h"
#include "GameStateMgr.h"
#include "Character/Fullbody/GameCharaFullbody.h"
#include "Character/Fullbody/FullbodyMisc.h"
#include "Character/Fullbody/GameCharaDomiSting.h"
#include "Character/Fullbody/DomiDinoGrenades.h"

#include "Mode/Domination/GameMode_Domination.h"
#include "Mode/Domination/DominationSkillSystem.h"
#include "Mode/Domination/DomiUserSkillMgr.h"
#include "Mode/Domination/DomiSkillObjBulletinBoard.h"
#include "GameDropItem.h"
#include "GameDropItemManager.h"

#include "BattleHud.h"
#include "ui/UIBattleFrame.h"
#include "ui/UIBattlePopupSkillShop.h"
#include "ui/hud/UIHUD_Domination_Manager.h"
#include "Mode/Domination/DomiStageProperties.h"
#include "Mode/Domination/DomiSkillProperties.h"
#include "UI/UIHudObserver.h"
#include "UI/UIHUDTabMinimapDomination.h"
#include "UI/Unit.h"
#include "UI/UIUtil.h"

#include "GameCharaManager.h"

#include "./StageBattle/AIModeContext.h"
#include "./StageBattle/ScoreContext.h"
#include "./StageBattle/UserContext.h"
#include "./StageBattle/NetworkContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"
#include "HackContext.h"
#include "GameContextUtil.h"
#include "UserInfoContext.h"

#include "GlobalVariables.h"

#include "GameMissionManager.h"
#include "MainWeapon_C5.h"
#include "MainWeapon_GrenadeShell.h"
#include "MainWeapon_DualCIC.h"
#include "MainWeapon_DualSMG.h"
#include "MainWeapon_Claymore.h"
#include "MainWeapon_Grenade.h"

#include "BattleSyncTimer.h"
#include "GameMaster.h"
#include "Weapon/WeaponItemID.h"

#include "i3Base/string/ext/mb_to_utf16.h"

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

#define UDP_PINGSENDTIME		10.f				// CS -> MS 타임


CGameUDPHandler::CGameUDPHandler()
{
	m_pUDPReceiver = nullptr;

	m_rElapsedTime = 0.f;

	InitRound();
}

CGameUDPHandler::~CGameUDPHandler()
{
	InitRound();

	UDPDisconnect();

	I3_SAFE_RELEASE(m_pUDPReceiver);
}

const GAMEINFO_CHARACTER * CGameUDPHandler::getGameInfo_Chara(INT32 userIdx, CHARA::BUFFER_TYPE buffer_type) const
{
	return BattleSlotContext::i()->getGameInfo_Chara(userIdx, buffer_type);
}

GAMEINFO_CHARACTER * CGameUDPHandler::setGameInfo_Chara(INT32 userIdx, CHARA::BUFFER_TYPE buffer_type)
{
	return BattleSlotContext::i()->setGameInfo_Chara(userIdx, buffer_type);
}

const GAMEINFO_CHARA_NETDATA * CGameUDPHandler::getNetInfo_Chara(INT32 userIdx, CHARA::BUFFER_TYPE buffer_type) const
{
	return &(BattleSlotContext::i()->getGameInfo_Chara(userIdx, buffer_type)->_tNetData);
}

GAMEINFO_CHARA_NETDATA * CGameUDPHandler::setNetInfo_Chara(INT32 userIdx, CHARA::BUFFER_TYPE buffer_type)
{
	return &(BattleSlotContext::i()->setGameInfo_Chara(userIdx, buffer_type)->_tNetData);
}

void CGameUDPHandler::UDPConnect(bool bHost)
{
	if (m_pUDPReceiver == nullptr)
	{
		m_pUDPReceiver = UDPPacketReceiver::new_object();
		m_pUDPReceiver->Create(PRIORITY_NORMAL);
	}

	m_pUDPReceiver->Connect( MyRoomInfoContext::i()->getStage()->GetStageMode() );


	//Create UDP Sender 방장일때 아닐때 다른것이 생성됨
	if (bHost)
	{
		CMulticastSystem::i()->Create();			// 멀티캐스터 세팅
	}
	else
	{
		UDPPacketSender::i()->Create();			// Sender create
	}
}

void CGameUDPHandler::UDPDisconnect(void)
{
	CMulticastSystem::i()->Destroy();
	UDPPacketSender::i()->Destroy();

	if (m_pUDPReceiver != nullptr)
		m_pUDPReceiver->Destroy();
}

void CGameUDPHandler::ChangeHost(bool bHost)
{
	//새로운 방장이 나일때 동작해야함. 
	if (bHost && (BattleServerContext::i()->IsUseDedicationMode() == false))
	{
		// 이전의 PacketSender는 해제
		UDPPacketSender::i()->Destroy();

		// 멀티캐스트 시스템을 설정합니다.
		CMulticastSystem::i()->Create();
	}
}

void CGameUDPHandler::InitRound(void)
{
	INT32 i;

	for (i = 0; i < SLOT_MAX_COUNT; i++)
	{
		m_AssistInfo[i].Reset();
		m_fUDPConnectTime[i] = 0.f;

		m_SendAssist[i]._SendCountForHitByObj = 0;
		for (UINT32 j = 0; j < m_SendAssist[i]._CharaDamageInfoList.size(); j++)
		{
			CHARA_DAMAGEINFO * pInfo = m_SendAssist[i]._CharaDamageInfoList.at(j);
			I3_SAFE_DELETE(pInfo);
		}
		m_SendAssist[i]._CharaDamageInfoList.clear();

		if (m_pUDPReceiver != nullptr)
			m_pUDPReceiver->resetRecvTime(i);

	}
}

void CGameUDPHandler::InitUserInfo(INT32 userIdx)
{
	I3_BOUNDCHK(userIdx, SLOT_MAX_COUNT);

	setNetInfo_Chara(userIdx)->_tMissionData.Reset();
	setGameInfo_Chara(userIdx)->ResetSendCount();
	setGameInfo_Chara(userIdx)->ResetSendCountForHost();

	m_AssistInfo[userIdx].Reset();
	m_fUDPConnectTime[userIdx] = 0.f;
	ResetRecvTime(userIdx);
}

void CGameUDPHandler::StartBattle(INT32 userIdx)
{
	m_fUDPConnectTime[userIdx] = 0.f;
}

void CGameUDPHandler::ResetRecvTime(INT32 userIdx)
{
	if (m_pUDPReceiver != nullptr)
		m_pUDPReceiver->resetRecvTime(userIdx);
}

void CGameUDPHandler::RespawnChara(INT32 userIdx)
{
	CMulticastSystem::i()->ResetBuffer(userIdx);
}

void CGameUDPHandler::RequestIntrudeProcess(void)
{
	CMulticastSystem::i()->RequestIntrudeProcess();
}

/***********************************************************************************/

void CGameUDPHandler::WriteGameEvent_Fire(CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon)
{
	I3ASSERT(pWeapon != nullptr);

	if (pChara->isCharaStateMask(CHARA_STATE_DEATH))	return;

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();

	bool bExtension = pWeapon->isExtensionActivate();
	if (pWeapon->getPairWeapon() != nullptr)
	{
		bExtension = pWeapon->getPairWeapon()->isExtensionActivate();
		pWeaponInfo = pWeapon->getPairWeapon()->getWeaponInfo();
	}

	pSendBuffer->SetInfo_Fire(pWeaponInfo->GetItemID(), bExtension, pWeapon->GetAttackType());

	pSendBuffer->_tFireData.setStartPosition(pChara->GetPos());
	pSendBuffer->_tFireData.setTargetPosition(pTargetPos);
	pSendBuffer->_tFireData.setRand(g_pRand->GetCode());

	UINT16 id = pSendBuffer->_tFireData.getID();
	pWeapon->setNetworkIdx(id++);
	pSendBuffer->_tFireData.setID(id);

#if defined( CHECK_LOADEDBULLET)
	pSendBuffer->_tFireData.setLoadedBulletCount((UINT8)pWeapon->getLoadedBulletCount());
	pSendBuffer->_tFireData.setLoadedDualBulletCount((UINT8)pWeapon->getLoadedBulletDualCount());
#endif

	if (isNetThrowWeapon(&pSendBuffer->_tWeaponData))
	{

		if (pSendBuffer->_tFireData.getNetSlotIndex() < MAX_THROWWEAPON_SLOP * slotIdx)
			pSendBuffer->_tFireData.setNetSlotIndex((UINT16)(MAX_THROWWEAPON_SLOP * slotIdx));

		INT32 tryCount = 0;

		for (; ; )
		{
			if (tryCount > MAX_THROWWEAPON_SLOP)
			{
				I3TRACE("Fire Throw is over count...\n");
				return;
			}

			if (pSendBuffer->_tFireData.getNetSlotIndex() >= MAX_THROWWEAPON_SLOP * (slotIdx + 1))
			{
				pSendBuffer->_tFireData.setNetSlotIndex((UINT16)(MAX_THROWWEAPON_SLOP * slotIdx));
				setGameInfo_Chara(slotIdx)->_MaxThrowingIndex = -1;
			}

			const GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon(pSendBuffer->_tFireData.getNetSlotIndex());
			if (pThrowWeaponInfo->getWeaponBase() == nullptr)
			{
				if (getGameInfo_Chara(slotIdx)->_MaxThrowingIndex >= pSendBuffer->_tFireData.getNetSlotIndex())
				{
					pSendBuffer->_tFireData.addNetSlotIndex();
					tryCount++;
					I3TRACE("Throwing weapon index inserted midway: current value : %d incremented value : %d \n", getGameInfo_Chara(slotIdx)->_MaxThrowingIndex, pSendBuffer->_tFireData.getNetSlotIndex());
					continue;
				}

				setGameInfo_Chara(slotIdx)->_MaxThrowingIndex = pSendBuffer->_tFireData.getNetSlotIndex();
				I3TRACE("Final throwing weapon index : %d throwing weapon name : %s \n", pSendBuffer->_tFireData.getNetSlotIndex(), pWeapon->GetName());

				pWeapon->setNetworkIdx(pSendBuffer->_tFireData.getNetSlotIndex());

				break;
			}

			pSendBuffer->_tFireData.addNetSlotIndex();
			tryCount++;
		}

		m_AssistInfo[slotIdx]._nThrowState = GRENADE_THROW_SEND;
		m_AssistInfo[slotIdx]._nThrowSlotIdx = pWeaponInfo->GetTypeUsage();

		//I3TRACE( "Fire throw. %d\n", pSendBuffer->_tFireData._nNetSlotIdx);
	}

	//폭탄일때 보내지 않는다.
	if (!i3::kind_of<WeaponGrenade*>(pWeapon))
	{
		pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_FIRE);
		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
		setGameInfo_Chara(slotIdx)->_bSendFlag = true;
		setGameInfo_Chara(slotIdx)->_rCharaPosSendTime = 0.f;
	}
	else
	{
		if (false == BattleServerContext::i()->IsUseDedicationMode())
		{
			pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_FIRE);
			if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
				pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
			setGameInfo_Chara(slotIdx)->_bSendFlag = true;
			setGameInfo_Chara(slotIdx)->_rCharaPosSendTime = 0.f;
		}
	}
}

#ifdef DOMI_STING_POISON_GRENADE
void CGameUDPHandler::WriteGameEvent_Fire_DomiDinoGrenade(CGameCharaBase * pChara, VEC3D * pTargetPos, WeaponBase * pWeapon)
{
	I3ASSERT(pWeapon != nullptr);

	if (pChara->isCharaStateMask(CHARA_STATE_DEATH))	return;

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx, CHARA::BUFFER_NEW_AI);

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT(pWeaponInfo != nullptr);

	pSendBuffer->SetInfo_Fire(pWeaponInfo->GetItemID(), false, pWeapon->GetAttackType());

	pSendBuffer->_tFireData.setStartPosition(pChara->GetPos());
	pSendBuffer->_tFireData.setTargetPosition(pTargetPos);
	pSendBuffer->_tFireData.setRand(g_pRand->GetCode());
	pSendBuffer->_tFireData.setID(pSendBuffer->_tFireData.getID() + 1);

#if defined( CHECK_LOADEDBULLET)
	pSendBuffer->_tFireData.setLoadedBulletCount((UINT8)pWeapon->getLoadedBulletCount());
	pSendBuffer->_tFireData.setLoadedDualBulletCount((UINT8)pWeapon->getLoadedBulletDualCount());
#endif

	if (isNetThrowWeapon(&pSendBuffer->_tWeaponData))
	{
		m_AssistInfo[slotIdx]._nThrowState = GRENADE_THROW_SEND;
		m_AssistInfo[slotIdx]._nThrowSlotIdx = pWeaponInfo->GetTypeUsage();
	}

	pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE | P2P_INFO_FLAG_FIRE);

	setGameInfo_Chara(slotIdx, CHARA::BUFFER_NEW_AI)->_rCharaPosSendTime = 0.f;
	setGameInfo_Chara(slotIdx, CHARA::BUFFER_NEW_AI)->_bSendFlag = true;
}
#endif


void CGameUDPHandler::WriteGameEvent_Hit(CGameCharaBase * pChara, const CHARA_DAMAGEINFO * pDamageInfo, bool bByObj)
{
	ISNETWORKRETURN(false)

		INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index.");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

		REAL32 rMaxValue = 255.f;

		if (i3::same_of<WeaponDinoExplosion*>(pDamageInfo->_pWeapon))
			rMaxValue = 500.0f;

#if defined(I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if (pDamageInfo->_HitType == GHT_AI)
		{
			I3_BOUNDCHK(pDamageInfo->_nVictimIdx, MAX_COUNT_DOMI_DINO);
			I3_BOUNDCHK(pDamageInfo->GetHitPart(), CHARA_HIT_MAXCOUNT);
		}
		else if (pDamageInfo->_HitType == GHT_CHARA)
		{
			I3_BOUNDCHK(pDamageInfo->_nVictimIdx, SLOT_MAX_COUNT);
			I3_BOUNDCHK(pDamageInfo->GetHitPart(), CHARA_HIT_MAXCOUNT);
		}
		else if (pDamageInfo->_HitType == GHT_ITEM_C5)
		{
			I3_BOUNDCHK(pDamageInfo->_nVictimIdx, MAX_THROWWEAPON_COUNT);
		}
		else
#endif
			if (pDamageInfo->_HitType == GHT_OBJECT)
			{
				I3_BOUNDCHK(pDamageInfo->_nVictimIdx, (INT32)BattleObjContext::i()->getObjectCountInfo()->m_pTotalObjectList.size());
				I3_BOUNDCHK(pDamageInfo->_ObjNetType, I3_NETWORK_GAMEOBJECT_TYPE_COUNT);

				rMaxValue = 1500.f;
			}

		INT32 idx = pSendBuffer->_tFireData._tHitData.getHitCount();

		if (idx == MAX_FIREHIT_COUNT)
		{	// 더 이상 추가할 수 없다.
			return;
		}

		pSendBuffer->_tFireData._tHitData.setHitType(idx, pDamageInfo->_HitType);
		pSendBuffer->_tFireData._tHitData.setHitWho(idx, (UINT16)pDamageInfo->_nVictimIdx);

		//오브젝트는 히트타입에 I3_NETWORK_GAMEOBJECT_TYPE을 세팅하므로 기존코드를 사용합니다.
		if (pDamageInfo->_HitType == GHT_OBJECT)
		{
			pSendBuffer->_tFireData._tHitData.setHitPart(idx, (UINT8)pDamageInfo->_ObjNetType);
		}
		else
		{
#if defined (_SHUFFLE_HIT_PART)
			pSendBuffer->_tFireData._tHitData.setHitPart(idx, (UINT8)HitPartContext::i()->EncryptHitPart(pDamageInfo->GetHitPart()));
#else
			pSendBuffer->_tFireData._tHitData.setHitPart(idx, (UINT8)pDamageInfo->GetHitPart());
#endif
		}

		REAL32 rDamage = pDamageInfo->_rDamage;
		//헤드샷을 할 경우 각 방마다 유니크한 값을 부쳐서 서버와 판별합니다.
		CGameCharaBase * pVictim = nullptr;

		if (pDamageInfo->_HitType == GHT_CHARA)
		{
			pVictim = g_pCharaManager->getCharaByNetIdx(pDamageInfo->_nVictimIdx);
		}

		if (pVictim != nullptr)
		{
			if (pDamageInfo->GetHitPart() == CHARA_HIT_HEAD && !pVictim->IsDino())
			{
				rDamage = static_cast<REAL32>(CalcHeadShotDamage(HitPartContext::i()->HitPartIndex, CHARA_HIT_MAXCOUNT,
					(UINT8)UserInfoContext::i()->GetMyRespawnCnt(), BattleSlotContext::i()->getMySlotIdx()));
				I3TRACE("Headshot Damage Test : %f \n", rDamage);
			}
		}

		if (MyRoomInfoContext::i()->getStage()->IsOneShotOneKill()
			&& pDamageInfo->_HitType == GHT_CHARA
			&& pDamageInfo->_pWeapon
			&&!(i3::same_of<WeaponWPSmoke*>(pDamageInfo->_pWeapon)))
			rDamage = rDamage * 35;

		rDamage = MINMAX(0.f, rDamage, rMaxValue);

		pSendBuffer->_tFireData._tHitData.setHitDamage(idx, (UINT16)rDamage);

		// Dedicated Mode용 Packet 설정
		pSendBuffer->_tFireData._tHitData.setHelmetFlag(idx, pDamageInfo->_HelmetFlag);
		pSendBuffer->_tFireData._tHitData.setWeaponID(idx, pDamageInfo->_ui32BaseWeaponID);
		pSendBuffer->_tFireData._tHitData.setWeaponExtension(idx, pDamageInfo->_bExtension);

		if (pDamageInfo->_pVecDir != nullptr)
			pSendBuffer->_tFireData._tHitData.setHitDirection(idx, pDamageInfo->_pVecDir);
		else
		{
			VEC3D vZero;
			pSendBuffer->_tFireData._tHitData.setHitDirection(idx, &vZero);
		}
		pSendBuffer->_tFireData._tHitData.setDeathType(idx, pDamageInfo->_DeathType);
		pSendBuffer->_tFireData._tHitData.setByObjectState(idx, pDamageInfo->_ByObjectState);
		pSendBuffer->_tFireData._tHitData.setByObjectIndex(idx, pDamageInfo->_ByObjectIndex);

		pSendBuffer->_tFireData._tHitData.setFirePos(idx, &pDamageInfo->_vStartPos);
		pSendBuffer->_tFireData._tHitData.setHitPos(idx, &pDamageInfo->_vHitPos);

		UINT8 critical_flag = (pDamageInfo->_pWeapon != nullptr) ? (UINT8)pDamageInfo->_pWeapon->GetWeaponDamage()->IsCritical() : 0;
		pSendBuffer->_tFireData._tHitData.setCriticalFlag(idx, critical_flag);
		pSendBuffer->_tFireData._tHitData.SetAttackType(idx, (UINT8)pDamageInfo->_AttackType);

		// revision 62135
		UINT16 slot = 0;
		for (INT8 i = 0; i < SLOT_MAX_COUNT; i++)
		{
			slot |= pDamageInfo->_HitByObjExplosion[i] << i;
		}

		pSendBuffer->_tFireData._tHitData.setHitbyObjectExplosion(idx, slot);

		if (BattleServerContext::i()->IsUseDedicationMode())
		{	// KEYWORD : DEDICATED2
			// DeathType에 따라 Packet 타입을 따로 보내주도록 합니다. 2011.11.10 swoongoo.
			if (pDamageInfo->_DeathType == CHARA_DEATH_B ||
				pDamageInfo->_DeathType == CHARA_DEATH_POISON ||
				pDamageInfo->_DeathType == CHARA_DEATH_BUFF ||
				pDamageInfo->_DeathType == CHARA_DEATH_HOWL ||
				pDamageInfo->_DeathType == CHARA_DEATH_F)
			{
				pSendBuffer->_tFireData._tHitData.setByType(idx, GHBT_EXPLOSION);
				pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_HITBYEXPLOSION;

				// 애시드 자폭으로 자신이 죽을 경우 죽을때의 체력을 저장해둔다.
				if (119016 == pSendBuffer->_tWeaponData.getWeaponID())
				{
					CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
					if (i3::kind_cast<CGameCharaDinoAcid*>(pMyChara))
					{
						pMyChara->setCharaInfo()->SaveHP();
					}
				}
			}
			else if (pDamageInfo->_DeathType == CHARA_DEATH_O ||
				pDamageInfo->_DeathType == CHARA_DEATH_FAST_OBJECT)
			{
				pSendBuffer->_tFireData._tHitData.setByType(idx, GHBT_OBJECT);
				pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_HITBYOBJECT;

				if (bByObj == true)
				{	// Object에 의해서 설정된 경우 버퍼링 한다.
					m_SendAssist[slotIdx]._SendCountForHitByObj = 4;

					CHARA_DAMAGEINFO * pNewInfo = new CHARA_DAMAGEINFO;
					MEMDUMP_NEW(pNewInfo, sizeof(CHARA_DAMAGEINFO));
					i3mem::Copy(pNewInfo, pDamageInfo, sizeof(CHARA_DAMAGEINFO));

					pNewInfo->_pVecPos = nullptr;// 이 포인터는 없어지기 때문에 nullptr로 설정해야 한다.
					pNewInfo->_pVecDir = nullptr;// 이 포인터는 없어지기 때문에 nullptr로 설정해야 한다.
					pNewInfo->_pWeapon = nullptr;
					pNewInfo->_pWeaponInfo = nullptr;
					pNewInfo->_pHitResult = nullptr;
					pNewInfo->_pWallHitResult = nullptr;

					m_SendAssist[slotIdx]._CharaDamageInfoList.push_back(pNewInfo);
				}
			}
			else
			{
				pSendBuffer->_tFireData._tHitData.setByType(idx, GHBT_CHARA);
				pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_HITBYCHARA;
			}
		}
		else
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_HIT;

		idx++;
		I3ASSERT(idx <= MAX_FIREHIT_COUNT);

		pSendBuffer->_tFireData._tHitData.setHitCount(idx);
	}

	GAMEINFO_CHARACTER * pCharaInfo = setGameInfo_Chara(slotIdx);
	pCharaInfo->_bSendFlag = true;

	//GlobalFunc::PB_TRACE("WriteGameEvent_Hit %d %d", pDamageInfo->_WeaponType, pDamageInfo->_nWeaponNumber);

	I3TRACE("Send Hit(%d) %d -> %d (%d)\n", pDamageInfo->_HitType, slotIdx, pDamageInfo->_nVictimIdx, pDamageInfo->_DeathType);
}

void CGameUDPHandler::WriteGameEvent_DropWeapon(CGameCharaBase * pChara, WEAPON_SLOT_TYPE iWeaponSlot, INT32 iLoadedBullet, INT32 iDualLoadedBullet, INT32 iTotalBullet)
{
	WeaponBase * pWeapon = nullptr;

	if (iWeaponSlot > WEAPON_SLOT_SECONDARY)	return;

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

		if (BattleServerContext::i()->IsP2PHost())
		{
			pSendBuffer->_tDropWeaponData.Reset();

			if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
			{
				pChara->getMultiWeaponContext()->SetPickupProhibit( true);
			}

			INT16	nDropWeaponIdx = BattleObjContext::i()->getDropedWeaponCount() % MAX_DROPEDWEAPON_COUNT;
			pWeapon = g_pWeaponManager->SetDropWeapon(slotIdx, iWeaponSlot, nDropWeaponIdx, !pChara->isCharaStateMask(CHARA_STATE_DEATH));

			if (pWeapon != nullptr)
			{	// 성공하면 패킷을 세팅합니다.
				CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
				I3ASSERT(pWeaponInfo != nullptr);

				// 드랍된 무기에 대한 정보를 세팅합니다.
				{
					GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(nDropWeaponIdx);
					pDropedWeaponInfo->Bind(pWeapon, pChara);
				}

				// 해당 무기에 탄약 정보를 세팅
				{
					pWeapon->SetBulletCountForNetwork(iLoadedBullet, iDualLoadedBullet, iTotalBullet);

					if (pChara->isCharaStateMask(CHARA_STATE_DEATH))
					{	// Death flag
						pSendBuffer->_tDropWeaponData.setSuccess(1);
						pSendBuffer->_tDropWeaponData.setDropType(1);
					}
					else
					{
						pSendBuffer->_tDropWeaponData.setSuccess(1);
						pSendBuffer->_tDropWeaponData.setDropType(0);
					}

					pSendBuffer->_tDropWeaponData.setIndex((UINT8)nDropWeaponIdx);
				}

				nDropWeaponIdx++;
				BattleObjContext::i()->SetDropedWeaponCount((UINT16)nDropWeaponIdx);
			}
			else
			{
				for (INT32 i = 0; i < MAX_DROPEDWEAPON_COUNT; i++)
				{
					const GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->getGameInfo_DropedWeapon(i);

					if (pDropedWeaponInfo->_pWeaponRef == nullptr)												continue;
					if (pDropedWeaponInfo->_pWeaponRef->getOwner() != pChara)								continue;
					if (pDropedWeaponInfo->_pWeaponRef->getWeaponInfo()->GetTypeUsage() != iWeaponSlot)		continue;
					if (pDropedWeaponInfo->_pWeaponRef->IsDroped() == true)									continue;

					if (pChara->isCharaStateMask(CHARA_STATE_DEATH))
					{	// Death flag
						pSendBuffer->_tDropWeaponData.setSuccess(1);
						pSendBuffer->_tDropWeaponData.setDropType(1);
					}
					else
					{
						pSendBuffer->_tDropWeaponData.setSuccess(1);
						pSendBuffer->_tDropWeaponData.setDropType(0);
					}

					pSendBuffer->_tDropWeaponData.setIndex((UINT8)i);

					pWeapon = pDropedWeaponInfo->_pWeaponRef;
					break;
				}

				if (pWeapon == nullptr)	return;
			}
		}
		else
		{
			pWeapon = pChara->GetCharaWeapon(iWeaponSlot);
			if (pWeapon == nullptr)									return;
			if (pWeapon->getWeaponInfo()->isEnableDrop() == false)
				return;

			pSendBuffer->_tDropWeaponData.setIndex((UINT8)0);
			pSendBuffer->_tDropWeaponData.setSuccess(1);

			// Death flag
			if ( pChara->isCharaStateMask( CHARA_STATE_DEATH))
				pSendBuffer->_tDropWeaponData.setDropType(1);
			else
				pSendBuffer->_tDropWeaponData.setDropType(0);
		}

		if ( pChara->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr
			&& iWeaponSlot == WEAPON_SLOT_PRIMARY
			&& MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			UINT8 nMultiWeaponIndex = pChara->getMultiWeaponContext()->GetCurrSlotInfo()->nSyncNumber;
			pSendBuffer->_tDropWeaponData.SetMultiWeaponSlot( nMultiWeaponIndex);
		}
		else
		{
			pSendBuffer->_tDropWeaponData.SetMultiWeaponSlot(0);
		}

		ISNETWORKRETURN(false)

			pSendBuffer->_tDropWeaponData.setWeaponID(pWeapon->getAbsoluteWeaponInfo()->GetItemID());
		pWeapon->GetNetBullet(pSendBuffer->_tDropWeaponData.GetBullet());

		//pSendBuffer->_tDropWeaponData.setWeaponID(pWeapon->getAbsoluteWeaponInfo()->GetItemID());
		//pSendBuffer->_tDropWeaponData.setLoadedBulletCount((UINT8)pWeapon->getLoadedBulletCount());
		//pSendBuffer->_tDropWeaponData.setDualLoadedBulletCount((UINT8)pWeapon->getLoadedBulletDualCount());
		//pSendBuffer->_tDropWeaponData.setMaxBulletCount((UINT16)pWeapon->getTotalBulletCount());
		I3TRACE("DropWeapon Index : %d\n", pSendBuffer->_tDropWeaponData.getIndex());

		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DROPWEAPON;
	}

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;
}

void CGameUDPHandler::WriteGameEvent_PickupWeapon(CGameCharaBase * pChara, WeaponBase * pWeapon)
{
	if (BattleServerContext::i()->IsP2PHost())
	{
		pChara->Cmd_PickupWeapon(pWeapon);

		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(pWeapon->getNetworkIdx());
		pDropedWeaponInfo->Reset();
	}

	ISNETWORKRETURN(false)

		INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			pChara->getMultiWeaponContext()->SetDropProhibit(true);
		}

		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);
		pSendBuffer->_tGetWeaponData.setSuccess(1);
		pSendBuffer->_tGetWeaponData.setDropType(0);
		pSendBuffer->_tGetWeaponData.setIndex((UINT8)pWeapon->getNetworkIdx());
		pSendBuffer->_tGetWeaponData.setWeaponID(pWeapon->getWeaponInfo()->GetItemID());
		pWeapon->GetNetBullet(pSendBuffer->_tGetWeaponData.GetBullet());
		//pSendBuffer->_tGetWeaponData.setLoadedBulletCount( (UINT8) pWeapon->getLoadedBulletCount());
		//pSendBuffer->_tGetWeaponData.setDualLoadedBulletCount( (UINT8) pWeapon->getLoadedBulletDualCount());
		//pSendBuffer->_tGetWeaponData.setMaxBulletCount( (UINT16) pWeapon->getTotalBulletCount());
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_GETWEAPON;
	}

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;
}

void CGameUDPHandler::WriteGameEvent_DirectPickupWeapon(CGameCharaBase * pChara, T_ItemID GetItemID)
{
	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}
	
	ISNETWORKRETURN(false)

	GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

	WEAPON_SLOT_TYPE slot = (WEAPON_SLOT_TYPE) (GET_ITEM_TYPE(GetItemID) - 1);
	WeaponBase * pDropWeapon = pChara->GetCharaWeapon(slot);

	if (pDropWeapon == nullptr)									return;
	if (pDropWeapon->getWeaponInfo()->isEnableDrop() == false)
		return;

	if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		pChara->getMultiWeaponContext()->SetPickupProhibit( true);
		pChara->getMultiWeaponContext()->SetDropProhibit( true);
	}

	pSendBuffer->_tDirectPickUp.setWeaponID(GetItemID);
	pSendBuffer->_tDirectPickUp.getDropWeapon()->setIndex((UINT8)0);
	if (pChara->isCharaStateMask(CHARA_STATE_DEATH))
	{	// Death flag
		pSendBuffer->_tDirectPickUp.getDropWeapon()->setSuccess(1);
		pSendBuffer->_tDirectPickUp.getDropWeapon()->setDropType(1);
	}
	else
	{
		pSendBuffer->_tDirectPickUp.getDropWeapon()->setSuccess(1);
		pSendBuffer->_tDirectPickUp.getDropWeapon()->setDropType(0);
	}

	pSendBuffer->_tDirectPickUp.getDropWeapon()->setWeaponID(pDropWeapon->getAbsoluteWeaponInfo()->GetItemID());
	pDropWeapon->GetNetBullet(pSendBuffer->_tDirectPickUp.getDropWeapon()->GetBullet());

	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DIRECTPICKUP;

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;
}

void CGameUDPHandler::WriteGameEvent_Mission(CGameCharaBase * pChara, NET_MISSION_STATE nState, BOMB_AREA nArea, T_ItemID ItemId
#ifdef ALTER_DOMI_INSTALL_OBJ
	, DOMI_TYPE ObjType, INT16 ObjIdx
#else
	, INT32 ObjIdx, REAL32 fTime = 0.0f
#endif
	)
{
	ISNETWORKRETURN(false)

		INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

	if (pSendBuffer->_tMissionData.GetState() != nState)
	{
		pSendBuffer->_tMissionData.SetState(nState);
		pSendBuffer->_tMissionData.SetArea(nArea);
		pSendBuffer->_tMissionData._ItemID = ItemId;
#ifdef ALTER_DOMI_INSTALL_OBJ
		pSendBuffer->_tMissionData._ObjType = static_cast<INT16>(ObjType);
		pSendBuffer->_tMissionData._ObjIdx = ObjIdx;
#endif
		pSendBuffer->_tMissionData.SetStartTime(fTime);
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_MISSION;

		{
			GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(slotIdx);
			pSendCharaInfo->_rMissionSyncSendTime = 0.f;
			pSendCharaInfo->_bSendFlag = true;
		}
	}
}

void CGameUDPHandler::WriteGameEvent_SuicideDamage(CGameCharaBase * pChara,
	CHARA_DEATH_TYPE type, CHARA_HIT_PART part, UINT16 nDamage, VEC3D * pDir,
	WEAPON_CLASS_TYPE WeaponClass, INT32 WeaponNum, UINT16 iByObjectIdx,
	INT32 KillerIdx, WEAPON_SLOT_TYPE WeaponUsage)
{
	I3ASSERT(pChara != nullptr);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

		if (pSendBuffer->_tSuicideDamageData.getCount() >= MAX_SUICIDE_COUNT)
		{	// Death Object에 낀 경우 계속 들어 올수 있습니다.
			return;
		}

		INT32 countIdx = pSendBuffer->_tSuicideDamageData.getCount();
		pSendBuffer->_tSuicideDamageData.setCount(countIdx + 1);

		I3ASSERT(part > CHARA_HIT_UNKNOWN);
#if defined (_SHUFFLE_HIT_PART)
		UINT8 CovertHitPart = HitPartContext::i()->EncryptHitPart(part);
		pSendBuffer->_tSuicideDamageData.setHitPart(countIdx, (CHARA_HIT_PART)CovertHitPart);
#else
		pSendBuffer->_tSuicideDamageData.setHitPart(countIdx], part);
#endif

		pSendBuffer->_tSuicideDamageData.setDeathType(countIdx, type);
		pSendBuffer->_tSuicideDamageData.setDamage(countIdx, nDamage);
		pSendBuffer->_tSuicideDamageData.setKillerIndex(countIdx, KillerIdx);
		pSendBuffer->_tSuicideDamageData.setWeaponID(countIdx, Weapon::GetItemID(WeaponClass, WeaponNum));
		//pSendBuffer->_tSuicideDamageData.setWeaponExtension( countIdx, bWeaponExtension);

		if (pDir != nullptr)
			pSendBuffer->_tSuicideDamageData.setHitDirection(countIdx, pDir);
		else
			pSendBuffer->_tSuicideDamageData.zeroHitDirection(countIdx);

		pSendBuffer->_tSuicideDamageData.setObjectIndex(countIdx, iByObjectIdx);
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SUICIDEDAMAGE;
	}

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;

	//I3ASSERT( slotIdx == KillerIdx);
}

void CGameUDPHandler::WriteGameEvent_RadioChat(CGameCharaBase * pChara, UINT8 nRadio)
{
	ISNETWORKRETURN(false)

		INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);
		pSendBuffer->_tRadioChat.setRadio(nRadio);
		pSendBuffer->_tRadioChat.setSectorIndex((UINT8)pChara->getPortalID());
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_RADIO;
	}

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;
}

void CGameUDPHandler::WriteGameEvent_UseObject(CGameCharaBase * pChara, i3Object * pObj, UINT8 nActionKey)
{
	I3_BOUNDCHK(pObj->getNetIndex(), 256);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);
		UINT8 count = pSendBuffer->_tActionKey.getActionCount();
		if (count >= MAX_ACTION_COUNT) return;

		// Count 증가
		count++;
		pSendBuffer->_tActionKey.setActionCount(count);
		pSendBuffer->_tActionKey.setActionKey(count - 1, nActionKey);
		pSendBuffer->_tActionKey.setUseActionObjectIndex(count - 1, (UINT8)pObj->getNetIndex());
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;
	}

	setGameInfo_Chara(slotIdx)->_bSendFlag = true;

}

void CGameUDPHandler::WriteGameEvent_UseObjectSync(CGameCharaBase * pChara, i3Object * pObj, UINT8 nSyncActionKey)
{
	// Client는 처리하지 않습니다.
	if (!BattleServerContext::i()->IsP2PHost())
		return;

	I3_BOUNDCHK(pObj->getNetIndex(), 256);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);
		pSendBuffer->_tSyncObject.setIndex((UINT16)pObj->getNetIndex());
		pSendBuffer->_tSyncObject.setState(nSyncActionKey);
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(slotIdx);
		pSendCharaInfo->_rObjectSyncSendTime = 0.f;
		pSendCharaInfo->_bSendFlag = true;
	}
}

void CGameUDPHandler::WriteGameEvent_SufferingDamage(CGameCharaBase * pChara, CHARA_DEATH_TYPE type, INT32 nShooter, CHARA_HIT_PART nHitPart, bool bIsNeedDamageMotion)
{
	ISNETWORKRETURN(false)

		I3ASSERT(BattleServerContext::i()->IsP2PHost() == true);

	I3_BOUNDCHK(nShooter, SLOT_MAX_COUNT);
	I3_BOUNDCHK(nHitPart, CHARA_HIT_MAXCOUNT);

	INT32 slotIdx = pChara->getCharaNetIndex();
	if (slotIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN, "invalid character index..");
		return;
	}

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

		I3ASSERT(nHitPart > CHARA_HIT_UNKNOWN);
#if defined (_SHUFFLE_HIT_PART)
		UINT8 CovertHitPart = HitPartContext::i()->EncryptHitPart(nHitPart);
		pSendBuffer->_tSufferData.setHitPart(CovertHitPart);
#else
		pSendBuffer->_tSufferData.setHitPart((UINT8)nHitPart);
#endif

		pSendBuffer->_tSufferData.setDeathType((UINT8)type);
		pSendBuffer->_tSufferData.setShooterIndex((UINT8)nShooter);
		pSendBuffer->_tSufferData.setNeedDamageMotion((UINT8)bIsNeedDamageMotion);

		pSendBuffer->_tHP.setHP((UINT16)pChara->getCharaInfo()->GetHP());
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CS_LIFE | P2P_INFO_FLAG_CS_SUFFERING;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(slotIdx);
		pSendCharaInfo->_rLifePacketSendTime = 0.f;
		pSendCharaInfo->_bSendFlag = true;
	}
}

//#ifdef DOMI_UDP_OBJECT
void CGameUDPHandler::WriteGameEvent_DomiSkillObject_Install(INT32 customerIdx, INT8 objectIdx)
{
	ISNETWORKRETURN(false);
	I3_BOUNDCHK(customerIdx, SLOT_MAX_COUNT);

	{
		GAMEINFO_CHARA_NETDATA* pSendBuffer = setNetInfo_Chara(customerIdx);
		pSendBuffer->_tDomiObjectData.install.i8Idx = objectIdx;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DOMI_INSTALL;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(customerIdx);
		pSendCharaInfo->_iDomiObjectSendCnt = 3;
		pSendCharaInfo->_bSendFlag = true;
		pSendCharaInfo->_iDomiObjectActivateFlag = P2P_INFO_FLAG_DOMI_INSTALL;
	}
}
void CGameUDPHandler::WriteGameEvent_DomiSkillObject_Repair(INT32 customerIdx, INT8 objectIdx)
{
	ISNETWORKRETURN(false);
	I3_BOUNDCHK(customerIdx, SLOT_MAX_COUNT);

	{
		GAMEINFO_CHARA_NETDATA* pSendBuffer = setNetInfo_Chara(customerIdx);
		pSendBuffer->_tDomiObjectData.repair.i8Idx = objectIdx;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DOMI_REPAIR;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(customerIdx);
		pSendCharaInfo->_iDomiObjectSendCnt = 3;
		pSendCharaInfo->_bSendFlag = true;
		pSendCharaInfo->_iDomiObjectActivateFlag = P2P_INFO_FLAG_DOMI_REPAIR;
	}
}
void CGameUDPHandler::WriteGameEvent_DomiSkillObject_Upgrade(INT32 customerIdx, INT8 objectIdx, INT8 lv)
{
	ISNETWORKRETURN(false);
	I3_BOUNDCHK(customerIdx, SLOT_MAX_COUNT);

	{
		GAMEINFO_CHARA_NETDATA* pSendBuffer = setNetInfo_Chara(customerIdx);
		pSendBuffer->_tDomiObjectData.upgrade.i8Idx = objectIdx;
		pSendBuffer->_tDomiObjectData.upgrade.i8Level = lv;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DOMI_UPGRADE;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(customerIdx);
		pSendCharaInfo->_iDomiObjectSendCnt = 3;
		pSendCharaInfo->_bSendFlag = true;
		pSendCharaInfo->_iDomiObjectActivateFlag = P2P_INFO_FLAG_DOMI_UPGRADE;
	}
}
void CGameUDPHandler::WriteGameEvent_DomiSkillObject_SupplyUse(INT32 customerIdx, INT8 objectIdx)
{
	ISNETWORKRETURN(false);
	I3_BOUNDCHK(customerIdx, SLOT_MAX_COUNT);

	{
		GAMEINFO_CHARA_NETDATA* pSendBuffer = setNetInfo_Chara(customerIdx);
		pSendBuffer->_tDomiObjectData.supplyUse.i8Idx = objectIdx;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DOMI_USE;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(customerIdx);
		pSendCharaInfo->_iDomiObjectSendCnt = 3;
		pSendCharaInfo->_bSendFlag = true;
		pSendCharaInfo->_iDomiObjectActivateFlag = P2P_INFO_FLAG_DOMI_USE;
	}
}
void CGameUDPHandler::WriteGameEvent_DomiDropItem_Catched(INT32 userIdx, INT8 objectIdx)
{
	ISNETWORKRETURN(false);
	I3_BOUNDCHK(userIdx, SLOT_MAX_COUNT);

	{
		GAMEINFO_CHARA_NETDATA* pSendBuffer = setNetInfo_Chara(userIdx);
		pSendBuffer->_tDomiObjectData.dropItem.i8Idx = objectIdx;
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_DOMI_CATCH;
	}

	{
		GAMEINFO_CHARACTER * pSendCharaInfo = setGameInfo_Chara(userIdx);
		pSendCharaInfo->_iDomiObjectSendCnt = 3;
		pSendCharaInfo->_bSendFlag = true;
		pSendCharaInfo->_iDomiObjectActivateFlag = P2P_INFO_FLAG_DOMI_CATCH;
	}
}
//#endif

namespace
{
	inline void FillThrowData(INT32 idx, GAMEINFO_THROW_WEAPON* pGrenadeInfo, GAMETOCONTEXT_INFO_WEAPON_THROW* pThrowWeaponInfo)
	{
		// 초기화된 수류탄입니다. 보내지 않습니다. 2008.06.19 dfly79
		if (pGrenadeInfo->_pWeaponBase == nullptr)
			return;

		bool	bCheckSend = false;
		UINT8	nStateFlag = pThrowWeaponInfo->_nAddStateFlag;

		pGrenadeInfo->_fSendTime += g_pViewer->getDeltaTime();

		if (nStateFlag & NET_GRENADE_STATE_SETTLED)
		{
			pGrenadeInfo->_tNetData.setRemainingTime(pThrowWeaponInfo->_ucRemainingTime);
			pGrenadeInfo->_tAssistData._SendCount = 0;	// 총 1회 보냅니다.
			bCheckSend = true;
		}
		else  if (nStateFlag & NET_GRENADE_STATE_EXPLOSION)
		{
			pGrenadeInfo->_tAssistData._SendCount = 0;
			bCheckSend = true;
		
		}
		else if(nStateFlag & NET_GRENADE_STATE_SLEEP)
		{
			// 현재 보내려는 타입이 싱크용이지만 슬립된 상태라면 횟수를 조절합니다.
			if (pGrenadeInfo->_fSendTime > pGrenadeInfo->_fBroadcastCheckSleepTime)
				bCheckSend = true;
		}
		else if(nStateFlag & NET_GRENADE_STATE_BOUND)
		{
			pGrenadeInfo->_tAssistData._SendCount = 1;
			bCheckSend = true;
		}
		else if(nStateFlag & NET_GRENADE_STATE_FLY)
		{
			pGrenadeInfo->_tAssistData._SendCount = 1;
			if (pGrenadeInfo->_fSendTime > pGrenadeInfo->_fBroadcastCheckSyncTime)
				bCheckSend = true;

			VEC3D vecDist;
			if (i3Vector::Length(pGrenadeInfo->_tNetData.getPrevPos()) != 0.0f)
			{
				i3Vector::Sub(&vecDist, pThrowWeaponInfo->_pPos, pGrenadeInfo->_tNetData.getPrevPos());
			}

#if defined( I3_DEBUG)
			REAL32 fLen = i3Vector::Length(&vecDist);
			I3TRACE("Send Fly Count : %d Distance : %.3f\n", pGrenadeInfo->_tNetData.getFlyCount(), fLen);
#endif

			pGrenadeInfo->_tNetData.setPrevPos(pThrowWeaponInfo->_pPos);
		}
		else
		{
			// 현재 보내려는 타입이 싱크용이 아니면 바로 보냅니다.
			bCheckSend = true;
		}

		if (nStateFlag & NET_GRENADE_STATE_TAKEOBJECT)
		{
			if (pThrowWeaponInfo->_AttachedObjPos != nullptr)
				pGrenadeInfo->_tNetData.SetAttachPos(*pThrowWeaponInfo->_AttachedObjPos);
		}

		if (bCheckSend)
		{
#if defined( I3_DEBUG) && 0
			if (nStateFlag == NET_GRENADE_STATE_FLY)
				I3TRACE("Send Fly\n");
			if (nStateFlag == NET_GRENADE_STATE_BOUND)
				I3TRACE("Send Bound\n");
			if (nStateFlag == NET_GRENADE_STATE_C5_SETTLED)
				I3TRACE("Send C5 Settled\n");
			if (nStateFlag == NET_GRENADE_STATE_SLEEP)
				I3TRACE("Send Sleep\n");
			if (nStateFlag == NET_GRENADE_STATE_SETTLED)
				I3TRACE("Send Settled %f %f %f\n", pThrowWeaponInfo->_pPos->x, pThrowWeaponInfo->_pPos->y, pThrowWeaponInfo->_pPos->z);
			if (nStateFlag == NET_GRENADE_STATE_TAKEOBJECT)
				I3TRACE("Send TakeObject\n");
			if (nStateFlag == NET_GRENADE_STATE_CLICKFIRE)
				I3TRACE("Send Clickfire\n");
			if (nStateFlag == NET_GRENADE_STATE_EXPLOSION)
				I3TRACE("Send Explosion\n");
#endif
			// send할 때 서버로 카운트 정보를 보낸다.
			UINT8 cnt = pGrenadeInfo->_tNetData.getFlyCount() + 1;
			if (cnt >= 255)
				cnt = 0;
			pGrenadeInfo->_tNetData.setFlyCount(cnt);

			INT32 GrenadeIdx = 0;
			GrenadeIdx = idx % MAX_THROWWEAPON_SLOP;

			pGrenadeInfo->_bSend = true;
			pGrenadeInfo->_fSendTime = 0.f;
			pGrenadeInfo->_tNetData.SetState(nStateFlag);

			pGrenadeInfo->_tNetData.setWeaponID(pThrowWeaponInfo->_ItemID);
			pGrenadeInfo->_tNetData.SetAttackType(pThrowWeaponInfo->_AttackType);
			pGrenadeInfo->_tNetData.setWeaponExtension(pThrowWeaponInfo->_bExtension);

			//I3TRACE("Settled Time : %d CharaSlot:%d\n",pGrenadeInfo->_RemainingTime,idx);

			if (nStateFlag == NET_GRENADE_STATE_BOUND ||
				(nStateFlag & NET_GRENADE_STATE_TAKEOBJECT))
			{
				I3_BOUNDCHK(pThrowWeaponInfo->_nBoundType, 255);
				pGrenadeInfo->_tNetData.SetBoundType((UINT8)pThrowWeaponInfo->_nBoundType);
			}

			if (pThrowWeaponInfo->_pPos != nullptr)
			{
				pGrenadeInfo->_tNetData.SetPos(*pThrowWeaponInfo->_pPos);

				VEC3D vCharaPos, vGrenadePos;

				CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

				i3Vector::Copy(&vCharaPos, pMyChara->GetPos());
				i3Vector::Copy(&vGrenadePos, pThrowWeaponInfo->_pPos);


				if (i3::same_of<WeaponK400*, WeaponDualGrenade*, WeaponC5*, WeaponThrowKnife*>(pGrenadeInfo->_pWeaponBase) &&
					(!(MyRoomInfoContext::i()->getStage()->IsAiMode()))
					)
				{
					GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].GrenadePosList.push_back(vGrenadePos);
					GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].CharaPosList.push_back(vCharaPos);
					GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].WeaponClass = pGrenadeInfo->_pWeaponBase->getWeaponInfo()->GetTypeClass();
					GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].WeaponNumber = pGrenadeInfo->_pWeaponBase->getWeaponInfo()->GetNumber();
				}
			}
			if (pThrowWeaponInfo->_pNormal != nullptr)
			{
				pGrenadeInfo->_tNetData.SetDir(*pThrowWeaponInfo->_pNormal);
			}

			if (nStateFlag == NET_GRENADE_STATE_EXPLOSION)
			{
				if (!(MyRoomInfoContext::i()->getStage()->IsAiMode()))
				{
					VEC3D vLen;
					REAL32 rDist;

					for (UINT32 i = 0; i < GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].GrenadePosList.size(); i++)
					{
						if (i == 0)
						{
							VEC3D grenade_pos = GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].GrenadePosList[0];

							i3Vector::Sub(&vLen, &grenade_pos, &grenade_pos);
							i3Vector::SetY(&vLen, 0.0f);
							rDist = i3Vector::Length(&vLen);

							if (rDist > 10.f)
							{
								if (LocaleValue::Enable("CheckGrenadePos"))
								{
									INT32 nTemp = (INT32)rDist;
									HackContext::i()->HackingDetected((REAL32)CC_GRENADE_POS + nTemp, C_GRENADE_POS);
								}
							}
						}
						else
						{
							VEC3D pos1 = GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].GrenadePosList[i - 1];
							VEC3D pos2 = GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].GrenadePosList[i];

							i3Vector::Sub(&vLen, &pos1, &pos2);
							rDist = i3Vector::Length(&vLen);
							if (rDist > 10.f)
							{
								if (LocaleValue::Enable("CheckGrenadePos"))
								{
									INT32 nTemp = (INT32)rDist;
									HackContext::i()->HackingDetected((REAL32)(CC_GRENADE_POS + nTemp), C_GRENADE_POS);
								}
							}
						}
					}

					GrenadeContext::i()->m_GrenadeCheck[GrenadeIdx].Reset();
				}
			}
			// GlobalFunc::PB_TRACE("WriteGameEvent_ThrowWeaponData _vPos %f %f %f", pGrenadeInfo->_tNetData._vPos.x, pGrenadeInfo->_tNetData._vPos.y, pGrenadeInfo->_tNetData._vPos.z);
		}
	}

	// 투척 무기 생성 후, Bind
	void BindNetThrowData(INT32 idx, GAMEINFO_THROW_WEAPON_NETDATA * pRecvBuf)
	{
		if (BattleServerContext::i()->IsUseDedicationMode() == false)
			return;

		if (pRecvBuf->GetState() == NET_GRENADE_STATE_EXPLOSION)
			return;

		INT32 nCharaSlotNum = idx / MAX_THROWWEAPON_SLOP;

		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(nCharaSlotNum);
		if (nullptr != pChara)
		{
			if (i3::same_of<CGameCharaDinoSting*>(pChara))
			{
				WEAPON::RES_ID ResID = WEAPON::getItemIndex(WEAPON::GSHELL_POISON);
				WeaponBase* pWeapon = g_pWeaponManager->GetFreeThrowWeapon(WEAPON_CLASS_GRENADESHELL, ResID);

				if (pWeapon != nullptr)
				{
					VEC3D vecTargetPos = *(pWeapon->GetPos());
					VEC3D vNetPos = pRecvBuf->GetPos();

					MainWeapon_GrenadeShell* mainWeapon = static_cast<MainWeapon_GrenadeShell*>(pWeapon->GetMainWeapon());
					mainWeapon->Throw(&vNetPos, &vecTargetPos, pWeapon->GetThrowSpeed(), false);

					GAMEEVENT::BindNetGrenadeInfo(pWeapon->getNetworkIdx(), pWeapon, &vNetPos);
				}
			}
			else
			{
				// 발사체 ItemID로 무기를 찾아야 한다.
				T_ItemID itemID = pRecvBuf->getWeaponID();
				CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(itemID);
				if (pWeaponInfo != nullptr)
				{
					WeaponBase * pWeapon = pChara->getWeaponContext()->FindWeaponByExtensionItemID(itemID);

					if (pWeapon == nullptr)
					{// 익스텐션이 아닌 무기 자체가 발사형태(수류탄)
						pWeapon = pChara->getWeaponContext()->getWeapon((WEAPON_SLOT_TYPE)WEAPON::ItemID2Slot(itemID));
					}

					if (pWeapon)
					{
						VEC3D vecTargetPos = *(pWeapon->GetPos());
						VEC3D vNetPos = pRecvBuf->GetPos();

						if (i3::same_of<WeaponBow*>(pWeapon))
						{
							VEC3D vDir;
							i3Vector::Sub(&vDir, &vecTargetPos, &vNetPos);
							i3Vector::Normalize(&vDir, &vDir);

							bool bExtension = false;
							if (WEAPON::ItemID2Class(itemID) != WEAPON_CLASS_KNIFE)
								bExtension = true;
							((WeaponBow*)pWeapon)->ThrowArrow(&vNetPos, &vDir, pWeapon->GetThrowSpeed(), idx, bExtension);
						}
						else
						{
							pWeapon->NET_Throw(&vNetPos, &vecTargetPos, idx);
							if (i3::kind_of<WeaponGrenade*>(pWeapon))
							{
								MainWeapon* pMainWeapon = pWeapon->GetMainWeapon();
								if (pMainWeapon != nullptr)
								{
									// Bullet 을 처리합니다. 이미 Net_Throw을 한다는 거 자체가 나 말고 다른 사람이 쏜 것이기 때문에.
									static_cast<MainWeapon_Grenade*>(pMainWeapon)->CheckBullet();
								}
							}
						}
					}
				}
			}
		}
	}

	// 투척 무기 Net 정보 설정 
	void SetNetThrowData(GAMEINFO_THROW_WEAPON * pInfo, GAMEINFO_THROW_WEAPON_NETDATA * pRecvBuf)
	{
		// 설정된 무기 정보가 없을 경우, 처리 안함.
		if (pInfo->_pWeaponBase == nullptr)
			return;

		pInfo->_tNetData.SetState(pRecvBuf->GetState());
		pInfo->_tNetData.SetBoundType(pRecvBuf->GetBoundType());
		pInfo->_tNetData.SetPos(pRecvBuf->GetPos());
		pInfo->_tNetData.SetDir(pRecvBuf->GetDir());
		pInfo->_tNetData.setRemainingTime(pRecvBuf->getRemainingTime());
		pInfo->_tNetData.SetAttachPos(pRecvBuf->GetAttachPos());

		i3Vector::Copy(&pInfo->_tAssistData._vPostPos, ((WeaponBase*)pInfo->_pWeaponBase)->GetPos());
		pInfo->_tAssistData._fInterpolateTime = 0.f;
		pInfo->_bRecv = true;

		//I3TRACE("받은 궤적 : OLD: %f, %f, %f DEST: %f, %f, %f\n", pInfo->_tNetData._vPos.x, pInfo->_tNetData._vPos.y, pInfo->_tNetData._vPos.z, 
		//	pInfo->_tAssistData._vPostPos.x, pInfo->_tAssistData._vPostPos.y, pInfo->_tAssistData._vPostPos.z);
	}
}

void CGameUDPHandler::WriteGameEvent_ThrowWeaponData(INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo)
{
	ISNETWORKRETURN(false)

		if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	if (idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return;

	FillThrowData(idx, BattleObjContext::i()->setGameInfo_ThrowWeapon(idx), pThrowWeaponInfo);
}

#ifdef DOMI_STING_POISON_GRENADE
void CGameUDPHandler::WriteGameEvent_DomiDinoThrowWeaponData(INT32 idx, GAMETOCONTEXT_INFO_WEAPON_THROW * pThrowWeaponInfo)
{
	ISNETWORKRETURN(false)

		if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->FindDomiDinoGrenade(idx);
	if (pGrenadeInfo == nullptr)
		return;

	FillThrowData(idx, pGrenadeInfo, pThrowWeaponInfo);
}
#endif


void CGameUDPHandler::WriteGameEvent_ObjectData(INT32 idx, INT32 EventSender, UINT16 nHP, VEC3D * pPos, QUATERNION * pRot)
{
	bool bSend = false;

	if (BattleServerContext::i()->IsP2PHost() == false) return;
	if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	// Event Sender 추가 2011.04.20 swoongoo
	I3_BOUNDCHK(EventSender, SLOT_MAX_COUNT);
	pObjInfo->_tNetData.setEventSender((UINT8)EventSender);

	if (pObjInfo->_tNetData.getHP() != nHP)
	{
		pObjInfo->_tNetData.setHP(nHP);
		bSend = true;
	}

	if ((pObjInfo->_pObjectRef != nullptr) && (pObjInfo->_tNetData.getHP() != 0))
	{	// 마지막 스테이트 상태라면 HP를 0으로 강제합니다. 2008. 11. 28
		if (pObjInfo->_pObjectRef->getCurState() == (pObjInfo->_pObjectRef->getStateCount() - 1))
		{
			pObjInfo->_tNetData.setHP(0);
		}
	}

	switch (pObjInfo->_nType)
	{
	case I3_NETWORK_GAMEOBJECT_TYPE_NONE:		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_STATIC:
		pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
		if (pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME)
			bSend = true;
		break;
	case I3_NETWORK_GAMEOBJECT_TYPE_MOVE:
	{
		if (pPos == nullptr)
		{
			I3TRACE("[GameContext::WriteGameEvent_ObjectData()] move position is nullptr.\n");
			return;
		}
		pObjInfo->_tNetData.setPosition(pPos);

		pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
		if ((pObjInfo->_pObjectRef != nullptr) &&
			(pObjInfo->_pObjectRef->isEnable() && pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_TIME) ||
			(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME))
			bSend = true;
	}
	break;
	case I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC:
	{
		if (pPos == nullptr)
		{
			I3TRACE("[GameContext::WriteGameEvent_ObjectData()] dynamic position is nullptr.\n");
			return;
		}

		if (pRot == nullptr)
		{
			I3TRACE("[GameContext::WriteGameEvent_ObjectData()] dynamic rotate is nullptr.\n");
			return;
		}
		pObjInfo->_tNetData.setPosition(pPos);
		pObjInfo->_tNetData.setRotation(pRot);

		pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
		if ((pObjInfo->_pObjectRef != nullptr) &&
			(pObjInfo->_pObjectRef->isEnable() && pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_TIME) ||
			(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME))
			bSend = true;
	}
	break;
	default:
		I3PRINTLOG(I3LOG_FATAL, "[GameContext::WriteGameEvent_ObjectData()] - invalid Object type.");
		break;
	}

	if (bSend)
	{
		pObjInfo->_bBroadcast = true;
		pObjInfo->_fBroadcastTime = 0.f;
	}
}

void CGameUDPHandler::WriteGameEvent_AnimObjectData(INT32 idx, INT32 EventSender, UINT16 nHP, REAL32 rKeyFrame)
{
	if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	if (pObjInfo->_nType != I3_NETWORK_GAMEOBJECT_TYPE_ANIM)
	{
		I3TRACE("[GameContext::WriteGameEvent_AnimObjectData()] - invalid object type..\n");
		return;
	}

	{
		// EventSender 추가. 2011.04.20 swoongoo
		I3_BOUNDCHK(EventSender, 256);
		pObjInfo->_tNetData.setEventSender((UINT8)EventSender);
		pObjInfo->_tNetData.setObjectType(0);
		pObjInfo->_tNetData.setState(0);
		pObjInfo->_tNetData.setHP(nHP);
		pObjInfo->_tNetData.setKeyFrame(rKeyFrame);

		pObjInfo->_bBroadcast = true;
		pObjInfo->_fBroadcastTime = 0.f;
	}
}


void CGameUDPHandler::WriteGameEvent_DropedWeaponData(INT32 idx, VEC3D * pPos, MATRIX * pMtxRot)
{
	if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(idx);

	pDropedWeaponInfo->_tNetData.setPosition(pPos);

	QUATERNION q;
	i3Matrix::GetRotateQuat(&q, pMtxRot);
	pDropedWeaponInfo->_tNetData.setRotation(&q);

	pDropedWeaponInfo->_bSend = true;
}


void CGameUDPHandler::BindNetGrenadeInfo(INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	REAL32 syncTime;

	I3ASSERT(pWeapon != nullptr);
	if (idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pGrenadeInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(idx);

	//if (i3::same_of<WeaponGrenadeShell*>(pWeapon)
	//	|| i3::same_of<WeaponRocketLauncherShell*>(pWeapon)
	//	|| i3::same_of<WeaponBow*>(pWeapon)
	//	|| i3::same_of<WeaponHandGrenade*>(pWeapon)
	//	|| i3::same_of<WeaponThrowKnife*>(pWeapon))				// 쓰로잉 나이프류
	//{
	//	syncTime = UDPCASTTIME_GRENADESHELL;	// 초당 10회 보냅니다.
	//}
	//else
	//{
	//	syncTime = UDPCASTTIME_GRENADE;			// 그 외 수류탄류는 초당 5회
	//}

	syncTime = UDPCASTTIME_GRENADESHELL;		// 구분 없이 초당 10회 전송
	pGrenadeInfo->Bind(pWeapon, UDPCASTTIME_SLEEPGRENADE, syncTime, pInitPos);
}

void CGameUDPHandler::BindNetC5Info(INT32 idx, WeaponBase * pWeapon, VEC3D * pInitPos)
{
	I3ASSERT(pWeapon != nullptr);
	if (idx < 0 || idx >= MAX_THROWWEAPON_COUNT)	return;

	GAMEINFO_THROW_WEAPON * pC5Info = BattleObjContext::i()->setGameInfo_ThrowWeapon(idx);
	pC5Info->Bind(pWeapon, UDPCASTTIME_SLEEPC5, UDPCASTTIME_C5, pInitPos);
}


void CGameUDPHandler::WriteGameEvent_AnimObjectDataForOutpost(INT32 idx, INT32 EventSender, UINT8 nPathKey, UINT16 nHP, REAL32 rKeyFrame, bool bSend)
{
	if (g_pFramework->IsBattleStage() == false) return;
	if ((gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE))
		return;

	GAMEINFO_OBJECT *	pObjInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	I3ASSERT(pObjInfo->_nType == I3_NETWORK_GAMEOBJECT_TYPE_ANIM);

	pObjInfo->_fBroadcastTime += g_pViewer->getDeltaTime();
	if ((pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJECT_ANIM) ||
		(pObjInfo->_tNetData.getPathKey() != nPathKey))							// 패스 키가 바뀐경우
	{
		if ((bSend == true) ||
			(pObjInfo->_tNetData.getHP() != nHP) ||								// Client도 HP를 받아 게이지를 표시해줘야 합니다.
			(pObjInfo->_fBroadcastTime > UDPCASTTIME_OBJ_SYNC_MINTIME))		// 싱크 타임 5초에 한번 싱크를 맞추도록 합니다.
		{
			// EventSender 추가. 2011.04.20 swoongoo
			pObjInfo->_tNetData.setState(nPathKey);
			pObjInfo->_tNetData.setHP(nHP);
			I3_BOUNDCHK(EventSender, 256);
			pObjInfo->_tNetData.setEventSender((UINT8)EventSender);
			pObjInfo->_tNetData.setKeyFrame(rKeyFrame);
			pObjInfo->_tNetData.addObjectType(UDP_OBJECT_TYPE_STATE | UDP_OBJECT_TYPE_HP | UDP_OBJECT_TYPE_SENDER | UDP_OBJECT_TYPE_ANIMKEY);
			pObjInfo->_bBroadcast = true;
			pObjInfo->_fBroadcastTime = 0.f;
		}
	}
}

void CGameUDPHandler::WriteGameEvent_TouchDown(CGameCharaBase* pChara, UINT16 idx)
{
	if (nullptr == pChara) return;

	INT32 slotIdx = pChara->getCharaNetIndex();

	{
		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);
		if (nullptr == pSendBuffer) return;

		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_TOUCHDOWN;
		pSendBuffer->_ui16TouchDownIdx = idx;
	}
}

/*

*/
void CGameUDPHandler::_CalcPing_DediCation(REAL32 rDeltaTime)
{
	//UDP 패킷을 보낼건지를 체크 합니다. 
	PING* pPingInfo = BattleServerContext::i()->SetPingInfo();
	INT32 iMySlotIdx = BattleSlotContext::i()->getMySlotIdx();

	pPingInfo->m_rPingCheckTime += rDeltaTime;
	if (pPingInfo->m_rPingCheckTime > UDP_PINGSENDTIME)
	{
		//이전 것을 보낸 핑을검사합니다. 
		REAL32 rSendTime = pPingInfo->m_rPingSendTime;
		if (pPingInfo->m_fPingRecvTime < 0.000000001f)rSendTime = 0.0f;
		pPingInfo->RecvPing(iMySlotIdx, rSendTime, pPingInfo->m_fPingRecvTime);

		//Set Value 
		//Send Ping TCP -> GameServer <- 이부분은 PROTOCOL_BATTLE_TIMERSYNC_REQ에서 보내고 있음.
		pPingInfo->m_Pings[iMySlotIdx] = _GetPingValue(pPingInfo->m_rPingList[iMySlotIdx]);

		//Send Ping UDP -> Dedication 
		pPingInfo->m_rPingSendTime = BattleSyncTimer::i()->GetTimeElapsed();
		pPingInfo->m_fPingRecvTime = 0.0000f;
		UDPPacketSender::i()->SendDedication_Ping(pPingInfo->m_rPingSendTime);

		//Set CheckTime
		pPingInfo->m_rPingCheckTime = 0.f;
	}

	return;
}

UINT8 CGameUDPHandler::_GetPingValue(REAL32 rPingTime)
{
	const REAL32 * rPingTable = &g_pConfigEx->GetNetwork().PingLevelCheckValue[0];
	if (rPingTime < rPingTable[0])return 5;
	if (rPingTime < rPingTable[1])return 4;
	if (rPingTime < rPingTable[2])return 3;
	if (rPingTime < rPingTable[3])return 2;

	return 1;
}

void CGameUDPHandler::_CalcPing_CS(REAL32 rDeltatime)
{
	INT32 i;

	PING* pInfo = BattleServerContext::i()->SetPingInfo();

	pInfo->m_rPingCheckTime += rDeltatime;
	if (pInfo->m_rPingCheckTime > UDP_PINGSENDTIME)
	{
		//Set Ping Data
		for (i = 0; i < SLOT_MAX_COUNT; i++)
		{
			pInfo->m_Pings[i] = 1;
			if (BattleSlotContext::i()->getSlotInfo(i)->_State != SLOT_STATE_BATTLE)continue;
			pInfo->m_Pings[i] = _GetPingValue(pInfo->m_rPingList[i]);
		}

		// revision 32106 챌린지 모드 AI 핑값을 MAX(5)로 설정
		// 챌린지 모드 AI 핑 값 셋팅 (Max - 현재 5)
		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
		{
			INT32	AICount = AIModeContext::i()->EnemyCount;
			INT32	AITeam;	// revision 32260 AI 핑값 팀에 따라 처리하도록 수정

							// AI는 나의 반대팀 ( NetIndex : Red팀 - 0, 2, 4, 6...<짝수>, Blue팀 - NetIndex 1, 3, 5, 7 ...<홀수> )
			if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED) AITeam = 1;
			else													AITeam = 0;

			for (i = 0; i < AICount; i++) pInfo->m_Pings[i * 2 + AITeam] = 5;
		}

		// Ping data send to server
		GameEventSender::i()->SetEvent(EVENT_SENDPING_N);

		pInfo->m_rPingCheckTime = 0.f;
	}
}


//-------------------------------------------------------------------------------------------//
//	UDP 패킷으로 받은 데이터를 확인하고 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_P2PMergeRecvData(REAL32 rDeltatime)
{
	INT32	idx;
	INT32	nRecvCount;
	RECV_P2P_DATA	*			pRecvData;
	GAMEINFO_CHARACTER *		pCharaInfo;
	INT32	nMainSlotIdx = BattleSlotContext::i()->getMainSlotIdx();

	if (m_pUDPReceiver == nullptr)
		return;

	nRecvCount = m_pUDPReceiver->GetRecvCount();

	if (!BattleServerContext::i()->IsP2PHost() && nRecvCount)
	{	// Client는 Host로부터로만 받게 되므로 받는 경우가 있으면 체크
		m_fUDPConnectTime[nMainSlotIdx] = 0.f;
	}

	UINT8 currentRound = ScoreContext::i()->getCurrentRound(BattleServerContext::i()->IsUseDedicationMode());

	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(i);
		if (pChara == nullptr)
			continue;

		pChara->SetHelmProtectionFlag(true);
	}

	// Life Packet을 먼저 처리합니다.
	// HelmProtection에 대한 처리 문제를 회피하기 위해 LifePacket을 먼저 설정해야됨.. swoongoo
	if (BattleServerContext::i()->IsP2PHost() == false)
	{
		for (INT32 i = 0; i < nRecvCount; i++)
		{
			pRecvData = m_pUDPReceiver->ReadGameData(i);
			idx = pRecvData->Idx;

			// 2009.06.09 패킷에 라운드값 추가. dfly79
			// 라운드가 다르면 패킷을 무시합니다.
			//if( pRecvData->iRoundNum == currentRound ) // 잠시 주석(제압)
			{
				switch (pRecvData->Type)
				{
				case INFO_DATA_CHARACTER:
				{	//캐릭터 데이터 입니다.
					I3_BOUNDCHK(idx, SLOT_MAX_COUNT);
					CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(idx);
					if (pChara == nullptr)
					{	// 아직 캐릭터를 등록하지 않은 상태에서 받았습니다. 해당 패킷은 무시
						break;
					}

					pCharaInfo = setGameInfo_Chara(idx);

					_ProcessRecvPacket_Chara_HP(idx, pCharaInfo, pRecvData, pChara);
				}
				break;

				case INFO_DATA_AI:
				{
					GAMEINFO_AI_DINOs_NETDATA* pRecvBuf = (GAMEINFO_AI_DINOs_NETDATA*)pRecvData->pArg;
					for (size_t j = 0; j < pRecvBuf->list.size(); ++j)
					{
						GAMEINFO_AI_DINO* buf = pRecvBuf->list[j];

						if (buf->i32Slot >= 0 && buf->i32Slot < MAX_COUNT_DOMI_DINO)
						{
							_ProcessRecvPacket_AI_Respawn(buf);

							CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(buf->i32Slot, true);
							if (chara)
							{
								pCharaInfo = setGameInfo_Chara(buf->i32Slot, CHARA::BUFFER_NEW_AI);
								_ProcessRecvPacket_AI_HP(buf->i32Slot, buf, pCharaInfo, chara);
							}
						}

					}
				}
				break;
				}
			}
		}
	}

	for (INT32 i = 0; i < nRecvCount; i++)
	{
		pRecvData = m_pUDPReceiver->ReadGameData();
		idx = pRecvData->Idx;

//#ifdef DOMI_NEW_FLOW
//		if (gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE) continue;
//#endif

		// 2009.06.09 패킷에 라운드값 추가. dfly79
		// 라운드가 다르면 패킷을 무시합니다.
		bool bRun = (pRecvData->iRoundNum == currentRound);
#ifndef CHANGE_USURPATION
		CGameMissionManager * pMissionMng = CGameMissionManager::i();
		pMissionMng->setUsurpationIdx(pRecvData->iUsurpationIdx);
		VEC3D* pPos = pMissionMng->setMissionObjPos();
		pPos->x = pRecvData->rMissionObjPos[0];
		pPos->y = pRecvData->rMissionObjPos[1];
		pPos->z = pRecvData->rMissionObjPos[2];
#endif

		if (bRun)
		{
			//GlobalFunc::PB_TRACE("pRecvData->Type %d", pRecvData->Type);
			switch (pRecvData->Type)
			{
				// Host or Client
			case INFO_DATA_CHARACTER:
			{	//캐릭터 데이터 입니다.
				I3_BOUNDCHK(idx, SLOT_MAX_COUNT);
				CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(idx);
				if (pChara == nullptr)
				{	// 아직 캐릭터를 등록하지 않은 상태에서 받았습니다. 해당 패킷은 무시
					break;
				}

				pCharaInfo = setGameInfo_Chara(idx);

				if (BattleServerContext::i()->IsP2PHost())
				{ // Host는 각 Client로부터 Character 정보를 받으므로 받았다는 표시를 여기서 해줍니다.
					m_fUDPConnectTime[idx] = 0.f;
					// 패킷 처리
					_ProcessRecvPacket_Chara_ForHost(idx, pCharaInfo, pRecvData, pChara);

					// Recv Ping //방장이 보낸 핑타임과 확인을 해야 한다. 성백
					if (pRecvData->iPing)
					{
						BattleServerContext::i()->SetPingInfo()->RecvPing(idx, pRecvData->rPingSendTime, BattleSyncTimer::i()->GetTimeElapsed());
					}
				}
				else
				{
					_ProcessRecvPacket_Chara_ForClient(idx, pCharaInfo, pRecvData, pChara);
				}
			}
			break;
			case INFO_DATA_AI:
			{
				GAMEINFO_AI_DINOs_NETDATA* pRecvBuf = (GAMEINFO_AI_DINOs_NETDATA*)pRecvData->pArg;
				for (size_t j = 0; j < pRecvBuf->list.size(); ++j)
				{
					GAMEINFO_AI_DINO* buf = pRecvBuf->list[j];

					CGameCharaBase * chara = g_pCharaManager->getCharaByNetIdx(buf->i32Slot, true);
					I3ASSERT(chara != nullptr);										//hp 설정시 케릭터를 생성 하기 때문에 여기서는 반드시 있어야 합니다.

					if (chara == nullptr) continue;									//packet 무시.

					pCharaInfo = setGameInfo_Chara(buf->i32Slot, CHARA::BUFFER_NEW_AI);

					_ProcessRecvPacket_AI(buf->i32Slot, buf, pCharaInfo, chara, rDeltatime);
				}
				pRecvBuf->list.clear();
			}
			break;

			case INFO_DATA_DOMISKILLOBJECT:
			{
				GAMEINFO_DOMIOBJECT_CONT* pRecvBuf = (GAMEINFO_DOMIOBJECT_CONT*)pRecvData->pArg;
				_ProcessRecvPacket_DomiObject(pRecvBuf);
			}
			break;

			case INFO_DATA_DOMIEVENT:
			{
				GAMEINFO_DOMIEVENT_CONT* pRecvBuf = (GAMEINFO_DOMIEVENT_CONT*)pRecvData->pArg;
				_ProcessRecvPacket_DomiEvent(pRecvBuf);
			}
			break;

			case INFO_DATA_SENTRYGUN_DEGREE:
			{
				GAMEINFO_SENTRYGUN_DEGREE* pRecvBuf = (GAMEINFO_SENTRYGUN_DEGREE*)pRecvData->pArg;
				if (pRecvBuf)
				{
					_ProcessRecvPacket_SentryGun_TakingAim(pRecvBuf);
				}
			}
			break;

			// for only client
			case INFO_DATA_GRENADE:	_ProcessRecvPacket_Grenade(idx, pRecvData, rDeltatime);		break;
			case INFO_DATA_OBJECT:	_ProcessRecvPacket_Obj(idx, pRecvData, rDeltatime);			break;
			case INFO_DATA_DROPEDWEAPON:	_ProcessRecvPacket_DropedWeapon(idx, pRecvData, rDeltatime);	break;
			case INFO_DATA_STAGEINFO_CHARA:	_ProcessRecvPacket_StageInfo_Chara(idx, pRecvData);			break;
			case INFO_DATA_STAGEINFO_MISSION:	_ProcessRecvPacket_StageInfo_Mission(pRecvData);				break;
			default: { I3TRACE("Unknown recv data type...\n"); }				break;
			}

			// Recv Ping for Client
			if (!BattleServerContext::i()->IsP2PHost() && pRecvData->iPing
#if defined( DEF_OBSERVER_MODE)
				// Observer는 처리하지 않는다.
				&& !BattleSlotContext::i()->isObserverMe()
#endif
				)
			{
				GAMEINFO_CHARACTER * pCharaInfo2 = setGameInfo_Chara(BattleSlotContext::i()->getMySlotIdx());
				pCharaInfo2->_iPing = pRecvData->iPing;
				pCharaInfo2->_rPingSendTime = pRecvData->rPingSendTime;
			}
		}

		// Receive Buffer를 초기화 하고 리드 포지션을 증가시켜줍니다.
		pRecvData->Reset();
		m_pUDPReceiver->PlusReadPos();
	}

	// 이것이 세팅되야 2번 Effect나오는 것을 막는다.
	for (INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(i);
		if (pChara == nullptr)
			continue;

		pChara->SetHelmProtectionFlag(true);
	}

	// 받은 패킷이 없으면 서버에 알려줘야됩니다.
#if !defined( I3_DEBUG) && !defined( BUILD_RELEASE_QA_VERSION)	// 디버그 모드, QA 빌드 에서 한 컴퓨터에서 2개의 클라를 돌리기 위해 처리하지 않는다.
	_CalcUDPTimeOut(rDeltatime);
#endif

	return;
}

void CGameUDPHandler::_CalcUDPTimeOut(REAL32 rDeltatime)
{
#if !defined( NO_TIMEOUT )

	INT32 i;
	char szTrace[256] = { 0, };
	CGameCharaBase * pChara;

	INT32 nMainSlotIdx = BattleSlotContext::i()->getMainSlotIdx();

	if ((BattleSlotContext::i()->getMySlotInfo()->_State == SLOT_STATE_BATTLE) &&
		(NetworkContext::i()->UdpHoleState[BattleSlotContext::i()->getMySlotIdx()] == UDP_STATE_DONE))
	{//성백 추가.. 게임시작시 네트워크에서 빠져나가는거 방지.
		if (BattleServerContext::i()->IsP2PHost())
		{
			for (i = 0; i < SLOT_MAX_COUNT; i++)
			{
				pChara = g_pCharaManager->getCharaByNetIdx(i);

				if (pChara == nullptr)
					continue;

				if (pChara->isAI() == true)
					continue;

				if ((i != nMainSlotIdx) &&
					(BattleSlotContext::i()->getSlotInfo(i)->_State == SLOT_STATE_BATTLE) &&
					!pChara->isCharaStateMask(CHARA_STATE_DEATH))	// 캐릭터가 살아 있는 경우에만 체크
				{
					if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) ||
						MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION) ||
						MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
					{
						if (!UserContext::i()->IsPlayingSlot(i))
							m_fUDPConnectTime[i] = 0.f;	// 미션 모드 일때 난입한 경우
						if (g_pCharaManager->getCharaTask(i) != CHARA_TASK_RESPAWN)
							m_fUDPConnectTime[i] = 0.f; //미션 모드 일때 게임후 죽은 경우
					}

					m_fUDPConnectTime[i] += rDeltatime;
					if (m_fUDPConnectTime[i] > UDPTIMEOUT_CS)
					{
						m_fUDPConnectTime[i] = 0.f;
						sprintf_s(szTrace, "Client UDP timeout idx %d\n", i);
						I3PRINTLOG(I3LOG_NOTICE, szTrace);
						GameEventSender::i()->SetEvent(EVENT_TIMEOUT_HOST, &i);
					}
				}
			}
		}
		else
		{
			m_fUDPConnectTime[nMainSlotIdx] += rDeltatime;
			if (m_fUDPConnectTime[nMainSlotIdx] > UDPTIMEOUT_CN)
			{//
				m_fUDPConnectTime[nMainSlotIdx] = 0.f;
				sprintf_s(szTrace, "Host UDP timeout idx%d r%d\n", nMainSlotIdx, ScoreContext::i()->getCurrentRound());
				I3PRINTLOG(I3LOG_NOTICE, szTrace);
				GameEventSender::i()->SetEvent(EVENT_TIMEOUT_CLIENT);
				return;
			}
		}
	}
#endif
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_HP(INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *)pRecvData->pArg;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_LIFE)
	{
		if (!pChara->isInvincible())
		{
			UINT16 hp = pRecvBuf->_tHP.getHP();

			if (pChara->getCharaInfo()->IsRex())
			{//TRex는 항상 hp가 100입니다. (100%) Dedi는 0xffff값으로 TRex구분
			 //I3PRINTLOG(I3LOG_DEFALUT, "##TRex'HP	%d\n", hp);
				return;
			}

			pCharaNetData->_tHP.setHP(hp);

			/*
			if( (((INT32)(pChara->getCharaInfo()->GetHP())) == hp) && (hp != 0) )
			pChara->SetHelmProtectionFlag( true);
			else
			pChara->SetHelmProtectionFlag( false);
			*/
			// 위의 조건에 따라 플래그 값을 바꾸는 것에 의미가 없고 버그가 발생하여 주석 처리합니다.
			pChara->SetHelmProtectionFlag(false);

			pChara->setCharaInfo()->SetHP((REAL32)hp);
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_AI_Respawn(GAMEINFO_AI_DINO* buf)
{
	CGameCharaBase* chara = g_pCharaManager->getCharaByNetIdx(buf->i32Slot, true);
	GLOBAL_CHARA_INFO* chara_info = nullptr;

	if (chara != nullptr)
	{
		chara_info = chara->setCharaInfo();
		I3ASSERT_RETURN(chara_info != nullptr);
	}

	bool need_to_respawn = false;
	DINO_KIND kind = (DINO_KIND)buf->i8type;

	if (chara == nullptr || (chara_info != nullptr && chara_info->GetResType() != aiu::DinoKind2CharaResId(kind)))
	{
		T_ItemID ItemID = aiu::DinoKind2ItemID(kind);

		AI_CREATE_DATA data(ItemID, buf->info._tHP.getHP(), buf->info._tPosRotData.getPos(), 0, 1);
		g_pCharaManager->SetAiCharaData(buf->i32Slot, &data);

		chara = g_pCharaManager->RequestFreeAI(buf->i32Slot, ItemID);

		if (chara == nullptr)
			chara = g_pCharaManager->AddAiChara(buf->i32Slot, CHARA::ItemID2ResID(ItemID), true, CHARA::TYPE_DOMI_FULLBODY_AI);

		I3ASSERT(chara != nullptr);
		need_to_respawn = true;
	}
	else
	{
		if (chara != nullptr && chara->isCharaStateMask(CHARA_STATE_DEATH) &&
			buf->info._tCharaState.getFBState() != DINO_STATE_DEAD)
		{
			NET_CHARA_INFO* net_info = BattleSlotContext::i()->setNetCharaInfo(buf->i32Slot, chara->getBufferType());
			I3ASSERT(net_info != nullptr);

			net_info->setRespawnPos(buf->info._tPosRotData.getPos());				//position은 항상 받음.
			chara_info->SetHP(buf->info._tHP.getHP());
			need_to_respawn = true;
		}
	}

	if (need_to_respawn)
	{
		g_pCharaManager->RespawnCharaForAI(buf->i32Slot);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_DOMI_DINO, buf->i32Slot);
	}
}

void CGameUDPHandler::_ProcessRecvPacket_AI_HP(INT32 idx, GAMEINFO_AI_DINO * pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *)&pRecvData->info;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	UINT16 hp = pRecvBuf->_tHP.getHP();

	pCharaNetData->_tHP.setHP(hp);
	pChara->setCharaInfo()->SetHP((REAL32)hp);
}

namespace
{
	// Theta 계산
	inline bool	CalcTheta(REAL32 x, REAL32 z, REAL32& Theta)
	{
		VEC2D	vDir;
		vDir.x = x;
		vDir.y = z;

		REAL32	fDist = i3Vector::Normalize(&vDir, &vDir);
		if (fDist <= 0.0f)
		{
			return false;
		}

		Theta = i3Math::atan2(x, z);

		return true;
	}
}

// AI 관련 처리 코드 필요합니다.
void CGameUDPHandler::_ProcessRecvPacket_AI(INT32 idx, GAMEINFO_AI_DINO* pRecvData, GAMEINFO_CHARACTER * pCharaInfo, CGameCharaBase * pChara, REAL32 rDeltatime)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *)&pRecvData->info;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	pCharaInfo->_bRecvFlag = true;

	{
		bool	bRotatedTheta = false;

		if (pRecvData->i8SubMask & (0x01))
		{
			// 공격시에는 따로 각도를 계산
			REAL32 theta = 0.f;
			bool	rv = CalcTheta(pRecvBuf->_tFireData.getTargetX() - pCharaNetData->_tPosRotData.getPos()->x,
				pRecvBuf->_tFireData.getTargetZ() - pCharaNetData->_tPosRotData.getPos()->z, theta);

			if (rv)
			{
				pCharaNetData->_tPosRotData.setTheta(theta);
			}

			pChara->getInputContext()->ProcessRotationTheta(pRecvBuf);
			bRotatedTheta = true;
		}

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
		{
			if (!bRotatedTheta)
			{
				REAL32 theta = 0.f;
				if (CalcTheta(pRecvBuf->_tPosRotData.getPos()->x - pCharaNetData->_tPosRotData.getPos()->x,
					pRecvBuf->_tPosRotData.getPos()->z - pCharaNetData->_tPosRotData.getPos()->z, theta))
				{
					pRecvBuf->_tPosRotData.setTheta(theta);
				}
				else
					pRecvBuf->_tPosRotData.setTheta(pCharaNetData->_tPosRotData.getTheta());
			}

			pRecvBuf->_tPosRotData.CopyTo(&pCharaNetData->_tPosRotData);
			_ProcessRecvPacket_Chara_PosRotation(pRecvBuf, pChara, bRotatedTheta);
		}


		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
		{
			pRecvBuf->_tCharaState.CopyTo(&pCharaNetData->_tCharaState);

			pCharaNetData->SetInfo_Weapon(pCharaNetData->_tWeaponData.getWeaponID(),
				pCharaNetData->_tWeaponData.getWeaponExtension(), pRecvBuf->_tWeaponData.GetAttackType());

			pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;

			_ProcessRecvPacket_Dino_FullBody(pRecvBuf, pChara);
		}
	}

	if (pRecvData->i8SubMask & (0x01 << 1))
	{
		if (pChara->IsDomiSting() || pChara->IsDomiStingMars())
			static_cast<CGameCharaDomiSting*>(pChara)->Spit(pRecvBuf);
	}

	if (pRecvData->i8SubMask & (0x01 << 2))
	{
#ifdef DOMI_STING_POISON_GRENADE
		if (pChara->IsDomiSting() || pChara->IsDomiStingMars())
			static_cast<CGameCharaDomiSting*>(pChara)->ThrowPoisonGrenade(pRecvBuf);

#endif
	}
}

//#ifdef DOMI_UDP_OBJECT
namespace
{
	//----------------------------------------------
	// 데이터
	//----------------------------------------------

	// enum DOMI_TYPE(commondef.h)으로는 테이블 활용에 제약이 있어
	// DOMI_UDP_OBJECT_TYPE 을 따로 만듬 
	enum SKILL_OBJ_TYPE
	{
		NONE = -1,
		SENTRYGUN,
		DUMMY,
		SKILL_OBJ_TYPE_MAX
	};

	INT8 GetSkillObjTypeByIdx(INT8 idx)
	{
		if (0 <= idx && idx <= 9)	return DOMI_TYPE_SENTRYGUN;
		if (10 <= idx && idx <= 19) return DOMI_TYPE_DUMMY;
		return DOMI_TYPE_NO;
	}

	SKILL_OBJ_TYPE ConvertType(const INT32 type)
	{
		switch (type)
		{
		case DOMI_TYPE_SENTRYGUN:	return SENTRYGUN;
		case DOMI_TYPE_DUMMY:		return DUMMY;
		}

		return NONE;
	}

	INT32 ConvertIdx(const INT32 type, const INT32 idx)
	{
		SKILL_OBJ_TYPE ctype = ConvertType(type);

		if (ctype == -1)
		{
			I3ASSERT(ctype != -1);
			return idx;
		}

		INT32 cidx[2] =
		{
			idx,		// SENTRYGUN	0 ~ 9
			idx - 10,	// DUMMY		10 ~19
		};

		return cidx[ctype];
	}


	//--------------------------------------------------------------------------------------------
	// 스킬 오브젝트 수명주기 행위 : (센트리건, 더미, 서플라이 HP/AMMO/SKILLSHOP) 설치, 업그레이드, 파괴, 상태 설정
	//--------------------------------------------------------------------------------------------
	struct SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info) = 0;
	};

	// 설치 / 업그레이드 / 해제
	struct NothingToDo : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info) {}
	}g_nothing_to_do;

	// 투명 오브젝트 설치 : 센트리건, 더미, 서플라이 HP/AMMO/SKILLSHOP
	struct InstallTransparentSkillObj : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			INT8 type = GetSkillObjTypeByIdx(info->idx);

			UINT32	itemID = 0;
			if (type == DOMI_TYPE_SENTRYGUN)
				itemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_TRANSPARENT));
			else
				itemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_TRANSPARENT));

			const domi::LOCATION_TYPE& loc =
				domi::StageProp::i()->GetLocations(static_cast<DOMI_TYPE>(type))->GetLocation(ConvertIdx(type, info->idx));

			CDominationSkillSystem::i()->Install(itemID, type, info->idx, &loc.position,
				g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx()), loc.rotation);

			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);
			static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::INSTALLABLE);
		}
	}g_install_transparent_skillobj;

	// 센트리건/더미 설치
	struct InstallSolidSkillObj : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			INT8 type = GetSkillObjTypeByIdx(info->idx);

			const domi::LOCATION_TYPE& loc =
				domi::StageProp::i()->GetLocations(static_cast<DOMI_TYPE>(type))->GetLocation(ConvertIdx(type, info->idx));

			// 설치된 오브젝트가 있으면 삭제
			dss::CIdentity* installedObj = CDominationSkillSystem::i()->Find(type, info->idx);
			if (installedObj)
			{
				CDominationSkillSystem::i()->UnInstall(installedObj);
			}

			// 새로 설치
			T_ItemID itemIDTable[5] = { 0, };
			if (type == DOMI_TYPE_DUMMY)
			{
				itemIDTable[0] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_TRANSPARENT));
				itemIDTable[1] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_LV1));
				itemIDTable[2] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_LV2));
				itemIDTable[3] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_LV3));
				itemIDTable[4] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::DUMMY_LV4));
			}
			else if (type == DOMI_TYPE_SENTRYGUN)
			{
				itemIDTable[0] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_TRANSPARENT));
				itemIDTable[1] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_LV1));
				itemIDTable[2] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_LV2));
				itemIDTable[3] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_LV3));
				itemIDTable[4] = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SENTRYGUN_LV4));
			}

			CDominationSkillSystem::i()->Install(itemIDTable[info->lv], type, info->idx, &loc.position,
				g_pCharaManager->getCharaByNetIdx(info->installerIdx), loc.rotation);

			// 상태 설정
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);
			CWeaponInfo* weaponInfo = obj->GetWeapon()->getWeaponInfo();
			if (weaponInfo->GetLevel() != weaponInfo->GetMaxLevel())
				static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::UPGRADABLE);
			else
				static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::NONE);
		}
	}g_install_solid_skillobj;

	// 파괴 : 센트리건, 더미
	struct UninstallSkillObj : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			INT8 type = GetSkillObjTypeByIdx(info->idx);
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);
			if (!obj) return;

			// 파괴
			CDominationSkillSystem::i()->UnInstall(obj);

			// 투명 오브젝트 재설치
			g_install_transparent_skillobj(info);
		}
	}g_uninstall_skillobj;

	// 업그레이드 : 센트리건, 더미
	struct ProcessUpgradeSkillObj : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			// 업그레이드
			INT8 type = GetSkillObjTypeByIdx(info->idx);

			const domi::LOCATION_TYPE& loc =
				domi::StageProp::i()->GetLocations(static_cast<DOMI_TYPE>(type))->GetLocation(ConvertIdx(type, info->idx));

			CDominationSkillSystem::i()->Upgrade(type, info->idx, info->installerIdx, loc.rotation);

			// 상태 설정
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);
			CWeaponInfo* weaponInfo = obj->GetWeapon()->getWeaponInfo();
			if (weaponInfo->GetLevel() != weaponInfo->GetMaxLevel())
				static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::UPGRADABLE);
			else
				static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::NONE);
		}
	}g_upgrade_skillobj;

	enum SKILL_OBJ_LIFESPAN_BAHAVIOR
	{
		SKILL_OBJ_NOTHING_TO_DO,
		SKILL_OBJ_INSTALL_TRANSPARENT,
		SKILL_OBJ_INSTALL_SOLID,
		SKILL_OBJ_UPGRADE,
		SKILL_OBJ_UNINSTALL,
		SKILL_OBJ_LIFESPAN_BEHAVIOR_MAX
	};

	// 센트리건, 더미 행위 판단 : 투명 오브젝트 설치, 1 ~ 4 레벨 설치, 파괴, 업그레이드
	SKILL_OBJ_LIFESPAN_BAHAVIOR WhatSkillObjLifeSpanBehavior(GAMEINFO_SKILL_OBJ* info)
	{
		INT8 type = GetSkillObjTypeByIdx(info->idx);
		dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);

		if (info->lv == 0 && obj == nullptr) return SKILL_OBJ_INSTALL_TRANSPARENT;

		if (info->lv == 0 && obj != nullptr)
			if (::strcmp(static_cast<dss::CServiceable*>(obj)->GetName(), "RAW_OBJECT") != 0)
				return SKILL_OBJ_UNINSTALL;

		if (info->lv > 0 && obj == nullptr) return SKILL_OBJ_INSTALL_SOLID;

		if (info->lv > 0 && obj != nullptr)
		{
			if (::strcmp(static_cast<dss::CServiceable*>(obj)->GetName(), "RAW_OBJECT") == 0)
				return SKILL_OBJ_INSTALL_SOLID;

			if (info->lv > obj->GetWeapon()->getWeaponInfo()->GetLevel())
				return SKILL_OBJ_UPGRADE;
		}

		return SKILL_OBJ_NOTHING_TO_DO;
	}

	SkillObjBehavior* g_skillobj_lifespan_behavior[SKILL_OBJ_LIFESPAN_BEHAVIOR_MAX] =
	{
		&g_nothing_to_do,
		&g_install_transparent_skillobj,
		&g_install_solid_skillobj,
		&g_upgrade_skillobj,
		&g_uninstall_skillobj,
	};



	//----------------------------------------------
	// 센트리건, 더미 HP 설정
	//----------------------------------------------
	struct SetSentrygunHP : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			INT8 type = GetSkillObjTypeByIdx(info->idx);
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, info->idx);
			WeaponBase* wp = obj->GetWeapon();
			if (wp)
			{
				WeaponSentryGun* sentry = static_cast<WeaponSentryGun*>(wp);
				if (sentry->GetHP() != info->hp)
					sentry->SetHP(info->hp);

				if (info->hp < domi::SkillProp::i()->GetSentrygun(info->lv).max_hp->GetVal())
				{
					dss::CServiceable* service = static_cast<dss::CServiceable*>(obj);
					service->SetState(DominationSkill::REPAIRABLE);
				}
				else
				{
					if (sentry->getWeaponInfo()->GetLevel() != sentry->getWeaponInfo()->GetMaxLevel())
						static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::UPGRADABLE);
					else
						static_cast<dss::CServiceable*>(obj)->SetState(DominationSkill::NONE);
				}
			}
		}
	}g_set_sentrygun_hp;

	struct SetDummyHP : SkillObjBehavior
	{
		virtual void operator()(GAMEINFO_SKILL_OBJ* info)
		{
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(DOMI_TYPE_DUMMY, info->idx);
			WeaponBase* wp = obj->GetWeapon();
			if (wp)
			{
				WeaponDinoDummy* dummy = static_cast<WeaponDinoDummy*>(wp);
				if (dummy->GetHP() != info->hp)
					dummy->SetHP(info->hp);

				dss::CServiceable* service = static_cast<dss::CServiceable*>(obj);

				if (info->hp < domi::SkillProp::i()->GetDummy(info->lv).max_hp->GetVal())
					service->SetState(DominationSkill::REPAIRABLE);
				else
				{
					if (dummy->getWeaponInfo()->GetLevel() != dummy->getWeaponInfo()->GetMaxLevel())
						service->SetState(DominationSkill::UPGRADABLE);
					else
						service->SetState(DominationSkill::NONE);
				}
			}
		}
	}g_set_dummy_hp;

	enum SKILL_OBJ_HP_BEHAVIOR
	{
		HP_NONE,
		SET_SENTRY_HP,
		SET_DUMMY_HP,
		SKILL_OBJ_HP_BEHAVIOR_MAX
	};
	SkillObjBehavior* g_skillobj_hp_behavior[SKILL_OBJ_HP_BEHAVIOR_MAX] =
	{
		&g_nothing_to_do,
		&g_set_sentrygun_hp,
		&g_set_dummy_hp
	};
	SKILL_OBJ_HP_BEHAVIOR WhatSkillObjHpBeSet(INT8 idx)
	{
		INT8 type = GetSkillObjTypeByIdx(idx);
		if (type == DOMI_TYPE_SENTRYGUN) return SET_SENTRY_HP;
		if (type == DOMI_TYPE_DUMMY) return SET_DUMMY_HP;
		return HP_NONE;
	}


	//----------------------------------------------
	// 드랍 아이템
	//----------------------------------------------
	struct DropItemBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_DROPITEM* info) = 0;
	};

	struct DropItemNone : DropItemBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_DROPITEM* info) {}
	}g_dropitem_none;

	struct DropItemInstall : DropItemBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_DROPITEM* info)
		{
			// 월드에 떨군다.
			tinst<CGameDropItemManager>()->DropItemFromAI(info->idx, info->type, &info->pos, 0.5f);
		}
	}g_dropitem_install;

	struct DropItemUninstall : DropItemBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_DROPITEM* info)
		{
			// 월드에서 삭제
			CGameDropItem* pItem = CGameDropItemManager::i()->FindDropItemByNetCode(info->idx);
			if (pItem != nullptr)
				pItem->Disappear();
		}
	}g_dropitem_uninstall;


	enum DROPITEM_BEHAVIOR
	{
		DROPITEM_NONE,
		DROPITEM_INSTALL,
		DROPITEM_UNINSTALL,
		DROPITEM_BEHAVIOR_MAX
	};
	DropItemBehavior* g_process_domidropitem[DROPITEM_BEHAVIOR_MAX] =
	{
		&g_dropitem_none,
		&g_dropitem_install,
		&g_dropitem_uninstall,
	};
	DROPITEM_BEHAVIOR WhatDropItemBehaivor(GAMEINFO_DOMI_DROPITEM* info)
	{
		CGameDropItem* pItem = CGameDropItemManager::i()->FindDropItemByNetCode(info->idx);

		if (info->dropped == true && pItem == nullptr) return DROPITEM_INSTALL;
		if (info->dropped == false && pItem != nullptr) return DROPITEM_UNINSTALL;
		return DROPITEM_NONE;
	}

	//----------------------------------------------
	// 이벤트 처리
	//----------------------------------------------
	struct DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event) = 0;
	};

	struct EventNone : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event) {}
	}g_event_none;

	struct SentryDummyRepaired : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event) {}
	}g_event_sentry_dummy_repaired;

	struct SupplyHPUsed : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_SUPPLY_USE* evt = static_cast<GAMEINFO_SUPPLY_USE*>(event);

			CGameCharaBase* customer = g_pCharaManager->getCharaByNetIdx(evt->customerIdx);
			if (customer)
				CGameDropItem::effect(customer, DINOKILLITEM_HP);
		}
	}g_event_used_supply_hp;

	struct SupplyAmmoUsed : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_SUPPLY_USE* evt = static_cast<GAMEINFO_SUPPLY_USE*>(event);

			CGameCharaBase* customer = g_pCharaManager->getCharaByNetIdx(evt->customerIdx);
			if (customer == nullptr) return;

			WeaponBase* primary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
			WeaponBase* secondary = customer->getWeaponContext()->getWeapon(WEAPON_SLOT_SECONDARY);
			if (primary == nullptr && secondary == nullptr) return;

			WeaponBase* wp[2] = { primary, secondary };
			for (INT32 i = 0; i<2; i++)
			{
				if (wp[i] == nullptr) continue; // 무기를 버려서 해당 슬롯에 무기가 없을 경우.
				wp[i]->setLoadedBulletCount(wp[i]->getWeaponInfo()->GetLoadBullet());

				INT32 lv = DomiUserSkillMgr::i()->GetLevel(evt->customerIdx, USER_SKILL_MAGAZINE_UP);
				REAL32 ratio = domi::SkillProp::i()->GetUserSkill(USER_SKILL_MAGAZINE_UP).GetIncrRatio(lv);

				INT32 mazineCapacity = wp[i]->GetLoadBulletWithCashItem(); //WEAPON_SLOT_TYPE 1 개 탄창 최대 탄수
				INT32 maxBullet = static_cast<INT32>(wp[i]->GetMaxBulletWithCashItem() * ratio); // 스킬 적용된 최대 탄수
				wp[i]->setTotalBulletCount(maxBullet - mazineCapacity);
			}

			if (primary != nullptr)
			{
				CGameCharaWeaponContext* weaponCtx = customer->getWeaponContext();
				WEAPON_SLOT_TYPE curWeaponSlot = weaponCtx->getCurrentSlotNum();
				if (WEAPON_SLOT_PRIMARY == curWeaponSlot)
				{
					WEAPON_SLOT_TYPE trickSlot = static_cast<WEAPON_SLOT_TYPE>(curWeaponSlot + 1);
					weaponCtx->setCurrentSlotNum(trickSlot);
					weaponCtx->setSwapSlotNum(trickSlot);
				}
				customer->Cmd_ChangeWeapon(WEAPON_SLOT_PRIMARY, false);
			}

			CGameDropItem::effect(customer, DINOKILLITEM_AMMO);
		}
	}g_event_used_supply_ammo;

	struct SentrygunFire : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_SENTRYGUN_FIRE* evt = static_cast<GAMEINFO_SENTRYGUN_FIRE*>(event);

			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(DOMI_TYPE_SENTRYGUN, static_cast<INT16>(evt->idx));
			if (obj && ::strcmp(obj->GetName(), "RAW_OBJECT") != 0 && obj->GetWeapon())
			{
				WeaponSentryGun* sentrygun = static_cast<WeaponSentryGun*>(obj->GetWeapon());
				sentrygun->ProcessPacket(&evt->target);
			}
		}
	}g_event_sentrygun_fire;

	struct DropItemDropped : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			// 공룡이 아이템을 떨군다.
			GAMEINFO_DROPITEM_DROPPED* evt = static_cast<GAMEINFO_DROPITEM_DROPPED*>(event);

			CGameDropItem* pItem = CGameDropItemManager::i()->FindDropItemByNetCode(evt->idx);
			if (pItem == nullptr)
				tinst<CGameDropItemManager>()->DropItemFromAI(evt->idx, evt->type, &evt->pos);
		}
	}g_event_dropitem_dropped;

	struct DropItemCatched : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			// 유저가 아이템을 획득했다.
			GAMEINFO_DROPITEM_CATCHED* evt = static_cast<GAMEINFO_DROPITEM_CATCHED*>(event);

			CGameDropItem* pItem = CGameDropItemManager::i()->FindDropItemByNetCode(evt->idx);
			if (pItem != nullptr)
			{
				pItem->Disappear();

				CGameCharaBase* p = g_pCharaManager->getCharaByNetIdx(evt->userIdx);
				if (p)
					pItem->process_authentication(p);
			}
		}
	}g_event_dropitem_catched;

	struct SpScoreEvent : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_SP_SCORE* evt = static_cast<GAMEINFO_SP_SCORE*>(event);

			//score sync
			domi_state_info* info = domi_state_info::i();
			info->dino_count = evt->totalKill;

			//debug code.
			//I3TRACE("# domi sync total socre recv : %d #\n", evt->totalScore);
			if (UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_COMBO, dui::COMBO_E_IS_WORKING, -1) == false &&
				UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_IS_WORKING_SCORE, -1) == false &&
				UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SPECIAL_SCORE, dui::SPECIAL_E_IS_WORKING, -1) == false)
			{//combo 중이 아닐 경우에만 sync를 맞춤.
				info->total_score[evt->userIdx / 2] = evt->totalScore;

				UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_USER_STATE, dus::USER_STATE_E_SYNC);
			}


			//sp sync
			UINT16 oldSP = info->sp.sp[evt->userIdx / 2];

			if (UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_IS_WORKING_SP, -1) == false)
			{
				info->sp.sp[evt->userIdx / 2] = evt->totalSP;
			}


			//refresh
			if (evt->userIdx == BattleSlotContext::i()->getMySlotIdx())
			{
				UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);

				static_cast<CUIHudDomi_Skill*>(UIHUDDominationMgr::i()->GetHUD(HUD_DOMI_SKILL))->SetSPGauge(info->sp.sp[evt->userIdx / 2]);
				if (UIBattleFrame::i()->IsOpenPopup(UBP_SKILL_SHOP))
				{
					if (info->sp.sp[evt->userIdx / 2] != oldSP)
						static_cast<UIBattlePopupSkillShop*>(GameUI::GetBattlePopup(UBP_SKILL_SHOP))->RefreshPage();
				}
			}
		}
	}g_event_sp_score;

	struct DinoDeathEvent : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_DINO_DEATH* evt = static_cast<GAMEINFO_DINO_DEATH*>(event);

			domi_state_info* info = domi_state_info::i();
			INT32 arr_pos = BattleSlotContext::i()->getMySlotIdx() / 2;

			if (evt->killerIdx == BattleSlotContext::i()->GetMySlotIdxEx())
			{
				if (evt->combo > 0 && evt->score[arr_pos] > 0)
					UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_KILL_DINO, reinterpret_cast<INT32>(evt));

				if (evt->sp[arr_pos] > 0)
					UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_KILL_SP, reinterpret_cast<INT32>(evt));
			}
			else
			{
				if (evt->score[arr_pos] > 0)
					UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_ASSIST_DINO, reinterpret_cast<INT32>(evt));

				if (evt->sp[arr_pos] > 0)
					UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SCORE_QUEUE, dsq::SCORE_E_ASSIST_SP, reinterpret_cast<INT32>(evt));
			}

			//ST_EVENT_SCORE 패킷을 받기 전 5초 동안 Kill이 발생 했을 경우 UI상으로 보여주기 위해서
			info->dino_count++;
			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_REFRESH, dui::flow_state);
		}
	}g_event_dino_death;

	struct MutantTrexDashEvent : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_MUTANTTREX_DASH* evt = static_cast<GAMEINFO_MUTANTTREX_DASH*>(event);
			domi_state_info::i()->boss_dash_target_idx = (evt->dash_info & 0x0F);

			domi_state_info::i()->boos_dash_target_type = DOMI_TYPE_USER;

			if (evt->dash_info & MASK_TREX_DASH_SENTRY)
				domi_state_info::i()->boos_dash_target_type = DOMI_TYPE_SENTRYGUN;

			if (evt->dash_info & MASK_TREX_DASH_DUMMY)
				domi_state_info::i()->boos_dash_target_type = DOMI_TYPE_DUMMY;

			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_FLOW, dui::FLOW_E_DETAIL, dui::FLOW_D_BOSS_DASH);
		}
	}g_event_mutanttrex_dash;

	struct MutantTrexHowlEvent : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_MUTANTTREX_HOWL* evt = static_cast<GAMEINFO_MUTANTTREX_HOWL*>(event);
			//evt->mutantIdx; // 지금 필요없음.
			domi_state_info::i()->boss_howl_cancel_threshold = evt->threshold;
		}
	}g_event_mutanttrex_howl;

	struct SpecialEffectEvent : DomiEventBehavior
	{
		virtual void operator()(GAMEINFO_DOMI_EVENT* event)
		{
			GAMEINFO_SPECIAL_EFFECT* evt = static_cast<GAMEINFO_SPECIAL_EFFECT*>(event);

			INT32 i32Arg;
			PACKET_DOMINATION_DIVULGE_SPECIAL_EFFECT_ACK* ps = &evt->data;
			i3mem::Copy(&i32Arg, &ps, sizeof(INT32));

			UIHUDDominationMgr::i()->SetHudEvent(HUD_DOMI_SPECIAL_SCORE, dui::SPECIAL_E_START, i32Arg);
		}
	}g_event_special_effect;

	enum DOMI_EVENT_TYPE
	{
		DOMI_EVENT_NONE,
		DOMI_EVENT_SENTRY_DUMMY_REPAIR,
		DOMI_EVENT_SUPPLY_HP,
		DOMI_EVENT_SUPPLY_AMMO,
		DOMI_EVENT_SENTRYGUN_FIRE,
		DOMI_EVENT_DROPITEM_DROPPED,
		DOMI_EVENT_DROPITEM_CATCHED,
		DOMI_EVENT_SP_SCORE,
		DOMI_EVENT_DINO_DEATH,
		DOMI_EVENT_MUTANTTREX_DASH,
		DOMI_EVENT_MUTANTTREX_HOWL,
		DOMI_EVENT_SPECIAL_EFFECT,
		DOMI_EVENT_MAX
	};
	DomiEventBehavior* g_process_event[DOMI_EVENT_MAX] =
	{
		&g_event_none,
		&g_event_sentry_dummy_repaired,
		&g_event_used_supply_hp,
		&g_event_used_supply_ammo,
		&g_event_sentrygun_fire,
		&g_event_dropitem_dropped,
		&g_event_dropitem_catched,
		&g_event_sp_score,
		&g_event_dino_death,
		&g_event_mutanttrex_dash,
		&g_event_mutanttrex_howl,
		&g_event_special_effect,
	};

	DOMI_TYPE GetSupplyType(INT8 idx)
	{
		if (20 <= idx && idx <= 24) return DOMI_TYPE_HPSUPPLY;
		if (25 <= idx && idx <= 29) return DOMI_TYPE_AMMOSUPPLY;
		if (30 <= idx && idx <= 35) return DOMI_TYPE_SKILLSHOP;
		return DOMI_TYPE_NO;
	}

	DOMI_EVENT_TYPE EventWhat(GAMEINFO_DOMI_EVENT* evt)
	{
		if (evt->evtType == 0) return DOMI_EVENT_SENTRY_DUMMY_REPAIR;
		if (evt->evtType == 1)
		{
			GAMEINFO_SUPPLY_USE* supplyEvt = static_cast<GAMEINFO_SUPPLY_USE*>(evt);

			INT8 type = static_cast<INT8>(GetSupplyType(supplyEvt->idx));
			dss::CIdentity* obj = CDominationSkillSystem::i()->Find(type, supplyEvt->idx);
			if (obj == nullptr) return DOMI_EVENT_NONE;

			if (type == DOMI_TYPE_AMMOSUPPLY) return DOMI_EVENT_SUPPLY_AMMO;
			if (type == DOMI_TYPE_HPSUPPLY) return DOMI_EVENT_SUPPLY_HP;
		}
		if (evt->evtType == 2) return DOMI_EVENT_SENTRYGUN_FIRE;
		if (evt->evtType == 3) return DOMI_EVENT_DROPITEM_DROPPED;
		if (evt->evtType == 4) return DOMI_EVENT_DROPITEM_CATCHED;
		if (evt->evtType == 5) return DOMI_EVENT_SP_SCORE;
		if (evt->evtType == 6) return DOMI_EVENT_DINO_DEATH;
		if (evt->evtType == 7) return DOMI_EVENT_MUTANTTREX_DASH;
		if (evt->evtType == 8) return DOMI_EVENT_MUTANTTREX_HOWL;
		if (evt->evtType == 9) return DOMI_EVENT_SPECIAL_EFFECT;

		return DOMI_EVENT_NONE;
	}
}

void CGameUDPHandler::_ProcessRecvPacket_DomiObject(GAMEINFO_DOMIOBJECT_CONT* pRecvData)
{
	GAMEINFO_SKILL_OBJ* skillObjInfo
		= const_cast<GAMEINFO_SKILL_OBJ*>(pRecvData->getSkillObjectInfo());
	if (skillObjInfo)
	{
		(*g_skillobj_lifespan_behavior[WhatSkillObjLifeSpanBehavior(skillObjInfo)])(skillObjInfo);

		(*g_skillobj_hp_behavior[WhatSkillObjHpBeSet(skillObjInfo->idx)])(skillObjInfo);
	}

	GAMEINFO_DOMI_DROPITEM* dropItemInfo
		= const_cast<GAMEINFO_DOMI_DROPITEM*>(pRecvData->getDropItemInfo());
	if (dropItemInfo)
	{
		(*g_process_domidropitem[WhatDropItemBehaivor(dropItemInfo)])(dropItemInfo);
	}
}

void CGameUDPHandler::_ProcessRecvPacket_DomiEvent(GAMEINFO_DOMIEVENT_CONT* pRecvData)
{
	i3::vector<GAMEINFO_DOMI_EVENT*>& events = pRecvData->events;
	for (size_t i = 0; i<events.size(); i++)
	{
		DOMI_EVENT_TYPE type = EventWhat(events[i]);
		(*g_process_event[type])(events[i]);
	}
}

void CGameUDPHandler::_ProcessRecvPacket_SentryGun_TakingAim(GAMEINFO_SENTRYGUN_DEGREE* pRecvData)
{
	DominationSkill::CIdentity* obj = CDominationSkillSystem::i()->Find(DOMI_TYPE_SENTRYGUN, pRecvData->idx);
	if (obj)
	{
		WeaponSentryGun* sg = static_cast<WeaponSentryGun*>(obj->GetWeapon());
		if (sg)
		{
			FireArmAngle* faa = sg->GetFireArmAngle();
			faa->ReceiveTheta(pRecvData->theta);
			FireArmAngle::LERP_TIME = 0.3f;
			//I3TRACE("SG THETA : %f\n", pRecvData->theta);
		}
	}
}
//#endif

//파싱 순서 중요합니다.
void CGameUDPHandler::_ProcessRecvPacket_Chara_ForClient(INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *)pRecvData->pArg;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	pCharaInfo->_bRecvFlag = true;

	if (pChara->isCharaStateMask(CHARA_STATE_DEATH) == false)
	{	// 이미 죽은 상태라면 처리 하지 않는다.

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
			_ProcessRecvPacket_Chara_GetWeaponForClient(pRecvBuf, pChara);

		// 기본 데이터
		if (pChara->isNetwork())
		{
			if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
			{
				pRecvBuf->_tTakingObject.CopyTo(&pCharaNetData->_tTakingObject);
				//_ProcessRecvPacket_Chara_OnLoadObject( pCharaNetData, pRecvBuf, pChara);
			}

			if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
			{
				pRecvBuf->_tPosRotData.CopyTo(&pCharaNetData->_tPosRotData);
				_ProcessRecvPacket_Chara_PosRotation(pRecvBuf, pChara);
			}
		}

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_MOVE_POS)
		{
			pRecvBuf->_tPosRotData.CopyTo(&pCharaNetData->_tPosRotData);
			_ProcessRecvPacket_Chara_PosRotation(pRecvBuf, pChara);
		}


		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON)
		{
			pCharaNetData->_tWeaponData.setWeaponID(pRecvBuf->_tWeaponData.getWeaponID());
			_ProcessRecvPacket_Chara_WeaponSync(&pRecvBuf->_tWeaponData, &pRecvBuf->_tCharaState, pChara);
		}

		if (pChara->isNetwork())
		{
			if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
			{
				pRecvBuf->_tCharaState.CopyTo(&pCharaNetData->_tCharaState);
				pCharaNetData->SetInfo_Weapon(pCharaNetData->_tWeaponData.getWeaponID(),
					pCharaNetData->_tWeaponData.getWeaponExtension(), pRecvBuf->_tWeaponData.GetAttackType());

				pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;

				_ProcessRecvPacket_Chara_ActionState(pRecvBuf, pChara);
			}

			if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
			{
				_ProcessRecvPacket_Chara_Suicide(pCharaNetData, pRecvBuf, pChara);
			}

			if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_MISSION)
				_ProcessRecvPacket_Chara_MissionDataForClient(pRecvBuf, pChara);
		}

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_RADIO)
		{
			pRecvBuf->_tRadioChat.CopyTo(&pCharaNetData->_tRadioChat);
			_ProcessRecvPacket_Chara_RadioChat(pRecvBuf, pChara);
		}

		if ((pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE))
		{
			pRecvBuf->_tWeaponData.setWeaponID(pRecvBuf->_tFireData.getWeaponID());
			pRecvBuf->_tWeaponData.setWeaponExtension(pRecvBuf->_tFireData.getWeaponExtension());
			pRecvBuf->_tWeaponData.SetAttackType(pRecvBuf->_tFireData.getWeaponAttackType());

			pRecvBuf->_tWeaponData.CopyTo(&pCharaNetData->_tWeaponData);

			_ProcessRecvPacket_Chara_WeaponSync(&pRecvBuf->_tWeaponData, &pRecvBuf->_tCharaState, pChara);

			// Process packet
			if (pChara->isLocal())		_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient(pCharaNetData, pRecvBuf);		// 자신이 던진 수류탄만 처리
			else
			{
				if (pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireDataOnObject(pCharaNetData, pRecvBuf, pChara->GetOnObject());
				else						_ProcessRecvPacket_Chara_FireData(pCharaNetData, pRecvBuf, pChara);
			}
		}

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HIT ||
			pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HITBYCHARA ||
			pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HITBYEXPLOSION)
		{
			if (pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireNHitDataOnObject(pRecvBuf, pChara->GetOnObject());
			else						_ProcessRecvPacket_Chara_FireNHitData(pRecvBuf, pChara);
		}

	}

	if ((pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DROPWEAPON))
		_ProcessRecvPacket_Chara_DropWeaponForClient(pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ACTIONKEY)
	{
		_ProcessRecvPacket_Chara_ActionKey(pCharaNetData, pRecvBuf, pChara);

		// 여기서 내 ActionKey를 덮어서 문제가 된다... Count가 있으면 그 뒤로 붙이도록 합니다.
		// Count 대로 붙이면 안됩니다. 또 보내니까..
		if (pCharaNetData->_tActionKey.getActionCount() == 0)
			pRecvBuf->_tActionKey.CopyTo(&pCharaNetData->_tActionKey);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
		_ProcessRecvPacket_Chara_DirectPickUp(pCharaNetData, pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_DEATH)
		_ProcessRecvPacket_Chara_DeathDataForClient(pRecvBuf, pChara);

	//제압 모드 일 경우 서버에서 보내 주는 Death 패킷
	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DOMI_DEATH)
		_ProcessRecvPacket_Chara_DeathDataForClient(pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CS_SUFFERING)
		_ProcessRecvPacket_Chara_SufferDataForClient(pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCOBJECTSTATE)
		_ProcessRecvPacket_Chara_ActionForObjectSync(pRecvBuf, pChara);

}

//파싱 순서 중요합니다.
void CGameUDPHandler::_ProcessRecvPacket_Chara_ForHost(INT32 idx, GAMEINFO_CHARACTER * pCharaInfo, RECV_P2P_DATA * pRecvData, CGameCharaBase * pChara)
{
	GAMEINFO_CHARA_NETDATA * pRecvBuf = (GAMEINFO_CHARA_NETDATA *)pRecvData->pArg;
	GAMEINFO_CHARA_NETDATA * pCharaNetData = &pCharaInfo->_tNetData;

	pCharaInfo->_bRecvFlag = true;
	pCharaInfo->_bSendFlag = true;			// broadcast character info

	if (pChara->isCharaStateMask(CHARA_STATE_DEATH))
	{	// 이미 죽은 상태라면 처리 하지 않는다.
		return;
	}

	// process packet
	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ONLOADOBJECT)
	{
		pRecvBuf->_tTakingObject.CopyTo(&pCharaNetData->_tTakingObject);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_ONLOADOBJECT;

		//_ProcessRecvPacket_Chara_OnLoadObject( pCharaNetData, pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARAPOS)
	{
		pRecvBuf->_tPosRotData.CopyTo(&pCharaNetData->_tPosRotData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_CHARAPOS;

		_ProcessRecvPacket_Chara_PosRotation(pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_CHARASTATE)
	{
		pRecvBuf->_tCharaState.CopyTo(&pCharaNetData->_tCharaState);
		pCharaNetData->SetInfo_Weapon(pCharaNetData->_tWeaponData.getWeaponID(),
			pCharaNetData->_tWeaponData.getWeaponExtension(), pRecvBuf->_tWeaponData.GetAttackType());
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;

		_ProcessRecvPacket_Chara_ActionState(pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_RADIO)
	{
		pRecvBuf->_tRadioChat.CopyTo(&pCharaNetData->_tRadioChat);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_RADIO;

		_ProcessRecvPacket_Chara_RadioChat(pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SYNCWEAPON)
	{
		pRecvBuf->_tWeaponData.CopyTo(&pCharaNetData->_tWeaponData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;

		_ProcessRecvPacket_Chara_WeaponSync(&pRecvBuf->_tWeaponData, &pRecvBuf->_tCharaState, pChara);

		//GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_ForHost P2P_INFO_FLAG_SYNCWEAPON");
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
	{
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_SUICIDEDAMAGE;

		_ProcessRecvPacket_Chara_Suicide(pCharaNetData, pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_MISSION)
	{
		pRecvBuf->_tMissionData.CopyTo(&pCharaNetData->_tMissionData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_MISSION;

		_ProcessRecvPacket_Chara_MissionDataForHost(pRecvBuf, pChara);
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_GETWEAPON)
		_ProcessRecvPacket_Chara_GetWeaponForHost(pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DIRECTPICKUP)
		_ProcessRecvPacket_Chara_DirectPickUp( pCharaNetData, pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE)
	{
		if (pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireDataOnObject(pCharaNetData, pRecvBuf, pChara->GetOnObject());
		else						_ProcessRecvPacket_Chara_FireData(pCharaNetData, pRecvBuf, pChara);

		_ProcessRecvPacket_Chara_WeaponSync(&pRecvBuf->_tWeaponData, &pRecvBuf->_tCharaState, pChara);

		pRecvBuf->_tWeaponData.CopyTo(&pCharaNetData->_tWeaponData);
		pRecvBuf->_tFireData.CopyToFire(&pCharaNetData->_tFireData);
		pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_FIRE;
	}

	if ((pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HIT) ||
		(pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HITBYCHARA))
	{
		if (pChara->IsOnObject())	_ProcessRecvPacket_Chara_FireNHitDataOnObject(pRecvBuf, pChara->GetOnObject());
		else						_ProcessRecvPacket_Chara_FireNHitData(pRecvBuf, pChara);

		pRecvBuf->_tFireData._tHitData.CopyTo(&pCharaNetData->_tFireData._tHitData);

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_HIT)
			pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_HIT;
		else
			pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_HITBYCHARA;

		//GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_ForHost P2P_INFO_FLAG_HIT");
	}

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DROPWEAPON)
		_ProcessRecvPacket_Chara_DropWeaponForHost(pRecvBuf, pChara);

	if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_ACTIONKEY)
	{
		_ProcessRecvPacket_Chara_ActionKey(pCharaNetData, pRecvBuf, pChara);

		if (pCharaNetData->_tActionKey.getActionCount() == 0)
		{
			pRecvBuf->_tActionKey.CopyTo(&pCharaNetData->_tActionKey);
			pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Grenade(INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	if (idx < 0 || idx >= MAX_THROWWEAPON_COUNT)
	{
		I3TRACE("[_ProcessRecvPacket_Grenade] invalid index.%d\n", idx);
		return;
	}

	GAMEINFO_THROW_WEAPON * pInfo = BattleObjContext::i()->setGameInfo_ThrowWeapon(idx);
	GAMEINFO_THROW_WEAPON_NETDATA * pRecvBuf = reinterpret_cast<GAMEINFO_THROW_WEAPON_NETDATA*>(pRecvData->pArg);

	if (BattleServerContext::i()->IsUseDedicationMode())
	{
		// 자신이 보낸 패킷이 온 경우는 무시
		INT32 maxLimit = BattleSlotContext::i()->getMySlotIdx() * MAX_THROWWEAPON_SLOP;
		INT32 lowLimit = idx - maxLimit;

		if (lowLimit >= 0 && lowLimit < MAX_THROWWEAPON_SLOP)
			return;
	}

	pInfo->_tNetData.setRemainingTime(pRecvBuf->getRemainingTime());

	// 생성된 투척 무기가 없으면, 새 투척무기를 Bind 해줍니다.
	if (pInfo->getWeaponBase() == nullptr)
		BindNetThrowData(idx, pRecvBuf);

	// 투척 무기의 네트워크 정보를 설정합니다.
	SetNetThrowData(pInfo, pRecvBuf);
}

void CGameUDPHandler::_ProcessRecvPacket_Obj(INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	if (idx < 0 || idx >= OBJ_TOTAL_COUNT)
	{
		I3TRACE("[_ProcessRecvPacket_Obj] invalid index.%d\n", idx);
		return;
	}

	GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->setGameInfo_Object(idx);
	I3ASSERT(pObjInfo != nullptr);

	if (pObjInfo->_pObjectRef == nullptr)
	{
		I3PRINTLOG(I3LOG_FATAL, "No objects found.");
		return;
	}

	GAMEINFO_OBJECT_NETDATA * pRecvBuf = reinterpret_cast<GAMEINFO_OBJECT_NETDATA*>(pRecvData->pArg);
	I3ASSERT(pRecvBuf != nullptr);

	pRecvBuf->CopyTo(&pObjInfo->_tNetData);

	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	if (pRecvBuf->getObjectType() & UDP_OBJECT_TYPE_OWN)
	{
		if (pRecvBuf->getObjectType() & UDP_OBJECT_TYPE_CONVOY)
			pMissionMng->setUsurpationIdx(pRecvBuf->getEnterCharaIndex());

		if (pRecvBuf->getEnterCharaIndex() == 255)
		{
			pObjInfo->_tNetData.setShellCount(1);
			pObjInfo->_tNetData.setState(0);
		}
		else
		{
			pObjInfo->_tNetData.setShellCount(0);
			pObjInfo->_tNetData.setState(1);
		}
	}

	if (0xFF == pRecvBuf->getEnterCharaIndex())
	{
		if (pRecvBuf->getObjectType() & UDP_OBJECT_TYPE_POSITION)
		{
			i3Vector::Copy(&pObjInfo->_tAssistData._vPostPos, ((i3Object *)pObjInfo->_pObjectRef)->GetPos());
			pObjInfo->_tAssistData._fAssistTime = 0.f;
		}
	}

	if (pRecvBuf->getObjectType() & UDP_OBJECT_TYPE_ROTATION)
	{
		i3Matrix::GetRotateQuat(&pObjInfo->_tAssistData._qPostRot, (((i3Object*)pObjInfo->_pObjectRef)->GetMatrix()));
		pObjInfo->_tAssistData._fAssistTime = 0.f;
	}

}


void CGameUDPHandler::_ProcessRecvPacket_DropedWeapon(INT32 idx, RECV_P2P_DATA * pRecvData, REAL32 rDeltatime)
{
	if (idx < 0 || idx >= MAX_DROPEDWEAPON_COUNT)
	{
		I3TRACE("[_ProcessRecvPacket_DropedWeapon] invalid index.%d\n", idx);
		return;
	}

	GAMEINFO_DROPED_WEAPON * pObjInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(idx);
	I3ASSERT(pObjInfo != nullptr);

	GAMEINFO_DROPEDWEAPON_NETDATA * pRecvBuf = reinterpret_cast<GAMEINFO_DROPEDWEAPON_NETDATA*>(pRecvData->pArg);
	I3ASSERT(pRecvBuf != nullptr);

	if (pObjInfo->_pWeaponRef == nullptr)
	{// 무기가 없습니다.
		return;
	}

	if (BattleServerContext::i()->IsUseDedicationMode())
	{
		if (pObjInfo->_pWeaponRef->getOwner() != nullptr)
		{	// 자신이 버린 무기는 Network처리를 하지 않는다.
			if (pObjInfo->_pWeaponRef->getOwner()->isLocal())
				return;
		}
	}

	pRecvBuf->CopyTo(&pObjInfo->_tNetData);

	i3Vector::Copy(&pObjInfo->_tAssistData._vPostPos, pObjInfo->_pWeaponRef->GetPos());
	i3Matrix::GetRotateQuat(&pObjInfo->_tAssistData._qPostRot, pObjInfo->_pWeaponRef->GetMatrix());

	if (pRecvBuf->getState() == NET_WEAPONDROPED_FLY)
	{
		VEC3D vDir;
		i3Vector::Sub(&pObjInfo->_tAssistData._vDir, pObjInfo->_tNetData.getPosition(), &pObjInfo->_tAssistData._vPostPos);
		i3Vector::Scale(&vDir, &pObjInfo->_tAssistData._vDir, 0.03f);
		REAL32 rLen = i3Vector::Length(&vDir);
		i3Vector::Add(&vDir, 0.f, -rLen, 0.f);
		i3Vector::Add(&pObjInfo->_tAssistData._vPostPos, &pObjInfo->_tAssistData._vPostPos, &vDir);
	}

	pObjInfo->_tAssistData._fAssistTime = 0.f;
	pObjInfo->_bRecv = true;
}


bool CGameUDPHandler::_ProcessRecvPacket_StageInfo_Chara(INT32 idx, RECV_P2P_DATA * pRecvData)
{
	return true;
}



void CGameUDPHandler::_ProcessRecvPacket_StageInfo_Obj_Anim(INT32 idx, RECV_P2P_DATA * pRecvData)
{
	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = reinterpret_cast<GAMEINFO_OBJECT_NETDATA*>(pRecvData->pArg);

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	i3Object * pObj = pObjectInfo->_pObjectRef;

	if (pObj == nullptr)		return;

	// revision 63360
	//pRecvObjectInfo->CopyAnimationObj(&pObjectInfo->_tNetData);

	//pObjectInfo->_bRecv = true;
	pObjectInfo->_tNetData.setState(pRecvObjectInfo->getState());
	pObjectInfo->_tNetData.setHP(pRecvObjectInfo->getHP());
	pObjectInfo->_tNetData.setKeyFrame(pRecvObjectInfo->getKeyFrame());

	// 사운드 및 이팩트 등을 처리하지 않도록 세팅합니다. //
	// 현재 sound 및 effect 이벤트가 처리 되기 때문에 함수 명 변경. //
	GlobalFunc::ChangeObjState_(pObj, pRecvObjectInfo->getState(), pRecvObjectInfo->getKeyFrame());

	// 특수 오브젝트입니다.
	if (i3::kind_of<CGameControledObject*>(pObjectInfo->_pObjectRef))
	{
		// 난입에 대한 특수 오브젝트 처리
		((CGameControledObject*)pObjectInfo->_pObjectRef)->ProcessIntrude(pRecvObjectInfo);

		// 무기 오브젝트 처리
		if (i3::same_of<CGameObjectRidingHelicopter*, CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
		{
			CGameObjectWithWeapon * pWeaponObject = (CGameObjectWithWeapon*)pObjectInfo->_pObjectRef;

			// revision 62269 오브젝트 폭파 컬리전 체크 수정 및 파괴미션 헬기 버그 수정
			if (!pWeaponObject->GetHalfDestroyed() && pRecvObjectInfo->getState() == 1)
			{
				//I3PRINTLOG(I3LOG_NOTICE,"Object Leaved!!!!!-------------");
				pWeaponObject->LeaveObject();
				pWeaponObject->SetHalfDestroyed(true);
			}

			//I3TRACE( "StageInfo Recv:: %d, %.4f, %d, %d\n", pRecvObjectInfo->_nState, pRecvObjectInfo->_fKey, pRecvObjectInfo->_nPathKey, pRecvObjectInfo->_EnterCharaIdx);
			if (pRecvObjectInfo->getObjectType() & UDP_OBJECT_TYPE_OWN)
			{
				if (pRecvObjectInfo->getEnterCharaIndex() < SLOT_MAX_COUNT)
				{
					CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(pRecvObjectInfo->getEnterCharaIndex());
					if (pChara != nullptr)
						pWeaponObject->EnterObject(pChara);
				}
			}

			// 탑승 헬기에 대한 애니메이션 패스를 세팅합니다.
			pObjectInfo->_tNetData.setPathKey(pRecvObjectInfo->getPathKey());
		}
	}

	pObjectInfo->_bIntrudeCheck = true;

}

void CGameUDPHandler::_ProcessRecvPacket_StageInfo_Obj_Static(INT32 idx, RECV_P2P_DATA * pRecvData)
{
	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = reinterpret_cast<GAMEINFO_OBJECT_NETDATA*>(pRecvData->pArg);

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	if (pObjectInfo->_pObjectRef == nullptr)
		return;
	if (CGameMissionManager::i()->isMissionObject(pObjectInfo->_pObjectRef))
		return;

	// Netstate가 아닌 Object의 현재 State 번호입니다.
	if ((INT32)pRecvObjectInfo->getState() >= pObjectInfo->_pObjectRef->getStateCount())
		return;

	if (pRecvObjectInfo->getState() > pObjectInfo->_pObjectRef->getCurState())
	{
		I3_OBJECT_STATE * pState = pObjectInfo->_pObjectRef->getState(pRecvObjectInfo->getState());
		REAL32 rDamage = (1.f - pState->m_HPRatio) * pObjectInfo->_pObjectRef->getMaxHP();

		GlobalFunc::ChangeObjState_(pObjectInfo->_pObjectRef, pRecvObjectInfo->getState(), 0.f);

		pObjectInfo->_pObjectRef->setCurrentHP(pObjectInfo->_pObjectRef->getMaxHP() - rDamage);

		pObjectInfo->_tNetData.setHP((UINT16)pObjectInfo->_pObjectRef->getCurrentHP());
	}
}

void CGameUDPHandler::_ProcessRecvPacket_StageInfo_Obj_Move(INT32 idx, RECV_P2P_DATA * pRecvData)
{
}

bool CGameUDPHandler::_ProcessRecvPacket_StageInfo_Obj_Dynamic(INT32 idx, RECV_P2P_DATA * pRecvData)
{
	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = reinterpret_cast<GAMEINFO_OBJECT_NETDATA*>(pRecvData->pArg);

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	if (pObjectInfo->_pObjectRef == nullptr)	return false;
	if (CGameMissionManager::i()->isMissionObject(pObjectInfo->_pObjectRef)) return false;

	// Netstate가 아닌 Object의 현재 State 번호입니다.
	if ((INT32)pRecvObjectInfo->getState() >= pObjectInfo->_pObjectRef->getStateCount())	return false;

	if (pObjectInfo->_bIntrudeCheck == true) return true;

	if (pRecvObjectInfo->getState() > pObjectInfo->_pObjectRef->getCurState())
	{
		I3_OBJECT_STATE * pState = pObjectInfo->_pObjectRef->getState(pRecvObjectInfo->getState());
		REAL32 rDamage = (1.f - pState->m_HPRatio) * pObjectInfo->_pObjectRef->getMaxHP();

		GlobalFunc::ChangeObjState_(pObjectInfo->_pObjectRef, pRecvObjectInfo->getState(), 0.f);

		pObjectInfo->_pObjectRef->setCurrentHP(pObjectInfo->_pObjectRef->getMaxHP() - rDamage);

		pObjectInfo->_tNetData.setHP((UINT16)pObjectInfo->_pObjectRef->getCurrentHP());
	}

	i3Matrix::SetRotateQuat(pObjectInfo->_pObjectRef->getSceneObject()->GetMatrix(), pRecvObjectInfo->getRotation());

	VEC3D vecTemp;
	UDP_UTIL::ConvertNet3DtoVec3D(&vecTemp, pRecvObjectInfo->getPosition());
	i3Matrix::PostTranslate(pObjectInfo->_pObjectRef->getSceneObject()->GetMatrix(), &vecTemp);

	pObjectInfo->_bIntrudeCheck = true;

	//I3TRACE( "StageInfo Recv DynamicObject %d\n", nNetIdx);

	return true;
}

bool CGameUDPHandler::_ProcessRecvPacket_StageInfo_Mission(RECV_P2P_DATA * pRecvData)
{
	if (UserContext::i()->IsPlayingSlot(BattleSlotContext::i()->getMySlotIdx()))
		return true;
	// Mission Object
	// 설치되어 있으면 생성해서 월드에 붙여줍니다.
	GAMEINFO_MISSION_INTRUDER * pInfo = (GAMEINFO_MISSION_INTRUDER*)pRecvData->pArg;
	if (pInfo->getInstallMode() != 1)
		return false;

	if ((pInfo->getInstallerIndex() >= SLOT_MAX_COUNT) || (pInfo->getInstallerIndex() < 0))
		return false;

	WeaponBase * pC4 = nullptr;
#if defined( USE_EVENT_SHUTTLEX)
	pC4 = g_pWeaponManager->FindDropWeapon(nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::SHUTTLEX));
#else
	pC4 = g_pWeaponManager->FindDropWeapon(nullptr, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
#endif

	if (pC4 == nullptr)
	{
		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

#if defined( DEF_OBSERVER_MODE)
		if (pPlayer == nullptr)
		{
			// Observer에서는 Play중인 Character에서 처리
			INT32 i;
			for (i = 0; i < g_pCharaManager->getCharaCount(); i++)
			{
				if (g_pCharaManager->getChara(i) != nullptr)
				{
					pPlayer = g_pCharaManager->getChara(i);
					break;
				}
			}

		}
#endif

		if (pPlayer != nullptr)
		{
			// revision 54648 2013. 3. 6 박기성
			// 실제 폭탄 설치 위치를 맞춰주는 코드
			VEC3D tempVector;
			i3Vector::Copy(&tempVector, const_cast<VEC3D*>(pInfo->getInstallPosition()));
			i3Vector::Add(&tempVector, 0.f, 0.1f, 0.f);

			// 자신이 C4가 없으면 생성해서 월드에 붙여줍니다.
			if (pPlayer->getWeaponContext()->getWeapon(WEAPON_SLOT_MISSION) == nullptr)
			{
				T_ItemID Itemid = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MISSION, WEAPON_CLASS_MISSION, WEAPON::getItemIndex(WEAPON::C4));
				pPlayer->Cmd_CreateWeapon(WEAPON_SLOT_MISSION, Itemid);
			}

			pPlayer->Cmd_DropWeapon(WEAPON_SLOT_MISSION, &tempVector);
		}
	}

	return true;
}

bool CGameUDPHandler::_ProcessRecvPacket_StageInfo_ControledObject(INT32 idx, RECV_P2P_DATA * pRecvData)
{
	GAMEINFO_OBJECT_NETDATA * pRecvObjectInfo = reinterpret_cast<GAMEINFO_OBJECT_NETDATA*>(pRecvData->pArg);

	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object(idx);

	/*GlobalFunc::PB_TRACE("_ProcessRecv idx %d _nShellCount %d _nState %d _bIntrudeCheckForControledObject %d _fKey %f", idx, pRecvObjectInfo->_nShellCount, pRecvObjectInfo->_nState, pObjectInfo->_bIntrudeCheckForControledObject, pRecvObjectInfo->_fKey);*/

	if (pObjectInfo->_pObjectRef == nullptr)	return false;
	if (CGameMissionManager::i()->isMissionObject(pObjectInfo->_pObjectRef)) return false;

	if (i3::same_of<CGameObjectWeaponBox*>(pObjectInfo->_pObjectRef))
	{	// WeaponBox의 respawn timer를 맞춘다.
		CGameObjectWeaponBox * pBox = (CGameObjectWeaponBox*)pObjectInfo->_pObjectRef;

		if (pRecvObjectInfo->getState() == 0)
			pBox->DisableObjectInBox();
		else
			pBox->EnableObjectInBox();

		pBox->setRespawnTimer(pRecvObjectInfo->getKeyFrame());
		pBox->setMaxShellCount(pRecvObjectInfo->getShellCount());
	}
	else if (i3::same_of<CGameObjectEvent*>(pObjectInfo->_pObjectRef))
	{
		CGameObjectEvent * pEvent = (CGameObjectEvent*)pObjectInfo->_pObjectRef;
		if (pRecvObjectInfo->getState() == 0)
			pEvent->SetEnable(false);
		else
			pEvent->SetEnable(true);
	}

	pObjectInfo->_bIntrudeCheckForControledObject = true;

	//I3TRACE( "StageInfo Recv StaticObject %d\n", nNetIdx);

	return true;
}


void CGameUDPHandler::_CheckGameDataToSend(CGameCharaBase * pChara)
{
#if defined( DEF_OBSERVER_MODE)
	I3ASSERT(BattleSlotContext::i()->isObserverMe() == false);
#endif

	INT32 slotIdx = pChara->getCharaNetIndex();

	I3_BOUNDCHK(slotIdx, SLOT_MAX_COUNT);

	GAMEINFO_CHARACTER * pCharaInfo = setGameInfo_Chara(slotIdx);
	GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(slotIdx);

	// Host는 죽어 있는 경우가 있습니다. 이 경우 HP가 0인 경우를 보내줘야 합니다. [mantis 0000081]
	// HitPoint
	pSendBuffer->_tHP.setHP((UINT16)pChara->getCharaInfo()->GetHP());

	if (pChara->getWeaponContext() == nullptr && pChara->IsDino() == false)
		return;

	UINT8 fireAction = 0;
	WEAPON::ATTACK_TYPE eAttackType = WEAPON::ATTACK_TYPE_L1;

	if (pChara->IsDino() == false)
	{
		// current weapon info
		WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();
		if (pWeapon != nullptr)
		{
			bool bWeaponExtension = pWeapon->isExtensionActivate();

			if (pSendBuffer->_tWeaponData.getWeaponUsage() != (WEAPON_SLOT_TYPE)pChara->getWeaponContext()->getCurrentSlotNum())
			{
				if (pSendBuffer->SetInfo_Weapon(pWeapon->getWeaponInfo()->GetItemID(), bWeaponExtension, pWeapon->GetAttackType()))
				{
					I3TRACE("_tWeaponData : %d, getCurrentSlotNum : %d\n", (INT32)pSendBuffer->_tWeaponData.getWeaponUsage(), (INT32)pChara->getWeaponContext()->getCurrentSlotNum());
					
					// 현재 무기의 MultiWeaponSync  전송
					if (pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
					{
						pSendBuffer->_tCharaState.setMultiWeapon(pChara->getMultiWeaponContext()->GetCurrSlotInfo()->nSyncNumber);
					}
					else
						pSendBuffer->_tCharaState.setMultiWeapon(0);

					if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
						pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
				}
			}
			else
			{
				if (pChara->getMultiWeaponContext()->GetCurrSlotInfo() != nullptr && pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY)
				{
					if (pSendBuffer->_tCharaState.getMultiWeapon() != pChara->getMultiWeaponContext()->GetCurrSlotInfo()->nSyncNumber)
					{
						pSendBuffer->_tCharaState.setMultiWeapon(pChara->getMultiWeaponContext()->GetCurrSlotInfo()->nSyncNumber);
						I3TRACE("CharaState MultiWeapon : %d\n", pSendBuffer->_tCharaState.getMultiWeapon());
					}
				}
			}

			// 네트워크로 동작만 보내주는 무기
			pWeapon = pWeapon->getUsedWeapon();
			if (pWeapon != nullptr)
			{
#if defined( USE_CLIENT_SENDCOUNT)
				fireAction = pSendBuffer->_tCharaState.getFireAction();
				fireAction |= (UINT8)pWeapon->isWeaponStateMask(WSM_NEED_SEND_FIREACTION);
#else
				fireAction = (UINT8)pWeapon->isWeaponStateMask(WSM_NEED_SEND_FIREACTION);
#endif

				pWeapon->removeWeaponStateMask(WSM_NEED_SEND_FIREACTION);

				// 현재 무기의 공격 타입을 보냅니다.
				eAttackType = pWeapon->GetAttackType();

				// 원본 무기의 ItemID를 보냅니다.
				UINT32 ui32WeaponID = pWeapon->getWeaponInfo()->GetItemID();
				if (pWeapon->getPairWeapon() != nullptr)
				{
					ui32WeaponID = pWeapon->getPairWeapon()->getWeaponInfo()->GetItemID();
					bWeaponExtension = pWeapon->getPairWeapon()->isExtensionActivate();
				}

				if (pSendBuffer->SetInfo_Weapon(ui32WeaponID, bWeaponExtension, eAttackType))
				{
					if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
						pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
				}
			}
			else
			{
				fireAction = 0;
				pSendBuffer->SetInfo_Weapon(0, false, eAttackType);
			}
		}
	}
	else
	{
		fireAction = (UINT8)((CGameCharaDino*)pChara)->GetNowAttackFirstFrame();

		((CGameCharaDino*)pChara)->SetNowAttackFirstFrame(false);

		UINT32 weaponID = ((CGameCharaDino*)pChara)->GetDinoWeapon()->getWeaponInfo()->GetItemID();
		if (((CGameCharaDino*)pChara)->GetFireOrder() == WEAPON::SECONDARY_FIRE)
			eAttackType = WEAPON::ATTACK_TYPE_R1;
		pSendBuffer->SetInfo_Weapon(weaponID, false, eAttackType);
	}

	// character action state
	UINT8 bodyLower = (UINT8)pChara->getActionContext()->getBodyLower();
	UINT8 bodyUpper = (UINT8)pChara->getActionContext()->getBodyUpper();
	if (pChara->isCharaStateMask(CHARA_STATE_ZOOM))
		bodyUpper = CHARA_BODYUPPER_ZOOM;
	UINT8 upper = (UINT8)pChara->getActionContext()->getUpper();
	UINT8 lower = (UINT8)pChara->getActionContext()->getLower();
	UINT8 jumpstate; 
	{
		if		(pChara->isCharaStateMask(CHARA_STATE_FORCEJUMP))		jumpstate = CHARA_SPACE_FORCE; 
		else if (pChara->isCharaStateMask(CHARA_STATE_FLOOR))			jumpstate = CHARA_SPACE_NONE;
		else if (pChara->isCharaStateMask(CHARA_STATE_JUMP))			jumpstate = CHARA_SPACE_JUMP;
		else															jumpstate = CHARA_SPACE_DOWN;
	}

	if (pSendBuffer->SetInfo_ActionState(bodyLower, bodyUpper, upper, lower, fireAction, jumpstate, WEAPON::AttackType2FireOrder(eAttackType)))
		pCharaInfo->_bSendFlag = true;

	//UINT8 moveKey = (UINT8)(pChara->getInputContext()->getKeyPressInput() & 0x0F);
	//if (pSendBuffer->_tActionKey.getMoveKey(0) != moveKey)
	//{
	//	pSendBuffer->_tActionKey.setActionCount(1);
	//	pSendBuffer->_tActionKey.setMoveKey(0, moveKey);
	//	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_ACTIONKEY;
	//	pCharaInfo->_bSendFlag = true;
	//}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// chara position, theta, rho

	{
		bool bPosChange = false;
		bool bOnLoadObject = false;
		VEC3D * pPos = pChara->GetPos();

		if (pChara->isCharaStateMask(CHARA_STATE_ONOBJECT))
		{
			bOnLoadObject = true;
			pPos = pChara->getMoveContext()->getRideObjectLocalPos();
		}

		if (pSendBuffer->SetInfo_Movement((INT16)pChara->getPortalID(), pPos, pChara->getBoneContext()->getTheta(), pChara->getBoneContext()->getRho()))
		{
			bPosChange = true;
		}

		if (pSendBuffer->SetInfo_TakingObject(bOnLoadObject, (UINT16)pChara->getMoveContext()->getRideObjectIndex()))
		{
			bPosChange = true;
		}

		if (bPosChange)
		{
			pCharaInfo->_bPosSendCheck = true;
			pCharaInfo->_bPosStopSendCheck = false;
		}
	}

	// Mission data
	pCharaInfo->_rMissionSyncSendTime += g_pViewer->getDeltaTime();

	/** \brief 폭파 미션용 패킷은 설치중, 설치완료, 해체중, 해체 완료를 초당 5회 보냅니다.
	\date 2011.07.08*/
	if (((pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_ARM_START) ||
		(pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_DISARM_START) ||
		(pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_USE_START) ||
		//	(pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_ARM_DONE) ||
		(pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_USE_DONE)
		//	(pSendBuffer->_tMissionData.GetState() == NET_MISSION_TRIGGER_DISARM_DONE) ) 
		&& (getGameInfo_Chara(slotIdx)->_rMissionSyncSendTime > UDPSENDTIME_MISSION)))
	{
		pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_MISSION;
		pCharaInfo->_rMissionSyncSendTime = 0.f;
		pCharaInfo->_bSendFlag = true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	//									process send sync packet								//
	//////////////////////////////////////////////////////////////////////////////////////////////
	pCharaInfo->_rCharaPosSendTime += g_pViewer->getDeltaTime();

	if (pCharaInfo->_bPosSendCheck)
	{
		if (pCharaInfo->_rCharaPosSendTime > UDPSENDTIME_SYNCPOS)
		{
			// 이동 패킷을 보낸중에 싱크용으로 무기 정보를 보냅니다.
			pCharaInfo->_rWeaponSyncSendTime += pCharaInfo->_rCharaPosSendTime;
			if (pCharaInfo->_rWeaponSyncSendTime > 1.5f)
			{
				if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
					pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
				pCharaInfo->_rWeaponSyncSendTime = 0.f;
				if (i3Math::abs(BattleSyncTimer::i()->GetTimeElapsed() - getElapsedTime()) > 5.f)
				{
					setElapsedTime(BattleSyncTimer::i()->GetTimeElapsed());
					//pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_WEAPONPARAM );
				}
			}

			pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT);
			pCharaInfo->_bPosSendCheck = false;
			pCharaInfo->_rCharaPosSendTime = 0.f;
			pCharaInfo->_bSendFlag = true;
		}
	}
	else if (!pCharaInfo->_bPosStopSendCheck)
	{	// 캐릭터 인터폴레이트로 인해 정지하고 일정시간 후에 한번 더 보내줘야 합니다.
		// 정지했다는 패킷을 받지 못하면 그 캐릭터는 계속 이동하는 모션을 취할 것입니다.
		if (pCharaInfo->_rCharaPosSendTime > UDPSENDTIME_SYNCPOS)
		{
			// 이동 패킷을 보낸중에 싱크용으로 무기 정보를 보냅니다.
			pCharaInfo->_rWeaponSyncSendTime += pCharaInfo->_rCharaPosSendTime;
			if (pCharaInfo->_rWeaponSyncSendTime > 1.5f)
			{
				if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
					pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
				pCharaInfo->_rWeaponSyncSendTime = 0.f;
				if (i3Math::abs(BattleSyncTimer::i()->GetTimeElapsed() - getElapsedTime()) > 5.f)
				{
					setElapsedTime(BattleSyncTimer::i()->GetTimeElapsed());
					//pSendBuffer->_iInfoFlag	|= (P2P_INFO_FLAG_WEAPONPARAM );
				}
			}

			pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT);
			pCharaInfo->_bPosStopSendCheck = true;
			pCharaInfo->_rCharaPosSendTime = 0.f;
			pCharaInfo->_bSendFlag = true;
		}
	}

	if (pCharaInfo->_rCharaPosSendTime > UDPSENDTIME_NOMOVE)
	{
		if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))	pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
		pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_ONLOADOBJECT | P2P_INFO_FLAG_CHARASTATE);

		if (i3Math::abs(BattleSyncTimer::i()->GetTimeElapsed() - getElapsedTime()) > 5.f)
		{
			setElapsedTime(BattleSyncTimer::i()->GetTimeElapsed());
		}
		pCharaInfo->_rCharaPosSendTime = 0.f;
		pCharaInfo->_bSendFlag = true;
	}

	//Grenerade 패킷안에 Weapon 정보가 있다..
	//다시 보낼 필요 없다?
	if (m_AssistInfo[slotIdx]._nThrowState == GRENADE_THROW_SEND)
	{	// 아직 패킷을 못 받은 상태 계속 보내줍니다.
#if defined( USE_CLIENT_SENDCOUNT)
		if (m_AssistInfo[slotIdx]._iSendGrenadeCount < 5)
#else
		if ((pChara->getActionContext()->isUpper(CHARA_UPPER_FIRE) ||
			pChara->getActionContext()->isUpper(CHARA_UPPER_SECONDARY_FIRE)) &&
			(m_AssistInfo[slotIdx]._iSendGrenadeCount < 5))
#endif
		{
			if ((MyRoomInfoContext::i()->getStage()->IsAiMode()))
				pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCWEAPON;
			pSendBuffer->_iInfoFlag |= (P2P_INFO_FLAG_CHARAPOS | P2P_INFO_FLAG_CHARASTATE);
			pCharaInfo->_rCharaPosSendTime = 0.f;
			pCharaInfo->_bSendFlag = true;
			//GlobalFunc::PB_TRACE("_nThrowState == GRENADE_THROW_SEND");
		}
		else
		{
			m_AssistInfo[slotIdx]._nThrowState = GRENADE_THROW_NONE;
			m_AssistInfo[slotIdx]._iSendGrenadeCount = 0;
		}
	}

	//#ifdef DOMI_UDP_OBJECT
	if (pCharaInfo->_iDomiObjectSendCnt > 0)
	{
		pCharaInfo->_rDomiObjectSendTime += g_pViewer->getDeltaTime();

		if (pCharaInfo->_rDomiObjectSendTime > 0.05f)
		{
			pSendBuffer->_iInfoFlag |= getGameInfo_Chara(slotIdx)->_iDomiObjectActivateFlag;
			pCharaInfo->_rDomiObjectSendTime = 0.f;
			pCharaInfo->_iDomiObjectSendCnt--;
			pCharaInfo->_bSendFlag = true;
		}
	}
	//#endif

	// HitByObject 체크, 다른 패킷이 가는 경우에 같이 보내도록 한다.
	if (pCharaInfo->_bSendFlag && m_SendAssist[slotIdx]._SendCountForHitByObj > 0)
	{
		if (m_SendAssist[slotIdx]._SendCountForHitByObj < 4)
		{
			for (UINT32 i = 0; i < m_SendAssist[slotIdx]._CharaDamageInfoList.size(); i++)
			{
				CHARA_DAMAGEINFO * pByObjInfo = m_SendAssist[slotIdx]._CharaDamageInfoList.at(i);
				WriteGameEvent_Hit(pChara, pByObjInfo, false);
			}
		}

		m_SendAssist[slotIdx]._SendCountForHitByObj--;
		if (m_SendAssist[slotIdx]._SendCountForHitByObj == 0)
		{
			for (UINT32 i = 0; i < m_SendAssist[slotIdx]._CharaDamageInfoList.size(); i++)
			{
				CHARA_DAMAGEINFO * pByObjInfo = m_SendAssist[slotIdx]._CharaDamageInfoList.at(i);
				I3_SAFE_DELETE(pByObjInfo);
			}
			m_SendAssist[slotIdx]._CharaDamageInfoList.clear();
		}
	}

}

void CGameUDPHandler::_CheckGameDataToSendForAi(void)
{
	// 훈련전 전용
	if (false == (MyRoomInfoContext::i()->getStage()->IsAiMode()))
	{
		return;
	}

	// NPC 시작 Idx
	INT32 beginIdx;

	if (0 == BattleSlotContext::i()->getMySlotIdx() % 2)
	{
		beginIdx = 1;
	}
	else
	{
		beginIdx = 0;
	}

	// 모든 NPC 정보를 쌓는다.
	for (INT32 i = beginIdx; (i / 2) < AIModeContext::i()->EnemyCount && i < SLOT_MAX_COUNT; i += 2)
	{
		CGameCharaBase* aiChara = g_pCharaManager->getCharaByNetIdx(i);

		if (aiChara != nullptr)
			_CheckGameDataToSend(aiChara);
	}
}

void CGameUDPHandler::_CheckGameDataToSendForHost()
{
	INT32 i;

	for (i = 0; i < SLOT_MAX_COUNT; i++)
	{
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(i);

		if (pChara == nullptr)	continue;

		GAMEINFO_CHARA_NETDATA * pSendBuffer = setNetInfo_Chara(i);
		GAMEINFO_CHARACTER * pCharaInfo = setGameInfo_Chara(i);

		pCharaInfo->_rObjectSyncSendTime += g_pViewer->getDeltaTime();
		if (pCharaInfo->_rObjectSyncSendTime > 1.f)
		{
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_SYNCOBJECTSTATE;
			pCharaInfo->_rObjectSyncSendTime = 0.f;
			pCharaInfo->_bSendFlag = true;
		}

		pCharaInfo->_rLifePacketSendTime += g_pViewer->getDeltaTime();
		if (pCharaInfo->_rLifePacketSendTime > 1.f)
		{
			pSendBuffer->_iInfoFlag |= P2P_INFO_FLAG_CS_LIFE;
			pCharaInfo->_rLifePacketSendTime = 0.f;
			pCharaInfo->_bSendFlag = true;
		}
	}
}

namespace
{
	INT32 Random(INT32 n)
	{
		return rand() % n;
	}

	void ExcuteRandomDeath(CGameCharaBase* chara, fb_fsm::StateMgr* sm)
	{
		INT32 id = 0;

		if (chara->IsSting())
			id = fb_fsm::domi_sting::DEATH_MID1 + Random(3);
		else if (chara->IsRaptor())
			id = fb_fsm::domi_raptor::DEATH_MID1 + Random(3);

		fb_fsm::Event evt(id);
		sm->Notify(&evt);
	}

	bool IsDeathPacket(UINT8 st, UINT8 subSt)
	{
		return (st == DINO_STATE_DEAD && subSt == DINO_DEAD_STATE_NORMAL) ? true : false;
	}

	bool IsMovingPacket(UINT8 st)
	{
		return (st == DINO_STATE_MOVE) ? true : false;
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Dino_FullBody(GAMEINFO_CHARA_NETDATA* pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	CGameCharaFullbody* dino = static_cast<CGameCharaFullbody*>(pChara);

	if (dino->isCharaStateMask(CHARA_STATE_DEATH)) return;

	fb_fsm::StateMgr* sm = dino->GetFBStateMgr();
	if (!sm)
		return;

	fb_fsm::State* pState = static_cast<fb_fsm::State*>(sm->GetCurState());
	if (pState == nullptr)
		return;

	UINT8 st = pRecvBuf->_tCharaState.getFBState();
	UINT8 subSt = pRecvBuf->_tCharaState.getFBSubState();

	// 서버 요구 사항으로(랜덤 계산량 클라로 이전) 스팅과 랩터의 DINO_DEAD_STATE_NORMAL은 DEATH_MID1/MID2/MID3 중 랜덤으로 클라에서 출력. 
	// 클라별로 DEATH 애니메이션이 달라지나 버그는 아님.
	if (IsDeathPacket(st, subSt) && (dino->IsRaptor() || dino->IsSting()))
	{
		ExcuteRandomDeath(dino, sm);
		return;
	}

	fb_fsm::Event evt(st, subSt, dino);
	if (pState->GetID() != evt.id)
		sm->Notify(&evt);
}

//-------------------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------------------//
//	패킷으로 받은 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_ActionState(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	pChara->getInputContext()->ProcessActionBody((CHARA_ACTION_BODYLOWER)pRecvBuf->_tCharaState.getBody());
	pChara->getInputContext()->ProcessActionBody2((CHARA_ACTION_BODYUPPER)pRecvBuf->_tCharaState.getBody2());
	pChara->getInputContext()->ProcessActionUpper((CHARA_ACTION_UPPER)pRecvBuf->_tCharaState.getUpper());

	// 발사 동작이 있는 경우 모션만 취합니다.
	if (!pChara->isLocal() && pRecvBuf->_tCharaState.getFireAction() == 1)
	{
		VEC3D vStartPos;

		pChara->getCameraContext()->getVirtualCamPosition(&vStartPos);

		INT32 iSlot = getNetInfo_Chara(pChara->getCharaNetIndex())->_tWeaponData.getWeaponUsage();
		pChara->Cmd_Attack(pRecvBuf->_tCharaState.getWeaponFireOrder(), &vStartPos, nullptr, true, iSlot);
	}

	if (!BattleServerContext::i()->IsP2PHost() && !pChara->isInvincible())
	{
		// 드랍된 상태라면 헬멧을 떨어뜨립니다.
		if (pRecvBuf->_tCharaState.getHelmet() == NET_HELMET_STATE_DROP)
		{
			pChara->DropEquip();
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_PosRotation(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara, bool bRotated /*= false*/)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	pChara->getInputContext()->ProcessPosition(pRecvBuf);
	if (!bRotated)
		pChara->getInputContext()->ProcessRotationTheta(pRecvBuf);
	pChara->getInputContext()->ProcessRotationRho(pRecvBuf);
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_ActionKey(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	for (INT32 i = 0; i < pRecvBuf->_tActionKey.getActionCount(); i++)
	{
		I3TRACE("Action start _ProcessRecvPacket_Chara_ActionKey\n");
		UINT16 objIndex = pRecvBuf->_tActionKey.getUseActionObjectIndex(i);
		if (objIndex >= OBJ_TOTAL_COUNT)
			return;

		bool bOnLoadObject = pCharaNetData->_tTakingObject.getOnLoadObject();
		UINT8 actionKey = pRecvBuf->_tActionKey.getActionKey(i);

		//GlobalFunc::PB_TRACE("_nActionKey %d", pRecvBuf->_tDefData._nActionKey);
		// 오브젝트 사용
		I3TRACE("Action Key Use Point _ProcessRecvPacket_Chara_ActionKey\n");
		if ((actionKey & NET_ACTIONKEY_USEOBJECT))
		{
			const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

			//I3TRACE( "Object use. %d\n", pRecvBuf->_tDefData._nUseActionObjectIdx);

			if (pObjectInfo->_pObjectRef != nullptr)
			{
				if (i3::same_of<CGameObjectRidingHelicopter*>(pObjectInfo->_pObjectRef))
				{
					if (BattleServerContext::i()->IsP2PHost() &&
						((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject) == false)
					{
						pRecvBuf->_tActionKey.removeActionKey(i, NET_ACTIONKEY_USEOBJECT);
					}
					else
					{
						((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnableObject();
						((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
					}
				}
				else if (i3::same_of<CGameObjectWeaponBox*>(pObjectInfo->_pObjectRef))
				{
					CGameObjectWeaponBox * pRPG7Box = (CGameObjectWeaponBox*)pObjectInfo->_pObjectRef;
					pRPG7Box->HoldToChara(pChara);
				}
				else if (i3::same_of<CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
				{
					if (BattleServerContext::i()->IsP2PHost() &&
						((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject) == false)
					{
						pRecvBuf->_tActionKey.removeActionKey(i, NET_ACTIONKEY_USEOBJECT);
					}
					else
					{
						((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
					}
				}
				else if (i3::same_of<CGameObjectEvent*>(pObjectInfo->_pObjectRef))
				{
					CGameObjectEvent * ObjEvent = (CGameObjectEvent*)pObjectInfo->_pObjectRef;
					BattleObjContext::i()->ObjectHitReceveFunction((OBJ_EVENT_TYPE)ObjEvent->GetType(), ObjEvent, pChara);
				}
			}
		}
		else if ((actionKey & NET_ACTIONKEY_LEAVE_WEAPON_ON_OBJECT))
		{
			const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

			if (pObjectInfo->_pObjectRef != nullptr)
			{
				if (i3::same_of<CGameObjectRidingHelicopter*>(pObjectInfo->_pObjectRef))
				{
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject();
				}
				else if (i3::same_of<CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
				{
					((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject();
				}
			}
		}
		else if ((actionKey & NET_ACTIONKEY_ENTER_WEAPON_ON_OBJECT))
		{
			const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

			if (pObjectInfo->_pObjectRef != nullptr)
			{
				if (i3::same_of<CGameObjectRidingHelicopter*>(pObjectInfo->_pObjectRef))
				{
					if (((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject))
					{
						((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
					}
				}
				else if (i3::same_of<CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
				{
					if (((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject))
					{
						((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
					}
				}
			}
		}
		else if ((actionKey & NET_ACTIONKEY_USE_MOVE_CHARA_OBJECT))
		{
			const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

			if (pObjectInfo->_pObjectRef != nullptr)
			{
				if (i3::same_of<CGameObjectJumpHold*>(pObjectInfo->_pObjectRef) &&
					((pChara->getCharaNetIndex()) != BattleSlotContext::i()->GetMySlotIdxEx()))
				{
					CGameObjectJumpHold * ObjJumpHold = (CGameObjectJumpHold*)pObjectInfo->_pObjectRef;
					if (ObjJumpHold)
					{
						ObjJumpHold->PlayJumpAnim();
						g_pSndMng->StartObjectSnd(GSND_OBJECT_JUMPHOLD, ObjJumpHold->GetPos());
					}
				}
			}
		}
		else if ((actionKey & NET_ACTIONKEY_USEITEMBOX))
		{
			const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

			if (pObjectInfo->_pObjectRef != nullptr)
			{
				if(i3::same_of<CGameObjectBattleUseItem*>(pObjectInfo->_pObjectRef))
				{
					// Item을 사용합니다.
					CGameObjectBattleUseItem * pItemBox = (CGameObjectBattleUseItem*)pObjectInfo->_pObjectRef;
					UINT8 nActionKey = pRecvBuf->_tActionKey.getMoveKey(i) & 0x0F;
					UINT8 nMultiWeaponSync = (pRecvBuf->_tActionKey.getMoveKey(i) >> 4) & 0x0F;
					pItemBox->UseItem( nActionKey, pChara, nMultiWeaponSync);
					
					// 사용했으니 사운드 틀어준다.
					g_pSndMng->StartObjectSnd(GSND_OBJECT_BATTLE_USE_ITEM, pItemBox->GetPos());

					// Item 사용하는 순간 Enable을 꺼줘야 Sync가 맞는 형식으로 보입니다.
					if (pItemBox->isEnable() == TRUE )
						pItemBox->SetEnable(FALSE);
				};
			}
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_OnLoadObject(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pCharaNetData != nullptr);
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);
}

// 발사 패킷 또는 무기 스왑 받으면 무기 싱크 맞추는 곳
void CGameUDPHandler::_ProcessRecvPacket_Chara_WeaponSync(GAMEINFO_CHARA_WEAPON * pRecvBuf, GAMEINFO_CHARA_STATE * pRecvCharaStateBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	if (pChara->IsDino())
	{
		return;
	}

	// 자신은 스킵 (7080 : 근접 투척 무기 모두 소진 후 M7 으로 변경하지 않음)
	if (pChara->isLocal())
		return;

	I3TRACE("### %d character %d slot WeaponSync :%d\n", pChara->getCharaNetIndex(), pChara->getWeaponContext()->getCurrentSlotNum(), pRecvCharaStateBuf->getMultiWeapon() );

	// 무기 슬롯을 먼저 변경합니다.
	// 소음기일 경우에 Extension을 미리 바꾸어주어야 무기 스왑을 한 후에 바로 소음기를 달고 있다.
	WeaponBase * pWeapon = pChara->getWeaponContext()->getWeapon(pRecvBuf->getWeaponUsage());

	if (pRecvBuf->getWeaponUsage() != pChara->getWeaponContext()->getCurrentSlotNum())
	{
		// 2014-09-18 익스텐션이 듀얼모드 변환이 있을 경우에는 익스텐션 엑티비티리를 미리 켜줘야한다.
		// 그렇지 않다면 싱크가 맞지 않는다. 위의 소음기랑 비슷한 문제.
		// ENG_김대영
		if (i3::same_of<WeaponKrissSuperV*, WeaponShotGunGenderWithDual*>(pWeapon))
		{
			pWeapon->ExtensionActivate(pRecvBuf->getWeaponExtension(), true);
		}

		pChara->Cmd_ChangeWeapon(pRecvBuf->getWeaponUsage());

		//if( (pWeapon != nullptr) && pWeapon->IsExtensionWeapon())
		//{
		//	// 2014-08-05 나이프 투척 무기에서만 변환하도록 조건이 걸려있어 버그 발생하여 조건 수정합니다.
		//	// ENG_김대영
		//	if ( !(WEAPON::getItemIndex( WEAPON::CHINESECLEAVER) == pRecvBuf->getWeaponNumber() || 
		//		WEAPON::getItemIndex( WEAPON::CHINESECLEAVER_SEIZED) == pRecvBuf->getWeaponNumber() ||
		//		WEAPON::getItemIndex( WEAPON::CHINESECLEAVER_CNY2015) == pRecvBuf->getWeaponNumber()))
		//	{
		//		pWeapon->setExtensionActivate(pRecvBuf->getWeaponExtension());
		//	}
		//}
	}

	// 무기가 맞는지 체크해봐야합니다.
	// Fire , 난입시가 아니더라도 서버 정보와 무기가 다를 경우 매번 동기화를 진행하여 주므로 항상 확인합니다. - 서동권
	//if( (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_FIRE) || IntrudeContext::i()->IsIntrudeStart())
	{
		pChara->getWeaponContext()->WeaponValidCheck( pRecvBuf->getWeaponClass(), pRecvBuf->getWeaponNumber(), static_cast<WEAPON_SLOT_TYPE>( pRecvBuf->getWeaponUsage()), pRecvCharaStateBuf->getMultiWeapon(), pRecvBuf->getWeaponSwap());
	}

	// 본인의 Extension 상태가 바뀌지 않도록
	// 현재 무기는 다음 프레임에 바뀌게 된다. 바뀔 무기로 선택되야 함. 2014.10.24 swoongoo
	pWeapon = pChara->getWeaponContext()->getWeapon(pRecvBuf->getWeaponUsage());

	// Weapon Extension
	if ((pWeapon != nullptr) && pWeapon->IsExtensionWeapon())
	{
		bool bActivate = pRecvBuf->getWeaponExtension();
		if (bActivate != pWeapon->isExtensionActivate())
		{
			if (i3::same_of<WeaponCrossFire*>(pWeapon))
			{
				// 무기 익스텐션 상태로 캐릭터에 명령을 보내야하는 무기
				pChara->Cmd_Extension(bActivate);
			}
			else
			{
				// revision 42335 SS1-R5 익스텐션 변환시 총기 애니메이션 안바뀌는 버그 수정 (redmine.5406)
				// 수정 : WeaponGunWithKnife도 ExtensionActivate(bActivate, false);를 수행해야 함.
				// revision 35022  DSR 익스텐션재장전 모션 버그 수정

				if (i3::same_of<WeaponSniperDualMagazine*>(pWeapon))
				{
					pChara->Cmd_Extension(bActivate);
				}
				else if (i3::same_of<WeaponGunWithKnife*>(pWeapon) == false)
				{
					if (i3::same_of<WeaponMiniAxe*>(pWeapon))
						bActivate = !bActivate;

					pChara->Cmd_Extension(bActivate);
					pWeapon->ExtensionActivate(bActivate, false);
				}

				// hansoft. 7210. SS1-R5 Carbine의 Extension 해제 모션에 칼이 보이지 않음.
				// 익스텐션 무기의 visible은 익스텐션 교체 애니메이션 중일 때
				// 특정 시점에서(OnExtensionActivate(), OnExtensionDeactivate()) 호출되어야 함. 
				//pWeapon->OnSetVisibleExtension( pRecvBuf->getWeaponExtension());

				//관전시 익스텐션을 On/Off하면 관전자의 HUD에도 무기 이름을 반영되게 수정(여기말고는 마땅히 호출할 곳이 없어 보임)
				observer_hud::Context::i()->Notify(observer_hud::EVT_WEAPON_CHANGED, pChara->getCharaNetIndex());
			}
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_RadioChat(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	//	char szMessage[128];
	i3::stack_wstring  wstrMessage;
	i3::stack_wstring  wstrSector;
	//	char szSector[256];

	CStageBattle * pStage = g_pFramework->GetStageBattle();
	if (pStage == nullptr)
		return;

#if defined( DEF_OBSERVER_MODE)
	// Observer는 처리하지 않는다.
	if (BattleSlotContext::i()->isObserverMe())
	{
		return;
	}
#endif

	I3ASSERT(pChara != nullptr);

	// Radio Chat
	UINT8 radio = pRecvBuf->_tRadioChat.getRadio();
	if (radio > 0)
	{
		if (pChara->getCharaTeam() == BattleSlotContext::i()->getMyTeam())
		{
			RADIOCHAT_TYPE	nType = (RADIOCHAT_TYPE)(radio / 10);
			INT32	nMsgNum = (INT32)(radio) % 10;

			if (nType < RADIOCHAT_TYPE_AUTO)
			{
				i3::rc_wstring wstrRadioMessage = GameContextUtil::GetRadioMessage(nType, nMsgNum);

				i3World * pWorld = pStage->getWorld();
				I3ASSERT(pWorld != nullptr);
				i3MapSectorTable * pTable = pWorld->getMapSectorTable();

				if (pTable != nullptr)
				{
					INT32 idNew = pRecvBuf->_tRadioChat.getSectorIndex();
					I3ASSERT((idNew >= 0) && (idNew < pTable->getCount()));
					i3::pack::MAP_SECTOR_INFO * pSector = pTable->getMapSector(idNew);
					I3ASSERT(pSector != nullptr);

					if (i3::generic_string_size(pSector->m_szName) > 0 && idNew != 0)
					{
						i3::sprintf(wstrSector, L"{col:98,180,253,255}[%s]{/col}", pSector->m_szName);		// 자동변환..
						i3::sprintf(wstrMessage, L"[Radio] %s %s", wstrRadioMessage, wstrSector);
					}
					else
					{
						i3::sprintf(wstrMessage, L"[Radio] %s", wstrRadioMessage);
					}
				}
				else
				{
					i3::sprintf(wstrMessage, L"[Radio] %s", wstrRadioMessage);
				}

				g_pFramework->getChatBox()->PutUserChatting(BattleSlotContext::i()->getNickForSlot(pChara->getCharaNetIndex()), wstrMessage, false);
			}
			else if (nType == RADIOCHAT_TYPE_AUTO)
			{	// Auto message
				if (nMsgNum != 1)
				{
					//수류탄은 메시지를 출력하지 않고 사운드만 출력합니다.
					i3::rc_wstring wstrRadioMessage = GameContextUtil::GetRadioMessage(nType, nMsgNum);
					g_pFramework->getChatBox()->PutUserChatting(BattleSlotContext::i()->getNickForSlot(pChara->getCharaNetIndex()), wstrRadioMessage, false);
				}
			}

			GameContextUtil::PlayRadioMessageSound(nType, nMsgNum, pChara);
		}
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_ActionForObjectSync(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);
	UINT8 state = pRecvBuf->_tSyncObject.getState();
	UINT16 objIndex = pRecvBuf->_tSyncObject.getIndex();

	if ((state & NET_SYNCKEY_ON_OBJECT))
	{
		// 아웃포스트 헬기만 해당됩니다.. 그외의 오브젝트가 세팅된다면 문제가 발생합니다.
		const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

		if (pObjectInfo->_pObjectRef != nullptr)
		{
			bool bOnLoadObject = getNetInfo_Chara(pChara->getCharaNetIndex())->_tTakingObject.getOnLoadObject();

			if (i3::same_of<CGameObjectRidingHelicopter*>(pObjectInfo->_pObjectRef))
			{
				I3_BOUNDCHK(pChara->getCharaNetIndex(), SLOT_MAX_COUNT);
				if (((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject))
					((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
			}
			else if (i3::same_of<CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
			{
				I3_BOUNDCHK(pChara->getCharaNetIndex(), SLOT_MAX_COUNT);
				if (((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->IsEnableEnter(pChara, bOnLoadObject))
					((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->EnterObject(pChara);
			}
		}
	}
	else if ((state & NET_SYNCKEY_OFF_OBJECT))
	{
		const GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->getGameInfo_Object(objIndex);

		if (pObjectInfo->_pObjectRef != nullptr)
		{
			if (i3::same_of<CGameObjectRidingHelicopter*>(pObjectInfo->_pObjectRef))
				((CGameObjectRidingHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject(pChara->getCharaNetIndex());
			else if (i3::same_of<CGameObjectRespawnHelicopter*>(pObjectInfo->_pObjectRef))
				((CGameObjectRespawnHelicopter*)pObjectInfo->_pObjectRef)->LeaveObject(pChara->getCharaNetIndex());
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	미션용 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_MissionDataForClient(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	switch (pRecvBuf->_tMissionData.GetState())
	{
	case NET_MISSION_TRIGGER_ARM_START:
		if (pChara->isNetwork() && !pChara->getActionContext()->isUpper(CHARA_UPPER_FIRE))
		{
			if ((pChara->GetCurrentCharaWeapon() != nullptr) &&
				(pChara->GetCurrentCharaWeapon()->getWeaponInfo() != nullptr) &&
				(pChara->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION))
			{	// 확인 코드. 미션용 C4만 애니메이션을 하도록 수정. 2009.06.09 dfly79
				pChara->Cmd_Attack();
				//pChara->Cmd_Only_AttackWeapon();
			}
		}
		CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), true);
		break;
	case NET_MISSION_TRIGGER_ARM_CANCEL:
		if (pChara->isNetwork() && pChara->getActionContext()->isUpper(CHARA_UPPER_FIRE))
			pChara->Cmd_Idle();
		CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), false);
		break;
	case NET_MISSION_TRIGGER_ARM_DONE:			break;
	case NET_MISSION_TRIGGER_ARM_FAIL:			CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), false);	break;
	case NET_MISSION_TRIGGER_DISARM_START:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), true);									break;
	case NET_MISSION_TRIGGER_DISARM_DONE:		break;
	case NET_MISSION_TRIGGER_DISARM_CANCEL:	CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);									break;
	case NET_MISSION_TRIGGER_DISARM_FAIL:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);									break;
	case NET_MISSION_TRIGGER_USE_START:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), true);									break;
	case NET_MISSION_TRIGGER_USE_DONE:			break;
	case NET_MISSION_TRIGGER_USE_CANCEL:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);									break;
	case NET_MISSION_TRIGGER_USE_FAIL:			CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);
	default: break;
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_MissionDataForHost(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	// Process
	{
		switch (pRecvBuf->_tMissionData.GetState())
		{
		case NET_MISSION_TRIGGER_ARM_START:
			if (pChara->isNetwork() && !pChara->getActionContext()->isUpper(CHARA_UPPER_FIRE))
			{
				if ((pChara->GetCurrentCharaWeapon() != nullptr) &&
					(pChara->GetCurrentCharaWeapon()->getWeaponInfo() != nullptr) &&
					(pChara->GetCurrentCharaWeapon()->getWeaponInfo()->GetTypeClass() == WEAPON_CLASS_MISSION))
				{	// 확인 코드. 미션용 C4만 애니메이션을 하도록 수정. 2009.06.09 dfly79
					pChara->Cmd_Attack();
					//pChara->Cmd_Only_AttackWeapon();
				}
			}
			CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), true);
			break;
		case NET_MISSION_TRIGGER_ARM_CANCEL:
			if (pChara->isNetwork() && pChara->getActionContext()->isUpper(CHARA_UPPER_FIRE))
				pChara->Cmd_Idle();
			CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), false);
			break;
		case NET_MISSION_TRIGGER_ARM_DONE:			CGameMissionManager::i()->SendInstallComplete(pChara->getCharaNetIndex());								break;
		case NET_MISSION_TRIGGER_ARM_FAIL:			CGameMissionManager::i()->Install(pChara->getCharaNetIndex(), pRecvBuf->_tMissionData.GetArea(), false);	break;
		case NET_MISSION_TRIGGER_DISARM_START:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), true);									break;
		case NET_MISSION_TRIGGER_DISARM_DONE:		CGameMissionManager::i()->SendUninstallComplete(pChara->getCharaNetIndex());								break;
		case NET_MISSION_TRIGGER_DISARM_CANCEL:	CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);									break;
		case NET_MISSION_TRIGGER_DISARM_FAIL:		CGameMissionManager::i()->Uninstall(pChara->getCharaNetIndex(), false);									break;
		default: break;
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	낙하 데미지에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_Suicide(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pCharaNetData != nullptr);
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	bool bFalling = false;

	for (INT32 i = 0; i < pRecvBuf->_tSuicideDamageData.getCount(); i++)
	{
		if (pRecvBuf->_tSuicideDamageData.getObjectIndex(i) == 0xFFFF)
			bFalling = true;

		pChara->Cmd_DamageSelf(pRecvBuf->_tSuicideDamageData.getDamage(i),
			pRecvBuf->_tSuicideDamageData.getObjectIndex(i), bFalling);
	}

	pRecvBuf->_tSuicideDamageData.CopyTo(&pCharaNetData->_tSuicideDamageData);
}

//-------------------------------------------------------------------------------------------//
//	_CalcGetWeapon()
//	무기 습득 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_GetWeaponForClient(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	if (pChara->IsDino()) {
		return;
	}

	// check valid data
	UINT8 index = pRecvBuf->_tGetWeaponData.getIndex();
	UINT8 state = pRecvBuf->_tGetWeaponData.getSuccess();
	if (index >= MAX_DROPEDWEAPON_COUNT)		return;
	if (state == 0)						return;

	I3TRACE("_ProcessRecvPacket_Chara_GetWeaponForClient\n");

	GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(index);
	WeaponBase * pWeapon = g_pWeaponManager->FindDropWeaponByIdx(index);
	if (pWeapon != nullptr)
	{
		pChara->Cmd_PickupWeapon( pWeapon, pRecvBuf->_tGetWeaponData.GetMultiWeaponSlot());
	}
	else
	{
		// Netsync
		// 무기가 없습니다. 생성해서 붙여줍니다.
		WEAPON_CLASS_TYPE classType = (WEAPON_CLASS_TYPE)pRecvBuf->_tGetWeaponData.getWeaponClass();
		INT32 i32Number = pRecvBuf->_tGetWeaponData.getWeaponNumber();
		WEAPON_SLOT_TYPE weaponSlot = static_cast<WEAPON_SLOT_TYPE>(pRecvBuf->_tGetWeaponData.getWeaponUsage());

		pChara->getWeaponContext()->WeaponValidCheck( classType, i32Number, weaponSlot, pRecvBuf->_tCharaState.getMultiWeapon(), false);
		pWeapon = pChara->getWeaponContext()->getWeapon( weaponSlot);
	}

	if (pWeapon != nullptr)
	{
		//UINT8 bullet = pRecvBuf->_tGetWeaponData.getLoadedBulletCount();
		//UINT8 dual = pRecvBuf->_tGetWeaponData.getDualLoadedBulletCount();
		//UINT16 max = pRecvBuf->_tGetWeaponData.getMaxBulletCount();
		//pWeapon->SetBulletCountForNetwork( bullet, dual, max );

		pWeapon->SetNetBullet(pRecvBuf->_tGetWeaponData.GetBullet());
	}

	pDropedWeaponInfo->Reset();
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_GetWeaponForHost(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	if (pChara->IsDino()) {
		return;
	}

	// check valid data
	UINT8 index = pRecvBuf->_tGetWeaponData.getIndex();
	if (index >= MAX_DROPEDWEAPON_COUNT)		return;

	// Process
	{
		bool	bTakeWeapon = false;
		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(index);
		WeaponBase * pWeapon = g_pWeaponManager->FindDropWeaponByIdx(index);
		if (pWeapon != nullptr)
		{
			// 무기를 들게 합니다.
			pChara->Cmd_PickupWeapon( pWeapon, pRecvBuf->_tGetWeaponData.GetMultiWeaponSlot());
			bTakeWeapon = true;
		}
		else
		{
			// 그 캐릭터가 들고 있는지 체크합니다
			// 해당 캐릭터가 이미 들고 있지만 확인 메세지를 못 받은 경우 일 수 있습니다. 해당 무기를 들도록 합니다.
			WeaponBase * pPrimWeapon = pChara->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
			WeaponBase * pSecWeapon = pChara->getWeaponContext()->getWeapon(WEAPON_SLOT_SECONDARY);

			if (pPrimWeapon != nullptr &&
				(pPrimWeapon->getNetworkIdx() == index))
			{
				bTakeWeapon = true;
				pWeapon = pPrimWeapon;
			}
			else if (pSecWeapon != nullptr &&
				(pSecWeapon->getNetworkIdx() == index))
			{
				bTakeWeapon = true;
				pWeapon = pSecWeapon;
			}
		}

		if (bTakeWeapon)
		{
			// Set network info
			// 드랍된 무기 정보 초기화
			pDropedWeaponInfo->_fDropedWeaponLifeTime = 0.f;
			pDropedWeaponInfo->setState(NET_WEAPONDROPED_GET);

			// broadcast info set
			WriteGameEvent_PickupWeapon(pChara, pWeapon);
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	_CalcDropWeapon()
//	무기 버리기 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_DropWeaponForClient(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	UINT8 state = pRecvBuf->_tDropWeaponData.getSuccess();
	if (state == 0)	return;

	I3TRACE("_ProcessRecvPacket_Chara_DropWeaponForClient\n");

	UINT8 index = pRecvBuf->_tDropWeaponData.getIndex();

	// 살아있으면 다음 무기로 변경
	bool bNextWeapon = pRecvBuf->_tDropWeaponData.getDropType() ? false : true;

	WeaponBase * pWeapon = g_pWeaponManager->SetDropWeapon(pChara->getCharaNetIndex(),
		static_cast<WEAPON_SLOT_TYPE>(pRecvBuf->_tDropWeaponData.getWeaponUsage()), index, bNextWeapon);

	if (pWeapon != nullptr)
	{
		CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
		I3ASSERT(pWeaponInfo != nullptr);

		// 드랍된 무기에 대한 정보 세팅
		GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(index);

		pDropedWeaponInfo->Bind(pWeapon, pChara);

		BattleObjContext::i()->SetDropedWeaponCount(index + 1);

		if (((MyRoomInfoContext::i()->getStage()->IsAiMode())) == true)
		{
			UINT8 bullet = pRecvBuf->_tDropWeaponData.GetLoadBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
			UINT8 dual = pRecvBuf->_tDropWeaponData.GetLoadBulletCount(WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT);
			UINT16 max = pRecvBuf->_tDropWeaponData.GetStandbyBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
			pWeapon->SetBulletCountForNetwork(bullet, dual, max);
		}

		pWeapon->SetNetBullet(pRecvBuf->_tDropWeaponData.GetBullet());
	}
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_DropWeaponForHost(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	if (((MyRoomInfoContext::i()->getStage()->IsAiMode())) == true)
	{
		UINT8 bullet = pRecvBuf->_tDropWeaponData.GetLoadBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
		UINT8 dual = pRecvBuf->_tDropWeaponData.GetLoadBulletCount(WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT);
		UINT16 max = pRecvBuf->_tDropWeaponData.GetStandbyBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
		WriteGameEvent_DropWeapon(pChara, static_cast<WEAPON_SLOT_TYPE>(pRecvBuf->_tDropWeaponData.getWeaponUsage()),
			bullet, dual, max);
	}

}

void CGameUDPHandler::_ProcessRecvPacket_Chara_DirectPickUp(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	// 여기서 SetDrop과 PickUp 처리
	// 받은 무기로 교체합니다.
	GAMEINFO_DROPWEAPON * pDropWeapon = pRecvBuf->_tDirectPickUp.getDropWeapon();

	BOOL bCheckDropList = FALSE;

	{
		UINT8 index = pDropWeapon->getIndex();

		bool bSwap = false;

		if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() || pDropWeapon->getWeaponUsage() != WEAPON_SLOT_SECONDARY)
			bSwap = true;

		// 무기 떼어내기
		WeaponBase * pWeapon = g_pWeaponManager->SetDropWeapon(pChara->getCharaNetIndex(),
			static_cast<WEAPON_SLOT_TYPE>(pDropWeapon->getWeaponUsage()), index, bSwap, true);

		if (MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
		{
			pChara->getMultiWeaponContext()->SetDropProhibit( true);
		}

		if (pWeapon != nullptr)
		{
			CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
			I3ASSERT(pWeaponInfo != nullptr);

			// 드랍된 무기에 대한 정보 세팅
			GAMEINFO_DROPED_WEAPON * pDropedWeaponInfo = BattleObjContext::i()->setGameInfo_DropedWeapon(index);

			pDropedWeaponInfo->Bind(pWeapon, pChara);

			BattleObjContext::i()->SetDropedWeaponCount(index + 1);

			if (((MyRoomInfoContext::i()->getStage()->IsAiMode())) == TRUE)
			{
				UINT8 bullet = pDropWeapon->GetLoadBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
				UINT8 dual = pDropWeapon->GetLoadBulletCount(WEAPON::EXTENSION_ON, WEAPON::HAND_TYPE_LEFT);
				UINT16 max = pDropWeapon->GetStandbyBulletCount(WEAPON::EXTENSION_OFF, WEAPON::HAND_TYPE_LEFT);
				pWeapon->SetBulletCountForNetwork(bullet, dual, max);
			}

			pWeapon->SetNetBullet(pDropWeapon->GetBullet());

			bCheckDropList = TRUE;

			I3TRACE("Direct Pickup\n");
		}
		else
		{
			I3PRINTLOG(I3LOG_NOTICE, "Not Set Drop Weapon ID %d, PickUp Weapon %d", pDropWeapon->getWeaponID(), pRecvBuf->_tDirectPickUp.getWeaponID());
		}
	}
	
	// 이제 무기를 들 차례
	// 즉시 먹는 패킷을 만들자.
	T_ItemID GetItem = pRecvBuf->_tDirectPickUp.getWeaponID();
	WeaponBase * pPickUpWeapon = g_pWeaponManager->QueryWeapon(pChara, GetItem);

	I3ASSERT(pPickUpWeapon != nullptr);
	pPickUpWeapon->Reset();
	pPickUpWeapon->SetEndurance(100);	// revision 61642 방어미션 미리 드랍되어 있는 무기를 들 시, 내구도를 100으로 세팅해줌(hansoft. 5832)

	pChara->Cmd_PickupWeapon( pPickUpWeapon, pRecvBuf->_tDirectPickUp.getDropWeapon()->GetMultiWeaponSlot(), FALSE, FALSE);

	pCharaNetData->_iInfoFlag |= P2P_INFO_FLAG_CHARASTATE;
	GAMEINFO_CHARACTER * pCharaInfo = setGameInfo_Chara(pChara->getCharaNetIndex());
	pCharaInfo->_bSendFlag;
}

//-------------------------------------------------------------------------------------------//
//	발사 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_FireDataOnObject(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj)
{
	I3ASSERT(pObj);
	I3ASSERT(pObj->GetWeaponOnObject());

	pObj->GetWeaponOnObject()->FireWeaponOnObject();
	pObj->GetWeaponOnObject()->FireCollision(pObj->GetWeaponOnObject()->GetStartPos(), pObj->GetWeaponOnObject()->GetTargetPos());
}

//-------------------------------------------------------------------------------------------//
//	발사 패킷에 대한 처리
//-------------------------------------------------------------------------------------------//
bool CGameUDPHandler::_ProcessRecvPacket_Chara_FireData(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	I3ASSERT(pCharaNetData != nullptr);
	I3ASSERT(pRecvBuf != nullptr);
	I3ASSERT(pChara != nullptr);

	if (isNetThrowWeapon(&pRecvBuf->_tWeaponData))
	{
		if (pRecvBuf->_tFireData.isValidThrow(pChara->getCharaNetIndex()) == false)	return false;

		UINT16 idx = pRecvBuf->_tFireData.getNetSlotIndex();
		const GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon(idx);
		if (pThrowWeaponInfo->_pWeaponBase != nullptr)
		{
			//I3TRACE( "이미 사용중인 수류탄..\n");
			return false;
		}

		// revision 33497
		// 정상적으로 성공하면 +1을 한다.
		// 다음에 던질때 이 Index로 싱크를 맞추게 된다.
		pCharaNetData->_tFireData.setNetSlotIndex(idx);
	}
	else
	{
		// 같은 패킷이거나 늦게 날아온 패킷입니다.
		if (pCharaNetData->_tFireData.getID() >= pRecvBuf->_tFireData.getID())	return false;
		pCharaNetData->_tFireData.setID(pRecvBuf->_tFireData.getID());
	}

	pChara->getInputContext()->ProcessNetFire(pRecvBuf);

	return true;
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_FireNHitDataOnObject(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameObjectWithWeapon* pObj)
{

	// Process
	VEC3D	vInvDir;
	CHARA_DAMAGEINFO DamageInfo;

	WeaponOnObject* pWeapon = pObj->GetWeaponOnObject();
	I3ASSERT(pWeapon);

	CGameCharaBase * pHitUser = nullptr;

	INT32 hitCount = pRecvBuf->_tFireData._tHitData.getHitCount();

	if (hitCount > MAX_FIREHIT_COUNT)
	{
		hitCount = MAX_FIREHIT_COUNT;		// 문제가 있을 수 있습니다.
		pRecvBuf->_tFireData._tHitData.setHitCount(hitCount);
	}

	for (INT32 i = 0; i < hitCount; i++)
	{
#if defined (_SHUFFLE_HIT_PART)
		CHARA_HIT_PART hitPart = HitPartContext::i()->DecryptHitPart(pRecvBuf->_tFireData._tHitData.getHitPart(i));
#else
		CHARA_HIT_PART hitPart = (CHARA_HIT_PART)pRecvBuf->_tFireData._tHitData.getHitPart(i);
#endif
		UINT16 hitWho = pRecvBuf->_tFireData._tHitData.getHitWho(i);
		GAMEINFO_HIT_TYPE hitType = pRecvBuf->_tFireData._tHitData.getHitType(i);

		if (hitType == GHT_CHARA || hitType == GHT_AI)
		{
			if (hitPart == CHARA_HIT_UNKNOWN)	continue;
			if (((hitWho >> 7) & 0x01) != 0)	continue;

			// 캐릭터 히트
			INT32 nHitIndex = (hitWho & 0x0F);

			pHitUser = g_pCharaManager->getCharaByNetIdx(nHitIndex);

			// 맞는것과 동시에 캐릭터가 나간경우는 예외 처리가 되야합니다.. - 정순구
			if (pHitUser == nullptr)		continue;

			i3Vector::Sub(&vInvDir, pHitUser->GetPos(), pWeapon->GetMuzzlePos());
			REAL32 _dist = i3Vector::Normalize(&vInvDir, &vInvDir);

			DamageInfo.Reset();
			pWeapon->GetDamageInfo(&DamageInfo);

			DamageInfo._FireOrder = pRecvBuf->_tWeaponData.getFireOrder();
			DamageInfo._rDamage = (REAL32)pRecvBuf->_tFireData._tHitData.getHitDamage(i);
			DamageInfo._pVecDir = &vInvDir;
			DamageInfo._IsOwner = false;
			DamageInfo._nKillerIdx = pObj->GetEnterCharaIdx();

			DamageInfo._pHitResult = nullptr;
			// 월샷 데미지 감소량
			DamageInfo._WallShotReduceRate = 0;
			DamageInfo._pWallHitResult = nullptr;

			DamageInfo._HitType = hitType;
			DamageInfo.SetHitPart(hitPart);
			DamageInfo._rDistance = _dist;

			//	헤드샷 죽음
			if ((CHARA_HIT_PART)hitPart == CHARA_HIT_HEAD)
				DamageInfo._DeathType = CHARA_DEATH_H;
			//	보통 죽음
			else
				DamageInfo._DeathType = CHARA_DEATH_A;	//	CHARA_DEATH_A 는 임시이므로 다른 Death 동작이 필요하면 변경이 필요합니다.

			DamageInfo._pVecPos = pWeapon->GetMuzzlePos();

			DamageInfo._IsNPC = false;

			I3ASSERT(DamageInfo.isValid());

			if (BattleServerContext::i()->IsP2PHost())
			{	// CS만 데미지 계산
				pHitUser->NET_HitForHost(&DamageInfo, DamageInfo._rDamage);
			}
			else
			{
				//	데미지 동작
				if (pHitUser->isCharaStateMask(CHARA_STATE_DEATH) == false)
				{
					if (DamageInfo._DeathType == CHARA_DEATH_H)
						pHitUser->getEquipContext()->setHelmetProtect(true);

					pHitUser->Cmd_Damage(&DamageInfo);
				}
			}
		}
		else if (hitType == GHT_ITEM_C5)
		{
			WeaponBase*	pHitedWeapon = nullptr;

			UINT32		nCount = WEAPON::getSettableItemCount();

			for (UINT32 j = 0; j < nCount; j++)
			{
				pHitedWeapon = g_pWeaponManager->FindThrowWeaponByNetIndex(WEAPON::getSettableItemIndex(j), hitWho);
				if (pHitedWeapon)
					break;
			}

			if (i3::same_of<WeaponC5*>(pHitedWeapon))
			{
				MainWeapon_C5* mainWeapon_C5 = static_cast<MainWeapon_C5*>(pHitedWeapon->GetMainWeapon());
				mainWeapon_C5->Explosion_C5();
			}

			if (i3::same_of<WeaponClaymore*>(pHitedWeapon))
			{
				MainWeapon_Claymore* pMainWeapon = static_cast<MainWeapon_Claymore*>(pHitedWeapon->GetMainWeapon());
				pMainWeapon->Explosion();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	_ProcessRecvPacket_Chara_FireNHitData()
//	발사 및 히트에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_FireNHitData(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase* pChara)
{
	CHARA_DAMAGEINFO DamageInfo;
	VEC3D	vInvDir, vPos;
	INT32 hitCount = pRecvBuf->_tFireData._tHitData.getHitCount();

	if (hitCount > MAX_FIREHIT_COUNT)
	{
		hitCount = MAX_FIREHIT_COUNT;		// 문제가 있을 수 있습니다.
		pRecvBuf->_tFireData._tHitData.setHitCount(hitCount);
	}

	for (INT32 i = 0; i < hitCount; i++)
	{

		if (pRecvBuf->_tWeaponData.getWeaponID() == 0)
		{
			pRecvBuf->SetInfo_Fire(pRecvBuf->_tFireData._tHitData.getWeaponID(i),
				pRecvBuf->_tWeaponData.getWeaponExtension(), pRecvBuf->_tWeaponData.GetAttackType());
		}

#if defined (_SHUFFLE_HIT_PART)
		CHARA_HIT_PART hitPart = (CHARA_HIT_PART)HitPartContext::i()->DecryptHitPart(pRecvBuf->_tFireData._tHitData.getHitPart(i));
		//	I3ASSERT( hitPart > CHARA_HIT_UNKNOWN);
#else		
		CHARA_HIT_PART hitPart = (CHARA_HIT_PART)pRecvBuf->_tFireData._tHitData.getHitPart(i);
#endif
		UINT16 hitWho = pRecvBuf->_tFireData._tHitData.getHitWho(i);
		GAMEINFO_HIT_TYPE hitType = pRecvBuf->_tFireData._tHitData.getHitType(i);
		UINT8	HelmetFlag = pRecvBuf->_tFireData._tHitData.getHelmetFlag(i);
		CHARA_DEATH_TYPE deathType = pRecvBuf->_tFireData._tHitData.getDeathType(i);

		//// 무기를 확인합니다. pChara is the shooter
		if (pRecvBuf->_tWeaponData.getWeaponUsage() >= WEAPON_SLOT_COUNT)
			continue;

		WeaponBase * pWeapon = nullptr;
		if (pChara->IsDino())
			pWeapon = static_cast<CGameCharaDino*>(pChara)->GetDinoWeapon(pRecvBuf->_tWeaponData.getFireOrder());
		else
			pWeapon = pChara->GetCharaWeapon(pRecvBuf->_tWeaponData.getWeaponUsage());

		if (pWeapon == nullptr)		continue;

		// KEYWORD : WEAPON_DUALHANDGUN
		if (i3::same_of<WeaponDualHandGun*, WeaponCrossFire*>(pWeapon))
		{
			// 데미지 계산은 왼손 무기로..
			// UI 표시는 듀얼 무기로 처리해야 합니다.
			MainWeapon_Dual* mainWeapon_Dual = static_cast<MainWeapon_Dual*>(pWeapon->GetMainWeapon());

			WeaponBase * pLeftWeapon = mainWeapon_Dual->getLeftWeapon();
			if (pLeftWeapon->getWeaponInfo()->GetNumber() == pRecvBuf->_tWeaponData.getWeaponNumber())
				pWeapon = pLeftWeapon;
		}
		else if (i3::kind_of<WeaponDualSMG*>(pWeapon))
		{
			MainWeapon_DualSMG* mainWeapon_DualSMG = static_cast<MainWeapon_DualSMG*>(pWeapon->GetMainWeapon());


			WeaponBase * pLeftWeapon = mainWeapon_DualSMG->getLeftWeapon();
			if (pLeftWeapon->getWeaponInfo()->GetNumber() == pRecvBuf->_tWeaponData.getWeaponNumber())
				pWeapon = pLeftWeapon;
		}
		else if (i3::kind_of<WeaponDualCIC*>(pWeapon))
		{	// KEYWORD : WEAPON_CIC
			if (pRecvBuf->_tWeaponData.getFireOrder() == WEAPON::SECONDARY_FIRE)
			{
				MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pWeapon->GetMainWeapon());
				pWeapon = mainWeapon_DualCIC->getLeftWeapon();
			}
		}

		WEAPON_CLASS_TYPE weaponClass = (WEAPON_CLASS_TYPE)pRecvBuf->_tFireData._tHitData.getWeaponClass(i);
		WEAPON::RES_ID weaponNum = pRecvBuf->_tFireData._tHitData.getWeaponNumber(i);

		CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(weaponClass, weaponNum);
		if (pWeaponInfo == nullptr)
			continue;

		switch (hitType)
		{
		case GHT_AI:
		case GHT_CHARA:// 캐릭터 히트
		{
			if (hitPart == CHARA_HIT_UNKNOWN)			break;
			if (((hitWho >> 7) & 0x01) != 0)			break;

			INT32 nHitIndex = (hitWho & 0x0F);

			CGameCharaBase * pHitUser = g_pCharaManager->getCharaByNetIdx(nHitIndex, (hitType == GHT_AI));

			// 맞는것과 동시에 캐릭터가 나간경우는 예외 처리가 되야합니다.. - 정순구
			if (pHitUser == nullptr)		break;

			i3Vector::Sub(&vInvDir, pHitUser->GetPos(), pChara->GetPos());
			REAL32 _dist = i3Vector::Normalize(&vInvDir, &vInvDir);

			DamageInfo.Reset();

			pWeaponInfo->GetDamageInfo(&DamageInfo);
			DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);

			DamageInfo._FireOrder = pRecvBuf->_tWeaponData.getFireOrder();
			DamageInfo._rDamage = (REAL32)pRecvBuf->_tFireData._tHitData.getHitDamage(i);

			// GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_FireNHitData _rDamage %f \n", DamageInfo._rDamage);

			DamageInfo._pVecDir = &vInvDir;
			DamageInfo._IsOwner = pChara->isLocal();
			DamageInfo._nKillerIdx = pChara->getCharaNetIndex();

			DamageInfo._pHitResult = nullptr;

			DamageInfo._pWeapon = pWeapon;
			DamageInfo._pWeaponInfo = pWeaponInfo;

			// 월샷 데미지 감소량
			DamageInfo._WallShotReduceRate = 0;
			DamageInfo._pWallHitResult = nullptr;

			DamageInfo._HitType = hitType;
			DamageInfo.SetHitPart(hitPart);
			DamageInfo._rDistance = _dist;

			if ((pWeaponInfo->GetTypeClass() == WEAPON_CLASS_THROWING_GRENADE) ||
				(pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MISSION))
			{	//	폭탄등에 죽음
				DamageInfo._DeathType = CHARA_DEATH_B;
				DamageInfo._pVecPos = pWeapon->GetPos();
			}
			else
			{
				if (deathType == CHARA_DEATH_UN)
					deathType = CHARA_DEATH_A;

				DamageInfo.SetDeathTypeByHitPart(hitPart, deathType);

				i3Vector::Set(&vPos, getX(pChara->GetPos()), getY(pChara->GetPos()) + pChara->getCameraContext()->getIKHeightForCam(), getZ(pChara->GetPos()));
				DamageInfo._pVecPos = &vPos;
			}

			DamageInfo._IsNPC = false;

			I3ASSERT(DamageInfo.isValid());

			if (BattleServerContext::i()->IsP2PHost())
			{	// CS만 데미지 계산
				//	네트워크 데미지 정보 처리
				pHitUser->NET_HitForHost(&DamageInfo, DamageInfo._rDamage);

				// 방장은 Broadcast에 Helmet 정보를 담아야 한다.
				pRecvBuf->_tFireData._tHitData.setHelmetFlag(i, DamageInfo._HelmetFlag);
			}
			else
			{
				//	데미지 동작
				if (pHitUser->isCharaStateMask(CHARA_STATE_DEATH) == false)
				{
					if (DamageInfo._DeathType == CHARA_DEATH_H &&
						pHitUser->IsDino() == false)
					{
						if (HelmetFlag & 0x01)
						{
							pHitUser->DropEquip(DamageInfo._pVecDir);
							pHitUser->HelmetEffect(&DamageInfo);
						}

						if (HelmetFlag & 0x02)
						{
							if (BattleServerContext::i()->IsUseDedicationMode())
							{
								if (pChara->isLocal() == false)
									pHitUser->SetHelmetProtection(true);
							}
							else
								pHitUser->SetHelmetProtection(true);
						}
					}

					if (pChara->isLocal() && (hitPart != CHARA_HIT_HEAD))
					{// Effect만
						pHitUser->Cmd_HitEffect(&DamageInfo);
					}
					else
					{
						////Critical Damage
						if (pWeapon->GetWeaponDamage())
						{
							pWeapon->GetWeaponDamage()->SetCriticalDamageForClient(pRecvBuf->_tFireData._tHitData.getCriticalFlag(i));
							if (pWeapon->GetWeaponDamage()->IsCritical())
							{
								g_pSndMng->StartCharacterActionSnd(GTBCA_ON_HIT_CRITICAL, (UINT32)pHitUser->getDBInfo()->GetCharaSexTypeDISPLAY(), pHitUser->GetPos(), pHitUser->is1PV());
								I3TRACE("critical damage!!!!!!!!!!!!!!!!!!!!!!!\n");
							}
						}

						pHitUser->Cmd_Damage(&DamageInfo);
					}
				}
				else
				{
					// 미리 앞선에서 죽고 나서 Hit가 와도 Effect는 뿌려줘야 한다.
					pHitUser->Cmd_HitEffect(&DamageInfo);

					if (pHitUser->isLocal() &&
						!(DamageInfo._DeathType == CHARA_DEATH_BUFF) &&
						!(DamageInfo._pWeaponInfo == nullptr)
						)
					{
						INT32 filter_index = DamageInfo._pWeaponInfo->GetWeaponSpecialHitHudIndex();
						if (filter_index != -1 && UserInfoContext::i()->IsGM_Observer() == false)
						{
							CHud::i()->OnDamageFilter(filter_index);
						}
					}
				}
			}
		}
		break;
		case GHT_ITEM_C5:
		{
			WeaponBase*	pHitedWeapon = nullptr;

			UINT32		nCount = WEAPON::getSettableItemCount();

			for (UINT32 j = 0; j < nCount; j++)
			{
				pHitedWeapon = g_pWeaponManager->FindThrowWeaponByNetIndex(WEAPON::getSettableItemIndex(j), hitWho);
				if (pHitedWeapon)
					break;
			}

			if (i3::same_of<WeaponC5*>(pHitedWeapon))
			{
				MainWeapon_C5* mainWeapon_C5 = static_cast<MainWeapon_C5*>(pHitedWeapon->GetMainWeapon());
				mainWeapon_C5->Explosion_C5();
			}

			if (i3::same_of<WeaponClaymore*>(pHitedWeapon))
			{
				MainWeapon_Claymore* pMainWeapon = static_cast<MainWeapon_Claymore*>(pHitedWeapon->GetMainWeapon());
				pMainWeapon->Explosion();
			}
		}
		break;
		}
	}
}

//-------------------------------------------------------------------------------------------//
//	발사 및 히트에 대한 데이터를 처리합니다.
//-------------------------------------------------------------------------------------------//
void CGameUDPHandler::_ProcessRecvPacket_Chara_DeathDataForClient(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	VEC3D vInvDir;
	VEC3D vPos;

	// Check valid data
	if (pRecvBuf->_tSufferData.getDeathType() == CHARA_DEATH_UN)	return;
	if (pChara->isInvincible() == true) return; // revision 46073죽을 사람이 무적상태인지 확인 해서 무적상태면 죽는 패킷을 처리 안함.

												// P2P_INFO_FLAG_DOMI_DEATH(제압모드) 일 경우는 ShooterType 정보가 있습니다.
	DOMI_TYPE eShooterType = (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_DOMI_DEATH) ? DOMI_TYPE(pRecvBuf->_tSufferData.getShooterType()) : DOMI_TYPE_USER;
	UINT8 shooterIndex = pRecvBuf->_tSufferData.getShooterIndex();
	if (eShooterType == DOMI_TYPE_USER && shooterIndex >= SLOT_MAX_COUNT)	return;
	if (eShooterType == DOMI_TYPE_DINO && shooterIndex >= MAX_COUNT_DOMI_DINO)	return;

	// 쏜 유저입니다.
	CGameCharaBase * pKiller = nullptr;
	pKiller = g_pCharaManager->getCharaByNetIdx(shooterIndex, (eShooterType == DOMI_TYPE_DINO));

	if (pKiller == nullptr)
		return;


	CHARA_DAMAGEINFO DamageInfo;

	CWeaponInfo * pWeaponInfo = nullptr;
	WeaponBase* pWeapon = nullptr;

	DamageInfo._rDamage = 50.f;
	DamageInfo._FireOrder = WEAPON::PRIMARY_FIRE;

	pWeapon = pKiller->GetCurrentCharaWeapon();

	if (pWeapon != nullptr)
	{
		pWeaponInfo = pWeapon->getWeaponInfo();
		DamageInfo._FireOrder = pWeapon->getFireOrder();
		DamageInfo._rDamage = pWeapon->getAP();

		if (pRecvBuf->_iInfoFlag & P2P_INFO_FLAG_SUICIDEDAMAGE)
		{
			T_ItemID weaponItemID = pRecvBuf->_tSuicideDamageData.getWeaponID(0);
			pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(weaponItemID);
			DamageInfo._rDamage = (REAL32)pWeaponInfo->GetDamage();
		}
	}
	else
	{
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_KNIFE, WEAPON::getItemIndex(WEAPON::KNIFE_M7), true);
		DamageInfo._rDamage = (REAL32)pWeaponInfo->GetDamage();
	}

	CHARA_DEATH_TYPE deathType = (CHARA_DEATH_TYPE)pRecvBuf->_tSufferData.getDeathType();
	if ((deathType == CHARA_DEATH_B) ||
		(deathType == CHARA_DEATH_O))
	{
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_THROWING_GRENADE, WEAPON::getItemIndex(WEAPON::THROWING_K400), true);
		DamageInfo._rDamage = (REAL32)pWeaponInfo->GetDamage();

		DamageInfo._pVecPos = pKiller->GetPos();
		i3Vector::Scale(&vInvDir, const_cast<VEC3D*>(pRecvBuf->_tSufferData.getHitDirection()), -1.f);
	}
	else
	{
		i3Vector::Set(&vPos, getX(pKiller->GetPos()), getY(pKiller->GetPos()) + pKiller->getCameraContext()->getIKHeightForCam(), getZ(pKiller->GetPos()));
		DamageInfo._pVecPos = &vPos;
		i3Vector::Sub(&vInvDir, pChara->GetPos(), pKiller->GetPos());
	}

	DamageInfo._rDistance = i3Vector::Normalize(&vInvDir, &vInvDir);
	DamageInfo._pWeapon = pWeapon;
	DamageInfo._pWeaponInfo = pWeaponInfo;

	I3ASSERT(pWeaponInfo != nullptr);

	pWeaponInfo->GetDamageInfo(&DamageInfo);
	DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);

	DamageInfo._pVecDir = &vInvDir;
	DamageInfo._IsOwner = pKiller->isLocal();
	DamageInfo._nKillerIdx = pKiller->getCharaNetIndex();
	DamageInfo._nKillerType = pKiller->getCharaInfo()->GetType();

	DamageInfo._pHitResult = nullptr;
	DamageInfo._WallShotReduceRate = 0;
	DamageInfo._pWallHitResult = nullptr;
	DamageInfo._DeathType = deathType;


#if defined (_SHUFFLE_HIT_PART)
	DamageInfo.SetHitPart((CHARA_HIT_PART)HitPartContext::i()->DecryptHitPart(pRecvBuf->_tSufferData.getHitPart()));
	I3ASSERT(DamageInfo.GetHitPart() > CHARA_HIT_UNKNOWN);
#else
	DamageInfo.SetHitPart((CHARA_HIT_PART)pRecvBuf->_tSufferData.getHitPart());
#endif

	I3ASSERT(DamageInfo.isValid());

	// 캐릭터의 죽음에 대한 설정
	g_pCharaManager->DeathChara(pChara, pRecvBuf->_tSufferData.getShooterIndex(), &vInvDir,
		(CHARA_DEATH_TYPE)pRecvBuf->_tSufferData.getDeathType(), (CHARA_HIT_PART)DamageInfo.GetHitPart());

	//20150127(김기원: 수류탄으로 죽게되면 현재무기가 수류탄정보가아닌 주무기정보로 바뀌어와서 문제가 발생함
	//수류탄이나 object로 죽을떄는 hiteffect를 출력하지 않음)
	if ((deathType != CHARA_DEATH_B) && (deathType != CHARA_DEATH_O))
	{
		pChara->Cmd_HitEffect(&DamageInfo);
	}

	// 해당 캐릭터가 죽었음을 체크
	if (!pChara->isAIToClient())
		UserContext::i()->Death[pChara->getCharaNetIndex()] = true;
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_SufferDataForClient(GAMEINFO_CHARA_NETDATA * pRecvBuf, CGameCharaBase * pChara)
{
	VEC3D vShootPos, vDir;
	CHARA_DAMAGEINFO DamageInfo;
	WeaponBase* pWeapon = nullptr;

	if (pRecvBuf->_tSufferData.getShooterIndex() >= SLOT_MAX_COUNT)
		return;

#if defined (_SHUFFLE_HIT_PART)
	DamageInfo.SetHitPart((CHARA_HIT_PART)HitPartContext::i()->DecryptHitPart(pRecvBuf->_tSufferData.getHitPart()));
	//I3ASSERT( DamageInfo.GetHitPart() > CHARA_HIT_UNKNOWN);
#else
	DamageInfo.SetHitPart((CHARA_HIT_PART)pRecvBuf->_tSufferData.getHitPart());
#endif

	DamageInfo._pVecDir = nullptr;
	DamageInfo._pVecDir = const_cast<VEC3D*>(pRecvBuf->_tSufferData.getHitDirection());
	DamageInfo._nKillerIdx = pRecvBuf->_tSufferData.getShooterIndex();
	DamageInfo._DeathType = (CHARA_DEATH_TYPE)pRecvBuf->_tSufferData.getDeathType();

	CGameCharaBase * pShooter = g_pCharaManager->getCharaByNetIdx(pRecvBuf->_tSufferData.getShooterIndex());
	if (pShooter != nullptr)
	{
		if (((pRecvBuf->_tSufferData.getDeathType() == CHARA_DEATH_A) ||
			(pRecvBuf->_tSufferData.getDeathType() == CHARA_DEATH_H)))
		{
			// 쏜 사람이 자기 자신이고, 데미지 모션이 필요 없는 경우(수류탄, 미니액스, 탄도단검 외에)
			if (pShooter->isLocal() && pRecvBuf->_tSufferData.getNeedDamageMotion() == 0)
			{
				return;
			}
			else
			{
				// 일반적인 무기로 맞은 경우
				pWeapon = pShooter->GetCurrentCharaWeapon();
			}

			if ((pRecvBuf->_tSufferData.getDeathType() == CHARA_DEATH_H) && (pChara->IsDino() == false))
			{
				pChara->getEquipContext()->setHelmetProtect(true);
			}
		}

		i3Vector::Copy(&vShootPos, pShooter->GetPos());

		if ((pWeapon != nullptr) &&
			(pWeapon->getWeaponInfo() != nullptr))
		{
			DamageInfo.SetWeaponItemID(pWeapon);
			DamageInfo._pWeaponInfo = pWeapon->getWeaponInfo();
		}

		if (pShooter->IsSting() && pRecvBuf->_tSufferData.getDeathType() == CHARA_DEATH_POISON)
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_GRENADESHELL, WEAPON::getItemIndex(WEAPON::GSHELL_POISON), true);
			DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);
			DamageInfo._pWeaponInfo = pWeaponInfo;
		}
		if (pShooter->IsOnObject())
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(WEAPON_CLASS_OBJECT_GUN, WEAPON::getItemIndex(WEAPON::ON_OBJECT_M197), true);
			DamageInfo.SetWeaponItemID(pWeaponInfo, pWeaponInfo);
			DamageInfo._pWeaponInfo = pWeaponInfo;
		}

		i3Vector::Sub(&vDir, pChara->GetPos(), &vShootPos);
		i3Vector::Normalize(&vDir, &vDir);
		DamageInfo._pVecDir = &vDir;
	}
	DamageInfo._pVecPos = &vShootPos;

	pChara->Cmd_Damage(&DamageInfo);

	return;
}

void CGameUDPHandler::_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient(GAMEINFO_CHARA_NETDATA * pCharaNetData, GAMEINFO_CHARA_NETDATA * pRecvBuf)
{
	VEC3D	vPos, normal, vDir;

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	if (pMyChara == nullptr)
		return;

	if (isNetThrowWeapon(&pRecvBuf->_tWeaponData) == false)	return;
	if (pRecvBuf->_tFireData.isValidThrow(BattleSlotContext::i()->getMySlotIdx()) == false)				return;

	INT32 idx = pRecvBuf->_tFireData.getNetSlotIndex();
	const GAMEINFO_THROW_WEAPON * pThrowWeaponInfo = BattleObjContext::i()->getGameInfo_ThrowWeapon(idx);
	if (pThrowWeaponInfo->_pWeaponBase != nullptr)
	{
		//I3TRACE( "이미 사용중인 수류탄..\n");
		return;
	}

	pMyChara->getCameraContext()->getVirtualCamPosition(&vPos);
	i3Vector::Sub(&vPos, &vPos, pMyChara->GetPos());

	if (pCharaNetData->_tTakingObject.getOnLoadObject())
	{
		// local to world matrix
		if (pCharaNetData->_tTakingObject.getOnLoadObjectIndex() != 0xFFFF)
		{
			const GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->getGameInfo_Object(pCharaNetData->_tTakingObject.getOnLoadObjectIndex());
			if (pObjInfo->_pObjectRef != nullptr)
			{
				i3Object * pObject = pObjInfo->_pObjectRef;
				i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj(pObject->getSceneObject());
				if (pTrans != nullptr)
				{
					VEC3D vTemp;
					i3Vector::TransformCoord(&vTemp, pCharaNetData->_tPosRotData.getPos(), pTrans->GetCacheMatrix());
					pCharaNetData->_tPosRotData.setPos(&vTemp);
				}
			}
		}
	}

	i3Vector::Add(&vPos, &vPos, pCharaNetData->_tPosRotData.getPos());
	i3Vector::Sub(&vDir, pRecvBuf->_tFireData.getTargetPosition(), &vPos);
	i3Vector::Normalize(&normal, &vDir);

	WeaponBase * pWeapon = nullptr;

	if (pMyChara->IsSting())
		pWeapon = ((CGameCharaDino*)pMyChara)->GetDinoWeapon();
	else
		pWeapon = pMyChara->getWeaponContext()->getWeapon(m_AssistInfo[BattleSlotContext::i()->getMySlotIdx()]._nThrowSlotIdx);

	GlobalFunc::PB_TRACE("_ProcessRecvPacket_Chara_ThrowGrenadeForLocalClient end");
}

//---------------------------------------------------------------------------------------------//
// P2P Data's Receive and Send
//---------------------------------------------------------------------------------------------//
void CGameUDPHandler::P2PUpdate(REAL32 rDeltatime)
{
	I3_BOUNDCHK(BattleSlotContext::i()->getMySlotIdx(), USER_SLOT_COUNT);

	if (rDeltatime < 0.f)
	{
		I3PRINTLOG(I3LOG_FATAL, "time error!!!!! Please update CMOS or DirectX.");
	}

	BattleSyncTimer::i()->Update();

	//I3TRACE("GameElapsedTime : %f \n", BattleSyncTimer::i()->GetTimeElapsed());

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Recv 받은 데이터 파싱후 처리까지 완료 합니다.
	_P2PMergeRecvData(rDeltatime);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Send 해야 하는 패킷이 있는지 확인합니다.
	INT32 iStageState = gstatemgr::i()->getStageState();
	if ((iStageState == GAMESTAGE_STATE_SHOWSCORE) || (iStageState == GAMESTAGE_STATE_ENDGAME)
		|| (iStageState == GAMESTAGE_STATE_PRESTART_ROUND))// revision 49889 PreStartRound 시 UDP Packet Send 안되도록 처리
	{
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Send 확인하는 부분은 수정해야함.
#if defined( DEF_OSBERVER_MODE)
	// Observer가 아닌 경우에만 Send
	if (BattleSlotContext::i()->isObserverMe() != false)return;
#endif

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if (pMyChara != nullptr)
	{
		_CheckGameDataToSend(pMyChara);
	}

	if (BattleServerContext::i()->IsP2PHost())
	{//방장일때 처리
		_CheckGameDataToSendForAi();		// 훈련전 AI 캐릭터

		_CheckGameDataToSendForHost();
		_CalcPing_CS(rDeltatime);			// TCP로 게임서버에 UDP패킷의 Ping을 보낸다.
		UserContext::i()->SendDeathChara();	// TCP GameServer에 DeathEvent를 보냅니다.

		CMulticastSystem::i()->Broadcast(rDeltatime);	// 각 유저에게 패킷을 보낸다.
	}
	else
	{//일반 유저일때 처리

	 //패킷 만들기 내부에서 패킷을 보내요.. 나중에 수정합니다.
#if defined( DEF_OBSERVER_MODE)
	 // Observer가 아닌 경우에만 Send
		if (BattleSlotContext::i()->isObserverMe() == false)
#endif
		{
			UINT8 currentRound = ScoreContext::i()->getCurrentRound(BattleServerContext::i()->IsUseDedicationMode());

			UDPPacketSender::i()->WriteMyGameInfo(getGameInfo_Chara(BattleSlotContext::i()->getMySlotIdx()),
				BattleObjContext::i()->getGameInfo_ThrowWeapon(BattleSlotContext::i()->getMySlotIdx(), 0),
				BattleObjContext::i()->getGameInfo_DropedWeapon(0), rDeltatime, currentRound);
		}
	}


	//Dedication Server 일때 핑처리 
	if (BattleServerContext::i()->IsUseDedicationMode())
	{
		_CalcPing_DediCation(rDeltatime);
	}

	return;
}