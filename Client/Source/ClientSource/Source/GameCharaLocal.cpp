#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaLocal.h"
#include "StageBattle.h"
#include "GameCamera_FPS.h"

I3_CLASS_INSTANCE( CGameCharaLocal, CGameCharaPlayer );

CGameCharaLocal::CGameCharaLocal( void)
{
	OnInitVariable();
}

CGameCharaLocal::~CGameCharaLocal( void)
{	
}

void CGameCharaLocal::OnInitVariable( void)
{
	CGameCharaPlayer::OnInitVariable();

	//m_KeyPressInput			= 0;
	//m_KeyStrokeInput		= 0;

//	m_bDisarmingBomb		= FALSE;
//	m_bArmingBomb			= FALSE;

	//m_rWorldCollisionLength = 0.f;

#if defined( I3_DEBUG)
	m_pRenderableWorldCol		= NULL;
	m_pRenderableRoughHitCol	= NULL;
#endif
}

/*
BOOL CGameCharaLocal::_CrossCollisionToWorld( void )
{	
	VEC3D vTargetPos, vAt, vTemp;
	I3_PHYSIX_HIT_RESULT * pResult;

	m_rWorldCollisionLength	= 1000000.f;

	i3Vector::Scale( &vAt, i3Matrix::GetAt( g_pCamera->getCamMatrix()), -1000000.f );
	i3Vector::Add( &vTargetPos, &vAt, i3Matrix::GetPos( g_pCamera->getCamMatrix()));

	m_CameraLine.SetStart( i3Matrix::GetPos( g_pCamera->getCamMatrix()));
	m_CameraLine.SetEnd( &vTargetPos);
	
	pResult = g_pFramework->raycastClosest( &m_CameraLine, CGRP_ATTACK);
	if( pResult != NULL)
	{
		i3Vector::Sub( &vTemp, m_CameraLine.GetStart(), &pResult->m_Intersect );
		m_rWorldCollisionLength = i3Vector::Length( &vTemp);
		return TRUE;
	}

	return FALSE;
}

*/
void CGameCharaLocal::Create( i3GameResChara * pRes, CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo)
{
	CGameCharaPlayer::Create( pRes, pCharaInfo, pDBInfo);
	//	Add after here

	//setAnimSet( CHARA_ANIMSET_1PV);
	Change1PV();
	ChangeLocal();

	//i3BoundBox * pBoundBox = m_pBody->GetBound();
	//if( pBoundBox != NULL )
	//	m_pBody->SetBound( NULL );
}

void CGameCharaLocal::ResetChara( void)
{
	CGameCharaPlayer::ResetChara();

	//	키 초기화
//	m_KeyPressInput			= 0;
//	m_KeyStrokeInput		= 0;

//	m_bDisarmingBomb		= FALSE;
//	m_bArmingBomb			= FALSE;
	
	//EFFECT_Clear();					// 스크린 이펙트 클리어
}

void CGameCharaLocal::OnUpdate( REAL32 tm)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
	// 이하의 처리 순서는 매우 중요하고, 변경할 때는 숙고가 필요하다.

	//REAL_PROFILE( "CGameCharaLocal");  

	CGameCharaPlayer::OnUpdate( tm);

	

	//	Add code here
	/*
	if( !g_pGameContext->IsP2PActivate() || !g_pGameContext->IsBattle() )	return;

	INT32 nMyIdx = getCharaInfo()->_NetIdx;
	I3ASSERT( nMyIdx != -1);

	if( g_pCharaManager->getCharaTask( nMyIdx) == CHARA_TASK_RESPAWN)	//	조작 가능한 상태이면
	{
		i3Stage * pStage = g_pFramework->GetCurrentStage();
		if( ((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_BATTLE ||
			((CStageBattle*)pStage)->getStageState() == GAMESTAGE_STATE_ENDROUND )
		{
			//_ProcessOwner( tm);
			//_ProcessWeapon( tm);
			//_ProcessExtension( tm );
		}
		else
		{
			ACTION_ReturnToReady( TRUE);
		}
	}
	else			//	캐릭터가 죽었으면
	{	
		//_ProcessRespawn( tm);
	}

	//	사용자의 HUD 화면 이펙트 
//	_ProcessScreenEffect( tm);

	// 네트워크 데이터 저장
	//_SaveNetworkData();

	//if( g_pGameContext->IsP2PHost() )
	//{
	//	if( getCharaNetIndex() != -1 )
	//	{
	//		GAMEINFO_RECV * pGameInfo		= g_pGameContext->getRecvGameInfo( getCharaNetIndex() );
	//		pGameInfo->_tDefData._nHP		= (UINT8) m_pCharaInfo->_Hp;
	//	}
	//}
	*/
}

void CGameCharaLocal::DumpChara( void)
{
	CGameCharaPlayer::DumpChara();
}

/*
void CGameCharaLocal::_SetMoveDir( UINT64 key)
{
	if( key & GAME_KEY_MOVE_FORWARD )			m_MoveDir.z = 1.0f;		
	else if( key & GAME_KEY_MOVE_BACKWARD)		m_MoveDir.z = -1.0f;
	else										m_MoveDir.z = 0.0f;

	if( key & GAME_KEY_MOVE_LEFT )				m_MoveDir.x = 1.0f;		
	else if( key & GAME_KEY_MOVE_RIGHT )		m_MoveDir.x = -1.0f;
	else										m_MoveDir.x = 0.0f;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Process
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void CGameCharaLocal::ProcessFirst( REAL32 tm)
{	
	CGameCharaPlayer::ProcessFirst( tm);
	//	Add code here

	
	
	
	if( g_pFramework->getKeyStroke() & GAME_KEY_CMD_ALLDEATH )
	{
		if( g_pGameContext->IsP2PHost() )
		{
			VEC3D vec;
			for( INT32 i = 0; i < SLOT_MAX_COUNT; i++ )
			{
				CGameCharaPlayer * pChara = g_pCharaManager->getCharaByNetIdx( i );

				if( pChara != NULL && pChara != this )
				{
					if( pChara->getCharaTeam() != getCharaTeam() )
					{
						i3Vector::Sub( &vec, pChara->GetPos(), GetPos() );
						i3Vector::Normalize( &vec, &vec );
						
						g_pCharaManager->DeathChara( i, g_pGameContext->getMySlotIdx(), &vec, CHARA_DEATH_H, CHARA_HIT_HEAD );
						g_pGameContext->WriteCharaDeath( g_pGameContext->getMySlotIdx(), i, WEAPON_USAGE_MELEE, WEAPON_CLASS_KNIFE,
							WEAPON_NUM_KNIFE_M7, CHARA_DEATH_H, CHARA_HIT_HEAD, &vec );
					}
				}
			}
		}
	}
	
	{
		if( isStateMask( CHARA_STATE_FIRSTRESPAWN) )
		{
			// 무기 드랍으로 무기가 없으면 월드에 있는 무기들을 체크합니다.
			WEAPON_CheckTaking( tm);

			if( getKeyStrokeInput() & GAME_KEY_ACTION_DROP )
			{
				if( isAlive() )
					g_pGameContext->WriteMyDropWeapon( WEAPON_getCurrentSlotNum() );
			}

			if( getKeyStrokeInput() & GAME_KEY_ACTION_TURNBACK)
			{
				ACTION_TurnBack();
			}
		}

		//	키 입력( 본게임중에만 키입력을 받음)
		if(	!g_pCamera->IsTypeOf( CGameCamera_FLY::GetClassMeta()) && 
			!isEnableStateMask( CHARA_STATE_DISABLE_CONTROL) &&
			g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
		{
			m_KeyPressInput		= g_pFramework->getKeyPress();
			m_KeyStrokeInput	= g_pFramework->getKeyStroke();

			//REAL32 fl = 0.16666666f * tm;
			//	캐릭터 회전
			if( !g_pEnvSet->m_bEnableMouseInvert)
			{
				m_Rho		-= g_pFramework->getMouseMoveDelta()->y * 0.005f;
			}
			else
			{
				m_Rho		+= g_pFramework->getMouseMoveDelta()->y * 0.005f;
			}

			m_Theta		-= g_pFramework->getMouseMoveDelta()->x * 0.005f; 

			if( g_pFramework->getMouseMoveDelta()->x != 0.0f)
			{
				// 수평 반동의 회복 중, 마우스가 돌아가면 회복 모드를 Cancel한다.
				//if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
					m_RecoilData.m_HorzState = RECOIL_HORZ_NA;
			}
			
			m_Theta		= i3Math::NormalizeAngle( m_Theta);
			m_Rho		= i3Math::NormalizeAngle( m_Rho);
			//	각도

			REAL32 fAimingAngleMax = g_pCharaInfoDataBase->getGlobal()->m_rAimingAngleMax;	//	최대각
			REAL32 fAimingAngleMin = g_pCharaInfoDataBase->getGlobal()->m_rAimingAngleMin;	//	최소각

			//	각도 제한 (시작시 바라보는 시선의 수평선을 0도로 기준)
			if( m_Rho > I3_DEG2RAD( fAimingAngleMax))			m_Rho = I3_DEG2RAD(fAimingAngleMax);	
			else if( m_Rho < I3_DEG2RAD( fAimingAngleMin))		m_Rho = I3_DEG2RAD(fAimingAngleMin);	
		}
		else
		{
			m_KeyPressInput = m_KeyStrokeInput = 0;

			if( g_pFramework->getGameKeyMode() != GAMEKEY_MODE_MENU )
				MoveStop();	//	키 입력이 없으면 이동 정지
		}	

		//	이동 값을 처리합니다.
		ProcessMoveInput( tm);
	}
}

void CGameCharaLocal::_ProcessOwner( REAL32 tm)
{	//	자기자신에 필요한 처리를 합니다.
	//	경과 시간 체크
	//ProcessTimer( tm);	
}

void CGameCharaLocal::_ProcessWeapon( REAL32 tm)
{	
	UINT64 InputKeyStroke = getKeyStrokeInput();
	UINT64 InputKeyPress = getKeyPressInput();
	
	WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
	I3ASSERT( pCurWeapon != NULL);

	CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
	I3ASSERT( pCurWeaponInfo != NULL);

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	BOOL bCancelMission = FALSE;	//	미션 행동 취소 여부
	BOOL bCancelDisarm = FALSE;		//	폭탄 해체 취소 여부


	////////////////////////////////////////////////////////////	무기 발사
	if( InputKeyPress & GAME_KEY_ACTION_ATTACK )
	{	
		if( (!isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && !pCurWeapon->isStateMask( WSM_FIRING))
			&& (( pCurWeapon->getMagazineType() != WEAPON_DUAL_MAGAZINE_RIGHT && pCurWeapon->getLoadedBulletCount() > 0)
			|| (pCurWeapon->getMagazineType() == WEAPON_DUAL_MAGAZINE_RIGHT && pCurWeapon->getLoadedBulletDualCount() > 0 ))  )
		{	
			if( pCurWeapon->isFireEnabled() )
			{
				//	Mission 무기 예외처리
				if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION)
				{
					if( g_pGameContext->IsBombMissionMode() || !g_pConfig->m_bNetworking)
					{	//	행동
						if( ACTION_ActiveMission( pCurWeapon))
							bCancelMission = FALSE;
					}
				}
				else
				{	
					BOOL bFireEnable = TRUE;

					if( g_pGameContext->IsBombMissionMode())
					{
						if( isStateMask( CHARA_STATE_DISARMINGBOMB))	//	폭탄 해체 중일땐 발사 불가
							bFireEnable = FALSE;
					}

					if( bFireEnable)
					{
						//	무기 발사!!		(투척 무기만 발사준비)
						if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING)
							bCancelDisarm = ACTION_FireReady();					
						else
						{
							if (g_pFramework->IsBattleStage())
							{
								bCancelDisarm = ACTION_Fire();
							}
						}
					}
				}
			}
			else if( pCurWeapon->getWeaponInfo()->m_TypeTrigger == WEAPON_TRIGGER_RELOADING_SHOT )
			{	// 장전중 발사 가능하면 탄약 장전 동작을 취한다
				bCancelMission = ACTION_LoadBullet();
			}
		}
		else if( pCurWeapon->getState() == WEAPON_STATE_NEEDTO_LOADMAG)
		{
			bCancelMission = ACTION_LoadMagazine();		//	장전
		}
		else if( pCurWeapon->getState() == WEAPON_STATE_NEEDTO_LOADBULLET)
		{
			bCancelMission = ACTION_LoadBullet();		//	장전
		}
	}
	else if( (InputKeyPress & GAME_KEY_ACTION_ATTACK) == 0 )	//	발사 클릭후 버튼을 떼면 (Key Stroke 시점)
	{
		pCurWeapon->FireEnd();		//	발사 종료 시점을 무기에 알려준다.	

		bCancelMission = TRUE;  

		//	투척 무기일 경우 버튼을 놓으면 던진다.
		if( pCurWeaponInfo->m_TypeClass == WEAPON_CLASS_THROWING && pCurWeapon->isStateMask( WSM_FIRING) )
		{
			if( getActionContext()->isUpper( CHARA_UPPER_FIRE_READY) && !isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->getLoadedBulletCount() > 0)
			{
				bCancelDisarm = ACTION_Fire();
			}
		}
	}

	//총알이 모자라서 빈상태인경우 격발시도를 하게되면 나는 소리
	if( InputKeyStroke & GAME_KEY_ACTION_ATTACK )
	{
		if( pCurWeapon->getState() == WEAPON_STATE_EMPTY_MAGAZINE)
		{
			if( pCurWeaponInfo->m_TypeFire == WEAPON_FIRE_RANGE )
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_EMPTY_BULLET, i3Math::Rand()%2, GetPos(), TRUE);
			}
		}
	}

	////////////////////////////////////////////////////////////	보조 공격	
	if( InputKeyPress & GAME_KEY_ACTION_SUBFUNC )	//	<- Pressed 다
	{
		switch( pCurWeaponInfo->m_TypeClass)
		{
		//	현재 나이프만 가능
		case WEAPON_CLASS_KNIFE:
			if( (!isEnableStateMask( CHARA_STATE_DISABLE_SHOT) && pCurWeapon->isFireEnabled() && 
				!pCurWeapon->isStateMask( WSM_FIRING) && pCurWeapon->getLoadedBulletCount() > 0))	
			{
				BOOL bFireEnable = TRUE;

				if( g_pGameContext->IsBombMissionMode())
				{
					if( isStateMask( CHARA_STATE_DISARMINGBOMB))	//	폭탄 해체 중일땐 발사 불가
						bFireEnable = FALSE;
				}

				//	무기 발사!!	
				if( bFireEnable)
					bCancelDisarm = ACTION_Fire( 0, WEAPON_SECONDARY_FIRE);
			}
			break;
		}
		
	}

	////////////////////////////////////////////////////////////	리로드
	if( InputKeyStroke & GAME_KEY_ACTION_RELOAD )
	{
		if( pCurWeapon->isLoadMagazineEnabled() && !(InputKeyPress & GAME_KEY_ACTION_SPECIAL) )
		{
			bCancelMission = ACTION_LoadMagazine();
		}
	}

	////////////////////////////////////////////////////////////	무기 교체	
	CHud * pHud = ((CStageBattle*)g_pFramework->GetCurrentStage())->getHud();

	if( !isEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE) && !pHud->IsRadioChat() )
	{
		if(! g_pFramework->getKeyboard()->GetPressState( I3I_KEY_LALT ))
		{
			if( InputKeyStroke & GAME_KEY_CHANGEWEAPON1 )
			{
				bCancelMission = ACTION_SwapWeapon( 0);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON2 )
			{
				bCancelMission = ACTION_SwapWeapon( 1);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON3 )
			{
				bCancelMission = ACTION_SwapWeapon( 2);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON4 )
			{
				bCancelMission = ACTION_SwapWeapon( 3);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON5 )
			{
				bCancelMission = ACTION_SwapWeapon( 4);
			}
			else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON6 )
			{
				bCancelMission = ACTION_SwapWeapon( 5);
			}
		}

		//	예전 무기로 교체
		if ( InputKeyStroke & GAME_KEY_CHANGEWEAPON_QUICK )
		{
			bCancelMission = ACTION_SwapOldWeapon();
		}
		//	Mouse Wheel Down (다음 무기 교체)
		else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON_NEXT)
		{			
			bCancelMission = ACTION_SwapNextWeapon();
		}
		//	Mouse Wheel Up (이전 무기 교체)		
		else if( InputKeyStroke & GAME_KEY_CHANGEWEAPON_PRE)
		{			
			bCancelMission = ACTION_SwapPrevWeapon();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//	이 이후 특수 버튼이외에는 bCancelMission과 관련되어 다른 무기 버튼을 정의해선 안됩니다.
	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////	특수 버튼 (폭탄 해체등)
	if( InputKeyPress & GAME_KEY_ACTION_SPECIAL )
	{	
		if( !bCancelDisarm)
		{
			//	미션전 일 경우
			if( g_pGameContext->IsBombMissionMode() )
			{			
				//	미션 진행중
				if( (pMissionMng->getMissionState() == GAME_MISSION_STATE_INSTALLED ))
				{					
					ACTION_ActiveDisarm();		//	네트워크 폭탄 해체
				}
				//	미션 종료
				else if( isStateMask( CHARA_STATE_DISARMINGBOMB))
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
					removeStateMask( CHARA_STATE_DISARMINGBOMB);
				}
			}
		}
	}
	else if( (InputKeyPress & GAME_KEY_ACTION_SPECIAL) == 0)
	{	//	미션전 일 경우
		if( isStateMask( CHARA_STATE_DISARMINGBOMB))
		{
			if( g_pGameContext->IsBombMissionMode() )
			{
				bCancelDisarm = TRUE;
			}
		}
	} 

	////////////////////////////////////////////////////////////////////////
	//	미션중 행동 취소	(입력 행동에 영향을 받는다.)
	if( bCancelMission)
	{
		//	미션 모드
		ACTION_CancelMission();		//	행동 취소
	}
	
	//	폭탄 해체 취소	(입력 행동에 영향을 받는다.)	
	if( bCancelDisarm)
	{
		if( isStateMask( CHARA_STATE_DISARMINGBOMB))
		{
			ACTION_CancelDisarm();			//	네트워크 폭탄 해체 취소
		}
	}
}

*/

/*
void CGameCharaLocal::_ProcessScreenEffect( REAL32 tm)
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	섬광 이펙트 시간 계산 (섬광탄에서 사용)
	if( m_rFlashElapseTime != 0.f)
	{		
		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);

		CHud * pHud = pStage->getHud();

		if( m_rFlashElapseTime <= 0.f)
		{			
			i3Sprite2D * pSrc = pHud->GetScreenEffect();
			I3ASSERT( pSrc != NULL);

			INT32 FadeAlpha = (INT32) i3Color::GetA( pSrc->GetColor(0));		
			FadeAlpha -= (INT32) (tm * 255.0f);
			if( FadeAlpha < 0)	FadeAlpha = 0;

			//	이펙트가 끝나고 서서히 없어진다.
			if( FadeAlpha <= 0)
			{
				m_rFlashElapseTime = 0.0f;
				m_rFlashEffectiveTime = 0.0f;

				pHud->SetScreenEffectEnable( FALSE);
				pHud->SetScreenTex2DEnable( FALSE);

				return;	//	나가야 합니다.
			}
			else
			{
				pHud->SetScreenEffectEnable( TRUE);
				pHud->SetScreenTex2DEnable( 0, TRUE);
				pHud->SetScreenEffectColor( 255, 255, 255, (UINT8) FadeAlpha);			
				pHud->SetScreenTex2DColor( 0, 255, 255, 255, (UINT8) FadeAlpha);
			}
		}
		else	//	이펙트 중
		{
			REAL32 fMinReduceAlpha = 150.0f;
			REAL32 fMinAlpha = 255.0f - fMinReduceAlpha;
			REAL32 fGapTime = m_rFlashEffectiveTime - m_rFlashElapseTime;
			
			if( fGapTime <= 0.5f)		//	갑자기 하얀화면으로 페이드 된다.
			{
				INT32 a = (INT32) (fGapTime * 2.0f * 255.f);
				if( a > 255) a = 255;

				pHud->SetScreenEffectColor( 255, 255, 255, (UINT8) a);
			}	
			else if( fGapTime <= 1.0f)	//	하얀화면이 지속된다.
			{
				pHud->SetScreenEffectColor( 255, 255, 255, 255);
			}					 
			else						//	하얀화면이 울렁(?) 거린다.
			{			
				pHud->SetScreenEffectEnable( TRUE);

				REAL32 fAlpha = i3Math::abs( i3Math::sin( m_rFlashElapseTime)) * fMinReduceAlpha;
				UINT8 alpha = (UINT8) (255.f - fAlpha);
				if( alpha < (UINT8) fMinAlpha)		alpha = (UINT8) fMinAlpha;

				pHud->SetScreenEffectColor( 255, 255, 255, alpha);
			}

			pHud->SetScreenEffectEnable( TRUE);
			pHud->SetScreenTex2DEnable( 0, TRUE);
			pHud->SetScreenTex2DColor( 0, 255, 255, 255, 150);

			for( INT32 i = 1; i < SCR_BACKBUFFER_COUNT; i++)
			{	//	첫번째 스크린만 빼고 안나오게
				pHud->SetScreenTex2DEnable( i, FALSE);
			}
		}

		m_rFlashElapseTime -= tm;			
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//	데미지 스크린 이펙트 (수류탄에서 사용)
	if( m_rDamageElapseTime != 0.0f)
	{
		i3RasterizeChain_Blur * pRasterizeBlur = g_pFramework->GetRasterizeBlur();
		I3ASSERT( pRasterizeBlur != NULL);

		CStageBattle	* pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		I3ASSERT( pStage != NULL);
		CHud * pHud = pStage->getHud();

		m_rDamageElapseTime -= tm;

		if( pRasterizeBlur->getEnable())
		{	//	블러 효과(?)가 진행중
			if( m_rDamageElapseTime <= 0.0f)
			{	//	블러 효과(?)가 서서히 사라진다. (값이 Min 이하가 되면 자동적으로 내부에서 Disable된다)
				pRasterizeBlur->AddBlurAlpha( -0.02f);

				//	색상의 알파가 사라진다.
				I3COLOR * pBlurColor = pHud->GetScreenEffectColor();				
				INT32 a = (INT32) i3Color::GetA( pBlurColor);
				a -= 2;
				if( a > 255)	a = 255; 
				else if( a < 0) a = 0;
				i3Color::SetA( pBlurColor, (UINT8) a);

				pHud->SetScreenEffectEnable( TRUE);
				pHud->SetScreenEffectColor( pBlurColor);

				//I3TRACE( "블러가 사라짐..(%.4f)\n", pRasterizeBlur->GetBlurAlpha());
			}
			
			{
				//I3TRACE( "블러가 진행중..(%.4f)\n", pRasterizeBlur->GetBlurAlpha());
			}
		}
		else
		{	//	효과 종료
			pHud->SetScreenEffectEnable( FALSE);
			
			m_rDamageElapseTime = 0.0f;
		}

	}
}
*/

/*
BOOL CGameCharaLocal::ACTION_ActiveMission( WeaponBase * pWeapon)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

#if defined( I3_DEBUG)
	if( g_pConfig->m_bNetworking )
	{
#endif
	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치전이 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
		return FALSE;
#if defined( I3_DEBUG)
	}
#endif

	I3ASSERT( pWeapon != NULL);
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != NULL);
	
	CGameMissionManager * pMissionManager = g_pGameContext->getMissionManager();
	
	if( (pWeaponInfo->m_TypeClass == WEAPON_CLASS_MISSION) && (pWeaponInfo->m_nNumber == WEAPON_NUM_MISSION_C4 ) )
	{		
		if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
			(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
		{	
			switch( pMissionManager->getMissionState() )
			{
			case GAME_MISSION_STATE_INBATTLE :					//	미션에 폭탄이 설치되어 있지 않다면
				if( pMissionManager->IsEnableMy())
				{//	폭탄 설치가 가능하면
					//	이동 불가. 
					i3Vector::Zero( &m_MoveDir);

					if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
						ACTION_IdleLower();

					ACTION_Crouch();	// 설치시 강제로 앉는다.
					ACTION_Fire();		// 폭탄 설치!!

					addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
					addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

					m_bArmingBomb = TRUE;

					g_pGameContext->WriteMyInstallingMission( TRUE );

					return TRUE;
				}
				else
				{
					CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
					I3ASSERT( pBattle != NULL);
					pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// 지점에서만 설치할수 있심.
				}
				break;

			case GAME_MISSION_STATE_INSTALLED :					//	이미 설치 됐다면
				{
					removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				}
				
			default :
				{
				//	CStageBattle * pBattle = (CStageBattle*)g_pFramework->GetCurrentStage();
				//	I3ASSERT( pBattle != NULL);
				//	pBattle->getHud()->SetMissionNotice( STBL_IDX_BOMB_MISSION_NONAREA);			// 지점에서만 설치할수 있심.
				}
				
				break;
			}
		}
	}

	return FALSE;
}

BOOL CGameCharaLocal::ACTION_ActiveDisarm( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// 방어팀이 아닌경우
	{
		return FALSE;
	}
	
	//	폭탄 해체	
	if( (getActionContext()->isLower( CHARA_LOWER_IDLE) || getActionContext()->isLower( CHARA_LOWER_MOVE) || getActionContext()->isLower( CHARA_LOWER_WALK)) &&
		(getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE)))
	{	
		switch( pMissionMng->getMissionState() )
		{
		case GAME_MISSION_STATE_INSTALLED :					//	미션에 폭탄의 설치되어 있다면
			if( pMissionMng->IsEnableMy() && pMissionMng->IsEnableCrossHit() )
			{
				WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

				if( pWeapon != NULL)
				{
					I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
					WeaponC4 * pBomb = (WeaponC4 *) pWeapon;
					
					m_bDisarmingBomb = TRUE;

					pBomb->ActiveDisarm( TRUE);
				}
				else
				{
					m_bDisarmingBomb = FALSE;
				}
			}
			else
			{
				m_bDisarmingBomb = FALSE;
			}
			break;
		}
	}

	if( m_bDisarmingBomb)
	{
		//	이동 불가. 
		i3Vector::Zero( &m_MoveDir);

		if( !getActionContext()->isLower( CHARA_LOWER_IDLE))
			ACTION_IdleLower();

		if( !getActionContext()->isUpper( CHARA_UPPER_IDLE))
			ACTION_IdleUpper();

		ACTION_Crouch();	// 해체시 강제로 앉는다.

		addEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		addEnableStateMask( CHARA_STATE_DISABLE_MOVE);
		addEnableStateMask( CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( TRUE );		//	폭탄 해체

		return TRUE;
	}
	else
	{
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		g_pGameContext->WriteMyUninstallingMission( FALSE );

		//I3TRACE( "-- 폭탄 해체취소...\n");

		return FALSE;
	}
}

BOOL CGameCharaLocal::ACTION_CancelMission( void)
{
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INBATTLE) ||			// 설치 전이 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_OFFENSIVE) )		// 공격팀이 아닌경우
		return FALSE;
	
	if( pMissionMng->IsInstalling( g_pGameContext->getMySlotIdx() ) )
	{	//	자신이 폭탄 설치 중이면 취소
		g_pGameContext->WriteMyInstallingMission( FALSE );	//	폭탄 설치 취소
		
		removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

		//if( getActionContext()->isUpper( CHARA_UPPER_FIRE) || getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE))
			ACTION_ReturnToReady( TRUE);

		//I3TRACE("-- 폭탄 설치 취소!!\n");

		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaLocal::ACTION_CancelDisarm( void)
{	//	폭탄 해체 중이면
	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	if( !g_pGameContext->IsBombMissionMode() || //(pMissionMng->getMissionMode() != STAGE_TYPE_BLASTING) ||			// 폭파모드가 아닌경우
		(pMissionMng->getMissionState() != GAME_MISSION_STATE_INSTALLED) ||			// 설치 후가 아닌경우
		(pMissionMng->getMyMissionObject() != MY_MISSION_OBJECT_DEFENSIVE) )		// 방어팀이 아닌경우
		return FALSE;

	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	if( m_bDisarmingBomb)
	{
		//	자신이 폭탄 해제 중이면 취소
		//I3TRACE("isUnInstalling() : %d, WhoUnInstall() : %d\n", pMissionData->IsUninstalling( g_pGameContext->getMySlotIdx() ), pMissionData->WhoUninstall( nMyNetIdx));
		if( pMissionMng->IsUninstalling( mySlotIdx ))
		{			
			g_pGameContext->WriteMyUninstallingMission( FALSE );		//	폭탄 해체 취소

			removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);

			m_bDisarmingBomb = FALSE;

			WeaponBase * pWeapon = g_pWeaponManager->FindDropWeapon( NULL, WEAPON_CLASS_MISSION, WEAPON_NUM_MISSION_C4);

			if( pWeapon != NULL)
			{
				I3ASSERT( pWeapon->IsExactTypeOf( WeaponC4::GetClassMeta()));
				WeaponC4 * pBomb = ( WeaponC4*)pWeapon;

				pBomb->ActiveDisarm( FALSE);
			}

			//I3TRACE("-- 폭탄 해체 취소!!\n");

			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	HUD
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaLocal::ShowDamageHUD( VEC3D * pShooter, VEC3D * pHitUser, REAL32 fHitTheta )
{
	VEC3D	vDir;

	i3Vector::Sub( &vDir, pShooter, pHitUser );
	i3Vector::Normalize( &vDir, &vDir );
	
	REAL32 Rotation = atan2f( i3Vector::GetX( &vDir ), i3Vector::GetZ( &vDir ) ); // XZ 평면
	REAL32 rTargetAngle = i3Math::NormalizeAngle( Rotation );
	REAL32 fAngleGap = i3Math::NormalizeAngle( rTargetAngle - fHitTheta );		
	((CStageBattle*)(g_pFramework->GetCurrentStage()))->getHud()->SetDamageDirection( fAngleGap);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Network
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaLocal::_SaveNetworkData( void)
{
#ifdef I3_DEBUG
	if( g_pConfig->m_bNetworking == FALSE )	return;
#endif

	BOOL	bSaved = FALSE;
	GAMEINFO_SEND * pMyGameInfo = g_pGameContext->getSendGameInfo();

	if( pMyGameInfo->_nDecentSendCount > 0 )
	{
		bSaved = TRUE;
	}

	// 발사체에 대한 처리
	if( pMyGameInfo->_nFireSendCount > 0 )
	{	
		bSaved = TRUE;
	}

	// 액션에 대한 처리
	if( bSaved == FALSE && pMyGameInfo->_nMissionSendCount <= 0 )
	{	
		if( pMyGameInfo->_tDefData._Body != (UINT32) getActionContext()->getBody() )
			bSaved = TRUE;

		if( pMyGameInfo->_tDefData._Body2 != (UINT32) getActionContext()->getBody2() )
		{
			bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._Upper != (UINT32) getActionContext()->getUpper() )
		{
			if( !getActionContext()->isUpper( CHARA_UPPER_FIRE) &&
				!getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) &&
				!getActionContext()->isUpper(CHARA_UPPER_DAMAGE) &&	
				!getActionContext()->isUpper( CHARA_UPPER_MOVE) )
				bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._Lower != (UINT32) getActionContext()->getLower() )
		{
			if( !getActionContext()->isLower( CHARA_LOWER_DEATH) &&
				!getActionContext()->isLower( CHARA_LOWER_MOVE) &&
				!getActionContext()->isLower( CHARA_LOWER_WALK) )
				bSaved = TRUE;
		}

		if( pMyGameInfo->_tDefData._nFireAction == 1 )
			bSaved = TRUE;					// 발사 동작만 있는 경우

		if( pMyGameInfo->_tDefData._nRadio > 0 )
			bSaved = TRUE;

		if( pMyGameInfo->_tDefData._nWeaponSlot != getWeaponContext()->getCurrentSlotNum() ) 
			bSaved = TRUE; // must send packet for Weapon change

		if( bSaved )
			pMyGameInfo->_nActionSendCount	= 1;
	}

	pMyGameInfo->_tDefData._nWeaponSlot		= (UINT8) getWeaponContext()->getCurrentSlotNum();

	if( getWeaponContext()->getCurrent() != NULL )
	{
		pMyGameInfo->_tDefData._nWeaponExtension = (UINT8) getWeaponContext()->getCurrent()->isExtensionActivate();
	}
	
	g_pGameContext->WriteMyKey( getKeyPressInput() );

	if( !isStateMask( CHARA_STATE_ONOBJECT) )
	{
		g_pGameContext->WriteMyCharaPos( GetPos(), m_Theta, m_Rho, FALSE, 0 );
	}

	UINT32 nSecFire = 0;
	if( getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE ))
		nSecFire = 1;

	UINT32 nHelmet = HELMET_STATE_NONE;
	if( g_pGameContext->IsP2PHost() )
	{	
		if( EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) )
			nHelmet = HELMET_STATE_EQUIP;
	}

	UINT32	nBody2;
	if( isStateMask( CHARA_STATE_ZOOM ) )
		nBody2 = CHARA_BODY_ZOOM;
	else
		nBody2 = CHARA_BODY_ATTACKIDLE;

	g_pGameContext->WriteMyCharaAction( getActionContext()->getBody(), nBody2, getActionContext()->getUpper(), getActionContext()->getLower(), nSecFire, nHelmet );
}
*/

// 자신의 네트워크 정보 처리입니다.
// Network Client에서 Damage 및 Death 상태 체크
/*
void CGameCharaLocal::ProcessNetworkMime( void)
{	// UDP 데이터를 처리합니다.
	if( g_pGameContext->IsP2PHost() )		return;

	INT32	idx = g_pGameContext->getMySlotIdx();
	GAMEINFO_RECV * pGameInfo	= g_pGameContext->getRecvGameInfo( idx );

	if( !pGameInfo->_bRecvFlag )			return;

	GAMEINFO_HIT *	pHitInfo	= &pGameInfo->_tHitData;
	GAMEINFO_FIRE * pFireInfo	= &pGameInfo->_tFireData;

	switch( pGameInfo->_RecvType )
	{
	case CHARA_PACKETTYPE_DEATH :
		if( (pGameInfo->_tDefData._nHP == 0) &&
			!getActionContext()->isLower( CHARA_LOWER_DEATH) &&
			pHitInfo->_nDeathType != CHARA_DEATH_UN )
		{	// 자신이 죽은 경우
			if( pHitInfo->_nDeathType == CHARA_DEATH_B || pHitInfo->_nDeathType == CHARA_DEATH_O )
				i3Vector::Scale( &pHitInfo->_vHitDir, &pHitInfo->_vHitDir, -1.f );

			g_pCharaManager->DeathChara( idx, pHitInfo->_nShooter,
				&pHitInfo->_vHitDir, (CHARA_DEATH_TYPE) pHitInfo->_nDeathType, (CHARA_HIT_PART) pHitInfo->_nHitpart );
			
			g_pGameContext->setUserDeath( idx, TRUE );
			pGameInfo->_tHitData.Reset();
		}
		break;

	case CHARA_PACKETTYPE_HIT :
		// 여기는 수류탄 및 오브젝트 파괴에 의한 데미지만 들어옵니다.
		if( (pHitInfo->_nHitpart != CHARA_HIT_UNKNOWN) &&
			m_pCharaInfo->_Hp != pGameInfo->_tDefData._nHP )
		{	// 자신이 맞은 경우
			if( EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) && ((CHARA_HIT_PART) pHitInfo->_nHitpart == CHARA_HIT_HEAD 
				|| (pGameInfo->_tDefData._nHelmet == HELMET_STATE_DROP)))
			{	// 머리에 맞았어요.. 헬멧 드랍
				EQUIP_DropToWorld( EQUIP_CLASS_TYPE_HELMET, &pHitInfo->_vHitDir );
				//	헬멧에 맞아 불꽃이 튄다.
				g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, WEAPON_CLASS_THROWING,
					i3Matrix::GetPos( getHeadMatrix()), &pHitInfo->_vHitDir, !isNetwork());
			}
			
			ACTION_Damage( (CHARA_HIT_PART) pHitInfo->_nHitpart, &pHitInfo->_vHitDir );
			// 비명 소리
			PlayCharaDamageSound((CHARA_HIT_PART) pHitInfo->_nHitpart, TRUE);
			
			g_pSndMng->StartWeaponStateSnd( GTBWS_ON_HIT_THROW, 0, GetPos(), true);

			pGameInfo->_tHitData.Reset();
		}
		break;

	case CHARA_PACKETTYPE_FIRENHIT :
		// 자신이 맞춘 상대입니다.
		VEC3D vDir;
		for( INT32 i = 0; i < pFireInfo->_FireHitCount; i++ )
		{
			CGameCharaPlayer * pHitUser = g_pCharaManager->getCharaByNetIdx( pFireInfo->_FireHitWho[ i ] );

			if( pHitUser != NULL)
			{
				i3Vector::Sub( &pHitInfo->_vHitDir, pHitUser->GetPos(), GetPos() );
				i3Vector::Normalize( &pHitInfo->_vHitDir, &pHitInfo->_vHitDir );

				i3Vector::Scale( &vDir, &pHitInfo->_vHitDir, -1.f );

				// 헬멧 장비 체크
				if( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ] == CHARA_HIT_HEAD &&
					pHitUser->EQUIP_IsAttachedOnChara( EQUIP_CLASS_TYPE_HELMET ) )
				{
					pHitUser->EQUIP_DropToWorld( EQUIP_CLASS_TYPE_HELMET, &vDir );

					//	헬멧에 맞아 불꽃이 튄다.
					if( getWeaponContext()->getCurrent() != NULL )
					{
						CWeaponInfo * pWeaponInfo = getWeaponContext()->getCurrent()->getWeaponInfo();
						g_pEffectManager->AddEquipHitEffect( EQUIP_CLASS_TYPE_HELMET, pWeaponInfo->m_TypeClass,
							i3Matrix::GetPos( pHitUser->getHeadMatrix()), &pGameInfo->_tHitData._vHitDir, pHitUser->is1PV());
					}
				}
				
				pHitUser->ACTION_Damage( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ], &pHitInfo->_vHitDir );
				//	데미지 동작을 안할때도 비명 소리는 나야 한다.
				pHitUser->PlayCharaDamageSound( (CHARA_HIT_PART) pFireInfo->_FireHitpart[ i ] , TRUE);
			}
		}

		pFireInfo->_FireHitCount	= 0;
		break;
	}

	// 수류탄
	GAMEINFO_GRENADE * pMyGameObj = &g_pGameContext->getSendGameInfo()->_tGrenadeData;
	if( pMyGameObj->_nThrowState == NET_GRENADE_THROW_SEND )
	{	// 아직 패킷을 못 받은 상태 계속 보내줍니다.
		if( getActionContext()->isUpper( CHARA_UPPER_FIRE ) )
		{
			g_pGameContext->getSendGameInfo()->_nFireSendCount = 1;
		}
	}
	else if( pMyGameObj->_nThrowState == NET_GRENADE_THROW_RECV )
	{ // 자신의 것은 던져줍니다.
		VEC3D	vPos, vDir, normal;
		GetVirtualCamPosition( &vPos );
		i3Vector::Sub( &vPos, &vPos, GetPos());

		if( pGameInfo->_tDefData._bOnLoadObject )
		{
			// local to world matrix
			if( g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetCount() > pGameInfo->_tDefData._nOnLoadObjectIdx )
			{
				i3Object * pObject = (i3Object*) g_pGameContext->getObjectCountInfo()->m_pAnimObjectList->GetItem( pGameInfo->_tDefData._nOnLoadObjectIdx );
				i3Transform2 * pTrans = (i3Transform2*) i3Scene::FindNodeByExactType( pObject->GetSceneObject(), i3Transform2::GetClassMeta() );

				i3Vector::TransformCoord( &pFireInfo->_vStartPos, &pFireInfo->_vStartPos, pTrans->GetCacheMatrix() );
			}
		}
		
		i3Vector::Add( &vPos, &vPos, &pFireInfo->_vStartPos);
		i3Vector::Sub( &vDir, &pFireInfo->_vTargetPos, &vPos );
		i3Vector::Normalize( &normal, &vDir );
		
		MATRIX mat;
		i3Matrix::Identity( &mat);
		i3Matrix::SetRotateXDeg( &mat, 60.0f);
		i3Vector::TransformCoord( &vDir, &normal, &mat);
		
		i3Vector::Add( &vPos, &vPos, &vDir );
		WeaponBase * pWeapon = WEAPON_get( g_pGameContext->getSendGameInfo()->_tGrenadeData._nSlotIdx);
		I3ASSERT( pWeapon != NULL);
		pWeapon->NET_FirstThrow( &vPos, &vDir );
		g_pGameContext->getSendGameInfo()->_tGrenadeData._nThrowState	= NET_GRENADE_THROW_DONE;
	}

	{//Hit된경우 이동속도 느려지는 효과
		if( m_pCharaInfo->_Hp > pGameInfo->_tDefData._nHP )
		{
			EnableSpeedReduction( (UINT8)m_pCharaInfo->_Hp - pGameInfo->_tDefData._nHP);
		}
	}
	
	m_pCharaInfo->_Hp		= pGameInfo->_tDefData._nHP;
	pGameInfo->_bRecvFlag	= FALSE;
	pGameInfo->_RecvType	= CHARA_PACKETTYPE_NONE;
}

*/
