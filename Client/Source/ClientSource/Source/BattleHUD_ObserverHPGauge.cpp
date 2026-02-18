#include "pch.h"
#include "BattleHUD_ObserverHPGauge.h"

#include "GameCharaContexts.h"
#include "BattleHud.h"
#include "ConfigRenderBattle.h"

#include "../StageBattle/ScoreContext.h"
#include "../StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "StageBattle.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE( CHUD_ObserverHPGauge);//, i3GameObjBase);

namespace inner { CHUD_ObserverHPGauge* this_ptr = 0; }
CHUD_ObserverHPGauge* CHUD_ObserverHPGauge::instance() { return inner::this_ptr; }

CHUD_ObserverHPGauge::CHUD_ObserverHPGauge()
{
	inner::this_ptr = this;

	m_pHud = nullptr;
	m_pSprite	= nullptr;
	m_pSprite2	= nullptr;	// GMObserver용 CallSignNumber Sprite

	for(INT32 i=0; i<2; i++) m_pTextTitle[i] = nullptr;
	for(INT32 i=0; i<SLOT_MAX_COUNT; i++)
	{
		m_pTextName[i] = nullptr;
		m_pTextCount[i] = nullptr;
		m_pTextWeapon[i] = nullptr;
	}
	m_bEnableHPGauge = false;
	m_bFirstIn = false;
	m_fDelayTime = 0.0f;
	m_nOffsetHeight = 0;
}

CHUD_ObserverHPGauge::~CHUD_ObserverHPGauge()
{
	INT32 i = 0;

	I3_SAFE_NODE_RELEASE( m_pSprite);
	I3_SAFE_NODE_RELEASE( m_pSprite2);

	for( i=0; i<2; i++) I3_SAFE_NODE_RELEASE( m_pTextTitle[i]);
	for( i=0; i<SLOT_MAX_COUNT; i++) I3_SAFE_NODE_RELEASE( m_pTextName[i]);
	for( i=0; i<SLOT_MAX_COUNT; i++) I3_SAFE_NODE_RELEASE( m_pTextCount[i]);
	for( i=0; i<SLOT_MAX_COUNT; i++) I3_SAFE_NODE_RELEASE( m_pTextWeapon[i]);
}

// Create()는 반드시 호출되어야 합니다.
void CHUD_ObserverHPGauge::Create( CHud * pHud)
{
	INT32 i = 0;
	m_pHud = pHud;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();	
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	i3Texture * pTex = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);

	m_pSprite = i3Sprite2D::new_object();	
	I3ASSERT( m_pSprite != nullptr);
	m_pSprite->Create( (SLOT_MAX_COUNT*3)+6, true, true);
	m_pSprite->SetTexture( pTex );
	I3_MUST_RELEASE(pTex);

	// HP Slot
	INT32 nRedCurIdx = 0, nBlueCurIdx = 0;
	for( i=0; i<(SLOT_MAX_COUNT*2) + 6; i++)
	{
		m_pSprite->SetEnable( true);		
		m_pSprite->SetColor( i, 255,255,255,255);		
		
		if( i <= 15)		// Slot
		{
			if(i%2 == 0)	// Red Team Slot
			{
				m_pSprite->SetTextureCoord( i, 473.0f, 38.0f, 692.0f, 73.0f);
				m_pSprite->SetRect( i, 30.0f, (REAL32)nScreenHeight - 145.0f - (37.0f*nRedCurIdx), 219.0f, 35.0f);
				nRedCurIdx++;
			}
			else			// Blue Team Slot
			{
				m_pSprite->SetTextureCoord( i, 473.0f, 38.0f, 692.0f, 73.0f);
				m_pSprite->SetRect( i, (REAL32)nScreenWidth - 249.0f, (REAL32)nScreenHeight - 145.0f - (37.0f*nBlueCurIdx), 219.0f, 35.0f);
				nBlueCurIdx++;
			}	

			if( i==15) 
			{
				nRedCurIdx = 0;
				nBlueCurIdx = 0;
			}
		}
		else if( i <= 31)		// Gauge
		{
			if(i%2 == 0)	// Red Team Gauge
			{
				m_pSprite->SetTextureCoord( i, 473.0f, 74.0f, 662.0f, 91.0f);
				m_pSprite->SetRect( i, 57.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nRedCurIdx), 189.0f, 17.0f);
				nRedCurIdx++;
			}
			else			// Blue Team Gauge
			{
				m_pSprite->SetTextureCoord( i, 473.0f, 92.0f, 662.0f, 109.0f);
				m_pSprite->SetRect( i, (REAL32)nScreenWidth - 222.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nBlueCurIdx), 189.0f, 17.0f);
				nBlueCurIdx++;				
			}

			if( i==31) 
			{
				nRedCurIdx = 0;
				nBlueCurIdx = 0;
			}
		}
		else
		{
			if(i%2 == 0)	// Red Team Title
			{
				if( nRedCurIdx == 0)
				{
					m_pSprite->SetTextureCoord( i, 693.0f, 38.0f, 721.0f, 68.0f);
					m_pSprite->SetRect( i, 2.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 28.0f, 30.0f);
				}
				else if( nRedCurIdx == 1)
				{
					m_pSprite->SetTextureCoord( i, 726.0f, 38.0f, 751.0f, 68.0f);
					m_pSprite->SetRect( i, 30.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 191.0f, 30.0f);
				}
				else
				{
					m_pSprite->SetTextureCoord( i, 757.0f, 38.0f, 784.0f, 68.0f);
					m_pSprite->SetRect( i, 221.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 27.0f, 30.0f);
				}

				nRedCurIdx++;
			}
			else			// Blue Team Title
			{
				if( nBlueCurIdx == 0)
				{
					m_pSprite->SetTextureCoord( i, 693.0f, 69.0f, 720.0f, 99.0f);
					m_pSprite->SetRect( i, (REAL32)nScreenWidth - 249.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 27.0f, 30.0f);
				}
				else if( nBlueCurIdx == 1)
				{
					m_pSprite->SetTextureCoord( i, 726.0f, 69.0f, 751.0f, 99.0f);
					m_pSprite->SetRect( i, (REAL32)nScreenWidth - 222.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 191.0f, 30.0f);
				}
				else
				{
					m_pSprite->SetTextureCoord( i, 757.0f, 69.0f, 784.0f, 99.0f);
					m_pSprite->SetRect( i, (REAL32)nScreenWidth - 31.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*8), 27.0f, 30.0f);
				}

				nBlueCurIdx++;
			}
		}
	}
	m_pHud->getHUDRoot()->AddChild( m_pSprite);

	CreateGMObserverTeamGuageSpr();

	{
		nRedCurIdx = 0;
		nBlueCurIdx = 0;

		// Title
		for( i = 0; i<2; i++)
		{		
			m_pTextTitle[i] = i3TextNodeDX2::new_object();
			m_pTextTitle[i]->Create( GetDefaultFontName(), FONT_COUNT_32, GAME_FONT_SIZE_HUD_OBSERVER_TITLE, FW_BOLD, false, false, DRAFT_QUALITY, true, FONT_SHADOW_QUALITY_LOW);
			m_pTextTitle[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pTextTitle[i]->setSize( 191, 23);
			m_pTextTitle[i]->SetEnable( true);
			m_pTextTitle[i]->SetColor( 255, 255, 255, 255);	

			if( i == 0)
			{
				m_pTextTitle[i]->setPos( 30, nScreenHeight - 135 - (37*8));
			}
			else
			{
				m_pTextTitle[i]->setPos( nScreenWidth - 240, nScreenHeight - 135 - (37*8));
			}
			
			m_pTextTitle[i]->SetText( GAME_RCSTRING("STR_TBL_INGAME_HUD_LIFE_COUNT_TEXT"));

			m_pHud->getHUDRoot()->AddChild( m_pTextTitle[i]);		
		}		

		for( i = 0; i<SLOT_MAX_COUNT; i++)
		{
			CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( i);


			m_pTextName[i] = i3TextNodeDX2::new_object();
			m_pTextName[i]->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_OBSERVER_NAME, FW_NORMAL, false, false, DRAFT_QUALITY, true, FONT_SHADOW_QUALITY_LOW);

			m_pTextCount[i] = i3TextNodeDX2::new_object();
			m_pTextCount[i]->Create( BATTLE_CHARA_SET, 6, 15, FW_NORMAL, false, false, DRAFT_QUALITY, true, FONT_SHADOW_QUALITY_LOW);
			m_pTextCount[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			m_pTextWeapon[i] = i3TextNodeDX2::new_object();
			m_pTextWeapon[i]->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_SIZE_HUD_OBSERVER_WEAPON, FW_NORMAL, false, false, DRAFT_QUALITY, true, FONT_SHADOW_QUALITY_LOW);

			if(pChara == nullptr)
				continue;

			if( pChara->getTeam() == TEAM_RED)	// Red Team
			{
				// Name
				m_pTextName[i]->setPos( 60, nScreenHeight - 140 - (37*nRedCurIdx));				
				m_pTextName[i]->setSize( 189, 17);
				m_pTextName[i]->SetTextEllipsis(L"Red Team");

				// Count
				m_pTextCount[i]->setPos( 34, nScreenHeight - 140 - (37*nRedCurIdx));				
				m_pTextCount[i]->setSize( 21, 26);
				m_pTextCount[i]->SetTextEllipsis(L"9");

				// Weapon
				m_pTextWeapon[i]->setPos( 60, nScreenHeight - 123 - (37*nRedCurIdx));				
				m_pTextWeapon[i]->setSize( 129, 17);
				m_pTextWeapon[i]->SetTextEllipsis(L"G36C");


				nRedCurIdx++;
			}
			else if( pChara->getTeam() == TEAM_BLUE)			// Blue Team
			{
				// Name
				m_pTextName[i]->setPos( nScreenWidth - 219, nScreenHeight - 140 - (37*nBlueCurIdx));
				m_pTextName[i]->setSize( 189, 17);
				m_pTextName[i]->SetTextEllipsis(L"Blue Team");

				// Count
				m_pTextCount[i]->setPos( nScreenWidth - 246, nScreenHeight - 140 - (37*nBlueCurIdx));
				m_pTextCount[i]->setSize( 21, 26);
				m_pTextCount[i]->SetTextEllipsis(L"4");

				// Weapon
				m_pTextWeapon[i]->setPos( nScreenWidth - 219, nScreenHeight - 123 - (37*nBlueCurIdx));
				m_pTextWeapon[i]->setSize( 129, 17);
				m_pTextWeapon[i]->SetTextEllipsis(L"Ak-47");

				nBlueCurIdx++;
			}	

			m_pHud->getHUDRoot()->AddChild( m_pTextName[i]);		
			m_pHud->getHUDRoot()->AddChild( m_pTextCount[i]);		
			m_pHud->getHUDRoot()->AddChild( m_pTextWeapon[i]);		
		}		
	}
}

void CHUD_ObserverHPGauge::SetEnableGauge( bool bFlag)
{
	INT32 i = 0;

	m_bEnableHPGauge = bFlag;

	if( !bFlag)
	{
		m_pSprite->SetEnable( bFlag);

		if(m_pSprite2)
			m_pSprite2->SetEnable( bFlag);

		for( i=0; i<2; i++) m_pTextTitle[i]->SetEnable( bFlag);
		for( i=0; i<SLOT_MAX_COUNT; i++) m_pTextName[i]->SetEnable( bFlag);
		for( i=0; i<SLOT_MAX_COUNT; i++) m_pTextCount[i]->SetEnable( bFlag);
		for( i=0; i<SLOT_MAX_COUNT; i++) m_pTextWeapon[i]->SetEnable( bFlag);
	}
}

void CHUD_ObserverHPGauge::OnUpdate( REAL32 rDeltaSeconds )
{	
	i3GameObjBase::OnUpdate( rDeltaSeconds );

	if( m_bEnableHPGauge)
	{
		UpdateGauge( rDeltaSeconds);
	}
}

void CHUD_ObserverHPGauge::UpdateGauge( REAL32 rDeltaSeconds)
{	
	INT32 iHeightOffset = 0;

	if(UserInfoContext::i()->IsGM_Observer())
	{
		iHeightOffset = -220;
	}

	if( !m_bFirstIn)
	{
		m_fDelayTime += rDeltaSeconds;
		if( m_fDelayTime > 0.5f) m_bFirstIn = true;

		return;
	}

	INT32 i = 0;

	INT32 nRedTeamCount = 0;
	INT32 nBlueTeamCount = 0;
	INT32 nRedLiveCount = 0;
	INT32 nBlueLiveCount = 0;
	bool bRedPass = false;
	bool bBluePass = false;

	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();	
	INT32	nScreenHeight	= pViewer->GetViewHeight();

	nScreenHeight += iHeightOffset;

	for( i = 0; i<SLOT_MAX_COUNT; i++)
	{
		m_pTextName[i]->SetText(L"");	
		m_pTextCount[i]->SetText(L"");
		m_pTextWeapon[i]->SetText(L"");

		m_pSprite->SetEnable( i, false);
		m_pSprite->SetEnable( i+16, false);

		if(m_pSprite2 && UserInfoContext::i()->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
			m_pSprite2->SetEnable(i, false);

		m_pTextName[i]->SetEnable( false);	
		m_pTextCount[i]->SetEnable( false);
		m_pTextWeapon[i]->SetEnable( false);
	}

	i3::rc_wstring wstrTemp;
	i3::stack_wstring wstrStackTemp;

	for( i = 0; i<SLOT_MAX_COUNT; i++)
	{	
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i);	
	

		if( pChara != nullptr && UserContext::i()->IsPlayingSlot(i)&&	BattleSlotContext::i()->getSlotInfo(i)->_Rank != RANK_GAMEMASTER ) 
		{
				m_pSprite->SetEnable( i, true);
				m_pSprite->SetEnable( i+16, true);
				m_pTextName[i]->SetEnable( true);	
				m_pTextCount[i]->SetEnable( true);
				m_pTextWeapon[i]->SetEnable( true);

				if(UserInfoContext::i()->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
					m_pSprite2->SetEnable( i, true);
			
			// 이름
			m_pTextName[i]->SetTextEllipsis(BattleSlotContext::i()->getNickForSlot(i));

			// 킬 카운트
			{
				const SCORE_INFO* pScoreInfo = ScoreContext::i()->getIndividualScore(i);
				i3::sprintf( wstrStackTemp, L"%d", pScoreInfo->m_KillCount);
				m_pTextCount[i]->SetTextEllipsis( wstrStackTemp );
			}

			// 보유무기			
			if( pChara != nullptr)	
			{
				if(pChara->IsDino() == false)
				{
					WeaponBase * pWeapon = pChara->GetCurrentCharaWeapon();

					if( pWeapon != nullptr)
					{
						WeaponBase * pWeaponBase = pWeapon->getUsedWeapon();

						if( pWeaponBase != nullptr)
						{
							pWeaponBase->getWeaponInfo()->GetWeaponName( wstrTemp);
							
							m_pTextWeapon[i]->SetTextEllipsis( wstrTemp );
						}
					}
				}
			}

			// HP Gauge
			if( pChara != nullptr)	
			{
				INT32 nHP = pChara->getCurHP();	
				if( pChara->isCharaStateMask( CHARA_STATE_DEATH))
				{
					nHP = 0;
					m_pTextWeapon[i]->SetText(L"");
				}
				REAL32 fRate = (REAL32)nHP / (REAL32)(pChara->getFullHP());			

				if( TESTBIT(MyRoomInfoContext::i()->GetMyRoomLockObserver(), LOCK_OBSERVER_ENEMY_HP) == 0)
				{
#if defined( DEF_OBSERVER_MODE)
					if( BattleSlotContext::i()->isObserverMe())
#else
					if(UserInfoContext::i()->IsGM_Observer())
#endif
					{
						bBluePass = false;
						bRedPass = false;
					}
					else {
						if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_RED)
						{
							bBluePass = true;
						}
						else if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
						{
							bRedPass = true;
						}
					}
				}				

				if( pChara->getTeam() == TEAM_RED)	// RED
				{			
					if( !bRedPass)
					{
						// 슬롯
						m_pSprite->SetRect( i, 30.0f, (REAL32)nScreenHeight - 145.0f - (37.0f*nRedTeamCount), 219.0f, 35.0f);

						if(UserInfoContext::i()->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
						{
							m_pSprite2->SetRect( i, 252.0f, (REAL32)nScreenHeight - 139.0f - (37.0f*nRedTeamCount), 24.f, 24.0f); 
						}

						// 게이지
						m_pSprite->SetTextureCoord( i+16, 473.0f, 74.0f, 473.0f + (189.0f*fRate), 91.0f);
						m_pSprite->SetRect( i+16, 57.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nRedTeamCount), 189.0f*fRate, 17.0f);

						// Name
						m_pTextName[i]->setPos( 60, nScreenHeight - 140 - (37*nRedTeamCount));				

						// Count
						m_pTextCount[i]->setPos( 34, nScreenHeight - 140 - (37*nRedTeamCount));				

						// Weapon
						m_pTextWeapon[i]->setPos( 60, nScreenHeight - 123 - (37*nRedTeamCount));				

						if( fRate > 0.0f)	nRedLiveCount++;
					}
					else
					{
						m_pSprite->SetEnable( i, false);
						m_pSprite->SetEnable( i+16, false);
						m_pTextName[i]->SetEnable( false);	
						m_pTextCount[i]->SetEnable( false);
						m_pTextWeapon[i]->SetEnable( false);

						if(m_pSprite2)
							m_pSprite2->SetEnable( i, false);
					}
				}
				else if( pChara->getTeam() == TEAM_BLUE)			// BLUE
				{
					if( !bBluePass)
					{
						// 슬롯
						m_pSprite->SetRect( i, (REAL32)nScreenWidth - 249.0f, (REAL32)nScreenHeight - 145.0f - (37.0f*nBlueTeamCount), 219.0f, 35.0f);

						if(UserInfoContext::i()->IsGMObserverViewFlag(UserInfoContext::GMOBSERVER_VIEWFLAG_CALLSIGN_NUMBER))
						{
							m_pSprite2->SetRect( i, (REAL32)nScreenWidth - 278.0f, (REAL32)nScreenHeight - 139.0f - (37.0f*nBlueTeamCount), 24.f, 24.0f);
						}

						// 게이지
						m_pSprite->SetTextureCoord( i+16, 473.0f, 92.0f, 473.0f + (189.0f*fRate), 109.0f);
						m_pSprite->SetRect( i+16, (REAL32)nScreenWidth - 222.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nBlueTeamCount), 189.0f*fRate, 17.0f);

						// Name
						m_pTextName[i]->setPos( nScreenWidth - 219, nScreenHeight - 140 - (37*nBlueTeamCount));

						// Count
						m_pTextCount[i]->setPos( nScreenWidth - 246, nScreenHeight - 140 - (37*nBlueTeamCount));

						// Weapon
						m_pTextWeapon[i]->setPos( nScreenWidth - 219, nScreenHeight - 123 - (37*nBlueTeamCount));

						if( fRate > 0.0f)	nBlueLiveCount++;
					}
					else
					{
						m_pSprite->SetEnable( i, false);
						m_pSprite->SetEnable( i+16, false);
						m_pTextName[i]->SetEnable( false);	
						m_pTextCount[i]->SetEnable( false);
						m_pTextWeapon[i]->SetEnable( false);

						if(m_pSprite2)
							m_pSprite2->SetEnable( i, false);
					}
				}
			}

			if( pChara->getTeam() == TEAM_RED)	// Red Team
			{
				if( !bRedPass)
				{
					nRedTeamCount++;
				}
			}
			else if( pChara->getTeam() == TEAM_BLUE)			// Red Team
			{
				if( !bBluePass)
				{
					nBlueTeamCount++;
				}
			}
		}		
	}

	// Title의 위치를 결정합니다.
	if( nRedTeamCount > 0)
	{
		m_pSprite->SetEnable( 32, true);
		m_pSprite->SetEnable( 34, true);
		m_pSprite->SetEnable( 36, true);
		m_pTextTitle[0]->SetEnable( true);

		m_pSprite->SetRect( 32, 2.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nRedTeamCount), 28.0f, 30.0f);
		m_pSprite->SetRect( 34, 30.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nRedTeamCount), 191.0f, 30.0f);
		m_pSprite->SetRect( 36, 221.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nRedTeamCount), 27.0f, 30.0f);
		m_pTextTitle[0]->setPos( 30, nScreenHeight - 135 - (37*nRedTeamCount));				// Red

	
		i3::sprintf( wstrStackTemp, L"%s%d", GAME_RCSTRING("STR_TBL_INGAME_HUD_LIFE_COUNT_TEXT"), nRedLiveCount);
		m_pTextTitle[0]->SetText( wstrStackTemp );		

		m_nOffsetHeight = (INT32)(37.0f*nRedTeamCount) + 140;
	}
	else
	{
		// Default Chat Pos At ObserverMode
		m_nOffsetHeight = 100;

		m_pSprite->SetEnable( 32, false);
		m_pSprite->SetEnable( 34, false);
		m_pSprite->SetEnable( 36, false);
		m_pTextTitle[0]->SetEnable( false);
	}

	if( nBlueTeamCount > 0)
	{
		m_pSprite->SetEnable( 33, true);
		m_pSprite->SetEnable( 35, true);
		m_pSprite->SetEnable( 37, true);
		m_pTextTitle[1]->SetEnable( true);

		m_pSprite->SetRect( 33, (REAL32)nScreenWidth - 249.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nBlueTeamCount), 27.0f, 30.0f);
		m_pSprite->SetRect( 35, (REAL32)nScreenWidth - 222.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nBlueTeamCount), 191.0f, 30.0f);
		m_pSprite->SetRect( 37, (REAL32)nScreenWidth - 31.0f, (REAL32)nScreenHeight - 142.0f - (37.0f*nBlueTeamCount), 27.0f, 30.0f);
		m_pTextTitle[1]->setPos( nScreenWidth - 240, nScreenHeight - 135 - (37*nBlueTeamCount)); // Blue

		i3::sprintf( wstrStackTemp, L"%s%d", GAME_RCSTRING("STR_TBL_INGAME_HUD_LIFE_COUNT_TEXT"), nBlueLiveCount);
		m_pTextTitle[1]->SetText( wstrStackTemp );
	}
	else
	{
		m_pSprite->SetEnable( 33, false);
		m_pSprite->SetEnable( 35, false);
		m_pSprite->SetEnable( 37, false);
		m_pTextTitle[1]->SetEnable( false);
	}	
}

void CHUD_ObserverHPGauge::CreateGMObserverTeamGuageSpr()
{
	if(!UserInfoContext::i()->IsGM_Observer())
		return;

	if(m_pSprite2)
		return;

	i3Texture* pTex2 = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD4);

	m_pSprite2 = i3Sprite2D::new_object();	
	I3ASSERT( m_pSprite2 != nullptr);

	m_pSprite2->Create( SLOT_MAX_COUNT, true, true);
	m_pSprite2->SetTexture( pTex2 );

	pTex2->Release();
	
	INT32 nRedCurIdx = 0, nBlueCurIdx = 0;

	for(INT32 i=0; i<SLOT_MAX_COUNT; ++i)
	{
		m_pSprite2->SetEnable(true);
		m_pSprite2->SetColor( i, 255,255,255,255);

		if( i % 2 == 0)	// Red Team Gauge
		{
			m_pSprite2->SetTextureCoord(i, 5, 5+(27.f*nRedCurIdx), 29.f, 29.f+(27.f*nRedCurIdx));
			m_pSprite2->SetRect( i, 0.f, 0.f, 24.f, 24.f);
			nRedCurIdx++;			
		}
		else // Blue Team Gauge
		{
			m_pSprite2->SetTextureCoord( i, 35.0f, 5.0f+(27.f*nBlueCurIdx), 59.f, 29.f+(27.f*nBlueCurIdx));
			m_pSprite2->SetRect( i, 100.f, 0.f, 24.f, 24.f);
			nBlueCurIdx++;
		}
			
		if(i==SLOT_MAX_COUNT-1) 
		{
			nRedCurIdx = 0;
			nBlueCurIdx = 0;
		}
	}

	m_pHud->getHUDRoot()->AddChild(m_pSprite2);
}

