#include "pch.h"
#include "BattleHUD_TargetHP.h"

#include "BattleHUD.h"
#include "ConfigRenderBattle.h"
#include "GameCharaBoneContext.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "Weapon/WeaponM197.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "ChannelContext.h"
#include "GameMissionManager.h"

#define	WARN_OBJECT_DEFENCE	0.3f
#define DETECT_ENEMY_UPDATE_TIME	3.f

I3_CLASS_INSTANCE( CHUD_TargetHP);//, i3GameObjBase);

namespace inner { CHUD_TargetHP* this_ptr = 0; }
CHUD_TargetHP* CHUD_TargetHP::instance() { return inner::this_ptr; }

CHUD_TargetHP::CHUD_TargetHP(void) : m_pSpriteDefence(nullptr)
{
	inner::this_ptr = this;

	for(INT32 i = 0; i < 2; ++i)
	{
		i3Vector::Zero( &m_vDestroyObjectCenterPos[i]);
	}

	for(INT32 i = 0; i < 2; ++i)
	{
		m_bUnderAttackWarn[ i]				= false;
		m_rUnderAttackWarnElapsed[ i]		= 0.0f;
		m_rUnderAttackWarnElapsedTotal[ i]	= 0.0f;
	}

	for(INT32 i = 0 ; i < MAX_TEAM_USER; ++i)
	{
		m_DEUI[i].Init();
		m_DEUI_Weak[i].Init();
		m_DEUI_Weak[i + MAX_TEAM_USER].Init();
	}

	m_pHUD = nullptr;
	m_pSprite = nullptr;
	m_pTex = nullptr;

	m_pSpriteDetectedEnemy			= nullptr;
	m_pSpriteWeakedEnemy			= nullptr;
	m_pTextureHUD3					= nullptr;

	m_OldHP_RED = m_OldHP_BLUE = 10000.0f;
	m_bInitObjectHP = false;
	m_bInitObjectSynchronize = false;
	m_bPlayBGMforDefence = false;
}

CHUD_TargetHP::~CHUD_TargetHP(void)
{
	I3_SAFE_RELEASE(m_pTextureHUD3);			// 없어서 릭발생(2013.11.11.수빈)
	I3_SAFE_RELEASE( m_pTex);
	I3_SAFE_RELEASE( m_pSpriteDefence);
	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE(m_pSpriteDetectedEnemy);
	I3_SAFE_NODE_RELEASE(m_pSpriteWeakedEnemy);
}


bool CHUD_TargetHP::Create( CHud * pHUD)
{
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();
	
	INT32 nPosX = 0;
	INT32 nPosY = 0;
	REAL32 fClanOffsetY = 0.0f;
	i3Node * pRoot;

	m_pHUD = pHUD;
	pRoot = pHUD->getHUDRoot();

	if( ChannelContext::i()->GetBattleChannel())
	{
		fClanOffsetY = 18.0f;
	}

	// HUD 기본 Texture
	m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
	{
		CreateWeakedEnemySprite();
		return true;
	}

	nPosX = (INT32)(nScreenWidth * 0.5f);
	nPosY = (INT32)(nScreenHeight * 0.5f);

	m_pSprite = i3Sprite2D::new_object();	
	I3ASSERT( m_pSprite != nullptr);

	m_pSprite->SetTexture( m_pTex );
	m_pSprite->Create( HUD_TARGETHP_SPR_HPSTATE_COUNT, true, true);

	m_pSprite->SetEnable( HUD_TARGETHP_SPR_OBJECTICON_RED, true);
	m_pSprite->SetColor( HUD_TARGETHP_SPR_OBJECTICON_RED, 255,255,255,255);
	m_pSprite->SetEnable( HUD_TARGETHP_SPR_OBJECTICON_BLUE, true);
	m_pSprite->SetColor( HUD_TARGETHP_SPR_OBJECTICON_BLUE, 255,255,255,255);

	m_pTextureHUD3 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);

	m_pSpriteDefence = i3Sprite2D::new_object();	
	I3ASSERT( m_pSpriteDefence != nullptr);
	m_pSpriteDefence->SetTexture( m_pTextureHUD3 );
	m_pSpriteDefence->Create( HUD_TARGETHP_SPR_HPSTATE_COUNT, true, true);
	m_pSpriteDefence->SetEnable(false);

	//목표물 표시 UI( 적군의 오브젝트 위치 표시)
	
	switch (MyRoomInfoContext::i()->getStage()->GetStageUID())
	{
	case STAGE_UID_BREAKDOWN:
	{
		m_pSprite->SetEnable(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, false);
		m_pSprite->SetRect(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, (REAL32)(nPosX - 144.0f), (150.0f), (50.0f), (53.0f));
		m_pSprite->SetColor(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, 255, 255, 255, 255);
		m_pSprite->SetTextureCoord(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, 950.0f, 65.0f, 1000.0f, 118.0f);

		m_pSprite->SetEnable(HUD_TARGETHP_SPR_OBJECT_IMG_RED, false);
		m_pSprite->SetRect(HUD_TARGETHP_SPR_OBJECT_IMG_RED, (REAL32)(nPosX + 144.0f), (150.0f), (50.0f), (53.0f));
		m_pSprite->SetColor(HUD_TARGETHP_SPR_OBJECT_IMG_RED, 255, 255, 255, 255);
		m_pSprite->SetTextureCoord(HUD_TARGETHP_SPR_OBJECT_IMG_RED, 950.0f, 2.0f, 1000.0f, 55.0f);
	}
	break;
	case STAGE_UID_HELISPOT:
	{
		m_pSprite->SetEnable(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, false);
		m_pSprite->SetRect(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, (REAL32)(nPosX + 144.0f), (150.0f), (50.0f), (53.0f));
		m_pSprite->SetColor(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, 255, 255, 255, 255);
		m_pSprite->SetTextureCoord(HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, 950.0f, 2.0f, 1000.0f, 55.0f);

		m_pSprite->SetEnable(HUD_TARGETHP_SPR_OBJECT_IMG_RED, false);
		m_pSprite->SetRect(HUD_TARGETHP_SPR_OBJECT_IMG_RED, (REAL32)(nPosX + 144.0f), (150.0f), (50.0f), (53.0f));
		m_pSprite->SetColor(HUD_TARGETHP_SPR_OBJECT_IMG_RED, 255, 255, 255, 255);
		m_pSprite->SetTextureCoord(HUD_TARGETHP_SPR_OBJECT_IMG_RED, 950.0f, 2.0f, 1000.0f, 55.0f);
	}
	break;
	case STAGE_UID_GIRAN:
	case STAGE_UID_GIRAN2:
		break;
	default:
		I3PRINTLOG(I3LOG_WARN, "오브젝트 사용 모드에서 HUD가 생성되지 않았습니다.");
		break;
	}
	
	pRoot->AddChild(m_pSprite);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY) )
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
		
		i3Object * pRedObj = pMissionMng->getRedObject();
		i3BoundBox Box;
		VEC3D vDiff;
		
		if( pRedObj != nullptr)
		{
			pRedObj->getSceneObject()->GetWrappingBound( &Box);
			i3Vector::Add( &vDiff, Box.GetMax(), Box.GetMin());
			i3Vector::Scale( &vDiff, &vDiff, 0.5f);
			i3Vector::Sub( &m_vDestroyObjectCenterPos[0], &vDiff, pRedObj->GetPos());
		}
		else
		{
			i3Vector::Zero( &m_vDestroyObjectCenterPos[0]);
		}

		i3Object * pBlueObj = pMissionMng->getBlueObject();		
		if( pBlueObj != nullptr)
		{
			Box.Reset();
			pBlueObj->getSceneObject()->GetWrappingBound( &Box);
			i3Vector::Add( &vDiff, Box.GetMax(), Box.GetMin());
			i3Vector::Scale( &vDiff, &vDiff, 0.5f);

			i3Vector::Sub( &m_vDestroyObjectCenterPos[1], &vDiff, pBlueObj->GetPos());
		}
		else
		{
			i3Vector::Zero( &m_vDestroyObjectCenterPos[1]);
		}
	}
	else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE) )
	{
		CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
		
		i3Object * pDefObj0 = pMissionMng->getDefenceObj( 0);
		i3BoundBox Box;
		VEC3D vDiff;
		
		if( pDefObj0 != nullptr)
		{
			pDefObj0->getSceneObject()->GetWrappingBound( &Box);
			i3Vector::Add( &vDiff, Box.GetMax(), Box.GetMin());
			i3Vector::Scale( &vDiff, &vDiff, 0.5f);
			i3Vector::Sub( &m_vDestroyObjectCenterPos[0], &vDiff, pDefObj0->GetPos());
		}
		else
		{
			i3Vector::Zero( &m_vDestroyObjectCenterPos[0]);
		}

		i3Object * pDefObj1 = pMissionMng->getDefenceObj( 1);
		if( pDefObj1 != nullptr)
		{
			Box.Reset();
			pDefObj1->getSceneObject()->GetWrappingBound( &Box);
			i3Vector::Add( &vDiff, Box.GetMax(), Box.GetMin());
			i3Vector::Scale( &vDiff, &vDiff, 0.5f);

			i3Vector::Sub( &m_vDestroyObjectCenterPos[1], &vDiff, pDefObj1->GetPos());
		}
		else
		{
			i3Vector::Zero( &m_vDestroyObjectCenterPos[1]);
		}
	}

	CreateDetectedEnemySprite();

	return true;
}

void CHUD_TargetHP::Init(void)
{
	m_OldHP_RED = m_OldHP_BLUE = 10000.0f;
}

void CHUD_TargetHP::Reset( void)
{
	if( m_bPlayBGMforDefence)
	{
		g_pSndMng->StopBGM( 1.f);
		m_bPlayBGMforDefence = false;
	}

	// 기본 Icon
	if( m_pSpriteDefence != nullptr )
	{
		m_pSpriteDefence->SetTextureCoord( HUD_TARGETHP_SPR_OBJECTICON_RED, 700.0f, 66.0f, 735.0f, 86.0f);
		m_pSpriteDefence->SetTextureCoord( HUD_TARGETHP_SPR_OBJECTICON_BLUE, 700.0f, 86.0f, 735.0f, 106.0f);
	}
}


void CHUD_TargetHP::SetEnableTargetHP( bool bEnable)
{
	if( m_pSprite != nullptr)
	{
		m_pSprite->SetEnable( bEnable);

		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY))
		{
			if( bEnable)
			{
				m_pSprite->SetEnable( HUD_TARGETHP_SPR_OBJECT_IMG_RED, false);
				m_pSprite->SetEnable( HUD_TARGETHP_SPR_OBJECT_IMG_BLUE, false);
			}
		}
	}
}

void CHUD_TargetHP::SetEnableWithoutNotice( bool bFlag, GAME_GUI_STATUS nState, bool bDeath)
{
}

void CHUD_TargetHP::_UpdateTargetHP_DestroyMission( REAL32 tm)
{
	REAL32 fClanOffsetY = 0.0f;
	REAL32 RedHP, BlueHP;

	I3ASSERT( m_pSprite != nullptr);

	if( ChannelContext::i()->GetBattleChannel())
		fClanOffsetY = 18.0f;

	_UpdateUnderAttack( tm);

	_UpdateTargetPos();

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32 nPosX = (INT32)(nScreenWidth * 0.5f);

	CGameMissionManager * pMissionMng = CGameMissionManager::i(); 

	i3Object * pBlueObj = pMissionMng->getBlueObject();
	if( pBlueObj == nullptr)
		return;

	i3Object * pRedObj = pMissionMng->getRedObject();
	if( pRedObj == nullptr)
		return;

	if( !m_bInitObjectHP )
	{
		m_OldHP_RED = pRedObj->getCurrentHP();
		m_OldHP_BLUE = pBlueObj->getCurrentHP();

		m_bInitObjectHP = true;
	}

	RedHP		= pRedObj->getCurrentHP();
	BlueHP		= pBlueObj->getCurrentHP();

	bool _bWarningFlag = true;

	if( (m_OldHP_RED != RedHP ) ||  ( m_OldHP_BLUE != BlueHP ) )
	{
		if( !m_bInitObjectSynchronize )
		{
			m_bInitObjectSynchronize = true;
			_bWarningFlag = false;
		}
	}

	REAL32 hpBarSize	= 99.0f;

	STAGE_ID	StageUID		= MyRoomInfoContext::i()->getStage()->GetStageUID();

	if( m_OldHP_RED != RedHP)
	{
		REAL32 hpRatioRed		= RedHP / pRedObj->getMaxHP();
		REAL32 hpBarSizeRed		= hpBarSize * hpRatioRed;
		REAL32 hpBarPosXRed	= nPosX - 144.0f + ( hpBarSize - hpBarSizeRed);

		m_pSprite->SetRect( HUD_TARGETHP_SPR_HPBAR_RED, (REAL32)hpBarPosXRed, (53.0f)+fClanOffsetY, hpBarSizeRed, (10.0f));
		m_pSprite->SetEnable( HUD_TARGETHP_SPR_HPBAR_RED, true);

		if( m_OldHP_RED > RedHP)
		{
			if( _bWarningFlag )
				_SetTargetHPWarning( 0);

			if( (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED) &&
				((m_OldHP_RED / pRedObj->getMaxHP()) > 0.4f) &&
				hpRatioRed <= 0.4f)
			{
				switch(StageUID)
				{
				case STAGE_UID_BREAKDOWN :	m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_WARN_HELICOPTER"));		break;
				case STAGE_UID_GIRAN	 :
				case STAGE_UID_GIRAN2	 :	m_pHUD->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_WARNING_HOG_CANNON")); break;
				case STAGE_UID_HELISPOT	 :	m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_WARN_HELICOPTER"));		break;
				default :
						I3PRINTLOG(I3LOG_WARN,  "HUD no work.");
					break;
				}
			
				//헬기가 위험하답
				g_pSndMng->StartAnnounceMessage( GTA_MISSION_WARN_HELICOPTER);
			}
		}
	}

	if( m_OldHP_BLUE != BlueHP)
	{
		REAL32 hpRatioBlue		= BlueHP / pBlueObj->getMaxHP();
		REAL32 hpBarSizeBlue	= hpBarSize * hpRatioBlue;

		m_pSprite->SetRect( HUD_TARGETHP_SPR_HPBAR_BLUE, (REAL32)(nPosX + 43.0f), (53.0f)+fClanOffsetY, hpBarSizeBlue, (10.0f));

		if( !m_pSprite->GetEnable( HUD_TARGETHP_SPR_HPBAR_BLUE))
		{
			m_pSprite->SetEnable( HUD_TARGETHP_SPR_HPBAR_BLUE, true);
		}

		if( m_OldHP_BLUE > BlueHP)
		{
			if( _bWarningFlag )
				_SetTargetHPWarning( 1);

			if( (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE) &&
				(m_OldHP_BLUE / pBlueObj->getMaxHP() > 0.4f) &&
				hpRatioBlue <= 0.4f)
			{
				switch (StageUID)
				{
				case STAGE_UID_BREAKDOWN:	m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_WARN_GENERATOR"));		break;
				case STAGE_UID_GIRAN:
				case STAGE_UID_GIRAN2:	m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_WARNING_GOLEM")); break;
				case STAGE_UID_HELISPOT:	m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DESTROY_WARN_HELICOPTER"));		break;
				default:
					I3PRINTLOG(I3LOG_WARN, "HUD no work.");
					break;
				}

				g_pSndMng->StartAnnounceMessage( GTA_MISSION_WARN_GENERATOR);
			}
		}
	}

	m_OldHP_RED	= RedHP;
	m_OldHP_BLUE	= BlueHP;
}

void CHUD_TargetHP::_UpdateTargetHP_DefenceMission( REAL32 tm)
{
	REAL32 fClanOffsetY = 0.0f;
	REAL32 defHP0, defHP1;

	I3ASSERT( m_pSprite != nullptr);

	if( ChannelContext::i()->GetBattleChannel())
		fClanOffsetY = 18.0f;

	_UpdateUnderAttack( tm);

	CGameMissionManager * pMissionMng = CGameMissionManager::i(); 

	i3Object * pDefObj0 = pMissionMng->getDefenceObj( 0);
	if( pDefObj0 == nullptr)
		return;

	i3Object * pDefObj1 = pMissionMng->getDefenceObj( 1);
	if( pDefObj1 == nullptr)
		return;

	if( !m_bInitObjectHP )
	{
		m_OldHP_RED = pDefObj0->getCurrentHP();
		m_OldHP_BLUE = pDefObj1->getCurrentHP();

		m_bInitObjectHP = true;
	}

	defHP0		= pDefObj0->getCurrentHP();
	defHP1		= pDefObj1->getCurrentHP();

	bool _bWarningFlag = true;

	if( (m_OldHP_RED != defHP0 ) ||  ( m_OldHP_BLUE != defHP1 ) )
	{
		if( !m_bInitObjectSynchronize )
		{
			m_bInitObjectSynchronize = true;
			_bWarningFlag = false;
		}
	}

	STAGE_ID	StageUID = MyRoomInfoContext::i()->getStage()->GetStageUID();
	if( m_OldHP_RED != defHP0)
	{
		I3ASSERT( pDefObj0->getMaxHP() > 0);

		REAL32 hpRatioRed		= defHP0 / pDefObj0->getMaxHP();

		if( (defHP0 == 0))
		{
			// Defence Mission의 경우 파괴되면 배경음 출력
			if( m_bPlayBGMforDefence == false)
			{
				g_pSndMng->StartBGM( "Sound/BackGround/Breakdown.ogg", true);
				m_bPlayBGMforDefence = true;
			}

			// 경고 문구 탱크가 파괴되었다.
			
			if(StageUID == STAGE_UID_BLACKPANTHER)
			{
				m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_BREAK_ATANK"));
			}
			else
				I3PRINTLOG(I3LOG_WARN, "HUD no work.");

			//탱크가 파괴되었다.
			//g_pSndMng->StartAnnounceMessage( GTA_MISSION_BREAK_ATANK);
		}
		else if( m_OldHP_RED > defHP0)
		{
			if( _bWarningFlag )
				_SetTargetHPWarning( 0);

			if( (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE) &&
				((m_OldHP_RED / pDefObj0->getMaxHP()) > WARN_OBJECT_DEFENCE) &&
				(hpRatioRed <= WARN_OBJECT_DEFENCE) )
			{
				// 경고 문구
				if(StageUID == STAGE_UID_BLACKPANTHER)
				{
					m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_WARN_ATANK"));
				}
				else 
					I3PRINTLOG(I3LOG_WARN,  "HUD no work.");

				//헬기가 위험하답
				g_pSndMng->StartAnnounceMessage( GTA_MISSION_WARN_ATANK);
			}
		}
	}

	if( m_OldHP_BLUE != defHP1)
	{
		REAL32 hpRatioBlue		= defHP1 / pDefObj1->getMaxHP();

		if( (defHP1 == 0))
		{
			// Defence Mission의 경우 파괴되면 배경음 출력
			if(m_bPlayBGMforDefence == false)
			{
				g_pSndMng->StartBGM( "Sound/BackGround/Breakdown.ogg", true);
				m_bPlayBGMforDefence = true;
			}

			// 경고 탱크가 파괴되었다.
			if(StageUID == STAGE_UID_BLACKPANTHER)
			{
				m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_BREAK_BTANK"));
			}
			else
				I3PRINTLOG(I3LOG_WARN,  "HUD no work.");

			//탱크가 파괴되었다.
			//g_pSndMng->StartAnnounceMessage( GTA_MISSION_BREAK_BTANK);
		}
		else if( m_OldHP_BLUE > defHP1)
		{
			if( _bWarningFlag )
				_SetTargetHPWarning( 1);

			if( (BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE) &&
				(m_OldHP_BLUE / pDefObj1->getMaxHP() > WARN_OBJECT_DEFENCE) &&
				(hpRatioBlue <= WARN_OBJECT_DEFENCE))
			{
				if( StageUID == STAGE_UID_BLACKPANTHER)
				{
					m_pHUD->SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_MISSION_DEFENCE_WARN_BTANK"));
				}
				else 
					I3PRINTLOG(I3LOG_WARN,  "HUD no work.");

				g_pSndMng->StartAnnounceMessage( GTA_MISSION_WARN_BTANK);
			}
		}
	}

	m_OldHP_RED		= defHP0;
	m_OldHP_BLUE	= defHP1;
}

void CHUD_TargetHP::_UpdateTargetHP_BossMission( REAL32 tm)
{
	I3ASSERT( m_pSprite != nullptr);

	_UpdateUnderAttack( tm);
}

void CHUD_TargetHP::_UpdateTargetPos()//파괴할 목표물의 위치를 표시해준다아~
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call _UpdateTargetPos....\n");
		return;
	}
#endif

	CGameMissionManager * pMissionMng = CGameMissionManager::i(); 
	i3Object * pObj = pMissionMng->getRedObject();
	if( pObj == nullptr )
		return;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	HUD_TARGETHP_SPR state = HUD_TARGETHP_SPR_OBJECT_IMG_RED;
	CGameCharaBase * pChara = _GetCurrentChara();
	if( pChara == nullptr)
		return;

	VEC3D * pCharaPos = pChara->GetPos();
	VEC3D * pObjPos = pObj->GetPos();

	INT32 centeridx = 0;

	if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
	{
		pObjPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
		if( (pChara->getAttachedObject() == pObj) )
			pCharaPos = pObjPos;

		i3Vector::Zero( &m_vDestroyObjectCenterPos[ 0]);
	}

	if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED)
	{
		pObj		= pMissionMng->getBlueObject();
		if( pObj == nullptr) return;

		pObjPos		= pObj->GetPos();

		if( i3::same_of<CGameObjectRespawnHelicopter*>(pObj))
		{
			pObjPos = i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pObj)->getRootDummy()->GetCacheMatrix());
			if( (pChara->getAttachedObject() == pObj) )
				pCharaPos = pObjPos;

			i3Vector::Zero( &m_vDestroyObjectCenterPos[ 1]);
		}

		state		= HUD_TARGETHP_SPR_OBJECT_IMG_BLUE;
		centeridx	= 1;
	}

	if( pObj->getCurrentHP() == 0.f )
	{
		if( m_pSprite->GetEnable( state))
			m_pSprite->SetEnable( state, false);

		return;
	}

	REAL32 X, Y, length;
	VEC3D	vLength;
	i3Vector::Sub( &vLength, pCharaPos, pObjPos);

	length = i3Vector::Length( &vLength);

	if( length > 10.0f)
	{
		VEC3D vPos;
		i3Vector::Add( &vPos, pObjPos, &m_vDestroyObjectCenterPos[centeridx]);

		if( _CalcProjectionPos( &X, &Y, &vPos))
		{
			REAL32 rate = 1.1f - (length / 10.0f) * 0.1f;
			REAL32 size = MINMAX( 26.5f, 53.0f * rate, 53.0f);

			REAL32 XX = nScreenWidth * X - size * 0.5f;
			REAL32 YY = nScreenHeight * Y - size * 0.5f;

			m_pSprite->SetRect( state, XX, YY, size, size);

			if( !m_pSprite->GetEnable( state))
			{
				m_pSprite->SetEnable( state, true);
			}

			return;
		}
	}

	if( m_pSprite->GetEnable( state))
	{
		m_pSprite->SetEnable( state, false);
	}
}

void CHUD_TargetHP::_SetTargetHPWarning( INT32 idx)
{
	if( !m_bUnderAttackWarn[ idx] )
	{
		m_rUnderAttackWarnElapsed[ idx] = 0.0f;
	}

	m_bUnderAttackWarn[ idx] = true;
	
	m_rUnderAttackWarnElapsedTotal[ idx] = 0.0f;

	m_pHUD->SetDestroyObjectUnderAttack( idx, true);

	// 탭미니맵의 WAVE 동기화를 위해 추가. 2011.11.03. ugeun.ji
	// * 탭미니맵은 활성화 때에만 Update 됩니다.
	//m_DestroyObjectAttackState.SetUnderAttack(idx, true);
}

void CHUD_TargetHP::_UpdateUnderAttack( REAL32 tm)
{
	bool bEnable[2] = { false, false};

	for( INT32 i = 0; i < 2; ++i)
	{
		if( m_bUnderAttackWarn[ i])
		{
			m_rUnderAttackWarnElapsed[ i] += tm;
			m_rUnderAttackWarnElapsedTotal[ i] += tm;

			if( m_rUnderAttackWarnElapsed[ i] > 0.2f)
			{
				bEnable[i] = true;

				if( m_rUnderAttackWarnElapsed[ i] > 0.4f)
				{	
					m_rUnderAttackWarnElapsed[ i] = 0.0f;
				}
			}

			if( m_rUnderAttackWarnElapsedTotal[ i] > 3.0f)
			{
				m_bUnderAttackWarn[ i] = false;
				m_rUnderAttackWarnElapsed[ i] = 0.0f;
				m_rUnderAttackWarnElapsedTotal[ i] = 0.0f;

				m_pHUD->SetDestroyObjectUnderAttack( i, false);
				// 탭미니맵의 WAVE 동기화를 위해 추가. 2011.11.03. ugeun.ji
				// * 탭미니맵은 활성화 때에만 Update 됩니다.
				//m_DestroyObjectAttackState.SetUnderAttack(i, false);
			}
		}
	}

	if( m_pSprite->GetEnable( HUD_TARGETHP_SPR_HPBAR_WARING_RED) != bEnable[0])
	{
		m_pSprite->SetEnable( HUD_TARGETHP_SPR_HPBAR_WARING_RED, bEnable[0]);
	}

	if( m_pSprite->GetEnable( HUD_TARGETHP_SPR_HPBAR_WARING_BLUE) != bEnable[1])
	{
		m_pSprite->SetEnable( HUD_TARGETHP_SPR_HPBAR_WARING_BLUE, bEnable[1]);
	}
}

bool CHUD_TargetHP::_CalcProjectionPos( REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn)
{
	VEC3D	vTemp;
	VEC3D	vTempIn;
	MATRIX	CamMat;
	VEC3D*	pvCamAt;
	VEC3D	vSub;

	pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

	i3Vector::Sub( &vSub, i3Matrix::GetPos( g_pCamera->getCamMatrix()) , vIn );
	i3Vector::Normalize( &vSub, &vSub );

	REAL32 rDot = i3Vector::Dot( pvCamAt, &vSub);
	if( rDot <= 0.0f)
	{
		return false;
	}
	
	i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

	i3Vector::Copy( &vTempIn, vIn);
	i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

	*fOutX	= i3Vector::GetX( &vTemp );
	*fOutY	= i3Vector::GetY( &vTemp );

	*fOutX *= 0.5f;		*fOutX += 0.5f;
	*fOutY *= -0.5f;	*fOutY += 0.5f;

	return true;
}

void CHUD_TargetHP::CreateDetectedEnemySprite( void)
{
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DESTROY) && 
		!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE) )
		return; //방어코드니까 일단 살려주겠다.

	m_pSpriteDetectedEnemy = i3Sprite2D::new_object();
	m_pSpriteDetectedEnemy->Create( MAX_TEAM_USER, true, true);

	m_pSpriteDetectedEnemy->SetTexture( m_pTex);

	COLOR col;
	i3Color::Set( &col, (UINT8) 255, 255, 255, 255);

	for( INT32 i = 0 ; i < MAX_TEAM_USER; ++i)
	{
		m_pSpriteDetectedEnemy->SetEnable( i , false);
		m_pSpriteDetectedEnemy->SetTextureCoord( i, 285.0f, 120.0f, 320.0f, 155.0f);
		m_pSpriteDetectedEnemy->SetColor( i, &col);
		m_pSpriteDetectedEnemy->SetRect( i, 10.0f, 10.0f, 35.0f, 35.0f);
	}

	m_pHUD->getHUDRoot()->AddChild(m_pSpriteDetectedEnemy);
}

void CHUD_TargetHP::CreateWeakedEnemySprite()
{
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ) return;	//방어코드니까 일단 살려주겠다.

	m_pSpriteWeakedEnemy = i3Sprite2D::new_object();
	m_pSpriteWeakedEnemy->Create( MAX_ALL_USER, true, true);

	m_pSpriteWeakedEnemy->SetTexture( m_pTex);

	COLOR col;
	i3Color::Set( &col, (UINT8) 255, 255, 255, 255);

	for( INT32 i = 0 ; i < MAX_ALL_USER; ++i)
	{
		m_pSpriteWeakedEnemy->SetEnable( i , false);
		m_pSpriteWeakedEnemy->SetTextureCoord( i, 285.0f, 120.0f, 320.0f, 155.0f);
		m_pSpriteWeakedEnemy->SetColor( i, &col);
		m_pSpriteWeakedEnemy->SetRect( i, 10.0f, 10.0f, 35.0f, 35.0f);
	}

	m_pHUD->getHUDRoot()->AddChild(m_pSpriteWeakedEnemy);
}

INT32 CHUD_TargetHP::_GetCurrentSlotIndex( void)
{
	I3ASSERT( m_pHUD != nullptr);

	return m_pHUD->GetCurrentSlotIndex();
}

CGameCharaBase * CHUD_TargetHP::_GetCurrentChara( void)
{
	return g_pCharaManager->getCharaByNetIdx( _GetCurrentSlotIndex());
}

void CHUD_TargetHP::_UpdateWeakedEnemySprite( REAL32 tm)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call _UpdateWeakedEnemySprite....\n");
		return;
	}
#endif

	if( m_pSpriteWeakedEnemy == nullptr)
		return;

	CGameCharaBase * pChara = _GetCurrentChara();
	if(pChara == 0 ) return;

	if( pChara->IsDino() == false )
	{
		for( INT32 i = 0 ; i < MAX_ALL_USER; ++i)
		{
			if( m_pSpriteWeakedEnemy->GetEnable( i))
				m_pSpriteWeakedEnemy->SetEnable( i, false);
		}

		return;
	}

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	CGameCharaBase *	pPlayer;
	bool				bInit			= false;

	for( INT32 i = 0 ; i < MAX_ALL_USER; ++i)
	{
		bInit = false;

		pPlayer	= g_pCharaManager->getCharaByNetIdx( i);
		if( pPlayer != nullptr)
		{
			if( pPlayer->isCharaStateMask( CHARA_STATE_DEATH))	bInit = true;
			if( pPlayer->getDamagedByHowl() == false)	bInit = true;
			if( pPlayer->isLocal() )	bInit = true;
			if( pPlayer->IsDino() )		bInit = true;

			if( bInit == false)
			{
				m_DEUI_Weak[i].LocalElapsedTime += tm;

				REAL32 X, Y;
				VEC3D vDiff, vPos;

				i3Vector::Copy( &vPos, pPlayer->GetPos());
				i3Vector::Sub( &vDiff, i3Matrix::GetPos( g_pCamera->getCamMatrix()), &vPos);
						
				REAL32 length = i3Vector::Length( &vDiff);

				i3Vector::Add( &vPos, i3Matrix::GetPos( pPlayer->getBoneContext()->getIKMatrix()), i3Matrix::GetPos( pPlayer->getBoneContext()->getHeadMatrix()));
				i3Vector::Scale( &vPos, &vPos, 0.5f);

				if( _CalcProjectionPos( &X, &Y, &vPos))
				{
					REAL32 XX = nScreenWidth * X;
					REAL32 YY = nScreenHeight * Y;

					m_pSpriteWeakedEnemy->SetEnable( i, true);
					
					REAL32 rate = 1.1f - (length / 10.0f) * 0.1f;
					REAL32 ssize = MINMAX( 26.5f, 53.0f * rate, 53.0f);

					REAL32 fac  = MIN( m_DEUI_Weak[i].LocalElapsedTime, 0.3f);
					REAL32 size = ssize * ( 1.0f + i3Math::cos( I3_DEG2RAD( 90.0f * (fac / 0.3f))) * 2.0f);

					m_pSpriteWeakedEnemy->SetRect( i, XX - size * 0.5f, YY - size * 0.5f, size, size);
				}
				else
				{
					bInit = true;
				}
			}
		}
		else
		{
			bInit = true;
		}

		if( bInit)
		{
			m_DEUI_Weak[i].Init();

			if( m_pSpriteWeakedEnemy->GetEnable( i))
			{
				m_pSpriteWeakedEnemy->SetEnable( i, false);
			}
		}
	}
}

void CHUD_TargetHP::SetDetectedEnemyInfo( INT32 idx)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call SetDetectedEnemyInfo.....\n");
		return;
	}
#endif

	if( m_pSpriteDetectedEnemy == nullptr)
		return;

	CGameCharaBase * pChara = _GetCurrentChara();

	CGameCharaBase * pPlayer	= g_pCharaManager->getCharaByNetIdx( idx);
	if( pPlayer == nullptr)	return;
	if( pPlayer->getCharaTeam() == pChara->getCharaTeam())
		return ;

	// 공수교대가 들어가면서 실제 슬롯 인덱스 고정
	// 2로 나누게 되면 0~7의 값이 고정됩니다.
	INT32 slotidx = idx / 2;

	if( m_DEUI[slotidx].bEnableUpdate)//이미 공격중 표시된 적군이면
	{
		m_DEUI[slotidx].TotalUpdateTime = DETECT_ENEMY_UPDATE_TIME + m_DEUI[slotidx].LocalElapsedTime;
	}
	else
	{
		m_DEUI[slotidx].bEnableUpdate		= true;
		m_DEUI[slotidx].LocalElapsedTime	= 0.0f;
		m_DEUI[slotidx].TotalUpdateTime		= DETECT_ENEMY_UPDATE_TIME;
	}
}

void CHUD_TargetHP::_UpdateDetectedEnemySprite( REAL32 tm)
{
	if( m_pSpriteDetectedEnemy == nullptr)
		return;

	CGameCharaBase * pChara = _GetCurrentChara();

	if( pChara == nullptr)
		return;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	// 내 index를 가지고 체크 해야 한다.
	INT32 idx = pChara->getCharaNetIndex() % 2;

	for( INT32 i = 0 ; i < MAX_TEAM_USER; ++i)
	{
		if( m_DEUI[i].bEnableUpdate )
		{
			m_DEUI[i].LocalElapsedTime += tm;

			if( m_DEUI[i].LocalElapsedTime < m_DEUI[i].TotalUpdateTime)
			{
				// 상대편 인덱스는 내가 Red 팀 슬롯일 때만 +1 하면 됩니다.
				INT32 slotidx = i * 2;
				if (idx == 0)
					slotidx++;

				CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( slotidx);
				
				if( pPlayer == nullptr )
				{
					m_DEUI[i].Init();

					m_pSpriteDetectedEnemy->SetEnable( i, false);

					continue;
				}

				if( !pPlayer->isCharaStateMask(CHARA_STATE_DEATH) )
				{
					REAL32 X, Y;
					VEC3D vDiff, vPos;
					bool bValidCharaPos = true;

					i3Vector::Copy( &vPos, pPlayer->GetPos());
					if( pPlayer->IsOnObject())
					{
						CGameObjectWithWeapon	* pOnObj	= pPlayer->GetOnObject();
						if( pOnObj != nullptr)
						{
							WeaponOnObject			* pWeapon	= pOnObj->GetWeaponOnObject();
							if( i3::same_of<WeaponM197*>(pWeapon))
							{
								if( i3::same_of<CGameObjectRidingHelicopter*>(pOnObj))
								{
									i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRidingHelicopter*)pOnObj)->getRootDummy()->GetCacheMatrix()));
									bValidCharaPos = false;
								}
								else if( i3::same_of<CGameObjectRespawnHelicopter*>(pOnObj))
								{
									i3Vector::Copy( &vPos, i3Matrix::GetPos( ((CGameObjectRespawnHelicopter*)pOnObj)->getRootDummy()->GetCacheMatrix()));
									bValidCharaPos = false;
								}
							}
						}
					}

					i3Vector::Sub( &vDiff, i3Matrix::GetPos( g_pCamera->getCamMatrix()), &vPos);
					
					REAL32 length = i3Vector::Length( &vDiff);

					if( bValidCharaPos)
					{
						i3Vector::Add( &vPos, i3Matrix::GetPos( pPlayer->getBoneContext()->getIKMatrix()), i3Matrix::GetPos( pPlayer->getBoneContext()->getHeadMatrix()));
						i3Vector::Scale( &vPos, &vPos, 0.5f);
					}

					if( _CalcProjectionPos( &X, &Y, &vPos))
					{
						REAL32 XX = nScreenWidth * X;
						REAL32 YY = nScreenHeight * Y;

						if( !m_pSpriteDetectedEnemy->GetEnable( i))
						{
							m_pSpriteDetectedEnemy->SetEnable( i, true);
						}
						
						REAL32 rate = 1.1f - (length / 10.0f) * 0.1f;
						REAL32 ssize = MINMAX( 26.5f, 53.0f * rate, 53.0f);

						REAL32 fac  = MIN( m_DEUI[i].LocalElapsedTime, 0.3f);
						REAL32 size = ssize * ( 1.0f + i3Math::cos( I3_DEG2RAD( 90.0f * (fac / 0.3f))) * 2.0f);

						m_pSpriteDetectedEnemy->SetRect( i, XX - size * 0.5f, YY - size * 0.5f, size, size);
					}
					else
					{
						if( m_pSpriteDetectedEnemy->GetEnable( i))
						{
							m_pSpriteDetectedEnemy->SetEnable( i, false);
						}
					}
				}
				else
				{
					m_DEUI[i].Init();

					m_pSpriteDetectedEnemy->SetEnable( i, false);
				}
			}
			else
			{
				m_DEUI[i].Init();

				m_pSpriteDetectedEnemy->SetEnable( i, false);
			}
		}
	}
}

void CHUD_TargetHP::Update( REAL32 tm)
{
	STAGE_MODE modetype = MyRoomInfoContext::i()->getStage()->GetStageMode();
	switch (modetype)
	{
	case STAGE_MODE_DESTROY:
		_UpdateTargetHP_DestroyMission(tm);
		_UpdateDetectedEnemySprite(tm);
		break;
	case STAGE_MODE_ESCAPE:
		_UpdateWeakedEnemySprite(tm);
		break;
	case STAGE_MODE_DEFENCE:
		_UpdateTargetHP_DefenceMission(tm);
		_UpdateDetectedEnemySprite(tm);
		break;
	}
}