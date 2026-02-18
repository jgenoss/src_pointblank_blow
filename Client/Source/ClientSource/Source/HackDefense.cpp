#include "pch.h"
#include "HackDefense.h"
#include "CoreFileChecker.h"
#include "GameStateMgr.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "Weapon/WeaponItemID.h"

// #include "MagnetCheck.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define NETWORK_TIME_SYNC	12.0f	//서버에게 현재 자신에게 남아있는 fBattleTime을 보냅니다.
#define NETWORK_SPEED_TIME_SYNC	1.0f	//서버에게 현재 자신에게 남아있는 fBattleTime을 보냅니다.

extern CCharaGlobal2*	g_pGlobalInfo;			// global chara info
extern CCharaGlobal2*	g_pGlobalDinoInfo;			// global Dino info
extern i3::vector<CCharaInfo2*>	g_CharaInfoList;		// Character info
extern UINT32			g_GroupMask[32];


#include "GameCharaWeaponContext.h"
#include "GameCharaMoveContext.h"

bool CheckHackingInGameP2PDef();
bool CheckHankingInRecord();

static CGameCharaBase * getPlayer( void)
{
	return g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
}

namespace
{
	const REAL32 DEFAULT_RESPAWN_TIME		= 5.f;
	const REAL32 DEFAULT_INVINCIBLE_TIME	= 4.f;
}

namespace
{
	bool IsSame(REAL32 rTemp1, REAL32 rTemp2)
	{
		REAL32 rTemp = rTemp1 - rTemp2;

		rTemp = i3Math::abs( rTemp );

		if(rTemp != 0.f) {
			return false;
		}

		return true;
	}

	inline
	BOOL IsSame( VEC4D * pV1, VEC4D * pV2)
	{
		return	(pV1->vec[0] == pV2->vec[0]) &&
			(pV1->vec[1] == pV2->vec[1]) &&
			(pV1->vec[2] == pV2->vec[2]) &&
			(pV1->vec[3] == pV2->vec[3]);
	}

	bool CheckCharaInfo(const CCharaInfo* pCharaInfoOrg, CCharaInfo2* pCharaInfo)
	{
		if(pCharaInfoOrg == nullptr) {
			return false;
		}

		if(pCharaInfo == nullptr) {
			return false;
		}

		if (pCharaInfoOrg->GetHP() != pCharaInfo->GetHP()) {
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetJumpHeight(), pCharaInfo->GetJumpHeight()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetJumpPenaltyTime(), pCharaInfo->GetJumpPenaltyTime()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetJumpPenaltyRate(), pCharaInfo->GetJumpPenaltyRate()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetWeaponSwap(), pCharaInfo->GetWeaponSwap()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetScopeWaveVelocity(), pCharaInfo->GetScopeWaveVelocity()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetScopeWaveRange(), pCharaInfo->GetScopeWaveRange()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetReloadSpeed(), pCharaInfo->GetReloadSpeed()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetShoulderSpeed(), pCharaInfo->GetShoulderSpeed()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetAccelRateBackVsFront(), pCharaInfo->GetAccelRateBackVsFront()) == false)
		{
			return false;
		}


		for (INT32 i = 0; i < CHARA::eDAMAGEPART_COUNT; i++)
		{
			if (IsSame(pCharaInfoOrg->GetPartDamageRate((CHARA::eDAMAGEPART) i), pCharaInfo->GetPartDamageRate((CHARA::eDAMAGEPART) i)) == false)
			{
				return false;
			}
		}

		if (IsSame(pCharaInfoOrg->GetNPCAttackDelay(), pCharaInfo->GetNPCAttackDelay()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetNPCInvincibleTime(), pCharaInfo->GetNPCInvincibleTime()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetNPCRespawnTime(), pCharaInfo->GetNPCRespawnTime()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetNPCSearchRandomValue(), pCharaInfo->GetNPCSearchRandomValue()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetClunchJumpHeight(), pCharaInfo->GetClunchJumpHeight()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetClunchJumpCheckTime(), pCharaInfo->GetClunchJumpCheckTime()) == false)
		{
			return false;
		}

		VEC3D vDB, vChara;

		pCharaInfoOrg->GetFPSCamPos(&vDB);
		pCharaInfo->GetFPSCamPos(&vChara);

		if (i3Vector::IsSame(&vDB, &vChara) == false)
		{
			return false;
		}

		VEC4D vDBIK, vCharaIK;

		pCharaInfoOrg->GetCamIKHeight(&vDBIK);
		pCharaInfo->GetCamIKHeight(&vCharaIK);

		if (IsSame(&vDBIK, &vCharaIK) == false)
		{
			return false;
		}

		if (IsSame(pCharaInfoOrg->GetJumpDelay(), pCharaInfo->GetJumpDelay()) == false)
		{
			return false;
		}
		if (IsSame(pCharaInfoOrg->GetAccelerationTime(), pCharaInfo->GetAccelerationTime()) == false)
		{
			return false;
		}
		for (INT32 x = MMT_NORMAL; x < MMT_COUNT; x++){
			for (INT32 i = MST_WALK; i < MST_COUNT; i++) {
				for (INT32 j = MDT_FRONT; j < MDT_COUNT; j++) {
					if (IsSame(pCharaInfoOrg->GetMoveSpeed((EMoveSpeedType)i, (EMoveDirectionType)j, (EMoveModeType)x), pCharaInfo->GetMoveSpeed((EMoveSpeedType)i, (EMoveDirectionType)j, (EMoveModeType)x)) == false)
					{
						return false;
					}
				}

			}
		}

		for(INT32 i=CHPS_BODY; i<CHPS_COUNT; i++) {
			const CSpeedReduction* pTemp1 = pCharaInfoOrg->GetSpeedReduction((CHARA_HIT_PART_SIMPLE)i);
			const CSpeedReduction* pTemp2 = pCharaInfo->GetSpeedReduction((CHARA_HIT_PART_SIMPLE)i);

			if(IsSame(pTemp1->GetRate(), pTemp2->GetRate()) == false) 
			{
				return false;
			}
			if(IsSame(pTemp1->GetTime(), pTemp2->GetTime()) == false) 
			{
				return false;
			}

		}

		return true;
	}


	bool CheckGlobalInfo()
	{
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetInvincibleTime(), g_pGlobalInfo->GetInvincibleTime()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetInvincibleTime_4vs4(), g_pGlobalInfo->GetInvincibleTime_4vs4()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMax(), g_pGlobalInfo->GetAimingAngleMax()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetAimingAngleMin(), g_pGlobalInfo->GetAimingAngleMin()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetSpeedReductionTime(), g_pGlobalInfo->GetSpeedReductionTime()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetSpeedReductionMaxRate(), g_pGlobalInfo->GetSpeedReductionMaxRate()) == false) 
		{
			return false;
		}

		if(g_pCharaInfoDataBase->getGlobal()->GetFallMinDamage() != g_pGlobalInfo->GetFallMinDamage()) 
		{
			return false;
		}

		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetFallDamageMinHeight(), g_pGlobalInfo->GetFallDamageMinHeight()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getGlobal()->GetFallDamageSlope(), g_pGlobalInfo->GetFallDamageSlope()) == false) 
		{
			return false;
		}

		/// Dino
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetInvincibleTime(), g_pGlobalDinoInfo->GetInvincibleTime()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetInvincibleTime_4vs4(), g_pGlobalDinoInfo->GetInvincibleTime_4vs4()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetAimingAngleMax(), g_pGlobalDinoInfo->GetAimingAngleMax()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetAimingAngleMin(), g_pGlobalDinoInfo->GetAimingAngleMin()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetSpeedReductionTime(), g_pGlobalDinoInfo->GetSpeedReductionTime()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetSpeedReductionMaxRate(), g_pGlobalDinoInfo->GetSpeedReductionMaxRate()) == false) 
		{
			return false;
		}

		if(g_pCharaInfoDataBase->getDinoGlobal()->GetFallMinDamage() != g_pGlobalDinoInfo->GetFallMinDamage()) 
		{
			return false;
		}

		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetFallDamageMinHeight(), g_pGlobalDinoInfo->GetFallDamageMinHeight()) == false) 
		{
			return false;
		}
		if(IsSame(g_pCharaInfoDataBase->getDinoGlobal()->GetFallDamageSlope(), g_pGlobalDinoInfo->GetFallDamageSlope()) == false) 
		{
			return false;
		}

		return true;
	}
	
	bool CheckPlayerCharaInfo()
	{
		CCharaInfo2* pCharaInfo = nullptr;
		CGameCharaBase * pPlayer = getPlayer();
		
		if (pPlayer == nullptr)
			return true;

		const size_t cnt = g_CharaInfoList.size();
		for( size_t i = 0; i < cnt; ++i)
		{
			CCharaInfo2 * pInfo = g_CharaInfoList[i];
			I3ASSERT( pInfo != nullptr);

			if( pInfo->GetITEMID() == (INT32)pPlayer->getDBInfo()->GetITEMID() )
				pCharaInfo = pInfo;
		}

		{
			//
			// 해킹방지 코드
			// CGameCharaManager::AttachCharaToStage에서 캐릭터 리소스를 얻기위해
			// GLOBAL_CHARA_INFO의 ResType으로 QueryCharaResource를 호출하게 되는데
			// 로딩때만 코드변조로 캐릭터ID를 변조하면 캐릭터가 변견된다.
			// 이 것을 감지하기 위해서 내 캐릭터 리소스가 일치하는 지 확인하는 작업을 합니다.
			// 리소스와 매칭되는 TypeNumber을 얻어내고 현재 캐릭터 TypeNumber를 비교합니다.
			// 로딩시에만 TypeNumber를 변경하게 되면 아래 코드로 감지할 수 있습니다.
			
			
			CHARA_RES_ID Type = g_pCharaManager->GetCharaResType((const i3GameResChara *) pPlayer->getSgRes());
			CHARA_RES_ID PlayerType = (CHARA_RES_ID) pPlayer->getDBInfo()->GetResID();
			//동일한 리소스를 재사용하는 공룡들이 있어서 예외처리함

			switch( PlayerType)
			{
			case CHARA_RES_ID_DINO_CC_RAPTOR :
			/*case CHARA_RES_ID_DINO_RAPTOR_BLUE_STAIN :
			case CHARA_RES_ID_DINO_RAPTOR_GREEN_STRIPES :*/
				{
					PlayerType = CHARA_RES_ID_DINO_RAPTOR;
					break;
				}
			case CHARA_RES_ID_DINO_CC_ACID :
				{
					PlayerType = CHARA_RES_ID_DINO_ACID;
					break;
				}
			case CHARA_RES_ID_DINO_CC_STING :
				PlayerType = CHARA_RES_ID_DINO_STING;
				break;
			}

			if (PlayerType != Type)
			{
				return false;
			}

		}

		if( CheckCharaInfo(pPlayer->getDBInfo(), pCharaInfo) == false)
		{
			return false;
		}

		return CheckGlobalInfo();
	}

	bool CheckAllCharaInfo()
	{
		INT32 cnt = g_pCharaInfoDataBase->GetInfoCount( EQUIP::ePART_CHARA);

		for( INT32 i = 0; i < cnt; ++i)
		{
			const CCharaInfo * pInfo = (const CCharaInfo*) g_pCharaInfoDataBase->GetInfo( EQUIP::ePART_CHARA, i);
			CCharaInfo2 * pInfo2 = nullptr;
			I3ASSERT( pInfo != nullptr);

			const size_t cnt2 = g_CharaInfoList.size();
			for( size_t j = 0; j < cnt2; ++j)
			{
				pInfo2 = g_CharaInfoList[j];
				I3ASSERT( pInfo != nullptr);

				if( pInfo2->GetITEMID() == (INT32)pInfo->GetITEMID())
					break;
			}

			if( CheckCharaInfo(pInfo, pInfo2) == false)
			{
				return false;
			}
		}

		return CheckGlobalInfo();
	}

	bool CheckGroupMask()
	{

		for(INT32 i=0; i<32; i++) {
			if(g_GroupMask[i] != g_pFramework->GetViewer()->GetPhysixContext()->getGroupMask(i)) {
				return false;
			}
		}

		return true;
	}

	bool CheckAllWeaponInfo()
	{
		for( INT32 nWeaponType = 0; nWeaponType < WEAPON_CLASS_COUNT; nWeaponType++)
		{
			INT32 nWeaponNumMax = WEAPON::getItemCount( (WEAPON_CLASS_TYPE) nWeaponType);

			//	Weapon Num
			for( INT32 nWeaponNum = 1; nWeaponNum < nWeaponNumMax; nWeaponNum++)
			{
				CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( (WEAPON_CLASS_TYPE) nWeaponType, nWeaponNum);
				if( pWeaponInfo == nullptr)
					continue;
			}
		}

		return true;
	}

}




namespace hack_defense
{
	void	check_observer::ProcessCreateStage()
	{
		i3mem::FillZero(&m_roomInfo, sizeof(m_roomInfo));
	}

	void	check_observer::init_roominfo()
	{
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&m_roomInfo);

	}

	void	check_observer::check()
	{
		ROOM_INFO_ADD	roomAddInfo;
		MyRoomInfoContext::i()->getMyRoomInfoAdd(&roomAddInfo);
		if ( 0 != memcmp(&m_roomInfo, &roomAddInfo, sizeof(m_roomInfo)) )
		{
			MyRoomInfoContext::i()->setMyRoomInfoAdd(&m_roomInfo);	// 원래 값으로 복구 시켜주기
			HackContext::i()->HackingDetected(257.5f,0);		// 해킹 처리하기. (임의의 마법수가 들어가 있다.)
		}
	}

	bool			CheckMemDataPerTick( CHEAT_CONTEXT& CheatContext )
	{
		return true;
	}

	bool CheckMemDataPerNETWORK_TIME_SYNC( CHEAT_CONTEXT& CheatContext ,CHEAT_CLIENT& CheatClient)
	{
		CGameCharaBase * pPlayer = getPlayer();

		if( pPlayer != nullptr) 
		{
			if( pPlayer->getDBInfo() != nullptr) 
			{
				WeaponBase * pWeapon = pPlayer->GetCurrentCharaWeapon();
				if( pWeapon != nullptr)
				{
					CWeaponInfo* pWeaponInfo = pWeapon->getWeaponInfo();

					if( pPlayer->getMoveContext() != nullptr )
					{
						//스페이스 모드는 속도 제한값을 매직넘버 10.0f 로 판단할 수 없다.
						//서버에서 체크하므로 이렇게 처리합니다.
						if(!MyRoomInfoContext::i()->getStage()->IsSpace() && 
							pPlayer->getMoveContext()->getMaxSpeed() > 10.f)
						{	
							CheatContext = CC_MAX_SPEED;
							CheatClient  = C_MAX_SPEED;
							return false;
						}
						//GlobalFunc::PB_TRACE("rMoveSpeed %f", g_pPlayer->getMoveContext()->getMaxSpeed());
					}

					if(pWeaponInfo->GetNeedToLoadBullet() == 0) 
					{
						if(pWeaponInfo->GetTypeClass() == WEAPON_CLASS_SNIPER)
						{
							WEAPON::RES_ID _Number = pWeaponInfo->GetNumber();

							if( _Number == GetItemIndex("SNIPER_SSG69")					|| _Number == GetItemIndex("SNIPER_SSG69_CAMO")  ||
								_Number == GetItemIndex("SNIPER_SSG69_SILVER")			|| _Number == GetItemIndex("SNIPER_L115A1")		 || 
								_Number == GetItemIndex("SNIPER_L115A1_S_EXP_PT_20")	|| _Number == GetItemIndex("SNIPER_L115A1_GOLD") ||
								_Number == GetItemIndex("SNIPER_L115A1_D") )
							{
								CheatContext = CC_NEED_TO_LOAD_BULLET_SNIPER;
								CheatClient =	C_NEED_TO_LOAD_BULLET_SNIPER;

								return false;
							}
						}
					}

					if( i3::same_of<WeaponKrissSuperV*>(pWeapon))
					{
						WeaponBase* pDaulWeapon = ((WeaponKrissSuperV*)pWeapon)->getDualWeapon();
						if(pDaulWeapon->getLoadedBulletCount() > 60) {
							CheatContext = CC_DAUL_KRISS_SUPPER_V;
							CheatClient = C_DAUL_KRISS_SUPPER_V;
							return false;
						}
						if(pDaulWeapon->getWeaponInfo()->GetFireDecBulletCount() <= 0) {
							CheatContext = CC_DAUL_KRISS_SUPPER_V;
							CheatClient =	C_DAUL_KRISS_SUPPER_V;
							return false;
						}
					}

					if( i3::same_of<WeaponRocketLauncher*>(pWeapon))
					{
						if(pWeapon->getLoadedBulletCount() > 1) {
							CheatContext = CC_LOAD_BULLET_COUNT_ROCKET;
							CheatClient = C_NEED_TO_LOAD_BULLET_SNIPER;
							return false;
						}
					}

					if( i3::same_of<WeaponK400*>(pWeapon))
					{
						if(pWeapon->getLoadedBulletCount() > 3) {
							CheatContext = CC_LOAD_BULLET_COUNT_K400;
							CheatClient = C_NEED_TO_LOAD_BULLET_SNIPER;
							return false;
						}
					}

					if(pWeaponInfo->GetTypeClass() == WEAPON_CLASS_MG) 
					{
						if(pWeapon->getLoadedBulletCount() > 200) {
							CheatContext = CC_LOAD_BULLET_COUNT_MG;
							CheatClient = C_LOAD_BULLET_COUNT_MG;
							return false;
						}
					}
					else 
					{
						if(pWeapon->getLoadedBulletCount() > 100) {	// revision 51869 듀얼우지 골드 한탄창당 90발로 기존 80발 제한 코드에 탐지되어 100발제한으로 상향
							CheatContext = CC_LOAD_BULLET_COUNT_OTHERS;
							CheatClient = C_LOAD_BULLET_COUNT_OTHERS;
							return false;
						}
					}

					if(pWeaponInfo->GetFireDecBulletCount() <= 0) 
					{
						CheatContext = CC_FIRE_BULLET_COUNT;
						CheatClient = C_FIRE_BULLET_COUNT;
						return false;
					}

				}

				/*if(CheckAllWeaponInfo() == false) 
				{
				rMoveSpeed = 200;
				GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed);
				}*/
				/*if(CheckAllCharaInfo() == false) 
				{
				rMoveSpeed = 200;
				GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed);
				}*/

				// 1.5 캐릭터 리소스 핵체크 기능 현재 작동하지 않으므로 일단 주석처리로 막습니다.
				//if(CheckPlayerCharaInfo() == false) 
				//{
				//	CheatContext = CC_INVALID_CHARA;
				//	CheatClient  = C_INVALID_CHARA;
				//	return false;
				//}

				if(CheckGroupMask() == false) 
				{
					CheatContext = CC_PHYSIX_MASK;
					CheatClient  = C_PHYSIX_MASK;
					return false;
				}
			}
		}

		return true;
	}

	void	check_UnknwonPos::check(i3CollideeLine * pLine)
	{	// 이 기능은 문제가 생길 수 있어서 쓰지 않습니다.
		//STAGE_ID StageID = MyRoomInfoContext::i()->getStageID();
		//CGameCharaBase * pPlayer = getPlayer();

		//if (pPlayer!= nullptr)
		//{
		//	if (g_pFramework->IsBattleStage() && pPlayer->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx() && pPlayer->isCharaStateMask( CHARA_STATE_DEATH) == false
		//		&& StageID != STAGE_ID_TWOTOWERS && !pPlayer->getMoveContext()->getRideObject()) 	
		//	{
		//		//스테이지 정보를 가져옵니다.
		//		I3_PHYSIX_HIT_RESULT * pWorld = g_pFramework->raycastClosest( pLine, (UINT32) CGRP_MASKGROUP_ONLYWORLDHIT );

		//		if (pWorld == nullptr)
		//		{
		//			HackContext::i()->HackingDetected(CC_UNKNOWN_POS,C_UNKNOWN_POS);
		//		}
		//	}

		//}
	
	}

	void	check_PhysixDll::check()
	{
		unsigned char md5[16] = {};
		UINT128 ui128Temp1;
		char strMD5[33] = {};
		md5_file(".\\NxCharacter.2.8.1.dll", md5);
		memcpy(&ui128Temp1, md5, sizeof(char)*16);
		i3MD5::GetMD5String(strMD5, ui128Temp1, sizeof(strMD5));
		if (i3::generic_is_iequal("DBE167B6822D00CA906CAC90B3F8C9F6",strMD5) == false)
		{
			HackContext::i()->HackingDetected(CC_FILE_FALSIFY,C_FILE_FALSIFY);
		}

	}

	void	check_speedhack::init_pos()
	{
		if (getPlayer())
			i3Vector::Copy( &m_vPos, getPlayer()->GetPos());
	}


	void	check_speedhack::ProcessCreateStage()
	{
		i3Vector::Zero( &m_vPos);
		m_bFirstSpeedCheck = true;
	}

	void	check_speedhack::checkSpeedPos()		//speed hack//
	{
		//공룡일 경우 일단 체크 하지 않습니다.
		CGameCharaBase * pPlayer = getPlayer();
		
		if(  pPlayer == nullptr || pPlayer->IsDino() )
		{
			return;
		}

		VEC3D vDir;


		//Y축은 거리에 고려하지 않기 위하여 강제로 0으로 세팅합니다.
		VEC3D v;
		i3Vector::Copy( &v, pPlayer->GetPos());
		v.y = 0.f;
		m_vPos.y = 0.f;

		i3Vector::Sub( &vDir,  &v, &m_vPos);
		REAL32 rLen = i3Vector::Length( &vDir);

		//10미터 이상 차이가 나면 스피드 핵으로 자릅니다.
		if(!MyRoomInfoContext::i()->getStage()->IsSpace() && rLen > 10.f)
		{
			REAL32 rMoveSpeed = 1.f;
			UINT8  nType = 0;
			UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;
			rMoveSpeed = (REAL32)(CC_MAX_SPEED_POS);
			nType = (UINT8)(C_MAX_SPEED_POS);

			GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed, &nType);
		}
		
		m_bFirstSpeedCheck = false;
		check_speedhack::init_pos();

	
	}

	void	check_speedhack::checkMaxSpeed( REAL32 fSpeed)		//speed hack//
	{

		CGameCharaBase * pPlayer = getPlayer();
		if (pPlayer != nullptr)
		{

			if (pPlayer->getCharaNetIndex()   == BattleSlotContext::i()->getMySlotIdx())
			{
				if (!MyRoomInfoContext::i()->getStage()->IsSpace() && !pPlayer->isAI()&& !pPlayer->IsDino() && fSpeed > 7.1f)
				{
					//서버에게 현재 남은게임타임을 전달
					REAL32 rMoveSpeed=0.f;
					UINT8  nType=0;
					rMoveSpeed = (REAL32)(CC_MAX_SPEED);
					nType = (UINT32)(C_MAX_SPEED);

#if !defined( I3_DEBUG )
					UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;
					GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed,&nType);
#endif
				}
			}


			}

			
		}


	void	check_mapdata::ProcessCreateStage()
	{
		m_IsStageDataSafe = true;
	}

	void	check_mapdata::check(INT32	nStageIndex)
	{
		m_IsStageDataSafe = true;

		if ( g_pConfigEx->GetGame().CheckMapData )
		{
			i3StageInfo * pInfo = g_pFramework->getStageInfo( nStageIndex);
			if (pInfo == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "i3StageInfo를 찾지 못했습니다.");
				m_IsStageDataSafe = false;
				return;
			}

			TCHAR	_CurPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, _CurPath);
			tstring	curPath(_CurPath);
			tstring	str;
			str += _T(".\\Pack\\");
			str += pInfo->GetName();
			str += _T("*.*");

			CFileFind	ff;
			BOOL rv = ff.FindFile(str.c_str());
			vector<tstring>	checkingfiles;
			while (rv)
			{
				rv = ff.FindNextFile();

				tstring	path(ff.GetFilePath());
				path.erase(0, curPath.size()+1);

				checkingfiles.push_back(tstring(path));
			}


			CCoreFileChecker	checker;
			if ( checker.Open() )
			{
				if ( checkingfiles.empty() )
				{
					I3PRINTLOG(I3LOG_FATAL, "맵데이터 변조를 확인 할 파일이 없습니다.");
				}

				for ( size_t i = 0 ; i < checkingfiles.size() ; ++i )
				{
					int result = checker.Check(checkingfiles[i]);
					if ( result != 0 )
					{
						// 에러 통보하기.
						m_IsStageDataSafe = false;
					}
				}
			}
		}
	}
	
	void	check_elapsedsynctimer::ProcessCreateStage()
	{
		m_fElapsedTimerCheck = 0.f;
		m_fElapsedSpeedTimerCheck = 0.f;
		m_fElapsedGravityTimerCheck = 0.f;
	}

	void	check_elapsedsynctimer::Update(REAL32 rDeltaSeconds)
	{
		if( gstatemgr::i()->getStageState() == GAMESTAGE_STATE_BATTLE)
		{
			REAL32 rMoveSpeed = 1.f;
			UINT32  nType = 0;
			CHEAT_CONTEXT Cheat_Context = CC_NON_CHEATING;
			CHEAT_CLIENT Cheat_Client = C_NON_CHEATING;
			static bool s_bSendInvalidMoveSpeed = false;

			if( !s_bSendInvalidMoveSpeed )
			{
				if( hd::CheckMemDataPerTick( Cheat_Context ) == false )
				{
					m_fElapsedTimerCheck += NETWORK_TIME_SYNC;
					s_bSendInvalidMoveSpeed = true;
				}
			}

			//CGameCharaBase * pPlayer = getPlayer();
			/*
			if (!pPlayer->isCharaStateMask(CHARA_STATE_FLOOR) &&  !pPlayer->isCharaStateMask(CHARA_STATE_DEATH))
			{
				m_fElapsedGravityTimerCheck+= rDeltaSeconds;	
				//5초이상 캐릭터의 상태가 CHARA_STATE_FLOOR상태가 유지 되지 않을 경우 중력핵으로 의심합니다.
				if (m_fElapsedGravityTimerCheck > 5.f)
				{
					m_fElapsedTimerCheck += NETWORK_TIME_SYNC;
					s_bSendInvalidMoveSpeed = true;
					Cheat_Context = CC_GRAVITY_HACK;
					Cheat_Client =  C_GRAVITY_HACK;
					m_fElapsedGravityTimerCheck =0.f;
				}
			}
			else
			{
				m_fElapsedGravityTimerCheck = 0.f;

			}
			

			if ( pPlayer->getMoveContext()->getRideObject() || pPlayer->isCharaStateMask(CHARA_STATE_DEATH) ||hd::check_speedhack::i()->getFirstRespawn() )
			{
				//InitCheckDataPos함수는 m_vPos에 g_pPlayer->GetPos()를 카피하는 함수입니다.
				hd::check_speedhack::i()->init_pos();
			}

			m_fElapsedSpeedTimerCheck += rDeltaSeconds;	
			if (m_fElapsedSpeedTimerCheck > NETWORK_SPEED_TIME_SYNC )
			{
				if( !s_bSendInvalidMoveSpeed )
				{
					//hd::check_speedhack::i()->checkSpeedPos();

				}
				m_fElapsedSpeedTimerCheck =0.f;

			}
			*/

			m_fElapsedTimerCheck += rDeltaSeconds;	
			if( m_fElapsedTimerCheck > NETWORK_TIME_SYNC)
			{
				m_fElapsedTimerCheck -= NETWORK_TIME_SYNC;

				//서버에게 현재 남은게임타임을 전달
				UINT32 nRemainBattleTime = (UINT32)UserContext::i()->fRemainingBattleTime;

				if( !s_bSendInvalidMoveSpeed )
					hd::CheckMemDataPerNETWORK_TIME_SYNC( Cheat_Context ,Cheat_Client);

				rMoveSpeed = (REAL32)(Cheat_Context);
				nType = (UINT32)(Cheat_Client);

				GameEventSender::i()->SetEvent( EVENT_BATTLE_REMAINTIME_N, &nRemainBattleTime, &rMoveSpeed,&nType);
			}

		}
				
	}

//	void	check_elapsedsynctimer::InitMagnetStartTime()
//	{
//		m_MagnetCheck.InitStartTime();
//	}

//	void	check_elapsedsynctimer::InitMagnet()
//	{
//		m_MagnetCheck.Init( g_pCharaManager->getCharaList() );
//	}



}
