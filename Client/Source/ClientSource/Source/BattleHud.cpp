#include "pch.h"
#include "BattleHud.h"

#include "BattleGui.h"
#include "ConfigRenderBattle.h"
#include "GameCharaManager.h"
#include "GlobalVariables.h"

#include "StageBattle.h"
#include "GameCharaContexts.h"
#include "GameObject.h"
#include "./Camera/GameCamera_OnObject.h"
#include "./Weapon/WeaponM197.h"
#include "./Weapon/WeaponInfo/M197Info.h"
#include "./StageBattle/ScoreContext.h"
#include "Weapon/WeaponItemID.h"

#include "ChainKills.h"
#include "BattleHUD_Scope.h"
#include "BattleHUD_MiniMap.h"
#include "BattleHUD_TrainingScore.h"
#include "BattleHUD_WarnningHack.h"
#include "BattleHUD_Helicopter.h"
#include "BattleHUD_ObserverHPGauge.h"
#include "BattleHUD_TargetHP.h"
#include "BattleHUD_Revenge.h"
#include "BattleHUD_ObserverHPGauge.h"
#include "BattleHUD_TextEtc.h"
#include "BattleHUD_CountDown.h"
#include "BattleHUD_Help.h"

#include "UI/UIUtil.h"
#include "UI/UIBattlePopupBase.h"
#include "UI/UIBattleFrame.h"				// 강퇴 제안 팝업시 닉네임마크를 삭제하기 위해 포함시킴.(11.06.09.수빈)
#include "UI/UIHUDNoticeShop.h"
#include "UI/UIHUDBase.h"
#include "UI/UIHUDIngameChat.h"
#include "UI/UIHUDRecordBoard.h"
#include "UI/UIHudDashBoard.h"
#include "UI/UIHudUtil.h"
#include "UI/UIHudC4Indicator.h"
#include "UI/UIMath.h"
#include "UI/UIHudMissionBoard.h"

#include "ui/UIHudManager.h"

#include "i3Base/string/ext/ismb.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/generic_string_cat.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "GameContextUtil.h"
#include "GameMissionManager.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

I3_CLASS_INSTANCE(CHud);

#include "../Weapon/SentryGun/SentrygunStuff.h"

#include "BattleServerContext.h"
#include "UserInfoContext.h"
#include "EnvInfoContext.h"
#include "ChannelContext.h"
#include "AssistContext.h"
#include "HackContext.h"
#include "MainWeapon_Grenade.h"
#include "MainWeapon_C5.h"

#include "CQAWeaponCMgr.h"
#include "GameCharaBase.h"

#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"

#include "ClientStageInfoUtil.h"

#define NOTICE_TEXT_HEIGHT (30*i3GuiRoot::getHeightRatioBy768())
#define NOTICE_LINE_HEIGHT (30*i3GuiRoot::getHeightRatioBy768())

#define GAP_BETWEEN_SCOREBOARD_BOMBIMAGE	40.0f

CHud::CHud(void)
{
	i3_prof_func();
	UINT32 i;

	m_pBattleFrame = nullptr;
	m_CurrentUserIdx = -1;

	m_pSceneNode = i3Node::new_object();

	m_pSpriteHit					= nullptr;
	m_pSpriteDeath					= nullptr;
	m_pSpriteKillTextBG				= nullptr;

	for(i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		m_pSpriteKillMark[i] = nullptr;
	}

	i3::cu::for_each_delete_clear(m_InGameMessageList);

	m_pSpriteHeadShot				= nullptr;
	m_pSpriteObjectDefence			= nullptr;
	m_pSpriteObjDefenctAndHead		= nullptr;
	m_pSpriteAssistLine				= nullptr;
//Key:EscapeMission
	m_pSpriteLostWorldEP5			=nullptr;
	m_pSpriteGrenadeLauncherCrossHair = nullptr;
	m_pSpriteMission				= nullptr;
	m_pSpriteBoss					= nullptr;
	m_pSpriteObjectReply			= nullptr;
	m_pGrenadeInd					= nullptr;
	m_pSpriteUsurpation				= nullptr;
	m_pSpriteUsurpationTextBG		= nullptr;

	m_pHUDRoot						= nullptr;
	m_pScrEffectAttr				= nullptr;
	m_pScrEffect					= nullptr;
	m_pScrTex2D						= nullptr;
	m_pScrTexture					= nullptr;

	m_pTexture						= nullptr;
	m_pTextureHUD2					= nullptr;
	m_pTextureHUD3					= nullptr;
	m_bUpdateNotice					= false;

	m_pTextureRankMark				= nullptr;

	m_gmObserverCallSignNumSpr		= nullptr;

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		// 신규 InGameMessageList에서 사용하는 텍스트
		m_pLeftText[i] = nullptr;
		m_pRightText[i] = nullptr;
		m_pNoticeText[i] = nullptr;
	}

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		m_pTextName[i] = nullptr;
	}

	m_pTextEtc = nullptr;
	m_pCHUD_CountDown = nullptr;

	for(i=0; i<HUD_BOMB_TEXT_COUNT; i++)
	{
		m_pTextBomb[i] = nullptr;
	}
	for(i=0; i<HUD_MISSION_TEXT_COUNT; i++)
	{
		m_pTextMission[i] = nullptr;
	}

	for(i=0; i<HUD_USURPATION_TEXT_COUNT; i++)
	{
		m_pTextUsurpation[i] = nullptr;
	}

	for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
	{
		m_pTextRadioChat[ i ] = nullptr;
	}

	for( i = 0; i < MAX_INDICATOR; i++ )
	{
		m_pTextGrenade[ i ] = nullptr;
	}

	m_pTextDummy		= nullptr;

	for( i = 0; i < HUD_OBJECT_REPLY_COUNT; i++ )
	{
		m_pTextObjectReply[ i ] = nullptr;
		m_fDeltaTimeObjectReply[ i ] = 0.0f;
	}

	m_pTextCheatkeyWarp = nullptr;
	m_pTextTabminimapProps = nullptr;

	m_pTextProfile = nullptr;
	m_bEnableProfile = false;

	m_fBulletScale		= 0.0f;
	
	m_fViewHalfWidth	= 0.0f;	//화면의 중앙 좌표, 화면크기의 반
	m_fViewHalfHeight	= 0.0f;	//

	m_fBulletBarPosX	= 0.0f;
	m_fBulletBarPosY	= 0.0f;

	m_bUpdateNotice		= false;
	m_bOnlyNoticeUpdate = false;
	m_bUpdateHeadShot = false;

	m_fHeadShotTime		= 0.0f;

	m_nInGameMessageCount = 0;
	m_idxDamageDirectionOrder = 0;

	for(i=0; i<HUD_SPRITE_HIT_ANGLE_MAX; i++)
	{
		m_fDamageDirectionAngle[i] = 0.0f;
	}

	m_pSpriteObserver				= nullptr;

	for( i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++ )
	{
		m_pTextObserver[i] = nullptr;
	}
	m_bEnableObserverMode			= false;
	
	m_bBombMissionEnable			= false;
	m_bOneTimeAreaUpdate			= false;
	m_bDettachBomb					= false;

	m_bUsurpationEnable				= false;
	m_bUsurpationIndicator			= false;
	m_bUserpationMission			= false;
	m_bStartUsurpationMode			= false;
	m_bFirstUsurpationUI			= false;
	m_bFirstUsurpationHUD			= false;
	m_bOpenUsurpationHUD			= false;
	m_bGetSoundPlay					= false;
	m_bFirstGetSoundPlay			= false;
	m_rUsurpationStartTime			= 0.0f;

	m_bFirstHUD						= true;

	m_fMissionMessageTime			= 0.0f;
	m_fMissionNoticeTimer			= 0.0f;
	m_fMissionNoticeLifeTime		= 3.f;

	m_bEnableDeathSprite			= false;
	m_fDeathSpriteTime				= 0.0f;

	m_bEnableRadioChat				= false;
	m_bEnableRadio1					= false;
	m_bEnableRadio2					= false;
	m_bEnableRadio3					= false;
	m_rRadioReusingTimer			= 0.f;

	m_bGameEnd						= false;

	m_pChainKills					= nullptr;

	m_pMiniMap	= nullptr;

	m_HudMode = HUD_MODE_NONE;

	m_fEtcMessageLocalTime = 0.0f;

	m_fOriginalMouseSensivity = 0.0f;

	// 피격필터
	m_pDamageFilterTexture = nullptr;
	m_pDamageFilterSprite = nullptr;

	// 다이필터
	m_pDieFilterTexture = nullptr;
	m_pDieFilterSprite = nullptr;

	m_bShowChatNotice		= false;
	m_nCurrentChatNotice	= 0;

	m_pTextureGrenadeLauncherCrossHair = nullptr;

	m_pRideHelicopter = nullptr;
//	m_pPanelDinoHP	= nullptr;

	m_pObserverHPGaugeHUD = nullptr;

	m_pPanelTargetHP = nullptr;

	m_pWarnningHack = nullptr;

	m_pScope = nullptr;

	m_bEnabledGrenadeLauncherCrossHair = false;
	m_bEnableObserverFlyMode = false;

	m_pTextTarget = nullptr;
	m_pTextureHitMark = nullptr;
	m_pTexturePause = nullptr;
	m_pSpritePause = nullptr;
	
	m_UItypelist.push_back( HUD::PLAYER_STATE );
	m_UItypelist.push_back( HUD::PLAYER_DINO_STATE );
	m_UItypelist.push_back( HUD::AMMO_STATE );
	m_UItypelist.push_back( HUD::DINO_STATE );

	m_pTrainingScore = 0;


	ChatMoveFlag = false;
	ChatPosSaveFlag = false;
	memset(ChatPosY,0,sizeof(ChatPosY));

#if legacy_not_use_texture
	m_pFrindlyWepMarkTexture		= nullptr;
	m_pDominationWepMarkTexture		= nullptr;
#endif
}

CHud::~CHud(void)
{
	i3_prof_func();

	if( m_pPanelTargetHP != nullptr)
	{
		RemoveChild( m_pPanelTargetHP);
		I3_MUST_RELEASE(m_pPanelTargetHP);
		m_pPanelTargetHP = nullptr;
	}

	// 피결필터
	I3_SAFE_NODE_RELEASE(m_pDamageFilterSprite);
	I3_SAFE_RELEASE(m_pDamageFilterTexture);

	// 다이필터
	I3_SAFE_NODE_RELEASE(m_pDieFilterSprite);
	I3_SAFE_RELEASE(m_pDieFilterTexture);

	I3_SAFE_NODE_RELEASE(m_gmObserverCallSignNumSpr);

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
	I3_SAFE_NODE_RELEASE(m_pSpriteLostWorldEP5);
	I3_SAFE_NODE_RELEASE(m_pSpriteKillTextBG);
	for(i = 0; i < HUD_NOTICE_COUNT; i++)
	{
        I3_SAFE_NODE_RELEASE(m_pSpriteKillMark[i]);
	}

	I3_SAFE_NODE_RELEASE(m_pSpriteHeadShot);
	I3_SAFE_NODE_RELEASE( m_pSpriteObjectDefence);
	I3_SAFE_NODE_RELEASE( m_pSpriteObjDefenctAndHead);
	I3_SAFE_NODE_RELEASE( m_pSpriteAssistLine);

	I3_SAFE_NODE_RELEASE( m_pSpriteGrenadeLauncherCrossHair);
	I3_SAFE_RELEASE(m_pTexture);
	I3_SAFE_RELEASE(m_pTextureHUD2);
	I3_SAFE_RELEASE( m_pTextureHUD3);
	I3_SAFE_RELEASE( m_pTextureRankMark);
	I3_SAFE_NODE_RELEASE(m_pSpriteMission);
	I3_SAFE_NODE_RELEASE(m_pSpriteBoss);
	I3_SAFE_NODE_RELEASE(m_pSpriteObjectReply);
	I3_SAFE_NODE_RELEASE(m_pSpriteUsurpation);
	I3_SAFE_NODE_RELEASE(m_pSpriteUsurpationTextBG);

	I3_SAFE_NODE_RELEASE(m_pScrEffect);
	I3_SAFE_NODE_RELEASE(m_pScrTex2D);

	I3_SAFE_RELEASE(m_pScrTexture);
	I3_SAFE_NODE_RELEASE(m_pScrEffectAttr);

	for(i=0; i<HUD_NOTICE_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pLeftText[i]);
		I3_SAFE_NODE_RELEASE(m_pRightText[i]);
		I3_SAFE_NODE_RELEASE(m_pNoticeText[i]);
	}	

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextName[i]);
	}

	for(i=0; i<HUD_BOMB_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextBomb[i]);
	}
	
	for(i=0; i<HUD_TEXT_OBSERVER_FLY_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextObserverFly[i]);
	}
		
	for(i=0; i<HUD_USURPATION_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextUsurpation[i]);
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
	I3_SAFE_RELEASE( m_pTextCheatkeyWarp);
	I3_SAFE_RELEASE( m_pTextTabminimapProps);
	I3_SAFE_RELEASE( m_pTextProfile);

	for( i = 0; i < HUD_OBJECT_REPLY_COUNT; i++ )
	{
		I3_SAFE_NODE_RELEASE( m_pTextObjectReply[ i ] );
	}
	I3_SAFE_RELEASE( m_pMiniMap);

	// 마우스 감도가 변경되었다면 서버에 저장합니다.
	if( m_fOriginalMouseSensivity != g_pEnvSet->m_rMouseSensivity)
	{
		EnvInfoContext::i()->SetNetEnvSetBasicChange( 1);
		g_pEnvSet->Net_Save(); 
		GameEventSender::i()->SetEventOnce( EVENT_OPTION_SAVE_N);
	}

	I3_SAFE_RELEASE( m_pTextureGrenadeLauncherCrossHair);

	// 
	//g_pFramework->getChatBox()->setWhisperEnable( false);

	I3_SAFE_RELEASE( m_pRideHelicopter);
	I3_SAFE_RELEASE( m_pObserverHPGaugeHUD);
	I3_SAFE_RELEASE( m_pScope);
	I3_SAFE_RELEASE( m_pTrainingScore);
	
	I3_SAFE_RELEASE( m_pTextEtc);
	I3_SAFE_RELEASE( m_pCHUD_CountDown);
	I3_SAFE_RELEASE( m_pRevengeHUD);
	I3_SAFE_RELEASE( m_pWarnningHack);
	I3_SAFE_RELEASE( m_pTexturePause);
	I3_SAFE_RELEASE( m_pSpritePause);

	I3_SAFE_DELETE( m_pHelpHUD);

#if legacy_not_use_texture
	I3_SAFE_RELEASE( m_pFrindlyWepMarkTexture);
	I3_SAFE_RELEASE(m_pDominationWepMarkTexture);
#endif

	i3::cu::for_each_safe_release_clear( m_vSpecialDamageFilterTexture, &i3Texture::Release);
	i3::cu::for_each_safe_release_clear( m_vSpecialDamageFilterSprite, &i3Sprite2D::Release);
	m_vSpecialDamageFilterTime.clear();

	i3::cu::for_each_safe_release_clear(m_vSpecialDieFilterTexture, &i3Texture::Release);
	i3::cu::for_each_safe_release_clear(m_vSpecialDieFilterSprite, &i3Sprite2D::Release);
	m_vSpecialDieFilterTime.clear();

	i3::cu::for_each_delete_clear(m_InGameMessageList);

	I3_SAFE_RELEASE(m_pBattleFrame);
}

void CHud::SetCurrentScreenImage( INT32 idx, UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{
	i3_prof_func();

	i3Viewer * pViewer = g_pFramework->GetViewer();
	REAL32 fScrWidth = (REAL32) pViewer->GetViewWidth();
	REAL32 fScrHeight = (REAL32) pViewer->GetViewHeight();

	//	백버퍼에서 전체 이미지를 가져온다.
	g_pViewer->GetRenderContext()->GetBackBufferTexture( m_pScrTexture);

	m_pScrTex2D->SetEnable( idx, true);
	m_pScrTex2D->SetTexture( m_pScrTexture); 
	m_pScrTex2D->SetRect( idx, 0.f, 0.f, fScrWidth, fScrHeight);
	m_pScrTex2D->SetTextureCoord( idx, 0.f, 0.f, (REAL32) SCR_BACKBUFFER_TEX_SIZE, (REAL32) SCR_BACKBUFFER_TEX_SIZE);	
	m_pScrTex2D->SetColor( idx, r, g, b, a);
}

bool CHud::Create( INT32 InitialUserIdx)
{
	i3_prof_func();
	
	m_pBattleFrame = UIBattleFrame::i();
	m_pBattleFrame->AddRef();

	if( ! i3GameObjBase::Create( g_pFramework->getHudLayer() ) )
	{
		I3PRINTLOG(I3LOG_FATAL,  "Create HUD fail!!!!!!!");
		return false;
	}

	m_CurrentUserIdx = InitialUserIdx;

	m_pHUDRoot = i3AttrSet::new_object();
	GetNode()->AddChild( m_pHUDRoot);

	m_pTexture	= crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
	m_pTextureHUD2 = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);
	m_pTextureRankMark = crb::i()->LoadHudTexture( HUD_TEXTURE_LEVEL_ICON);
	m_pTexturePause = crb::i()->LoadHudTexture( HUD_TEXTURE_PAUSE);

#if legacy_not_use_texture
	m_pFrindlyWepMarkTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_CHARA_MARK);
	m_pDominationWepMarkTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_CHARA_MARK2);
#endif

	{	// HUD Texture는 Linear filter 사용
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		m_pHUDRoot->AddAttr( pAttr);
	}

	///////////////////////////////////////////////////////////////
	// 반드시 아래와 같은 순서로 Create해야 합니다.
	// 함부로 생성 순서를 바꾸지 마세요.
	///////////////////////////////////////////////////////////////

	CreateGrenadeLauncherCrossHair();

	// 1.Scope Hud
	m_pScope = CHUD_Scope::new_object();
	g_pWeaponManager->CreateAllScopeHudTexture();
	m_pScope->CreateScopeHUD( m_pHUDRoot);

	// 2.Screen Effect
	CreateScreenEffectHUD();

	// 3.탑승 Helicopter hud생성
	CreateRideHelicopter();

	// 4.Hud
	CreateHUD();

	// 핵 프로그램 사용 경고창은 최상위에 떠야 합니다.
	CreateWarnningHack();	

	m_pHelpHUD = new CHUD_Help(m_pHUDRoot);

	m_pRevengeHUD = CHUD_Revenge::new_object();
	m_pRevengeHUD->CreateHUD(m_pHUDRoot);

	m_pTextEtc = CHUD_TextEtc::new_object();
	m_pTextEtc->CreateHUD(m_pHUDRoot, m_pTexture);

	m_pCHUD_CountDown = CHUD_CountDown::new_object();
	m_pCHUD_CountDown->CreateHUD(m_pHUDRoot);


	// 훈련전 개인 랭킹 관련
	if( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
	{
		m_pTrainingScore = CHud_TrainingScore::new_object();
		m_pTrainingScore->CreateHUD(m_pHUDRoot, m_pTexture, m_pTextureRankMark );
	}

	// 초기화
	InitHud();

	for (INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		InGameMessage* message = new InGameMessage();
		m_InGameMessageList.push_back(message);
	}

	// 초기 마우스 감도를 저장합니다.
	m_fOriginalMouseSensivity = g_pEnvSet->m_rMouseSensivity;

	// 
	g_pFramework->getChatBox()->setWhisperEnable( true);
	
	//특수 피격 필터를 한번 만들어줘야합니다. HUD가 생기기전에 무기를 로딩 하기 때문에
	g_pWeaponManager->CreateAllSpecialDamageFilter();
	g_pWeaponManager->CreateAllSpecialDieFilter();

	ChatMoveFlag = false;
	ChatPosSaveFlag = false;
	memset(ChatPosY,0,sizeof(ChatPosY));

	return true;
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

	if( m_pMiniMap != nullptr)
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
				if (m_pBattleFrame->GetHUD(UIHUD_TABMINIMAP)->isVisible() == false)
					m_pMiniMap->SetEnableMiniMap( true );
			}
		}
		else
		{
			// Tutorial Mode는 TabMinimap->Visible()과 관계 없이 활성화 적용.
			UIHUDBase* pHud = m_pBattleFrame->GetHUD(UIHUD_TABMINIMAP);
			if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{
				m_pMiniMap->SetEnableMiniMap( bEnable);
			}
			else if (pHud && pHud->isVisible() == false)
			{
				m_pMiniMap->SetEnableMiniMap( bEnable);
				HUD::instance()->GetHud(HUD::MINIMAP)->SetVisible(bEnable);
			}
		}
	}
}

void CHud::SetMoveChatting()
{
	i3_prof_func();

	
	UIHUDIngameChat * pChatScene = (UIHUDIngameChat *)m_pBattleFrame->GetHUD(UIHUD_INGAMECHATING);

	if(pChatScene)
	{
		i3UIScene* pScene = pChatScene->GetScene(0);
		i3UIFrameWnd* pChatBox[3] = {0,};
		if(pScene)
		{
			pChatBox[0] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIFrameWnd");
			pChatBox[1] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIImageBox0");
			pChatBox[2] = (i3UIFrameWnd*)pScene->FindChildByName("i3UIEditBox");
		}

		if(pChatBox[0] && pChatBox[1] && pChatBox[2])
		{
			if(!ChatPosSaveFlag)
			{
				for(int i = 0;i < 3; ++i)
				{
					ChatPosY[i] = pChatBox[i]->getPos()->y;
				}
				ChatPosSaveFlag = true;
			}

			if(m_HudMode == HUD_MODE_MISSION_OBSERVER)
			{
				if(!ChatMoveFlag)
				{
					for(int i = 0;i < 3; ++i)
					{
						VEC3D* pPos = pChatBox[i]->getPos();
						pPos->y = ChatPosY[i] - 108;
					}

					ChatMoveFlag = true;
				}
			}
			else
			{
				if(ChatMoveFlag)
				{
					for(int i = 0;i < 3; ++i)
					{
						VEC3D* pPos = pChatBox[i]->getPos();
						pPos->y = ChatPosY[i];
					}
					ChatMoveFlag = false;
				}
			}
		}
	}
}

void	CHud::IncreaseMiniMapScale( bool bIncrease, REAL32 rAmount)	
{	
	m_pMiniMap->IncreaseMiniMapScale( bIncrease, rAmount);
}
void	CHud::SetMiniMapFloorIndex( INT32 idx)	
{	
	m_pMiniMap->SetCurrentFloorIdx( idx);
}
void	CHud::SetMiniMapLocationText( const i3::wliteral_range& wLocationRng)	
{	
	m_pMiniMap->SetCurrentLocationText( wLocationRng);
}

const i3::wstring& CHud::getMiniMapLocationText(void) const
{ 
	return m_pMiniMap->getCurrentLocationText();		
}
void	CHud::SetDestroyObjectUnderAttack( INT32 idx, bool bFlag)
{ 
	m_pMiniMap->SetDestroyObjectUnderAttack( idx, bFlag); 
}

void CHud::CreateScreenEffectHUD()
{
	i3_prof_func();

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pScrEffectAttr = i3AttrSet::new_object();

#if defined( I3_DEBUG)
	m_pScrEffectAttr->SetName( "EffectHUDAttrSet");
#endif	

	i3LightingEnableAttr *pLightEnable = i3LightingEnableAttr::new_object_ref();
	pLightEnable->Set( false );
	m_pScrEffectAttr->AddAttr( pLightEnable );

	i3BlendEnableAttr *pBlendEnable = i3BlendEnableAttr::new_object_ref();
	pBlendEnable->Set( true );
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
#if defined( I3_DEBUG)
	m_pScrTexture->SetName( "ScreenEffect");
#endif
	m_pScrTexture->Create( fImageWidth, fImageHeight, 1, I3G_IMAGE_FORMAT_BGRA_8888, I3G_USAGE_RENDERTARGET);

	m_pScrTex2D = i3Sprite2D::new_object();
	m_pScrEffectAttr->AddChild( m_pScrTex2D);
	
	m_pScrTex2D->Create( SCR_BACKBUFFER_COUNT, true, true);
	for( INT32 i = 0; i < SCR_BACKBUFFER_COUNT; i++)
	{
		m_pScrTex2D->SetRect( i, 0.0f, 0.0f, (REAL32) nScreenWidth, (REAL32) nScreenHeight);
		m_pScrTex2D->SetColor( i, 255, 255, 255, 255);
	}		
	m_pScrTex2D->SetEnable( false);

	//	섬광탄등 화면 단순한 이펙트에 사용합니다.
	m_pScrEffect = i3Sprite2D::new_object();
	m_pScrEffectAttr->AddChild( m_pScrEffect);

	m_pScrEffect->Create( 1, false, true);		
	m_pScrEffect->SetRect( 0, 0.0f, 0.0f, (REAL32) nScreenWidth, (REAL32) nScreenHeight);
	m_pScrEffect->SetColor( 0, 255, 255, 255, 255);		
	m_pScrEffect->SetEnable( false);
	
	m_pHUDRoot->AddChild( m_pScrEffectAttr);
}

void CHud::ChangeTextNameSize(bool bPlus)
{
	i3_prof_func();

	//유저이름 최소, 최대크기(PBBUG-15353)
	const INT32 minTextSize = 6;
	const INT32 maxTextSize = 18;

	if(bPlus)
	{
		m_iTextNameSize++;

		// 최대값을 막습니다.
		if (maxTextSize < m_iTextNameSize)
		{
			m_iTextNameSize = maxTextSize;
			return;
		}
	}
	else
	{
		m_iTextNameSize--;

		// 최소값을 막습니다.
		if (m_iTextNameSize < minTextSize)
		{
			m_iTextNameSize = minTextSize;
			return;
		}
	}

	for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextName[i]);
	}

	for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		m_pTextName[i] = i3TextNodeDX2::new_object();
		m_pTextName[i]->Create( GetDefaultFontName(), FONT_COUNT_64, m_iTextNameSize, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextName[i]->SetText(L"");	
		m_pTextName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextName[i]->setAutoWrapEnable( false);
		m_pTextName[i]->SetEnable(false);
		// revision 63559 [옵저버] 유저 닉네임 사이즈 변경시 닉네임이 UI 가리는 현상 수정. hansoft.5654
		m_pHUDRoot->InsertChild( 0, m_pTextName[i]);	
	}
}

void CHud::CreateHUD()
{ 
	i3_prof_func();

 //	INT32 i = 0;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;
	INT32	nScreenHeight	= 0;

	if( pViewer != nullptr )
	{
		nScreenWidth	= pViewer->GetViewWidth();
		nScreenHeight	= pViewer->GetViewHeight();
	}

	// 피격필터
	{		
		m_pDamageFilterTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_DAMAGE_FILTER);
		
		m_pDamageFilterSprite = i3Sprite2D::new_object();
		m_pDamageFilterSprite->Create(2, true, true);
		m_pDamageFilterSprite->SetTexture(m_pDamageFilterTexture);
		m_pDamageFilterSprite->SetEnable(false);

		m_pDamageFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDamageFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
		m_pDamageFilterSprite->SetColor(0, 255, 255, 255, 255);

		m_pDamageFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDamageFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
		m_pDamageFilterSprite->SetColor(1, 255, 255, 255, 255);
		
		m_pHUDRoot->AddChild(m_pDamageFilterSprite);
	}

	// 피격필터
	{
		m_pDieFilterTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_DAMAGE_FILTER);

		m_pDieFilterSprite = i3Sprite2D::new_object();
		m_pDieFilterSprite->Create(2, true, true);
		m_pDieFilterSprite->SetTexture(m_pDieFilterTexture);
		m_pDieFilterSprite->SetEnable(false);

		m_pDieFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDieFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
		m_pDieFilterSprite->SetColor(0, 255, 255, 255, 255);

		m_pDieFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		m_pDieFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
		m_pDieFilterSprite->SetColor(1, 255, 255, 255, 255);

		m_pHUDRoot->AddChild(m_pDieFilterSprite);
	}

	{
		m_fViewHalfWidth	= (REAL32)( nScreenWidth >> 1);	//화면의 중앙 좌표, 화면크기의 반
		m_fViewHalfHeight	= (REAL32)( nScreenHeight >> 1);//
	}

	// 죽었을 경우 나오는 스프라이트
	{
		REAL32 fHeight = (REAL32)nScreenHeight*0.5f;

		m_pSpriteDeath = i3Sprite2D::new_object();
		m_pSpriteDeath->SetTexture( m_pTexture );
		m_pSpriteDeath->Create(2, true, true);

		m_pSpriteDeath->SetEnable(0, false);
		m_pSpriteDeath->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth, fHeight);
		m_pSpriteDeath->SetColor( 0, 255,255,255,0);
		m_pSpriteDeath->SetTextureCoord(0, 1012.0f, 2.0f, 1021.0f, 382.0f);

		m_pSpriteDeath->SetEnable(1, false);
		m_pSpriteDeath->SetRect(1, 0.0f, fHeight, (REAL32)nScreenWidth, fHeight);
		m_pSpriteDeath->SetColor( 1, 255,255,255,0);
		m_pSpriteDeath->SetFlip( 1, false, true);
		m_pSpriteDeath->SetTextureCoord(1, 1012.0f, 2.0f, 1021.0f, 382.0f);

		m_pHUDRoot->AddChild(m_pSpriteDeath);
	}

	// 스코어 비교
	{
		m_pSpriteBoss = i3Sprite2D::new_object();	
		m_pSpriteBoss->SetTexture( m_pTextureHUD2 );
		m_pSpriteBoss->Create( 1, true, true);
		m_pSpriteBoss->SetEnable(0, false);
		m_pSpriteBoss->SetRect(0, m_fViewHalfWidth - 192.0f, (REAL32)nScreenHeight - 255.0f, 383.0f, 84.0f);
		m_pSpriteBoss->SetColor( 0, 255,255,255,255);
		m_pSpriteBoss->SetTextureCoord(0, 324.0f, 409.0f, 707.0f, 493.0f);
		m_pHUDRoot->AddChild(m_pSpriteBoss);
	}

#if defined( DEF_OBSERVER_MODE)
	// Observer에서는 생성할 필요가 없다.
	if( !BattleSlotContext::i()->isObserverMe())
#endif

	// 미션 - Breakdown mission
	if( CStageInfoUtil::IsUsingMissionObject( *MyRoomInfoContext::i()->getStage()) )
	{
		m_pPanelTargetHP = CHUD_TargetHP::new_object();
		m_pPanelTargetHP->Create( this);
	}

	{
		m_pSpriteObjectReply = i3Sprite2D::new_object();
		m_pSpriteObjectReply->SetTexture( m_pTexture );
		m_pSpriteObjectReply->Create(HUD_OBJECT_REPLY_COUNT, false, true);
		
		for(INT32 i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
		{
			m_pSpriteObjectReply->SetEnable( i, false);
			m_pSpriteObjectReply->SetRect( i, 0.0f, 0.0f, 0.0f, 0.0f);
			m_pSpriteObjectReply->SetColor( i, 0,0,0,255);
		}

		m_pHUDRoot->AddChild(m_pSpriteObjectReply);
	}

	// HUD에서 사용하는 Object들의 이름 및 대화에사용되어집니다.
	{
		for(INT32 i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
		{
			m_pTextObjectReply[i] = i3TextNodeDX2::new_object();
			m_pTextObjectReply[i]->Create(GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextObjectReply[i]->SetText(L"");	
			m_pTextObjectReply[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			m_pTextObjectReply[i]->setAutoWrapEnable( false);
			m_pTextObjectReply[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pTextObjectReply[i]);		
		}
	}

	// HUD에서 사용하는 캐릭터들의 이름을 설정합니다.
	{
		if(UserInfoContext::i()->IsGM_Observer())
		{
			m_iTextNameSize = GAME_FONT_TARGET_HP_TEXT_SIZE + 4;
		}
		else
		{
			m_iTextNameSize = GAME_FONT_TARGET_HP_TEXT_SIZE;
		}
		
		for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
		{
			m_pTextName[i] = i3TextNodeDX2::new_object();
			m_pTextName[i]->Create( GetDefaultFontName(), FONT_COUNT_64, m_iTextNameSize, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextName[i]->SetText(L"");	
			m_pTextName[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pTextName[i]->setAutoWrapEnable( false);
			m_pTextName[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pTextName[i]);		
		}
	}

	// Radio chat
	{
		const INT32 offset = 15;
		REAL32 height_ratio = i3GuiRoot::getHeightRatioBy768();
		const INT32 start_y = static_cast<INT32>(nScreenHeight * 0.475f);

		for( INT32 i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
		{
			m_pTextRadioChat[ i ] = i3TextNodeDX2::new_object();
			m_pTextRadioChat[ i ]->Create( GetDefaultFontName(), 64, GAME_FONT_BIG_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true);
			m_pTextRadioChat[ i ]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP );
			m_pTextRadioChat[ i ]->SetColor( 80, 255, 60, 255 );

			if( i == 0 )
				m_pTextRadioChat[ i ]->setPos( 20, static_cast<UINT32>( start_y * height_ratio) );
			else
				m_pTextRadioChat[ i ]->setPos( 20, static_cast<UINT32>( (start_y + 10 + (i * offset)) * height_ratio ) );
			m_pHUDRoot->AddChild( m_pTextRadioChat[ i ] );
		}
	}

	// Kill Text의 배경으로 사용되는 Sprite
	{
		m_pSpriteKillTextBG = i3Sprite2D::new_object();
		m_pSpriteKillTextBG->SetTexture( m_pTexture );
		m_pSpriteKillTextBG->Create(HUD_NOTICE_COUNT, true, true);
		for(INT32 i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteKillTextBG->SetEnable( i, false);
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
			m_pSpriteKillMark[j]->Create(1, true, false);
			m_pSpriteKillMark[j]->SetRect(0, 0.0f, 0.0f, 81.0f, 24.0f);
			m_pSpriteKillMark[j]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pSpriteKillMark[j]);
		}
	}

	// Defence Object 마크
	{
		m_pTextureHUD3 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
		I3ASSERT( m_pTextureHUD3 != nullptr);

		m_pSpriteObjectDefence = i3Sprite2D::new_object();
		m_pSpriteObjectDefence->SetTexture( m_pTextureHUD3);
		m_pSpriteObjectDefence->Create( HUD_NOTICE_COUNT, true, true);

		m_pSpriteObjDefenctAndHead = i3Sprite2D::new_object();
		m_pSpriteObjDefenctAndHead->SetTexture( m_pTextureHUD3);
		m_pSpriteObjDefenctAndHead->Create( HUD_NOTICE_COUNT, true, true);

		for( INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteObjectDefence->SetEnable( i, false);
			m_pSpriteObjectDefence->SetRect( i, 0.f, 0.f, 28.f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteObjectDefence->SetColor( i, 255, 255, 255, 255);
			m_pSpriteObjectDefence->SetTextureCoord( i, 542.0f, 112.0f, 570.0f, 142.0f);

			m_pSpriteObjDefenctAndHead->SetEnable( i, false);
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
			m_pSpriteLostWorldEP5->Create( HUD_NOTICE_COUNT, true, true);

		
			for(INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
			{
				m_pSpriteLostWorldEP5->SetEnable( i, false);
				m_pSpriteLostWorldEP5->SetRect( i, 0.f, 0.f, 28.f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
				m_pSpriteLostWorldEP5->SetColor( i, 255, 255, 255, 255);
				m_pSpriteLostWorldEP5->SetTextureCoord( i, 542.0f, 112.0f, 570.0f, 142.0f);

			}

			m_pHUDRoot->AddChild( m_pSpriteLostWorldEP5);
		}



	}
	
	// 탈취모드 HUD
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;

		REAL32 fClanOffsetY = 5.0f;

		if( ChannelContext::i()->GetBattleChannel())
		{
			fClanOffsetY = 18.0f;
		}

		if( pViewer != nullptr)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pSpriteUsurpation = i3Sprite2D::new_object();
		m_pSpriteUsurpation->SetTexture( m_pTextureHUD3);
		m_pSpriteUsurpation->Create(HUD_SPRITE_USURPATION_COUNT, true, true);

		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
			MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
		{
			m_pSpriteUsurpation->SetEnable(HUD_SPRITE_USURPATION_WATERMELON, false);
			m_pSpriteUsurpation->SetRect(HUD_SPRITE_USURPATION_WATERMELON,(REAL32)(nPosX-30), (75.0f)+fClanOffsetY, (59.0f), (80.0f));
			m_pSpriteUsurpation->SetColor(HUD_SPRITE_USURPATION_WATERMELON, 255,255,255,255);
			m_pSpriteUsurpation->SetTextureCoord(HUD_SPRITE_USURPATION_WATERMELON, 697.0f, 255.0f, 756.0f, 335.0f);

			m_pSpriteUsurpation->SetEnable(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, false);
			m_pSpriteUsurpation->SetRect(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR,(REAL32)(nPosX-30), (75.0f)+fClanOffsetY, (55.0f), (55.0f));
			m_pSpriteUsurpation->SetColor(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, 255,255,255,255);
			m_pSpriteUsurpation->SetTextureCoord(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, 704.0f, 192.0f, 759.0f, 247.0f);
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL) )
		{
			m_pSpriteUsurpation->SetEnable(HUD_SPRITE_USURPATION_WATERMELON, false);
			m_pSpriteUsurpation->SetRect(HUD_SPRITE_USURPATION_WATERMELON,(REAL32)(nPosX-30), (75.0f)+fClanOffsetY, (100.0f), (90.0f));
			m_pSpriteUsurpation->SetColor(HUD_SPRITE_USURPATION_WATERMELON, 255,255,255,255);
			m_pSpriteUsurpation->SetTextureCoord(HUD_SPRITE_USURPATION_WATERMELON, 824.0f, 255.0f, 924.0f, 345.0f);

			m_pSpriteUsurpation->SetEnable(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, false);
			m_pSpriteUsurpation->SetRect(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR,(REAL32)(nPosX-30), (75.0f)+fClanOffsetY, (80.0f), (70.0f));
			m_pSpriteUsurpation->SetColor(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, 255,255,255,255);
			m_pSpriteUsurpation->SetTextureCoord(HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, 936.0f, 255.0f, 1016.0f, 325.0f);
		}
		
		m_pHUDRoot->AddChild(m_pSpriteUsurpation);

		{
			m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE] = i3TextNodeDX2::new_object();
			m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->Create( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_HUD_MISSION_NOTICE, FW_BOLD, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->setPos((UINT32)nPosX-300, nScreenHeight-170);
			m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->setSize(600, 67);

			for(INT32 i=0; i<HUD_USURPATION_TEXT_COUNT; i++)
			{
				m_pTextUsurpation[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_TOP);
				m_pTextUsurpation[i]->SetEnable(false);
				m_pHUDRoot->AddChild(m_pTextUsurpation[i]);		
			}
		}
	}


	// HeadShot에 사용되는 Sprite
	{
		// Head Shot
		m_pSpriteHeadShot = i3Sprite2D::new_object();	
		m_pSpriteHeadShot->SetTexture( m_pTexture );
		m_pSpriteHeadShot->Create(HUD_NOTICE_COUNT, true, true);
		for(INT32 i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteHeadShot->SetEnable( i, false);
			m_pSpriteHeadShot->SetRect( i, 0.0f, 0.0f, 55.0f*i3GuiRoot::getHeightRatioBy768(), 37.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteHeadShot->SetColor( i, 255,255,255,200);
			m_pSpriteHeadShot->SetTextureCoord( i, 183.0f, 204.0f, 238.0f, 242.0f);
		}

		m_pHUDRoot->AddChild(m_pSpriteHeadShot);
	}

	// Assist에 사용되는 Sprite
	{
		m_pSpriteAssistLine = i3Sprite2D::new_object();
		m_pSpriteAssistLine->SetTexture(m_pTexture);
		m_pSpriteAssistLine->Create(HUD_NOTICE_COUNT, true, true);
		for (INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
		{
			m_pSpriteAssistLine->SetEnable(i, false);
			m_pSpriteAssistLine->SetRect(i, 0.0f, 0.0f, 20.0f*i3GuiRoot::getHeightRatioBy768(), 18.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteAssistLine->SetColor(i, 255, 255, 255, 200);
			m_pSpriteAssistLine->SetTextureCoord(i, 188.0f, 178.0f, 208.0f, 196.0f);
		}

		m_pHUDRoot->AddChild(m_pSpriteAssistLine);
	}

	// HUD에서 사용하는 Notice를 설정합니다.
	{
		m_pTextDummy = i3TextNodeDX2::new_object();
		m_pTextDummy->Create(  GetDefaultFontName(), 128, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextDummy->SetText(L"");	
		m_pTextDummy->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
		m_pTextDummy->setSize( 400, 20);
		m_pTextDummy->SetEnable(false);
		
		REAL32 rFontScale = (i3GuiRoot::getHeightRatioBy768() + i3GuiRoot::getHeightRatioBy768()) * 0.5f;
		for(INT32 i=0; i<HUD_NOTICE_COUNT; i++)
		{
			m_pLeftText[i] = i3TextNodeDX2::new_object();
			m_pLeftText[i]->Create(GetDefaultFontName(), 128, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pLeftText[i]->SetText(L"");
			m_pLeftText[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pLeftText[i]->setSize((UINT32)(400 * i3GuiRoot::getHeightRatioBy768()), (UINT32)(20 * i3GuiRoot::getHeightRatioBy768()));
			m_pLeftText[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pLeftText[i]);

			m_pRightText[i] = i3TextNodeDX2::new_object();
			m_pRightText[i]->Create(GetDefaultFontName(), 128, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pRightText[i]->SetText(L"");
			m_pRightText[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pRightText[i]->setSize((UINT32)(400 * i3GuiRoot::getHeightRatioBy768()), (UINT32)(20 * i3GuiRoot::getHeightRatioBy768()));
			m_pRightText[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pRightText[i]);

			m_pNoticeText[i] = i3TextNodeDX2::new_object();
			m_pNoticeText[i]->Create(GetDefaultFontName(), 128, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
			m_pNoticeText[i]->SetText(L"");
			m_pNoticeText[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
			m_pNoticeText[i]->setSize((UINT32)(400 * i3GuiRoot::getHeightRatioBy768()), (UINT32)(20 * i3GuiRoot::getHeightRatioBy768()));
			m_pNoticeText[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pNoticeText[i]);
		}
	}

	// 수류탄 위치 표시
	{
		m_pGrenadeInd = i3Sprite2D::new_object();
		m_pGrenadeInd->SetTexture( m_pTexture);
		m_pGrenadeInd->Create( 2 * MAX_INDICATOR, true, true);

		for( INT32 i = 0; i < MAX_INDICATOR; i++)
		{
			// 첫번째 Sprite : 수류탄 모양
			m_pGrenadeInd->SetEnable(		(i * 2) + 0, false);
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
			m_pGrenadeInd->SetEnable(		(i * 2) + 1, false);
			m_pGrenadeInd->SetSize(			(i * 2) + 1, 32.0f, 32.0f);
			m_pGrenadeInd->SetColor(		(i * 2) + 1, 255, 255, 255, 255);
			m_pGrenadeInd->SetTextureCoord(	(i * 2) + 1, 0.0f, 64.0f, 32.0f, 64.0f + 32.0f);

			m_pTextGrenade[i] = i3TextNodeDX2::new_object();
			m_pTextGrenade[i]->Create(  GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_DEFAULT_SIZE, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
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
		m_pSpriteHit->Create(HUD_SPRITE_HIT_COUNT, true, true);

	

		// Hit Damage
		{
			// Blind Filter로 준비된 이미지가 없습니다
			// 레이더에 표시되는 아군 동그라미(흰색 점)의 일부분을 임시로 사용하고 있습니다
			m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_DAMAGE, false);
			m_pSpriteHit->SetRect( HUD_SPRITE_HIT_DAMAGE, 0.0f, 0.0f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
			m_pSpriteHit->SetColor( HUD_SPRITE_HIT_DAMAGE, 200,0,0,255);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_HIT_DAMAGE, 290.0f, 70.0f, 292.0f, 72.0f);
		}
		
		// Hit Left Front
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_LEFT_FRONT, true);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_LEFT_FRONT, 0.0f, 0.0f, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_LEFT_FRONT, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_LEFT_FRONT, 245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_LEFT_FRONT, true, false);
		}

		// Hit Right Front
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_RIGHT_FRONT, true);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_RIGHT_FRONT,(REAL32)nScreenWidth/2, 0.0f, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_RIGHT_FRONT, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_RIGHT_FRONT,  245.0f, 208.0f, 404.0f, 328.0f);
		}

		// Hit Left Back
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_LEFT_BACK, true);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_LEFT_BACK, 0.0f, (REAL32)nScreenHeight/2, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_LEFT_BACK, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_LEFT_BACK,  245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_LEFT_BACK, true, true);
		}

		// Hit Right Back
		{
			m_pSpriteHit->SetEnable( HUD_SPRITE_WARNING_RIGHT_BACK, true);
			m_pSpriteHit->SetRect( HUD_SPRITE_WARNING_RIGHT_BACK, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2, (REAL32)nScreenWidth/2, (REAL32)nScreenHeight/2);
			m_pSpriteHit->SetColor( HUD_SPRITE_WARNING_RIGHT_BACK, 255,0,0,0);
			m_pSpriteHit->SetTextureCoord( HUD_SPRITE_WARNING_RIGHT_BACK,  245.0f, 208.0f, 404.0f, 328.0f);
			m_pSpriteHit->SetFlip(HUD_SPRITE_WARNING_RIGHT_BACK, false, true);
		}	

		// Hit Damage Angle
		{
			for(INT32 i = 0; i < HUD_SPRITE_HIT_ANGLE_MAX; i++)
			{
				m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_ANGLE_START + i, false);
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
		if( pViewer != nullptr)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pSpriteMission = i3Sprite2D::new_object();	
		m_pSpriteMission->SetTexture( m_pTexture );
		m_pSpriteMission->Create( HUD_SPRITE_MISSION_COUNT, true, true);

		// Mission message BG
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, false);
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_MESSAGE_BG, 255,255,255,255);

		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_MESSAGE_BG, (REAL32)(nPosX-200), (REAL32)(nScreenHeight-170), (401.0f), (67.0f));

		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_MESSAGE_BG, 531.0f, 14.0f, 532.0f, 15.0f);

		// Mission결과시 Team
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, false);		
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_TEAM, 255,255,255,255);
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_TEAM, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 0.0f, 0.0f, 0.0f, 0.0f);

		// Mission결과시 승패여부
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, false);		
		m_pSpriteMission->SetColor( HUD_SPRITE_MISSION_RESULT, 255,255,255,255);
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, 0.0f, 0.0f, 0.0f, 0.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 0.0f, 0.0f, 0.0f, 0.0f);


		m_pHUDRoot->AddChild(m_pSpriteMission);
	}

	// HUD에서 사용하는 폭탄관련 텍스트 박스
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;

		INT32 nClanOffsetY = 5;

		if( ChannelContext::i()->GetBattleChannel())
		{
			nClanOffsetY = 18;
		}

		if( pViewer != nullptr)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pTextBomb[HUD_BOMB_TEXT_TIME] = i3TextNodeDX2::new_object();
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->Create(  GetDefaultFontName(), 16, GAME_FONT_SIZE_HUD_BOMB_TIME, FW_BOLD, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->setPos(nPosX-26, 80+nClanOffsetY);
		m_pTextBomb[HUD_BOMB_TEXT_TIME]->setSize(133, 36);

		for(INT32 i=0; i<HUD_BOMB_TEXT_COUNT; i++)
		{
			m_pTextBomb[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			//m_pTextBomb[i]->setShadowEnable( true, FONT_SHADOW_QUALITY_NORMAL);
			m_pTextBomb[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pTextBomb[i]);		
		}
		
	}

	// Hud에서 사용되는 미션관련 텍스트 박스
	{
		COLOR col;
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		if( pViewer != nullptr)
		{
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pTextMission[HUD_MISSION_TEXT_MESSAGE] = i3TextNodeDX2::new_object();
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->Create(  GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_HUD_MISSION_MESSAGE, FW_BOLD, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);

		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->setPos(nPosX-300, nScreenHeight-170);
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->setSize(600, 67);

		m_pTextMission[HUD_MISSION_TEXT_NOTICE] = i3TextNodeDX2::new_object();
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->Create(  GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_HUD_MISSION_NOTICE, FW_BOLD, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setPos(nPosX-300, 130);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setSize(600, 36);
		i3Color::Set( &col, (UINT8) 240, 210, 110, 255);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetColor( &col);

		for(INT32 i=0; i<HUD_MISSION_TEXT_COUNT; i++)
		{
			m_pTextMission[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);					
			m_pTextMission[i]->SetEnable(false);
			m_pHUDRoot->AddChild(m_pTextMission[i]);		
		}
	}

	// 치트키 텍스트.
	{
		m_pTextCheatkeyWarp = i3TextNodeDX2::new_object();
		REAL32 rFontScale = (i3GuiRoot::getHeightRatioBy768() + i3GuiRoot::getHeightRatioBy768()) * 0.5f;
		m_pTextCheatkeyWarp->Create( GetDefaultFontName(), 2048, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextCheatkeyWarp->SetText(L"");
		m_pTextCheatkeyWarp->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
		m_pTextCheatkeyWarp->setSize( (UINT32)(400*i3GuiRoot::getHeightRatioBy768()), (UINT32)(20*i3GuiRoot::getHeightRatioBy768()));
		m_pTextCheatkeyWarp->SetEnable(false);
		m_pHUDRoot->AddChild(m_pTextCheatkeyWarp);	
	}

	// 탭미니맵 속성값 텍스트.
	{
		m_pTextTabminimapProps = i3TextNodeDX2::new_object();
		REAL32 rFontScale = (i3GuiRoot::getHeightRatioBy768() + i3GuiRoot::getHeightRatioBy768()) * 0.5f;
		m_pTextTabminimapProps->Create( GetDefaultFontName(), 256, (INT32)(GAME_FONT_DEFAULT_SIZE*rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextTabminimapProps->SetText(L"");	
		m_pTextTabminimapProps->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
		m_pTextTabminimapProps->setSize( (UINT32)(400*i3GuiRoot::getHeightRatioBy768()), (UINT32)(20*i3GuiRoot::getHeightRatioBy768()));
		m_pTextTabminimapProps->SetEnable(false);
		m_pHUDRoot->AddChild(m_pTextTabminimapProps);	
	}

	// 프로파일링.
	{
		m_pTextProfile = i3TextNodeDX2::new_object();
		REAL32 rFontScale = (i3GuiRoot::getHeightRatioBy768() + i3GuiRoot::getHeightRatioBy768()) * 0.5f;
		m_pTextProfile->Create( GetDefaultFontName(), 256, (INT32)(12 * rFontScale), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextProfile->SetText(L"");	
		m_pTextProfile->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
		m_pTextProfile->setSize( (UINT32)(400*i3GuiRoot::getHeightRatioBy768()), (UINT32)(20*i3GuiRoot::getHeightRatioBy768()));
		m_pTextProfile->SetEnable(false);
		m_pHUDRoot->AddChild(m_pTextProfile);
	}

	// ObserverMode
	{
		m_pSpriteObserver = i3Sprite2D::new_object();
		m_pSpriteObserver->SetTexture( m_pTexture );
		m_pSpriteObserver->Create(1, true, true);

		m_pSpriteObserver->SetEnable(0, false);
		m_pSpriteObserver->SetRect(0, 0.0f, (REAL32)nScreenHeight - 100.0f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
		m_pSpriteObserver->SetColor(0, 0, 0, 0, 150);
		m_pSpriteObserver->SetTextureCoord(0, 603.0f, 1.0f, 812.0f, 59.0f);		
		m_pSpriteObserver->SetEnable(false);

		for(INT32 i = 0; i < HUD_TEXT_OBSERVER_FLY_COUNT; i++)
		{
			m_pTextObserverFly[i] = i3TextNodeDX2::new_object();
			m_pTextObserverFly[i]->Create(  GetDefaultFontName(), FONT_COUNT_64, 8);

			if(i == HUD_TEXT_OBSERVER_FLY_MOVE_SPEED) {
				m_pTextObserverFly[i]->SetText(L"Move Speed");
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
			m_pTextObserverFly[i]->SetEnable(false);
			
			m_pHUDRoot->AddChild(m_pTextObserverFly[i]);
		}

		m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_MOVE_SPEED]->setPos( (UINT32)30.f, (UINT32)130.f);
		//m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_ROTATE_SPEED]->setPos( 30.f, 150.f);

		for(INT32 i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++)
		{
			m_pTextObserver[i] = i3TextNodeDX2::new_object();
			
			if( HUD_TEXT_OBSERVER_ID == i )
			{
				if(UserInfoContext::i()->IsGM_Observer())
				{
					m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_128, 25, FW_BOLD);	// revision 54410		
					m_pTextObserver[i]->setSize(400, 40);
				}
				else
				{
					m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_128, 15, FW_BOLD);	// revision 54410
					m_pTextObserver[i]->setSize(400, 20);
				}
				
			}
			else
			{
				m_pTextObserver[i]->Create(  GetDefaultFontName(), FONT_COUNT_128, 13);					// revision 54410
				m_pTextObserver[i]->setSize(400, 20);
			}

			{
				i3::stack_wstring wstr_observerText;

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

					i3::sprintf(wstr_observerText, L"%s %s", 
							GAME_RCSTRING("STBL_IDX_OBSERVER_WEAPON"),
							GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_CHANGEWEAPON]) );
				}
				else
				{
					wstr_observerText = GAME_RCSTRING_BY_HANDLE(arTemp[i]);
				}

				m_pTextObserver[i]->SetText(wstr_observerText);
			}

			
			m_pTextObserver[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			m_pTextObserver[i]->SetColor(255, 255, 255, 255);
			m_pTextObserver[i]->SetEnable(false);
		}
				
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		i3::pack::POINT2D pt = g_pFramework->GetCenterPosition(400, 100);
		
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->setPos( pt.x, nScreenHeight - 90);
		m_pTextObserver[HUD_TEXT_OBSERVER_NEXT]->setPos( 30, nScreenHeight - 60);
		m_pTextObserver[HUD_TEXT_OBSERVER_PREV]->setPos( 30, nScreenHeight - 40);
		m_pTextObserver[HUD_TEXT_OBSERVER_WEAPON]->setPos( nScreenWidth - 200 - 30, nScreenHeight - 60);
		m_pTextObserver[HUD_TEXT_OBSERVER_VIEW]->setPos( nScreenWidth - 200 - 30, nScreenHeight - 40);

		// Observer HP Gauge
		m_pObserverHPGaugeHUD = CHUD_ObserverHPGauge::new_object();
		m_pObserverHPGaugeHUD->Create(this);
		m_pObserverHPGaugeHUD->SetEnableGauge( false);

		m_bEnableObserverMode = false;
	}

	CreateGMObserverCallSignNumSprite();

	{
		//GM Pause 기능 스프라이트 초기화
		m_pSpritePause = i3Sprite2D::new_object();
		m_pSpritePause->SetTexture(m_pTexturePause);
		m_pSpritePause->Create(1, true, false);

		m_pSpritePause->SetEnable(0, false);
		m_pSpritePause->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth, (REAL32)nScreenHeight);
		m_pSpritePause->SetTextureCoord(0, 0.0f, 0.0f, 1024.0f, 572.0f);

		m_pHUDRoot->AddChild(m_pSpritePause);
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
}

void CHud::CreateGMObserverCallSignNumSprite()
{
	if(!UserInfoContext::i()->IsGM_Observer())
		return;

	if(m_gmObserverCallSignNumSpr)
		return;

	i3Texture* pTex2 = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD4);
	I3ASSERT(pTex2 != nullptr);

	m_gmObserverCallSignNumSpr = i3Sprite2D::new_object();	
	I3ASSERT(m_gmObserverCallSignNumSpr != nullptr);

	m_gmObserverCallSignNumSpr->Create(SLOT_MAX_COUNT, true, true);
	m_gmObserverCallSignNumSpr->SetTexture(pTex2);

	pTex2->Release();

	INT32 nRedCurIdx = 0;
	INT32 nBlueCurIdx = 0;

	m_gmObserverCallSignNumSpr->SetEnable(true);

	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_gmObserverCallSignNumSpr->SetEnable(i,false);
		m_gmObserverCallSignNumSpr->SetColor( i, 255,255,255,255);		

		if(i % 2 == 0)	// Red Team Gauge
		{
			m_gmObserverCallSignNumSpr->SetTextureCoord( i, 5, 5+(27.f*nRedCurIdx), 29.f, 29.f+(27.f*nRedCurIdx));
			m_gmObserverCallSignNumSpr->SetRect( i, 0.f, 0.f, 24.f, 24.f);
			nRedCurIdx++;

		}
		else // Blue Team Gauge
		{
			m_gmObserverCallSignNumSpr->SetTextureCoord( i, 35.0f, 5.0f+(27.f*nBlueCurIdx), 59.f, 29.f+(27.f*nBlueCurIdx));
			m_gmObserverCallSignNumSpr->SetRect( i, 0, 0.f, 24.f, 24.f);
			nBlueCurIdx++;
		}

		if(i == SLOT_MAX_COUNT-1) 
		{
			nRedCurIdx = 0;
			nBlueCurIdx = 0;
		}
	}

	getHUDRoot()->AddChild(m_gmObserverCallSignNumSpr);
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
	m_pSpriteGrenadeLauncherCrossHair->Create( 1, true, false);
	m_pSpriteGrenadeLauncherCrossHair->SetEnable(true);	
	m_pSpriteGrenadeLauncherCrossHair->SetTextureCoord( 0, 0.0f, 0.0f, (REAL32)nImageWidth, (REAL32)nImageHeight);
	
	m_pSpriteGrenadeLauncherCrossHair->SetRect( 0, (REAL32)(nHalfScreenWidth - nImageWidth/2), (REAL32)(nHalfScreenHeight - nImageHeight/2),
		(REAL32)(nImageWidth), (REAL32)(nImageHeight));
	
	m_pHUDRoot->AddChild( m_pSpriteGrenadeLauncherCrossHair);

	SetEnableGrenadeLauncherCrossHair( false);
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

	m_pWarnningHack->SetEnable( true);
}
void CHud::SetActivateHackWarnning(void)				
{ 
	m_pWarnningHack->SetActivate();		
}

void CHud::OnUpdate( REAL32 rDeltaSeconds )
{
	i3_prof_func();

 	i3GameObjBase::OnUpdate( rDeltaSeconds );

	//SetDamageDirection(0.0f);

	//	Key State
	UINT64 nPressKey	= g_pFramework->getKeyPress();

	UserInfoContext* userInfoCtx = UserInfoContext::i();

	// GMObserver
	if(userInfoCtx->IsGM_Observer())
	{
		if(g_pFramework->getKeyboard()->GetStrokeState(I3I_KEY_F9))
		{
			// -1 for toggle, 0:hide, 1:show
			//HUD::instance()->SetParameter(HUD::OBSERVER, HUD::OS_ShowCallSignNumber, -1); 

			if(userInfoCtx->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
			{
				// 원래대로 선수이름표시
				userInfoCtx->RemoveGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER);
			}
			else
			{
				// 선수이름표시를 콜사인번호표시로 대체
				userInfoCtx->AddGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER);
			}
		}

		if(m_gmObserverCallSignNumSpr)
			m_gmObserverCallSignNumSpr->SetEnable(false);
	}

//	if( !m_bOnlyNoticeUpdate)
	{	
		// Radio chat
		UpdateRadioChat( rDeltaSeconds );

		// Mouse sense
		if( nPressKey & GAME_KEY_CMD_MOUSESENE_DOWN )
		{
			if( m_fEtcMessageLocalTime > 0.3f || m_fEtcMessageLocalTime == 0.0f)
			{
				g_pEnvSet->m_rMouseSensivity -= 0.005f;
				if( g_pEnvSet->m_rMouseSensivity <= MIN_MOUSE_SENSIVITY)		g_pEnvSet->m_rMouseSensivity = MIN_MOUSE_SENSIVITY;

				i3::stack_wstring wstrNotice;
			#if defined(LOCALE_TAIWAN)	// revision 55598
				i3::sprintf( wstrNotice, L"%s  %.3f", GAME_RCSTRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
			#else
				i3::sprintf( wstrNotice, L"%s : %.3f", GAME_RCSTRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
			#endif
				m_pTextEtc->SetEtcText( wstrNotice);
			}

			m_fEtcMessageLocalTime += rDeltaSeconds;
		}
		else if( nPressKey & GAME_KEY_CMD_MOUSESENE_UP )
		{			
			if( m_fEtcMessageLocalTime > 0.3f || m_fEtcMessageLocalTime == 0.0f)
			{
				g_pEnvSet->m_rMouseSensivity += 0.005f;
				if( g_pEnvSet->m_rMouseSensivity > MAX_MOUSE_SENSIVITY)		g_pEnvSet->m_rMouseSensivity = MAX_MOUSE_SENSIVITY;

				i3::stack_wstring wstrNotice;
			#if defined(LOCALE_TAIWAN)	// revision 55598
				i3::sprintf( wstrNotice, L"%s  %.3f", GAME_RCSTRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
			#else
				i3::sprintf( wstrNotice, L"%s : %.3f", GAME_RCSTRING("STBL_IDX_MOUSE_SENSE"), g_pEnvSet->m_rMouseSensivity);
			#endif
				m_pTextEtc->SetEtcText( wstrNotice);
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

	if( m_pTrainingScore != 0 )
	{
		m_pTrainingScore->OnUpdate(rDeltaSeconds);
	}

	m_pRevengeHUD->OnUpdate(rDeltaSeconds);

	if( !m_bGameEnd)
	{
		if( m_pPanelTargetHP != nullptr)
			m_pPanelTargetHP->Update( rDeltaSeconds);
	}

	// EtcText Update
	m_pTextEtc->OnUpdate(rDeltaSeconds);
	m_pCHUD_CountDown->OnUpdate(rDeltaSeconds);

	// 상대방을 죽였을때 나오는 메시지입니다.
	UpdateInGameMessageList(rDeltaSeconds);

	// 폭탄 설치
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB)
		|| MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL)
		|| MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_RUN_AWAY))
	{
		ProcessBomb( rDeltaSeconds);

		if( CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INSTALLED )
		{
			UpdateBomb( rDeltaSeconds);
		}
	}

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )
	{
		ProcessUsurpationHUD(rDeltaSeconds);
		ProcessUsurpation(rDeltaSeconds);
	}


	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
	{
		ProcessEscape(rDeltaSeconds);
	}

	// Space 모드
	if (MyRoomInfoContext::i()->getStage()->IsSpace() == true)
	{
		ProcessSpace(rDeltaSeconds);
	}

	// Mission Message Update
	UpdateMissionMessage( rDeltaSeconds);

	// 플레이어 자신이 죽었을때 나오는 스프라이트 입니다.
	if( m_bEnableDeathSprite)
	{
		UpdateDeathSprite( rDeltaSeconds);		
	}

	m_pHelpHUD->OnUpdate(rDeltaSeconds);

	// 피격필터
	OnUpdateDamageFilter(rDeltaSeconds);
	OnUpdateDieFilter(rDeltaSeconds);

	// chat notice
	_ProcessChatNotice();

	if( m_pScope != nullptr)
		m_pScope->UpdateSniperScope( rDeltaSeconds);

	// 자유 카메라 
	if(m_bEnableObserverFlyMode) {
		if(i3::same_of<CGameCamera_FLY*>(g_pCamera))
		{
			i3::stack_wstring wstrTemp;

			i3::sprintf(wstrTemp, L"Move Speed %.3f", ((CGameCamera_FLY*)g_pCamera)->getMoveSpeed());
			m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_MOVE_SPEED]->SetText(wstrTemp);

			/*StringCchPrintf(szTemp, 256, "Rotate Speed %.3f", ((CGameCamera_FLY*)g_pCamera)->getRotateSpeed());
			m_pTextObserverFly[HUD_TEXT_OBSERVER_FLY_ROTATE_SPEED]->SetText(szTemp);*/
		}
	}

#if defined( ENABLE_TEST_COMMAND )
	if( g_pFramework->getCommandSet()->IsShowPos())
	{
		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
		if( pPlayer != nullptr)
		{
			i3::stack_wstring wstrPos;

			VEC3D *pos = pPlayer->GetPos();
			i3::sprintf( wstrPos, L"Player position: X(%.4f)\n, Y(%.4f)\n, Z(%.4f)\n", 
				i3Vector::GetX(pos), i3Vector::GetY(pos), i3Vector::GetZ(pos));
			SetMissionMessageString( wstrPos);
		}
	}

	if(CQAWeaponCMgr::i()->getOnWeaponCollision())
	{
		SetMissionMessageString( CQAWeaponCMgr::i()->getOutCollisionBox() );
	}
#endif

	ShowProfile();
}

bool CHud::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
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
					SetChatEnable(false);
					return true;
				}
			}
		}
	}

	return false;
}

void CHud::InitHud()
{
	static const REAL32 BULLET_MAX = 147.0f;

	i3_prof_func();

	m_fBulletScale = i3Math::div(BULLET_MAX, 100.0f);	
	m_bUpdateNotice = false;

	if( m_pPanelTargetHP != nullptr)
		m_pPanelTargetHP->Init();
}

void CHud::ResetHud(void)
{
	i3_prof_func();
	// HUD의 Reset작업을 수행합니다. 라운드 시작 및 게임 시작시 호출됨.
	// StageBattle::ProcessGameContext()에서 EVENT_START_BATTLE, EVENT_M_ROUNDSTART_BATTLE단계 수행시 호출됩니다.	
	if( m_pPanelTargetHP != nullptr)
		m_pPanelTargetHP->Reset();
	
	if (CGameMissionManager::i()->getMissionState() != GAME_MISSION_STATE_INBATTLE)
		m_bFirstHUD = false;

	ResetUsurpationHUD();
}

void CHud::SetEnableWithoutNotice( bool bFlag, GAME_GUI_STATUS nState, bool bDeath)
{
	i3_prof_func();

	INT32 i;

	m_pChainKills->SetEnableWithoutNotice( bFlag );

	if( m_pPanelTargetHP != nullptr)
		m_pPanelTargetHP->SetEnableWithoutNotice( bFlag, nState, bDeath);

	m_bOnlyNoticeUpdate = ! bFlag;

	if( bFlag)
	{
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) || 
			MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ANNIHILATION))
			SetEnableBombMission(true);

		m_bOneTimeAreaUpdate = false;
	}
	else
	{
		m_pSpriteMission->SetEnable( false);
		m_pSpriteBoss->SetEnable( false);

		for( i=0; i<HUD_BOMB_TEXT_COUNT; i++) m_pTextBomb[i]->SetEnable( false);
		for( i=0; i<HUD_MISSION_TEXT_COUNT; i++) m_pTextMission[i]->SetEnable( false);
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

	SetEnableObserverFly(false);

	switch( mod)
	{
	case HUD_MODE_DEFAULT:
		{
			SetEnableObserver(false);			
			SetEnableMiniMap( true);
			SetMoveChatting();

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( true);

			HUD::instance()->SetParameter( m_UItypelist, HUD::MISSION_DEFAULT );
			UIHUDNoticeShop::instance()->SetObserverMode(false);
		}
		break;
	case HUD_MODE_MISSION_ROUND_START:
		{
			SetEnableObserver(false);
			UpdateScore( false);
			SetEnableMiniMap( true);
			if( m_pMiniMap != nullptr)
				m_pMiniMap->SetMiniMapMode( CBattleHUD_MiniMap::MINIMAP_MODE_DEFAULT );

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( true);

			m_pRevengeHUD->SetRevenge( false);
		}
		break;
	case HUD_MODE_MISSION_SCORE:
		{
			//if(0) //cjtemp ScoreContext::i()->IsPendingTeamChange()) 
			//{
			//	//// 팀바꾸는중 HUD, 3초정도
			//	//HUD::instance()->SetParameter(HUD::DEFAUTOCHANGE, true);
			//}
			//else
			{
				HUD::instance()->SetParameter(HUD::GAME_RESULT, HUD::GRS_RoundShow);
				SetEnableMiniMap( false);
				SetMoveChatting();
				SetEnableBombMission(false);
				SetEnableMissionResult( false);
				SetEnableMissionMessage( false);
				SetEnableObserver(false);
			}

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( false);

			m_pRevengeHUD->SetRevenge( false);

			if( CBattleGui::i()->GetEnablePopup())
			{
				CBattleGui::i()->SetEnablePopup();
			}
		}
		break;
	case HUD_MODE_MISSION_OBSERVER:
		{
			SetEnableObserver(true);
			SetEnableMiniMap( true);
			SetMoveChatting();

			UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
			pBoard->SetVisible(true);

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( false);

			HUD::instance()->SetParameter( m_UItypelist, HUD::MISSION_OBSERVER );
			UIHUDNoticeShop::instance()->SetObserverMode(true);
		}
		break;
	case HUD_MODE_MISSION_FLY:
		{
			SetEnableBombMission(false);
			SetEnableMissionResult( false);
			SetEnableMissionMessage( false);
			SetEnableObserver(false);
			SetEnableMiniMap( false);
			SetMoveChatting();
			SetEnableObserverFly(true);

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( false);			

			m_pRevengeHUD->SetRevenge( false);
		}
		break;
	case HUD_MODE_NOTHING:
		{
			SetEnableBombMission(false);
			SetEnableMissionResult( false);
			SetEnableMissionMessage( false);
			SetEnableObserver(false);
			SetEnableMiniMap( false);
			SetMoveChatting();
			SetEnableObserverFly(false);

			if( m_pPanelTargetHP != nullptr)
				m_pPanelTargetHP->SetEnableTargetHP( false);

			UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
			pBoard->SetVisible(false);

			m_pRevengeHUD->SetRevenge( false);
		}
		break;
	case HUD_MODE_CHAOS:
	{
		SetEnableObserver(false);
		UpdateScore(false);
		SetEnableMiniMap(false);
		if (m_pMiniMap != nullptr)
			m_pMiniMap->SetMiniMapMode(CBattleHUD_MiniMap::MINIMAP_MODE_DEFAULT);

		if (m_pPanelTargetHP != nullptr)
			m_pPanelTargetHP->SetEnableTargetHP(true);

		m_pRevengeHUD->SetRevenge(false);
	}
	break;
	default:
		break;
	}
}

void CHud::SetMissionMessageString(const i3::wliteral_range& wMessageRng)
{
	i3_prof_func();

	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, true);
	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( true);
	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetText(wMessageRng);

	// Mission Message Time초기화를 합니다.
	m_fMissionMessageTime = 0.0f;
}

void CHud::SetMissionNoticeString( const i3::rc_wstring& wstrMessage, REAL32 rLifeTime, INT32 nOffsetX, INT32 nOffsetY, bool bAutoLineUp)
{
	i3_prof_func();

	INT32 nPosX = (INT32)m_fViewHalfWidth;
	INT32 iClanOffsetY = 0;
	INT32 iBombMissionOffsetY = 0;
	INT32 iLineUpOffsetY = 0;

	// Clan mark가 있으면 아래쪽으로 표시
	if( ChannelContext::i()->GetBattleChannel())
		iClanOffsetY = 18;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DEFENCE) )
		iClanOffsetY += 30;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_BOMB) )
		iBombMissionOffsetY += 40;
 
	VEC3D bombImgLeftTopPos;
	
	UIHudC4Indicator * pHud = static_cast<UIHudC4Indicator*>( HUD::instance()->GetHud(HUD::C4INDICATOR) );
	if( pHud != nullptr
		&& pHud->GetC4AvailablePosImage() != nullptr
		&& pHud->GetC4AvailablePosImage()->getAccumulatedPos() != nullptr)
	{
		if (bAutoLineUp)
			iLineUpOffsetY = -25;

		i3Vector::Copy(&bombImgLeftTopPos, pHud->GetC4AvailablePosImage()->getAccumulatedPos());
		REAL32 bombImgHeight = pHud->GetC4AvailablePosImage()->getSize()->y;
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->setPos(nPosX - 300 + nOffsetX, static_cast<UINT32>((bombImgLeftTopPos.y/2) + bombImgHeight + nOffsetY + iClanOffsetY + iBombMissionOffsetY + iLineUpOffsetY + 45.0f));

		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( true);
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetText(wstrMessage);

	}



	// Mission Notice Time초기화를 합니다.
	m_fMissionNoticeTimer = 0.0f;
	m_fMissionNoticeLifeTime = rLifeTime;
}

void CHud::SetEnableMissionMessage( bool bFlag)
{
	i3_prof_func();

	m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( bFlag);
	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, bFlag);
}

void CHud::SetEnableMissionNotice( bool bFlag)
{
	i3_prof_func();

	m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( bFlag);
}

void CHud::SetEnableMissionResult( bool bFlag)
{
	i3_prof_func();

	// 튜토리얼 모드에서는 표시하지 않는다.
	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) && 
		!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
	{
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, bFlag);
		m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, bFlag);
		if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
			m_pSpriteBoss->SetEnable( 0, bFlag);
	}
}

void CHud::SetRedRoundResult(INT32 nPosX, INT32 nPosY)
{
	i3_prof_func();

	REAL32 positionX = 0.f;
	REAL32 positionY = 0.f;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ) {
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
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_TEAM, 513.0f, 335.0f, 838.0f, 392.0f); // 'RED TEAM' in HUD.i3i

		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(325.0f*0.5f)+30.0f + 157.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 862.0f, 453.0f, 1019.0f, 510.0f); //  'WIN!' (red team) in HUD.i3i
#endif
	}
}

void CHud::SetBlueRoundResult(INT32 nPosX, INT32 nPosY)
{
	i3_prof_func();

	REAL32 positionX = 0.f;
	REAL32 positionY = 0.f;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ) {
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
		// Result
		m_pSpriteMission->SetRect( HUD_SPRITE_MISSION_RESULT, positionX+(364.0f*0.5f)+30.0f+157.0f, positionY, 157.0f, 57.0f);
		m_pSpriteMission->SetTextureCoord( HUD_SPRITE_MISSION_RESULT, 704.0f, 453.0f, 861.0f, 510.0f);
#endif
	}
}

void CHud::SetMissionRoundResult( INT32 nWinTeamIdx)
{
	i3_prof_func();

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) ) return;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= 0;
	INT32	nScreenHeight	= 0;

	if( pViewer != nullptr)
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
			if (0 == BattleSlotContext::i()->getMyTeam() - 1)
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


	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_TEAM, true);
	m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_RESULT, true);

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
		m_pSpriteBoss->SetEnable( 0, true);
}

void CHud::UpdateMissionMessage( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	// Mission Message
	if( m_pTextMission[HUD_MISSION_TEXT_MESSAGE] != nullptr && 
		m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->GetEnable())
	{
		m_fMissionMessageTime += rDeltaSeconds;
		if( m_fMissionMessageTime > 5.0f)
		{
			m_fMissionMessageTime = 0.0f;
			m_pSpriteMission->SetEnable( HUD_SPRITE_MISSION_MESSAGE_BG, false);
			m_pTextMission[HUD_MISSION_TEXT_MESSAGE]->SetEnable( false);
		}
	}

	// Mission Notice
	if( m_pTextMission[HUD_MISSION_TEXT_NOTICE] != nullptr &&  
		m_pTextMission[HUD_MISSION_TEXT_NOTICE]->GetEnable())
	{
		m_fMissionNoticeTimer += rDeltaSeconds;
		if( m_fMissionNoticeTimer > m_fMissionNoticeLifeTime ) //3.0f)
		{
			m_fMissionNoticeTimer = 0.0f;
 			m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetEnable( false);
			m_pTextMission[HUD_MISSION_TEXT_NOTICE]->SetText(L"");
		}
	}
}

void CHud::EnableDeathSprite( bool bFlag)
{
	i3_prof_func();

	if( m_bEnableDeathSprite == bFlag ) return;

	m_bEnableDeathSprite = bFlag;

	if( bFlag)
	{
		m_fDeathSpriteTime = 0.0f;
		m_pSpriteDeath->SetEnable(true);		
	}
	else
	{
		m_pSpriteDeath->SetColor( 0, 255,255,255,0);
		m_pSpriteDeath->SetColor( 1, 255,255,255,0);
		m_pSpriteDeath->SetEnable(false);
	}
}

void CHud::UpdateDeathSprite( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	COLOR* pCol = nullptr;
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

void CHud::SetEnableBombMission(bool bFlag)		
{ 
	i3_prof_func();

	m_bBombMissionEnable = bFlag;
	
	if( !bFlag)
	{
		UIHudC4Indicator * pHud = static_cast<UIHudC4Indicator*>( HUD::instance()->GetHud(HUD::C4INDICATOR) );
		pHud->SetEnableInstalledImage(false);
		pHud->SetEnableAvailablePositionImage(false);
	}
}

void CHud::ProcessBomb( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);

	bool	bCheckRange = false;
	if( m_bEnableObserverMode)
	{
		SetEnableMissionMessage( false);
		return;
	}

	UIHudC4Indicator * pHud = static_cast<UIHudC4Indicator*>( HUD::instance()->GetHud(HUD::C4INDICATOR) );

	pHud->SetEnableAvailablePositionImage(false);
	pHud->SetEnableInstalledImage(false);

	INT32 nPosX = 0;
	INT32 nPosY = 0;
	INT32 nScreenWidth = 0;
	INT32 nScreenHeight= 0;

	REAL32 fClanOffsetY = 5.0f;

	if( ChannelContext::i()->GetBattleChannel())
	{
		fClanOffsetY = 18.0f;
	}

	//1 폭파 가능지역인지...
	if( CGameMissionManager::i()->IsEnableMy() )	
	{	
		if( g_pViewer != nullptr)
		{
			nScreenWidth = g_pViewer->GetViewWidth();
			nScreenHeight = g_pViewer->GetViewHeight();
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		// 폭파 가능지역이라면, 폭탄이미지를 활성화 합니다.
		pHud->SetEnableAvailablePositionImage(true);
		bCheckRange = true;
	}
	else
	{
		// 폭파 가능지역이 아니라면, 폭탄이미지를 비활성화 합니다.
		pHud->SetEnableAvailablePositionImage(false);
		SetEnableMissionMessage( false);
		m_bOneTimeAreaUpdate = false;
	}

	if( bCheckRange )
	{
		switch( CGameMissionManager::i()->getMissionState() )
		{
		case GAME_MISSION_STATE_INBATTLE :
			if( CGameMissionManager::i()->getMyMissionObject() == MY_MISSION_OBJECT_OFFENSIVE )
			{	// Offensive team
				if( !m_bOneTimeAreaUpdate)
				{
					SetMissionMessageString( GAME_RCSTRING("STBL_IDX_BOMB_MISSION_AREA"));		// 폭파목표지역입니다. 폭탄으로 교체후...
					m_bOneTimeAreaUpdate = true;
				}

				// 폭탄설치중인지...		
				if( CGameMissionManager::i()->IsInstalling( m_CurrentUserIdx ) )
				{
					SetEnableMissionMessage( false);

					// 게이지 증가
					REAL32 fSetTimeByUser = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( m_CurrentUserIdx);
					fSetTimeByUser += BattleSlotContext::i()->getMissionBomb_SetTime_UsingSkill( m_CurrentUserIdx);

					REAL32 fGage = ( 1.f - CGameMissionManager::i()->getInstallingTimer( m_CurrentUserIdx ) / fSetTimeByUser );

					I3TRACE("progress Value : %f\n", pHud->GetC4ProgressBar()->getProgress());
					pHud->SetEnableC4Progress(true);

					i3UIProgressBar * pBar = pHud->GetC4ProgressBar();
					pBar->setProgress(fGage);

					I3TRACE("fGage Value : %f\n", fGage);
				}
				else
				{
					// 게이지 없앰!!
					pHud->SetEnableC4Progress(false);				
				}
			}
			
			break;

		case GAME_MISSION_STATE_INSTALLED:
			if( CGameMissionManager::i()->getMyMissionObject() == MY_MISSION_OBJECT_DEFENSIVE || 
				MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
			{	// Defensive team
				if( !m_bOneTimeAreaUpdate )
				{
					i3::stack_wstring wstr_message;
					GAME_KEY_STRUCT* pKeyMap = g_pEnvSet->getGameKeyMap();

					i3::sprintf(wstr_message, L"%s %s", 
							GameUI::GetKeyboardString( &pKeyMap[GAME_KEY_MAP_SPECIALACTION] ),
							GAME_RCSTRING("STBL_IDX_BOMB_MISSION_DETTACH_EXPLAIN"));				  

					SetMissionMessageString(wstr_message);
					m_bOneTimeAreaUpdate = true;
				}

				// 폭탄해제중인지...
				if( CGameMissionManager::i()->IsUninstalling( m_CurrentUserIdx ) )
				{
					SetMissionNoticeString(GAME_RCSTRING("STBL_IDX_BOMB_MISSION_DETTACH_ING"));			// 폭탄 해체중입니다.
					m_bDettachBomb = true;

					SetEnableMissionMessage( false);

					// 게이지 증가
					REAL32 fDisarmTimeByUser = BattleSlotContext::i()->getMissionBomb_DisarmTime_UsingSpeedKit( m_CurrentUserIdx);
					fDisarmTimeByUser += BattleSlotContext::i()->getMissionBomb_DisarmTime_UsingSkill( m_CurrentUserIdx);

					REAL32 fGage = ( 1.f - CGameMissionManager::i()->getUninstallingTimer( m_CurrentUserIdx ) / fDisarmTimeByUser );	
					
					pHud->SetEnableC4Progress(true);
					i3UIProgressBar * pBar = pHud->GetC4ProgressBar();
					pBar->setProgress(fGage);
				}
				else // 폭탄해제중이 아니라면 타이머 리셋
				{
					if( m_bDettachBomb) 
					{
						SetEnableMissionNotice( false);
						m_bDettachBomb = false;
					}

					if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
					{
						SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_4"), 1800.f);
						
					}
					// 게이지 없앰!!
					pHud->SetEnableC4Progress(false);
				}
			}
			break;
		case GAME_MISSION_STATE_UNINSTALLED:
			{
				SetEnableBombMission( false);
			}
			break;

		default :
			// 게이지 없앰!!
			pHud->SetEnableC4Progress(false);
			break;
		}
	}
	else
	{
		// revision 55571 여기 예외처리 해줘야 함.
		//단순히 끄는게 아니라, checkRange == false일 때 폭탄 해제중이라면?
		if( m_bDettachBomb) 
		{
			SetEnableMissionNotice( false);
			m_bDettachBomb = false;
		}

		pHud->SetEnableC4Progress(false);
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

	REAL32 fTime = CGameMissionManager::i()->getExplosionTimer();

	REAL32 fClanOffsetY = 5.0f;

	if( ChannelContext::i()->GetBattleChannel())
	{
		fClanOffsetY = 18.0f;
	}
	
	if( g_pViewer != nullptr)
	{
		nScreenWidth = g_pViewer->GetViewWidth();
		nPosX = (INT32)(nScreenWidth * 0.5f);
	}
	UIHudC4Indicator * pHud = static_cast<UIHudC4Indicator*>( HUD::instance()->GetHud(HUD::C4INDICATOR) );

	if( !MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
	{
		if( g_pCharaManager->getCharaInfo( m_CurrentUserIdx)->GetTeam() & CHARACTER_TEAM_RED)
		{
			SetEnableMissionMessage( false);
			pHud->SetEnableC4Progress(false);
		}
	}
	
	pHud->SetEnableInstalledImage(true);
	pHud->SetEnableAvailablePositionImage(false);

	// Timer

	i3::stack_wstring wstrTime;
	{
		INT32 Minute = ((INT32)fTime)/60;
		INT32 Second = ((INT32)fTime)%60;
		INT32 MiliSecond = (INT32)((fTime - ((INT32)fTime))*100.0f);

		if( Minute <0) Minute = 0;
		if( Second <0) Second = 0;
		if( MiliSecond <0) MiliSecond = 0;

		if( Minute > 0)
		{
			i3::sprintf( wstrTime, L"%02d:%02d:%02d", Minute, Second, MiliSecond);
		}
		else
		{
			i3::sprintf( wstrTime, L"%02d:%02d", Second, MiliSecond);
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
			
			pHud->GetC4Timer()->SetTextColor(&col);
		}
		else
		{
			i3Color::Set( &col, (UINT8) 255, 255, 255, 255);			
			pHud->GetC4Timer()->SetTextColor(&col);
		}
	}

	pHud->GetC4Timer()->SetText(wstrTime);
}

// Vector값에서 2D 좌표계에서의 각도를 구함. (X,Z 평면에서 Z축 방향일 경우 0.0f );
REAL32 CHud::GetRotationAngle2D( VEC3D *pDirection )
{
	i3_prof_func();

	//REAL32 Rotation = -1.0f * atan2f( i3Vector::GetX( pDirection ), i3Vector::GetY( pDirection ) ); // XY 평면
	REAL32 Rotation = atan2f( i3Vector::GetX( pDirection ), i3Vector::GetZ( pDirection ) ); // XZ 평면
	return i3Math::NormalizeAngle( Rotation );
}

// InGameMessage 생성 - Notice 타입
void CHud::CreateInGameMessage(const i3::wliteral_range& wTextRng, COLOR* pColor, INT32 AssistIdx, INT32 DeathIdx)
{
	InGameMessage* message = GetEmptyInGameMessage();

	if (message == nullptr)
		return;

	message->Reset();

	i3Viewer *	pViewer = g_pFramework->GetViewer();
	INT32		nScreenWidth = pViewer->GetViewWidth();
	INT32		StartPosOffset = (INT32)(13 * i3GuiRoot::getHeightRatioBy768());
	INT32		nTextWidth = 0;
	INT32		idx = GetEmptyTextIndex(m_pNoticeText);

	// 비어있는 text가 없을 경우 출력 불가
	if (idx == -1)
		return;

	m_nInGameMessageCount++;

	m_pNoticeText[idx]->SetEnable(true);

	if (pColor != nullptr)
		m_pNoticeText[idx]->SetColor(pColor);

	m_pNoticeText[idx]->SetText(wTextRng);
	nTextWidth = m_pNoticeText[idx]->getTextWidth();
	m_pNoticeText[idx]->setPos(nScreenWidth - StartPosOffset - nTextWidth, (UINT32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT));

	message->mType = INGAME_MESSAGE_NOTICE;
	message->strNotice = m_pNoticeText[idx];
	message->nIdx = m_nInGameMessageCount;

	if (AssistIdx != -1 && AssistIdx != DeathIdx)
	{
		VEC2D vecKiller;
		m_pNoticeText[idx]->getPos(&vecKiller);

		CreateInGameMessage(&vecKiller, AssistIdx);
	}
}

// InGameMessage 생성 - Point 타입
void CHud::CreateInGameMessage(INT32 nUserIdx, INT32 nObjectIdx, INT32 nPoint)
{
	if (0 > nUserIdx || SLOT_MAX_COUNT <= nUserIdx)
	{
		I3TRACE("[WARNING] CreateInGameMessage() - Under/over-flow nUserIdx(%d)\n", nUserIdx);
		return;
	}

	InGameMessage* message = GetEmptyInGameMessage();

	if (message == nullptr)
		return;

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx(nUserIdx);

	if (pChara == nullptr)
		return;

	INT32		objIdx = 0;
	i3Viewer *	pViewer = g_pFramework->GetViewer();
	INT32		nScreenWidth = pViewer->GetViewWidth();
	INT32		StartPosOffset = (INT32)(63 * i3GuiRoot::getHeightRatioBy768());
	INT32		nLeftIdx = GetEmptyTextIndex(m_pLeftText);
	INT32		nRightIdx = GetEmptyTextIndex(m_pRightText);
	INT32		nSpriteIdx = GetEmptySpriteIndex(m_pSpriteKillMark);
	INT32		nTextWidth = 0;
	VEC2D		vecPos;

	if (!CheckIndex(nLeftIdx) || !CheckIndex(nRightIdx) || !CheckIndex(nSpriteIdx))
		return;

	message->Reset();

	if (pChara->getTeam() == TEAM_RED)
	{
		m_pLeftText[nLeftIdx]->SetColor(252, 88, 88, 255);	// Red
		objIdx = 1;
	}
	else
	{
		m_pLeftText[nLeftIdx]->SetColor(67, 182, 243, 255);	// Blue
		objIdx = 0;
	}

	i3::stack_wstring wstrTemp = BattleSlotContext::i()->getNickForSlot(nUserIdx);
	if (i3::generic_string_size(wstrTemp) == 0)
	{
		I3ASSERT_0;
	}

	m_nInGameMessageCount++;

	m_pLeftText[nLeftIdx]->setSize((UINT32)(512 * i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
	m_pLeftText[nLeftIdx]->SetText(wstrTemp);

	//EscapeMission 공룡전은 추가 포인트를 표시하지 않습니다.
	if ((!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))
		&& !(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)))
	{
		m_pRightText[nRightIdx]->SetColor(125, 125, 0, 255);
		m_pRightText[nRightIdx]->SetEnable(true);
		i3::sprintf(wstrTemp, GAME_RCSTRING("STR_TBL_INGAME_HUD_GET_EXP"), nPoint);/*경험치 + %d*/
		m_pRightText[nRightIdx]->setSize((UINT32)(512 * i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
		m_pRightText[nRightIdx]->SetText(wstrTemp);
	}

	nTextWidth = m_pLeftText[nLeftIdx]->getTextWidth() + m_pRightText[nRightIdx]->getTextWidth();
	m_pLeftText[nLeftIdx]->setPos(nScreenWidth - StartPosOffset - nTextWidth, (UINT32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT));
	m_pLeftText[nLeftIdx]->getPos(&vecPos);
	m_pLeftText[nLeftIdx]->SetEnable(true);

	REAL32 rX = i3Vector::GetX(&vecPos) + m_pLeftText[nLeftIdx]->getTextWidth() + (20 * i3GuiRoot::getHeightRatioBy768());
	m_pRightText[nRightIdx]->setPos((INT32)rX, (INT32)(i3Vector::GetY(&vecPos)));

	// 입력된 모든 Text로부터 길이값을 가져와 KillMark위치를 정합니다.
	{
		nTextWidth = m_pLeftText[nLeftIdx]->getTextWidth() + m_pRightText[nRightIdx]->getTextWidth();

		{
			// 위치 계산
			rX = i3Vector::GetX(&vecPos) + nTextWidth + (23 * i3GuiRoot::getHeightRatioBy768());
			REAL32 rY = i3Vector::GetY(&vecPos) - (5.0f*i3GuiRoot::getHeightRatioBy768());

			m_pSpriteKillMark[nSpriteIdx]->SetTexture(m_pTexture);

			STAGE_ID	StageUID = MyRoomInfoContext::i()->getStage()->GetStageUID();
			STAGE_MODE	StageType = MyRoomInfoContext::i()->getStage()->GetStageMode();

			switch (StageUID)
			{
			case STAGE_UID_BREAKDOWN:
			{
				if (objIdx == 0)	m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 870.0f, 149.0f, 902.0f, 173.0f);
				else					m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 910.0f, 149.0f, 937.0f, 174.0f);
			}
			break;
			case STAGE_UID_GIRAN:
			case STAGE_UID_GIRAN2:
				break;
			case STAGE_UID_HELISPOT:
			{
				if (objIdx == 0)	m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 870.0f, 149.0f, 902.0f, 173.0f);
				else					m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 933.0f, 282.0f, 966.0f, 306.0f);
			}
			break;
			case STAGE_UID_BLACKPANTHER:
			{
				m_pSpriteKillMark[nSpriteIdx]->SetTexture(m_pTextureHUD3);

				if (nObjectIdx == 0)	m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 583.f, 116.0f, 615.0f, 140.0f);
				else					m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 623.0f, 116.0f, 655.0f, 140.0f);
			}
			break;
			case 1:
			default:
			{
				switch (StageType)
				{
				case STAGE_MODE_DESTROY:
				case STAGE_MODE_DEFENCE:
					I3PRINTLOG(I3LOG_WARN, "HUD no work.");
					break;
				case STAGE_MODE_ESCAPE:
				case STAGE_MODE_CONVOY:
					m_pSpriteKillMark[nSpriteIdx]->SetTexture(m_pTextureHUD3);
					if (nObjectIdx == 0)	m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 485.0f, 152.0f, 518.0f, 180.0f);
					g_pSndMng->StartAnnounceMessage(GTA_POINT_CONVOY);
					break;
				}
			}
			break;
			}

			m_pSpriteKillMark[nSpriteIdx]->SetRect(0, rX, rY, 32.0f*i3GuiRoot::getHeightRatioBy768(), 24.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteKillMark[nSpriteIdx]->SetEnable(true);
		}

		message->pSprite = m_pSpriteKillMark[nSpriteIdx];
	}

	if (BattleSlotContext::i()->getMySlotIdx() == nUserIdx)
	{
		nSpriteIdx = GetEmptySpriteIndex(m_pSpriteKillTextBG);

		if (CheckIndex(nSpriteIdx))
		{
			m_pSpriteKillTextBG->SetEnable(nSpriteIdx, true);
			m_pSpriteKillTextBG->SetRect(nSpriteIdx, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()), i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768())
				, 319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());

			message->nBGImageIdx = nSpriteIdx;
		}
	}

	message->mType = INGAME_MESSAGE_POINT;
	message->strLeft = m_pLeftText[nLeftIdx];
	message->strRight = m_pRightText[nRightIdx];
	message->nIdx = m_nInGameMessageCount;
}

// InGameMessage 생성 - TrainingPoint 타입
void CHud::CreateInGameMessage(INT32 nObjectIdx, INT32 nPoint)
{
	InGameMessage* message = GetEmptyInGameMessage();

	if (message == nullptr)
		return;

	CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	if (pMyChara == nullptr)
		return;

	i3Viewer *	pViewer = g_pFramework->GetViewer();
	INT32		nScreenWidth = pViewer->GetViewWidth();
	INT32		StartPosOffset = (INT32)(13 * i3GuiRoot::getHeightRatioBy768());
	INT32		nUserIdx = 0;
	INT32		nTextWidth = 0;
	INT32		nLeftIdx = GetEmptyTextIndex(m_pLeftText);
	INT32		nRightIdx = GetEmptyTextIndex(m_pRightText);
	INT32		nSpriteIdx = GetEmptySpriteIndex(m_pSpriteKillMark);
	nUserIdx = BattleSlotContext::i()->getMySlotIdx();
	VEC2D		vecPos;

	if (!CheckIndex(nLeftIdx) || !CheckIndex(nRightIdx) || !CheckIndex(nSpriteIdx))
		return;

	message->Reset();

	if (pMyChara->getTeam() == TEAM_RED)
		m_pLeftText[nLeftIdx]->SetColor(252, 88, 88, 255);	// Red
	else
		m_pLeftText[nLeftIdx]->SetColor(67, 182, 243, 255);	// Blue

	i3::stack_wstring wstrTemp = BattleSlotContext::i()->getNickForSlot(nUserIdx);
	if (i3::generic_string_size(wstrTemp) == 0)
	{
		I3ASSERT_0;
	}

	m_nInGameMessageCount++;

	m_pLeftText[nLeftIdx]->setSize((UINT32)(512 * i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
	m_pLeftText[nLeftIdx]->SetText(wstrTemp);

	if (nObjectIdx == TargetObject::TYPE_FRIEND)
	{
		i3::sprintf(wstrTemp, L"-%d", nPoint);
	}
	else if (nObjectIdx == TargetObject::TYPE_ENEMY)
	{
		i3::sprintf(wstrTemp, L"+%d", nPoint);
	}

	m_pRightText[nRightIdx]->SetColor(255, 255, 255, 255);
	m_pRightText[nRightIdx]->SetEnable(true);
	m_pRightText[nRightIdx]->setSize((UINT32)(512 * i3GuiRoot::getHeightRatioBy768()), (UINT32)NOTICE_TEXT_HEIGHT);
	m_pRightText[nRightIdx]->SetText(wstrTemp);

	nTextWidth = m_pLeftText[nLeftIdx]->getTextWidth() + m_pRightText[nRightIdx]->getTextWidth();
	m_pLeftText[nLeftIdx]->setPos(nScreenWidth - StartPosOffset - nTextWidth, (UINT32)((m_nInGameMessageCount)* NOTICE_LINE_HEIGHT));
	m_pLeftText[nLeftIdx]->getPos(&vecPos);

	// 입력된 모든 Text로부터 길이값을 가져와 타겟스프라이트의 위치를 정합니다.
	{
		nTextWidth = m_pLeftText[nLeftIdx]->getTextWidth();

		{
			// 위치 계산
			REAL32 rX = i3Vector::GetX(&vecPos) + nTextWidth + 11;
			REAL32 rY = i3Vector::GetY(&vecPos) - 10.0f;

			m_pSpriteKillMark[nSpriteIdx]->SetTexture(m_pTexture);

			if (nObjectIdx == TargetObject::TYPE_ENEMY)			m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 144.0f, 411.0f, 180.0f, 447.0f);
			else if (nObjectIdx == TargetObject::TYPE_FRIEND)	m_pSpriteKillMark[nSpriteIdx]->SetTextureCoord(0, 105.0f, 411.0f, 141.0f, 447.0f);

			m_pSpriteKillMark[nSpriteIdx]->SetRect(0, rX, rY, 36.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
			m_pSpriteKillMark[nSpriteIdx]->SetEnable(true);
		}
	}

	// 포인트
	{
		REAL32 rX = i3Vector::GetX(&vecPos) + m_pLeftText[nLeftIdx]->getTextWidth() + (56 * i3GuiRoot::getHeightRatioBy768());
		m_pRightText[nRightIdx]->setPos((INT32)rX, (INT32)(i3Vector::GetY(&vecPos)));
	}

	message->mType = INGAME_MESSAGE_TRAININGPOINT;
	message->nIdx = m_nInGameMessageCount;
	message->pSprite = m_pSpriteKillMark[nSpriteIdx];
	message->strLeft = m_pLeftText[nLeftIdx];
	message->strRight = m_pRightText[nRightIdx];
}

// InGameMessage 생성 - KILL 타입
void CHud::CreateInGameMessage(INT32 KillerIdx, INT32 DeathIdx, INT32 AssistIdx, CWeaponInfo * pWeaponInfo, bool bHeadShot, bool bObjDefence, INT32 nObjIndex)
{
	InGameMessage* message = GetEmptyInGameMessage();

	if (message == nullptr)
		return;

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

	if (0 > AssistIdx || SLOT_MAX_COUNT <= AssistIdx)
	{
		I3TRACE("[WARNING] SetKillText() - Under/over-flow AssistIdx(%d)\n", AssistIdx);
		return;
	}

	i3Viewer *	pViewer = g_pFramework->GetViewer();
	INT32		nScreenWidth = pViewer->GetViewWidth();
	INT32		nLeftIdx = GetEmptyTextIndex(m_pLeftText);
	INT32		nRightIdx = GetEmptyTextIndex(m_pRightText);
	INT32		nSpriteIdx = GetEmptySpriteIndex(m_pSpriteKillMark);
	INT32		nDeathTextWidth = 0;
	INT32		nKillerTextWidth = 0;
	INT32		nTotalTextWidth = 0;
	INT32		nBGImageIdx = -1;
	INT32		nSubImageIdx = -1;
	VEC2D		vecPos;

	i3::wstring wstrTemp;

	if (!CheckIndex(nLeftIdx) || !CheckIndex(nRightIdx) || !CheckIndex(nSpriteIdx))
		return;

	message->Reset();

	{	// 죽은 자
		if (BattleSlotContext::i()->IsRedTeam(DeathIdx))
			m_pRightText[nRightIdx]->SetColor(252, 88, 88, 255); // Red
		else
			m_pRightText[nRightIdx]->SetColor(67, 182, 243, 255); // Blue

		m_pRightText[nRightIdx]->SetEnable(true);
		m_pRightText[nRightIdx]->SetText(BattleSlotContext::i()->getNickForSlot(DeathIdx));
		nDeathTextWidth = m_pRightText[nRightIdx]->getTextWidth();
		nTotalTextWidth += nDeathTextWidth;
	}

	if ( nObjIndex == -1)
	{
		// 오브젝트에 의한 사망이 아닐 때

		// 자살이 아니고  적에 의한 사망
		if ( KillerIdx != DeathIdx && !MyRoomInfoContext::i()->getStage()->IsAiMode()
			&& BattleSlotContext::i()->getTeam(KillerIdx) != BattleSlotContext::i()->getTeam(DeathIdx) )
		{
			UIBattleFrame * pFrame = UIBattleFrame::i();

			UIHUDRecordBoard * pRecordBoard = (UIHUDRecordBoard*)pFrame->GetHUD(UIHUD_RECORD_BOARD);

			bool bEnableBoard = false;
			if ( BattleSlotContext::i()->getMySlotIdx() == KillerIdx)
			{
				pRecordBoard->UIRecordRevenge( DeathIdx, 1);
			}
			else if ( BattleSlotContext::i()->getMySlotIdx() == DeathIdx)
			{
				pRecordBoard->UIRecordRevenge( KillerIdx, -1);
				bEnableBoard = true;
			}

			pFrame->EnableHUD( UIHUD_RECORD_BOARD, bEnableBoard);
		}

		// Killer
		if (BattleSlotContext::i()->IsRedTeam(KillerIdx))
			m_pLeftText[nLeftIdx]->SetColor(252, 88, 88, 255); // Red
		else
			m_pLeftText[nLeftIdx]->SetColor(67, 182, 243, 255); // Blue

		wstrTemp = BattleSlotContext::i()->getNickForSlot(KillerIdx);

		m_pLeftText[nLeftIdx]->SetEnable(true);
		m_pLeftText[nLeftIdx]->SetText(wstrTemp);
		nKillerTextWidth = m_pLeftText[nLeftIdx]->getTextWidth();
		nTotalTextWidth += nKillerTextWidth;
	}
	else
	{
		// 오브젝트에 의한 사망
		wstrTemp = BattleSlotContext::i()->getNickForSlot(KillerIdx);

		CStageBattle * pStage = (CStageBattle*)g_pFramework->GetCurrentStage();
		StageObjectListPtr pObjList = pStage->GetStageObjectList();
		if (pObjList != nullptr)
		{
			if (nObjIndex >= (INT32)pObjList->size())
				return;

			i3Object * pObj = (i3Object*)(*pObjList)[nObjIndex];

			if (pObj->hasName())
				i3::mb_to_utf16(pObj->GetName(), wstrTemp);
		}

		if (i3::generic_string_size(wstrTemp) == 0)
		{
			I3ASSERT_0;
		}

		m_pLeftText[nLeftIdx]->SetColor(252, 255, 255, 255);
		m_pLeftText[nLeftIdx]->SetEnable(true);
		m_pLeftText[nLeftIdx]->SetText(wstrTemp);
		nKillerTextWidth = m_pLeftText[nLeftIdx]->getTextWidth();
		nTotalTextWidth += nKillerTextWidth;
	}

	// 사이즈 오버 조절
	while (nTotalTextWidth > (200 * i3GuiRoot::getHeightRatioBy768()))
	{
		nTotalTextWidth = 0;

		if (nDeathTextWidth > nKillerTextWidth)
		{
			nDeathTextWidth -= (INT32)(2 * i3GuiRoot::getHeightRatioBy768());
		}
		else
		{
			nKillerTextWidth -= (INT32)(2 * i3GuiRoot::getHeightRatioBy768());
		}

		nTotalTextWidth = (nDeathTextWidth + nKillerTextWidth);
	}

	m_nInGameMessageCount++;

	m_pRightText[nRightIdx]->setSize(nDeathTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
	m_pRightText[nRightIdx]->SetTextEllipsis(BattleSlotContext::i()->getNickForSlot(DeathIdx));
	INT32 textWidth = m_pRightText[nRightIdx]->getTextWidth();
	m_pRightText[nRightIdx]->setPos((UINT32)(nScreenWidth - textWidth - (15.0f*i3GuiRoot::getHeightRatioBy768())), (UINT32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT));
	m_pRightText[nRightIdx]->getPos(&vecPos);

	// 입력된 모든 Text로부터 길이값을 가져와 KillMark위치를 정합니다.
	{
		// 어떤 무기로 죽었는지 셋팅!
		bool bHP_Up = false;

		message->mType = INGAME_MESSAGE_KILL;

		if (g_pCharaManager->IsHP_Recovery(KillerIdx, DeathIdx)) {
			bHP_Up = true;
		}

		// KillMark
		message->pSprite = SetKillWeapon(pWeaponInfo, nRightIdx, nSpriteIdx, bHP_Up, nObjIndex, bHeadShot);	// 좌표 확인 필요

		if (bObjDefence)
		{
			message->fScale = 3.0f;

			// 오브젝트/수류탄으로 죽었을 경우는 헤드샷이 없음!  
			if (bHeadShot && pWeaponInfo && pWeaponInfo->isEnableHeadShot())
			{
				nSubImageIdx = GetEmptySpriteIndex(m_pSpriteObjDefenctAndHead);
				if (CheckIndex(nSubImageIdx))
				{
					message->mType = INGAME_MESSAGE_DEFENCT_HEAD;
					m_pSpriteObjDefenctAndHead->SetEnable(nSubImageIdx, true);
					m_pSpriteObjDefenctAndHead->SetRect(nSubImageIdx, i3Vector::GetX(&vecPos) - (72.0f*i3GuiRoot::getHeightRatioBy768()),
						i3Vector::GetY(&vecPos) - (11.0f*i3GuiRoot::getHeightRatioBy768()), 44.0f * message->fScale * i3GuiRoot::getHeightRatioBy768(),
						38.0f * message->fScale * i3GuiRoot::getHeightRatioBy768());
				}
			}
			else
			{
				nSubImageIdx = GetEmptySpriteIndex(m_pSpriteObjectDefence);
				if (CheckIndex(nSubImageIdx))
				{
					message->mType = INGAME_MESSAGE_OBJECT_DEFENCE;
					m_pSpriteObjectDefence->SetEnable(nSubImageIdx, true);
					m_pSpriteObjectDefence->SetRect(nSubImageIdx, i3Vector::GetX(&vecPos) - (65.0f*i3GuiRoot::getHeightRatioBy768()),
						i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()), 28.0f * message->fScale * i3GuiRoot::getHeightRatioBy768(),
						30.0f * message->fScale * i3GuiRoot::getHeightRatioBy768());
				}
			}
		}
		else
		{
			// Head Shot 
			if (bHeadShot && pWeaponInfo && pWeaponInfo->isEnableHeadShot())
			{
				nSubImageIdx = GetEmptySpriteIndex(m_pSpriteHeadShot);
				if (CheckIndex(nSubImageIdx))
				{
					message->fScale = 3.0f;
					message->mType = INGAME_MESSAGE_KILL_HEADSHOT;
					m_pSpriteHeadShot->SetEnable(nSubImageIdx, true);
					m_pSpriteHeadShot->SetRect(nSubImageIdx, i3Vector::GetX(&vecPos) - (75.0f*i3GuiRoot::getHeightRatioBy768()),
						i3Vector::GetY(&vecPos) - (10.0f*i3GuiRoot::getHeightRatioBy768()), 55.0f * message->fScale * i3GuiRoot::getHeightRatioBy768(),
						36.0f * message->fScale * i3GuiRoot::getHeightRatioBy768());
				}
			}
		}
	}

	{
		m_pLeftText[nLeftIdx]->setSize(nKillerTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
		m_pLeftText[nLeftIdx]->SetTextEllipsis(wstrTemp);

		textWidth = m_pLeftText[nLeftIdx]->getTextWidth();
		m_pLeftText[nLeftIdx]->setPos((UINT32)(i3Vector::GetX(&vecPos) - (100.0f*i3GuiRoot::getHeightRatioBy768()) - textWidth), (UINT32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT));
	}

	// 하이라이트 효과
	if (BattleSlotContext::i()->getMySlotIdx() == KillerIdx || BattleSlotContext::i()->getMySlotIdx() == DeathIdx)
	{
		nBGImageIdx = GetEmptySpriteIndex(m_pSpriteKillTextBG);
		if (CheckIndex(nBGImageIdx))
		{
			m_pSpriteKillTextBG->SetEnable(nBGImageIdx, true);
			m_pSpriteKillTextBG->SetRect(nBGImageIdx, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()), i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()),
				319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());
		}
	}

	message->nBGImageIdx = nBGImageIdx;
	message->nSubImageIdx = nSubImageIdx;
	message->nIdx = m_nInGameMessageCount;
	message->strLeft = m_pLeftText[nLeftIdx];
	message->strRight = m_pRightText[nRightIdx];

	// ### 어시스트 ###
	// 자살일 경우 출력 안함(AssistIdx와 DeathIdx가 같을 경우)
	// 어시스트한 사람이 없을 경우 출력 안함(AssistIdx와 KillerIdx가 같을 경우)
	if (AssistIdx != KillerIdx && AssistIdx != DeathIdx && m_InGameMessageList.size() <= HUD_NOTICE_COUNT)
	{
		VEC2D vecKiller;
		m_pLeftText[nLeftIdx]->getPos(&vecKiller);

		CreateInGameMessage(&vecKiller, AssistIdx);
	}
}

// InGameMessage 생성 - Assist 타입
void CHud::CreateInGameMessage(VEC2D* vec, INT32 AssistIdx)
{
	// 어시스터가 AI전에서 유저이거나 멀티에서 어시스트한 유저가 배틀에 없을 경우 출력 안함
	if (((MyRoomInfoContext::i()->getStage()->IsAiMode() && BattleSlotContext::i()->IsMyTeam(AssistIdx)) || !MyRoomInfoContext::i()->getStage()->IsAiMode())
		&& BattleSlotContext::i()->getSlotInfo(AssistIdx)->_State != SLOT_STATE_BATTLE)
		return;

	InGameMessage* message = GetEmptyInGameMessage();

	if (message == nullptr)
		return;

	i3Viewer *	pViewer = g_pFramework->GetViewer();
	INT32		nScreenWidth = pViewer->GetViewWidth();
	INT32		nAssistTextWidth = 0;
	INT32		nBGImageIdx = -1;

	i3::wstring wstrTemp;

	message->Reset();

	// Assist 이음선 출력
	message->fScale = 3.0f;
	m_nInGameMessageCount++;

	INT32 nSpriteIdx = GetEmptySpriteIndex(m_pSpriteAssistLine);

	if (!CheckIndex(nSpriteIdx))
		return;

	INT32 nAssistIdx = GetEmptyTextIndex(m_pLeftText);

	if (!CheckIndex(nAssistIdx))
		return;

	m_pSpriteAssistLine->SetEnable(nSpriteIdx, true);
	m_pSpriteAssistLine->SetRect(nSpriteIdx, i3Vector::GetX(vec), (REAL32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT) - (10.0f * i3GuiRoot::getHeightRatioBy768()),
		(20.0f * message->fScale * i3GuiRoot::getHeightRatioBy768()),
		(18.0f * message->fScale * i3GuiRoot::getHeightRatioBy768()));


	if (BattleSlotContext::i()->IsRedTeam(AssistIdx))
		m_pLeftText[nAssistIdx]->SetColor(252, 88, 88, 255);
	else
		m_pLeftText[nAssistIdx]->SetColor(67, 182, 243, 255);

	wstrTemp = BattleSlotContext::i()->getNickForSlot(AssistIdx);

	m_pLeftText[nAssistIdx]->SetEnable(true);
	m_pLeftText[nAssistIdx]->SetText(wstrTemp);
	nAssistTextWidth = m_pLeftText[nAssistIdx]->getTextWidth();
	m_pLeftText[nAssistIdx]->setSize(nAssistTextWidth, (UINT32)NOTICE_TEXT_HEIGHT);
	m_pLeftText[nAssistIdx]->SetTextEllipsis(wstrTemp);
	m_pLeftText[nAssistIdx]->setPos((UINT32)(i3Vector::GetX(vec) + (20.0f * i3GuiRoot::getHeightRatioBy768()) + 2.f),
		(UINT32)(m_nInGameMessageCount * NOTICE_LINE_HEIGHT));

	// 하이라이트 효과(자신이 어시스트 달성 시)
	if (BattleSlotContext::i()->getMySlotIdx() == AssistIdx)
	{

		nBGImageIdx = GetEmptySpriteIndex(m_pSpriteKillTextBG);

		if (CheckIndex(nBGImageIdx))
		{
			VEC2D		vecPos;
			m_pLeftText[nAssistIdx]->getPos(&vecPos);
			m_pSpriteKillTextBG->SetEnable(nBGImageIdx, true);
			m_pSpriteKillTextBG->SetRect(nBGImageIdx, nScreenWidth - (319.0f*i3GuiRoot::getHeightRatioBy768()),
				i3Vector::GetY(&vecPos) - (8.0f*i3GuiRoot::getHeightRatioBy768()),
				319.0f*i3GuiRoot::getHeightRatioBy768(),
				(28.0f*i3GuiRoot::getHeightRatioBy768()));
		}
	}

	message->mType = INGAME_MESSAGE_ASSIST;
	message->nBGImageIdx = nBGImageIdx;
	message->nSubImageIdx = nSpriteIdx;
	message->nIdx = m_nInGameMessageCount;
	message->strLeft = m_pLeftText[nAssistIdx];
}

// 비어있는 InGameMessage 찾기
InGameMessage* CHud::GetEmptyInGameMessage()
{
	typedef i3::list<InGameMessage*>::iterator   iterator;

	// 타이머에 의한 메세지 삭제
	for (iterator it = m_InGameMessageList.begin(), it_end = m_InGameMessageList.end(); it != it_end; it++)
	{
		InGameMessage* obj = *it;

		if (obj->mType == INGAME_MESSAGE_NONE)
			return obj;
	}

	return nullptr;
}

// 비어있는 Text Index 반환
INT32 CHud::GetEmptyTextIndex(i3TextNodeDX2* text[])
{
	for (INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		if (!text[i]->GetEnable())
			return i;
	}

	return -1;
}

// 비어있는 Sprite Index 반환
INT32 CHud::GetEmptySpriteIndex(i3Sprite2D* sprite)
{
	for (INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		if (!sprite->GetEnable(i))
			return i;
	}

	return -1;
}

INT32 CHud::GetEmptySpriteIndex(i3Sprite2D* sprite[])
{
	for (INT32 i = 0; i < HUD_NOTICE_COUNT; i++)
	{
		if (!sprite[i]->GetEnable(0))
			return i;
	}

	return -1;
}

void CHud::ResetInGameMessage(InGameMessage* message)
{
	typedef i3::list<InGameMessage*>::iterator   iterator;

	// InGameMessage를 하나 리셋할 때 전체적으로 Index를 하나씩 줄여준다
	for (iterator it = m_InGameMessageList.begin(), it_end = m_InGameMessageList.end(); it != it_end; it++)
	{
		InGameMessage* obj = *it;

		if (obj->mType == INGAME_MESSAGE_NONE || obj->nIdx <= 0)
			continue;

		obj->nIdx--;
	}

	message->fLifeTime = 0.f;
	message->fScale = 1.0f;
	message->nIdx = 0;

	if (message->nBGImageIdx != -1)
	{
		m_pSpriteKillTextBG->SetEnable(message->nBGImageIdx, false);
		message->nBGImageIdx = -1;
	}

	if (message->nSubImageIdx != -1)
	{
		switch (message->mType)
		{
		case INGAME_MESSAGE_DEFENCT_HEAD:
			m_pSpriteObjDefenctAndHead->SetEnable(message->nSubImageIdx, false);
			break;
		case INGAME_MESSAGE_OBJECT_DEFENCE:
			m_pSpriteObjectDefence->SetEnable(message->nSubImageIdx, false);
			break;
		case INGAME_MESSAGE_KILL_HEADSHOT:
			m_pSpriteHeadShot->SetEnable(message->nSubImageIdx, false);
			break;
		case INGAME_MESSAGE_ASSIST:
			m_pSpriteAssistLine->SetEnable(message->nSubImageIdx, false);
			break;
		default:
			break;
		}
		message->nSubImageIdx = -1;
	}

	message->mType = INGAME_MESSAGE_NONE;

	if (message->pSprite != nullptr)
	{
		message->pSprite->SetEnable(false);
		message->pSprite = nullptr;
	}

	InitText(message->strNotice);
	InitText(message->strLeft);
	InitText(message->strRight);
}

void CHud::InitText(i3TextNodeDX2* text)
{
	if (text == nullptr)
		return;

	text->SetColor(255, 255, 255, 255);
	text->SetText(L"");
	text->SetEnable(false);
	text = nullptr;
}

void CHud::UpdateInGameMessageList(REAL32 fTime)
{
	typedef i3::list<InGameMessage*>::iterator   iterator;

	INT32 deletecount = 0;

	// 타이머에 의한 메세지 삭제
	for (iterator it = m_InGameMessageList.begin(), it_end = m_InGameMessageList.end(); it != it_end; it++)
	{
		InGameMessage* obj = *it;

		if (obj->mType == INGAME_MESSAGE_NONE)
			continue;

		obj->fLifeTime += fTime;
		// 소멸시간이 되면 메세지를 리셋 해준다
		if (obj->fLifeTime > HUD_NOTICE_TIME)
		{
			m_nInGameMessageCount--;
			ResetInGameMessage(obj);
			deletecount++;
		}

		if (obj->fScale > 1.0f)
		{
			obj->fScale -= (fTime * 8.0f);
			if (obj->fScale < 1.0f) obj->fScale = 1.0f;

			VEC3D vecPos;

			switch (obj->mType)
			{
			case INGAME_MESSAGE_KILL_HEADSHOT:
				m_pSpriteHeadShot->GetLeftTop(obj->nSubImageIdx, &vecPos);
				m_pSpriteHeadShot->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vecPos), i3Vector::GetY(&vecPos), (55.0f)*obj->fScale * i3GuiRoot::getHeightRatioBy768(),
					(36.0f)*obj->fScale * i3GuiRoot::getHeightRatioBy768());
				break;

			case INGAME_MESSAGE_OBJECT_DEFENCE:
				m_pSpriteObjectDefence->GetLeftTop(obj->nSubImageIdx, &vecPos);
				m_pSpriteObjectDefence->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vecPos), i3Vector::GetY(&vecPos), (28.0f)*obj->fScale * i3GuiRoot::getHeightRatioBy768(),
					(30.0f)*obj->fScale * i3GuiRoot::getHeightRatioBy768());
				break;

			case INGAME_MESSAGE_DEFENCT_HEAD:
				m_pSpriteObjDefenctAndHead->GetLeftTop(obj->nSubImageIdx, &vecPos);
				m_pSpriteObjDefenctAndHead->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vecPos), i3Vector::GetY(&vecPos), (44.f)*obj->fScale * i3GuiRoot::getHeightRatioBy768(),
					(38.f)*obj->fScale * i3GuiRoot::getHeightRatioBy768());
				break;

			case INGAME_MESSAGE_ASSIST:
				m_pSpriteAssistLine->GetLeftTop(obj->nSubImageIdx, &vecPos);
				m_pSpriteAssistLine->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vecPos), i3Vector::GetY(&vecPos), (20.f)*obj->fScale * i3GuiRoot::getHeightRatioBy768(),
					(18.f)*obj->fScale * i3GuiRoot::getHeightRatioBy768());
				break;

			default:
				I3TRACE("Scale 값을 사용하면 안되는 루틴입니다. %d\n", (INT32)obj->mType);
				break;
			}
		}
	}

	if (deletecount > 0)
		RealignInGameMessage();
}

// InGameMessage를 재정렬
void CHud::RealignInGameMessage()
{
	typedef i3::list<InGameMessage*>::iterator   iterator;

	VEC2D vecPos;

	// 좌표 수정(타입별로)
	for (iterator it = m_InGameMessageList.begin(), it_end = m_InGameMessageList.end(); it != it_end; it++)
	{
		InGameMessage* obj = *it;

		if (obj->mType == INGAME_MESSAGE_NONE || obj->fLifeTime > HUD_NOTICE_TIME)
			continue;

		if (obj->strNotice != nullptr)
			RealignText(obj->strNotice, obj->nIdx);

		if (obj->strLeft != nullptr)
			RealignText(obj->strLeft, obj->nIdx);

		if (obj->strRight != nullptr)
			RealignText(obj->strRight, obj->nIdx);

		// 킬마크 이미지
		if (obj->pSprite != nullptr)
		{
			VEC3D vec;
			VEC2D* pVecSize;

			obj->pSprite->GetLeftTop(0, &vec);
			pVecSize = obj->pSprite->GetSize(0);
			obj->pSprite->SetRect(0, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (11.0f * i3GuiRoot::getHeightRatioBy768()),
				i3Vector::GetX(pVecSize), i3Vector::GetY(pVecSize));
		}

		// 하이라이트 이미지
		if (obj->nBGImageIdx != -1)
		{
			VEC3D vec;
			m_pSpriteKillTextBG->GetLeftTop(obj->nBGImageIdx, &vec);
			m_pSpriteKillTextBG->SetRect(obj->nBGImageIdx, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (8.0f * i3GuiRoot::getHeightRatioBy768()),
				319.0f*i3GuiRoot::getHeightRatioBy768(), 28.0f*i3GuiRoot::getHeightRatioBy768());
		}

		// 기타 이미지(헤드샷, 방어모드 등)
		if (obj->nSubImageIdx != -1)
		{
			VEC3D vec;
			switch (obj->mType)
			{
			case INGAME_MESSAGE_KILL_HEADSHOT:
				m_pSpriteHeadShot->GetLeftTop(obj->nSubImageIdx, &vec);
				m_pSpriteHeadShot->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (10.0f * i3GuiRoot::getHeightRatioBy768()),
					55.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
				break;
			case INGAME_MESSAGE_DEFENCT_HEAD:
				m_pSpriteObjDefenctAndHead->GetLeftTop(obj->nSubImageIdx, &vec);
				m_pSpriteObjDefenctAndHead->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (11.0f * i3GuiRoot::getHeightRatioBy768()),
					44.f * i3GuiRoot::getHeightRatioBy768(), 38.f * i3GuiRoot::getHeightRatioBy768());
				break;
			case INGAME_MESSAGE_OBJECT_DEFENCE:
				m_pSpriteObjectDefence->GetLeftTop(obj->nSubImageIdx, &vec);
				m_pSpriteObjectDefence->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (8.0f * i3GuiRoot::getHeightRatioBy768()),
					28.0f*i3GuiRoot::getHeightRatioBy768(), 30.0f*i3GuiRoot::getHeightRatioBy768());
				break;
			case INGAME_MESSAGE_ASSIST:
				m_pSpriteAssistLine->GetLeftTop(obj->nSubImageIdx, &vec);
				m_pSpriteAssistLine->SetRect(obj->nSubImageIdx, i3Vector::GetX(&vec), (UINT32)(obj->nIdx * NOTICE_LINE_HEIGHT) - (10.0f * i3GuiRoot::getHeightRatioBy768()),
					(20.0f * i3GuiRoot::getHeightRatioBy768()), (18.0f * i3GuiRoot::getHeightRatioBy768()));
				break;
			default:
				I3TRACE("InGameMessage SubImageIdx error. Type : %d, Idx : %d\n", (INT32)obj->mType, obj->nSubImageIdx);
				break;
			}
		}

	}
}

void CHud::RealignText(i3TextNodeDX2* text, INT32 idx)
{
	VEC2D vecPos;

	text->getPos(&vecPos);
	text->setPos((UINT32)(i3Vector::GetX(&vecPos)), (UINT32)(idx * NOTICE_LINE_HEIGHT));
}

i3Sprite2D* CHud::SetKillWeapon(CWeaponInfo* pWeaponInfo, INT32 nIdx, INT32 nSpriteIdx, bool bHP_Up, INT32 nObjIndex, bool bHeadShot)
{
	i3_prof_func();

	// 위치 계산
	VEC2D  vecPos;

	m_pRightText[nIdx]->getPos(&vecPos);

	REAL32 rX = i3Vector::GetX(&vecPos) - (100.0f*i3GuiRoot::getHeightRatioBy768());
	REAL32 rY = i3Vector::GetY(&vecPos) - (11.0f*i3GuiRoot::getHeightRatioBy768());

	WEAPON_CLASS_TYPE iWeaponClass;
	WEAPON::RES_ID iWeaponNum;

	if (pWeaponInfo)
	{// 무기에 의한 사망
		iWeaponClass = pWeaponInfo->GetTypeClass();
		iWeaponNum = pWeaponInfo->GetNumber();
	}
	else
	{// 오브젝트에 의한 사망		이부분 정규화 합니다. 
		iWeaponClass = (WEAPON_CLASS_TYPE)100;
		if (nObjIndex == -1)
		{
			iWeaponNum = 1;
		}
		else
		{
			iWeaponNum = 2;
		}
	}
	g_pFramework->SetWeaponKillMark(m_pSpriteKillMark[nSpriteIdx], iWeaponClass, iWeaponNum, bHP_Up, bHeadShot);
	m_pSpriteKillMark[nSpriteIdx]->SetRect(0, rX, rY, 100.0f*i3GuiRoot::getHeightRatioBy768(), 36.0f*i3GuiRoot::getHeightRatioBy768());
	m_pSpriteKillMark[nSpriteIdx]->SetEnable(true);

	return m_pSpriteKillMark[nSpriteIdx];
}

void CHud::SetDamageDirection(REAL32 fAngle)
{
	i3_prof_func();

	if( m_bOnlyNoticeUpdate) return;

	// 360도 방위
	{
		m_pSpriteHit->SetEnable( HUD_SPRITE_HIT_ANGLE_START + m_idxDamageDirectionOrder, true);
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

	if( UserContext::i()->Death[m_CurrentUserIdx])
	{
		// 케릭터가 죽었으면 모든 피격 효과 취소
		m_pSpriteHit->SetEnable( false);
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

	if( pWpn == nullptr)  return false;
	if( pWpn->getWeaponInfo()->GetTypeClass() != WEAPON_CLASS_THROWING_GRENADE)		return false;

	switch( pWpn->getWeaponInfo()->GetIndicatorType())
	{
	case WEAPON_INDICATOR_GRENADE :
		{
			WeaponGrenade * pWeapon = (WeaponGrenade*)pWpn;

			if( (((MainWeapon_Grenade*)pWeapon->GetMainWeapon())->isThrowing() == true) && 
				(((MainWeapon_Grenade*)pWeapon->GetMainWeapon())->isSettled() == true))
				return true;
		}
		break;
	case WEAPON_INDICATOR_C5 :
		{
			if( i3::same_of<WeaponC5*, WeaponClaymore*, WeaponM14*>(pWpn) )
			{
				MainWeapon_Throw * pMainWeapon = static_cast<MainWeapon_Throw*>(pWpn->GetMainWeapon());
				if( (pMainWeapon->isThrowing() == true) && (pMainWeapon->isSettled() == true))
					return true;
			}
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

	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( m_CurrentUserIdx);

	if( pChara == nullptr)
		return;

	i3Viewer * pViewer = g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	

	REAL32 Radius1 = 100.0f;
	const REAL32 Radius2 = 35.0f;
	MATRIX inv, local;

	i3Matrix::Inverse( &inv, nullptr, pChara->GetMatrix());

	INT32 count = 0;

	for(size_t i = 0; i < List.size(); i++)
	{
		pWeapon = List[i];

		if( pWeapon == nullptr)					continue;
		if( pWeapon->getWeaponInfo() == nullptr)	continue;

		INT32	startIdx;

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
			if( bEnable && m_bEnableObserverMode == false)
			{
				m_pGrenadeInd->SetEnable( idx * 2, true);
				m_pGrenadeInd->SetEnable( idx * 2 + 1, true);

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
						i3::stack_wstring wstrTemp;
						i3::sprintf( wstrTemp, L"%.1fM", fDistance);
						m_pTextGrenade[idx]->SetEnable( true);
						m_pTextGrenade[idx]->SetText( wstrTemp);

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

	if( pPlayer == nullptr)
		return 0;

	pThrowWeapon = g_pWeaponManager->getThrowWeapon( wpn, 0);
	if( pThrowWeapon == nullptr )
		return 0;

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

		// 아군의 수류탄은 경고에서 제외
		CGameCharaBase * pOwner = pThrowWeapon->getOwner();

		if( (pOwner != nullptr) && (pOwner != pPlayer))
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

	
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		return;
#endif

	// 죽어 있으면 처리하지 않는다.
	CGameCharaBase * pPlayer = GetCurrentChara();

	if (pPlayer == nullptr)
	{
		return;
	}

	if( pPlayer->isCharaStateMask( CHARA_STATE_DEATH) )
	{	
		for(INT32 i = 0; i < MAX_INDICATOR; i++)
		{
			m_pTextGrenade[i]->SetEnable(false);
		}

		m_pGrenadeInd->SetEnable( false);
		m_pGrenadeInd->RemoveFlag( I3_NODEFLAG_DISABLE);
		return;
	}

	// 초기화
	for(INT32 i = 0; i < MAX_INDICATOR; i++)
	{
		m_pGrenadeInd->SetEnable( i * 2, false);
		m_pGrenadeInd->SetEnable( i * 2 +1, false);
		m_pTextGrenade[i]->SetEnable( false);
	}

	//수류탄 indicator
	bool bIndicated = false;

	m_IndicateWeaponList.clear();
	UINT32 cnt = WEAPON::getItemCount( WEAPON_CLASS_THROWING_GRENADE);
	for( UINT32 i = 1; i < cnt; i++)
	{
		if (i == WEAPON::getItemIndex(WEAPON::DOMI_MEATBOMB)) continue;

		// 등록되지 않은 무기인 경우, 인디케이터에 추가하지 않습니다.
		if( g_pWeaponManager->IsRegisteredWeapon(WEAPON_CLASS_THROWING_GRENADE, i) == false )
			continue;

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

void CHud::UpdateScore(bool bVisible)
{
	i3_prof_func();

	// 튜토리얼 모드는 결과창을 다른 방식으로 그려 준다.
	if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		return;
	}
	HUD::instance()->SetParameter( HUD::DASH_BOARD, HUD::DBS_TargetShow, !bVisible );
}

void CHud::InitHUDNick()
{
	i3_prof_func();

	INT32 i = 0;

	// 캐릭터 닉네임
	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if (m_pTextName[i] == nullptr)
			continue;

		if( m_pTextName[i]->GetEnable())
		{
			m_pTextName[i]->SetEnable( false);
		}
	}	
}

void CHud::SetCharaNick( const i3::wliteral_range& wNickRng, bool bFriendly, REAL32 fX, REAL32 fY, REAL32 fLength)
{
	i3_prof_func();

	INT32 i = 0;
	COLOR col;

	for(i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if( !m_pTextName[i]->GetEnable())
		{

i3_prof_start("SetCharaNick #1 Block");
			m_pTextName[i]->SetEnable( true);

			// Nick
			m_pTextName[i]->SetTextEllipsis( wNickRng);
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
						m_pTextName[i]->SetEnable( false);
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

void CHud::SetCharaNickForGM( const i3::wliteral_range& wNickRng, INT32 slotIdx, bool IsRed, REAL32 fX, REAL32 fY, REAL32 fLength)
{
	if (BattleSlotContext::i()->IsGMObserverForClient(slotIdx)) return;

	i3_prof_func();

	COLOR col;

	bool teamChanged = MyRoomInfoContext::i()->IsAutoTeamChanged();

	for(INT32 i=0; i<HUD_NAME_MAX_COUNT; i++)
	{
		if(UserInfoContext::i()->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
		{
				if(m_gmObserverCallSignNumSpr)
				{
					// 선수이름을 콜사인넘버로 표시
					m_pTextName[slotIdx]->SetEnable( false);

					i3Viewer* pViewer = g_pFramework->GetViewer();
					//REAL32 fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextName[i]->getTextWidth())*0.5f);
					REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY;

					for (INT32 j=(INT32)!IsRed; j<slotIdx ; j+=2)
					{
						bool is_gm_rank = BattleSlotContext::i()->getSlotInfo(j)->_Rank == RANK_GAMEMASTER ? true : false;
						bool is_observer = BattleSlotContext::i()->IsGMObserverForClient(j) ? true : false;

						if (is_gm_rank && is_observer)
							slotIdx-=2;
					}

					INT32 callSlotIdx = slotIdx;

					if(teamChanged)
						callSlotIdx = ConvertSlotIndexToOppositeTeam(callSlotIdx);

					if ( callSlotIdx >= 0 )
					{
						m_gmObserverCallSignNumSpr->SetRect(callSlotIdx, (REAL32)pViewer->GetViewWidth() * fX - 12, fPosY - 10, 24.f, 24.0f);
						m_gmObserverCallSignNumSpr->SetEnable(callSlotIdx, true);
					}
				}

			break;
		}
		else
		{
			// 선수이름으표시
			if(!m_pTextName[i]->GetEnable())
			{
				m_pTextName[i]->SetEnable( true);				
				m_pTextName[i]->SetTextEllipsis( wNickRng ); // Nick
				
				{ // 색상
					if(IsRed) 
						i3Color::Set( &col, (UINT8) 255, 0, 0, 255); // 아군
					else 
						i3Color::Set( &col, (UINT8) 0, 128, 255, 255); // 적군

					m_pTextName[i]->SetColor( &col);
				}

				// 좌표
				i3Viewer* pViewer = g_pFramework->GetViewer();
				REAL32 fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextName[i]->getTextWidth())*0.5f);
				//REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY - (fLength);
				REAL32 fPosY = (REAL32)pViewer->GetViewHeight() * fY;
				m_pTextName[i]->setPos( (INT32)fPosX, (INT32)fPosY);	
				//m_pTextName[i]->setSize( m_pTextName[i]->getTextWidth(), m_pTextName[i]->getTextHeight());

				m_pTextName[i]->setSize( 800, 40);

				break; // exit loop
			}
		}

	} // end of for

}

INT32 CHud::SetObjectReply( INT32 nType, REAL32 fX, REAL32 fY)
{
	i3_prof_func();

	INT32 i = 0;
//	COLOR col;
	INT32 nRndIdx;
//	char szTemp[256];
	i3::rc_wstring wstrTemp;

	REAL32 fPosX = 0.0f;
	REAL32 fPosY = 0.0f;

	for(i=0; i<HUD_OBJECT_REPLY_COUNT; i++)
	{
		if( !m_pTextObjectReply[i]->GetEnable())
		{
			m_pTextObjectReply[i]->SetEnable( true);
			m_fDeltaTimeObjectReply[i] = 0.0f;

			switch( nType)
			{
			case 0:
				nRndIdx = i3Math::Rand()%5;
				
				switch( nRndIdx)
				{
				case 0:	
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD5");/*이제 곧 포인트 블랭크가 FPS를 장악 하리라!\n포블을 외치는 자*/
					break;
				case 1:	
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD7");/*모두들 포블하라! 곧 이벤트 여신이 강림하리니!\n포블을 외치는 자*/					
					break;
				case 2:	
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD2");/*포블의 예언에 귀를 기울여라! 곧 포블의 시대가 시작될지니!\n포블을 외치는 자*/					
					break;
				case 3:	
					{
						USER_INFO_BASIC my_info;
						UserInfoContext::i()->GetMyUserInfoBasic(&my_info);
						
						i3::wliteral_range wstrNick = my_info.m_strNick;

						i3::sprintf( wstrTemp, GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD8"), wstrNick);/*[%s] 그대에게 포블의 권능을 내리노라!\n포블을 외치는 자*/					
					}
					break;
				case 4:	
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD6");/*무기를 정비 하라! 그대에게 Extension 의 축복을 내리노라!\n포블을 외치는 자*/					
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
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD4");/*포블의 계시에 귀를 기울여라! 잠들었던 FPS가 눈을 떴노라!\n포블을 설파하는 자*/					
					break;
				case 1:			
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD1");/*포인트 블랭크의 날이 가까워 졌다!\n포블을 설파하는 자*/		
					break;
				case 2:
					wstrTemp = GAME_RCSTRING("STR_TBL_INGAME_HUD_NPC_WORD3");/*포블의 계시에 귀를 기울여라! 타격 전율의 시대가 시작되었다!\n포블을 설파하는 자*/		
					break;
				default:
					break;
				}
				break;
			default:	
				I3ASSERT_0;
				break;
			}

			// 텍스트 입력
			{
				m_pTextObjectReply[i]->SetText(wstrTemp);

				// 좌표
				i3Viewer * pViewer	= g_pFramework->GetViewer();
				fPosX = (REAL32)pViewer->GetViewWidth() * fX - (((REAL32)m_pTextObjectReply[i]->getTextWidth())*0.5f);
				fPosY = (REAL32)pViewer->GetViewHeight() * fY;
				m_pTextObjectReply[i]->setPos( (INT32)fPosX, (INT32)fPosY);				
				m_pTextObjectReply[i]->setSize( m_pTextObjectReply[i]->getTextWidth(), 27);
			}
			
			// 텍스트 배경
			{
				m_pSpriteObjectReply->SetEnable( i, true);
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

	m_pTextObjectReply[idx]->SetText(L"");
	m_pTextObjectReply[idx]->SetEnable( false);
	m_pSpriteObjectReply->SetEnable( idx, false);
}

void CHud::ShowHUDNick( void )
{	
	i3_prof_func();

	INT32 i;
	CGameCharaBase * pChara = nullptr;
	INT32	MyTeam = BattleSlotContext::i()->getMyTeam();

	InitHUDNick();

	//캐릭터 닉네임
	{
		// 옵저버 모드일경우에는 Chara Nick을 보여주지 않습니다.
		if( (m_bEnableObserverMode || m_bEnableObserverFlyMode) &&
#if defined( DEF_OBSERVER_MODE)
			BattleSlotContext::i()->isObserverMe() == false)
#else
			UserInfoContext::i()->IsGM_Observer() == false)
#endif
			return;

		for( i = 0; i < g_pCharaManager->getCharaCount(); i++ )
		{
			pChara = g_pCharaManager->getChara( i );
			if( pChara != nullptr )
			{
				if( pChara->getCharaNetIndex() == -1)
					continue;

				if( pChara->isAIToClient() == false && UserContext::i()->IsPlayingSlot( pChara->getCharaNetIndex()) == false)
					continue;

				if( pChara->isCharaStateMask( CHARA_STATE_FIRSTRESPAWN) == false)	// 리스폰하지 않은 캐릭터는 스킵
					continue;

				CGameCharaMaterialContext * pMaterial = pChara->getMaterialContext();
				
				if (pMaterial == nullptr)
					continue;

				INT32 slot = pChara->getCharaInfo()->GetNetIdx();
				
				if( !(pChara->getCharaInfo()->GetAIToClient()) && HackContext::i()->isHackUserCaution( slot))
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
						if( i3::same_of<WeaponM197*>(pWeapon))
							((WeaponM197*)pWeapon)->CheckCrossHit();
					}
					else
					{
						if (pChara->getCollisionContext())
							pChara->getCollisionContext()->UpdateNickOnTarget();
						else
							TRACE("CHud::ShowHUDNick pChara->getCollisionContext()=nullptr\n");
					}
					continue;
				}

				const GLOBAL_CHARA_INFO * pCharaInfo = pChara->getCharaInfo();

#if defined( DEF_OBSERVER_MODE)
				if( BattleSlotContext::i()->isObserverMe())
#else
				if( UserInfoContext::i()->IsGM_Observer())
#endif
				{
					if(UserInfoContext::i()->IsViewHudNick()) 
					{
						pChara->Cmd_SetCharaNick( true);
					}
				}
				else if( MyTeam == (INT32) pCharaInfo->GetTeam() )
				{
					if( LocaleValue::Enable( "ForciblyRemove" ) )
					{
						UIBattlePopupBase * p = GameUI::GetBattlePopup( UBP_ACCUSE_CRIME);
						if(p != nullptr && p->isEnable() == false)
							pChara->Cmd_SetCharaNick( true);
					}
					else
					{
						pChara->Cmd_SetCharaNick( true);
					}
				}

			}
		}
	}
	
}

void CHud::SetGameEnd(void)
{
	i3_prof_func();

	m_bGameEnd = true;

	if( m_pScope != nullptr)
		m_pScope->SetEnableSinperScope( false);
	SetEnableObserver(false);
	SetEnableMiniMap(false);
	SetMoveChatting();

	if( m_pPanelTargetHP != nullptr)
		m_pPanelTargetHP->SetEnableTargetHP( false);
}

void CHud::SetEnableObserverFly(bool bEnable)
{
	i3_prof_func();

	m_bEnableObserverFlyMode = bEnable;

	for(INT32 i = 0; i < HUD_TEXT_OBSERVER_FLY_COUNT; i++)
	{
		if (m_pTextObserverFly[i])
			m_pTextObserverFly[i]->SetEnable(bEnable);
	}	
}

void CHud::SetEnableObserver(bool bEnable)
{
	i3_prof_func();

	if( m_bEnableObserverMode == bEnable ) return;

	m_bEnableObserverMode = bEnable;

	m_pBattleFrame->EnableHUD(UIHUD_OBSERVER, bEnable == 1);
	observer_hud::Context::i()->Notify(observer_hud::EVT_ENABLE, bEnable);

    m_pSpriteObserver->SetEnable(m_bEnableObserverMode);

	m_pObserverHPGaugeHUD->SetEnableGauge( bEnable);

	if (UserInfoContext::i()->IsGM_Observer())
	{ // 옵저버 모드에서는 미션카드 HUD를 끕니다.
		UIHudMissionBoard* MissionBoard = (UIHudMissionBoard*)HUD::instance()->GetHud(HUD::MISSION_BOARD);
		if (MissionBoard) MissionBoard->CloseMissionBoard();
	}
	
	for(INT32 i = 0; i < HUD_TEXT_OBSERVER_COUNT; i++)
	{
		m_pTextObserver[i]->SetEnable(m_bEnableObserverMode);
	}
}

void CHud::SetObservedNick(const i3::rc_wstring& wstrNick, bool bIsRed)
{
	i3_prof_func();

	m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetTextEllipsis(wstrNick);
	
	if(g_pViewer)
	{
		INT32 nScreenHeight = g_pViewer->GetViewHeight();		
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->setPos(m_pTextObserver[HUD_TEXT_OBSERVER_ID]->getPosX(), 
			(UINT32)(nScreenHeight-68));
	}

	if( bIsRed)
	{
		m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(255, 0, 0, 255);
	}
	else
	{
		if(UserInfoContext::i()->IsGM_Observer())
		{
			m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(0, 128, 255, 255);
		}
		else
		{
			m_pTextObserver[HUD_TEXT_OBSERVER_ID]->SetColor(100, 100, 255, 255);
		}
		
	}
}

void CHud::AddChainKills(INT32 nKillMessage,INT32 nKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg)
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

	g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_REMOVE_HELMET"));/*{col:255,120,0,255}헬멧에 의해 헤드샷으로부터 보호되었습니다. 다음 리스폰 까지는 더 이상 헬멧으로부터 보호받을 수 없습니다.{/col}*/
	g_pFramework->getChatBox()->ResetBackupBuffer();
	m_pBattleFrame->UpdateChatBuffer();
}

void CHud::AddTargetBonus( INT32 exp)
{
	i3_prof_func();

	i3::rc_wstring wstrExp;

	m_pChainKills->AddTargetBonus();

	i3::sprintf( wstrExp, GAME_RCSTRING("STR_TBL_INGAME_HUD_GET_TARGET_EXP"), exp);

	g_pFramework->getChatBox()->PutSystemChatting( wstrExp);/*{col:255,120,0,255}미션 목표물에 Damage를 주어 경험치 %d획득{/col}*/
	g_pFramework->getChatBox()->ResetBackupBuffer();
	m_pBattleFrame->UpdateChatBuffer();
}

void CHud::AddWeapon_BuildUp()
{
	i3_prof_func();

	g_pFramework->getChatBox()->PutSystemChatting(L"모든 무기 공격력 30% 증가");
	g_pFramework->getChatBox()->ResetBackupBuffer();
	m_pBattleFrame->UpdateChatBuffer();
}


void CHud::AddHP_Recovery()
{
	i3_prof_func();

	m_pChainKills->AddLargeMessage(CK_HP_RECOVERY);

	g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STR_TBL_INGAME_HUD_RAPTOR_HP_RECOVERY"));
	g_pFramework->getChatBox()->ResetBackupBuffer();
	m_pBattleFrame->UpdateChatBuffer();
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

	g_pFramework->getChatBox()->PutSystemChatting( GAME_RCSTRING("STR_TBL_INGAME_HUD_DEATH_BLOW"));
	g_pFramework->getChatBox()->ResetBackupBuffer();
	m_pBattleFrame->UpdateChatBuffer();
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

bool CHud::GetEnableChatScroll(void)
{
	i3_prof_func();
	return ((UIHUDIngameChat *)m_pBattleFrame->GetHUD(UIHUD_INGAMECHATING))->GetEnableChatScroll();
}

bool CHud::IsEnableGui(void)
{
	i3_prof_func();

	if (m_pParent)
	{
		return ((CStageBattle*)m_pParent)->IsEnableGui();
	}

	return false;
}

void CHud::SetDamageFilterInGame( i3Texture* pTex )
{
	i3_prof_func();

	I3_SAFE_RELEASE( m_pDamageFilterTexture );

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pDamageFilterTexture = pTex;
	I3_MUST_ADDREF(pTex);

	m_pDamageFilterSprite->SetTexture(m_pDamageFilterTexture);
	m_pDamageFilterSprite->SetEnable(false);

	m_pDamageFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDamageFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
	m_pDamageFilterSprite->SetColor(0, 255, 255, 255, 255);

	m_pDamageFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDamageFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
	m_pDamageFilterSprite->SetColor(1, 255, 255, 255, 255);
}

void CHud::ClearDamageFilter()
{
	i3_prof_func();
	
	m_pDamageFilterSprite->SetEnable(false);
	
	for(size_t i = 0; i < m_vSpecialDamageFilterSprite.size(); i++)
	{
		if( !i3::vu::is_value_valid(m_vSpecialDamageFilterSprite, i))
			continue;
		
		((i3Sprite2D*)m_vSpecialDamageFilterSprite[i])->SetEnable(false);
	}
}

void CHud::OnDamageFilter(INT32 index /*= -1*/)
{
	i3_prof_func();
	if( index == -1 )
	{
		m_rDamageFilterTime = 0.0f;
		m_pDamageFilterSprite->SetEnable(true);
	}
	else
	{
		if( i3::vu::is_value_valid(m_vSpecialDamageFilterSprite, index) )
		{
			m_vSpecialDamageFilterTime[index] = 0.0f;
			((i3Sprite2D*)m_vSpecialDamageFilterSprite[index])->SetEnable(true);
		}
	}
}

void CHud::OnUpdateDamageFilter(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if (m_pDamageFilterSprite != nullptr &&
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
			m_pDamageFilterSprite->SetEnable(false);
		}
	}

	OnUpdateSpecialDamageFilter(rDeltaSeconds);
}

void CHud::InsertSpecialDamageFilter( INT32 index )
{
	i3_prof_func();

	if( !i3::vu::is_value_valid(m_vSpecialDamageFilterTexture, index) )
	{		
		// 텍스쳐
		i3::rc_wstring texture_path;
		g_pWeaponInfoDataBase->GetSpecialDamageHUDTexturePath(index, texture_path);
		
		i3::string conv;
		i3::utf16_to_mb(texture_path, conv);
		i3Texture * pTex = g_pFramework->GetResourceManager()->LoadTexture(conv.c_str());
		if (pTex == nullptr)
		{
			I3PRINTLOG(I3LOG_NOTICE, "무기 피격 허드 리소스 누락 : %s", conv.c_str() );
			return;
		}

		i3::vu::set_value_force( m_vSpecialDamageFilterTexture, index, pTex );
		
		// 스프라이트( 텍스쳐가 없었으면 안만들어져야함 그래서 따로 유효성 검사는 하지 않습니다. )
		i3Viewer * pViewer		= g_pFramework->GetViewer();
		INT32	nScreenWidth	= pViewer->GetViewWidth();
		INT32	nScreenHeight	= pViewer->GetViewHeight();
		
		i3Sprite2D* temp_sprite = i3Sprite2D::new_object();
		temp_sprite->Create(2, true, true);
		temp_sprite->SetTexture((i3Texture*)m_vSpecialDamageFilterTexture[index]);
		temp_sprite->SetEnable(false);

		temp_sprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		temp_sprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
		temp_sprite->SetColor(0, 255, 255, 255, 255);

		temp_sprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		temp_sprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
		temp_sprite->SetColor(1, 255, 255, 255, 255);
		
		m_pHUDRoot->AddChild(temp_sprite);

		i3::vu::set_value_force( m_vSpecialDamageFilterSprite, index, temp_sprite );

		//타임 ( 텍스쳐가 없었으면 안만들어져야함 그래서 따로 유효성 검사는 하지 않습니다. )
		REAL32 temp_time = 0.0f;
		i3::vu::set_value_force( m_vSpecialDamageFilterTime, index, temp_time);
	}
}

void CHud::OnUpdateSpecialDamageFilter( REAL32 rDeltaSeconds )
{
	i3_prof_func();

	for(size_t i = 0; i < m_vSpecialDamageFilterSprite.size(); i++)
	{
		if( !i3::vu::is_value_valid(m_vSpecialDamageFilterSprite, i))
			continue;

		if (m_vSpecialDamageFilterSprite[i] != nullptr &&
			((i3Sprite2D*)m_vSpecialDamageFilterSprite[i])->GetEnable(0))
		{		
			m_vSpecialDamageFilterTime[i] += rDeltaSeconds;

			// cos()로 알파값을 빼면서 2초간 보여준다.
			INT32 alpha = INT32(255.0f * i3Math::cos(m_vSpecialDamageFilterTime[i] / 2.0f));
			alpha = MINMAX(0, alpha, 255);

			if (0 < alpha)
			{
				((i3Sprite2D*)m_vSpecialDamageFilterSprite[i])->SetColor(0, 255, 255, 255, (UINT8) (alpha));
				((i3Sprite2D*)m_vSpecialDamageFilterSprite[i])->SetColor(1, 255, 255, 255, (UINT8) (alpha));
			}
			else
			{
				((i3Sprite2D*)m_vSpecialDamageFilterSprite[i])->SetEnable(false);
			}
		}
	}
}

void CHud::SetDieFilterInGame(i3Texture * pTex)
{
	i3_prof_func();

	I3_SAFE_RELEASE(m_pDieFilterTexture);

	i3Viewer * pViewer = g_pFramework->GetViewer();
	INT32	nScreenWidth = pViewer->GetViewWidth();
	INT32	nScreenHeight = pViewer->GetViewHeight();

	m_pDieFilterTexture = pTex;
	I3_MUST_ADDREF(pTex);

	m_pDieFilterSprite->SetTexture(m_pDieFilterTexture);
	m_pDieFilterSprite->SetEnable(false);

	m_pDieFilterSprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDieFilterSprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
	m_pDieFilterSprite->SetColor(0, 255, 255, 255, 255);

	m_pDieFilterSprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
	m_pDieFilterSprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
	m_pDieFilterSprite->SetColor(1, 255, 255, 255, 255);
}

void CHud::ClearDieFilter(void)
{
	m_pDieFilterSprite->SetEnable(false);

	for (size_t i = 0; i < m_vSpecialDieFilterSprite.size(); i++)
	{
		if (!i3::vu::is_value_valid(m_vSpecialDieFilterSprite, i))
			continue;

		((i3Sprite2D*)m_vSpecialDieFilterSprite[i])->SetEnable(false);
	}
}

void CHud::OnDieFilter(INT32 index)
{
	if (index != -1)
	{
		if (i3::vu::is_value_valid(m_vSpecialDieFilterSprite, index))
		{
			m_vSpecialDieFilterTime[index] = 0.0f;
			((i3Sprite2D*)m_vSpecialDieFilterSprite[index])->SetEnable(true);
		}
	}
}

void CHud::OnUpdateDieFilter(REAL32 rDeltaSeconds)
{
	if (m_pDieFilterSprite->GetEnable(0))
	{
		m_rDieFilterTime += rDeltaSeconds;

		// cos()로 알파값을 빼면서 2초간 보여준다.
		INT32 alpha = INT32(255.0f * i3Math::cos(m_rDieFilterTime / 2.0f));
		alpha = MINMAX(0, alpha, 255);

		if (0 < alpha)
		{
			m_pDieFilterSprite->SetColor(0, 255, 255, 255, (UINT8)(alpha));
			m_pDieFilterSprite->SetColor(1, 255, 255, 255, (UINT8)(alpha));
		}
		else
		{
			m_pDieFilterSprite->SetEnable(false);
		}
	}

	OnUpdateSpecialDieFilter(rDeltaSeconds);
}

void CHud::InsertSpecialDieFilter(INT32 index)
{
	i3_prof_func();

	if (!i3::vu::is_value_valid(m_vSpecialDieFilterTexture, index))
	{
		// 텍스쳐
		i3::rc_wstring texture_path;
		g_pWeaponInfoDataBase->GetSpecialDieHUDTexturePath(index, texture_path);

		i3::string conv;
		i3::utf16_to_mb(texture_path, conv);
		i3Texture * pTex = g_pFramework->GetResourceManager()->LoadTexture(conv.c_str());
		if (pTex == nullptr)
		{
			I3PRINTLOG(I3LOG_NOTICE, "무기 다이 허드 리소스 누락 : %s", conv.c_str());
			return;
		}

		i3::vu::set_value_force(m_vSpecialDieFilterTexture, index, pTex);

		// 스프라이트( 텍스쳐가 없었으면 안만들어져야함 그래서 따로 유효성 검사는 하지 않습니다. )
		i3Viewer * pViewer = g_pFramework->GetViewer();
		INT32	nScreenWidth = pViewer->GetViewWidth();
		INT32	nScreenHeight = pViewer->GetViewHeight();

		i3Sprite2D* temp_sprite = i3Sprite2D::new_object();
		temp_sprite->Create(2, true, true);
		temp_sprite->SetTexture((i3Texture*)m_vSpecialDieFilterTexture[index]);
		temp_sprite->SetEnable(false);

		temp_sprite->SetRect(0, 0.0f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		temp_sprite->SetTextureCoord(0, 0.0f, 0.0f, 511.0f, 767.0f);
		temp_sprite->SetColor(0, 255, 255, 255, 255);

		temp_sprite->SetRect(1, (REAL32)nScreenWidth*0.5f, 0.0f, (REAL32)nScreenWidth*0.5f, (REAL32)nScreenHeight);
		temp_sprite->SetTextureCoord(1, 511.0f, 767.0f, 0.0f, 0.0f);
		temp_sprite->SetColor(1, 255, 255, 255, 255);

		m_pHUDRoot->AddChild(temp_sprite);

		i3::vu::set_value_force(m_vSpecialDieFilterSprite, index, temp_sprite);

		//타임 ( 텍스쳐가 없었으면 안만들어져야함 그래서 따로 유효성 검사는 하지 않습니다. )
		REAL32 temp_time = 0.0f;
		i3::vu::set_value_force(m_vSpecialDieFilterTime, index, temp_time);
	}
}

void CHud::OnUpdateSpecialDieFilter(REAL32 rDeltaSeconds)
{
	for (size_t i = 0; i < m_vSpecialDieFilterSprite.size(); i++)
	{
		if (!i3::vu::is_value_valid(m_vSpecialDieFilterSprite, i))
			continue;

		if (m_vSpecialDieFilterSprite[i] != nullptr &&
			((i3Sprite2D*)m_vSpecialDieFilterSprite[i])->GetEnable(0))
		{
			m_vSpecialDieFilterTime[i] += rDeltaSeconds;

			// cos()로 알파값을 빼면서 2초간 보여준다.
			//INT32 alpha = INT32(255.0f * i3Math::cos(m_vSpecialDieFilterTime[i] / 2.0f));
			REAL32 runingtime = 2.f;
			if (m_vSpecialDieFilterTime[i] <= runingtime)
			{
				((i3Sprite2D*)m_vSpecialDieFilterSprite[i])->SetColor(0, 255, 255, 255, 255);
				((i3Sprite2D*)m_vSpecialDieFilterSprite[i])->SetColor(1, 255, 255, 255, 255);
			}
			else
			{
				((i3Sprite2D*)m_vSpecialDieFilterSprite[i])->SetEnable(false);
			}
		}
	}
}

void CHud::InsertScopeHudTexture(INT32 index)
{
	if (m_pScope != nullptr)
	{
		m_pScope->InsertScopeHudTexture(index);
	}
	return;
}

void CHud::ClearPauseMessage(void)
{
	m_pSpritePause->SetEnable(false);
}

void CHud::OnPauseMessage(void)
{
	if( g_pFramework->getHudLayer()->IsFlag(I3_NODEFLAG_DISABLE))	
		g_pFramework->getHudLayer()->RemoveFlag(I3_NODEFLAG_DISABLE);

	m_pSpritePause->SetEnable(true);
}

bool CHud::_UpdateAttackedEffect(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	bool bEnabledEffect = false;

	static REAL32 fOldAngle = 0.0f;
	REAL32 fCurrentAngle = 0.0f;
	REAL32 fDeltaAngle;

	CGameCharaBase * pPlayer = GetCurrentChara();

	if( pPlayer == nullptr)
		return false;

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
				m_pSpriteHit->SetEnable(HUD_SPRITE_HIT_ANGLE_START + i, false);
			}
			else
			{
				pColor->a = (UINT8)nAlpha;
				bEnabledEffect = true;
			}
		}
	}			

	return bEnabledEffect;
}

bool CHud::_UpdateGrazedEffect(REAL32 rDeltaSeconds)
{
	i3_prof_func();

	bool bEnabledEffect = false;

	for(INT32 i = HUD_SPRITE_WARNING_LEFT_FRONT; i <= HUD_SPRITE_WARNING_RIGHT_BACK; i++)
	{
		if (m_pSpriteHit->GetEnable(i))
		{
			COLOR* pColor = m_pSpriteHit->GetColor(i);
			INT32 nAlpha = pColor->a;

			nAlpha -= (INT32)(WARNING_DIRECTION_DECREASE * rDeltaSeconds);

			if (0 >= nAlpha)
			{
				m_pSpriteHit->SetEnable(i, false);
			}
			else
			{
				pColor->a = (UINT8)nAlpha;
				bEnabledEffect = true;
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
		m_pSpriteHit->SetEnable(HUD_SPRITE_WARNING_LEFT_FRONT + i, false);
	}
}

bool CHud::_IsAttackedEffectEnabled(void)
{
	i3_prof_func();

	for( INT32 i = 0; i < HUD_SPRITE_HIT_ANGLE_MAX; i++)
	{			
		if (m_pSpriteHit->GetEnable(HUD_SPRITE_HIT_ANGLE_START + i))
		{
			return true;
		}
	}

	return false;
}

void CHud::UpdateRadioChat( REAL32 fTime )
{
	i3_prof_func();

	INT32	i, ChatNum, nLine;
	i3::stack_wstring	wstrTemp;
	RADIOCHAT_TYPE	ChatType;

	nLine = 0;

	m_rRadioReusingTimer += fTime;
	if( m_rRadioReusingTimer < 2.f )
		return;

	CGameCharaBase * pPlayer = GetCurrentChara();
	UINT32 slotIdx = BattleSlotContext::i()->getMySlotIdx();

	if( pPlayer == nullptr || UserContext::i()->Death[BattleSlotContext::i()->getMySlotIdx()] ||
		!UserContext::i()->IsPlayingSlot(slotIdx))
	{
		// clear message
		for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
		{
			m_pTextRadioChat[i]->SetText( L"" );
			m_pTextRadioChat[i]->SetEnable( false );
		}
		m_bEnableRadioChat = false;
		m_bEnableRadio1 = false;
		m_bEnableRadio2 = false;
		m_bEnableRadio3 = false;

		return;
	}

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO1) 
		&&  !(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO2) 
		&&  !(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO3)
		&&	HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		if( !m_bEnableRadio1 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{	
				// 9번은 없음
				if(i==9) {
					continue;
				}
				i3::rc_wstring wstrMessage = GameContextUtil::GetRadioMessage( RADIOCHAT_TYPE_COMMAND, i );
				if( i3::generic_string_size( wstrMessage ) > 0 )
				{
					if( i != 0 )
						i3::sprintf( wstrTemp, L"%d. ", i % 10 );
					i3::generic_string_cat( wstrTemp, wstrMessage );
					m_pTextRadioChat[nLine]->SetText( wstrTemp );
					m_pTextRadioChat[nLine]->SetEnable( true );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}

			m_bEnableRadioChat = true;
			m_bEnableRadio1 = true;
			m_bEnableRadio2 = false;
			m_bEnableRadio3 = false;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}
			m_bEnableRadioChat = false;
			m_bEnableRadio1 = false;
		}
	}

	nLine = 0;

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO2)
		&&  !(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO1) 
		&&	!(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO3)
		&&	HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		if( !m_bEnableRadio2 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				// 9번은 없음
				if(i==9) {
					continue;
				}
				i3::rc_wstring wstrMessage = GameContextUtil::GetRadioMessage( RADIOCHAT_TYPE_REQUEST, i );
				if( i3::generic_string_size( wstrMessage ) > 0 )
				{
					if( i != 0 )
						i3::sprintf( wstrTemp, L"%d. ", i % 10 );
					i3::generic_string_cat( wstrTemp, wstrMessage );
					m_pTextRadioChat[nLine]->SetText( wstrTemp );
					m_pTextRadioChat[nLine]->SetEnable( true );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}

			m_bEnableRadioChat = true;
			m_bEnableRadio2 = true;
			m_bEnableRadio1 = false;
			m_bEnableRadio3 = false;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}
			m_bEnableRadioChat = false;
			m_bEnableRadio2 = false;
		}
	}

	nLine = 0;

	if( (g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO3) 
		&&  !(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO1) 
		&&  !(g_pFramework->getKeyStroke() & GAME_KEY_CMD_RADIO2)
		&&	HUD::instance()->GetHud(HUD::MULTIWEAPON)->IsVisible() == false)
	{
		if( !m_bEnableRadio3 )
		{
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				i3::rc_wstring wstrMessage = GameContextUtil::GetRadioMessage( RADIOCHAT_TYPE_STATUS, i );
				if( i3::generic_string_size( wstrMessage ) > 0 )
				{
					if( i != 0 )
						i3::sprintf( wstrTemp, L"%d. ", i % 10 );
					i3::generic_string_cat( wstrTemp, wstrMessage );
					m_pTextRadioChat[nLine]->SetText( wstrTemp );
					m_pTextRadioChat[nLine]->SetEnable( true );
					nLine++;
					continue;
				}
			}

			for( i = nLine; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}

			m_bEnableRadioChat = true;
			m_bEnableRadio3 = true;
			m_bEnableRadio1 = false;
			m_bEnableRadio2 = false;
		}
		else
		{
			// clear message
			for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
			{
				m_pTextRadioChat[i]->SetText( L"" );
				m_pTextRadioChat[i]->SetEnable( false );
			}
			m_bEnableRadioChat = false;
			m_bEnableRadio3 = false;
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
					m_pTextRadioChat[i]->SetText(L"");
					m_pTextRadioChat[i]->SetEnable( false );	
				}
				m_bEnableRadioChat = false;
				m_bEnableRadio1 = false;
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
					m_pTextRadioChat[i]->SetText(L"");
					m_pTextRadioChat[i]->SetEnable( false );	
				}
				m_bEnableRadioChat = false;
				m_bEnableRadio2 = false;
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
					m_pTextRadioChat[i]->SetText(L"");
					m_pTextRadioChat[i]->SetEnable( false );	
				}
				m_bEnableRadioChat = false;
				m_bEnableRadio3 = false;
			}
		}


		if( ChatNum > 0 )
		{
			if( (ChatType == RADIOCHAT_TYPE_COMMAND && ChatNum != 9) ||
				(ChatType == RADIOCHAT_TYPE_REQUEST && ChatNum != 9) ||
				(ChatType == RADIOCHAT_TYPE_STATUS))
			{
				GAMEEVENT::Write_Chara_RadioChat( pPlayer, (UINT8)(ChatNum + (10*ChatType)));

				if( BattleServerContext::i()->IsP2PHost())
				{
					i3::rc_wstring wstrMessage = GameContextUtil::GetRadioMessage( ChatType, ChatNum );
					if( i3::generic_string_size( wstrMessage ) > 0 )
					{
						i3::rc_wstring wstrNick = BattleSlotContext::i()->getNickForSlot(BattleSlotContext::i()->getMySlotIdx());

						if( i3::generic_string_size( getMiniMapLocationText()) > 0)
						{
							i3::sprintf( wstrTemp, L"[Radio] %s {col:98, 180, 253, 255}[%s]{/col}", wstrMessage, getMiniMapLocationText());
						}
						else
						{
							i3::sprintf( wstrTemp, L"[Radio] %s", wstrMessage );
						}						

						g_pFramework->getChatBox()->PutUserChatting( wstrNick, wstrTemp ,0);
						GameContextUtil::PlayRadioMessageSound( ChatType, ChatNum, pPlayer);
					}
				}

				switch( ChatType )
				{
				case RADIOCHAT_TYPE_COMMAND : m_bEnableRadio1 = false; break;
				case RADIOCHAT_TYPE_REQUEST : m_bEnableRadio2 = false; break;
				case RADIOCHAT_TYPE_STATUS : m_bEnableRadio3 = false; break;
				}

				// clear message
				for( i = 0; i < HUD_RADIOCHAT_COUNT; i++ )
				{
					m_pTextRadioChat[i]->SetText(L"");
					m_pTextRadioChat[i]->SetEnable( false );	
				}
				m_bEnableRadioChat = false;

				m_rRadioReusingTimer = 0.f;
			}
		}
	}
}

bool CHud::GetChatEnable(void) const
{
	i3_prof_func();

	UIHUDIngameChat * pChat = (UIHUDIngameChat *)m_pBattleFrame->GetHUD(UIHUD_INGAMECHATING);

	if(pChat)
		return pChat->GetEnableChat();

	return false;
}

void	CHud::SetChatEnable(bool bFlag)
{
	i3_prof_func();

	UIHUDIngameChat * pChat = (UIHUDIngameChat *)m_pBattleFrame->GetHUD(UIHUD_INGAMECHATING);
	pChat->SetEnableChat( bFlag );
}

void CHud::_ProcessChatNotice( void)
{
	i3_prof_func();

	INT32 nTime = (INT32) g_pFramework->GetAccTime();

	if( nTime % 3600 == 0 )	// 1 hour
	{
		if( !m_bShowChatNotice )
		{
			i3::rc_wstring wstrNotice;

			if( m_nCurrentChatNotice == 0)
			{
				if( UserInfoContext::i()->IsAdultUser())
					wstrNotice = GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_GRADE1");/*{col:255,255,0,255}포인트 블랭크는 게임 산업 진흥에 의한 법률 제21조 2항 게임 등급 분류 기준에 의거하여 18세 이상 이용가임을 알려드립니다.{/col}*/
				else
					wstrNotice = GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_GRADE2");/*{col:255,255,0,255}포인트 블랭크는 게임 산업 진흥에 의한 법률 제21조 2항 게임 등급 분류 기준에 의거하여 15세 이상 이용가임을 알려드립니다.{/col}*/

				m_nCurrentChatNotice = 1;
			}
			else
			{
				wstrNotice = GAME_RCSTRING("STR_TBL_INGAME_HUD_HELP_USE_TEXT");/*게임 중 F1 키를 누르시면 도움말을 보실 수 있습니다.*/
				m_nCurrentChatNotice = 0;
			}

			g_pFramework->getChatBox()->PutSystemChatting( wstrNotice);
			g_pFramework->getChatBox()->ResetBackupBuffer();
			m_pBattleFrame->UpdateChatBuffer();
			m_bShowChatNotice = true;
		}
	}
	else
	{
		m_bShowChatNotice = false;
	}
}

void CHud::EditCharaNick( i3::wstring& wstrTarget, const i3::wliteral_range& wSourceRng, INT32 LimitWidth)
{
	i3_prof_func();

	if (!m_pTextDummy)
		return;
		
	m_pTextDummy->SetText( wSourceRng);
	i3TextAttrDX2* pAttr = m_pTextDummy->getTextAttrDX();

	// 글폭이 컨트롤폭을 넘어가면 "..."으로 끊는다
	if (pAttr->getTextWidth() > LimitWidth)
	{
		INT32 ellipsisLength = 0;

		// "..." 폭 구하기
		pAttr->SetText(L"...");
		REAL32 margin = (REAL32)(LimitWidth - pAttr->getTextWidth());

		// "..." 폭을 제외한 여유공간에 들어갈 글자 계산
		INT32 length = i3::generic_string_size(wSourceRng);
	//	INT32 codeType = 0;

		for(INT32 i = 0; i < length; i++)
		{
			margin -= pAttr->GetCharacterSizeX(&wSourceRng[i]);

			if (0.0f > margin)
			{
				break;
			}

		// TODO : UTF16의 서로게이트 문자열 처리가 필요할수 있는데..일단 스킵했다. (2014.07.11.수빈)
		// 멀티바이트 문자를 위한 문자단위 증가
		//	if (0 != i3String::WhatIsCode(pszSource, i))
		//	codeType = i3::curr_mbb_byte(pszSource[i], codeType);
		//	if ( 0 != codeType )
		//	{
		//		i++;
		//	}

			ellipsisLength = i + 1;
		}

		// 계산한 위치에 "..."을 붙인다
		i3::generic_string_ncopy(wstrTarget, wSourceRng, ellipsisLength);		
		i3::generic_string_cat(wstrTarget, L"...");				
	}
	else
	{
		wstrTarget.assign(wSourceRng.begin(), wSourceRng.end());
	}
}


void CHud::Bubble( SCORE_ARRAY *pArr, INT32 num)
{
	i3_prof_func();

	INT32 i = 0, j = 0;
	SCORE_ARRAY temp; 

	for( i=0; i< num-1; i++)
	{
		for( j=0; j< num-i-1; j++)
		{
			if( pArr[j].nKillCount < pArr[j+1].nKillCount)
			{
				temp = pArr[j+1];
				pArr[j+1] = pArr[j];
				pArr[j] = temp;
			}
			else if( pArr[j].nKillCount == pArr[j+1].nKillCount)
			{
				if( pArr[j].nDeath > pArr[j+1].nDeath)
				{
					temp = pArr[j+1];
					pArr[j+1] = pArr[j];
					pArr[j] = temp;
				}
			}
		}
	}
}

void CHud::SetEnableGrenadeLauncherCrossHair( bool bVisible)
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
			TRACE("CHud::SetEnableGrenadeLauncherCrossHair m_pSpriteGrenadeLauncherCrossHair = nullptr\n");
		m_bEnabledGrenadeLauncherCrossHair = false;
	}
}

CGameCharaBase * CHud::GetCurrentChara( void)
{	
	i3_prof_func();

	return g_pCharaManager->getCharaByNetIdx( m_CurrentUserIdx);
}

void CHud::SetDetectedEnemyInfo( INT32 idx)
{
	if( m_pPanelTargetHP )
	{
		m_pPanelTargetHP->SetDetectedEnemyInfo( idx);
	}
}

INT32	CHud::getChatOffsetPos(void)
{
	return m_pObserverHPGaugeHUD->getChatOffset();
}


// 탈취 모드
void CHud::SetEnableUsurpationMission( bool bFlag )
{
	i3_prof_func();
	if(bFlag && !m_pSpriteUsurpation->GetEnable(HUD_SPRITE_USURPATION_WATERMELON))
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		INT32 nScreenWidth = 0;
		INT32 nScreenHeight= 0;

		REAL32 fClanOffsetY = 5.0f;

		if( ChannelContext::i()->GetBattleChannel())
		{
			fClanOffsetY = 18.0f;
		}

		if( g_pViewer != nullptr)
		{
			nScreenWidth = g_pViewer->GetViewWidth();
			nScreenHeight = g_pViewer->GetViewHeight();
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		// 시작 되고 나서면 켜줘야 한다.
		m_pSpriteUsurpation->SetEnable( HUD_SPRITE_USURPATION_WATERMELON, true);
		
		UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
		m_pSpriteUsurpation->SetRect( HUD_SPRITE_USURPATION_WATERMELON, 
			(REAL32)(nPosX-30), 
			pBoard->GetTimeCtrlPos()->y + pBoard->GetTimeCtrlHeight() + GAP_BETWEEN_SCOREBOARD_BOMBIMAGE + fClanOffsetY, 
			(59.0f), 
			(80.0f));
	}
	else if(bFlag == false)
	{
		m_pSpriteUsurpation->SetEnable( HUD_SPRITE_USURPATION_WATERMELON, false);
	}
}

void CHud::SetEnableUsurpationMessage( bool bFlag, const i3::wliteral_range& wTextRng /*= nullptr*/ )
{
	i3_prof_func();

	if(bFlag)
	{
		INT32 nPosX = 0;
		INT32 nPosY = 0;
		INT32 nScreenWidth = 0;
		INT32 nScreenHeight= 0;

		REAL32 fClanOffsetY = 5.0f;
		REAL32 fWatermelonY = 120.0f;

		if( ChannelContext::i()->GetBattleChannel())
		{
			fClanOffsetY = 18.0f;
		}

		if( g_pViewer != nullptr)
		{
			nScreenWidth = g_pViewer->GetViewWidth();
			nScreenHeight = g_pViewer->GetViewHeight();
			nPosX = (INT32)(nScreenWidth * 0.5f);
			nPosY = (INT32)(nScreenHeight * 0.5f);
		}

		m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->SetEnable(true);

		UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));

		COLOR colReal;
		i3Color::Set( &colReal, (UINT8) 240, 210, 110, 255);
		m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->SetColor( &colReal);

		m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->SetText(wTextRng);
		m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->setPos((nPosX-300), 
			(UINT32)(pBoard->GetTimeCtrlPos()->y + pBoard->GetTimeCtrlHeight() + GAP_BETWEEN_SCOREBOARD_BOMBIMAGE + fClanOffsetY + fWatermelonY));
	}
	else
	{
		m_pTextUsurpation[HUD_USURPATION_TEXT_NOTICE]->SetEnable(false);
	}
}

void CHud::ConvoyMsgStart( void )
{
	i3::rc_wstring wstrText;

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);
	TEAM_TYPE team = g_pCharaManager->getCharaByNetIdx(m_CurrentUserIdx)->getTeam();
	if(	MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		if(team == TEAM_BLUE)
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_FIRST_BLUE");
		else
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_FIRST_RED");
	}
	else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
			 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL) )
	{
		if(team == TEAM_BLUE)
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_FIRST_BLUE");
		else
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_FIRST_RED");
	}

	m_bUserpationMission = true;
	SetEnableUsurpationMission(true);
	SetEnableUsurpationMessage(true, wstrText);
	// Indicator
	m_bUsurpationIndicator = true;
}

void CHud::ConvoyMsgDrop()
{
	i3::rc_wstring wstrText;

	m_bGetSoundPlay = false;
	m_bUsurpationEnable = true;
	m_rUsurpationStartTime = 0;

	INT32 iRedCount = BattleSlotContext::i()->GetBattleRedTeamCount();
	if( iRedCount < 1)
		return;

	I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);
	TEAM_TYPE team = g_pCharaManager->getCharaByNetIdx(m_CurrentUserIdx)->getTeam();
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		if(team == TEAM_BLUE)
		{
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_DROP_BLUE");
		}
		else
		{
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_DROP_RED");
		}
	}
	else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
		     MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL))
	{
		if(team == TEAM_BLUE)
		{
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_DROP_BLUE");
		}
		else
		{
			wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_DROP_RED");
		}
	}

	g_pSndMng->StartAnnounceMessage(GTA_DROP_CONVOY);

	SetEnableUsurpationMission(false);
	SetEnableUsurpationMessage(true, wstrText);

	// Indicator
	m_bUsurpationIndicator = true;
}

void CHud::ConvoyMsgGet(void)
{
	if(CGameMissionManager::i()->getMissionState() != GAME_MISSION_STATE_ENDBATTLE)
	{
		i3::rc_wstring wstrText;
		m_bUsurpationEnable = true;
		m_rUsurpationStartTime = 0;

		I3_BOUNDCHK( m_CurrentUserIdx, SLOT_MAX_COUNT);
		TEAM_TYPE team = g_pCharaManager->getCharaByNetIdx(m_CurrentUserIdx)->getTeam();
		
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
			MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
		{
			if(team == TEAM_BLUE)
			{
				wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_GET_BLUE");

				if(m_bGetSoundPlay == false)
				{
					g_pSndMng->StartAnnounceMessage(GTA_GET_WATERMELON_BLUE);
					m_bGetSoundPlay = true;
				}

				// Indicator
				m_bUsurpationIndicator = false;
			}
			else
			{
				wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_WATERMELON_GET_RED");

				if(m_bGetSoundPlay == false)
				{
					g_pSndMng->StartAnnounceMessage(GTA_GET_WATERMELON_RED);
					m_bGetSoundPlay = true;
				}

				// Indicator
				m_bUsurpationIndicator = true;
			}
		}
		else if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) &&
				 MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_NORMAL) )
		{
			if(team == TEAM_BLUE)
			{
				wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_GET_BLUE");
				// Indicator
				m_bUsurpationIndicator = false;
			}
			else
			{
				wstrText = GAME_RCSTRING("STR_TBL_INGAME_HUD_CONVOY_GET_RED");
				// Indicator
				m_bUsurpationIndicator = true;
			}

			if(m_bFirstGetSoundPlay == false)
			{
				g_pSndMng->StartAnnounceMessage(GTA_FIRSTGET_CONVOY);
				m_bFirstGetSoundPlay = true;
			}

			VEC3D obj_pos = *CGameMissionManager::i()->getMissionObjPos();
			if( m_bFirstGetSoundPlay == true && m_bGetSoundPlay == false)
			{
				g_pSndMng->StartAnnounceMessage(GTA_GET_CONVOY, &obj_pos);
				m_bGetSoundPlay = true;
			}
		}

		SetEnableUsurpationMission(true);
		SetEnableUsurpationMessage(true, wstrText);
	}
}
void CHud::ProcessUsurpation( REAL32 rDeltaSeconds )
{

	// 시작 3초간
	if( m_bUsurpationEnable == true)
	{
		if(m_rUsurpationStartTime < 3.0f)
		{
			// 3초간의 시간 더하기
			m_rUsurpationStartTime += rDeltaSeconds;
		}
		else
		{
			m_rUsurpationStartTime = 0.0f;
			m_bUsurpationEnable = false;

			SetEnableUsurpationMessage(false);
			SetEnableUsurpationMission(false);
			//if( m_bUserpationMission == true)
			//{
			//	SetEnableUsurpationMission(false);
			//	m_bUserpationMission = false;
			//}
		}
	}
	ProcessUsurpationIndicator(m_bUsurpationIndicator);
}

void CHud::ProcessUsurpationHUD( REAL32 rDeltaSeconds )
{
	if(m_bFirstUsurpationHUD == true 
		&& m_bFirstUsurpationUI == false
		&& MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) )
	{
		if(m_bOpenUsurpationHUD == false)
		{
			if(m_pBattleFrame != nullptr)
			{
				if(m_pBattleFrame->IsOpenPopup(UBP_HUD_USURPATION) == false)
				{
					m_pBattleFrame->OpenPopup(UBP_HUD_USURPATION);
					m_bOpenUsurpationHUD = true;
				}
			}
		}
	}
	else if( m_bFirstUsurpationUI == true)

	{
		m_bFirstUsurpationUI = false;
		m_bUsurpationEnable = true;

		ConvoyMsgStart();
	}

}

void CHud::ResetUsurpationHUD()
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY) )
	{
		if(CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INBATTLE)
		{
			// 2차 수박 3초 오픈
			m_bFirstUsurpationHUD = true;
			m_bFirstUsurpationUI = true;

			if(m_bUsurpationEnable == true)
				m_bUsurpationEnable = false;

			m_rUsurpationStartTime = 0.0f;
		}
		else
		{
			// 1차 HUD 오픈
			if(m_bFirstUsurpationUI == false )
				m_bFirstUsurpationHUD = true;

			if(m_bUsurpationEnable == true)
				m_bUsurpationEnable = false;

			m_rUsurpationStartTime = 0.0f;
		}
	}
	m_bUsurpationIndicator = false;

	m_bFirstGetSoundPlay = false;
	m_bGetSoundPlay = false;
}

void CHud::ProcessEscape(REAL32 rDeltaSeconds)
{
	if (m_bFirstHUD == false)
	{
			if (m_pBattleFrame != nullptr)
			{
				if (m_pBattleFrame->IsOpenPopup(UBP_HUD_ESCAPE) == false)
				{
					m_pBattleFrame->OpenPopup(UBP_HUD_ESCAPE);
					m_bFirstHUD = true;
				}
			}
	}	
}

void CHud::ProcessUsurpationIndicator(bool bFlag)
{
	if(bFlag)
	{
		CGameCharaBase* pOwner = g_pCharaManager->getCharaByNetIdx(m_CurrentUserIdx);

		VEC3D obj_pos = *CGameMissionManager::i()->getMissionObjPos();
		VEC3D* chara_pos = pOwner->GetPos();

		i3Viewer * pViewer		= g_pFramework->GetViewer();
		INT32	nScreenWidth	= pViewer->GetViewWidth();
		INT32	nScreenHeight	= pViewer->GetViewHeight();

		REAL32 X, Y, length;
		VEC3D	vLength;
		i3Vector::Sub( &vLength, chara_pos, &obj_pos);

		length = i3Vector::Length( &vLength);

		if( length > 10.0f)
		{
			if( _CalcProjectionPos( &X, &Y, &obj_pos))
			{
				REAL32 rate = 1.1f - (length / 10.0f) * 0.1f;
				REAL32 size = MINMAX( 27.5f, 55.0f * rate, 55.0f);
				
				if(X < 0) X = 0;
				else if(X > 1) X = 1;

				if(Y < 0) Y = 0;
				else if(Y > 1) Y = 1;

				REAL32 XX = nScreenWidth * X - size * 0.5f;
				REAL32 YY = nScreenHeight * Y - size * 0.5f;

				m_pSpriteUsurpation->SetRect( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, XX, YY, size, size);

				if( !m_pSpriteUsurpation->GetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR))
				{
					m_pSpriteUsurpation->SetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, true);
				}

				return;
			}
		}

		if( m_pSpriteUsurpation->GetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR))
		{
			m_pSpriteUsurpation->SetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, false);
		}
	}
	else
	{
		if( m_pSpriteUsurpation->GetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR))
		{
			m_pSpriteUsurpation->SetEnable( HUD_SPRITE_USURPATION_WATERMELON_INDICATOR, false);
		}
	}
}

void CHud::ProcessSpace(REAL32 rDeltaSeconds)
{
	if (m_bFirstHUD == false)
	{
		UIBattleFrame * pFrame = UIBattleFrame::i();
		if (pFrame != nullptr)
		{
			if (pFrame->IsOpenPopup(UBP_HUD_SPACE) == false)
			{
				pFrame->OpenPopup(UBP_HUD_SPACE);
				m_bFirstHUD = true;
			}
		}
	}
}

// 치트키
void CHud::ShowCheatkeyWarpList(const i3::wstring text)
{
	INT32 screenWidth = g_pFramework->GetViewer()->GetViewWidth();
	INT32 offset = (INT32)(13*i3GuiRoot::getHeightRatioBy768());

	m_pTextCheatkeyWarp->SetText(text);
	m_pTextCheatkeyWarp->SetEnable(true);
	INT32 msgWidth = m_pTextCheatkeyWarp->getTextWidth();
	m_pTextCheatkeyWarp->setPos(screenWidth - offset - msgWidth, static_cast<UINT32>(NOTICE_LINE_HEIGHT+50.0f));
}

void CHud::HideCheatkeyWarpList()
{
	m_pTextCheatkeyWarp->SetText(L"");
	m_pTextCheatkeyWarp->SetEnable(false);
}

void CHud::ShowProfile()
{
	if (m_bEnableProfile)
	{
		INT32 w = g_pFramework->GetViewer()->GetViewWidth();
		INT32 h = g_pFramework->GetViewer()->GetViewHeight();

		i3GfxPerformanceReport * pReport = g_pViewer->GetRenderContext()->GetPerformanceReport();

		i3::rc_wstring wProfileText;
		wProfileText += i3::format_string(L"FPS        : %.2f\n", pReport->GetFPS());
		wProfileText += i3::format_string(L"Tri Count  : %d\n", pReport->GetTriangleCountPerFrame());
		wProfileText += i3::format_string(L"Draw Calls : %d\n", pReport->GetDrawCallCountPerFrame());

		m_pTextProfile->SetText(wProfileText);
		m_pTextProfile->SetEnable(true);

		m_pTextProfile->setPos(static_cast<UINT32>(w * 0.8), static_cast<UINT32>(h * 0.2));
	}
	else
	{
		m_pTextProfile->SetEnable(false);
	}
}

bool CHud::isEnableCheatkeyWarpList() const
{
	return (m_pTextCheatkeyWarp->GetEnable() == true);
}

// 탭미니맵 속성값 출력
void CHud::ShowTabminimapProps(const i3::wstring text)
{
	INT32 screenWidth = g_pFramework->GetViewer()->GetViewWidth();
	INT32 screenHeight = g_pFramework->GetViewer()->GetViewHeight();

	m_pTextTabminimapProps->SetText(text);
	m_pTextTabminimapProps->SetEnable(true);
	m_pTextTabminimapProps->setPos(static_cast<UINT32>(screenWidth*0.65f), static_cast<UINT32>(screenHeight*0.035f));
}

void CHud::HideTabminimapProps()
{
	m_pTextTabminimapProps->SetText(L"");
	m_pTextTabminimapProps->SetEnable(false);
}

bool CHud::_CalcProjectionPos( REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn)
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

