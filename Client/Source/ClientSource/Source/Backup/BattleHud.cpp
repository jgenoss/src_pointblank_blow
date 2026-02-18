#include "pch.h"
#include "BattleHud.h"
#include "BattleGui.h"
#include "ConfigRenderBattle.h"
#include "GameCharaManager.h"
#include "GlobalVariables.h"
#include "ChainKills.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameObject.h"
#include "./Camera/GameCamera_OnObject.h"
#include "./Weapon/WeaponM197.h"
#include "./Weapon/WeaponInfo/M197Info.h"

#include "UI/UIUtil.h"
#include "UI/UIBattlePopupBase.h"
#include "UI/UIBattleFrame.h"				// 강퇴 제안 팝업시 닉네임마크를 삭제하기 위해 포함시킴.(11.06.09.수빈)
#include "UI/UIHUDNoticeShop.h"

#include "UI/UIHUDBase.h"
#include "UI/UIHUDIngameChat.h"
#include "UI/UIHUDRecordBoard.h"
#if defined(SKILL_VIEWER)
#include "UI/UIHUDSkillViewer.h"
#endif

#include "ui/UIHudManager.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE(CHud); //, i3GameObjBase);

#ifdef DOMI_SENTRYGUN
#include "../Weapon/SentryGun/SentrygunStuff.h"
#endif

#define ENABLE_TEST_SCORE	0

#define NOTICE_TEXT_HEIGHT (30*i3GuiRoot::getHeightRatioBy768())
#define NOTICE_LINE_HEIGHT (30*i3GuiRoot::getHeightRatioBy768())

CHud::CHud(void)
{
	i3_prof_func();
	UINT32 i;

	m_CurrentUserIdx = -1;

	m_pSceneNode = i3Node::new_object();

	m_pSpriteHit					= NULL;
	m_pSpriteDeath					= NULL;
	m_pSpriteEtcMessageBG			= NULL;

	m_pSpriteKillTextBG				= NULL;
	for(i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		m_pSpriteKillMark[i] = NULL;
	}

	m_pSpriteHeadShot				= NULL;
	m_pSpriteObjectDefence			= NULL;
	m_pSpriteObjDefenctAndHead		= NULL;
//Key:EscapeMission
	m_pSpriteLostWorldEP5			=NULL;
	m_pSpriteGrenadeLauncherCrossHair = NULL;
	m_pSpriteMission				= NULL;
	m_pSpriteBoss					= NULL;
	m_pSpriteBombMission			= NULL;
	m_pSpriteObjectReply			= NULL;
	m_pRevengeInfo					= NULL;
	m_pRevengeTextBlue				= NULL;
	m_pRevengeTextRed				= NULL;
	m_pRevengeTextBlueCount		= NULL;
	m_pRevengeTextRedCount			= NULL;

	m_pGrenadeInd					= NULL;

	m_GrenadeK400.reserve( 9);			// 자기 자신의 것도 포함해야합니다.
//	m_GrenadeK400.SetOptmizeEnableWhenClear( FALSE);

	m_GrenadeK413.reserve( 9);			// 자기 자신의 것도 포함해야합니다.
//	m_GrenadeK413.SetOptmizeEnableWhenClear( FALSE);

	m_GrenadeC5.reserve( 9);				// 자기 자신의 것도 포함해야합니다.
//	m_GrenadeC5.SetOptmizeEnableWhenClear( FALSE);

	m_GrenadeChocolateGrenade.reserve( 9);			// 자기 자신의 것도 포함해야합니다.
//	m_GrenadeChocolateGrenade.SetOptmizeEnableWhenClear( FALSE);

	m_pHUDRoot						= NULL;
	m_pScrEffectAttr				= NULL;
	m_pScrEffect					= NULL;
	m_pScrTex2D						= NULL;
	m_pScrTexture					= NULL;

	m_pTexture						= NULL;
	m_pTextureHUD2					= NULL;
	m_pTextureHUD3					= NULL;
	m_bUpdateNotice					= FALSE;

	m_bUpdateHelper					= FALSE;
	m_pSpriteHelper					= NULL;
	m_pTextureHelper				= NULL;

	m_pTextureRankMark				= NULL;

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		m_pTextNotice[i] = NULL;
		m_pTextDeathName[i] = NULL;
		m_pTextKillerName[i] = NULL;
	}

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		m_pTextName[i] = NULL;
	}

	m_pTextEtc = NULL;
	for(i=0; i<HUD_BOMB_TEXT_COUNT; i++)
	{
		m_pTextBomb[i] = NULL;
	}
	for(i=0; i<HUD_MISSION_TEXT_COUNT; i++)
	{
		m_pTextMission[i] = NULL;
	}

	for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
	{
		m_pTextRadioChat[ i ] = NULL;
	}

	for( i = 0; i < MAX_INDICATOR; i++ )
	{
		m_pTextGrenade[ i ] = NULL;
	}

	m_pTextDummy		= NULL;

	for( i = 0; i < HUD_OBJECT_REPLY_COUNT; i++ )
	{
		m_pTextObjectReply[ i ] = NULL;
		m_fDeltaTimeObjectReply[ i ] = 0.0f;
	}

	// 훈련전관련 변수 초기화
	m_pSpriteTrainingBG = NULL;
	for(i=0; i<MAX_TEAM_USER; i++)
	{
		m_pSpriteTrainingRankMark[i] = NULL;
		m_pTextTrainingUserNick[i] = NULL;
		m_pTextTrainingUserScore[i] = NULL;
		m_pTextTrainingHP[i] = NULL;
		m_pSpriteTrainingHPGauge[i] = NULL;
	}
	m_pSpriteTrainingRecordBG = NULL;
	for(i=0; i<HUD_TEXT_TRAINING_RECORD_COUNT; i++)
	{
		m_pTextTrainingRecord[i] = NULL;
	}

	m_fBulletScale		= 0.0f;
	
	m_fViewHalfWidth	= 0.0f;	//화면의 중앙 좌표, 화면크기의 반
	m_fViewHalfHeight	= 0.0f;	//

	m_fBulletBarPosX	= 0.0f;
	m_fBulletBarPosY	= 0.0f;

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		m_fNoticeTime[i] = 0.0f;
		m_fKillerTime[i] = 0.0f;
		m_fScaleSize[i] = 0.0f;
	}
	m_bUpdateNotice		= FALSE;
	m_bOnlyNoticeUpdate = FALSE;
	m_bUpdateHeadShot = FALSE;

	m_fHeadShotTime		= 0.0f;
	m_fEtcTextTime		= 0.0f;

	m_idxDamageDirectionOrder = 0;

	for(i=0; i<HUD_SPRITE_HIT_ANGLE_MAX; i++)
	{
		m_fDamageDirectionAngle[i] = 0.0f;
	}

	m_pDominationWepMarkTexture		= NULL;

	m_pFrindlyWepMarkTexture		= NULL;

	m_pSpriteObserver				= NULL;

	for( i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++ )
	{
		m_pTextObserver[i] = NULL;
	}
	m_bEnableObserverMode			= FALSE;
	
	m_bBombMissionEnable			= FALSE;
	m_bOneTimeAreaUpdate			= FALSE;
	m_bDettachBomb					= FALSE;

	m_fMissionMessageTime			= 0.0f;
	m_fMissionNoticeTimer			= 0.0f;
	m_fMissionNoticeLifeTime		= 3.f;

	m_bEnableDeathSprite			= FALSE;
	m_fDeathSpriteTime				= 0.0f;

	m_bEnableRadioChat				= FALSE;
	m_bEnableRadio1					= FALSE;
	m_bEnableRadio2					= FALSE;
	m_bEnableRadio3					= FALSE;
	m_rRadioReusingTimer			= 0.f;

	m_bGameEnd						= FALSE;

	m_pChainKills					= NULL;

	m_pMovingText					= NULL;

	m_pMiniMap	= NULL;

	m_HudMode = HUD_MODE_DEFAULT;

	m_fEtcMessageLocalTime = 0.0f;

	m_fOriginalMouseSensivity = 0.0f;
	
	// 피격필터
	m_pDamageFilterTexture = NULL;
	m_pDamageFilterSprite = NULL;

	m_bShowChatNotice		= FALSE;
	m_nCurrentChatNotice	= 0;

	m_pTextureGrenadeLauncherCrossHair = NULL;

	for( i = 0 ; i < 16; ++i)
	{
		m_Revenge[i].szTargetName[0] = 0;
		m_Revenge[i].nWinPoint = 0;
		m_Revenge[i].nLosePoint = 0;
	}
	m_bEnableRevengeUpdate = FALSE;
	m_fRevengeLocalTime = 0.0f;

	m_pRideHelicopter = NULL;
//	m_pPanelDinoHP	= NULL;

	m_pObserverHPGaugeHUD = NULL;

	m_pPanelTargetHP = NULL;
	m_pPanelScore = NULL;

	m_pWarnningHack = NULL;

	m_pScope = NULL;

	m_bEnabledGrenadeLauncherCrossHair = false;
	m_bEnableObserverFlyMode = false;

	m_pRevengeTexture = NULL;
	m_pTextTarget = NULL;
	m_pTextureHitMark = NULL;
	m_pTextureWeaponKill = NULL;

	m_UItypelist.push_back( HUD::PLAYER_STATE );
	m_UItypelist.push_back( HUD::PLAYER_DINO_STATE );
	m_UItypelist.push_back( HUD::AMMO_STATE );
	m_UItypelist.push_back( HUD::DINO_STATE );
}

CHud::~CHud(void)
{
	i3_prof_func();

	DestroyTrainingCompetition();

	if( m_pPanelTargetHP != NULL)
	{
		RemoveChild( m_pPanelTargetHP);

		m_pPanelTargetHP->Release();
		m_pPanelTargetHP = NULL;
	}

	if( m_pPanelScore != NULL)
	{
		RemoveChild( m_pPanelScore);

		m_pPanelScore->Release();
		m_pPanelScore = NULL;
	}

	// 피결필터
	I3_SAFE_NODE_RELEASE(m_pDamageFilterSprite);
	I3_SAFE_RELEASE(m_pDamageFilterTexture);


	UINT32 i;

	I3_SAFE_NODE_RELEASE( m_pHUDRoot);
	I3_SAFE_RELEASE(m_pChainKills);

	for(i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextObserver[i]);
	}	

	for(i = 0; i < HUD_TEXT_OBSERVER_FLY_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextObserverFly[i]);
	}	

	I3_SAFE_NODE_RELEASE(m_pSpriteObserver);

	I3_SAFE_NODE_RELEASE(m_pSpriteHit);
	I3_SAFE_NODE_RELEASE( m_pGrenadeInd);
	I3_SAFE_NODE_RELEASE(m_pSpriteDeath);
	I3_SAFE_NODE_RELEASE(m_pSpriteEtcMessageBG);
	I3_SAFE_NODE_RELEASE(m_pSpriteLostWorldEP5);
	I3_SAFE_NODE_RELEASE(m_pSpriteKillTextBG);
	for(i = 0; i < HUD_NOTICE_COUNT; i++)
	{
        I3_SAFE_NODE_RELEASE(m_pSpriteKillMark[i]);
	}

	I3_SAFE_NODE_RELEASE(m_pSpriteHeadShot);
	I3_SAFE_NODE_RELEASE( m_pSpriteObjectDefence);
	I3_SAFE_NODE_RELEASE( m_pSpriteObjDefenctAndHead);

	I3_SAFE_NODE_RELEASE( m_pSpriteGrenadeLauncherCrossHair);
	I3_SAFE_RELEASE(m_pTexture);
	I3_SAFE_RELEASE(m_pTextureHUD2);
	I3_SAFE_RELEASE( m_pTextureHUD3);
	I3_SAFE_RELEASE( m_pTextureRankMark);
	I3_SAFE_NODE_RELEASE(m_pSpriteMission);
	I3_SAFE_NODE_RELEASE(m_pSpriteBoss);
	I3_SAFE_NODE_RELEASE(m_pSpriteBombMission);
	I3_SAFE_NODE_RELEASE(m_pSpriteObjectReply);

	I3_SAFE_NODE_RELEASE(m_pScrEffect);
	I3_SAFE_NODE_RELEASE(m_pScrTex2D);

	I3_SAFE_NODE_RELEASE(m_pRevengeInfo);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextBlue);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextRed);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextBlueCount);
	I3_SAFE_NODE_RELEASE(m_pRevengeTextRedCount);

	I3_SAFE_RELEASE(m_pScrTexture);
	I3_SAFE_NODE_RELEASE(m_pScrEffectAttr);

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextNotice[i]);
		I3_SAFE_NODE_RELEASE(m_pTextDeathName[i]);
		I3_SAFE_NODE_RELEASE(m_pTextKillerName[i]);
		I3_SAFE_NODE_RELEASE(m_pTextTouchDown[i]);
	}	

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextName[i]);
	}

	I3_SAFE_NODE_RELEASE( m_pTextEtc);

	for(i=0; i<HUD_BOMB_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextBomb[i]);
	}
	
	for(i=0; i<HUD_TEXT_OBSERVER_FLY_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextObserverFly[i]);
	}

	for(i=0; i<HUD_MISSION_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextMission[i]);
	}

	for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
	{
		I3_SAFE_NODE_RELEASE( m_pTextRadioChat[ i ] );
	}

	for( i = 0; i < MAX_INDICATOR; i++ )
	{
		I3_SAFE_NODE_RELEASE( m_pTextGrenade[ i ] );
	}

	I3_SAFE_RELEASE( m_pTextDummy);

	for( i = 0; i < HUD_OBJECT_REPLY_COUNT; i++ )
	{
		I3_SAFE_NODE_RELEASE( m_pTextObjectReply[ i ] );
	}

	I3_SAFE_NODE_RELEASE( m_pSpriteHelper );
	I3_SAFE_RELEASE( m_pTextureHelper );

	I3_SAFE_RELEASE( m_pFrindlyWepMarkTexture);

	I3_SAFE_RELEASE(m_pDominationWepMarkTexture);

	I3_SAFE_RELEASE( m_pMiniMap);

	// 마우스 감도가 변경되었다면 서버에 저장합니다.
	if( m_fOriginalMouseSensivity != g_pEnvSet->m_rMouseSensivity)
	{
		g_pGameContext->SetNetEnvSetBasicChange( 1);
		g_pEnvSet->Net_Save(); 
		g_pGameContext->SetEventOnce( EVENT_OPTION_SAVE_N);
	}

	I3_SAFE_RELEASE( m_pTextureGrenadeLauncherCrossHair);

	// 
	g_pFramework->getChatBox()->setWhisperEnable( FALSE);

	//
	I3_SAFE_RELEASE( m_pRideHelicopter);

	// Observer HP Gauge
	I3_SAFE_RELEASE( m_pObserverHPGaugeHUD);

	I3_SAFE_RELEASE( m_pScope);

	//
	I3_SAFE_RELEASE( m_pWarnningHack);
}

void CHud::SetCurrentScreenImage( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	i3_prof_func();

	i3Viewer * pViewer = g_pFramework->GetViewer();
	REAL32 fScrWidth = (REAL32) pViewer->GetViewWidth();
	REAL32 fScrHeight = (REAL32) pViewer->GetViewHeight();

	//	백버퍼에서 전체 이미지를 가져온다.
	g_pViewer->GetRenderContext()->GetBackBufferTexture( m_pScrTexture);

	m_pScrTex2D->SetEnable( idx, TRUE);
	m_pScrTex2D->SetTexture( m_pScrTexture); 
	m_pScrTex2D->SetRect( idx, 0.f, 0.f, fScrWidth, fScrHeight);
	m_pScrTex2D->SetTextureCoord( idx, 0.f, 0.f, (REAL32) SCR_BACKBUFFER_TEX_SIZE, (REAL32) SCR_BACKBUFFER_TEX_SIZE);	
	m_pScrTex2D->SetColor( idx, r, g, b, a);
}

BOOL CHud::Create( INT32 InitialUserIdx)
{
	i3_prof_func();

	if( ! i3GameObjBase::Create( g_pFramework->getHudLayer() ) )
	{
		I3FATAL( "Create HUD fail!!!!!!!");
		return FALSE;
	}

	m_CurrentUserIdx = InitialUserIdx;

	m_pHUDRoot = i3AttrSet::new_object();

#ifndef __juul
	GetNode()->AddChild( m_pHUDRoot);

	{	// HUD Texture는 Linear filter 사용
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pHUDRoot->AddAttr( pAttr);
	}
#else
//	GetNode()->AddChild( m_pHUDRoot);

	{	// HUD Texture는 Linear filter 사용
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
//		m_pHUDRoot->AddAttr( pAttr);
	}
#endif



	///////////////////////////////////////////////////////////////
	// 반드시 아래와 같은 순서로 Create해야 합니다.
	// 함부로 생성 순서를 바꾸지 마세요.
	///////////////////////////////////////////////////////////////

	CreateGrenadeLauncherCrossHair();

	// 1.Scope Hud
	m_pScope = CHUD_Scope::new_object();
	I3ASSERT( m_pScope != NULL);
	m_pScope->CreateScopeHUD( m_pHUDRoot);

	// 2.Screen Effect
	CreateScreenEffectHUD();

	// 3.탑승 Helicopter hud생성
	CreateRideHelicopter();

	///Key:EscapeMission 티라노 HP노출하지 않습니다.
	//티라노 HP노출하지 않습니다.
	if( g_pGameContext->IsBossMissionMode())
	{
	//	m_pPanelDinoHP = CHUD_DinoHP::new_object();
	//	m_pPanelDinoHP->Create( this);

	//	I3_GAMENODE_ADDCHILD( this, m_pPanelDinoHP);
	}

	// 4.Hud
	CreateHUD();

	// 핵 프로그램 사용 경고창은 최상위에 떠야 합니다.
	CreateWarnningHack();	

	// 훈련전 개인 랭킹 관련
	InitTrainingCompetition();

	m_pFrindlyWepMarkTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_CHARA_MARK);

	m_pDominationWepMarkTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_CHARA_MARK2);

	// 초기화
	InitHud();

	// ChallengeBoard 생성후 호출해야한다.
	 ChangeChallenge();

	// 초기 마우스 감도를 저장합니다.
	m_fOriginalMouseSensivity = g_pEnvSet->m_rMouseSensivity;

	// 
	g_pFramework->getChatBox()->setWhisperEnable( TRUE);

	return TRUE;
}

void CHud::CreateMiniMap(i3Node* pParent)
{
	i3_prof_func();

	m_pMiniMap = CBattleHUD_MiniMap::new_object();

	if( m_pMiniMap->Create( g_pFramework->GetStageBattle(), pParent))
	{
		I3_GAMENODE_ADDCHILD( this, m_pMiniMap);

		m_pMiniMap->SetEnableMiniMap( false );
	}
}

void CHud::SetEnableMiniMap( bool bEnable)
{
	i3_prof_func();

	if( m_pMiniMap != NULL)
	{
		if(m_HudMode == HUD_MODE_MISSION_OBSERVER)
		{
			HUD::instance()->GetHud(HUD::MINIMAP)->SetVisible(false);
			if(m_pMiniMap->getCurrentMapMode() == CBattleHUD_MiniMap::MINIMAP_MODE_DEFAULT)
			{
				m_pMiniMap->SetEnableMiniMap( false);
			}
			else
			{
				m_pMiniMap->SetEnableMiniMap( true );
			}
		}
		else
		{
			m_pMiniMap->SetEnableMiniMap( bEnable);
			HUD::instance()->GetHud(HUD::MINIMAP)->SetVisible(bEnable);
		}
	}
}

void CHud::CreateScreenEffectHUD()
{
	i3_prof_func();

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pScrEffectAttr = i3AttrSet::new_object();

#if defined( I3_DEBUG)// 노드에 이름 넣기 디버그
	m_pScrEffectAttr->SetName( "EffectHUDAttrSet");
#endif	

	i3LightingEnableAttr *pLightEnable = i3LightingEnableAttr::new_object_ref();
	pLightEnable->Set( FALSE );
	m_pScrEffectAttr->AddAttr( pLightEnable );

	i3BlendEnableAttr *pBlendEnable = i3BlendEnableAttr::new_object_ref();
	pBlendEnable->Set( TRUE );
	m_pScrEffectAttr->AddAttr( pBlendEnable );

	i3BlendModeAttr * pScrEffectBlendModeAttr = i3BlendModeAttr::new_object_ref();
	pScrEffectBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
	pScrEffectBlendModeAttr->SetDestination( I3G_BLEND_ONE);
	m_pScrEffectAttr->AddAttr( pScrEffectBlendModeAttr);

	i3TextureFunctionAttr * pTexFuncAttr = i3TextureFunctionAttr::new_object_ref();
	pTexFuncAttr->SetFunction( I3G_TEXTURE_FUNC_ADD);
	m_pScrEffectAttr->AddAttr( pTexFuncAttr);

	//	스크린 텍스처 
	UINT32 fImageWidth = SCR_BACKBUFFER_TEX_SIZE;
	UINT32 fImageHeight = SCR_BACKBUFFER_TEX_SIZE;

	m_pScrTexture = i3Texture::new_object();
	m_pScrTexture->Create( fImageWidth, fImageHeight, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_RENDERTARGET);

	m_pScrTex2D = i3Sprite2D::new_object();
	m_pScrEffectAttr->AddChild( m_pScrTex2D);
	
	m_pScrTex2D->Create( SCR_BACKBUFFER_COUNT, TRUE, TRUE);
	for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
	{
		m_pScrTex2D->SetRect( i, 0.0f, 0.0f, (REAL32) nScreenWidth, (REAL32) nScreenHeight);
		m_pScrTex2D->SetColor( i, 255, 255, 255, 255);
	}		
	m_pScrTex2D->SetEnable( FALSE);

	//	섬광탄등 화면 단순한 이펙트에 사용합니다.
	m_pScrEffect = i3Sprite2D::new_object();
	m_pScrEffectAttr->AddChild( m_pScrEffect);

	m_pScrEffect->Create( 1, FALSE, TRUE);		
	m_pScrEffect->SetRect( 0, 0.0f, 0.0f, (REAL32) nScreenWidth, (REAL32) nScreenHeight);
	m_pScrEffect->SetColor( 0, 255, 255, 255, 255);		
	m_pScrEffect->SetEnable( FALSE);
	
	m_pHUDRoot->AddChild( m_pScrEffectAttr);
}

void CHud::ChangeTextNameSize(bool bPlus)
{
	i3_prof_func();

	if(bPlus)
	{
		m_iTextNameSize++;
	}
	else
	{
		m_iTextNameSize--;
	}

	for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextName[i]);
	}

	for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		m_pTextName[i] = i3TextNodeDX2::new_object();
		m_pTextName[i]->Create( GetDefaultFontName(), FONT_COUNT_64, m_iTextNameSize, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextName[i]->SetText("");	
		m_pTextName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextName[i]->setAutoWrapEnable( FALSE);
		m_pTextName[i]->SetEnable(FALSE);
		m_pHUDRoot->AddChild(m_pTextName[i]);		
	}
}

void CHud::CreateHUD()
{ 
	i3_prof_func();

 	INT32 i = 0;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;
	INT32	nScreenHeight	= 0;

	if( pViewer != NULL )
	{
		nScreenWidth	= pViewer->GetViewWidth();
		nScreenHeight	= pViewer->GetViewHeight();
	}

	// 피격필터
	{		
		m_pDamageFilterTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_DAMAGE_FILTER);
		
		m_pDamageFilterSprite = i3Sprite2D::new_object();
		m_pDamageFilterSprite->Create(2, TRUE, TRUE);
		m_pDamageFilterSprite->SetTexture(m_pDamageFilterTexture);
		m_pDamageFilterSprite->SetEnable(FALSE);

		m_pDamageFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDamageFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
		m_pDamageFilterSprite->SetColor(0, 255, 255, 255, 255);

		m_pDamageFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDamageFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
		m_pDamageFilterSprite->SetColor(1, 255, 255, 255, 255);
		
#ifndef __juul
		m_pHUDRoot->AddChild(m_pDamageFilterSprite);
#else
		//m_pHUDRoot->AddChild(m_pDamageFilterSprite);
#endif
	}


	{
		m_fViewHalfWidth	= (REAL32)( nScreenWidth >> 1);	//화면의 중앙 좌표, 화면크기의 반
		m_fViewHalfHeight	= (REAL32)( nScreenHeight >> 1);//
	}

	m_pTexture	= crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	m_pTextureRankMark = crb::i()->LoadHudTexture( HUD_TEXTURE_LEVEL_ICON);

	// 죽었을 경우 나오는 스프라이트
	{
		REAL32 fHeight = (REAL32)nScreenHeight*0.5f;

		m_pSpriteDeath = i3Sprite2D::new_object();
		m_pSpriteDeath->SetTexture( m_pTexture );
		m_pSpriteDeath->Create(2, TRUE, TRUE);

		m_pSpriteDeath->SetEnable(0, FALSE);
		m_pSpriteDeath->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth, fHeight);
		m_pSpriteDeath->SetColor( 0, 255,255,255,0);
		m_pSpriteDeath->SetTextureCoord(0, 1012.0f, 2.0f, 1021.0f, 382.0f);

		m_pSpriteDeath->SetEnable(1, FALSE);
		m_pSpriteDeath->SetRect(1, 0.0f, fHeight, (REAL32)nScreenWidth, fHeight);
		m_pSpriteDeath->SetColor( 1, 255,255,255,0);
		m_pSpriteDeath->SetFlip( 1, FALSE, TRUE);
		m_pSpriteDeath->SetTextureCoord(1, 1012.0f, 2.0f, 1021.0f, 382.0f);

#ifndef __juul
		m_pHUDRoot->AddChild(m_pSpriteDeath);
#else
		//m_pHUDRoot->AddChild(m_pSpriteDeath);
#endif
	}

	// 스코어 비교
	{
		m_pTextureHUD2 = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);
		I3ASSERT( m_pTextureHUD2 != NULL);
		m_pRevengeInfo = i3Sprite2D::new_object();
		m_pRevengeInfo->SetTexture( m_pTextureHUD2);
		m_pRevengeInfo->Create(1, TRUE, TRUE);

		m_pRevengeInfo->SetEnable(0, FALSE);
		m_pRevengeInfo->SetRect(0, m_fViewHalfWidth - 192.0f, (REAL32)nScreenHeight - 255.0f, 383.0f, 84.0f);
		m_pRevengeInfo->SetColor( 0, 255,255,255,255);
		m_pRevengeInfo->SetTextureCoord(0, 324.0f, 409.0f, 707.0f, 493.0f);
		m_pHUDRoot->AddChild(m_pRevengeInfo);

		m_pSpriteBoss = i3Sprite2D::new_object();	
		m_pSpriteBoss->SetTexture( m_pTextureHUD2 );
		m_pSpriteBoss->Create( 1, TRUE, TRUE);
		m_pSpriteBoss->SetEnable(0, FALSE);
		m_pSpriteBoss->SetRect(0, m_fViewHalfWidth - 192.0f, (REAL32)nScreenHeight - 255.0f, 383.0f, 84.0f);
		m_pSpriteBoss->SetColor( 0, 255,255,255,255);
		m_pSpriteBoss->SetTextureCoord(0, 324.0f, 409.0f, 707.0f, 493.0f);
		m_pHUDRoot->AddChild(m_pSpriteBoss);


		m_pRevengeTextRed = i3TextNodeDX2::new_object();
		m_pRevengeTextRed->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_REVENGE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pRevengeTextRed->SetText("");	
		m_pRevengeTextRed->SetColor( 252, 88, 88, 255);
		m_pRevengeTextRed->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
		m_pRevengeTextRed->setAutoWrapEnable( FALSE);
		m_pRevengeTextRed->SetEnable(FALSE);
		m_pRevengeTextRed->setPos( (UINT32)(m_fViewHalfWidth - 185.0f), (UINT32)((REAL32)nScreenHeight - 245.0f));
		m_pRevengeTextRed->setSize( 169, 16);
		m_pHUDRoot->AddChild(m_pRevengeTextRed);

		m_pRevengeTextBlue = i3TextNodeDX2::new_object();
		m_pRevengeTextBlue->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_REVENGE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pRevengeTextBlue->SetText("");	
		m_pRevengeTextBlue->SetColor( 67, 182, 243, 255);
		m_pRevengeTextBlue->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
		m_pRevengeTextBlue->setAutoWrapEnable( FALSE);
		m_pRevengeTextBlue->SetEnable(FALSE);
		m_pRevengeTextBlue->setPos( (UINT32)(m_fViewHalfWidth + 10.0f), (UINT32)((REAL32)nScreenHeight - 245.0f));
		m_pRevengeTextBlue->setSize( 169, 16);
		m_pHUDRoot->AddChild(m_pRevengeTextBlue);

		m_pRevengeTextRedCount = i3TextNodeDX2::new_object();
		m_pRevengeTextRedCount->Create(BATTLE_CHARA_SET, 8, 29, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pRevengeTextRedCount->SetText("");	
		m_pRevengeTextRedCount->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pRevengeTextRedCount->setAutoWrapEnable( FALSE);
		m_pRevengeTextRedCount->SetEnable(FALSE);
		m_pRevengeTextRedCount->setPos( (UINT32)(m_fViewHalfWidth - 185.0f), (UINT32)((REAL32)nScreenHeight - 225.0f));
		m_pRevengeTextRedCount->setSize( 169, 45);
		m_pHUDRoot->AddChild(m_pRevengeTextRedCount);

		m_pRevengeTextBlueCount = i3TextNodeDX2::new_object();
		m_pRevengeTextBlueCount->Create(BATTLE_CHARA_SET, 8, 29, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pRevengeTextBlueCount->SetText("");
		m_pRevengeTextBlueCount->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pRevengeTextBlueCount->setAutoWrapEnable( FALSE);
		m_pRevengeTextBlueCount->SetEnable(FALSE);
		m_pRevengeTextBlueCount->setPos( (UINT32)(m_fViewHalfWidth + 10.0f), (UINT32)((REAL32)nScreenHeight - 225.0f));
		m_pRevengeTextBlueCount->setSize( 169, 45);
		m_pHUDRoot->AddChild(m_pRevengeTextBlueCount);
	}

	// Etc Message BG
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		
		nPosX = nScreenWidth - 175;
		nPosY = nScreenHeight - 104;
		
		m_pSpriteEtcMessageBG = i3Sprite2D::new_object();
		m_pSpriteEtcMessageBG->SetTexture( m_pTexture );
		m_pSpriteEtcMessageBG->Create(1, TRUE, TRUE);
		
		m_pSpriteEtcMessageBG->SetEnable( 0, FALSE);
		m_pSpriteEtcMessageBG->SetRect( 0, (REAL32)(nPosX - 2), (REAL32)(nPosY - 30), (115.0f), (30.0f));
		m_pSpriteEtcMessageBG->SetColor( 0, 255,255,255,255);
		m_pSpriteEtcMessageBG->SetTextureCoord( 0, 817.0f, 0.0f, 932.0f, 30.0f);

		m_pHUDRoot->AddChild(m_pSpriteEtcMessageBG);
	}

#if defined( DEF_OBSERVER_MODE)
	// Observer에서는 생성할 필요가 없다.
	if( !g_pGameContext->isObserverMe())
#endif

	// 미션 - Breakdown mission
	if( g_pGameContext->IsUsingMissionObject() )
	{
		m_pPanelTargetHP = CHUD_TargetHP::new_object();

		m_pPanelTargetHP->Create( this);

		// 모드에 따라 Update를 별도로 호출하기 때문에 직접 Add시키지 않는다.
		//I3_GAMENODE_ADDCHILD( this, m_pPanelTargetHP);
	}


	{
		m_pSpriteObjectReply = i3Sprite2D::new_object();
		m_pSpriteObjectReply->SetTexture( m_pTexture );
		m_pSpriteObjectReply->Create(HUD_OBJECT_REPLY_COUNT, FALSE, TRUE);
		
		for(i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
		{
			m_pSpriteObjectReply->SetEnable( i, FALSE);
			m_pSpriteObjectReply->SetRect( i, 0.0f, 0.0f, 0.0f, 0.0f);
			m_pSpriteObjectReply->SetColor( i, 0,0,0,255);
		}

		m_pHUDRoot->AddChild(m_pSpriteObjectReply);
	}

	// HUD에서 사용하는 Object들의 이름 및 대화에사용되어집니다.
	{
		for(i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
		{
			m_pTextObjectReply[i] = i3TextNodeDX2::new_object();
			m_pTextObjectReply[i]->Create(GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextObjectReply[i]->SetText("");	
			m_pTextObjectReply[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			m_pTextObjectReply[i]->setAutoWrapEnable( FALSE);
			m_pTextObjectReply[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextObjectReply[i]);		
		}
	}

	// HUD에서 사용하는 캐릭터들의 이름을 설정합니다.
	{
		if(g_pGameContext->IsGM_Observer())
		{
			m_iTextNameSize = GAME_FONT_TARGET_HP_TEXT_SIZE + 4;
		}
		else
		{
			m_iTextNameSize = GAME_FONT_TARGET_HP_TEXT_SIZE;
		}
		
		for(i=0; i<HUD_NAME_MAX_COUNT; i++)
		{
			m_pTextName[i] = i3TextNodeDX2::new_object();
			m_pTextName[i]->Create( GetDefaultFontName(), FONT_COUNT_64, m_iTextNameSize, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextName[i]->SetText("");	
			m_pTextName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pTextName[i]->setAutoWrapEnable( FALSE);
			m_pTextName[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextName[i]);		
		}
	}

	// Radio chat
	{
		for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
		{
			m_pTextRadioChat[ i ] = i3TextNodeDX2::new_object();
			m_pTextRadioChat[ i ]->Create( GetDefaultFontName(), 64, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE);
			m_pTextRadioChat[ i ]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE );
			m_pTextRadioChat[ i ]->SetColor( 80, 255, 60, 255 );

			if( i == 0 )
				m_pTextRadioChat[ i ]->setPos( 20, nScreenHeight - 490 );
			else
				m_pTextRadioChat[ i ]->setPos( 20, nScreenHeight - 480 + (i*13) );
			m_pHUDRoot->AddChild( m_pTextRadioChat[ i ] );
		}
	}

	// 미션 - 폭파미션 관련
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;

		REAL32 fClanOffsetY = 0.0f;

		if( g_pGameContext->GetBattleChannel())
		{
			fClanOffsetY = 18.0f;
		}

		if( pViewer != NULL)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pSpriteBombMission = i3Sprite2D::new_object();	
		m_pSpriteBombMission->SetTexture( m_pTexture );
		m_pSpriteBombMission->Create(HUD_SPRITE_MISSION_BOMB_COUNT, TRUE, TRUE);

		// 폭탄
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE, FALSE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_LARGE, (REAL32)(nPosX-42), (75.0f)+fClanOffsetY, (84.0f), (56.0f));
		m_pSpriteBombMission->SetColor( HUD_SPRITE_MISSION_BOMB_LARGE, 255,255,255,255);
		m_pSpriteBombMission->SetTextureCoord( HUD_SPRITE_MISSION_BOMB_LARGE, 421.0f, 264.0f, 504.0f, 319.0f);

		// 설치된 폭탄 이미지
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, FALSE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, (REAL32)(nPosX-42), (75.0f)+fClanOffsetY, (84.0f), (56.0f));
		m_pSpriteBombMission->SetColor( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, 255,255,255,255);
		m_pSpriteBombMission->SetTextureCoord( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, 421.0f, 205.0f, 504.0f, 258.0f);

		// 폭탄 타이머 BG
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, FALSE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_TIMER_BG, (REAL32)(nPosX-96), (80.0f)+fClanOffsetY, (203.0f), (36.0f));
		m_pSpriteBombMission->SetColor( HUD_SPRITE_MISSION_BOMB_TIMER_BG, 255,255,255,255);
		m_pSpriteBombMission->SetTextureCoord( HUD_SPRITE_MISSION_BOMB_TIMER_BG, 376.0f, 455.0f, 510.0f, 491.0f);

		// 폭탄 게이지 BG
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_GAGE_BG, (REAL32)(nPosX-222), (REAL32)(nScreenHeight-95), (444.0f), (27.0f));
		m_pSpriteBombMission->SetColor( HUD_SPRITE_MISSION_BOMB_GAGE_BG, 255,255,255,255);
		m_pSpriteBombMission->SetTextureCoord( HUD_SPRITE_MISSION_BOMB_GAGE_BG, 535.0f, 98.0f, 536.0f, 99.0f);

		// 폭탄 게이지
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_GAGE, (REAL32)(nPosX-219), (REAL32)(nScreenHeight-92), (439.0f), (21.0f));
		m_pSpriteBombMission->SetColor( HUD_SPRITE_MISSION_BOMB_GAGE, 255,255,255,255);
		m_pSpriteBombMission->SetTextureCoord( HUD_SPRITE_MISSION_BOMB_GAGE, 176.0f, 101.0f, 177.0f, 102.0f);

		m_pSpriteBombMission->SetEnable(HUD_SPRITE_MISSION_BOMB_TOOL, FALSE);
		m_pSpriteBombMission->SetRect(HUD_SPRITE_MISSION_BOMB_TOOL, (REAL32)(nPosX-42), (75.0f)+fClanOffsetY, (33.0f), (33.0f));
		m_pSpriteBombMission->SetColor(HUD_SPRITE_MISSION_BOMB_TOOL, 255, 255, 255, 255);
		m_pSpriteBombMission->SetTextureCoord(HUD_SPRITE_MISSION_BOMB_TOOL, 412.0f, 323.0f, 444.0f, 355.0f);

		m_pHUDRoot->AddChild(m_pSpriteBombMission);
	}
	
	// Kill Text의 배경으로 사용되는 Sprite
	{
		m_pSpriteKillTextBG = i3Sprite2D::new_object();
		m_pSpriteKillTextBG->SetTexture( m_pTexture );
		m_pSpriteKillTextBG->Create(HUD_NOTICE_COUNT, TRUE, TRUE);
		for(i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteKillTextBG->SetEnable( i, FALSE);
			m_pSpriteKillTextBG->SetRect( i, 0.0f, 0.0f, 319.0f, 28.0f);
			m_pSpriteKillTextBG->SetColor( i, 255,255,255,255);
			m_pSpriteKillTextBG->SetTextureCoord( i, 568.0f, 284.0f, 888.0f, 310.0f);
		}
		m_pHUDRoot->AddChild(m_pSpriteKillTextBG);		
	}

	// Kill Mark에 사용되는 Sprite
	{
		for(UINT32 j = 0; j < HUD_NOTICE_COUNT; j++)
		{
			m_pSpriteKillMark[j] = i3Sprite2D::new_object();
			m_pSpriteKillMark[j]->Create(1, TRUE, FALSE);
			m_pSpriteKillMark[j]->SetRect(0, 0.0f, 0.0f, 81.0f, 24.0f);
			m_pSpriteKillMark[j]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pSpriteKillMark[j]);
		}
	}

	// Defence Object 마크
	{
		m_pTextureHUD3 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
		I3ASSERT( m_pTextureHUD3 != NULL);

		m_pSpriteObjectDefence = i3Sprite2D::new_object();
		m_pSpriteObjectDefence->SetTexture( m_pTextureHUD3);
		m_pSpriteObjectDefence->Create( HUD_NOTICE_COUNT, TRUE, TRUE);

		m_pSpriteObjDefenctAndHead = i3Sprite2D::new_object();
		m_pSpriteObjDefenctAndHead->SetTexture( m_pTextureHUD3);
		m_pSpriteObjDefenctAndHead->Create( HUD_NOTICE_COUNT, TRUE, TRUE);

		for( i = 0; i < HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteObjectDefence->SetEnable( i, FALSE);
			m_pSpriteObjectDefence->SetRect( i, 0.f, 0.f, 28.f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteObjectDefence->SetColor( i, 255, 255, 255, 255);
			m_pSpriteObjectDefence->SetTextureCoord( i, 542.0f, 112.0f, 570.0f, 142.0f);

			m_pSpriteObjDefenctAndHead->SetEnable( i, FALSE);
			m_pSpriteObjDefenctAndHead->SetRect( i, 0.f, 0.f, 44.f * i3GuiRoot::getHeightRatioBy768(), 38.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteObjDefenctAndHead->SetColor( i, 255, 255, 255, 255);
			m_pSpriteObjDefenctAndHead->SetTextureCoord( i, 490.0f, 108.0f, 534.0f, 146.0f);
		}

		m_pHUDRoot->AddChild( m_pSpriteObjectDefence);
		m_pHUDRoot->AddChild( m_pSpriteObjDefenctAndHead);
	}
//Key:EscapeMission
	{

		{
			m_pSpriteLostWorldEP5 = i3Sprite2D::new_object();
			m_pSpriteLostWorldEP5->SetTexture( m_pTextureHUD3);
			m_pSpriteLostWorldEP5->Create( HUD_NOTICE_COUNT, TRUE, TRUE);

		
			for( i = 0; i < HUD_NOTICE_COUNT; i++)
			{
				m_pSpriteLostWorldEP5->SetEnable( i, FALSE);
				m_pSpriteLostWorldEP5->SetRect( i, 0.f, 0.f, 28.f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
				m_pSpriteLostWorldEP5->SetColor( i, 255, 255, 255, 255);
				m_pSpriteLostWorldEP5->SetTextureCoord( i, 542.0f, 112.0f, 570.0f, 142.0f);

			}

			m_pHUDRoot->AddChild( m_pSpriteLostWorldEP5);
		}



	}


	// HeadShot에 사용되는 Sprite
	{
		// Head Shot
		m_pSpriteHeadShot = i3Sprite2D::new_object();	
		m_pSpriteHeadShot->SetTexture( m_pTexture );
		m_pSpriteHeadShot->Create(HUD_NOTICE_COUNT, TRUE, TRUE);
		for(i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteHeadShot->SetEnable( i, FALSE);
			m_pSpriteHeadShot->SetRect( i, 0.0f, 0.0f, 55.0f*i3GuiRoot::getHeightRatioBy768(), 37.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteHeadShot->SetColor( i, 255,255,255,200);
			m_pSpriteHeadShot->SetTextureCoord( i, 183.0f, 204.0f, 238.0f, 242.0f);
		}

		m_pHUDRoot->AddChild(m_pSpriteHeadShot);
	}

	// HUD에서 사용하는 Notice를 설정합니다.
	{
		m_pTextDummy = i3TextNodeDX2::new_object();
		m_pTextDummy->Create(  GetDefaultFontName(), 128, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextDummy->SetText("");	
		m_pTextDummy->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
		m_pTextDummy->setSize( 400, 20);
		m_pTextDummy->SetEnable(FALSE);
		
		REAL32 rFontScale = (i3GuiRoot::getHeightRatioBy768() + i3GuiRoot::getHeightRatioBy768()) * 0.5f;
		for(i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pTextNotice[i] = i3TextNodeDX2::new_object();
			m_pTextNotice[i]->Create( GetDefaultFontName(), 128, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextNotice[i]->SetText("");	
			m_pTextNotice[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pTextNotice[i]->setSize( (UINT32)(400*i3GuiRoot::getHeightRatioBy768()), (UINT32)(20*i3GuiRoot::getHeightRatioBy768()));
			m_pTextNotice[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextNotice[i]);		

			m_pTextDeathName[i] = i3TextNodeDX2::new_object();
			m_pTextDeathName[i]->Create( GetDefaultFontName(), 64, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextDeathName[i]->SetText("");	
			m_pTextDeathName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pTextDeathName[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextDeathName[i]);	

			m_pTextKillerName[i] = i3TextNodeDX2::new_object();
			m_pTextKillerName[i]->Create( GetDefaultFontName(), 64, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextKillerName[i]->SetText("");	
			m_pTextKillerName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);	
			m_pTextKillerName[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextKillerName[i]);	

			//Key:EscapeMission
			m_pTextTouchDown[i] = i3TextNodeDX2::new_object();
			m_pTextTouchDown[i]->Create( GetDefaultFontName(), 128, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextTouchDown[i]->SetText("");	
			m_pTextTouchDown[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pTextTouchDown[i]->setSize( (UINT32)(400*i3GuiRoot::getHeightRatioBy768()), (UINT32)(20*i3GuiRoot::getHeightRatioBy768()));
			m_pTextTouchDown[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextTouchDown[i]);		

		}
	}

	// HUD에서 사용하는 기타 텍스트 박스
	{
		m_pTextEtc = i3TextNodeDX2::new_object();
		m_pTextEtc->Create(  GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextEtc->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pTextEtc->setPos( nScreenWidth - 170, nScreenHeight - 143);
		m_pTextEtc->setSize( 200, 50);
		m_pTextEtc->SetColor( 255, 255, 255, 255);	
		m_pHUDRoot->AddChild( m_pTextEtc);		
	}

	// 수류탄 위치 표시
	{
		m_pGrenadeInd = i3Sprite2D::new_object();
		m_pGrenadeInd->SetTexture( m_pTexture);
		m_pGrenadeInd->Create( 2 * MAX_INDICATOR, TRUE, TRUE);

		for( INT32 i = 0; i < MAX_INDICATOR; i++)
		{
			// 첫번째 Sprite : 수류탄 모양
			m_pGrenadeInd->SetEnable(		(i * 2) + 0, FALSE);
			m_pGrenadeInd->SetColor(		(i * 2) + 0, 255, 255, 255, 255);

			if( i < INDICATOR_COUNT_PEROBJ)
			{
				m_pGrenadeInd->SetSize(			(i * 2) + 0, 64.0f, 64.0f);
				m_pGrenadeInd->SetTextureCoord(	(i * 2) + 0, 0.0f, 0.0f, 63.0f, 63.0f);
			}
			else
			{
				m_pGrenadeInd->SetSize(			(i * 2) + 0, 52.0f, 52.0f);
				m_pGrenadeInd->SetTextureCoord(	(i * 2) + 0, 104.0f, 448.0f, 104.0f + 51.0f, 448.0f + 51.0f);
			}


			// 두번째 Sprite : 화살표 모양
			m_pGrenadeInd->SetEnable(		(i * 2) + 1, FALSE);
			m_pGrenadeInd->SetSize(			(i * 2) + 1, 32.0f, 32.0f);
			m_pGrenadeInd->SetColor(		(i * 2) + 1, 255, 255, 255, 255);
			m_pGrenadeInd->SetTextureCoord(	(i * 2) + 1, 0.0f, 64.0f, 32.0f, 64.0f + 32.0f);

			m_pTextGrenade[i] = i3TextNodeDX2::new_object();
			m_pTextGrenade[i]->Create(  GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextGrenade[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			m_pTextGrenade[i]->setPos( nScreenWidth - 140, nScreenHeight - 100);
			m_pTextGrenade[i]->SetColor( 255, 255, 255, 255);	
			m_pHUDRoot->AddChild( m_pTextGrenade[i]);
		}

		m_pHUDRoot->AddChild( m_pGrenadeInd);
	}

	// Hit되었을시에 사용되는 Sprite
	{		
		m_pSpriteHit = i3Sprite2D::new_object();	
		m_pSpriteHit->SetTexture( m_pTexture );
		m_pSpriteHit->Create(HUD_SPRITE_HIT_COUNT, TRUE, TRUE);

	

		// Hit Damage
		{
			// Blind Filter로 준비된 이미지가 없습니다
			// 레이더에 표시되는 아군 동그라미(흰색 점)의 일부분을 임시로 사용하고 있습니다
			m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_DAMAGE, FALSE);
			m_pSpriteHit->SetRect( HUD_SPRITE_HIT_DAMAGE, 0.0f, 0.0f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
			m_pSpriteHit->SetColor( HUD_SPRITE_HIT_DAMAGE, 200,0,0,255);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_HIT_DAMAGE, 290.0f, 70.0f, 292.0f, 72.0f);
		}
		
		// Hit Left Front
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_LEFT_FRONT, TRUE);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_LEFT_FRONT, 0.0f, 0.0f, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_LEFT_FRONT, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_LEFT_FRONT, 245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_LEFT_FRONT, TRUE, FALSE);
		}

		// Hit Right Front
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_RIGHT_FRONT, TRUE);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_RIGHT_FRONT,(REAL32)nScreenWidth/2, 0.0f, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_RIGHT_FRONT, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_RIGHT_FRONT,  245.0f, 208.0f, 404.0f, 328.0f);
		}

		// Hit Left Back
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_LEFT_BACK, TRUE);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_LEFT_BACK, 0.0f, (REAL32)nScreenHeight/2, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_LEFT_BACK, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_LEFT_BACK,  245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_LEFT_BACK, TRUE, TRUE);
		}

		// Hit Right Back
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_RIGHT_BACK, TRUE);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_RIGHT_BACK, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_RIGHT_BACK, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_RIGHT_BACK,  245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_RIGHT_BACK, FALSE, TRUE);
		}	

		// Hit Damage Angle
		{
			for(INT32 i = 0; i < HUD_SPRITE_HIT_ANGLE_MAX; i++)
			{
				m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_ANGLE_START + i, FALSE);
                m_pSpriteHit->SetRect( HUD_SPRITE_HIT_ANGLE_START + i, (REAL32)m_fViewHalfWidth - 80.0f, (REAL32)m_fViewHalfHeight - 202.0f, 160.0f, 202.0f);
				m_pSpriteHit->SetColor( HUD_SPRITE_HIT_ANGLE_START + i, 255,0,0,0);
				m_pSpriteHit->SetTextureCoord( HUD_SPRITE_HIT_ANGLE_START + i, 1.0f, 203.0f, 161.0f, 405.0f);

				m_fDamageDirectionAngle[i] = 0.0f;
			}

			m_idxDamageDirectionOrder = 0;
		}
 
		m_pHUDRoot->AddChild(m_pSpriteHit);
	}

	// 미션관련
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		if( pViewer != NULL)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pSpriteMission = i3Sprite2D::new_object();	
		m_pSpriteMission->SetTexture( m_pTexture );
		m_pSpriteMission->Create( HUD_SPRITE_MISSION_COUNT, TRUE, TRUE);

		// Mission message BG
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, FALSE);
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_MESSAGE_BG, 255,255,255,255);


		if( g_pGameContext->IsDominationMode() )
			m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_MESSAGE_BG, (REAL32)(nPosX-200), (REAL32)(nScreenHeight-220), (401.0f), (67.0f));
		else

			m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_MESSAGE_BG, (REAL32)(nPosX-200), (REAL32)(nScreenHeight-170), (401.0f), (67.0f));

		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_MESSAGE_BG, 531.0f, 14.0f, 532.0f, 15.0f);

		// Mission결과시 Team
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, FALSE);		
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_TEAM, 255,255,255,255);
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 0.0f, 0.0f, 0.0f, 0.0f);

		// Mission결과시 승패여부
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, FALSE);		
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_RESULT, 255,255,255,255);
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 0.0f, 0.0f, 0.0f, 0.0f);


		m_pHUDRoot->AddChild(m_pSpriteMission);
	}

	// HUD에서 사용하는 폭탄관련 텍스트 박스
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;

		INT32 nClanOffsetY = 0;

		if( g_pGameContext->GetBattleChannel())
		{
			nClanOffsetY = 18;
		}

		if( pViewer != NULL)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pTextBomb[HUD_BOMB_TEXT_TIME] = i3TextNodeDX2::new_object();
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->Create(  GetDefaultFontName(), 16, GAME_FONT_SIZE_HUD_BOMB_TIME, FW_BOLD, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->setPos(nPosX-26, 80+nClanOffsetY);
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->setSize(133, 36);

		for(i=0; i<HUD_BOMB_TEXT_COUNT; i++)
		{
			m_pTextBomb[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			//m_pTextBomb[i]->setShadowEnable( TRUE, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextBomb[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextBomb[i]);		
		}
		
	}

	// Hud에서 사용되는 미션관련 텍스트 박스
	{
		COLOR col;
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		if( pViewer != NULL)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pTextMission[HUD_MISSION_TEXT_MESSAGE] = i3TextNodeDX2::new_object();
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->Create(  GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_HUD_MISSION_MESSAGE, FW_BOLD, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);

		if( g_pGameContext->IsDominationMode() )
			m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->setPos(nPosX-300, nScreenHeight-220);
		else
			m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->setPos(nPosX-300, nScreenHeight-170);
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->setSize(600, 67);

		m_pTextMission[HUD_MISSION_TEXT_NOTICE] = i3TextNodeDX2::new_object();
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->Create(  GetDefaultFontName(), FONT_COUNT_128, GAME_FONT_SIZE_HUD_MISSION_NOTICE, FW_BOLD, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setPos(nPosX-300, 130);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setSize(600, 36);
		i3Color::Set( &col, (UINT8) 240, 210, 110, 255);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetColor( &col);

		for(i=0; i<HUD_MISSION_TEXT_COUNT; i++)
		{
			m_pTextMission[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);					
			m_pTextMission[i]->SetEnable(FALSE);
			m_pHUDRoot->AddChild(m_pTextMission[i]);		
		}
	}

	// ObserverMode
	{
		m_pSpriteObserver = i3Sprite2D::new_object();
		m_pSpriteObserver->SetTexture( m_pTexture );
		m_pSpriteObserver->Create(1, TRUE, TRUE);

		m_pSpriteObserver->SetEnable(0, FALSE);
		m_pSpriteObserver->SetRect(0, 0.0f, (REAL32)nScreenHeight - 100.0f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
		m_pSpriteObserver->SetColor(0, 0, 0, 0, 150);
		m_pSpriteObserver->SetTextureCoord(0, 603.0f, 1.0f, 812.0f, 59.0f);		
		m_pSpriteObserver->SetEnable(FALSE);

		m_pHUDRoot->AddChild(m_pSpriteObserver);

		for(i = 0; i < HUD_TEXT_OBSERVER_FLY_COUNT; i++)
		{
			m_pTextObserverFly[i] = i3TextNodeDX2::new_object();
			m_pTextObserverFly[i]->Create(  GetDefaultFontName(), FONT_COUNT_64, 8);

			if(i == HUD_TEXT_OBSERVER_FLY_MOVE_SPEED) {
				m_pTextObserverFly[i]->SetText("Move Speed");
			}
			/*else if(i == HUD_TEXT_OBSERVER_FLY_ROTATE_SPEED) {
				m_pTextObserverFly[i]->SetText("Rotate Speed");
			}*/
			else {
				I3ASSERT_0;
			}

			m_pTextObserverFly[i]->setSize(400, 20);
			m_pTextObserverFly[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			m_pTextObserverFly[i]->SetColor(255, 255, 255, 255);
			m_pTextObserverFly[i]->SetEnable(FALSE);
			
			m_pHUDRoot->AddChild(m_pTextObserverFly[i]);
		}

		m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_MOVE_SPEED]->setPos( (UINT32)30.f, (UINT32)130.f);
		//m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_ROTATE_SPEED]->setPos( 30.f, 150.f);

		for(i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++)
		{
			m_pTextObserver[i] = i3TextNodeDX2::new_object();
			
			if( HUD_TEXT_OBSERVER_ID == i )
			{
				if(g_pGameContext->IsGM_Observer())
				{
					m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_64, 25, FW_BOLD);				
					m_pTextObserver[i]->setSize(400, 40);
				}
				else
				{
					m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_64, 15, FW_BOLD);				
					m_pTextObserver[i]->setSize(400, 20);
				}
				
			}
			else
			{
				m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_64, 13);
				m_pTextObserver[i]->setSize(400, 20);
			}

			{
				char observerText[MAX_STRING_COUNT];

				INT32 arTemp[HUD_TEXT_OBSERVER_COUNT];

				arTemp[HUD_TEXT_OBSERVER_ID] = GET_GAME_STRING_HANDLE("STBL_IDX_OBSERVER_ID");
				arTemp[HUD_TEXT_OBSERVER_NEXT] = GET_GAME_STRING_HANDLE("STBL_IDX_OBSERVER_NEXT");
				arTemp[HUD_TEXT_OBSERVER_PREV] = GET_GAME_STRING_HANDLE("STBL_IDX_OBSERVER_PREV");
				arTemp[HUD_TEXT_OBSERVER_WEAPON] = GET_GAME_STRING_HANDLE("STBL_IDX_OBSERVER_WEAPON");
				arTemp[HUD_TEXT_OBSERVER_VIEW] = GET_GAME_STRING_HANDLE("STBL_IDX_OBSERVER_VIEW");
				
				if (HUD_TEXT_OBSERVER_WEAPON == i)
				{					
					// 무기교체키는 환경설정에서 변경될 수 있어 별도 처리한다
					GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

					sprintf_s(observerText, "%s %s", 
							GAME_STRING("STBL_IDX_OBSERVER_WEAPON"),
							GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_CHANGEWEAPON]) );
				}
				else
				{
					i3String::Copy(observerText, GAME_STRING_BY_HANDLE(arTemp[i]), MAX_STRING_COUNT );
				}

				m_pTextObserver[i]->SetText(observerText);
			}

			
			m_pTextObserver[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			m_pTextObserver[i]->SetColor(255, 255, 255, 255);
			m_pTextObserver[i]->SetEnable(FALSE);
			
			m_pHUDRoot->AddChild(m_pTextObserver[i]);
		}
				
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		POINT2D pt = g_pFramework->GetCenterPosition(400, 100);
		
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->setPos( pt.x, nScreenHeight - 90);
		m_pTextObserver[HUD_TEXT_OBSERVER_NEXT]->setPos( 30, nScreenHeight - 60);
		m_pTextObserver[HUD_TEXT_OBSERVER_PREV]->setPos( 30, nScreenHeight - 40);
		m_pTextObserver[HUD_TEXT_OBSERVER_WEAPON]->setPos( nScreenWidth - 200 - 30, nScreenHeight - 60);
		m_pTextObserver[HUD_TEXT_OBSERVER_VIEW]->setPos( nScreenWidth - 200 - 30, nScreenHeight - 40);

		// Observer HP Gauge
		m_pObserverHPGaugeHUD = CHUD_ObserverHPGauge::new_object();
		m_pObserverHPGaugeHUD->Create(this);
		m_pObserverHPGaugeHUD->SetEnableGauge( FALSE);
		AddChild( m_pObserverHPGaugeHUD);

		m_bEnableObserverMode = FALSE;
	}

	// 외부 클래스 HUD 초기화
	// ScoreBoard 밑에 표시되어야한다
	{
		CreateMiniMap(m_pHUDRoot);

		// ChainKills 생성
		m_pChainKills = CChainKills::new_object();
		m_pChainKills->Create( m_pHUDRoot);
		I3_GAMENODE_ADDCHILD(this, m_pChainKills);
	}

	{
		m_pPanelScore = CHUD_Score::new_object();
		m_pPanelScore->Create( this);
	}

	// HUD Helper
	m_pTextureHelper = crb::i()->LoadHudTexture(HUD_TEXTURE_HELP);
	m_pSpriteHelper = i3Sprite2D::new_object();

	m_pSpriteHelper->Create( 1, TRUE, TRUE );
	m_pSpriteHelper->SetTexture( m_pTextureHelper );
	m_pSpriteHelper->SetEnable( FALSE);
	m_pSpriteHelper->SetRect( 0, (REAL32)(nScreenWidth/2 - 326), (REAL32)(nScreenHeight/2 - 256), (652.f), (512.0f));
	m_pSpriteHelper->SetColor( 0, 255,255,255,255);
	m_pSpriteHelper->SetTextureCoord( 0, (0.f), (0.f), (652.0f), (512.0f));
	m_pHUDRoot->AddChild( m_pSpriteHelper);
}

void CHud::CreateGrenadeLauncherCrossHair( void)
{
	i3_prof_func();

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nHalfScreenWidth	= pViewer->GetViewWidth() / 2;
	INT32	nHalfScreenHeight	= pViewer->GetViewHeight() / 2;

	m_pTextureGrenadeLauncherCrossHair	= crb::i()->LoadHudTexture( HUD_TEXTURE_GRENADELAUNCHER_CROSSHAIR);
	UINT32 nImageWidth = m_pTextureGrenadeLauncherCrossHair->GetWidth();
	UINT32 nImageHeight = m_pTextureGrenadeLauncherCrossHair->GetHeight();
	
	m_pSpriteGrenadeLauncherCrossHair = i3Sprite2D::new_object();	
	m_pSpriteGrenadeLauncherCrossHair->SetTexture( m_pTextureGrenadeLauncherCrossHair );
	m_pSpriteGrenadeLauncherCrossHair->Create( 1, TRUE, FALSE);
	m_pSpriteGrenadeLauncherCrossHair->SetEnable(TRUE);	
	m_pSpriteGrenadeLauncherCrossHair->SetTextureCoord( 0, 0.0f, 0.0f, (REAL32)nImageWidth, (REAL32)nImageHeight);
	
	m_pSpriteGrenadeLauncherCrossHair->SetRect( 0, (REAL32)(nHalfScreenWidth - nImageWidth/2), (REAL32)(nHalfScreenHeight - nImageHeight/2),
		(REAL32)(nImageWidth), (REAL32)(nImageHeight));
	
	m_pHUDRoot->AddChild( m_pSpriteGrenadeLauncherCrossHair);

	SetEnableGrenadeLauncherCrossHair( FALSE);
}

void CHud::CreateRideHelicopter(void)
{
	i3_prof_func();

	m_pRideHelicopter = CHud_Helicopter::new_object();
	m_pRideHelicopter->Create(this);
	AddChild( m_pRideHelicopter);
}

void CHud::CreateWarnningHack(void)
{
	i3_prof_func();

	m_pWarnningHack = CHud_WarnningHack::new_object();
	m_pWarnningHack->Create(this);
	AddChild( m_pWarnningHack);

	m_pWarnningHack->SetEnable( TRUE);
}

void CHud::OnUpdate( REAL32 rDeltaSeconds )
{
	i3_prof_func();

 	i3GameObjBase::OnUpdate( rDeltaSeconds );

	//SetDamageDirection(0.0f);

	//	Key State
	UINT64 nPressKey	= g_pFramework->getKeyPress();	

//	if( !m_bOnlyNoticeUpdate)
	{	
		// Radio chat
		UpdateRadioChat( rDeltaSeconds );

		// Challange board
		if( !g_pEnvSet->m_bChallange)
		{
			SetEnableChallengeBoard( FALSE);
		}

		// BLOOM
#if defined(BLOOM_ENABLE)
		if( g_pEnvSet->m_bEnableBloom)
		{
			if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_NUMPADPLUS))
			{
				REAL32 fBloomDispersion = g_pFramework->getBloomExtract()->getThreshold();
				g_pFramework->getBloomExtract()->setThreshold( fBloomDispersion + 0.001f);
				
				char szNotice[MAX_PATH];
				sprintf_s( szNotice, GAME_STRING("STR_TBL_INGAME_HUD_BLOOM_TEXT"), fBloomDispersion + 0.001f);/*Bloom강도 %.3f*/
				SetEtcText( szNotice);
			}
			else if( g_pFramework->getKeyboard()->GetPressState( I3I_KEY_NUMPADMINUS))
			{
				REAL32 fBloomDispersion = g_pFramework->getBloomExtract()->getThreshold();
				g_pFramework->getBloomExtract()->setThreshold( fBloomDispersion - 0.001f);

				char szNotice[MAX_PATH];
				sprintf_s( szNotice, GAME_STRING("STR_TBL_INGAME_HUD_BLOOM_TEXT"), fBloomDispersion - 0.001f);/*Bloom강도 %.3f*/
				SetEtcText( szNotice);
			}
		}
#endif
		
		// Mouse sense
		if( nPressKey & GAME_KEY_CMD_MOUSESENE_DOWN )
		{
			if( m_fEtcMessageLocalTime > 0.3f || m_fEtcMessageLocalTime == 0.0f)
			{
				g_pEnvSet->m_rMouseSensivity -= 0.005f;
				if( g_pEnvSet->m_rMouseSensivity <= MIN_MOUSE_SENSIVITY)		g_pEnvSet->m_rMouseSensivity = MIN_MOUSE_SENSIVITY;

				char szNotice[MAX_PATH];
				sprintf_s( szNotice, "%s   %.3f", GAME_STRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
				SetEtcText( szNotice);
			}

			m_fEtcMessageLocalTime += rDeltaSeconds;
		}
		else if( nPressKey & GAME_KEY_CMD_MOUSESENE_UP )
		{			
			if( m_fEtcMessageLocalTime > 0.3f || m_fEtcMessageLocalTime == 0.0f)
			{
				g_pEnvSet->m_rMouseSensivity += 0.005f;
				if( g_pEnvSet->m_rMouseSensivity > MAX_MOUSE_SENSIVITY)		g_pEnvSet->m_rMouseSensivity = MAX_MOUSE_SENSIVITY;

				char szNotice[MAX_PATH];
				sprintf_s( szNotice, "%s   %.3f", GAME_STRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
				SetEtcText( szNotice);
			}

			m_fEtcMessageLocalTime += rDeltaSeconds;
		}
		else
		{
			m_fEtcMessageLocalTime = 0.0f;
		}

		// 상대방에게 공격받았을때의 HUD를 Update합니다.
		UpdateDamageDirection(rDeltaSeconds);
	}

	// 수류탄 경고
	if( !m_bGameEnd)
	{
		UpdateIndicator( rDeltaSeconds);
	}

	if ( g_pGameContext->IsStageMaskTraining() )
	{
		// 훈련전이거나 제압모드의 경우 개인 경쟁점수를 항시 표시합니다.(단 2명이상일 경우)
		UpdateTrainingCompetition(rDeltaSeconds);
	}

	//
	if(m_bEnableRevengeUpdate)
	{
		UpdateRevenge(rDeltaSeconds);
	}

	if( !m_bGameEnd)
	{
		if( m_pPanelTargetHP != NULL)
			m_pPanelTargetHP->Update( rDeltaSeconds);
	}

	// EtcText Update
	if (m_pTextEtc)
	{
		if( m_pTextEtc->GetEnable())
		{
			UpDateEtcText( rDeltaSeconds);
		}
	}

	// 상대방을 죽였을때 나오는 메시지입니다.
	if(m_bUpdateNotice)
	{
		UpDateNoticeText(rDeltaSeconds);
	}

	// 폭탄 설치
	if( g_pGameContext->IsBombMissionMode() || g_pGameContext->IsTutorialMode() )
	{
		ProcessBomb( rDeltaSeconds);

		if( g_pGameContext->getMissionManager()->getMissionState() == GAME_MISSION_STATE_INSTALLED  && !g_pGameContext->IsDominationMode())
		{
			UpdateBomb( rDeltaSeconds);
		}
	}

	// Mission Message Update
	UpdateMissionMessage( rDeltaSeconds);

	// 플레이어 자신이 죽었을때 나오는 스프라이트 입니다.
	if( m_bEnableDeathSprite)
	{
		UpdateDeathSprite( rDeltaSeconds);		
	}

	// Helper 업데이트
	UpdateHelper();


	// 피격필터
	OnUpdateDamageFilter(rDeltaSeconds);

	// chat notice
	_ProcessChatNotice();

	if( m_pScope != NULL)
		m_pScope->UpdateSniperScope( rDeltaSeconds);

	// 자유 카메라 
	if(m_bEnableObserverFlyMode) {
		if(i3::same_of<CGameCamera_FLY*>(g_pCamera)) //->IsExactTypeOf(CGameCamera_FLY::static_meta())) 
		{
			char szTemp[256];

			StringCchPrintf(szTemp, 256, "Move Speed %.3f", ((CGameCamera_FLY*)g_pCamera)->getMoveSpeed());
			m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_MOVE_SPEED]->SetText(szTemp);

			/*StringCchPrintf(szTemp, 256, "Rotate Speed %.3f", ((CGameCamera_FLY*)g_pCamera)->getRotateSpeed());
			m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_ROTATE_SPEED]->SetText(szTemp);*/
		}
	}

#if defined( ENABLE_TEST_COMMAND )
	if( g_pFramework->getCommandSet()->IsShowPos())
	{
		char szPos[100];
		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( g_pGameContext->getMySlotIdx());
		if( pPlayer != NULL)
		{
			VEC3D *pos = pPlayer->GetPos();
			sprintf_s( szPos, "Player position: X(%.4f)\n, Y(%.4f)\n, Z(%.4f)\n", i3Vector::GetX(pos), i3Vector::GetY(pos), i3Vector::GetZ(pos));
			SetMissionMessageString( szPos);
		}
	}
#if defined(SKILLSYSTEM)
#if defined(SKILL_VIEWER)
	if( g_pFramework->getCommandSet()->isShowSkill())
	{
		UIBattleFrame::i()->EnableHUD(UIHUD_SKILL_VIEW, TRUE);

		SkillManager * pSkill = g_pGameContext->getSkillManager();
		if( pSkill == NULL)			return;
		NET_CHARA_INFO * pNetCharaInfo = g_pGameContext->getNetCharaInfo(g_pGameContext->getMySlotIdx());
		if( pNetCharaInfo == NULL)	return;
		CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(g_pGameContext->getMySlotIdx());
		if(pChara == NULL)			return;
		CWeaponInfo * pWeapon = pChara->GetCurrentCharaWeapon()->getWeaponInfo();
		if(pWeapon == NULL)			return;
		SKILL_APPLY_INFO * _skillApply = NULL;

		WEAPON_SLOT_TYPE _SlotType = pWeapon->GetTypeUsage();
		UINT8 _CurrentAvatar = pNetCharaInfo->getCurrentCharaIndex();
		WEAPON_CLASS_TYPE _WeaponType = pWeapon->GetTypeClass();
		INT32 _type = 0;

		UIHUDSkillViewer * viewer = (UIHUDSkillViewer *)UIBattleFrame::i()->GetHUD(UIHUD_SKILL_VIEW);
		I3ASSERT(viewer);
		viewer->ClearList();

		if(_SlotType == WEAPON_SLOT_PRIMARY)
		{
			_skillApply = pNetCharaInfo->getSkillApply( SKILL_TYPE_PRIMARY);
			_type = 0;
		}
		else if(_SlotType >= WEAPON_SLOT_SECONDARY && _SlotType < WEAPON_SLOT_THROWING2)
		{
			_skillApply = pNetCharaInfo->getSkillApply( SKILL_TYPE_SECONDARY);
			_type = 1;
		}
		else
			return;

		INT32 _skillCount = pSkill->getMaxSkillCountByClass(_skillApply->getCurrentClass());

		if(_skillApply->getCurrentClass() == pSkill->getWeaponTypeToSkill(_WeaponType))
		{
			//getMaxSkillCountByClass
			viewer->setTitleText("Skill is Activated.");
			
			for(INT32 i=0; i < _skillCount; i++)
			{
				SkillManager::SLOT_DIRECTION dir = (SkillManager::SLOT_DIRECTION)(pSkill->GetSlotState((INT32)_CurrentAvatar, _type, i) - 1);
				if( dir == -1 || dir == 2)
				{
					viewer->setSkillText(i,"","Not Acquire Slot","","");
				}
				else
				{
					INT32 _SkillClass = pSkill->getCurrentClass((INT32)_CurrentAvatar, _type);
					const INT32 _Skill = pSkill->getSkillByClass(_SkillClass,dir,i);
					SKILL_APPLY_TYPE _ApplyType = static_cast<SKILL_APPLY_TYPE>(_Skill);

					char SkillNum[MAX_STRING_COUNT] = {0,};
					char SkillValue[MAX_STRING_COUNT] = {0,};
					char *SkillName	= (char *)pSkill->getSkillName((INT32)_Skill);
					char *SkillSubName = (char *)pSkill->getSkillSubName((INT32)_Skill);
					sprintf_s(SkillNum, "%d", _Skill);
					sprintf_s(SkillValue, "%f", _skillApply->getSkillValue(_ApplyType, _SkillClass));
					viewer->setSkillText(i,SkillNum,SkillName,SkillSubName,SkillValue);
				}
			}
		}
		else
		{
			if(_skillApply->getCurrentClass() == 255)
			{
				viewer->setTitleText("You do not have Skill.(Select Skill in MyInfo)");
				for(INT32 i=0; i < _skillCount; i++)
				{
					viewer->setSkillText(i,"","","","");
				}
			}
			else
			{
				viewer->setTitleText("Skill Type is not matched.");
				for(INT32 i=0; i < _skillCount; i++)
				{
					viewer->setSkillText(i,"","","","");
				}
			}
			
		}
	}
	else
	{
		UIBattleFrame::i()->EnableHUD(UIHUD_SKILL_VIEW, FALSE);
	}
#endif
#endif
#endif

}

BOOL CHud::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	i3_prof_func();

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

	#if 0	
			// 스크린샷 촬영을 위한 HUD 비활성화 토클기능입니다.
			if (pKeyboard->GetStrokeState(I3I_KEY_F3))
			{
				if( g_pFramework->Get2DRoot()->IsFlag( I3_NODEFLAG_DISABLE))
				{
					g_pFramework->Get2DRoot()->RemoveFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
				}
				else
				{
					g_pFramework->Get2DRoot()->SetFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_INVISIBLE);
				}
			}
	#endif

			if (GetChatEnable())
			{
				if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
				{
					SetChatEnable(FALSE);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

void CHud::InitHud()
{
	i3_prof_func();

	m_fBulletScale = i3Math::div(BULLET_MAX, 100.0f);	
	m_bUpdateNotice = FALSE;

	if( m_pPanelTargetHP != NULL)
		m_pPanelTargetHP->Init();
}

void CHud::ResetHud(void)
{
	i3_prof_func();
	// HUD의 Reset작업을 수행합니다. 라운드 시작 및 게임 시작시 호출됨.
	// StageBattle::ProcessGameContext()에서 EVENT_START_BATTLE, EVENT_M_ROUNDSTART_BATTLE단계 수행시 호출됩니다.	
	if( m_pPanelTargetHP != NULL)
		m_pPanelTargetHP->Reset();
}

void CHud::UpdateHelper( void )
{
	i3_prof_func();

	if( g_pFramework ->getKeyboard()->GetPressState( I3I_KEY_F1 ) )
	{
		if( !m_bUpdateHelper )
		{
			if (m_pSpriteHelper)
				m_pSpriteHelper->SetEnable( TRUE );
			m_bUpdateHelper = TRUE;
		}
	}
	else
	{
		if( m_bUpdateHelper )
		{
			if (m_pSpriteHelper)
				m_pSpriteHelper->SetEnable( FALSE );
			m_bUpdateHelper = FALSE;
		}
	}
}

void CHud::SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	i3_prof_func();

	INT32 i;

	m_pChainKills->SetEnableWithoutNotice( bFlag );

	if( m_pPanelTargetHP != NULL)
		m_pPanelTargetHP->SetEnableWithoutNotice( bFlag, nState, bDeath);

	m_bOnlyNoticeUpdate = ! bFlag;

	if( bFlag)
	{
		if( g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode() )
		{
			SetEnableBombMission(TRUE);
		}

		m_bOneTimeAreaUpdate = FALSE;
	}
	else
	{
		m_pSpriteMission->SetEnable( FALSE);
		m_pSpriteBoss->SetEnable( FALSE);
		m_pSpriteBombMission->SetEnable( FALSE);

		for( i=0; i<HUD_BOMB_TEXT_COUNT; i++) m_pTextBomb[i]->SetEnable( FALSE);
		for( i=0; i<HUD_MISSION_TEXT_COUNT; i++) m_pTextMission[i]->SetEnable( FALSE);
	}
}

void CHud::SetHudMode( HUD_MODE mod)
{
	i3_prof_func();

	// 일반 HUD로 복귀하기 위해, DEFAULT 상태를 연속 호출하는 것을 막는다.
	// 다른 모드의 경우 매프레임 호출을 허용합니다.
	if (mod == HUD_MODE_DEFAULT && m_HudMode == mod)
	{
		return;
	}

	m_HudMode = mod;

	SetEnableObserverFly(FALSE);

	switch( mod)
	{
	case HUD_MODE_DEFAULT:
		{
			SetEnableObserver(FALSE);			
			SetEnableMiniMap( true);
			SetEnableChallengeBoard(TRUE);

			if( m_pPanelTargetHP != NULL)
				m_pPanelTargetHP->SetEnableTargetHP( TRUE);

			HUD::instance()->SetParameter( m_UItypelist, HUD::MISSION_DEFAULT );
			UIHUDNoticeShop::instance()->SetObserverMode(false);
		}
		break;
	case HUD_MODE_MISSION_ROUND_START:
		{
			SetEnableObserver(FALSE);
			UpdateScore( FALSE);
			SetEnableMiniMap( true);
			if( m_pMiniMap != NULL)
				m_pMiniMap->SetMiniMapMode( CBattleHUD_MiniMap::MINIMAP_MODE_DEFAULT );

			SetEnableChallengeBoard(TRUE);

			if( m_pPanelTargetHP != NULL)
				m_pPanelTargetHP->SetEnableTargetHP( TRUE);

			SetRevenge( FALSE);
		}
		break;
	case HUD_MODE_MISSION_SCORE:
		{
			UpdateScore(TRUE);
			SetEnableBombMission(FALSE);
			SetEnableMissionResult( FALSE);
			SetEnableMissionMessage( FALSE);
			SetEnableObserver(FALSE);
			SetEnableMiniMap( false);
			SetEnableChallengeBoard(FALSE);

			if( m_pPanelTargetHP != NULL)
				m_pPanelTargetHP->SetEnableTargetHP( FALSE);

			SetRevenge( FALSE);

			if( CBattleGui::i()->GetEnablePopup())
			{
				CBattleGui::i()->SetEnablePopup();
			}
		}
		break;
	case HUD_MODE_MISSION_OBSERVER:
		{
			SetEnableObserver(TRUE);
			SetEnableMiniMap( true);
			SetEnableChallengeBoard(FALSE);

			if( m_pPanelTargetHP != NULL)
				m_pPanelTargetHP->SetEnableTargetHP( FALSE);

			HUD::instance()->SetParameter( m_UItypelist, HUD::MISSION_OBSERVER );
			UIHUDNoticeShop::instance()->SetObserverMode(true);
		}
		break;
	case HUD_MODE_MISSION_FLY:
		{
			SetEnableBombMission(FALSE);
			SetEnableMissionResult( FALSE);
			SetEnableMissionMessage( FALSE);
			SetEnableObserver(FALSE);
			SetEnableMiniMap( false);
			SetEnableChallengeBoard(FALSE);
			SetEnableObserverFly(TRUE);

			if( m_pPanelTargetHP != NULL)
				m_pPanelTargetHP->SetEnableTargetHP( FALSE);			

			SetRevenge( FALSE);
		}
		break;
	default:
		break;
	}
}

void CHud::SetMissionMessageString(const char* strMessage)
{
	i3_prof_func();

	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, TRUE);
	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( TRUE);
	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetText((char*)strMessage);

	// Mission Message Time초기화를 합니다.
	m_fMissionMessageTime = 0.0f;
}

void CHud::SetMissionNoticeString( const char* strMessage, REAL32 rLifeTime, INT32 nOffsetX, INT32 nOffsetY)
{
	i3_prof_func();

	INT32 nPosX = (INT32)m_fViewHalfWidth;
	INT32 iClanOffsetY = 0;

	// Clan mark가 있으면 아래쪽으로 표시
	if( g_pGameContext->GetBattleChannel())
		iClanOffsetY = 18;

	if( g_pGameContext->IsDefenceMissionMode() )
		iClanOffsetY += 30;

	m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setPos(nPosX - 300 + nOffsetX, 130 + nOffsetY + iClanOffsetY);
	m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( TRUE);
	m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetText((char*)strMessage);

	// Mission Notice Time초기화를 합니다.
	m_fMissionNoticeTimer = 0.0f;
	m_fMissionNoticeLifeTime = rLifeTime;
}

void CHud::SetEnableMissionMessage( BOOL bFlag)
{
	i3_prof_func();

	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( bFlag);
	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, bFlag);
}

void CHud::SetEnableMissionNotice( BOOL bFlag)
{
	i3_prof_func();

	m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( bFlag);
}

void CHud::SetEnableMissionResult( BOOL bFlag)
{
	i3_prof_func();

	// 튜토리얼 모드에서는 표시하지 않는다.
	if(g_pGameContext->IsTutorialMode() == false
		&& g_pGameContext->IsBossMissionMode() == false)
	{
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, bFlag);
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, bFlag);
		if(g_pGameContext->IsBossMissionMode()) {
			m_pSpriteBoss->SetEnable( 0, bFlag);
		}
	}
}

void CHud::SetRedRoundResult(INT32 nPosX, INT32 nPosY)
{
	i3_prof_func();

	REAL32 positionX = 0.f;
	REAL32 positionY = 0.f;

	if(g_pGameContext->IsBossMissionMode()) {
		positionX = (REAL32)(nPosX-(364.0f*0.5f)) - 65.0f;
		positionY = (REAL32)(nPosY-(57.0f*0.5f)) - 10.0f;

		// Boss
		m_pSpriteBoss->SetRect( 0, positionX, positionY, 162.0f, 57.0f);
		m_pSpriteBoss->SetTextureCoord2( 0, 716.f, 398.0f, 162.0f, 57.0f);

#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+198.f, positionY, 153.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_RESULT, 864.0f, 456.0f, 153.0f, 53.0f);

		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM,  positionX+178.f+95.f, positionY, 179.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_TEAM, 657.0f, 338.0f, 179.0f, 53.0f);
#else
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+178.f , positionY, 179.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_TEAM, 657.0f, 338.0f, 179.0f, 53.0f);

		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+178.f+195.f, positionY, 153.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_RESULT, 864.0f, 456.0f, 153.0f, 53.0f);
#endif
	}
	else {
		positionX = (REAL32)(nPosX - (325.0f*0.5f)) - 75.0f;
		positionY = (REAL32)(nPosY - (57.0f*0.5f)) - 10.0f;

#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+(325.0f*0.5f)+130.0f, positionY, 325.0f, 57.0f);		
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 335.0f, 838.0f, 392.0f);

		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+20.f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 862.0f, 453.0f, 1019.0f, 510.0f);
#else
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX, positionY, 325.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 335.0f, 838.0f, 392.0f);
#if defined (LOCALE_CHINA)
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(325.0f*0.5f)+40.0f + 70.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 862.0f, 453.0f, 1019.0f, 510.0f);
#else
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(325.0f*0.5f)+30.0f + 157.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 862.0f, 453.0f, 1019.0f, 510.0f);
#endif
#endif
	}
}

void CHud::SetBlueRoundResult(INT32 nPosX, INT32 nPosY)
{
	i3_prof_func();

	REAL32 positionX = 0.f;
	REAL32 positionY = 0.f;

	if(g_pGameContext->IsBossMissionMode()) {
		positionX = (REAL32)(nPosX-(371.0f*0.5f)) - 65.0f;
		positionY = (REAL32)(nPosY-(57.0f*0.5f)) - 10.0f;

		// Union
		m_pSpriteBoss->SetRect( 0, positionX, positionY, 170.0f, 53.0f);
		m_pSpriteBoss->SetTextureCoord2( 0, 716.0f, 457.0f, 170.0f, 53.0f);

#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+206.f, positionY, 152.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_RESULT, 707.0f, 455.0f, 152.0f, 53.0f);

		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+186.f+134.f, positionY, 178.0f, 54.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_TEAM, 697.0f, 397.0f, 178.0f, 54.0f);
#else
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+186.f , positionY, 178.0f, 54.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_TEAM, 697.0f, 397.0f, 178.0f, 54.0f);

		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+186.f+194.f, positionY, 152.0f, 53.0f);
		m_pSpriteMission->SetTextureCoord2( HUD_SPRITE_MISSION_RESULT, 707.0f, 455.0f, 152.0f, 53.0f);
#endif
	}
	else {
		positionX = (REAL32)(nPosX-(364.0f*0.5f)) - 65.0f;
		positionY = (REAL32)(nPosY-(57.0f*0.5f)) - 10.0f;

#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+(325.0f*0.5f)+130.0f, positionY, 325.0f, 57.0f);		
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 395.0f, 877.0f, 452.0f);

		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+20.f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 704.0f, 453.0f, 861.0f, 510.0f);
#else
		// Team
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX , positionY, 364.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 395.0f, 877.0f, 452.0f);
#if defined (LOCALE_CHINA)		//	터키의 경우 팀과 결과 어순을 바꾼다.
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(364.0f*0.5f)+40.0f+70.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 704.0f, 453.0f, 861.0f, 510.0f);
#else
	// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(364.0f*0.5f)+30.0f+157.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 704.0f, 453.0f, 861.0f, 510.0f);
#endif
#endif
	}
}

void CHud::SetMissionRoundResult( INT32 nWinTeamIdx)
{
	i3_prof_func();

	if(g_pGameContext->IsTutorialMode()) 
	{
		return;
	}

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;
	INT32	nScreenHeight	= 0;

	if( pViewer != NULL)
	{
		nScreenWidth	= pViewer->GetViewWidth();
		nScreenHeight	= pViewer->GetViewHeight();
	}

	INT32 nPosX = 0;
	INT32 nPosY = 0;

	nPosX = (INT32)(nScreenWidth * 0.5f);
	nPosY = (INT32)(nScreenHeight * 0.5f);

	switch( nWinTeamIdx )//성백 추후 인덱스 변경해야함
	{
	case TEAM_RED_WIN:
		{
			SetRedRoundResult(nPosX, nPosY);
		}
		break;
	case TEAM_BLUE_WIN:
		{
			SetBlueRoundResult(nPosX, nPosY);
		}
		break;
	case TEAM_DRAW:	// Draw
		{
			REAL32 positionX = 0.0f;
			REAL32 positionY = 0.0f;

			// Red
			if (0 == g_pGameContext->getMyTeam() - 1)
			{
				positionX = (REAL32)(nPosX - (325.0f*0.5f)) - 75.0f;
				positionY = (REAL32)(nPosY - (57.0f*0.5f)) - 10.0f;

				// Team
#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
				m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+(325.0f*0.5f)+130.0f, positionY, 325.0f, 57.0f);	
				m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 335.0f, 838.0f, 392.0f);
#else
				m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX, positionY, 325.0f, 57.0f);
				m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 335.0f, 838.0f, 392.0f);
#endif
				
				positionX = positionX + (324.0f * 0.5f) + 30.f + 157.0f;
			}
			else
			{
				positionX = (REAL32)(nPosX-(364.0f*0.5f)) - 65.0f;
				positionY = (REAL32)(nPosY-(57.0f*0.5f)) - 10.0f;

				// Team
#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
				m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX+(325.0f*0.5f)+130.0f, positionY, 325.0f, 57.0f);		
				m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 395.0f, 877.0f, 452.0f);
#else
				m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, positionX , positionY, 364.0f, 57.0f);	
				m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 395.0f, 877.0f, 452.0f);
#endif
				
				positionX = positionX + (364.0f * 0.5f) + 40.f + 157.0f;
			}

#if defined (LOCALE_TURKEY)		//	터키의 경우 팀과 결과 어순을 바꾼다.
			// Result
			m_pSpriteMission->SetRect(HUD_SPRITE_MISSION_RESULT, 
										(REAL32)(nPosX-(364.0f*0.5f)) - 50.0f, 
										(REAL32)(nPosY-(57.0f*0.5f)) - 10.0f, 
										187.0f, 55.0f);		

			m_pSpriteMission->SetTextureCoord(HUD_SPRITE_MISSION_RESULT, 514.0f, 452.0f, 700.0f, 507.0f);
#else
			// Result
			m_pSpriteMission->SetRect(HUD_SPRITE_MISSION_RESULT, positionX, positionY, 187.0f, 55.0f);			
			m_pSpriteMission->SetTextureCoord(HUD_SPRITE_MISSION_RESULT, 514.0f, 452.0f, 700.0f, 507.0f);
#endif
		}
		break;
	default:
		/* Do nothing */
		break;
	}


	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, TRUE);
	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, TRUE);

	if(g_pGameContext->IsBossMissionMode()) {
		m_pSpriteBoss->SetEnable( 0, TRUE);
	}
}

void CHud::UpdateMissionMessage( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	// Mission Message
	if( m_pTextMission[HUD_MISSION_TEXT_MESSAGE] != NULL && 
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->GetEnable())
	{
		m_fMissionMessageTime += rDeltaSeconds;
		if( m_fMissionMessageTime > 5.0f)
		{
			m_fMissionMessageTime = 0.0f;
			m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, FALSE);
			m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( FALSE);
		}
	}

	// Mission Notice
	if( m_pTextMission[HUD_MISSION_TEXT_NOTICE] != NULL &&  
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->GetEnable())
	{
		m_fMissionNoticeTimer += rDeltaSeconds;
		if( m_fMissionNoticeTimer > m_fMissionNoticeLifeTime ) //3.0f)
		{
			m_fMissionNoticeTimer = 0.0f;
 			m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( FALSE);
			m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetText("");
		}
	}
}

void CHud::EnableDeathSprite( BOOL bFlag)
{
	i3_prof_func();

	if( m_bEnableDeathSprite == bFlag ) return;

	m_bEnableDeathSprite = bFlag;

	if( bFlag)
	{
		m_fDeathSpriteTime = 0.0f;
		m_pSpriteDeath->SetEnable(TRUE);		
	}
	else
	{
		m_pSpriteDeath->SetColor( 0, 255,255,255,0);
		m_pSpriteDeath->SetColor( 1, 255,255,255,0);
		m_pSpriteDeath->SetEnable(FALSE);
	}
}

void CHud::UpdateDeathSprite( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	COLOR* pCol = NULL;
	UINT8 nAlpha = 0;

	pCol =m_pSpriteDeath->GetColor( 0);
	nAlpha = i3Color::GetA( pCol);
		
	nAlpha = (UINT8)(m_fDeathSpriteTime * 300.0f);
	if( nAlpha >= 150)
	{
		nAlpha = 150;
	}
	else
	{
		m_fDeathSpriteTime += rDeltaSeconds;
	}

	m_pSpriteDeath->SetColor( 0, 255,255,255,nAlpha);
	m_pSpriteDeath->SetColor( 1, 255,255,255,nAlpha);
}

void CHud::SetEnableBombMission(BOOL bFlag)		
{ 
	i3_prof_func();

	m_bBombMissionEnable = bFlag;
	
	if( !bFlag)
	{
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE, FALSE);
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, FALSE);
		
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, FALSE);
		m_pSpriteBombMission->SetEnable(HUD_SPRITE_MISSION_BOMB_TOOL, FALSE);
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetEnable( FALSE);
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
	}
}

void CHud::ProcessBomb( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);

	BOOL	bCheckRange = FALSE;
	if( m_bEnableObserverMode)
	{
		SetEnableMissionMessage( FALSE);
		return;
	}

	m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetEnable( FALSE);
	m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, FALSE);

	INT32 nPosX = 0;
	INT32 nPosY = 0;
	INT32 nScreenWidth = 0;
	INT32 nScreenHeight= 0;

	REAL32 fClanOffsetY = 0.0f;

	if( g_pGameContext->GetBattleChannel())
	{
		fClanOffsetY = 18.0f;
	}

	//1 폭파 가능지역인지...
	if( g_pGameContext->getMissionManager()->IsEnableMy() )	
	{	
		if( g_pViewer != NULL)
		{
			nScreenWidth = g_pViewer->GetViewWidth();
			nScreenHeight = g_pViewer->GetViewHeight();
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		// 폭파 가능지역이라면, 폭탄이미지를 활성화 합니다.
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE, TRUE);
		m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_LARGE, (REAL32)(nPosX-42), (75.0f)+fClanOffsetY, (90.0f), (46.0f));
		
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, FALSE);
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, FALSE);

		bCheckRange = TRUE;
	}
	else
	{
		// 폭파 가능지역이 아니라면, 폭탄이미지를 비활성화 합니다.
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE, FALSE);		
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, FALSE);		
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TOOL, FALSE);
		
		// 폭파 가능지역이 아니라면, 미션 메시지를 비활성화 합니다.
		SetEnableMissionMessage( FALSE);
		m_bOneTimeAreaUpdate = FALSE;
	}

	if( bCheckRange )
	{
		switch( g_pGameContext->getMissionManager()->getMissionState() )
		{
		case GAME_MISSION_STATE_INBATTLE :
			if( g_pGameContext->getMissionManager()->getMyMissionObject() == MY_MISSION_OBJECT_OFFENSIVE )
			{	// Offensive team
				if( !m_bOneTimeAreaUpdate)
				{
					SetMissionMessageString( GAME_STRING("STBL_IDX_BOMB_MISSION_AREA"));		// 폭파목표지역입니다. 폭탄으로 교체후...
					m_bOneTimeAreaUpdate = TRUE;
				}

				// 폭탄설치중인지...		
				if( g_pGameContext->getMissionManager()->IsInstalling( m_CurrentUserIdx ) )
				{
					SetEnableMissionMessage( FALSE);

					// 게이지 증가
					REAL32 fSetTimeByUser = g_pGameContext->getMissionBomb_SetTime_UsingSpeedKit( m_CurrentUserIdx);
					fSetTimeByUser += g_pGameContext->getMissionBomb_SetTime_UsingSkill( m_CurrentUserIdx);

					REAL32 fGage = ( 1.f - g_pGameContext->getMissionManager()->getInstallingTimer( m_CurrentUserIdx ) / fSetTimeByUser );

					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, TRUE);
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, TRUE);
					m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_GAGE, (REAL32)(nPosX-219), (REAL32)(nScreenHeight-92), 439.0f*fGage, (21.0f));
				}
				else
				{
					// 게이지 없앰!!
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);					
				}
			}
			
			break;

		case GAME_MISSION_STATE_INSTALLED:
			if( g_pGameContext->getMissionManager()->getMyMissionObject() == MY_MISSION_OBJECT_DEFENSIVE || g_pGameContext->IsTutorialMode()|| g_pGameContext->IsDominationMode())
			{	// Defensive team
				if( !m_bOneTimeAreaUpdate &&  !g_pGameContext->IsDominationMode())
				{
					char message[MAX_STRING_COUNT] = {0, };
					GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

					sprintf_s(message, "%s %s", 
							GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_SPECIALACTION] ),
							GAME_STRING("STBL_IDX_BOMB_MISSION_DETTACH_EXPLAIN"));				  

					SetMissionMessageString(message);
					m_bOneTimeAreaUpdate = TRUE;
				}

				// 폭탄해제중인지...
				if( g_pGameContext->getMissionManager()->IsUninstalling( m_CurrentUserIdx ) )
				{
					SetMissionNoticeString(GAME_STRING("STBL_IDX_BOMB_MISSION_DETTACH_ING"));			// 폭탄 해체중입니다.
					m_bDettachBomb = TRUE;

					SetEnableMissionMessage( FALSE);

					// 게이지 증가
					REAL32 fDisarmTimeByUser = g_pGameContext->getMissionBomb_DisarmTime_UsingSpeedKit( m_CurrentUserIdx);
					fDisarmTimeByUser += g_pGameContext->getMissionBomb_DisarmTime_UsingSkill( m_CurrentUserIdx);

					REAL32 fGage = ( 1.f - g_pGameContext->getMissionManager()->getUninstallingTimer( m_CurrentUserIdx ) / fDisarmTimeByUser );	

					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, TRUE);
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, TRUE);
					m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_GAGE, (REAL32)(nPosX-219), (REAL32)(nScreenHeight-92), 439.0f*fGage, (21.0f));
				}
				else // 폭탄해제중이 아니라면 타이머 리셋
				{
					if( m_bDettachBomb) 
					{
						SetEnableMissionNotice( FALSE);
						m_bDettachBomb = FALSE;
					}

					if( g_pGameContext->IsTutorialMode())
					{
						SetMissionNoticeString( GAME_STRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_4"), 1800.f);
						
					}
					// 게이지 없앰!!
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
					m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
				}
			}
			break;
		case GAME_MISSION_STATE_UNINSTALLED:
			{
				SetEnableBombMission( FALSE);
			}
			break;

		default :
			// 게이지 없앰!!
			m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
			m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
			break;
		}
	}
	else
	{
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
		m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
	}
}

void CHud::UpdateBomb( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	// 폭탄이 설치되었다는 가정하에 Update가 들어옴
	COLOR col;

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);

	INT32 nPosX = 0;
	INT32 nScreenWidth = 0;

	char szTime[16];
	REAL32 fTime = g_pGameContext->getMissionManager()->getExplosionTimer();

	REAL32 fClanOffsetY = 0.0f;

	if( g_pGameContext->GetBattleChannel())
	{
		fClanOffsetY = 18.0f;
	}
	
	if( g_pViewer != NULL)
	{
		nScreenWidth = g_pViewer->GetViewWidth();
		nPosX = (INT32)(nScreenWidth * 0.5f);
	}

	if(g_pGameContext->IsTutorialMode() == false)
	{
		if( g_pCharaManager->getCharaInfo( m_CurrentUserIdx)->GetTeam() & CHARACTER_TEAM_RED)
		{
			SetEnableMissionMessage( FALSE);
			m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE, FALSE);
			m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_GAGE_BG, FALSE);
		}
	}
	
	
	m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, TRUE);
	m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_LARGE, FALSE);

	m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, TRUE);
	
	if( !m_pTextBomb[HUD_BOMB_TEXT_TIME]->GetEnable()) m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetEnable( TRUE);
	m_pSpriteBombMission->SetEnable( HUD_SPRITE_MISSION_BOMB_TIMER_BG, TRUE);

	m_pSpriteBombMission->SetRect( HUD_SPRITE_MISSION_BOMB_LARGE_INSTALLED, (REAL32)(nPosX-110), (75.0f)+fClanOffsetY, (90.0f), (46.0f));

	// 블루팀이라면, 해체마크를 추가한다
	if (1 == m_CurrentUserIdx % 2)
	{
		m_pSpriteBombMission->SetRect(HUD_SPRITE_MISSION_BOMB_TOOL, (REAL32)(nPosX-110 + 52), (75.0f + 10)+fClanOffsetY, (33.0f), (33.0f));
		m_pSpriteBombMission->SetEnable(HUD_SPRITE_MISSION_BOMB_TOOL, TRUE);
	}

	// Timer
	{
		INT32 Minute = ((INT32)fTime)/60;
		INT32 Second = ((INT32)fTime)%60;
		INT32 MiliSecond = (INT32)((fTime - ((INT32)fTime))*100.0f);

		if( Minute <0) Minute = 0;
		if( Second <0) Second = 0;
		if( MiliSecond <0) MiliSecond = 0;

		if( Minute > 0)
		{
			sprintf_s( szTime, "%02d:%02d:%02d", Minute, Second, MiliSecond);
		}
		else
		{
			sprintf_s( szTime, "%02d:%02d", Second, MiliSecond);
		}	

		// 점멸
		if( Minute == 0 && Second < 10)
		{		
			COLORREAL colReal;
			REAL32 alpha;
			alpha = (i3Math::sin(fTime*15.0f)*0.5f) + 1.0f;
			alpha = alpha > 1.0f ? 1.0f : alpha;
			i3Color::Set( &colReal, 1.0f, 0.2f, 0.2f, alpha);
			i3Color::Set( &col, &colReal);
			
			m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetColor( &col);
		}
		else
		{
			i3Color::Set( &col, (UINT8) 255, 255, 255, 255);			
			m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetColor( &col);
		}
	}

	// 
	m_pTextBomb[HUD_BOMB_TEXT_TIME]->SetText( szTime);
}

// Vector값에서 2D 좌표계에서의 각도를 구함. (X,Z 평면에서 Z축 방향일 경우 0.0f );
REAL32 CHud::GetRotationAngle2D( VEC3D *pDirection )
{
	i3_prof_func();

	//REAL32 Rotation = -1.0f * atan2f( i3Vector::GetX( pDirection ), i3Vector::GetY( pDirection ) ); // XY 평면
	REAL32 Rotation = atan2f( i3Vector::GetX( pDirection ), i3Vector::GetZ( pDirection ) ); // XZ 평면
	return i3Math::NormalizeAngle( Rotation );
}
/*
void CHud::SetTouchDown(INT32 TouchDownslotIdx)
{
	I3ASSERT(g_pGUIRoot);

	if (0 > TouchDownslotIdx || SLOT_MAX_COUNT <= TouchDownslotIdx)
	{
		I3TRACE("[WARNING] SetKillText() - Under/over-flow KillerIdx(%d)\n", TouchDownslotIdx);
		return;
	}
		VEC2D  vecPos;
	char szTemp[256];
	INT32 i = 0;
	INT32 nEnableTextCount = 0;
	INT32 nTextWidth = 0;
	
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		
		if(m_pTextTouchDown[i]->GetEnable())
		{
			nEnableTextCount++;
		}
	}
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{	
		if(!m_pTextTouchDown[i]->GetEnable())
		{
			nTextWidth = 0;

			m_fKillerTime[i] = 0.0f;

			VEC2D vecPos;
			i3Vector::Set(&vecPos, 0.0f, 0.0f);

			 
			{
				if(g_pGameContext->IsRedTeam(TouchDownslotIdx))
				{
					// Red
					m_pTextTouchDown[i]->SetColor( 252, 88, 88, 255);
				}
				else
				{
					// Blue
					m_pTextTouchDown[i]->SetColor( 67, 182, 243, 255);
				}

				m_pTextTouchDown[i]->SetEnable(TRUE);
				m_pTextTouchDown[i]->SetText(g_pGameContext->getNickForSlot(TouchDownslotIdx));
				nTextWidth = m_pTextTouchDown[i]->getTextWidth();
			}

			{
				m_pTextTouchDown[i]->setSize(nTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextTouchDown[i]->SetTextEllipsis(g_pGameContext->getNickForSlot(TouchDownslotIdx));
				INT32 textWidth = m_pTextTouchDown[i]->getTextWidth();
				m_pTextTouchDown[i]->setPos((UINT32)(nScreenWidth - textWidth - (15.0f*i3GuiRoot::getHeightRatioBy768())), (UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
				m_pTextTouchDown[i]->getPos(&vecPos);
			}
				
			m_pSpriteLostWorldEP5->SetEnable( i, TRUE);
			m_pSpriteLostWorldEP5->SetRect( i, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()), i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()), 
				319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());


		}
	}


		m_bUpdateNotice = TRUE;
}
*/	



void CHud::SetKillWeapon( CWeaponInfo* pWeaponInfo, INT32 nIdx, bool bHP_Up, INT32 nObjIndex)
{
	i3_prof_func();

	// 위치 계산
	VEC2D  vecPos;
	
	m_pTextDeathName[nIdx]->getPos(&vecPos);
	
	REAL32 rX = i3Vector::GetX( &vecPos) - (100.0f*i3GuiRoot::getHeightRatioBy768());
	REAL32 rY = i3Vector::GetY( &vecPos) - (11.0f*i3GuiRoot::getHeightRatioBy768());
	//REAL32 rY = i3Vector::GetY( &vecPos) - (10.0f);

	UINT8 iWeaponNum; 
	UINT8 iWeaponClass; 
	
	if (pWeaponInfo)
	{// 무기에 의한 사망
		iWeaponNum	 = (UINT8) pWeaponInfo->GetNumber(); 
		iWeaponClass = (UINT8) pWeaponInfo->GetTypeClass(); 
	}
	else
	{// 오브젝트에 의한 사망		이부분 정규화 합니다. 
		if( nObjIndex == -1)
		{
			iWeaponNum	 = 1; 
			iWeaponClass = 100; 
		}
		else
		{
			iWeaponNum	 = 2; 
			iWeaponClass = 100; 
		}
	}
	///GUI_WEAPON_VALID(iWeaponClass, iWeaponNum);
	g_pFramework->SetWeaponKillMark(m_pSpriteKillMark[nIdx], iWeaponClass, iWeaponNum, bHP_Up );
	m_pSpriteKillMark[nIdx]->SetRect(0, rX, rY, 100.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
	m_pSpriteKillMark[nIdx]->SetEnable(TRUE);
}

void CHud::SetKillText(INT32 KillerIdx, INT32 DeathIdx, CWeaponInfo * pWeaponInfo, BOOL bHeadShot, bool bObjDefence, INT32 nObjIndex )
{		
	i3_prof_func();

	I3ASSERT(g_pGUIRoot);

	if (0 > KillerIdx || SLOT_MAX_COUNT <= KillerIdx)
	{
		I3TRACE("[WARNING] SetKillText() - Under/over-flow KillerIdx(%d)\n", KillerIdx);
		return;
	}

	if (0 > DeathIdx || SLOT_MAX_COUNT <= DeathIdx)	
	{
		I3TRACE("[WARNING] SetKillText() - Under/over-flow DeathIdx(%d)\n", DeathIdx);
		return;
	}

	char szTemp[256];
	INT32 i = 0;
	INT32 nEnableTextCount = 0;
	INT32 nTotalTextWidth = 0;
	INT32 nKillerTextWidth = 0;
	INT32 nDeathTextWidth = 0;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();

	BOOL bMine = FALSE;

	// RECORD 기능은 플레이어와 해야한다.
	if( g_pGameContext->getMySlotIdx() == KillerIdx || g_pGameContext->getMySlotIdx() == DeathIdx)
	{
		bMine = TRUE;

		if( g_pEnvSet->m_b1VS1)
		{
			if( KillerIdx != DeathIdx) // 자살로 죽은것이 아니라면..
			{
				if( !g_pGameContext->IsStageMaskTraining()
					&& LocaleValue::Enable("EnableRevengeMode") 
					&& !g_pGameContext->IsDominationMode() )
				{
					if( !g_pGameContext->IsStageMaskTraining())
					{
						UIBattleFrame * pFrame = UIBattleFrame::i();
						UIHUDRecordBoard * RecordBoard = (UIHUDRecordBoard *)(pFrame->GetHUD(UIHUD_RECORD_BOARD));
						if( g_pGameContext->getMySlotIdx() == KillerIdx)
						{
							RecordBoard->UIRecordRevenge( DeathIdx, 1);
							pFrame->EnableHUD( UIHUD_RECORD_BOARD, false);
						}
						else if( g_pGameContext->getMySlotIdx() == DeathIdx)
						{
							RecordBoard->UIRecordRevenge( KillerIdx, -1);
							pFrame->EnableHUD( UIHUD_RECORD_BOARD, true);
						}
					}
					else
					{
						if( g_pGameContext->getMySlotIdx() == KillerIdx)		RecordRevenge( DeathIdx, 1);
						else if( g_pGameContext->getMySlotIdx() == DeathIdx)	RecordRevenge( KillerIdx, -1);
					}
				}
			}
		}
	}


	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if(m_pTextNotice[i]->GetEnable())
		{
			nEnableTextCount++;
		}

		if(m_pTextKillerName[i]->GetEnable())
		{
			nEnableTextCount++;
		}
	}
			
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{	
		if(!m_pTextKillerName[i]->GetEnable())
		{
			nTotalTextWidth = 0;

			m_fKillerTime[i] = 0.0f;

			VEC2D vecPos;
			i3Vector::Set(&vecPos, 0.0f, 0.0f);

			// 죽은 사람
			{
				//if( DeathIdx %2 == 0)
				if(g_pGameContext->IsRedTeam(DeathIdx))
				{
					// Red
					m_pTextDeathName[i]->SetColor( 252, 88, 88, 255);
				}
				else
				{
					// Blue
					m_pTextDeathName[i]->SetColor( 67, 182, 243, 255);
				}

				m_pTextDeathName[i]->SetEnable(TRUE);
				m_pTextDeathName[i]->SetText(g_pGameContext->getNickForSlot(DeathIdx));
				nDeathTextWidth = m_pTextDeathName[i]->getTextWidth();
				nTotalTextWidth += nDeathTextWidth;
			}
			
			if( nObjIndex == -1)
			{
				// 죽인 사람
				//if( KillerIdx %2 == 0)
				if(g_pGameContext->IsRedTeam(KillerIdx))
				{
					// Red
					m_pTextKillerName[i]->SetColor( 252, 88, 88, 255);
				}
				else
				{
					// Blue
					m_pTextKillerName[i]->SetColor( 67, 182, 243, 255);
				}

				sprintf_s( szTemp, "%s", g_pGameContext->getNickForSlot(KillerIdx));
/*
				if( i3String::Length( szTemp ) == 0 )
				{
					I3ASSERT_0;
				}*/

				m_pTextKillerName[i]->SetEnable(TRUE);
				m_pTextKillerName[i]->SetText(szTemp);
				nKillerTextWidth = m_pTextKillerName[i]->getTextWidth();				
				nTotalTextWidth += nKillerTextWidth;
			}
			else
			{
				// 오브젝트에 의한 사망
				sprintf_s( szTemp, "%s", g_pGameContext->getNickForSlot(KillerIdx));

				CStageBattle * pStage = (CStageBattle*) g_pFramework->GetCurrentStage();
				StageObjectListPtr pObjList = pStage->GetStageObjectList();
				if( pObjList != NULL)
				{
					if( nObjIndex >= (INT32)pObjList->size())
					{
						return;
					}

					i3Object * pObj = (i3Object*) (*pObjList)[nObjIndex];
					if( pObj->hasName())
					{
						i3String::Copy( szTemp, pObj->GetName(), 256 );
					}
				}

				if( i3String::Length( szTemp ) == 0 )
				{
					I3ASSERT_0;
				}

				m_pTextKillerName[i]->SetColor( 252, 255, 255, 255);
				m_pTextKillerName[i]->SetEnable(TRUE);
				m_pTextKillerName[i]->SetText(szTemp);
				nKillerTextWidth = m_pTextKillerName[i]->getTextWidth();				
				nTotalTextWidth += nKillerTextWidth;
			}

			while( nTotalTextWidth > (200*i3GuiRoot::getHeightRatioBy768()))
			{
				nTotalTextWidth = 0;

				if( nDeathTextWidth > nKillerTextWidth)
				{
					nDeathTextWidth -= (INT32)(2*i3GuiRoot::getHeightRatioBy768());
				}
				else
				{
					nKillerTextWidth -= (INT32)(2*i3GuiRoot::getHeightRatioBy768());
				}

				nTotalTextWidth = (nDeathTextWidth+nKillerTextWidth);
			}

			{
				m_pTextDeathName[i]->setSize(nDeathTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextDeathName[i]->SetTextEllipsis(g_pGameContext->getNickForSlot(DeathIdx));
				INT32 textWidth = m_pTextDeathName[i]->getTextWidth();
				m_pTextDeathName[i]->setPos((UINT32)(nScreenWidth - textWidth - (15.0f*i3GuiRoot::getHeightRatioBy768())), (UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
				m_pTextDeathName[i]->getPos(&vecPos);
			}

			// 입력된 모든 Text로부터 길이값을 가져와 KillMark위치를 정합니다.
			{
				// 어떤 무기로 죽었는지 셋팅!
				bool bHP_Up = false;
				
				if(g_pCharaManager->IsHP_Recovery(KillerIdx, DeathIdx)) {
					bHP_Up = true;
				}
				// KillMark1
				SetKillWeapon(pWeaponInfo, i, bHP_Up, nObjIndex);

				if( bObjDefence)
				{
					// Head Shot - 오브젝트/수류탄으로 죽었을 경우는 헤드샷이 없음!  
					if( bHeadShot && pWeaponInfo && pWeaponInfo->isEnableHeadShot())
					{	
						m_pSpriteObjDefenctAndHead->SetEnable( i, TRUE);
						m_pSpriteObjDefenctAndHead->SetRect( i, i3Vector::GetX(&vecPos) - (72.0f*i3GuiRoot::getHeightRatioBy768()), 
							i3Vector::GetY(&vecPos) - (11.0f*i3GuiRoot::getHeightRatioBy768()), 44.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768(), 
							38.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768());
					}
					else
					{	
						m_pSpriteObjectDefence->SetEnable( i, TRUE);
						m_pSpriteObjectDefence->SetRect( i, i3Vector::GetX(&vecPos) - (65.0f*i3GuiRoot::getHeightRatioBy768()), 
							i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()), 28.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768(), 
							30.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768());
					}

					m_fScaleSize[i] = 3.0f;
				}
				else
				// Head Shot - 오브젝트/수류탄으로 죽었을 경우는 헤드샷이 없음!  
				if( bHeadShot && pWeaponInfo && pWeaponInfo->isEnableHeadShot())
				{
					m_fScaleSize[i] = 3.0f;
					m_pSpriteHeadShot->SetEnable( i, TRUE);
					m_pSpriteHeadShot->SetRect( i, i3Vector::GetX(&vecPos) - (75.0f*i3GuiRoot::getHeightRatioBy768()), 
						i3Vector::GetY(&vecPos) - (10.0f*i3GuiRoot::getHeightRatioBy768()), 55.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768(), 
						36.0f * m_fScaleSize[i] * i3GuiRoot::getHeightRatioBy768());
				}
			}

			{
				m_pTextKillerName[i]->setSize(nKillerTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextKillerName[i]->SetTextEllipsis(szTemp);

				INT32 textWidth = m_pTextKillerName[i]->getTextWidth();
				m_pTextKillerName[i]->setPos( (UINT32)(i3Vector::GetX(&vecPos) - (100.0f*i3GuiRoot::getHeightRatioBy768()) - textWidth), 
												(UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
			}

			//
			if( bMine)
			{
				m_pSpriteKillTextBG->SetEnable( i, TRUE);
				m_pSpriteKillTextBG->SetRect( i, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()), i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()), 
					319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());
			}

			break;
		}	
	}
	
	m_bUpdateNotice = TRUE;
}

void CHud::SetGetPointText( INT32 nUserIdx, INT32 nObjectIdx, INT32 nPoint)
{	
	i3_prof_func();

	if (0 > nUserIdx || SLOT_MAX_COUNT <= nUserIdx)
	{
		I3TRACE("[WARNING] SetGetPointText() - Under/over-flow nUserIdx(%d)\n", nUserIdx);
		return;
	}

	char szTemp[256];
 	//INT32 x = 0;
	INT32 i = 0;
	INT32 nWidth = 0;
	INT32 StartPosOffset = (INT32)(63*i3GuiRoot::getHeightRatioBy768());
	INT32 nEnableTextCount = 0;	

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();

	BOOL bMine = FALSE;

	if( g_pGameContext->getMySlotIdx() == nUserIdx)
	{
		bMine = TRUE;
	}

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if(m_pTextNotice[i]->GetEnable())
		{
			nEnableTextCount++;
		}

		if(m_pTextKillerName[i]->GetEnable())
		{
			nEnableTextCount++;
		}
	}
			
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{		
		if(!m_pTextKillerName[i]->GetEnable())
		{			
			m_pTextKillerName[i]->SetEnable(TRUE);
			m_fKillerTime[i] = 0.0f;

			VEC2D vecPos;
			
			// 죽인 사람
			{
				if( nUserIdx %2 == 0)
				{
					// Red
					m_pTextKillerName[i]->SetColor( 252, 88, 88, 255);
				}
				else
				{
					// Blue
					m_pTextKillerName[i]->SetColor( 67, 182, 243, 255);
				}
				sprintf_s( szTemp, "%s", g_pGameContext->getNickForSlot(nUserIdx));
				if( i3String::Length( szTemp ) == 0 )
				{
					I3ASSERT_0;
				}
				m_pTextKillerName[i]->setSize((UINT32)(512*i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextKillerName[i]->SetText(szTemp);
			}

			// 죽은 사람
			{	//EscapeMission 공룡전은 추가 포인트를 표시하지 않습니다.
				if (!g_pGameContext->IsBossMissionMode())
				{
					m_pTextDeathName[i]->SetColor( 125, 125, 0, 255);
					m_pTextDeathName[i]->SetEnable(TRUE);
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_GET_EXP"), nPoint);/*경험치 + %d*/
					m_pTextDeathName[i]->setSize((UINT32)(512*i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
					m_pTextDeathName[i]->SetText( szTemp);
				}
				nWidth = m_pTextKillerName[i]->getTextWidth() + m_pTextDeathName[i]->getTextWidth();						
				m_pTextKillerName[i]->setPos( nScreenWidth - StartPosOffset - nWidth, (UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
				m_pTextKillerName[i]->getPos( &vecPos);
				
				REAL32 rX = i3Vector::GetX( &vecPos) + m_pTextKillerName[i]->getTextWidth() + (20 * i3GuiRoot::getHeightRatioBy768());				
				m_pTextDeathName[i]->setPos( (INT32)rX, (INT32)(i3Vector::GetY(&vecPos)) );		
			}

			// 입력된 모든 Text로부터 길이값을 가져와 KillMark위치를 정합니다.
			{
				nWidth = m_pTextKillerName[i]->getTextWidth() + m_pTextDeathName[i]->getTextWidth();

				{
					// 위치 계산
					REAL32 rX = i3Vector::GetX( &vecPos) + nWidth + (23*i3GuiRoot::getHeightRatioBy768());
					REAL32 rY = i3Vector::GetY( &vecPos) - (5.0f*i3GuiRoot::getHeightRatioBy768());

					m_pSpriteKillMark[i]->SetTexture(m_pTexture);

					STAGE_ID StageID = g_pGameContext->getStageID();
					STAGE_TYPE StageType = getStageType(StageID);
					switch( StageID)
					{
					case STAGE_ID_BREAKDOWN :
						{
							if( nObjectIdx == 0)	m_pSpriteKillMark[i]->SetTextureCoord(0, 870.0f, 149.0f, 902.0f, 173.0f);	
							else					m_pSpriteKillMark[i]->SetTextureCoord(0, 910.0f, 149.0f, 937.0f, 174.0f);	
						}
						break;
					case STAGE_ID_GIRAN:
					case STAGE_ID_GIRAN2 :
						{
							if( nObjectIdx == 0)	m_pSpriteKillMark[i]->SetTextureCoord(0, 938.0f, 147.0f, 970.0f, 174.0f);	
							else					m_pSpriteKillMark[i]->SetTextureCoord(0, 973.0f, 145.0f, 1006.0f, 174.0f);	
						}
						break;

					case STAGE_ID_HELISPOT :
						{
							if( nObjectIdx == 0)	m_pSpriteKillMark[i]->SetTextureCoord(0, 870.0f, 149.0f, 902.0f, 173.0f);
							else					m_pSpriteKillMark[i]->SetTextureCoord(0, 933.0f, 282.0f, 966.0f, 306.0f);	
						}
						break;

						// Defence mission
					case STAGE_ID_BLACKPANTHER :
						{
							m_pSpriteKillMark[i]->SetTexture( m_pTextureHUD3);

							if( nObjectIdx == 0)	m_pSpriteKillMark[i]->SetTextureCoord(0, 583.f, 116.0f, 615.0f, 140.0f);
							else					m_pSpriteKillMark[i]->SetTextureCoord(0, 623.0f, 116.0f, 655.0f, 140.0f);
						}
						break;
					case STAGE_ID_BREEDING_NEST :
					case STAGE_ID_D_UPTOWN :
					case STAGE_ID_DINO_BREAKDOWN :
						{
							m_pSpriteKillMark[i]->SetTexture( m_pTextureHUD3);

							if( nObjectIdx == 0)	m_pSpriteKillMark[i]->SetTextureCoord(0, 485.0f, 152.0f, 518.0f, 180.0f);
						
						}

						break;

					default :
						{
							if( (StageType == STAGE_TYPE_DESTROY)
								|| (StageType == STAGE_TYPE_DEFENCE)
								)
							{
								I3WARN( "HUD no work.");
							}
						}
						break;
					}

					m_pSpriteKillMark[i]->SetRect(0, rX, rY, 32.0f*i3GuiRoot::getHeightRatioBy768(), 24.0f*i3GuiRoot::getHeightRatioBy768());
					m_pSpriteKillMark[i]->SetEnable(TRUE);
				}
			}

			//
			if( bMine)
			{
				m_pSpriteKillTextBG->SetEnable( i, TRUE);
				m_pSpriteKillTextBG->SetRect( i, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()), i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768())
					, 319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());
			}
		
			break;
		}	
	}
	
	m_bUpdateNotice = TRUE;
}

void CHud::SetTrainingPointText( INT32 nObjectIdx, INT32 nPoint)
{
	i3_prof_func();

	char szTemp[256];
 	INT32 nUserIdx = 0;
	INT32 i = 0;
	INT32 nWidth = 0;
	INT32 StartPosOffset = (INT32)(63*i3GuiRoot::getHeightRatioBy768());
	INT32 nEnableTextCount = 0;	

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();

	nUserIdx = g_pGameContext->getMySlotIdx();

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if(m_pTextNotice[i]->GetEnable())
		{
			nEnableTextCount++;
		}

		if(m_pTextKillerName[i]->GetEnable())
		{
			nEnableTextCount++;
		}
	}
			
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{		
		if(!m_pTextKillerName[i]->GetEnable())
		{			
			m_pTextKillerName[i]->SetEnable(TRUE);
			m_fKillerTime[i] = 0.0f;

			VEC2D vecPos;
			
			// 획득한 사람
			{
				if( nUserIdx %2 == 0)
				{
					// Red
					m_pTextKillerName[i]->SetColor( 252, 88, 88, 255);
				}
				else
				{
					// Blue
					m_pTextKillerName[i]->SetColor( 67, 182, 243, 255);
				}
				sprintf_s( szTemp, "%s", g_pGameContext->getNickForSlot(nUserIdx));
				m_pTextKillerName[i]->setSize((UINT32)(512*i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextKillerName[i]->SetText(szTemp);

				m_pTextDeathName[i]->SetColor( 255, 255, 255, 255);
				m_pTextDeathName[i]->SetEnable(TRUE);
				if( nObjectIdx == TargetObject::TYPE_FRIEND)
				{
					sprintf_s( szTemp, "-%d", nPoint);
				}
				else if( nObjectIdx == TargetObject::TYPE_ENEMY)
				{
					sprintf_s( szTemp, "+%d", nPoint);
				}
				m_pTextDeathName[i]->setSize((UINT32)(512*i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
				m_pTextDeathName[i]->SetText( szTemp);

				nWidth = m_pTextKillerName[i]->getTextWidth() + m_pTextDeathName[i]->getTextWidth();						
				m_pTextKillerName[i]->setPos( nScreenWidth - StartPosOffset - nWidth, (UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
				m_pTextKillerName[i]->getPos( &vecPos);
			}

			// 입력된 모든 Text로부터 길이값을 가져와 타겟스프라이트의 위치를 정합니다.
			{
				nWidth = m_pTextKillerName[i]->getTextWidth();

				{
					// 위치 계산
					REAL32 rX = i3Vector::GetX( &vecPos) + nWidth + 11;
					REAL32 rY = i3Vector::GetY( &vecPos) - 10.0f;

					m_pSpriteKillMark[i]->SetTexture(m_pTexture);

					if( nObjectIdx == TargetObject::TYPE_ENEMY)			m_pSpriteKillMark[i]->SetTextureCoord(0, 144.0f, 411.0f, 180.0f, 447.0f);	
					else if( nObjectIdx == TargetObject::TYPE_FRIEND)	m_pSpriteKillMark[i]->SetTextureCoord(0, 105.0f, 411.0f, 141.0f, 447.0f);	

					m_pSpriteKillMark[i]->SetRect(0, rX, rY, 36.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
					m_pSpriteKillMark[i]->SetEnable(TRUE);
				}
			}

			// 포인트
			{
				REAL32 rX = i3Vector::GetX( &vecPos) + m_pTextKillerName[i]->getTextWidth() + (56*i3GuiRoot::getHeightRatioBy768());				
				m_pTextDeathName[i]->setPos( (INT32)rX, (INT32)(i3Vector::GetY(&vecPos)) );		
			}

			break;
		}	
	}
	
	m_bUpdateNotice = TRUE;
}

void CHud::SetNoticeText(const char* pszNotice, COLOR* pColor)
{
	i3_prof_func();

	INT32 i = 0;
	INT32 StartPosOffset = (INT32)(13*i3GuiRoot::getHeightRatioBy768());
	INT32 nEnableTextCount = 0;
	INT32 nTextWidth = 0;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if(m_pTextNotice[i]->GetEnable())
		{
			nEnableTextCount++;
		}

		if(m_pTextKillerName[i]->GetEnable())
		{
			nEnableTextCount++;
		}
	}
	
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{		
		if(!m_pTextNotice[i]->GetEnable())
		{
			m_pTextNotice[i]->SetEnable(TRUE);
			if( pColor == NULL)
			{
				m_pTextNotice[i]->SetColor(255, 255, 255, 255);
			}
			else
			{
				m_pTextNotice[i]->SetColor( pColor);
			}
			m_pTextNotice[i]->SetText( pszNotice);
			nTextWidth = m_pTextNotice[i]->getTextWidth();
			m_pTextNotice[i]->setPos(nScreenWidth - StartPosOffset - nTextWidth, (UINT32)((nEnableTextCount+1) * NOTICE_LINE_HEIGHT));
			m_fNoticeTime[i] = 0.0f;
			break;
		}	
	}
	
	m_bUpdateNotice = TRUE;
}

void CHud::UpDateNoticeText(REAL32 fTime)
{
	i3_prof_func();

	INT32 i = 0;
	INT32 nCount = 0;
	
	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if( m_pTextNotice[i]->GetEnable())
		{
			m_fNoticeTime[i] += fTime;

			if( m_fNoticeTime[i] > HUD_NOTICE_TIME)
			{
				m_fNoticeTime[i] = 0.0f;
				m_pTextNotice[i]->SetText( " ");
				m_pTextNotice[i]->SetEnable( FALSE);

				SetPosNoticeText( fTime);
			}

			nCount++;
		}

		if( m_pTextKillerName[i]->GetEnable() || m_pTextTouchDown[i]->GetEnable())
		{
			m_fKillerTime[i] += fTime;
			m_fScaleSize[i] -= (fTime*8.0f);

			if( m_fScaleSize[i] < 1.0f) m_fScaleSize[i] = 1.0f;

			VEC3D vecPos;

			if( m_pSpriteHeadShot->GetEnable( i))
			{
				m_pSpriteHeadShot->GetLeftTop( i, &vecPos);
				m_pSpriteHeadShot->SetRect( i, i3Vector::GetX( &vecPos), i3Vector::GetY( &vecPos), (55.0f)*m_fScaleSize[i]*i3GuiRoot::getHeightRatioBy768(), 
					(36.0f)*m_fScaleSize[i]*i3GuiRoot::getHeightRatioBy768());
			}

			if( m_pSpriteObjectDefence->GetEnable( i))
			{
				m_pSpriteObjectDefence->GetLeftTop( i, &vecPos);
				m_pSpriteObjectDefence->SetRect( i, i3Vector::GetX( &vecPos), i3Vector::GetY( &vecPos), (28.0f)*m_fScaleSize[i]*i3GuiRoot::getHeightRatioBy768(), 
					(30.0f)*m_fScaleSize[i]*i3GuiRoot::getHeightRatioBy768());
			}

			if( m_pSpriteObjDefenctAndHead->GetEnable( i))
			{
				m_pSpriteObjDefenctAndHead->GetLeftTop( i, &vecPos);
				m_pSpriteObjDefenctAndHead->SetRect( i, i3Vector::GetX( &vecPos), i3Vector::GetY( &vecPos), (44.f)*m_fScaleSize[ i] * i3GuiRoot::getHeightRatioBy768(),
					(38.f)*m_fScaleSize[ i] * i3GuiRoot::getHeightRatioBy768());
			}

//Key:EscapeMission KillMessage설정

			if( m_pSpriteLostWorldEP5->GetEnable( i))
			{
				m_pSpriteLostWorldEP5->GetLeftTop( i, &vecPos);
				m_pSpriteLostWorldEP5->SetRect( i, i3Vector::GetX( &vecPos), i3Vector::GetY( &vecPos), (44.f)*m_fScaleSize[ i] * i3GuiRoot::getHeightRatioBy768(),
					(38.f)*m_fScaleSize[ i] * i3GuiRoot::getHeightRatioBy768());
			}


			if( m_fKillerTime[i] > HUD_NOTICE_TIME)
			{
				m_fScaleSize[i] = 0.0f;
				m_fKillerTime[i] = 0.0f;
				
				m_pTextDeathName[i]->SetText( " ");
				m_pTextKillerName[i]->SetText( " ");
				m_pTextTouchDown[i]->SetText(" ");

				m_pTextDeathName[i]->SetEnable( FALSE);
				m_pTextKillerName[i]->SetEnable( FALSE);
				m_pTextTouchDown[i]->SetEnable(FALSE);
				
				m_pSpriteHeadShot->SetEnable( i, FALSE);

				m_pSpriteObjectDefence->SetEnable( i, FALSE);
				m_pSpriteObjDefenctAndHead->SetEnable( i, FALSE);

				m_pSpriteKillTextBG->SetEnable( i, FALSE);

				m_pSpriteKillMark[i]->SetEnable(FALSE);
//Key:EscapeMission KillMessage설정
				m_pSpriteLostWorldEP5->SetEnable(FALSE);

				SetPosNoticeText( fTime);
			}
			
			nCount++;
		}
	}

	if(nCount == 0)
	{
		m_bUpdateNotice = FALSE;
	}
}

void CHud::SetPosNoticeText(REAL32 fTime)
{
	i3_prof_func();

	INT32 i = 0;
	INT32 nEnableTextCount = 0;
	VEC2D vecPos;

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		if(m_pTextNotice[i]->GetEnable())
		{
			m_pTextNotice[i]->getPos( &vecPos);
			m_pTextNotice[i]->setPos( (UINT32)(i3Vector::GetX( &vecPos)), (UINT32)((i3Vector::GetY( &vecPos)) - NOTICE_LINE_HEIGHT));
			nEnableTextCount++;
		}

		if(m_pTextKillerName[i]->GetEnable())
		{
			VEC3D vec;
			VEC2D* pVecSize;

			if (m_pSpriteKillMark[i]->GetEnable(0))
			{
				m_pSpriteKillMark[i]->GetLeftTop(0, &vec);				
				pVecSize = m_pSpriteKillMark[i]->GetSize(0);
				m_pSpriteKillMark[i]->SetRect(0, i3Vector::GetX( &vec), i3Vector::GetY( &vec) - NOTICE_LINE_HEIGHT, 
					i3Vector::GetX(pVecSize), i3Vector::GetY(pVecSize));
			}

			if( m_pSpriteHeadShot->GetEnable( i))
			{
				m_pSpriteHeadShot->GetLeftTop( i, &vec);
				m_pSpriteHeadShot->SetRect( i, i3Vector::GetX( &vec), i3Vector::GetY( &vec) - NOTICE_LINE_HEIGHT, 
					55.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
			}

			if( m_pSpriteObjectDefence->GetEnable( i))
			{
				m_pSpriteObjectDefence->GetLeftTop( i, &vec);
				m_pSpriteObjectDefence->SetRect( i, i3Vector::GetX( &vec), i3Vector::GetY( &vec) - NOTICE_LINE_HEIGHT, 
					28.0f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
			}

			if( m_pSpriteObjDefenctAndHead->GetEnable( i))
			{
				m_pSpriteObjDefenctAndHead->GetLeftTop( i, &vec);
				m_pSpriteObjDefenctAndHead->SetRect( i, i3Vector::GetX( &vec), i3Vector::GetY( &vec) - NOTICE_LINE_HEIGHT,
					44.f * i3GuiRoot::getHeightRatioBy768(), 38.f * i3GuiRoot::getHeightRatioBy768());
			}

			m_pSpriteKillTextBG->GetLeftTop( i, &vec);
			m_pSpriteKillTextBG->SetRect( i, i3Vector::GetX( &vec), i3Vector::GetY( &vec) - NOTICE_LINE_HEIGHT, 
				319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());

			m_pTextDeathName[i]->getPos( &vecPos);
			m_pTextDeathName[i]->setPos( (UINT32)(i3Vector::GetX( &vecPos)), (UINT32)((i3Vector::GetY( &vecPos)) - NOTICE_LINE_HEIGHT));

			m_pTextKillerName[i]->getPos( &vecPos);
			m_pTextKillerName[i]->setPos( (UINT32)(i3Vector::GetX( &vecPos)), (UINT32)((i3Vector::GetY( &vecPos)) - NOTICE_LINE_HEIGHT));

			nEnableTextCount++;
		}
	}
}

#define ETC_TEXT_TIME 3.0f

void CHud::SetEtcText( const char* pszString)
{
	i3_prof_func();

	m_fEtcTextTime = 0.0f;
	m_pTextEtc->SetText( pszString);
	m_pTextEtc->SetEnable( TRUE);
	m_pSpriteEtcMessageBG->SetEnable( 0, TRUE);
}

void CHud::UpDateEtcText( REAL32 fTime)
{
	i3_prof_func();

	m_fEtcTextTime += fTime;

	if( m_fEtcTextTime > ETC_TEXT_TIME)
	{
		m_fEtcTextTime = 0.0f;
		m_pTextEtc->SetEnable( FALSE);
		m_pSpriteEtcMessageBG->SetEnable( 0, FALSE);
	}
}

void CHud::SetAttackWarning()
{
	return;
}

void CHud::SetDamageDirection(REAL32 fAngle)
{
	i3_prof_func();

	if( m_bOnlyNoticeUpdate) return;

	// 360도 방위
	{
		m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_ANGLE_START + m_idxDamageDirectionOrder, TRUE);
		m_pSpriteHit->SetColor( HUD_SPRITE_HIT_ANGLE_START + m_idxDamageDirectionOrder, 255, 255, 255, 255);				
		m_pSpriteHit->SetCenter( HUD_SPRITE_HIT_ANGLE_START + m_idxDamageDirectionOrder, m_fViewHalfWidth - (100.0f * i3Math::sin(fAngle)), m_fViewHalfHeight - (100.0f * i3Math::cos(fAngle)), 0.0f);
		
		m_pSpriteHit->SetRotation( HUD_SPRITE_HIT_ANGLE_START + m_idxDamageDirectionOrder, fAngle);
		m_fDamageDirectionAngle[m_idxDamageDirectionOrder] = fAngle;

		m_idxDamageDirectionOrder++;
		if( m_idxDamageDirectionOrder >= HUD_SPRITE_HIT_ANGLE_MAX)
		{
			m_idxDamageDirectionOrder = 0;
		}
	}
}

void CHud::UpdateDamageDirection(REAL32 fTime)
{
	i3_prof_func();

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);

	if( g_pGameContext->getUserDeath( m_CurrentUserIdx) == true )
	{
		// 케릭터가 죽었으면 모든 피격 효과 취소
		m_pSpriteHit->SetEnable( FALSE);
		return;
	}

	// Damaged Fx가 업데이트 시, Warning Fx는 출력하지 않는다
	if (_UpdateAttackedEffect(fTime))
	{
		_ClearGrazedEffect();
	}
	else
	{
		_UpdateGrazedEffect(fTime);
	}
}

// EXPORT_AVALIABLE_CODE_VALUE
bool	__isIndicatorEnable( WeaponBase * pWpn)
{
	i3_prof_func();

	if( pWpn == NULL)  return false;
	if( pWpn->getWeaponInfo()->GetTypeClass() != WEAPON_CLASS_THROWING1)		return false;

	switch( pWpn->getWeaponInfo()->GetIndicatorType())
	{
	case WEAPON_INDICATOR_GRENADE :
		{
			WeaponGrenade * pWeapon = (WeaponGrenade*)pWpn;

			if( (pWeapon->isThrowing() == true) && (pWeapon->isSettled() == true))
				return true;
		}
		break;
	case WEAPON_INDICATOR_C5 :
		{
			WeaponC5 * pWeapon = (WeaponC5*)pWpn;

			if( (pWeapon->isThrowing() == true) && (pWeapon->isSettled() == true))
				return true;
		}
		break;
	}

	return false;
}

void	CHud::UpdateThrowWeaponIndicator( REAL32 timeStep, i3::vector<WeaponBase*>& List)
{
	i3_prof_func();

	// 모든 수류탄을 파악
	WeaponBase * pWeapon;
	INT32  idx = 0;
	VEC3D diff;
	char szTemp[256];

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentUserIdx);

	if( pChara == NULL)
		return;

	i3Viewer * pViewer = g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	INT32	startIdx = -1;

	REAL32 Radius1 = 100.0f;
	const REAL32 Radius2 = 35.0f;
	MATRIX inv, local;

	i3Matrix::Inverse( &inv, NULL, pChara->GetMatrix());

	INT32 count = 0;

	for(size_t i = 0; i < List.size(); i++)
	{
		pWeapon = List[i];

		if( pWeapon == NULL)					continue;
		if( pWeapon->getWeaponInfo() == NULL)	continue;

		switch( pWeapon->getWeaponInfo()->GetIndicatorType())
		{
		case WEAPON_INDICATOR_GRENADE:		startIdx = 0;	break;
		case WEAPON_INDICATOR_C5:			startIdx = INDICATOR_COUNT_PEROBJ;	break;
		default:	startIdx = 0;	break;
		}

		idx = startIdx + count;

		if( idx >= MAX_INDICATOR)	continue;

		//if( __isIndicatorEnable( pWeapon) == false)	// 앞에서 이미 검사를 진행했다.. 필요없는 코드 swoongoo
		//{	// 제거대상
		//	i3::vu::erase_index(List, i);
		//	i--;
		//}
		//else
		{
			i3Vector::Sub( &diff, pChara->GetPos(), pWeapon->GetPos());
			REAL32 fDistance = i3Math::abs( i3Vector::Length( &diff));

			bool bEnable = true;
			// 지정거리 밖으로 벗어나면 표시 해제
			if( fDistance > pWeapon->getWeaponInfo()->GetIndicateDistance() )
				bEnable = false;

			// 옵저버 모드일경우 수류탄 마크를 표시하지 않습니다.
			if( bEnable && m_bEnableObserverMode == FALSE)
			{
				m_pGrenadeInd->SetEnable( idx * 2, TRUE);
				m_pGrenadeInd->SetEnable( idx * 2 + 1, TRUE);

				// 수류탄의 위치를 Player의 Local-Space로 변환
				i3Matrix::Mul( &local, pWeapon->GetMatrix(), &inv);
				i3Vector::Copy( &diff, i3Matrix::GetPos( &local));

				REAL32 theta = i3Math::atan2( diff.x, diff.z);
				REAL32 x, y;

				// 수류탄 모냥 
				x = (nScreenWidth * 0.5f) - i3Math::sin( theta) * Radius1;
				y = (nScreenHeight * 0.5f) - i3Math::cos( theta) * Radius1;
				m_pGrenadeInd->SetCenter( idx * 2, x, y, 0.0f);

				// 화살표 모냥
				x = x - i3Math::sin( theta) * Radius2;
				y = y - i3Math::cos( theta) * Radius2;
				m_pGrenadeInd->SetCenter( idx * 2 +1, x, y, 0.0f);
				m_pGrenadeInd->SetRotation( idx * 2 +1, theta);

				// 수류탄 거리 표시
				{
					VEC3D vecPos;
					m_pGrenadeInd->GetRightBottom(idx * 2, &vecPos);

					if( fDistance < pWeapon->getWeaponInfo()->GetIndicateDistance() )
					{
						sprintf_s( szTemp, "%.1fM", fDistance);
						m_pTextGrenade[idx]->SetEnable( TRUE);
						m_pTextGrenade[idx]->SetText( szTemp);

						if( idx < INDICATOR_COUNT_PEROBJ)
							m_pTextGrenade[idx]->setPos( (UINT32)(i3Vector::GetX( &vecPos)-43.0f), (UINT32)(i3Vector::GetY( &vecPos)-130.0f));
						else
							m_pTextGrenade[idx]->setPos( (UINT32)(i3Vector::GetX( &vecPos)-40.0f), (UINT32)(i3Vector::GetY( &vecPos)-124.0f));
					}
				}
			}

			count++;
		}
	}	
}

INT32 CHud::GetEnabledThrowWeaponList( i3::vector<WeaponBase*>& List, INT32 wpn, REAL32 rDistance)
{
	i3_prof_func();

	WeaponBase * pThrowWeapon;

	CGameCharaBase * pPlayer = GetCurrentChara();

	if( pPlayer == NULL)
		return 0;

	pThrowWeapon = g_pWeaponManager->getThrowWeapon( wpn, 0);
	I3ASSERT( pThrowWeapon != NULL);

	// 대표적으로 하나의 무기를 검사해본다.
	switch( pThrowWeapon->getWeaponInfo()->GetIndicatorType() )
	{
	case WEAPON_INDICATOR_GRENADE :		break;
	case WEAPON_INDICATOR_C5 :
		if( !pPlayer->isEnableTargetChase())	// C5는 목표추적이 있어야 감지됨
			return 0;
		break;
	default :
		return 0;
	}
	
	// 표시해야 하는 수류탄 파악
	for( INT32 i = 0; i < g_pWeaponManager->getThrowWeaponCount( wpn); i++)
	{
		pThrowWeapon = g_pWeaponManager->getThrowWeapon( wpn, i);

		if( __isIndicatorEnable( pThrowWeapon) == false)
			continue;

		// 아군의 수류탄은 경고에서 제외
		CGameCharaBase * pOwner = pThrowWeapon->getOwner();

		if( (pOwner != NULL) && (pOwner != pPlayer))
		{
			// 자신의 수류탄에 대해서는 Indicator를 켜야 한다.
			if( pOwner->getCharaTeam() == pPlayer->getCharaTeam())
			{
				// 아군의 수류탄
				continue;
			}
		}
		

		if( i3::vu::int_index_of(List, pThrowWeapon) == -1)
		{
			VEC3D diff;
			i3Vector::Sub( &diff, pPlayer->GetPos(), pThrowWeapon->GetPos());

			REAL32 dist = i3Vector::LengthSq( &diff);
			if( dist <= (rDistance * rDistance))
			{
				// 적의 수류탄은 인디게이터 리스트에 등록
				List.push_back( pThrowWeapon);
			}
		}
	}

	return (INT32)List.size();
}

void	CHud::UpdateIndicator( REAL32 timeStep)
{
	i3_prof_func();

	INT32 i;

#if defined( DEF_OBSERVER_MODE)
	if( g_pGameContext->isObserverMe())
		return;
#endif

	// 죽어 있으면 처리하지 않는다.
	CGameCharaBase * pPlayer = GetCurrentChara();

	if (pPlayer == NULL)
	{
		return;
	}

	if( pPlayer->isCharaStateMask( CHARA_STATE_DEATH) )
	{	
		for( i = 0; i < MAX_INDICATOR; i++)
		{
			m_pTextGrenade[i]->SetEnable(FALSE);
		}

		m_pGrenadeInd->SetEnable( FALSE);
		m_pGrenadeInd->RemoveFlag( I3_NODEFLAG_DISABLE);
		return;
	}

	// 초기화
	for( i = 0; i < MAX_INDICATOR; i++)
	{
		m_pGrenadeInd->SetEnable( i * 2, FALSE);
		m_pGrenadeInd->SetEnable( i * 2 +1, FALSE);
		m_pTextGrenade[i]->SetEnable( FALSE);
	}

	//수류탄 indicator
	bool bIndicated = false;

	m_IndicateWeaponList.clear();
	for( i = 1; i < g_pWeaponManager->GetWeaponCount( WEAPON_CLASS_THROWING1); i++)
	{
		if( GetEnabledThrowWeaponList( m_IndicateWeaponList, i, 20.f) > 0)
		{
			bIndicated = true;
		}
	}

	if( bIndicated)
	{
		UpdateThrowWeaponIndicator( timeStep, m_IndicateWeaponList);
		m_pGrenadeInd->RemoveFlag( I3_NODEFLAG_DISABLE);
	}
	else
		m_pGrenadeInd->AddFlag( I3_NODEFLAG_DISABLE);
}

void CHud::UpdateScore(BOOL bVisible)
{
	i3_prof_func();

	// 튜토리얼 모드는 결과창을 다른 방식으로 그려 준다.
	if(g_pGameContext->IsTutorialMode())
	{
		return;
	}
	m_pPanelScore->UpdateStageNormalScore( bVisible);
	HUD::instance()->SetParameter( HUD::DASH_BOARD, !bVisible );
}

void CHud::InitHUDNick()
{
	i3_prof_func();

	INT32 i = 0;

	// 캐릭터 닉네임
	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if (m_pTextName[i] == NULL)
			continue;

		if( m_pTextName[i]->GetEnable())
		{
			m_pTextName[i]->SetEnable( FALSE);
		}
	}	
}

void CHud::SetCharaNick( const char *pszNick, BOOL bFriendly, REAL32 fX, REAL32 fY, REAL32 fLength)
{

	i3_prof_func();

	INT32 i = 0;
	COLOR col;

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if( !m_pTextName[i]->GetEnable())
		{

i3_prof_start("SetCharaNick #1 Block");
			m_pTextName[i]->SetEnable( TRUE);

			// Nick
			m_pTextName[i]->SetTextEllipsis( pszNick);
i3_prof_end();

i3_prof_start("SetCharaNick #2 Block");
			// 색상
			{
				if( bFriendly)	// 아군
				{
					if( g_pEnvSet->m_bEnableFrindlyName)	// 옵션 설정에 따라서 캐릭터의 이름의 표시를 결정합니다.
					{
						i3Color::Set( &col, (UINT8) 255, 255, 255, 255);
					}
					else 
					{
						m_pTextName[i]->SetEnable( FALSE);
						continue;
					}
				}
				else			// 적군
					i3Color::Set( &col, (UINT8) 255, 0, 0, 255);

				m_pTextName[i]->SetColor( &col);
			}
i3_prof_end();

i3_prof_start("SetCharaNick #3 Block");
			// 좌표
			i3Viewer * pViewer		= g_pFramework->GetViewer();
			REAL32 fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextName[i]->getTextWidth())*0.5f);
			REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY - (fLength);
			m_pTextName[i]->setPos( (INT32)fPosX, (INT32)fPosY);	
			//m_pTextName[i]->setSize( m_pTextName[i]->getTextWidth(), m_pTextName[i]->getTextHeight());

			m_pTextName[i]->setSize( 400, 20);
i3_prof_end();

			break;

		}
	}
}

void CHud::SetCharaNickForGM( const char *pszNick, bool IsRed, REAL32 fX, REAL32 fY, REAL32 fLength)
{
	i3_prof_func();

	INT32 i = 0;
	COLOR col;

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if( !m_pTextName[i]->GetEnable())
		{
			m_pTextName[i]->SetEnable( TRUE);

			// Nick
			m_pTextName[i]->SetTextEllipsis( pszNick);

			// 색상
			{
				if( IsRed)	// 아군
				{
					i3Color::Set( &col, (UINT8) 255, 0, 0, 255);
				}
				else			// 적군
					i3Color::Set( &col, (UINT8) 0, 128, 255, 255);

				m_pTextName[i]->SetColor( &col);
			}

			// 좌표
			i3Viewer * pViewer		= g_pFramework->GetViewer();
			REAL32 fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextName[i]->getTextWidth())*0.5f);
			//REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY - (fLength);
			REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY;
			m_pTextName[i]->setPos( (INT32)fPosX, (INT32)fPosY);	
			//m_pTextName[i]->setSize( m_pTextName[i]->getTextWidth(), m_pTextName[i]->getTextHeight());

			m_pTextName[i]->setSize( 800, 40);
			break;
		}
	}
}

INT32 CHud::SetObjectReply( INT32 nType, REAL32 fX, REAL32 fY)
{
	i3_prof_func();

	INT32 i = 0;
//	COLOR col;
	INT32 nRndIdx;
	char szTemp[256];
	REAL32 fPosX = 0.0f;
	REAL32 fPosY = 0.0f;

	for(i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
	{
		if( !m_pTextObjectReply[i]->GetEnable())
		{
			m_pTextObjectReply[i]->SetEnable( TRUE);
			m_fDeltaTimeObjectReply[i] = 0.0f;

			switch( nType)
			{
			case 0:
				nRndIdx = i3Math::Rand()%5;
				
				switch( nRndIdx)
				{
				case 0:	
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD5"));/*이제 곧 포인트 블랭크가 FPS를 장악 하리라!\n포블을 외치는 자*/
					break;
				case 1:	
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD7") );/*모두들 포블하라! 곧 이벤트 여신이 강림하리니!\n포블을 외치는 자*/					
					break;
				case 2:	
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD2") );/*포블의 예언에 귀를 기울여라! 곧 포블의 시대가 시작될지니!\n포블을 외치는 자*/					
					break;
				case 3:	
					{
						const USER_INFO_BASIC* pMyInfo = g_pGameContext->GetUserInfoBasic();
						sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD8"), pMyInfo->_nick);/*[%s] 그대에게 포블의 권능을 내리노라!\n포블을 외치는 자*/					
					}
					break;
				case 4:	
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD6") );/*무기를 정비 하라! 그대에게 Extension 의 축복을 내리노라!\n포블을 외치는 자*/					
					break;
				default:
					break;
				}
				break;
			case 1:
				nRndIdx = i3Math::Rand()%3;
				switch( nRndIdx)
				{
				case 0:	
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD4") );/*포블의 계시에 귀를 기울여라! 잠들었던 FPS가 눈을 떴노라!\n포블을 설파하는 자*/					
					break;
				case 1:			
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD1") );/*포인트 블랭크의 날이 가까워 졌다!\n포블을 설파하는 자*/		
					break;
				case 2:
					sprintf_s( szTemp, GAME_STRING("STR_TBL_INGAME_HUD_NPC_WORD3") );/*포블의 계시에 귀를 기울여라! 타격 전율의 시대가 시작되었다!\n포블을 설파하는 자*/		
					break;
				default:
					break;
				}
				break;
			default:	
				I3ASSERT_0;;
				break;
			}

			// 텍스트 입력
			{
				m_pTextObjectReply[i]->SetText(szTemp);

				// 좌표
				i3Viewer * pViewer	= g_pFramework->GetViewer();
				fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextObjectReply[i]->getTextWidth())*0.5f);
				fPosY = (REAL32)pViewer->GetViewHeight() * fY;
				m_pTextObjectReply[i]->setPos( (INT32)fPosX, (INT32)fPosY);				
				m_pTextObjectReply[i]->setSize( m_pTextObjectReply[i]->getTextWidth(), 27);
			}
			
			// 텍스트 배경
			{
				m_pSpriteObjectReply->SetEnable( i, TRUE);
				m_pSpriteObjectReply->SetRect( i, fPosX, fPosY, (REAL32)m_pTextObjectReply[i]->getTextWidth(), 14.0f);
			}

			return i;
		}
	}

	return 0;
}

void CHud::SetObjectReplyPos( INT32 idx, REAL32 fX, REAL32 fY)
{

	i3_prof_func();

	I3ASSERT( idx < 5);

	// 좌표
	i3Viewer * pViewer	= g_pFramework->GetViewer();
	REAL32 fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextObjectReply[idx]->getTextWidth())*0.5f);
	REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY;
	m_pTextObjectReply[ idx]->setPos( (INT32)fPosX, (INT32)fPosY);

	m_pSpriteObjectReply->SetRect( idx, fPosX, fPosY, (REAL32)m_pTextObjectReply[idx]->getTextWidth(), 14.0f);
}

void CHud::DisableObjectReply( INT32 idx)
{
	i3_prof_func();

	m_pTextObjectReply[idx]->SetText("");
	m_pTextObjectReply[idx]->SetEnable( FALSE);
	m_pSpriteObjectReply->SetEnable( idx, FALSE);
}

void CHud::ShowHUDNick( void )
{	
	i3_prof_func();

	INT32 i;
	CGameCharaBase * pChara = NULL;
	INT32	MyTeam = g_pGameContext->getMyTeam();

	InitHUDNick();

	//캐릭터 닉네임
	{
		// 옵저버 모드일경우에는 Chara Nick을 보여주지 않습니다.
		if( (m_bEnableObserverMode || m_bEnableObserverFlyMode) &&
#if defined( DEF_OBSERVER_MODE)
			g_pGameContext->isObserverMe() == false)
#else
			g_pGameContext->IsGM_Observer() == false)
#endif
			return;

		for( i = 0; i < g_pCharaManager->getCharaCount(); i++ )
		{
			pChara = g_pCharaManager->getChara( i );
			if( pChara != NULL )
			{
				if( pChara->getCharaNetIndex() == -1)
					continue;

				if( pChara->isAIToClient() == false && g_pGameContext->IsPlayingSlot( pChara->getCharaNetIndex()) == FALSE)
					continue;

				CGameCharaMaterialContext * pMaterial = pChara->getMaterialContext();
				
				if (pMaterial == NULL)
					continue;

				INT32 slot = pChara->getCharaInfo()->GetNetIdx();
				
				if( !(pChara->getCharaInfo()->GetAIToClient()) && g_pGameContext->isHackUserCaution( slot))
				{
					pChara->Cmd_SetCharaNick( false );

					pMaterial->EnablePenalty();
				}
				else
				{
					pMaterial->DisablePenalty();
				}

				// 조작중인 캐릭터의 경우 그냥 넘어갑니다.
				if( pChara->isLocal())
				{
					if( pChara->IsOnObject() )
					{
						WeaponOnObject * pWeapon = pChara->GetOnObject()->GetWeaponOnObject();
						if( i3::same_of<WeaponM197*>(pWeapon)) //->IsExactTypeOf( WeaponM197::static_meta()) )
							((WeaponM197*)pWeapon)->CheckCrossHit();
					}
					else
					{
						if (pChara->getCollisionContext())
							pChara->getCollisionContext()->UpdateNickOnTarget();
						else
							TRACE("CHud::ShowHUDNick pChara->getCollisionContext()=NULL\n");
					}
					continue;
				}

				GLOBAL_CHARA_INFO * pCharaInfo = pChara->getCharaInfo();

#if defined( DEF_OBSERVER_MODE)
				if( g_pGameContext->isObserverMe())
#else
				if( g_pGameContext->IsGM_Observer())
#endif
				{
					if(g_pGameContext->IsViewHudNick()) {
						pChara->Cmd_SetCharaNick( true);
					}
				}
				else if( MyTeam == (INT32) pCharaInfo->GetTeam() )
				{
					if( LocaleValue::Enable( "ForciblyRemove" ) )
					{
						I3ASSERT(UIBattleFrame::i());

						if (UIBattleFrame::i()->GetPopup(UBP_ACCUSE_CRIME)->isEnable() == FALSE)
							pChara->Cmd_SetCharaNick( true);
					}
					else
					{
						pChara->Cmd_SetCharaNick( true);
					}
				}

				///AI 케릭터 닉네임 
				if( g_pGameContext->IsStageMode( STAGE_TYPE_DOMINATION))
				{
					if( pCharaInfo->GetAIToClient() && pChara->IsAttachedScene() )
						pChara->Cmd_SetCharaNick( true);
				}	
			}
		}
	}
	
}

void CHud::SetGameEnd(void)
{
	i3_prof_func();

	m_bGameEnd = TRUE;

	if( m_pScope != NULL)
		m_pScope->SetEnableSinperScope( FALSE);
	SetEnableObserver(FALSE);
	SetEnableChallengeBoard(FALSE);
	SetEnableMiniMap(FALSE);

	if( m_pPanelTargetHP != NULL)
		m_pPanelTargetHP->SetEnableTargetHP( FALSE);
}

void CHud::SetEnableObserverFly(BOOL bEnable)
{
	i3_prof_func();

	m_bEnableObserverFlyMode = bEnable;

	for(INT32 i = 0; i < HUD_TEXT_OBSERVER_FLY_COUNT; i++)
	{
		if (m_pTextObserverFly[i])
			m_pTextObserverFly[i]->SetEnable(bEnable);
	}	
}

void CHud::SetEnableObserver(BOOL bEnable)
{
	i3_prof_func();

	if( m_bEnableObserverMode == bEnable ) return;

	m_bEnableObserverMode = bEnable;

    m_pSpriteObserver->SetEnable(m_bEnableObserverMode);

	m_pObserverHPGaugeHUD->SetEnableGauge( bEnable);
	
	for(INT32 i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++)
	{
		m_pTextObserver[i]->SetEnable(m_bEnableObserverMode);
	}	
}

void CHud::SetObservedNick(const char* strNick, bool bIsRed)
{
	i3_prof_func();

	m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetTextEllipsis((char*)strNick);
	
	if( bIsRed)
	{
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(255, 0, 0, 255);
	}
	else
	{
		if(g_pGameContext->IsGM_Observer())
		{
			m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(0, 128, 255, 255);
		}
		else
		{
			m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(100, 100, 255, 255);
		}
		
	}
}

void CHud::AddChainKills(INT32 nKillMessage,INT32 nKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, BOOL revengeMsg)
{
	i3_prof_func();

	m_pChainKills->AddChainKills(nKillMessage, nKillCount, nNowKill, nWeaponSlot, chainHeadshotCount, chainStopperCount, revengeMsg);
}
void CHud::AddTouchDown(INT16 nTouchDown,INT32 nSlotIdx)
{
	i3_prof_func();

	m_pChainKills->AddTouchDown(nTouchDown, nSlotIdx);
}


void CHud::AddHelmetProtection(void)
{
	i3_prof_func();

	m_pChainKills->AddHelmetProtection();

	g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STR_TBL_INGAME_HUD_NOTICE_REMOVE_HELMET"));/*{col:255,120,0,255}헬멧에 의해 헤드샷으로부터 보호되었습니다. 다음 리스폰 까지는 더 이상 헬멧으로부터 보호받을 수 없습니다.{/col}*/
	g_pFramework->getChatBox()->ResetBackupBuffer();
	UIBattleFrame::i()->UpdateChatBuffer();
}

void CHud::AddTargetBonus( INT32 exp)
{
	i3_prof_func();

	char szExp[ 256];

	m_pChainKills->AddTargetBonus();

	sprintf_s( szExp, GAME_STRING("STR_TBL_INGAME_HUD_GET_TARGET_EXP"), exp);

	g_pFramework->getChatBox()->PutSystemChatting( szExp);/*{col:255,120,0,255}미션 목표물에 Damage를 주어 경험치 %d획득{/col}*/
	g_pFramework->getChatBox()->ResetBackupBuffer();
	UIBattleFrame::i()->UpdateChatBuffer();
}

void CHud::AddWeapon_BuildUp()
{
	i3_prof_func();

	g_pFramework->getChatBox()->PutSystemChatting("모든 무기 공격력 30% 증가");
	g_pFramework->getChatBox()->ResetBackupBuffer();
	UIBattleFrame::i()->UpdateChatBuffer();
}


void CHud::AddHP_Recovery()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_HP_RECOVERY);

	g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STR_TBL_INGAME_HUD_RAPTOR_HP_RECOVERY"));
	g_pFramework->getChatBox()->ResetBackupBuffer();
	UIBattleFrame::i()->UpdateChatBuffer();
}
void CHud::AddAssist_Kill()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_ASSISTKILL);
}
void CHud::AddEscapeBlock()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_HOLDTHELINE);
}
void CHud::AddMobilityUp()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_MOBILITYUP);
}
void CHud::AddDeathBlow()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_DEATH_BLOW);

	g_pFramework->getChatBox()->PutSystemChatting( GAME_STRING("STR_TBL_INGAME_HUD_DEATH_BLOW"));
	g_pFramework->getChatBox()->ResetBackupBuffer();
	UIBattleFrame::i()->UpdateChatBuffer();
}

void CHud::AddChallengeClear(void)
{
	i3_prof_func();

	m_pChainKills->AddChallengeClear();
}

void CHud::AddTrainingKills(INT32 killCount)
{
	i3_prof_func();
	// 훈련전아 아니면, 점수창도 없다.
	m_pChainKills->AddTrainingKills(killCount);
}

void CHud::ResetKillMessage(void)
{
	i3_prof_func();

	m_pChainKills->Reset();
}

BOOL CHud::GetEnableChatScroll(void)
{
	i3_prof_func();
	return ((UIHUDIngameChat *)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING))->GetEnableChatScroll();
}

BOOL CHud::IsEnableGui(void)
{
	i3_prof_func();

	if (m_pParent)
	{
		return ((CStageBattle*)m_pParent)->IsEnableGui();
	}

	return FALSE;
}

void CHud::SetDamageFilterInGame( i3Texture* pTex )
{
	i3_prof_func();

	I3_SAFE_RELEASE( m_pDamageFilterTexture );		

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pDamageFilterTexture = pTex;

	m_pDamageFilterSprite->SetTexture(m_pDamageFilterTexture);
	m_pDamageFilterSprite->SetEnable(FALSE);

	m_pDamageFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDamageFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
	m_pDamageFilterSprite->SetColor(0, 255, 255, 255, 255);

	m_pDamageFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDamageFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
	m_pDamageFilterSprite->SetColor(1, 255, 255, 255, 255);
}

void CHud::ClearDamageFilter(void)
{
	i3_prof_func();

	m_pDamageFilterSprite->SetEnable(FALSE);
}

void CHud::OnDamageFilter(void)
{
	i3_prof_func();

	m_rDamageFilterTime = 0.0f;
	m_pDamageFilterSprite->SetEnable(TRUE);
}

void CHud::OnUpdateDamageFilter(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if (m_pDamageFilterSprite != NULL &&
		m_pDamageFilterSprite->GetEnable(0))
	{		
		m_rDamageFilterTime += rDeltaSeconds;

		// cos()로 알파값을 빼면서 2초간 보여준다.
		INT32 alpha = INT32(255.0f * i3Math::cos(m_rDamageFilterTime / 2.0f));
		alpha = MINMAX(0, alpha, 255);

		if (0 < alpha)
		{
			m_pDamageFilterSprite->SetColor(0, 255, 255, 255, (UINT8) (alpha));
			m_pDamageFilterSprite->SetColor(1, 255, 255, 255, (UINT8) (alpha));
		}
		else
		{
			m_pDamageFilterSprite->SetEnable(FALSE);
		}
	}
}

void CHud::RecordRevenge(INT32 index, INT32 nPoint)
{
	i3_prof_func();

	INT32 i = 0;	
	SLOT_INFO * pSlotInfo;
	char szTemp[256];
	INT32 MySlotIdx = g_pGameContext->getMySlotIdx();

	const char * pszTarget = g_pGameContext->getNickForSlot(index);
	I3ASSERT( pszTarget != NULL);

	if(g_pGameContext->IsRedTeam())
	{
		// Red
		for(i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(g_pGameContext->IsRedTeam(i) == false)
			{	
				pSlotInfo = g_pGameContext->getSlotInfo(i);
								
				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( i3String::CompareCase( m_Revenge[i].szTargetName, g_pGameContext->getNickForSlot(i)) != 0)
					{
						i3String::Copy( m_Revenge[i].szTargetName, g_pGameContext->getNickForSlot(i), 100 );
						m_Revenge[i].szTargetName[i3String::Length( g_pGameContext->getNickForSlot(i))] = 0;
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}
	else
	{
		// Blue
		for(i=0; i< SLOT_MAX_COUNT; i++)
		{				
			if(g_pGameContext->IsRedTeam(i))
			{	
				pSlotInfo = g_pGameContext->getSlotInfo(i);
								
				if(pSlotInfo->_State == SLOT_STATE_BATTLE)
				{
					if( i3String::CompareCase( m_Revenge[i].szTargetName, g_pGameContext->getNickForSlot(i)) != 0)
					{
						i3String::Copy( m_Revenge[i].szTargetName, g_pGameContext->getNickForSlot(i), 100 );
						m_Revenge[i].szTargetName[i3String::Length( g_pGameContext->getNickForSlot(i))] = 0;
						m_Revenge[i].nWinPoint = 0;
						m_Revenge[i].nLosePoint = 0;
					}
				}
			}
		}
	}

	for( i=0; i<SLOT_MAX_COUNT; i++)
	{
		if( i3String::CompareCase( m_Revenge[i].szTargetName, pszTarget) == 0)
		{
			if( nPoint < 0)
			{
				m_Revenge[i].nLosePoint -= nPoint;
				m_bEnableRevengeUpdate = TRUE;
				m_fRevengeLocalTime = 0.0f;
			}
			else
			{
				m_Revenge[i].nWinPoint += nPoint;
			}

			if( g_pGameContext->IsRedTeam(index))
			{
				// Red(Enemy)
				m_pRevengeTextRed->SetTextEllipsis( m_Revenge[i].szTargetName);				
				sprintf_s( szTemp, "%d", m_Revenge[i].nLosePoint);
				m_pRevengeTextRedCount->SetText( szTemp);

				// Blue(Me)
				m_pRevengeTextBlue->SetTextEllipsis( g_pGameContext->getNickForSlot(MySlotIdx));
				sprintf_s( szTemp, "%d", m_Revenge[i].nWinPoint);
				m_pRevengeTextBlueCount->SetText( szTemp);

				if( m_Revenge[i].nWinPoint > m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextBlueCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
				}
				else if(m_Revenge[i].nWinPoint < m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextRedCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}		
				else
				{
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}	
			}
			else
			{
				// Blue(Enemy)
				m_pRevengeTextBlue->SetTextEllipsis( m_Revenge[i].szTargetName);
				sprintf_s( szTemp, "%d", m_Revenge[i].nLosePoint);
				m_pRevengeTextBlueCount->SetText( szTemp);

				// Red(Me)
				m_pRevengeTextRed->SetTextEllipsis( g_pGameContext->getNickForSlot(MySlotIdx));
				sprintf_s( szTemp, "%d", m_Revenge[i].nWinPoint);
				m_pRevengeTextRedCount->SetText( szTemp);
				
				if( m_Revenge[i].nWinPoint > m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextRedCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}
				else if(m_Revenge[i].nWinPoint < m_Revenge[i].nLosePoint)
				{
					m_pRevengeTextBlueCount->SetColor( 255, 200, 100, 255);
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
				}	
				else
				{
					m_pRevengeTextRedCount->SetColor( 255, 255, 255, 255);
					m_pRevengeTextBlueCount->SetColor( 255, 255, 255, 255);
				}	
			}	
			break;
		}
	}
}

void CHud::DeleteRecordRevenge(INT32 index)
{
	i3_prof_func();

	if( g_pEnvSet->m_b1VS1)
	{
		I3ASSERT( index < 16 && index >= 0);

		m_Revenge[index].szTargetName[0] = 0;
		m_Revenge[index].nWinPoint = 0;
		m_Revenge[index].nLosePoint = 0;
	}
}

void CHud::UpdateRevenge(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if( g_pEnvSet->m_b1VS1)
	{
		m_fRevengeLocalTime += rDeltaSeconds;

		if( m_fRevengeLocalTime < 4.0f)
		{
			SetRevenge( TRUE);
		}
		else
		{
			SetRevenge( FALSE);		
		}
	}
}

void CHud::SetRevenge(BOOL bTrue)
{
	i3_prof_func();

	if( g_pEnvSet->m_b1VS1)
	{
		if (m_pRevengeInfo)
		{
			m_pRevengeInfo->SetEnable(0, bTrue);
			m_pRevengeTextRed->SetEnable(bTrue);
			m_pRevengeTextBlue->SetEnable(bTrue);
			m_pRevengeTextRedCount->SetEnable(bTrue);
			m_pRevengeTextBlueCount->SetEnable(bTrue);	
		}

		if( !bTrue)
		{
			m_bEnableRevengeUpdate = FALSE;
			m_fRevengeLocalTime = 0.0f;
		}
	}
}

BOOL CHud::_UpdateAttackedEffect(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	BOOL bEnabledEffect = FALSE;

	static REAL32 fOldAngle = 0.0f;
	REAL32 fCurrentAngle = 0.0f;
	REAL32 fDeltaAngle;

	CGameCharaBase * pPlayer = GetCurrentChara();

	if( pPlayer == NULL)
		return FALSE;

	fCurrentAngle = pPlayer->getBoneContext()->getTheta();
				
	// 이전에 바라보던 방향과 현재 바라보는 방향의 차이각을 구함
	fDeltaAngle = i3Math::NormalizeAngle( fOldAngle - fCurrentAngle);
	fOldAngle = fCurrentAngle;

	// 버그 수정 (유동 frame일 경우 rDeltaSeconds가 너무 작아 alpha가 안빠지는 문제 발생 2010.08.17 순구
	if(	rDeltaSeconds < 0.01)
		rDeltaSeconds = 0.01f;

	for( INT32 i = 0; i < HUD_SPRITE_HIT_ANGLE_MAX; i++)
	{			
		if( m_pSpriteHit->GetEnable( HUD_SPRITE_HIT_ANGLE_START + i))
		{
			//I3TRACE1("DeltaAngle => %f\n", fDeltaAngle);

			// Hit 방향을 위에서 구한 차이각만큼 회전
			// i3Sprite2D::SetRotation()에서 넣은 값을 받아올 GetRotaion() 함수가 없다
			m_fDamageDirectionAngle[i] = i3Math::NormalizeAngle( m_fDamageDirectionAngle[i] + fDeltaAngle);
			m_pSpriteHit->SetCenter(	HUD_SPRITE_HIT_ANGLE_START + i, 
										m_fViewHalfWidth - (140.0f * i3Math::sin(m_fDamageDirectionAngle[i])), 
										m_fViewHalfHeight - (140.0f * i3Math::cos(m_fDamageDirectionAngle[i])), 0.0f);

			m_pSpriteHit->SetRotation( HUD_SPRITE_HIT_ANGLE_START + i, m_fDamageDirectionAngle[i]);
			
			COLOR *pColor = m_pSpriteHit->GetColor(HUD_SPRITE_HIT_ANGLE_START + i);
			INT32 nAlpha = pColor->a;

			nAlpha -= (INT32)(HIT_DIRECTION_DECREASE * rDeltaSeconds);

			if (0 >= nAlpha)
			{
				m_pSpriteHit->SetEnable(HUD_SPRITE_HIT_ANGLE_START + i, FALSE);
			}
			else
			{
				pColor->a = (UINT8)nAlpha;
				bEnabledEffect = TRUE;
			}
		}
	}			

	return bEnabledEffect;
}

BOOL CHud::_UpdateGrazedEffect(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	BOOL bEnabledEffect = FALSE;

	for(INT32 i = HUD_SPRITE_WARNING_LEFT_FRONT; i <= HUD_SPRITE_WARNING_RIGHT_BACK; i++)
	{
		if (m_pSpriteHit->GetEnable(i))
		{
			COLOR* pColor = m_pSpriteHit->GetColor(i);
			INT32 nAlpha = pColor->a;

			nAlpha -= (INT32)(WARNING_DIRECTION_DECREASE * rDeltaSeconds);

			if (0 >= nAlpha)
			{
				m_pSpriteHit->SetEnable(i, FALSE);
			}
			else
			{
				pColor->a = (UINT8)nAlpha;
				bEnabledEffect = TRUE;
			}
		}
	}

	return bEnabledEffect;
}

void CHud::_ClearGrazedEffect(void)
{
	i3_prof_func();

	for(INT32 i = 0; i < HUD_SPRITE_WARNING_MAX; i++)
	{
		m_pSpriteHit->SetEnable(HUD_SPRITE_WARNING_LEFT_FRONT + i, FALSE);
	}
}

BOOL CHud::_IsAttackedEffectEnabled(void)
{
	i3_prof_func();

	for( INT32 i = 0; i < HUD_SPRITE_HIT_ANGLE_MAX; i++)
	{			
		if (m_pSpriteHit->GetEnable(HUD_SPRITE_HIT_ANGLE_START + i))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CHud::UpdateRadioChat( REAL32 fTime )
{
	i3_prof_func();

	INT32	i, ChatNum, nLine;
	char	szTemp[ 128 ] = {0,};
	const char * pszMessage = NULL;
	RADIOCHAT_TYPE	ChatType;

	nLine = 0;

	m_rRadioReusingTimer += fTime;
	if( m_rRadioReusingTimer < 2.f )
		return;

	CGameCharaBase * pPlayer = GetCurrentChara();

	if( pPlayer == NULL || g_pGameContext->getUserDeath( g_pGameContext->getMySlotIdx() ) )
	{
		// clear message
		for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
		{
			m_pTextRadioChat[i]->SetText( "" );
			m_pTextRadioChat[i]->SetEnable( FALSE );
		}
		m_bEnableRadioChat = FALSE;
		m_bEnableRadio1 = FALSE;
		m_bEnableRadio2 = FALSE;
		m_bEnableRadio3 = FALSE;

		return;
	}

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO1) )
	{
		if( !m_bEnableRadio1 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{	
				// 9번은 없음
				if(i==9) {
					continue;
				}
				pszMessage = g_pGameContext->GetRadioMessage( RADIOCHAT_TYPE_COMMAND, i );
				if( i3String::Length( (const char*) pszMessage ) > 0 )
				{
					if( i != 0 )
						sprintf_s( szTemp, "%d. ", i % 10 );
					i3String::Concat( szTemp, pszMessage );
					m_pTextRadioChat[nLine]->SetText( szTemp );
					m_pTextRadioChat[nLine]->SetEnable( TRUE );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}

			m_bEnableRadioChat = TRUE;
			m_bEnableRadio1 = TRUE;
			m_bEnableRadio2 = FALSE;
			m_bEnableRadio3 = FALSE;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}
			m_bEnableRadioChat = FALSE;
			m_bEnableRadio1 = FALSE;
		}
	}

	nLine = 0;

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO2) )
	{
		if( !m_bEnableRadio2 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				// 9번은 없음
				if(i==9) {
					continue;
				}
				pszMessage = g_pGameContext->GetRadioMessage( RADIOCHAT_TYPE_REQUEST, i );
				if( i3String::Length( pszMessage ) > 0 )
				{
					if( i != 0 )
						sprintf_s( szTemp, "%d. ", i % 10 );
					i3String::Concat( szTemp, pszMessage );
					m_pTextRadioChat[nLine]->SetText( szTemp );
					m_pTextRadioChat[nLine]->SetEnable( TRUE );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}

			m_bEnableRadioChat = TRUE;
			m_bEnableRadio2 = TRUE;
			m_bEnableRadio1 = FALSE;
			m_bEnableRadio3 = FALSE;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}
			m_bEnableRadioChat = FALSE;
			m_bEnableRadio2 = FALSE;
		}
	}

	nLine = 0;

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO3) )
	{
		if( !m_bEnableRadio3 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				pszMessage = g_pGameContext->GetRadioMessage( RADIOCHAT_TYPE_STATUS, i );
				if( i3String::Length( pszMessage ) > 0 )
				{
					if( i != 0 )
						sprintf_s( szTemp, "%d. ", i % 10 );
					i3String::Concat( szTemp, pszMessage );
					m_pTextRadioChat[nLine]->SetText( szTemp );
					m_pTextRadioChat[nLine]->SetEnable( TRUE );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}

			m_bEnableRadioChat = TRUE;
			m_bEnableRadio3 = TRUE;
			m_bEnableRadio1 = FALSE;
			m_bEnableRadio2 = FALSE;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( "" );
				m_pTextRadioChat[i]->SetEnable( FALSE );
			}
			m_bEnableRadioChat = FALSE;
			m_bEnableRadio3 = FALSE;
		}
	}

	if( m_bEnableRadioChat )
	{
		ChatType = RADIOCHAT_TYPE_COMMAND;
		ChatNum = 0;
		
		if( m_bEnableRadio1 )
		{
			ChatType = RADIOCHAT_TYPE_COMMAND;
			ChatNum = 0;
			if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ) )		ChatNum = 1;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ) )	ChatNum = 2;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ) )	ChatNum = 3;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ) )	ChatNum = 4;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ) )	ChatNum = 5;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_6 ) )	ChatNum = 6;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_7 ) )	ChatNum = 7;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_8 ) )	ChatNum = 8;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_9 ) )	ChatNum = 9;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_0 ) )
			{
				// clear message
				for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
				{
					m_pTextRadioChat[i]->SetText("");
					m_pTextRadioChat[i]->SetEnable( FALSE );	
				}
				m_bEnableRadioChat = FALSE;
				m_bEnableRadio1 = FALSE;
			}
		}

		if( m_bEnableRadio2 )
		{
			ChatType = RADIOCHAT_TYPE_REQUEST;
			ChatNum = 0;
			if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ) )		ChatNum = 1;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ) )	ChatNum = 2;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ) )	ChatNum = 3;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ) )	ChatNum = 4;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ) )	ChatNum = 5;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_6 ) )	ChatNum = 6;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_7 ) )	ChatNum = 7;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_8 ) )	ChatNum = 8;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_9 ) )	ChatNum = 9;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_0 ) )
			{
				// clear message
				for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
				{
					m_pTextRadioChat[i]->SetText("");
					m_pTextRadioChat[i]->SetEnable( FALSE );	
				}
				m_bEnableRadioChat = FALSE;
				m_bEnableRadio2 = FALSE;
			}
		}

		
		if( m_bEnableRadio3 )
		{
			ChatType = RADIOCHAT_TYPE_STATUS;
			ChatNum = 0;
			if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_1 ) )		ChatNum = 1;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_2 ) )	ChatNum = 2;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_3 ) )	ChatNum = 3;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_4 ) )	ChatNum = 4;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_5 ) )	ChatNum = 5;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_6 ) )	ChatNum = 6;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_7 ) )	ChatNum = 7;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_8 ) )	ChatNum = 8;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_9 ) )	ChatNum = 9;
			else if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_0 ) )
			{
				// clear message
				for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
				{
					m_pTextRadioChat[i]->SetText("");
					m_pTextRadioChat[i]->SetEnable( FALSE );	
				}
				m_bEnableRadioChat = FALSE;
				m_bEnableRadio3 = FALSE;
			}
		}


		if( ChatNum > 0 )
		{
			if( (ChatType == RADIOCHAT_TYPE_COMMAND && ChatNum != 9) ||
				(ChatType == RADIOCHAT_TYPE_REQUEST && ChatNum != 9) ||
				(ChatType == RADIOCHAT_TYPE_STATUS))
			{
				GAMEEVENT::Write_Chara_RadioChat( pPlayer, (UINT8)(ChatNum + (10*ChatType)));

				if( g_pGameContext->IsP2PHost())
				{
					char szMessage[256];
					const char * pszMessage = g_pGameContext->GetRadioMessage( ChatType, ChatNum );
					if( i3String::Length( pszMessage ) > 0 )
					{
						const char * pszNick = g_pGameContext->getNickForSlot(g_pGameContext->getMySlotIdx());

						if( i3String::Length( getMiniMapLocationText()) > 0)
						{
							sprintf_s( szMessage, "[Radio] %s {col:98, 180, 253, 255}[%s]{/col}", pszMessage, getMiniMapLocationText());
						}
						else
						{
							sprintf_s( szMessage, "[Radio] %s", pszMessage );
						}						

						g_pFramework->getChatBox()->PutUserChatting( pszNick, szMessage ,0);
						g_pGameContext->PlayRadioMessageSound( ChatType, ChatNum, pPlayer);
					}
				}

				switch( ChatType )
				{
				case RADIOCHAT_TYPE_COMMAND : m_bEnableRadio1 = FALSE; break;
				case RADIOCHAT_TYPE_REQUEST : m_bEnableRadio2 = FALSE; break;
				case RADIOCHAT_TYPE_STATUS : m_bEnableRadio3 = FALSE; break;
				}

				// clear message
				for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
				{
					m_pTextRadioChat[i]->SetText("");
					m_pTextRadioChat[i]->SetEnable( FALSE );	
				}
				m_bEnableRadioChat = FALSE;

				m_rRadioReusingTimer = 0.f;
			}
		}
	}
}

bool CHud::GetChatEnable(void) const
{
	i3_prof_func();

	return ((UIHUDIngameChat *)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING))->GetEnableChat();
}

void	CHud::SetChatEnable(bool bFlag)
{
	i3_prof_func();

	UIHUDIngameChat * pChat = (UIHUDIngameChat *)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
	pChat->SetEnableChat( bFlag );
}

void CHud::_ProcessChatNotice( void)
{
	i3_prof_func();

	INT32 nTime = (INT32) g_pGameContext->GetAccTime();

	if( nTime % 3600 == 0 )	// 1 hour
	{
		if( !m_bShowChatNotice )
		{
			char szNotice[ 256] = {0,};

			if( m_nCurrentChatNotice == 0)
			{
				if( g_pGameContext->IsAdultUser())
					sprintf_s( szNotice, GAME_STRING("STR_TBL_INGAME_HUD_NOTICE_GAME_GRADE1"));/*{col:255,255,0,255}포인트 블랭크는 게임 산업 진흥에 의한 법률 제21조 2항 게임 등급 분류 기준에 의거하여 18세 이상 이용가임을 알려드립니다.{/col}*/
				else
					sprintf_s( szNotice, GAME_STRING("STR_TBL_INGAME_HUD_NOTICE_GAME_GRADE2"));/*{col:255,255,0,255}포인트 블랭크는 게임 산업 진흥에 의한 법률 제21조 2항 게임 등급 분류 기준에 의거하여 15세 이상 이용가임을 알려드립니다.{/col}*/

				m_nCurrentChatNotice = 1;
			}
			else
			{
				sprintf_s( szNotice, GAME_STRING("STR_TBL_INGAME_HUD_HELP_USE_TEXT"));/*게임 중 F1 키를 누르시면 도움말을 보실 수 있습니다.*/
				m_nCurrentChatNotice = 0;
			}

			g_pFramework->getChatBox()->PutSystemChatting( szNotice);
			g_pFramework->getChatBox()->ResetBackupBuffer();
			UIBattleFrame::i()->UpdateChatBuffer();
			m_bShowChatNotice = TRUE;
		}
	}
	else
	{
		m_bShowChatNotice = FALSE;
	}
}

void CHud::EditCharaNick( char* pszTarget, const char* pszSource, INT32 LimitWidth)
{
	i3_prof_func();

	if (!m_pTextDummy)
		return;
		
	m_pTextDummy->SetText( pszSource);
	i3TextAttrDX2* pAttr = m_pTextDummy->getTextAttrDX();

	// 글폭이 컨트롤폭을 넘어가면 "..."으로 끊는다
	if (pAttr->getTextWidth() > LimitWidth)
	{
		INT32 ellipsisLength = 0;

		// "..." 폭 구하기
		pAttr->SetText("...");
		REAL32 margin = (REAL32)(LimitWidth - pAttr->getTextWidth());

		// "..." 폭을 제외한 여유공간에 들어갈 글자 계산
		INT32 length = i3String::Length(pszSource);
		for(INT32 i = 0; i < length; i++)
		{
			margin -= pAttr->GetCharacterSizeX(pszSource + i);

			if (0.0f > margin)
			{
				break;
			}

			// 멀티바이트 문자를 위한 문자단위 증가
			if (0 != i3String::WhatIsCode(pszSource, i))
			{
				i++;
			}

			ellipsisLength = i + 1;
		}

		// 계산한 위치에 "..."을 붙인다
		i3String::NCopy(pszTarget, pszSource, ellipsisLength);		
		i3String::Concat(pszTarget, "...");				
		pszTarget[i3String::Length(pszTarget)] = 0;
	}
	else
	{
		i3String::NCopy(pszTarget, pszSource, i3String::Length(pszSource));
		pszTarget[i3String::Length(pszSource)] = 0;
	}
}


void CHud::ChangeChallenge(void)
{
	i3_prof_func();

#if defined(ENABLE_MISSION_CARD)
	ISNETWORKRETURN(false)
	m_pPanelScore->ChangeChallenge();
#endif
}

void CHud::SetEnableChallengeBoard(BOOL bEnable)
{
	i3_prof_func();

	if( g_pEnvSet->m_bChallange == FALSE)
	{
		bEnable = FALSE;
	}
}

void CHud::Bubble( SCORE_ARRY *pArr, INT32 num)
{
	i3_prof_func();

	INT32 i = 0, j = 0;
	SCORE_ARRY temp; 

	for( i=0; i< num-1; i++)
	{
		for( j=0; j< num-i-1; j++)
		{
			if( pArr[j].nKillCount < pArr[j+1].nKillCount)
			{
				i3mem::Copy( &temp, &pArr[j+1], sizeof(SCORE_ARRY));
				i3mem::Copy( &pArr[j+1], &pArr[j], sizeof(SCORE_ARRY));
				i3mem::Copy( &pArr[j], &temp, sizeof(SCORE_ARRY));
			}
			else if( pArr[j].nKillCount == pArr[j+1].nKillCount)
			{
				if( pArr[j].nDeath > pArr[j+1].nDeath)
				{
					i3mem::Copy( &temp, &pArr[j+1], sizeof(SCORE_ARRY));
					i3mem::Copy( &pArr[j+1], &pArr[j], sizeof(SCORE_ARRY));
					i3mem::Copy( &pArr[j], &temp, sizeof(SCORE_ARRY));
				}
			}
		}
	}
}

void CHud::SetEnableGrenadeLauncherCrossHair( BOOL bVisible)
{
	i3_prof_func();

	if( bVisible)
	{
		m_pSpriteGrenadeLauncherCrossHair->SetEnable( bVisible);
		m_pSpriteGrenadeLauncherCrossHair->RemoveFlag( I3_NODEFLAG_DISABLE);
		m_bEnabledGrenadeLauncherCrossHair = true;
	}
	else
	{
		if (m_pSpriteGrenadeLauncherCrossHair)
			m_pSpriteGrenadeLauncherCrossHair->AddFlag( I3_NODEFLAG_DISABLE);
		else
			TRACE("CHud::SetEnableGrenadeLauncherCrossHair m_pSpriteGrenadeLauncherCrossHair = NULL\n");
		m_bEnabledGrenadeLauncherCrossHair = false;
	}
}

CGameCharaBase * CHud::GetCurrentChara( void)
{	
	i3_prof_func();

	return g_pCharaManager->getCharaByNetIdx( m_CurrentUserIdx);
}

