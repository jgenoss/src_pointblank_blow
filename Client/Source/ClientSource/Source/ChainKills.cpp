#include "pch.h"
#include "ChainKills.h"
#include "GlobalVariables.h"
#include "RevengeManager.h"
#include "MedalManager.h"

#include "BattleHud.h"
#include "GameCharaEquipContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaContexts.h"

#include "ConfigRenderBattle.h"

#include "UI/UIHudManager.h"
#include "UI/UIUtil.h"
#include "ui/UIMath.h"
#include "ui/UIHudUtil.h"
#include "UI/UIHudQuestEffect.h"
#include "StageBattle/AIModeContext.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "MainWeapon_C5.h"


I3_CLASS_INSTANCE(CChainKills); //, i3GameObjBase);


namespace
{
	enum
	{
		CK_MAX_HISTORY_COUNT = 9,
		CK_MAX_MESSAGE_COUNT = 5,
		CK_MAX_BONUS_COUNT = 4,

		CK_TIME_SHOW_MESSAGE = 1,
		CK_TRAINING_TIME_SHOW_MESSAGE = 6,
	};

	const REAL32 CK_SIZE_X = 52.f;
	const REAL32 CK_SIZE_Y = 56.f;

	const REAL32 CK_DOUBLE_SIZE_X = CK_SIZE_X * 2.f;
	const REAL32 CK_DOUBLE_SIZE_Y = CK_SIZE_Y * 2.f;

	const INT32 CK_OFFSET = 65;
	const INT32 CK_Text_OFFSET = 13;

	CChainKills* this_ptr;
}

namespace wop //weapon operator
{
	bool is_throwing_wp_smoke(class CWeaponInfo* p); //wp 스모크
	bool is_throwing_grenade(class CWeaponInfo* p); //수류탄
}



CChainKills* CChainKills::instance()
{
	return this_ptr;
}

CChainKills::CChainKills()
{
	this_ptr = this;

	pf_add_chain_kil_listl[0] = &CChainKills::AddChainKills_Normal_Mode;
	pf_add_chain_kil_listl[1] = &CChainKills::AddChainKills_Boss_Mission_Mode;

	m_pSceneNode = i3Node::new_object();

	m_nFrontKillMessage = 0;
	m_nRearKillMessage = 0;
	m_nCountKillMessage = 0;	

	m_pTexKillMessage = nullptr;
	m_pTexKillMessage1 = nullptr;
	m_pTexKillMessage2 = nullptr;
	m_pTexTrainingKillMessage = nullptr;

	for(INT32 i = 0; i < 2; i++)
	{
		m_pTexKillHistory[i] = nullptr;
		m_pSprKillHistory[i] = nullptr;
	}
	m_pTexKillHistoryBG = nullptr;
	m_pSprKillHistoryBG = nullptr;

	m_pSprKillMessage = nullptr;
	m_pSprKillMessage1 = nullptr;
	m_pSprKillMessage2 = nullptr;
	m_pSprTrainingKillMessage = nullptr;


	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		m_pTextBonusPoint[i] = nullptr;
		m_pTextKillCount[i] = nullptr;

		m_qKillMessage[i].Reset();
		m_qKillHistory[i].Reset();

		m_CkStep[i] = 0;
	}

	m_pTextTrainingMessage = nullptr;
	m_pTextRevengeMessage = nullptr;

	m_nStartHistory = 0;
	m_nCountHistory = 0;		

	m_rKillMessageTerm = 0.f;
	m_rMedalMessageTerm = 0.f;
	m_rBonusPointTerm = 0.f;

	m_nBonusPointCount = 0;
	m_nCurrentShowPoint = 0;
	m_rMessageScale = 0.f;

	m_fViewerWidthRate = 0.0f;
	m_fViewerHeightRate = 0.0f;

	//m_difficultyLevelUpEnable = false;
	m_levelUpTextEnable = true;

	m_pSprite = nullptr;
	m_pTex = 0;
	m_bUpdateHelmet	 = 0;
	m_bUpdateC5 = false;

	m_BonusScore = 0;
}

CChainKills::~CChainKills()
{
	this_ptr = 0;

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextKillCount[i]);
	}

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		I3_SAFE_NODE_RELEASE(m_pTextBonusPoint[i]);
	}

	I3_SAFE_NODE_RELEASE(m_pTextTrainingMessage);
	I3_SAFE_NODE_RELEASE(m_pTextRevengeMessage);


	I3_SAFE_NODE_RELEASE(m_pSprKillHistoryBG);
	I3_SAFE_NODE_RELEASE(m_pSprKillMessage);
	I3_SAFE_NODE_RELEASE(m_pSprKillMessage1);
	I3_SAFE_NODE_RELEASE(m_pSprKillMessage2);
	I3_SAFE_NODE_RELEASE(m_pSprTrainingKillMessage);

	for(INT32 i = 0; i < 2; i++)
	{
		I3_SAFE_RELEASE(m_pTexKillHistory[i]);
		I3_SAFE_NODE_RELEASE(m_pSprKillHistory[i]);
	}
	I3_SAFE_RELEASE(m_pTexKillHistoryBG);
	I3_SAFE_RELEASE(m_pTexKillMessage);
	I3_SAFE_RELEASE(m_pTexKillMessage1);
	I3_SAFE_RELEASE(m_pTexKillMessage2);
	I3_SAFE_RELEASE(m_pTexTrainingKillMessage);

	I3_SAFE_RELEASE(m_pTex);
	I3_SAFE_NODE_RELEASE(m_pSprite);

	//m_pSceneNode는 i3GameObjBase에서 책임진다
	// Child도 책임진다
}

bool CChainKills::Create( i3Node *pParentNode, bool bEnable)
{
	if( ! i3GameObjBase::Create( g_pFramework->getHudLayer() ) )
	{
		return false;
	}

	m_finish_kill_mark = m_finish_kill_msg = true;

	m_fViewerWidthRate = ((REAL32)g_pViewer->GetViewWidth()) / 1024.0f;
	m_fViewerHeightRate = ((REAL32)g_pViewer->GetViewHeight()) / 768.0f;

	// Message Sprite 생성
	{
		i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(490, 512);

		m_pTexKillMessage = crb::i()->LoadHudTexture(HUD_TEXTURE_KILL_MESSAGE);
		m_pSprKillMessage = i3Sprite2D::new_object();
		m_pSprKillMessage->SetTexture(m_pTexKillMessage);
		m_pSprKillMessage->Create(1, true, true);		
		m_pSprKillMessage->SetTextureCoord(0, 0.0f, 0.0f, 512.0f, 102.0f);
		m_pSprKillMessage->SetColor(0, 255, 255, 255, 255);	
		m_pSprKillMessage->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f, 102.0f);	
		m_pSprKillMessage->SetEnable(0, false);
		GetNode()->AddChild(m_pSprKillMessage);

		m_pTexKillMessage1 = crb::i()->LoadHudTexture(HUD_TEXTURE_KILL_MESSAGE1);
		m_pSprKillMessage1 = i3Sprite2D::new_object();
		m_pSprKillMessage1->SetTexture(m_pTexKillMessage1);
		m_pSprKillMessage1->Create(1, true, true);		
		m_pSprKillMessage1->SetTextureCoord(0, 0.0f, 0.0f, 512.0f, 102.0f);
		m_pSprKillMessage1->SetColor(0, 255, 255, 255, 255);	
		m_pSprKillMessage1->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f, 102.0f);
		m_pSprKillMessage1->SetEnable(0, false);
		GetNode()->AddChild(m_pSprKillMessage1);

		m_pTexKillMessage2 = crb::i()->LoadHudTexture(HUD_TEXTURE_KILL_MESSAGE2);
		m_pSprKillMessage2 = i3Sprite2D::new_object();
		m_pSprKillMessage2->SetTexture(m_pTexKillMessage2);
		m_pSprKillMessage2->Create(1, true, true);
		m_pSprKillMessage2->SetTextureCoord(0, 0.0f, 0.0f, 512.0f, 102.0f);
		m_pSprKillMessage2->SetColor(0, 255, 255, 255, 255);
		m_pSprKillMessage2->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f, 102.0f);
		m_pSprKillMessage2->SetEnable(0, false);
		GetNode()->AddChild(m_pSprKillMessage2);

		if (MyRoomInfoContext::i()->getStage()->IsAiMode())
		{
			m_pTexTrainingKillMessage = crb::i()->LoadHudTexture(HUD_TEXTURE_TRAINING_KILL_MESSAGE);

			m_pSprTrainingKillMessage = i3Sprite2D::new_object();
			m_pSprTrainingKillMessage->SetTexture(m_pTexTrainingKillMessage);
			m_pSprTrainingKillMessage->Create(1, true, true);
			m_pSprTrainingKillMessage->SetTextureCoord(0, 0.0f, 0.0f, 513.0f, 95.0f);
			m_pSprTrainingKillMessage->SetColor(0, 255, 255, 255, 255);
			m_pSprTrainingKillMessage->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 513.0f, 95.0f);
			m_pSprTrainingKillMessage->SetEnable(0, false);
			GetNode()->AddChild(m_pSprTrainingKillMessage);
		}
	}

	// History Sprite BG 생성
	{
		i3::pack::POINT2D ptScreenSize = g_pFramework->GetSreenSize();
		ptScreenSize.x -= 215; ptScreenSize.y -= 65;

		m_pTexKillHistoryBG = crb::i()->LoadHudTexture(HUD_TEXTURE_KILLHISTORY_BG);
		m_pSprKillHistoryBG = i3Sprite2D::new_object();
		m_pSprKillHistoryBG->SetTexture(m_pTexKillHistoryBG);
		m_pSprKillHistoryBG->Create(1, true, true);
		m_pSprKillHistoryBG->SetTextureCoord(0, 0.0f, 0.0f, 128.0f, 128.0f);
		m_pSprKillHistoryBG->SetColor(0, 255, 255, 255, 255);
		m_pSprKillHistoryBG->SetRect(0, (REAL32)ptScreenSize.x, (REAL32)ptScreenSize.y, 128.0f, 128.0f);
		m_pSprKillHistoryBG->SetEnable(0, false);
		GetNode()->AddChild(m_pSprKillHistoryBG);
	}

	// History Sprite 생성
	{
		i3::pack::POINT2D ptScreenSize = g_pFramework->GetSreenSize();
		ptScreenSize.x -= 215; ptScreenSize.y -= 65;

		m_pTexKillHistory[0] = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);
		m_pTexKillHistory[1] = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD3);

		for(INT32 i = 0; i < 2; i++)
		{
			m_pSprKillHistory[i] = i3Sprite2D::new_object();
			m_pSprKillHistory[i]->SetTexture(m_pTexKillHistory[i]);
			m_pSprKillHistory[i]->Create(CK::MAX_BUFFER, true, true);

			for(INT32 j = 0; j < CK::MAX_BUFFER; j++)
			{
				m_pSprKillHistory[i]->SetColor(j, 255, 255, 255, 255);

				m_pSprKillHistory[i]->SetTextureCoord(i, 138.0f, 340.0f, 138.0f + 52.0f, 340.0f + 56.0f);
				m_pSprKillHistory[i]->SetRect(j, (REAL32)ptScreenSize.x, (REAL32)ptScreenSize.y, 52.0f, 56.0f);
				m_pSprKillHistory[i]->SetEnable(j, false);
			}
			GetNode()->AddChild(m_pSprKillHistory[i]);
		}
	}


	// TextNode 생성
	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		m_pTextBonusPoint[i] = i3TextNodeDX2::new_object();
		m_pTextBonusPoint[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 18, FW_NORMAL, false, false, ANTIALIASED_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextBonusPoint[i]->SetColor(220, 220, 220, 255);
		m_pTextBonusPoint[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextBonusPoint[i]->setAutoWrapEnable(false);
		m_pTextBonusPoint[i]->SetEnable(false);

		GetNode()->AddChild(m_pTextBonusPoint[i]);
	}		

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		m_pTextKillCount[i] = i3TextNodeDX2::new_object();
		m_pTextKillCount[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 25, FW_NORMAL, false, false, ANTIALIASED_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextKillCount[i]->SetColor(200, 200, 200, 255);
		m_pTextKillCount[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_BOTTOM);
		m_pTextKillCount[i]->setAutoWrapEnable(false);
		m_pTextKillCount[i]->setSize(52, 60/*56*/);
		m_pTextKillCount[i]->SetEnable(false);

		GetNode()->AddChild(m_pTextKillCount[i]);
	}

	m_pTextTrainingMessage = i3TextNodeDX2::new_object();
	m_pTextTrainingMessage->Create(GetDefaultFontName(), FONT_COUNT_256, 30, FW_NORMAL, false, false, ANTIALIASED_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pTextTrainingMessage->SetColor(220, 220, 220, 255);
	m_pTextTrainingMessage->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTextTrainingMessage->setAutoWrapEnable(false);

	i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(400, 380);
	m_pTextTrainingMessage->setPos(ptCenter.x, ptCenter.y);
	m_pTextTrainingMessage->setSize(400, 50);
	m_pTextTrainingMessage->SetEnable(false);
	m_pTextTrainingMessage->SetText(GAME_RCSTRING("STR_TBL_INGAME_HUD_TRAINING_DIFFICULTY_UP"));

	GetNode()->AddChild(m_pTextTrainingMessage);

	m_pTextRevengeMessage = i3TextNodeDX2::new_object();
	m_pTextRevengeMessage->Create(GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_SIZE_HUD_REVENGE, FW_NORMAL, false, false, ANTIALIASED_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
	m_pTextRevengeMessage->SetColor(220, 220, 220, 255);
	m_pTextRevengeMessage->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	m_pTextRevengeMessage->setAutoWrapEnable(false);

	ptCenter = g_pFramework->GetCenterPosition(400, 380);
	m_pTextRevengeMessage->setPos(ptCenter.x, ptCenter.y + 20); // 약간 밑으로..
	m_pTextRevengeMessage->setSize(400, 50);
	m_pTextRevengeMessage->SetEnable(false);

	GetNode()->AddChild(m_pTextRevengeMessage);


	i3Viewer * pViewer		= g_pFramework->GetViewer();

	INT32 nPosX = pViewer->GetViewWidth() - 175;
	INT32 nPosY = pViewer->GetViewHeight() - 104;

	// 장비 (헬멧)
	m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);

	m_pSprite = i3Sprite2D::new_object();
	GetNode()->AddChild(m_pSprite);

	m_pSprite->SetTexture( m_pTex );
	m_pSprite->Create( HUD_MAX, true, true);

	// 장비 (헬멧)
	m_pSprite->SetEnable( HUD_PROTECTION, false );
	m_pSprite->SetRect( HUD_PROTECTION, (REAL32)(nPosX + 115), (REAL32)(nPosY - 47), 46.f, 46.f);
	m_pSprite->SetColor( HUD_PROTECTION, 255, 255, 255, 255 );
	m_pSprite->SetTextureCoord( HUD_PROTECTION, 463.f, 323.f, 509.f, 369.f );

	// 장비 (C5)
	m_pSprite->SetEnable( HUD_C5,	false);
	m_pSprite->SetRect( HUD_C5, (REAL32) (nPosX + 115), (REAL32) (nPosY - 96), 46.f, 46.f);
	m_pSprite->SetColor( HUD_C5, 255, 255, 255, 255);
	m_pSprite->SetTextureCoord( HUD_C5, 413.f, 359.f, 458.f, 403.f);

	return true;	
}

void CChainKills::OnUpdate(REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	Process_KillMessage_Queue();

	_UpdateKillMessage(rDeltaSeconds);
	_UpdateBonusPoint(rDeltaSeconds);
	_UpdateKillHistory(rDeltaSeconds);
	_UpdateLoveShotMessage(rDeltaSeconds);
	_UpdateHelmetProtection(rDeltaSeconds);
	_UpdateMissionMessage(rDeltaSeconds);
	_UpdateC5(rDeltaSeconds);
}

void CChainKills::_UpdateHelmetProtection(REAL32 rDeltaSeconds)
{
	CGameCharaBase* myChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( myChara == nullptr || CHud::i() == nullptr ||
		(CHud::i()->getHudMode() == HUD_MODE_MISSION_OBSERVER) )
		return;

	if( myChara->IsDino() == false)
	{
		if( UserContext::i()->IsPlayingSlot( BattleSlotContext::i()->getMySlotIdx()) &&
			!myChara->isCharaStateMask( CHARA_STATE_DEATH))
		{
			if( myChara->isAttachedProtectParts() )
			{
				// 장비 (헬멧)
				if( !m_bUpdateHelmet )
				{
					//m_pSprite->SetEnable( HUD_PROTECTION, true );
					m_pSprite->SetTextureCoord( HUD_PROTECTION,  463.f, 323.f, 509.f, 369.f );
					m_bUpdateHelmet = true;
				}
			}
			else
			{
				// 장비 (헬멧)
				if( m_bUpdateHelmet && myChara->getEquipContext()->getHelmetProtect() )
				{
					//m_pSprite->SetEnable( HUD_PROTECTION, true );
					m_pSprite->SetTextureCoord( HUD_PROTECTION, 463.f, 373.f, 509.f, 419.f );
					m_bUpdateHelmet = false;

					// 헬멧 프로텍션
					CHud::i()->AddHelmetProtection();
				}
			}
		}
	}
}

void CChainKills::_UpdateC5(REAL32 rDeltaSeconds)
{
#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
	{
		I3TRACE( "isObserverMe : call UpdateEquipC5....\n");
		return;
	}
#endif

	if(CHud::i() == nullptr || (CHud::i()->getHudMode() == HUD_MODE_MISSION_OBSERVER))
		return;

	CGameCharaBase* myChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( myChara == nullptr || myChara->IsDino() == true)
		return;

	if( !myChara->isCharaStateMask( CHARA_STATE_DEATH))
	{
		CGameCharaWeaponContext * pCtx = myChara->getWeaponContext();
		if( pCtx == nullptr )		return;

		if( pCtx->getWeapon( WEAPON_SLOT_THROWING1) == nullptr)	return;

		if( i3::same_of<WeaponBombTrigger*>(pCtx->getWeapon( WEAPON_SLOT_THROWING1)) )
		{	
			WeaponBombTrigger * pBombTrigger = (WeaponBombTrigger*)pCtx->getWeapon( WEAPON_SLOT_THROWING1);

			bool bEnable = false;

			if( i3::same_of<WeaponC5*>(pBombTrigger->getBomb()))
			{
				for( INT32 i = 0; i < pBombTrigger->getDropedBombCount(); i++)
				{
					WeaponC5 * pC5 = (WeaponC5*) pBombTrigger->getDropedBomb( i);
					
					MainWeapon_C5* mainWeapon_C5 = static_cast<MainWeapon_C5*>(pC5->GetMainWeapon());

					if( pC5->isEnable() && mainWeapon_C5->isBombState( WEAPON_BOMBSTATE_SETTLED) )
					{	
						bEnable = true;
						break;
					}
				}

				if( pCtx->getCurrentSlotNum() == WEAPON_SLOT_THROWING1)
				{	
					if( bEnable )
					{
						if( m_bUpdateC5)
						{
							//m_pSprite->SetEnable( HUD_C5, true);
							m_pSprite->SetTextureCoord( HUD_C5, 413.f, 359.f, 458.f, 405.f);
							m_bUpdateC5 = false;
						}
					}
					else
					{
						if( !m_bUpdateC5)
						{
							//m_pSprite->SetEnable( HUD_C5, true);
							m_pSprite->SetTextureCoord( HUD_C5,	413.f, 407.f, 458.f, 453.f );
							m_bUpdateC5 = true;
						}
					}
				}
				else if( !bEnable )
				{
					m_pSprite->SetEnable( HUD_C5, false);
					m_bUpdateC5 = false;
				}
			}
		}
	}
}


void CChainKills::SetEnableWithoutNotice( bool bFlag )
{
	CGameCharaBase* myChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( myChara != nullptr)
	{
		if( myChara->IsDino() == true)
		{
			bFlag = false;
		}
		else
		{
			CGameCharaWeaponContext * pCtx = myChara->getWeaponContext();
			if( pCtx == nullptr )
				bFlag = false;
			else if( bFlag && pCtx->getWeapon( pCtx->getCurrentSlotNum()) != nullptr)
			{
				if( i3::same_of<WeaponBombTrigger*>(pCtx->getWeapon( pCtx->getCurrentSlotNum())))
					m_pSprite->SetEnable( HUD_C5, true);
				else
					m_pSprite->SetEnable( HUD_C5, false);
			}
		}
	}
	else
		bFlag = false;

	if( bFlag == false )
	{
		m_pSprite->SetEnable( HUD_PROTECTION, false );
		m_pSprite->SetEnable( HUD_C5, false);
	}
}
//Key:EscapeMission 킬메시지를 설정합니다.

void CChainKills::AddTouchDown(INT16 nNowTouchDown, INT32 nSlotIdx)
{
	if (!MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)) return;

	SKillMessage kmBuffer;
	kmBuffer.Reset();
	kmBuffer._KillMessageType = CK_TOUCHDOWN;

	INT32 nHistoryType = CK_HISTORY_TOUCHDOWN;	// revision 57501 [공통]탈출모드 공룡으로 킬 시 우측하단에 탈출아이콘이 생성됨 (hansoft.5114)

	// nNowKill이 1일 경우에만 들어온다
	if (4 <= nNowTouchDown)
	{
		kmBuffer._KillMessageType	= CK_GAMEMAKER;
	}
	else if (3 == nNowTouchDown)
	{
		kmBuffer._KillMessageType	= CK_HATTRICK;
	}
	else if (2 == nNowTouchDown)
	{
		kmBuffer._KillMessageType	= CK_ACEATTACKER;
	}
	else if (1 == nNowTouchDown)
	{
		kmBuffer._KillMessageType	= CK_TOUCHDOWN;
	}
	g_pSndMng->StartAnnounceMessage( GTA_TRAINING_LEVELUP);

	_PushKillMessage(&kmBuffer);
	_PushHistory(nHistoryType, nNowTouchDown);
}


void CChainKills::Set_Kill_Message(SKillMessage & kmBuffer, INT32 nKillMessage, INT32 nPreKillCount)
{
	// KillMessage Type
	// 우선순위가 높은 것이 위로 와야 합니다. 순서 중요 순구
	// revision 59507 어시스트킬, 인터셉터 킬메세지 안뜨는 버그 임시 수정
	if( nKillMessage & KILLING_MESSAGE_INTERCEPT)
	{
		kmBuffer._KillMessageType	= CK_INTERCEPT;
	}
	else if( nKillMessage & KILLING_MESSAGE_ASSISTKILL )
	{
		kmBuffer._KillMessageType	= CK_ASSISTKILL;
	}
	else if( nKillMessage & KILLING_MESSAGE_OBJECTDEFENCE)
	{
		kmBuffer._KillMessageType	= CK_OBJECT_DEFENCE;
	}
	else if (nKillMessage & KILLING_MESSAGE_PIERCINGSHOT)
	{
		kmBuffer._KillMessageType	= CK_PIERCINGSHOT;
	}
	else if (nKillMessage & KILLING_MESSAGE_MASSKILL)
	{
		kmBuffer._KillMessageType	= CK_MASSKILL;
	}
	else if (nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT)
	{
		kmBuffer._KillMessageType	= CK_CHAINHEADSHOT;
	}
	else if (nKillMessage & KILLING_MESSAGE_HEADSHOT)
	{
		kmBuffer._KillMessageType	= CK_HEADSHOT;
	}
	else if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)
	{
		kmBuffer._KillMessageType	= CK_CHAINSLUGGER;
	}
	else if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)
	{
		kmBuffer._KillMessageType	= CK_CHAINSTOPPER;
	}
	else if (nKillMessage & KILLING_MESSAGE_ASSIST)
	{
		kmBuffer._KillMessageType = CK_ASSIST;
	}
	else
	{
		// nNowKill이 1일 경우에만 들어온다
		if(nPreKillCount== 1)
		{
			kmBuffer._KillMessageType	= CK_DOUBLEKILL;
		}
		else if(nPreKillCount== 2)
		{
			kmBuffer._KillMessageType	= CK_TRIPLEKILL;
		}
		else if(nPreKillCount >= 3)
		{
			kmBuffer._KillMessageType	= CK_CHAINKILLER;
		}
	}
}

INT32 CChainKills::Set_Special_Kill_Message(SKillMessage & kmBuffer, INT32 nWeaponSlot)
{
	INT32 special_history = CK_HISTORY_TYPE_NONE;

	switch( nWeaponSlot )
	{
	case WEAPON_SLOT_PRIMARY: //리스폰 된 후 주무기로 첫 번째 쏜 총알로 적을 한번에 처치
		{
			CGameCharaBase* my_chara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx() );
			if( hu::is_primary_weapon_first_shot(my_chara) == true )
			{
				kmBuffer._KillMessageType = CK_ONE_SHOT_ONE_KILL;
				special_history = CK_HISTORY_ONE_SHOT_ONE_KILL;
			}
		}
		break;
	case WEAPON_SLOT_SECONDARY: //권총을 사용하여 적을 처치
		kmBuffer._KillMessageType = CK_SPECIAL_GUNNER;
		special_history = CK_HISTORY_GUNNER;
		break;
	case WEAPON_SLOT_MELEE: //적의 후방에서 근접무기로 적을 처치
		{
			REAL32 killer_theta = m_pKillerChara->getBoneContext()->getTheta();
			REAL32 victim_theta = m_pVictimChara->getBoneContext()->getTheta();

			if( i3Math::abs( victim_theta - killer_theta) < 0.7f )
			{
				kmBuffer._KillMessageType = CK_ASSASSIN;
				special_history = CK_HISTORY_ASSASSIN;
			}
		}
		break;
	case WEAPON_SLOT_THROWING1: //수류탄이나 유탄으로 적을 처치
		if( wop::is_throwing_grenade(m_pKillWeaponInfo) == true )
		{
			kmBuffer._KillMessageType = CK_BOMB_SHOT;
			special_history = CK_HISTORY_BOMB_SHOT;
		}
		break;
	case WEAPON_SLOT_THROWING2: //WP스모크로 적을 처치
		if( wop::is_throwing_wp_smoke(m_pKillWeaponInfo) == true )
		{
			kmBuffer._KillMessageType = CK_GAS_ATTACK;
			special_history = CK_HISTORY_GAS_ATTACK;
		}
		break;
	}

	return special_history;
}

INT32 CChainKills::Set_Revenge_Message(SKillMessage & kmBuffer, INT32 nKillMessage)
{
	INT32 history_mark = CK_HISTORY_TYPE_NONE;

	switch(nKillMessage)
	{
		case REVENGE_TYPE_HOTKILLER:
			kmBuffer._KillMessageType = CK_REVENGE_HOTKILLER;
			history_mark = CK_HISTORY_HOT_KILLER;
			break;
		case REVENGE_TYPE_NIGHTMARE:
			kmBuffer._KillMessageType = CK_REVENGE_NIGHTMARE;
			history_mark = CK_HISTORY_NIGHTMARE;
			break;
		case REVENGE_TYPE_MEATCHOPPER:
			kmBuffer._KillMessageType = CK_REVENGE_MEATCHOPPER;
			history_mark = CK_HISTORY_MEAT_CHOPPER;
			break;
		case REVENGE_TYPE_PAYBACK:
			kmBuffer._KillMessageType = CK_REVENGE_PAYBACK;
			break;
		case REVENGE_TYPE_AVENGER:
			kmBuffer._KillMessageType = CK_REVENGE_AVENGER;
			break;
		case REVENGE_TYPE_STALKER:
			kmBuffer._KillMessageType = CK_REVENGE_STALKER;
			break;
		case REVENGE_TYPE_LOVESHOT:
			kmBuffer._KillMessageType = CK_REVENGE_LOVESHOT;
			break;
		case REVENGE_TYPE_AWESOME:
			kmBuffer._KillMessageType = CK_REVENGE_AWESOME;
			break;
	}

	return history_mark;
}

INT32 CChainKills::Set_Kill_Bonus_Point(SKillMessage & kmBuffer, INT32 nKillMessage, 
										INT32 nPreKillCount, INT32 nNowKill, INT32 nWeaponSlot,
										INT32 nHistoryType)
{
	INT32 nCount = 0;

	// Kill BonusPoint
	if (nKillMessage & (KILLING_MESSAGE_PIERCINGSHOT | KILLING_MESSAGE_MASSKILL))
	{
		kmBuffer._aBonusPoint[nCount] = 20;
		nCount++;
	}

	if (nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT)
	{
		kmBuffer._aBonusPoint[nCount] = 20;
		nCount++;
	}

	if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)
	{
		kmBuffer._aBonusPoint[nCount] = 20;
		nCount++;
	}

	if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)
	{
		kmBuffer._aBonusPoint[nCount] = 40;
		nCount++;
	}

	

	// 킬링 보너스 - 죽인 수 만큼 반복한다
	INT32 nKillBonus = 0;

	switch(nPreKillCount + nNowKill)
	{
	case 0:		/* Do nothing */	break;
	case 1:		/* Do nothing */	break;
	case 2:		nKillBonus = 20;	break;
	case 3:		nKillBonus = 40;	break;
	default:	nKillBonus = 60;	break;
	}

	if (0 != nKillBonus)
	{
		kmBuffer._aBonusPoint[nCount] = nKillBonus;
		nCount++;
	}

	return nCount;
}
INT32 CChainKills :: Set_Kill_Bonus_Score(INT32 nKillMessage, INT32 nPreKillCount, INT32 nCurKillCount, bool RevengeFlag)
{
	m_BonusScore = 0;
	if(!RevengeFlag)
	{
		if (nKillMessage & KILLING_MESSAGE_PIERCINGSHOT)
		{
			m_BonusScore += 1;
		}
		if (nKillMessage & KILLING_MESSAGE_MASSKILL)
		{
			m_BonusScore += 1;
		}
		if (nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT)
		{
			m_BonusScore += 1;
		}
		if (nKillMessage & KILLING_MESSAGE_HEADSHOT)
		{
			m_BonusScore += 1;
		}
		if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)
		{
			m_BonusScore += 1;
		}
		if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)
		{
			m_BonusScore += 2;
		}
		if(nCurKillCount >= 1)
		{
			if(nPreKillCount == 1)
			{
				m_BonusScore += 1;
			}
			else if(nPreKillCount == 2)
			{
				m_BonusScore += 2;
			}
			else if(nPreKillCount >= 3)
			{
				m_BonusScore += 3;
			}
		}
	}
	else
	{
		switch(nKillMessage)
		{
		case REVENGE_TYPE_HOTKILLER:
		case REVENGE_TYPE_NIGHTMARE:
		case REVENGE_TYPE_MEATCHOPPER:
		case REVENGE_TYPE_PAYBACK:
		case REVENGE_TYPE_AVENGER:
		case REVENGE_TYPE_STALKER:
		case REVENGE_TYPE_LOVESHOT:
		case REVENGE_TYPE_AWESOME:
			m_BonusScore += 1;
			break;
		}
	}


	return m_BonusScore; 
}
INT32	CChainKills::Get_History_Type(INT32 nKillMessage, INT32 nWeaponSlot)
{
	INT32 ret_history_type = CK_HISTORY_DEFAULT_KILL;

	if (nKillMessage & KILLING_MESSAGE_CHAINHEADSHOT)
	{
		ret_history_type =  CK_HISTORY_HEADSHOT;
	}

	if (nKillMessage & KILLING_MESSAGE_HEADSHOT)
	{
		ret_history_type = CK_HISTORY_HEADSHOT;
	}

	if (nKillMessage & KILLING_MESSAGE_CHAINSLUGGER)
	{
		if (ret_history_type == CK_HISTORY_HEADSHOT)
		{
			ret_history_type = CK_HISTORY_HEADKNIFE;
		}
		else
		{
			ret_history_type = CK_HISTORY_KNIFE;
		}
	}

	if ( ret_history_type == CK_HISTORY_HEADSHOT)
	{
		if (WEAPON_SLOT_MELEE == nWeaponSlot)
		{
			ret_history_type = CK_HISTORY_HEADKNIFE;
		}
	}

	if (CK_HISTORY_DEFAULT_KILL == ret_history_type)
	{
		if (WEAPON_SLOT_MELEE == nWeaponSlot)
		{
			ret_history_type = CK_HISTORY_KNIFE;
		}
	}

	if (nKillMessage & KILLING_MESSAGE_CHAINSTOPPER)
	{
		switch(ret_history_type)
		{
		case CK_HISTORY_HEADSHOT:
			ret_history_type = CK_HISTORY_STOPPER_HEADSHOT;
			break;
		case CK_HISTORY_HEADKNIFE:
			ret_history_type = CK_HISTORY_STOPPER_HEADKNIFE;
			break;
		case CK_HISTORY_KNIFE:
			ret_history_type = CK_HISTORY_STOPPER_KNIFE;
			break;
		default:
			ret_history_type = CK_HISTORY_STOPPER;
			break;
		}
	}

	if (nKillMessage & KILLING_MESSAGE_ASSIST)
	{
		ret_history_type = CK_HISTORY_ASSIST;
	}
	return ret_history_type;
}

void CChainKills::AddChainKills_Normal_Mode(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,
											INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, 
											bool revengeMsg)
{
	// Special > Revenge > Normal 로 하나만 표시된다.
	INT32 HistoryMarkType; 

	SKillMessage kmBuffer;
	kmBuffer.Reset();
	kmBuffer._BonusPointCount = 0;
	kmBuffer._KillMessageType = CK_TYPE_NONE;

	HistoryMarkType = Set_Special_Kill_Message(kmBuffer, nWeaponSlot);

	// Special Kill Message가 없을 경우(주무기 제외한 무기 or One Shot One Kill) 
	if( kmBuffer._KillMessageType != CK_TYPE_NONE)
	{
		if( HistoryMarkType == CK_HISTORY_GAS_ATTACK || HistoryMarkType == CK_HISTORY_BOMB_SHOT )
			Push_KillMessage_Queue( kmBuffer, HistoryMarkType, nNowKill );
		else
			Push_KillMessage_Queue( kmBuffer, HistoryMarkType, 0 ); // <- 일반적인 주무기
	}

	kmBuffer._KillMessageType = CK_TYPE_NONE;

	if(revengeMsg == true)
	{
		HistoryMarkType = Set_Revenge_Message(kmBuffer, nKillMessage);
	}
	else // 복수 시스템 메세지가 아니고 nKillMessage에 정보가 있을 경우
	{
		Set_Kill_Message(kmBuffer, nKillMessage, nPreKillCount);
		HistoryMarkType = Get_History_Type(nKillMessage, nWeaponSlot);

		if (kmBuffer._KillMessageType != CK_TYPE_NONE)
		{
			if ( MyRoomInfoContext::i()->getStage()->IsAiMode() )
			{
				kmBuffer._BonusPointCount = 0;
			}
			else
			{
				INT32 nCount = Set_Kill_Bonus_Point(kmBuffer, nKillMessage, nPreKillCount, 
					nNowKill, nWeaponSlot, HistoryMarkType);
				kmBuffer._BonusPointCount = nCount;
			}

			if (kmBuffer._KillMessageType == CK_ASSIST)
				g_pSndMng->StartAnnounceMessage( GTA_REVENGE_EFFECT);
			else
				g_pSndMng->StartAnnounceMessage( kmBuffer._KillMessageType);
		}
	}
	
	// 킬에 의한 보너스 스코어 추가
	Set_Kill_Bonus_Score(nKillMessage,nPreKillCount, nNowKill, revengeMsg);

	// Set_Kill_Message에서 셋팅한 킬 메세지
	Push_KillMessage_Queue( kmBuffer, HistoryMarkType, nNowKill );
}

void CChainKills::AddChainKills_Boss_Mission_Mode(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg)
{
	SKillMessage kmBuffer;
	kmBuffer.Reset();
	kmBuffer._KillMessageType = CK_DEFAULT_KILL;

	if (  MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )
	{	
		if( nKillMessage & KILLING_MESSAGE_INTERCEPT)
		{
			kmBuffer._KillMessageType	= CK_INTERCEPT;
		}
		else if( nKillMessage & KILLING_MESSAGE_ASSISTKILL)
		{
			kmBuffer._KillMessageType	= CK_ASSISTKILL;
		}
		else if (3 <= nPreKillCount)
		{
			kmBuffer._KillMessageType	= CK_CHAINKILLER;
		}
		else if (2 == nPreKillCount)
		{
			kmBuffer._KillMessageType	= CK_TRIPLEKILL;
		}
		else if (1 == nPreKillCount)
		{
			kmBuffer._KillMessageType	= CK_DOUBLEKILL;
		}
		else if (nKillMessage & KILLING_MESSAGE_HEADSHOT)			// revision 54162 탈출모드 공룡 헤드샷 추가 ( 20130225 유영정 )
		{
			kmBuffer._KillMessageType	= CK_HEADSHOT;
		}
		else
		{
			kmBuffer._KillMessageType	= CK_DEFAULT_KILL;
		}
		g_pSndMng->StartAnnounceMessage( kmBuffer._KillMessageType);

		if (CK_DEFAULT_KILL != kmBuffer._KillMessageType)
		{
			_PushKillMessage(&kmBuffer);
		}
	}
}

void CChainKills::AddChainKills(INT32 nKillMessage,INT32 nPreKillCount,INT32 nNowKill,INT32 nWeaponSlot,INT32 chainHeadshotCount,INT32 chainStopperCount, bool revengeMsg)
{
	if( nKillMessage == -1)	return;
	//if (m_difficultyLevelUpEnable) return;

	// revision 56421 탈출 미션도 일반 모드와 동일한 Kill Message 사용. hansoft.4274
//	pf_add_chain_kill pf = pf_add_chain_kil_listl[ 0];//MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ];
	// chunjong.song 2014.3.26 - hansoft 7970 이슈요청에따라 탈출미션에서 킬마크 출력안되도록 되돌림
	INT32 procedureIndex = 0;
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ) procedureIndex = 1;

	pf_add_chain_kill pf = pf_add_chain_kil_listl[procedureIndex];

	(this->*pf)(nKillMessage, nPreKillCount, nNowKill, 
		nWeaponSlot, chainHeadshotCount, chainStopperCount, 
		revengeMsg);
}

void CChainKills::AddHelmetProtection(void)
{
	AddLargeMessage(CK_HELMET_PROTECTION);

	g_pSndMng->StartAnnounceMessage( GTA_MISSION_HELMETPROTECTION);
}

void CChainKills::AddTargetBonus(void)
{
	AddLargeMessage(CK_TARGETBONUS);
}

void CChainKills::AddLargeMessage(CK_TYPE ckType)
{
	//if (m_difficultyLevelUpEnable)
	//{
	//	return;
	//}

	SKillMessage kmBuffer;

	kmBuffer._KillMessageType = ckType;
	kmBuffer._BonusPointCount = 0;

	_PushKillMessage(&kmBuffer);
}

void CChainKills::AddChallengeClear(void)
{
	//if (m_difficultyLevelUpEnable)
	//{
	//	return;
	//}

	SKillMessage kmBuffer;

	kmBuffer._KillMessageType = CK_CHALLENGE_CLEAR;
	kmBuffer._BonusPointCount = 0;

	_PushKillMessage(&kmBuffer);
}

void CChainKills::AddTrainingKills(INT32 killCount)
{
	SKillMessage kmBuffer;

	switch(killCount)
	{
	case 20:	kmBuffer._KillMessageType = CK_TRAINING_20_KILL;	break;
	case 40:	kmBuffer._KillMessageType = CK_TRAINING_40_KILL;	break;
	case 60:	kmBuffer._KillMessageType = CK_TRAINING_60_KILL;	break;
	case 80:	kmBuffer._KillMessageType = CK_TRAINING_80_KILL;	break;
	case 100:	kmBuffer._KillMessageType = CK_TRAINING_100_KILL;	break;
	case 120:	kmBuffer._KillMessageType = CK_TRAINING_120_KILL;	break;
	case 140:	kmBuffer._KillMessageType = CK_TRAINING_140_KILL;	break;
	case 160:	kmBuffer._KillMessageType = CK_TRAINING_160_KILL;	break;
	case 180:	kmBuffer._KillMessageType = CK_TRAINING_180_KILL;	break;
	case 200:	kmBuffer._KillMessageType = CK_TRAINING_200_KILL;	break;
	default:
		kmBuffer._KillMessageType = CK_TRAINING_20_KILL;
		break;
	}

	kmBuffer._BonusPointCount = 0;

	if (_PushKillMessage(&kmBuffer))
	{
		//m_difficultyLevelUpEnable = true;

		if (10 == AIModeContext::i()->CurrentLevel)
		{
			m_levelUpTextEnable = false;
		}
		else
		{
			m_levelUpTextEnable = true;
			g_pSndMng->StartAnnounceMessage(GTA_TRAINING_LEVELUP);
		}
	}
}

void CChainKills::Reset(void)
{
	m_finish_kill_mark = m_finish_kill_msg = true;
	m_skill_message_que.clear();

	m_nFrontKillMessage = 0;
	m_nRearKillMessage = 0;
	m_nCountKillMessage = 0;

	m_nStartHistory = 0;
	m_nCountHistory = 0;
	m_DisplayChainkiller = 0;

	m_rKillMessageTerm = 0.0f;
	m_rMedalMessageTerm = 0.0f;

	if (m_pSprKillHistoryBG)
		m_pSprKillHistoryBG->SetEnable( false);

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		m_CkStep[i] = 0;
	}
}

bool CChainKills::_PushKillMessage(SKillMessage* pKillMessage)
{
	I3ASSERT(pKillMessage);	

	// 환경설정 텍스트 이펙트가 비활성화 되어 있다면 처리하지 않는다
	if (false == g_pEnvSet->m_bTextEffect) return true;

	if (CK_MAX_MESSAGE_COUNT <= m_nCountKillMessage)
	{
		I3TRACE("[WARNING] CChainKills::_PushKillMessage() - Skip Kill Message.\n");
		return false;
	}

	m_qKillMessage[m_nRearKillMessage]._KillMessageType = pKillMessage->_KillMessageType;
	m_qKillMessage[m_nRearKillMessage]._BonusPointCount = pKillMessage->_BonusPointCount;
	i3mem::Copy(m_qKillMessage[m_nRearKillMessage]._aBonusPoint, pKillMessage->_aBonusPoint, sizeof(INT32) * CK::MAX_BUFFER);

	m_nRearKillMessage = (m_nRearKillMessage + 1) % CK::MAX_BUFFER;
	m_nCountKillMessage++;

	return true;
}

bool CChainKills::_PopKillMessage(SKillMessage* pKillMessage)
{
	I3ASSERT(pKillMessage);

	if (0 == m_nCountKillMessage)
	{
		return false;
	}

	pKillMessage->_KillMessageType = m_qKillMessage[m_nFrontKillMessage]._KillMessageType;
	pKillMessage->_BonusPointCount = m_qKillMessage[m_nFrontKillMessage]._BonusPointCount;
	i3mem::Copy(pKillMessage->_aBonusPoint, m_qKillMessage[m_nFrontKillMessage]._aBonusPoint, sizeof(INT32) * CK::MAX_BUFFER);


	m_nFrontKillMessage = (m_nFrontKillMessage + 1) % CK::MAX_BUFFER;
	m_nCountKillMessage--;

	return true;
}

void CChainKills::_UpdateLoveShotMessage(REAL32 rDeltaSeconds)
{
	CRevengeManager::i()->OnUpdate(rDeltaSeconds);
}

void CChainKills::_UpdateMissionMessage(REAL32 rDeltaSeconds)
{
	/*	기획팀 요청으로 메달 진행상황 플로팅 보드 사용하지 않습니다.
	m_rMedalMessageTerm -= rDeltaSeconds;
	if( m_rMedalMessageTerm <= 0.0f)
	{
		MEDAL_STATE pData;
		if(CMedalManager::i()->getMedalMessageCnt() > 0)
		{
			CMedalManager::i()->getMedalMessageFront(&pData);
			HUD::instance()->SetParameter(HUD::FLOAT_BOARD, (INT32)&pData);
			m_rMedalMessageTerm = 2.0f;
		}
	}
	if(m_rMedalMessageTerm < 0.0f)
		m_rMedalMessageTerm = -1.0f;
	*/

	// 인게임 메달획득 이펙트 처리입니다.
	{
		MedalCompleteInfo pData;
		UIHudQuestEffect* pHud = static_cast<UIHudQuestEffect*>( HUD::instance()->GetHud(HUD::QUEST_EFFECT) );
		if(CMedalManager::i()->getCompleteMedalCnt() > 0 && pHud->GetEffectState() == QE::NONE )
		{
			CMedalManager::i()->getCompleteMedalFront(&pData);
			HUD::instance()->SetParameter(HUD::QUEST_EFFECT, (INT32)&pData);
		}
	}
}



//- _UpdateKillMessage ------------------//

void CChainKills::_FadeOutKillMessage(REAL32 rDeltaSeconds)
{
	//EscapeMission AssistKill관련 페이드아웃
	if (false == m_pSprKillMessage->GetEnable(0) && false == m_pSprKillMessage1->GetEnable(0) && false == m_pSprKillMessage2->GetEnable(0))
	{
		if ( !MyRoomInfoContext::i()->getStage()->IsAiMode() || 
			 !m_pSprTrainingKillMessage->GetEnable(0) )
		{
			return;
		}
	}

	COLOR* pColor = 0;
	if (m_pSprKillMessage->GetEnable(0))
	{
		pColor = m_pSprKillMessage->GetColor(0);
	}
	else if (m_pSprKillMessage1->GetEnable(0))
	{
		pColor = m_pSprKillMessage1->GetColor(0);
	}
	else if (m_pSprKillMessage2->GetEnable(0))
	{
		pColor = m_pSprKillMessage2->GetColor(0);
	}
	else if (MyRoomInfoContext::i()->getStage()->IsAiMode())
	{
		pColor = m_pSprTrainingKillMessage->GetColor(0);
	}

	if(pColor->a > 0)
	{
		REAL32 showTime;
		if (m_pSprKillMessage->GetEnable(0) || m_pSprKillMessage1->GetEnable(0) || m_pSprKillMessage2->GetEnable(0))
		{
			showTime = CK_TIME_SHOW_MESSAGE / 3.f;
		}
		else
		{
			showTime = CK_TRAINING_TIME_SHOW_MESSAGE;
		}		

		INT32 nAlpha = pColor->a;
		nAlpha = i3Math::Max(0, INT32(nAlpha - (255 * rDeltaSeconds / showTime)));
		pColor->a = (UINT8)nAlpha;

		if (m_pSprKillMessage->GetEnable(0))
		{
			m_pSprKillMessage->SetColor(pColor);
			if(m_pTextRevengeMessage->GetEnable() == true)
				m_pTextRevengeMessage->SetShadowAlpha(pColor->a);
		}
		else if (m_pSprKillMessage1->GetEnable(0))
		{
			m_pSprKillMessage1->SetColor(pColor);
			if(m_pTextRevengeMessage->GetEnable() == true)
				m_pTextRevengeMessage->SetShadowAlpha(pColor->a);
		}
		else if (m_pSprKillMessage2->GetEnable(0))
		{
			m_pSprKillMessage2->SetColor(pColor);
			if (m_pTextRevengeMessage->GetEnable() == true)
				m_pTextRevengeMessage->SetShadowAlpha(pColor->a);
		}
		else
		{
			if ( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
			{
				m_pSprTrainingKillMessage->SetColor(pColor);
				m_pTextTrainingMessage->SetColor(220, 220, 220, pColor->a);
				m_pTextTrainingMessage->SetShadowAlpha(pColor->a);
			}
		}

		for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
		{
			m_pTextBonusPoint[i]->SetColor(220, 220, 220, pColor->a);		
		}
	}
	else
	{
		m_finish_kill_msg = true;
		m_pSprKillMessage->SetEnable(0, false);
		m_pSprKillMessage1->SetEnable(0, false);
		m_pSprKillMessage2->SetEnable(0, false);

		if ( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
			m_pSprTrainingKillMessage->SetEnable(0, false);		

		m_rKillMessageTerm = 0.0f;

		for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
		{
			m_pTextBonusPoint[i]->SetColor(220, 220, 220, 255);
			m_pTextBonusPoint[i]->SetEnable(false);
		}

		m_pTextTrainingMessage->SetColor(220, 220, 220, 255);
		m_pTextTrainingMessage->SetShadowAlpha(255);
		m_pTextTrainingMessage->SetEnable(false);

		m_pTextRevengeMessage->SetColor(220,220,220,255);
		m_pTextRevengeMessage->SetShadowAlpha(255);
		m_pTextRevengeMessage->SetEnable(false);

		m_nBonusPointCount = 0;
	}
}

void CChainKills::_Reset_Sprite_Value()
{
	m_pSprKillMessage->SetEnable(0, false);
	m_pSprKillMessage1->SetEnable(0, false);
	m_pSprKillMessage2->SetEnable(0, false);

	if (MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE))
		m_pSprTrainingKillMessage->SetEnable(0, false);

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
	{
		m_pTextBonusPoint[i]->SetEnable(false);
	}

	m_pTextTrainingMessage->SetEnable(false);
	m_pTextRevengeMessage->SetEnable(false);

	m_nBonusPointCount = 0;
}

void CChainKills::_Set_Sprite_KillMessage(const SKillMessage& sm)
{
	REAL32 rOffsetX = sm._KillMessageType % 2 * 512.0f;
	REAL32 rOffsetY = (INT32)((sm._KillMessageType % 20) / 2) * 102.0f;

	I3TRACE("SetSprite Offset X: %f, Y: %f\n", rOffsetX, rOffsetY);

	// 3번째 페이지 NormalType MAX값 검사 필요
	if (sm._KillMessageType > CK_REVENGE_AWESOME)
	{
		m_pSprKillMessage2->SetTextureCoord(0, rOffsetX, rOffsetY, rOffsetX + 512.0f, rOffsetY + 102.0f);
		m_pSprKillMessage2->SetColor(0, 255, 255, 255, 255);
		m_pSprKillMessage2->SetEnable(0, true);
	} // 2번째 페이지
	else if (sm._KillMessageType > CK_GAMEMAKER)
	{
		m_pSprKillMessage1->SetTextureCoord(0, rOffsetX, rOffsetY, rOffsetX + 512.0f, rOffsetY + 102.0f);
		m_pSprKillMessage1->SetColor(0, 255, 255, 255, 255);
		m_pSprKillMessage1->SetEnable(0, true);
	} // 첫번째 페이지
	else
	{
		m_pSprKillMessage->SetTextureCoord(0, rOffsetX, rOffsetY, rOffsetX + 512.0f, rOffsetY + 102.0f);
		m_pSprKillMessage->SetColor(0, 255, 255, 255, 255);		
		m_pSprKillMessage->SetEnable(0, true);
	}

	m_rMessageScale = 2.5f;
}

void CChainKills::_Set_Sprite_TrainingMessage(const SKillMessage& sm)
{
	if( !MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) ) return;
	if( sm._KillMessageType == MAX_CK ) return;

	REAL32 rOffsetY = (sm._KillMessageType - CK_TRAINING_20_KILL) * 95.0f;
	m_pSprTrainingKillMessage->SetTextureCoord(0, 0.0f, rOffsetY, 513.0f, rOffsetY + 95.0f);
	m_pSprTrainingKillMessage->SetColor(0, 255, 255, 255, 255);
	m_pSprTrainingKillMessage->SetEnable(0, true);
	//m_difficultyLevelUpEnable = true;
	m_rMessageScale = 1.0f;

	m_pTextTrainingMessage->SetColor(220, 220, 220, 255);
	m_pTextTrainingMessage->SetShadowAlpha(255);
	if (m_levelUpTextEnable)
	{
		m_pTextTrainingMessage->SetEnable(true);
	}
}

void CChainKills::_Set_Sprite_BonusPoint(const SKillMessage& sm)
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) ) return; //EscapeMission : 공룡전은 따로 추가 경험치에 따른 처리를 하지 않습니다.

	for(INT32 i = 0; i < sm._BonusPointCount; i++)
	{
		i3::stack_wstring wstrBuf;
		i3::sprintf(wstrBuf, L"+%d%%", sm._aBonusPoint[i]);
		m_pTextBonusPoint[i]->SetText(wstrBuf);
	}
	_StartShowBonusPoint(sm._BonusPointCount);
}

void CChainKills::_Set_Sprite_RevengeMessage(const SKillMessage& sm)
{
	CRevengeManager* pRevenge = CRevengeManager::i();
	m_pTextRevengeMessage->SetText(pRevenge->getRevengeMessage(sm._KillMessageType));
	g_pSndMng->StartAnnounceMessage( GTA_REVENGE_EFFECT);
	_StartRevengeMessage();
}

void CChainKills::_Resize_Sprite_KillMessage(REAL32 rDeltaSeconds)
{
	// 크기 적용
	i3::pack::POINT2D ptCenter;
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL) )
	{
		ptCenter = g_pFramework->GetCenterPosition(490, 412); // 튜토리얼 모드에서는 조금 더 밑에 그린다.다른 ui와 충돌 때문
	}
	else
	{
		ptCenter = g_pFramework->GetCenterPosition(490, 512);
	}

	// revision 59507 어시스트킬, 인터셉터 킬메세지 안뜨는 버그 임시 수정
	if( m_qKillMessage->_KillMessageType == CK_ASSISTKILL  || m_qKillMessage->_KillMessageType == CK_INTERCEPT)
	{
		m_pSprKillMessage->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
		m_pSprKillMessage1->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
		m_pSprKillMessage2->SetRect(0, (REAL32)ptCenter.x, (REAL32)ptCenter.y, 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
	}
	else
	{
		m_pSprKillMessage->SetRect(0, ptCenter.x - ((512.0f * m_rMessageScale - 512.0f) * 0.5f), ptCenter.y - ((102.0f * m_rMessageScale - 102.0f) * 0.5f), 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
		m_pSprKillMessage1->SetRect(0, ptCenter.x - ((512.0f * m_rMessageScale - 512.0f) * 0.5f), ptCenter.y - ((102.0f * m_rMessageScale - 102.0f) * 0.5f), 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
		m_pSprKillMessage2->SetRect(0, ptCenter.x - ((512.0f * m_rMessageScale - 512.0f) * 0.5f), ptCenter.y - ((102.0f * m_rMessageScale - 102.0f) * 0.5f), 512.0f * m_rMessageScale, 102.0f * m_rMessageScale);
	}

	ptCenter = g_pFramework->GetCenterPosition(380, 512);	
	if ( MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE) )
	{
		m_pSprTrainingKillMessage->SetRect(0, ptCenter.x - ((400.0f * m_rMessageScale - 400.0f) * 0.5f), ptCenter.y - ((70.0f * m_rMessageScale - 70.0f) * 0.5f), 400.0f * m_rMessageScale, 70.0f * m_rMessageScale);
	}
	if (m_pSprKillMessage->GetEnable(0) || m_pSprKillMessage1->GetEnable(0) || m_pSprKillMessage2->GetEnable(0))
	{
		m_rMessageScale = MAX(1.0f, m_rMessageScale - (2.5f * rDeltaSeconds / 0.1f));
	}
	else
	{
		m_rMessageScale = MIN(1.5f, m_rMessageScale + (1.5f * rDeltaSeconds / 0.1f));
	}	
}

namespace inner
{
	struct Fn
	{
		bool operator()(const CK::Context & p) const
		{
			return p.msg._KillMessageType == CK_ONE_SHOT_ONE_KILL;
		}
	};
}

void CChainKills::Push_KillMessage_Queue(const SKillMessage & msg, INT32 mark, INT32 count)
{
	//AddChainKills 함수가 두번 연속 호출 되기 때문에.. 같은 조건으로 원샷 원킬이 된다.
	//그래서 원샷 원킬을 한번만 들어 오도록 한다.
	if(msg._KillMessageType == CK_ONE_SHOT_ONE_KILL)
	{
		std::deque< CK::Context >::iterator it = std::find_if(m_skill_message_que.begin(), m_skill_message_que.end(), inner::Fn());
		if(it != m_skill_message_que.end()) return;
	}

	CK::Context con;
	con.msg = msg;
	con.mark = mark;
	con.kill_count = count;

	m_skill_message_que.push_back( con );
}

void CChainKills::Process_KillMessage_Queue()
{
	//텍스트 이펙트 옵션이 꺼져있으면 킬 메세지가 출력될 일이 없기때문에 항상 true
	if (false == g_pEnvSet->m_bTextEffect)
		m_finish_kill_msg = true;

	if( m_finish_kill_msg == false || m_finish_kill_mark == false ) return;

	if( m_skill_message_que.empty() == true ) return;

	const CK::Context & con = m_skill_message_que.front();

	if( con.msg._KillMessageType != CK_TYPE_NONE )
	{
		_PushKillMessage( const_cast<SKillMessage*>(&con.msg) );
		m_finish_kill_msg = false;
	}
	
	//mark 표시 유무에 따라 m_finish_kill_mark 를 설정한다.
	m_finish_kill_mark = !_PushHistory(con.mark, con.kill_count);

	m_skill_message_que.pop_front();
}


void CChainKills::_UpdateKillMessage(REAL32 rDeltaSeconds)
{
	m_rKillMessageTerm -= rDeltaSeconds;

	if (m_rKillMessageTerm <= 0.0f)
	{
		_FadeOutKillMessage(rDeltaSeconds);

		SKillMessage sm;
		if( _PopKillMessage(&sm) == true ) // KillMessage가 남았으면 처리
		{
			_Reset_Sprite_Value();

			if( sm._KillMessageType < CK_TRAINING_20_KILL )
			{
				_Set_Sprite_KillMessage(sm);

				if ((sm._KillMessageType >= CK_REVENGE_LOVESHOT && sm._KillMessageType <= CK_REVENGE_STALKER) || sm._KillMessageType == CK_REVENGE_AWESOME)
				{
					_Set_Sprite_RevengeMessage(sm);
				}
				else
				{
					_Set_Sprite_BonusPoint(sm);
				}
			}
			else
			{
				_Set_Sprite_TrainingMessage(sm);
			}

			m_rKillMessageTerm = CK_TIME_SHOW_MESSAGE;			
		}
	}

	_Resize_Sprite_KillMessage(rDeltaSeconds);
}

void CChainKills::_UpdateBonusPoint(REAL32 rDeltaSeconds)
{
	if (0 == m_nBonusPointCount)
	{
		return;
	}

	if (m_nCurrentShowPoint >= m_nBonusPointCount)
	{
		return;
	}

	m_rBonusPointTerm -= rDeltaSeconds;

	if (m_rBonusPointTerm <= 0.0f)
	{
		m_nCurrentShowPoint++;

		if (m_nCurrentShowPoint >= m_nBonusPointCount)
		{
			return;
		}

		i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(-380, 380);

		m_pTextBonusPoint[m_nCurrentShowPoint]->setPos(ptCenter.x, ptCenter.y);
		m_pTextBonusPoint[m_nCurrentShowPoint]->SetColor(220, 220, 220, 255);
		m_pTextBonusPoint[m_nCurrentShowPoint]->SetEnable(true);

		m_rBonusPointTerm = m_rKillMessageTerm / CK_MAX_BONUS_COUNT;	
	}
	else if (0 < m_nCurrentShowPoint)
	{
		i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(-380, 380);

		for(INT32 i = 0; i <= m_nCurrentShowPoint; i++)
		{
			INT32 nTargetX = (m_nCurrentShowPoint - i) * 50;

			nTargetX = INT32(nTargetX + (50 - m_rBonusPointTerm * 50 / (m_rKillMessageTerm / CK_MAX_BONUS_COUNT)));

			m_pTextBonusPoint[i]->setPos(ptCenter.x - nTargetX, ptCenter.y);
		}
	}
}

void CChainKills::_StartShowBonusPoint(INT32 nBonusPointCount)
{
	m_nBonusPointCount = nBonusPointCount;
	m_nCurrentShowPoint = 0;

	if (0 == m_nBonusPointCount)
	{
		return;
	}

	i3::pack::POINT2D ptCenter = g_pFramework->GetCenterPosition(-380, 380);

	ptCenter.x -= 50;

	m_pTextBonusPoint[m_nCurrentShowPoint]->setPos(ptCenter.x, ptCenter.y);
	m_pTextBonusPoint[m_nCurrentShowPoint]->SetColor(220, 220, 220, 255);
	m_pTextBonusPoint[m_nCurrentShowPoint]->SetEnable(true);

	m_rBonusPointTerm = m_rKillMessageTerm / CK_MAX_BONUS_COUNT;
}

void CChainKills::_StartRevengeMessage()
{
	m_pTextRevengeMessage->SetColor(220, 220, 220, 255);
	m_pTextRevengeMessage->SetEnable(true);
}

bool CChainKills::_PushHistory(INT32 ckHistoryType,INT32 nKillCount)
{
	// revision 58594 탈출미션에서 Escape 이외의 킬 히스토리를 추가하려고 하면 바로 return;
// 	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) && !( ckHistoryType & CK_HISTORY_TOUCHDOWN ) )		return false;
	if( ckHistoryType == CK_HISTORY_TYPE_NONE ) return false;

	INT32 idxLast = (m_nStartHistory + m_nCountHistory) % CK::MAX_BUFFER;

	m_qKillHistory[idxLast]._KillHistoryType = ckHistoryType;
	m_qKillHistory[idxLast]._KillCount = nKillCount;

	if (ckHistoryType == CK_HISTORY_ASSIST)
		m_qKillHistory[idxLast]._KillImgIdx = 0;
	else
		m_qKillHistory[idxLast]._KillImgIdx = ckHistoryType / CK_HISTORY_GUNNER;

	if (CK_MAX_HISTORY_COUNT > m_nCountHistory)
	{
		m_nCountHistory++;
	}
	else
	{
		m_nStartHistory = (m_nStartHistory + 1) % CK::MAX_BUFFER;
	}

	m_CkStep[m_nCountHistory-1] = 0;

	return true;
}





/***************************************************/
namespace
{
	REAL32 fGoalX = 0.0f;	REAL32 fGoalY = 0.0f;
	REAL32 fMoveGoalX = 0.0f;	REAL32 fMoveGoalY = 0.0f;

	i3::pack::POINT2D ptScreenSize;

	INT32 Alpha[CK::MAX_BUFFER];

	REAL32 fMoveScaleX[CK::MAX_BUFFER];
	REAL32 fMoveScaleY[CK::MAX_BUFFER];
	INT32 nHitHistoryBGAlpha[CK::MAX_BUFFER];
}

// 첫번째 스텝!
// 체인킬 마크를 화면의 중앙부위에 약간 확대 및 알파를 100%으로 하여 출력합니다.
void CChainKills::_UpdateKillHistory_Step1(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds)
{
	REAL32 fStartX = 0.0f;	REAL32 fStartY = 0.0f;

	fStartX = ptScreenSize.x * 0.5f - 26.0f;
	fStartY = ptScreenSize.y * 0.5f - 140.0f;

	m_pSprKillHistory[img_idx]->SetRect( idx, fStartX, fStartY, CK_SIZE_X, CK_SIZE_Y);
	m_pSprKillHistory[img_idx]->SetColor( idx, 255, 255, 255, 0);
	m_pSprKillHistory[img_idx]->SetSize( idx, 
		CK_SIZE_X * 10.0f * m_fViewerWidthRate, 
		CK_SIZE_Y * 10.0f * m_fViewerHeightRate);	

	nHitHistoryBGAlpha[idx] = 255;
	m_pSprKillHistoryBG->SetRect( 0, fStartX-17.0f, fStartY-17.0f, 80.0f, 80.0f);
	m_pSprKillHistoryBG->SetColor( 0, 255, 255, 255, (UINT8)nHitHistoryBGAlpha[idx]);
	m_pSprKillHistoryBG->SetSize( 0, 80.0f* 0.7f * m_fViewerWidthRate, 80.0f* 0.7f * m_fViewerHeightRate);

	m_CkStep[idx] = 1;
}

// 두번째 스텝!
// 체인킬 마크를 원래 크기로 축소합니다. 이때 알파값을 50%로 합니다.
void CChainKills::_UpdateKillHistory_Step2(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds)
{
	// 사이즈 변경
	VEC2D* pSize = m_pSprKillHistory[img_idx]->GetSize(idx);

	REAL32 fSizeX = i3Vector::GetX(pSize) - (CK_SIZE_X*80.0f * rDeltaSeconds);
	REAL32 fSizeY = i3Vector::GetY(pSize) - (CK_SIZE_Y*80.0f * rDeltaSeconds);

	if( fSizeX < CK_SIZE_X*m_fViewerWidthRate) fSizeX = CK_SIZE_X*m_fViewerWidthRate;
	if( fSizeY < CK_SIZE_Y*m_fViewerHeightRate) fSizeY = CK_SIZE_Y*m_fViewerHeightRate;

	m_pSprKillHistory[img_idx]->SetSize( idx, fSizeX, fSizeY);	

	// 알파 변경			
	COLOR * pcol = m_pSprKillHistory[img_idx]->GetColor(idx);
	UINT32 alpha = pcol->a + (UINT32)(500.0f * rDeltaSeconds);
	if( alpha > 200 ) alpha = 200;
	pcol->a = (UINT8)alpha;

	if( pcol->a > 20)
	{
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE))//EscapeMission:공룡미션은 히스토리에 백그라운드를 입히지 않습니다.
			m_pSprKillHistoryBG->SetEnable( false);
		else
			m_pSprKillHistoryBG->SetEnable( true);
	}

	if( fSizeX == CK_SIZE_X*m_fViewerWidthRate && 
		fSizeY == CK_SIZE_Y*m_fViewerHeightRate && pcol->a == 200)
	{
		m_CkStep[idx] = 2;							
	}

	if( m_pSprKillHistoryBG->GetEnable(0))
	{
		m_pSprKillHistoryBG->SetEnable( true);
		pcol = m_pSprKillHistoryBG->GetColor(0);
		nHitHistoryBGAlpha[idx] = (INT32) i3Color::GetA( pcol);
		nHitHistoryBGAlpha[idx] -= (INT32)(300.0f * rDeltaSeconds);			
		if( nHitHistoryBGAlpha[idx] < 0) nHitHistoryBGAlpha[idx] = 0;
		m_pSprKillHistoryBG->SetColor( 0, 255, 255, 255, (UINT8)nHitHistoryBGAlpha[idx]);

		pSize = m_pSprKillHistoryBG->GetSize(0);
		fSizeX = i3Vector::GetX(pSize) + (80.0f * 3.0f * rDeltaSeconds);
		fSizeY = i3Vector::GetY(pSize) + (80.0f * 3.0f * rDeltaSeconds);
		if( fSizeX > 80.0f*m_fViewerWidthRate) fSizeX = 80.0f*m_fViewerWidthRate;
		if( fSizeY > 80.0f*m_fViewerHeightRate) fSizeY = 80.0f*m_fViewerHeightRate;
		m_pSprKillHistoryBG->SetSize( 0, fSizeX, fSizeY);	
	}
}

// 세번째 스텝!
// 크기가 작아지며, 알파가 빠지면서 원래 있어야 할 위치로 이동합니다.
void CChainKills::_UpdateKillHistory_Step3(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds)
{
	// 사이즈 변경
	VEC2D* pSize = m_pSprKillHistory[img_idx]->GetSize(idx);

	REAL32 fSizeX = i3Vector::GetX(pSize) - (CK_SIZE_X * 5.0f * rDeltaSeconds);
	REAL32 fSizeY = i3Vector::GetY(pSize) - (CK_SIZE_Y * 5.0f * rDeltaSeconds);

	if( fSizeX < 6.0f) fSizeX = 6.0f;
	if( fSizeY < 7.0f) fSizeY = 7.0f;

	// 알파 변경
	COLOR * pcol = m_pSprKillHistory[img_idx]->GetColor(idx);
	INT32 alpha = pcol->a - (INT32)(1100.0f * rDeltaSeconds);
	if( alpha < 0) alpha = 0;
	pcol->a = (UINT8)alpha;	

	if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE)) //EscapeMission:공룡미션은 히스토리에 백그라운드를 입히지 않습니다.
		m_pSprKillHistoryBG->SetEnable( false);
	else
	{
		m_pSprKillHistoryBG->SetEnable( true);
		pcol = m_pSprKillHistoryBG->GetColor(0);
		nHitHistoryBGAlpha[idx] = (INT32) i3Color::GetA( pcol);
		nHitHistoryBGAlpha[idx] -= (INT32)(700.0f * rDeltaSeconds);			
		if( nHitHistoryBGAlpha[idx] < 0) nHitHistoryBGAlpha[idx] = 0;
		m_pSprKillHistoryBG->SetColor( 0, 255, 255, 255, (UINT8)nHitHistoryBGAlpha[idx]);
	}

	if( pcol->a == 0 && fSizeX == 6.0f && fSizeY == 7.0f)
	{
		m_CkStep[idx] = 3;	
		m_pSprKillHistoryBG->SetEnable( false);
	}

	// 위치 이동
	VEC3D vec;
	m_pSprKillHistory[img_idx]->GetLeftTop( idx, &vec);

	REAL32 fStartX = 0.0f;	REAL32 fStartY = 0.0f;
	fStartX = i3Vector::GetX( &vec);
	fStartY = i3Vector::GetY( &vec);

	fMoveScaleX[idx] = i3Math::abs( fMoveGoalX-fStartX) / 10.0f;
	fMoveScaleY[idx] = i3Math::abs( fMoveGoalY-fStartY) / 10.0f;

	if( fMoveGoalX > fStartX) fStartX += fMoveScaleX[idx];
	else if( fMoveGoalX < fStartX) fStartX -= fMoveScaleX[idx];

	if( fMoveGoalY > fStartY) fStartY += fMoveScaleY[idx];
	else if( fMoveGoalY < fStartY) fStartY -= fMoveScaleY[idx];

	m_pSprKillHistory[img_idx]->SetRect(idx, fStartX, fStartY, fSizeX, fSizeY);

	pSize = m_pSprKillHistoryBG->GetSize(0);
	fSizeX = i3Vector::GetX(pSize) + (80.0f * 3.0f * rDeltaSeconds);
	fSizeY = i3Vector::GetY(pSize) + (80.0f * 3.0f * rDeltaSeconds);
	if( fSizeX > 80.0f*m_fViewerWidthRate) fSizeX = 80.0f*m_fViewerWidthRate;
	if( fSizeY > 80.0f*m_fViewerHeightRate) fSizeY = 80.0f*m_fViewerHeightRate;
	m_pSprKillHistoryBG->SetSize( 0, fSizeX, fSizeY);	
}

// 네번째 스텝
// 원래 크기와 100%의 알파를 가지고 원래 있어야 할자리에 자리잡습니다.
void CChainKills::_UpdateKillHistory_Step4(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds)
{
	// 사이즈 변경
	VEC2D* pSize = m_pSprKillHistory[img_idx]->GetSize(idx);

	REAL32 fSizeX = i3Vector::GetX(pSize) + (CK_SIZE_X*19.0f * rDeltaSeconds);
	REAL32 fSizeY = i3Vector::GetY(pSize) + (CK_SIZE_Y*19.0f * rDeltaSeconds);

	if( fSizeX > CK_DOUBLE_SIZE_X) fSizeX = CK_DOUBLE_SIZE_X;
	if( fSizeY > CK_DOUBLE_SIZE_Y) fSizeY = CK_DOUBLE_SIZE_Y;

	m_pSprKillHistory[img_idx]->SetColor( idx, 255, 255, 255, 255);
	m_pSprKillHistory[img_idx]->SetRect(idx,fGoalX, fGoalY, fSizeX, fSizeY);

	if( fSizeX == CK_DOUBLE_SIZE_X && fSizeY == CK_DOUBLE_SIZE_Y)
	{
		m_CkStep[idx] = 4;	
	}
}

// 다섯번째 스텝
// 사이즈가 원래 사이즈보다 확대되어집니다.
void CChainKills::_UpdateKillHistory_Step5(INT32 img_idx, INT32 idx, REAL32 rDeltaSeconds)
{
	// 사이즈 변경
	VEC2D* pSize = m_pSprKillHistory[img_idx]->GetSize(idx);

	REAL32 fSizeX = i3Vector::GetX(pSize) - (CK_SIZE_X*19.0f * rDeltaSeconds);
	REAL32 fSizeY = i3Vector::GetY(pSize) - (CK_SIZE_Y*19.0f * rDeltaSeconds);

	if( fSizeX < CK_SIZE_X) fSizeX = CK_SIZE_X;
	if( fSizeY < CK_SIZE_Y) fSizeY = CK_SIZE_Y;

	m_pSprKillHistory[img_idx]->SetRect(idx,fGoalX, fGoalY, fSizeX, fSizeY);

	if( fSizeX == CK_SIZE_X && fSizeY == CK_SIZE_Y)
	{
		m_finish_kill_mark = true;

		m_CkStep[idx] = 5;	

		if(CK_MAX_HISTORY_COUNT > m_DisplayChainkiller )
			++m_DisplayChainkiller;
	}
}

// 여섯번째 스텝
// 최종적으로 원래의 사이즈로 변환합니다..
void CChainKills::_UpdateKillHistory_Step6(INT32 img_idx, INT32 nKillCount, INT32 idx, REAL32 rDeltaSeconds)
{
	REAL32 x = (REAL32)GameUI::AutoOffset::get_auto_offset_position(
		GameUI::AOD_MIDDLE,
		(INT32)fMoveGoalX,
		CK_OFFSET,
		idx,
		m_DisplayChainkiller);

	m_pSprKillHistory[img_idx]->SetRect(idx,x, fGoalY, CK_SIZE_X, CK_SIZE_Y);

	if (1 < nKillCount) 
	{
		i3::stack_wstring wstrBuf;
		i3::sprintf(wstrBuf, L"%d", nKillCount);
		m_pTextKillCount[idx]->SetText(wstrBuf);
		m_pTextKillCount[idx]->setPos((UINT32)(x + CK_Text_OFFSET), (UINT32)fGoalY);

		m_pTextKillCount[idx]->SetEnable(true);
	}


}

void CChainKills::_UpdateKillHistory(REAL32 rDeltaSeconds)
{
	// Clear
	for(INT32 i = 0; i < 2; i++)
		m_pSprKillHistory[i]->SetEnable(false);	

	for(INT32 i = 0; i < CK::MAX_BUFFER; i++)
		m_pTextKillCount[i]->SetEnable(false);

	if (0 == m_nCountHistory)		return;

	// Set
	ptScreenSize = g_pFramework->GetSreenSize();

	//
	//fGoalX = (REAL32)uiMath::remap_ranges(0, ptScreenSize.x, 810, 0, 1024);
	//fGoalX -= (REAL32)(m_nCountHistory * 52.f);
	fGoalX = (REAL32)(ptScreenSize.x>>1);
	fGoalY = (REAL32)(ptScreenSize.y - 65);

	//
	fMoveGoalX = (REAL32)(ptScreenSize.x>>1);
	fMoveGoalY = (REAL32)(ptScreenSize.y - 65);

	for(INT32 i = 0 ; i < m_nCountHistory; i++)
	{	
		INT32 nKillHistoryType = m_qKillHistory[(m_nStartHistory + i) % CK::MAX_BUFFER]._KillHistoryType;
		INT32 nKillCount = m_qKillHistory[(m_nStartHistory + i) % CK::MAX_BUFFER]._KillCount;
		INT32 nKillImgIdx = m_qKillHistory[(m_nStartHistory + i) % CK::MAX_BUFFER]._KillImgIdx;

		//EscapeMission : 공룡모드일 경우 킬히스토리 대신 터치다운에 대한 히스토리를 뿌리도록 설정합니다. 
		if ((nKillHistoryType == CK_HISTORY_TOUCHDOWN) && 
			MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) )	// revision 57501
		{
			_SetSpriteDinoHistory(i);
			nKillImgIdx = 0; // 정신병 걸리겠음...
		}
		else
			_SetSpriteKillHistory(nKillImgIdx, i, nKillHistoryType);

		if( m_CkStep[i] == 0)	
			_UpdateKillHistory_Step1(nKillImgIdx, i, rDeltaSeconds);

		if( m_CkStep[i] == 1)
			_UpdateKillHistory_Step2(nKillImgIdx, i, rDeltaSeconds);

		if( m_CkStep[i] == 2)
			_UpdateKillHistory_Step3(nKillImgIdx, i, rDeltaSeconds);

		if( m_CkStep[i] == 3)
			_UpdateKillHistory_Step4(nKillImgIdx, i, rDeltaSeconds);

		if( m_CkStep[i] == 4) 
			_UpdateKillHistory_Step5(nKillImgIdx, i, rDeltaSeconds);

		if( m_CkStep[i] == 5)
			_UpdateKillHistory_Step6(nKillImgIdx, nKillCount, i, rDeltaSeconds);

		m_pSprKillHistory[nKillImgIdx]->SetEnable(i, true);
	}
}

void CChainKills::_SetSpriteKillHistory(INT32 img_idx, INT32 idxSpr,INT32 ckHistoryType)
{
	if (img_idx == 0 && ckHistoryType < CK_HISTORY_ASSIST)
	{
		INT32 u = 199 + ((ckHistoryType % 4) * 52);
		INT32 v = 337 + ((ckHistoryType / 4) * 56);
		m_pSprKillHistory[img_idx]->SetTextureCoord(idxSpr, (REAL32)u, (REAL32)v, u + 51.0f, v + 55.0f);
	}
	else if (ckHistoryType >= CK_HISTORY_ASSIST)
	{
		ckHistoryType -= CK_HISTORY_ASSIST;

		INT32 u = 199 + ((ckHistoryType % 4) * 52);
		INT32 v = 457 + ((ckHistoryType / 4) * 56);
		m_pSprKillHistory[img_idx]->SetTextureCoord(idxSpr, (REAL32)u, (REAL32)v, u + 51.0f, v + 55.0f);
	}
	else
	{
		ckHistoryType -= CK_HISTORY_GUNNER;

		INT32 u = 765 + ((ckHistoryType % 4) * 62);
		INT32 v = 139 + ((ckHistoryType / 4) * 61);
		m_pSprKillHistory[img_idx]->SetTextureCoord(idxSpr, (REAL32)u, (REAL32)v, u + 51.0f, v + 55.0f);
	}
}

void CChainKills::_SetSpriteDinoHistory(INT32 idxSpr)
{
	REAL32 u = 102.f;
	REAL32 v = 400.f;

	m_pSprKillHistory[0]->SetTextureCoord(idxSpr, u,v, u + 51.0f, v + 51.0f);
}



//- wop ---------------//
bool wop::is_throwing_wp_smoke(CWeaponInfo* p)
{
	if (p->GetTypeClass() == WEAPON_CLASS_THROWING_CONTAIN)
	{
		return true;
	}

	return false;
}

bool wop::is_throwing_grenade(CWeaponInfo* p)
{
	if (p->GetTypeClass() == WEAPON_CLASS_THROWING_GRENADE ||
		p->GetTypeClass() == WEAPON_CLASS_GRENADESHELL)
	{
		return true;
	}

	return false;
}