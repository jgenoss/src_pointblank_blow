#include "pch.h"
#include "BattleHUD_TrainingScore.h"

#include "GameGUI.h"

#include "StageBattle/ScoreContext.h"
#include "StageBattle/AIModeContext.h"
#include "BattleSlotContext.h"
#include "UI/UIUtil.h"

#include "i3Base/profile/profile.h"
#include "i3Base/itl/range/generic_empty.h"

I3_CLASS_INSTANCE(CHud_TrainingScore);

namespace inner { CHud_TrainingScore* this_ptr = 0; }
CHud_TrainingScore* CHud_TrainingScore::instance() { return inner::this_ptr; }

CHud_TrainingScore::CHud_TrainingScore(void)
{
	i3_prof_func();

	inner::this_ptr = this;

	m_pSpriteTrainingBG = nullptr;
	m_pSpriteTrainingRecordBG = nullptr;

	for(INT32 i=0; i<MAX_TEAM_USER; i++)
	{
		m_pSpriteTrainingRankMark[i] = nullptr;
		m_pTextTrainingUserNick[i] = nullptr;
		m_pTextTrainingUserScore[i] = nullptr;
		m_pTextTrainingHP[i] = nullptr;
		m_pSpriteTrainingHPGauge[i] = nullptr;
	}
	
	for(INT32 i=0; i<HUD_TEXT_TRAINING_RECORD_COUNT; i++)
	{
		m_pTextTrainingRecord[i] = nullptr;
	}
}

CHud_TrainingScore::~CHud_TrainingScore(void)
{
	i3_prof_func();

	inner::this_ptr = 0;

	I3_SAFE_NODE_RELEASE( m_pSpriteTrainingBG);
	I3_SAFE_NODE_RELEASE( m_pSpriteTrainingRecordBG);

	for(INT32 i=0; i<MAX_TEAM_USER; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pSpriteTrainingRankMark[i]);
		I3_SAFE_NODE_RELEASE( m_pTextTrainingUserNick[i]);
		I3_SAFE_NODE_RELEASE( m_pTextTrainingUserScore[i]);
		I3_SAFE_NODE_RELEASE( m_pTextTrainingHP[i]);
		I3_SAFE_NODE_RELEASE( m_pSpriteTrainingHPGauge[i]);
	}

	for(INT32 i=0; i<HUD_TEXT_TRAINING_RECORD_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTextTrainingRecord[i]);
	}
}


void CHud_TrainingScore::CreateHUD(i3AttrSet* pHUDRoot, i3Texture* pTexture, i3Texture* pTextureRankMark)
{
	// Rank BG
	m_pSpriteTrainingBG = i3Sprite2D::new_object();
	m_pSpriteTrainingBG->SetTexture( pTexture );
	m_pSpriteTrainingBG->Create(1, true, true);
	m_pSpriteTrainingBG->SetEnable( 0, false);		
	m_pSpriteTrainingBG->SetColor( 0, 255,255,255,170);
	m_pSpriteTrainingBG->SetTextureCoord( 0, 935.0f, 425.0f, 940.0f, 430.0f);
	pHUDRoot->AddChild(m_pSpriteTrainingBG);

	for(INT32 i=0; i<MAX_TEAM_USER; i++)
	{
		// Rank Mark
		m_pSpriteTrainingRankMark[i] = i3Sprite2D::new_object();
		m_pSpriteTrainingRankMark[i]->SetTexture( pTextureRankMark );
		m_pSpriteTrainingRankMark[i]->Create(1, true, true);
		m_pSpriteTrainingRankMark[i]->SetEnable( 0, false);
		m_pSpriteTrainingRankMark[i]->SetColor( 0, 255,255,255,255);
		m_pSpriteTrainingRankMark[i]->SetTextureCoord( 0, 817.0f, 0.0f, 932.0f, 30.0f);
		pHUDRoot->AddChild(m_pSpriteTrainingRankMark[i]);

		// Nick
		m_pTextTrainingUserNick[i] = i3TextNodeDX2::new_object();
		m_pTextTrainingUserNick[i]->Create( GetDefaultFontName(), FONT_COUNT_64, (INT32)(GAME_FONT_DEFAULT_SIZE *i3GuiRoot::getHeightRatioBy768()), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextTrainingUserNick[i]->SetTextEllipsis(L"");	
		m_pTextTrainingUserNick[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTrainingUserNick[i]->setAutoWrapEnable( false);
		m_pTextTrainingUserNick[i]->setSize( (UINT32)(100*i3GuiRoot::getHeightRatioBy768()), (UINT32)(16*i3GuiRoot::getHeightRatioBy768()));
		m_pTextTrainingUserNick[i]->SetEnable(false);
		pHUDRoot->AddChild(m_pTextTrainingUserNick[i]);

		// Score
		m_pTextTrainingUserScore[i] = i3TextNodeDX2::new_object();
		m_pTextTrainingUserScore[i]->Create( GetDefaultFontName(), FONT_COUNT_64, (INT32)(GAME_FONT_DEFAULT_SIZE *i3GuiRoot::getHeightRatioBy768()), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextTrainingUserScore[i]->SetTextEllipsis(L"");	
		m_pTextTrainingUserScore[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTrainingUserScore[i]->setAutoWrapEnable( false);
		m_pTextTrainingUserScore[i]->setSize( (UINT32)(80*i3GuiRoot::getHeightRatioBy768()), (UINT32)(16*i3GuiRoot::getHeightRatioBy768()));
		m_pTextTrainingUserScore[i]->SetEnable(false);
		pHUDRoot->AddChild(m_pTextTrainingUserScore[i]);

		// HP Text
		m_pTextTrainingHP[i] = i3TextNodeDX2::new_object();
		m_pTextTrainingHP[i]->Create( GetDefaultFontName(), FONT_COUNT_32, (INT32)(GAME_FONT_DEFAULT_SIZE *i3GuiRoot::getHeightRatioBy768()), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextTrainingHP[i]->SetTextEllipsis(L"HP");	
		m_pTextTrainingHP[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTrainingHP[i]->setAutoWrapEnable( false);
		m_pTextTrainingHP[i]->setSize( (UINT32)(20*i3GuiRoot::getHeightRatioBy768()), (UINT32)(16*i3GuiRoot::getHeightRatioBy768()));
		m_pTextTrainingHP[i]->SetEnable(false);
		pHUDRoot->AddChild(m_pTextTrainingHP[i]);

		// HP Gauge
		m_pSpriteTrainingHPGauge[i] = i3Sprite2D::new_object();
		m_pSpriteTrainingHPGauge[i]->SetTexture( pTexture );
		m_pSpriteTrainingHPGauge[i]->Create(1, true, true);
		m_pSpriteTrainingHPGauge[i]->SetEnable( 0, false);
		m_pSpriteTrainingHPGauge[i]->SetColor( 0, 255,0,0,255);
		m_pSpriteTrainingHPGauge[i]->SetTextureCoord( 0, 33.0f, 33.0f, 35.0f, 35.0f);
		pHUDRoot->AddChild(m_pSpriteTrainingHPGauge[i]);
	}

	// Record용 BG
	m_pSpriteTrainingRecordBG= i3Sprite2D::new_object();
	m_pSpriteTrainingRecordBG->SetTexture( pTexture );
	m_pSpriteTrainingRecordBG->Create(1, true, true);
	m_pSpriteTrainingRecordBG->SetEnable( 0, false);		
	m_pSpriteTrainingRecordBG->SetColor( 0, 255,255,255,170);
	m_pSpriteTrainingRecordBG->SetTextureCoord( 0, 280.0f, 280.0f, 284.0f, 284.0f);
	pHUDRoot->AddChild(m_pSpriteTrainingRecordBG);

	for(INT32 i=0; i<HUD_TEXT_TRAINING_RECORD_COUNT; i++)
	{
		// Rank Mark
		m_pTextTrainingRecord[i] = i3TextNodeDX2::new_object();
		m_pTextTrainingRecord[i]->Create( GetDefaultFontName(), FONT_COUNT_64, (INT32)(GAME_FONT_DEFAULT_SIZE *i3GuiRoot::getHeightRatioBy768()), FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL);
		m_pTextTrainingRecord[i]->SetTextEllipsis(L"");	
		m_pTextTrainingRecord[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextTrainingRecord[i]->setAutoWrapEnable( false);
		m_pTextTrainingRecord[i]->setSize( (UINT32)(150*i3GuiRoot::getWidthRatioBy768()), (UINT32)(16*i3GuiRoot::getHeightRatioBy768()));
		m_pTextTrainingRecord[i]->SetEnable(false);
		pHUDRoot->AddChild(m_pTextTrainingRecord[i]);

	}
}

void CHud_TrainingScore::OnUpdate( REAL32 rDeltaSeconds)
{
	INT32 i = 0;
	INT32 RedStartIdx = 0;
	INT32 BlueStartIdx = 0;
	INT32 nPeopleCount = 0;
	INT32 nStartPosX = 10;
	INT32 nStartPosY = static_cast<INT32>(g_pEnvSet->m_nScreenHeight * 0.43f);
	INT32 nOffsetPosY = 43;
	i3::stack_wstring wstrStackTemp;
//	char szTemp[256];

	SCORE_ARRAY	_RedPoint[MAX_TEAM_USER];
	SCORE_ARRAY	_BluePoint[MAX_TEAM_USER];
//	SCORE_ARRAY	_LastPoint[MAX_TEAM_USER];			// 에라이... (2014.07.10.수빈)
	const SCORE_ARRAY* LastPoint = nullptr;

	m_pSpriteTrainingBG->SetEnable( false);
	for(i=0; i<MAX_TEAM_USER; i++)
	{
		m_pSpriteTrainingRankMark[i]->SetEnable( false);
		m_pTextTrainingUserNick[i]->SetEnable( false);
		m_pTextTrainingUserScore[i]->SetEnable( false);
		m_pTextTrainingHP[i]->SetEnable( false);
		m_pSpriteTrainingHPGauge[i]->SetEnable( false);

		_RedPoint[i].wstrNick.clear();
		_BluePoint[i].wstrNick.clear();
	}

#if defined( ENABLE_BEST_RECORD)
	// 현재 최고기록을 소유한 사람의 정보를 표시합니다.
	UpdateTrainingRecord(rDeltaSeconds);
#endif

	for(i=0; i<SLOT_MAX_COUNT; i++)
	{
		const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo(i);
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i);		

		if( !pChara ) continue;

		if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
		{
			if(pSlotInfo->_State == SLOT_STATE_BATTLE)
			{
				if( pChara->getTeam() == TEAM_BLUE)	// BLUE
				{
					const SCORE_INFO * pScoreInfo = ScoreContext::i()->getIndividualScore(i);

					// Index
					_BluePoint[BlueStartIdx].nIdx = i;
					// ID
					_BluePoint[BlueStartIdx].wstrNick = BattleSlotContext::i()->getNickForSlot(i, CHARA::BUFFER_NEW_AI);
					// Hero Kill Count
					_BluePoint[BlueStartIdx].nKillCount = pScoreInfo->m_KillCount;
					// HP
					if( pChara != nullptr)
					{
						if( pChara->isCharaStateMask( CHARA_STATE_DEATH) )
							_BluePoint[BlueStartIdx].HP_Rate = 0.f;
						else
							_BluePoint[BlueStartIdx].HP_Rate = (REAL32)pChara->getCurHP() / (REAL32)(pChara->getFullHP());
					}

					BlueStartIdx++;

					nPeopleCount++;
				}
			}
		}
		else
		{
			if(pSlotInfo->_State == SLOT_STATE_BATTLE)
			{
				if( pChara->getTeam() == TEAM_RED)	// RED
				{
					const SCORE_INFO * pScoreInfo = ScoreContext::i()->getIndividualScore(i);

					// Index
					_RedPoint[RedStartIdx].nIdx = i;
					// ID
					_RedPoint[RedStartIdx].wstrNick = BattleSlotContext::i()->getNickForSlot(i);
					// Hero Kill Count
					_RedPoint[RedStartIdx].nKillCount = pScoreInfo->m_KillCount;
					// HP
					if( pChara != nullptr)
					{
						if( pChara->isCharaStateMask( CHARA_STATE_DEATH) )
							_RedPoint[RedStartIdx].HP_Rate = 0.f;
						else
							_RedPoint[RedStartIdx].HP_Rate = (REAL32)pChara->getCurHP() / (REAL32)(pChara->getFullHP());
					}

					RedStartIdx++;

					nPeopleCount++;
				}
			}
		}			
	}	

	// 플레이어의 수가 2이상이어야 화면에 표시합니다.
	if( nPeopleCount <= 1) return;

	if( BattleSlotContext::i()->getMyTeam() & CHARACTER_TEAM_BLUE)
	{
		LastPoint = _BluePoint;
	//	i3mem::Copy( _LastPoint, _BluePoint, sizeof( SCORE_ARRY) * MAX_TEAM_USER);
	}
	else
	{
		LastPoint = _RedPoint;
	//	i3mem::Copy( _LastPoint, _RedPoint, sizeof( SCORE_ARRY) * MAX_TEAM_USER);
	}

	INT32 nLastindex = 0;
	INT32 nCurindex = 0;

	REAL32 width_ratio = i3GuiRoot::getWidthRatioBy768();
	REAL32 height_ratio = i3GuiRoot::getHeightRatioBy768();

	for(i=0; i<MAX_TEAM_USER; i++)
	{
		if(i3::generic_empty( LastPoint[nCurindex].wstrNick) == false)
		{
			if( BattleSlotContext::i()->getMySlotIdx() == LastPoint[nCurindex].nIdx)
			{
				nCurindex++;
				continue;
			}

			// Rank		
			ShowRankMarkOnTraining( nLastindex, BattleSlotContext::i()->getSlotInfo( LastPoint[nCurindex].nIdx )->_Rank);
			m_pSpriteTrainingRankMark[nLastindex]->SetRect( 0, (REAL32)(nStartPosX * width_ratio), (REAL32)(nStartPosY - 5 + (nOffsetPosY * nLastindex) * height_ratio), 22.0f * width_ratio, 22.0f * height_ratio);
			// ID
			m_pTextTrainingUserNick[nLastindex]->SetEnable(true);
			m_pTextTrainingUserNick[nLastindex]->SetTextEllipsis( LastPoint[nCurindex].wstrNick);
			m_pTextTrainingUserNick[nLastindex]->setPos( (UINT32)((nStartPosX + 24) * width_ratio), (UINT32)(nStartPosY + (nOffsetPosY * nLastindex) * height_ratio));
			
			// Hero Kill Count
			i3::sprintf(wstrStackTemp, L": %d", AIModeContext::i()->UserScore[LastPoint[nCurindex].nIdx]);

			m_pTextTrainingUserScore[nLastindex]->SetEnable(true);
			m_pTextTrainingUserScore[nLastindex]->SetTextEllipsis(wstrStackTemp);
			m_pTextTrainingUserScore[nLastindex]->setPos( (UINT32)((nStartPosX + 124) * width_ratio), (UINT32)(nStartPosY + (nOffsetPosY * nLastindex) * height_ratio));

			// HP Text
			m_pTextTrainingHP[nLastindex]->SetEnable(true);				
			m_pTextTrainingHP[nLastindex]->setPos( (UINT32)((nStartPosX + 4) * width_ratio), (UINT32)((nStartPosY + (nOffsetPosY * nLastindex) * height_ratio) + (19 * height_ratio)));

			// HP Gauge				
			m_pSpriteTrainingHPGauge[nLastindex]->SetEnable(true);								
			m_pSpriteTrainingHPGauge[nLastindex]->SetRect( 0, (REAL32)((nStartPosX + 24) * width_ratio), (REAL32)(nStartPosY + (nOffsetPosY * nLastindex) * height_ratio) + (22 * height_ratio), (140.0f * width_ratio * LastPoint[nCurindex].HP_Rate), 3.0f* height_ratio);

			// HP의 값에 따라서 텍스트의 색상을 변경합니다.
			if( LastPoint[nCurindex].HP_Rate > 0.0f)
			{
				m_pTextTrainingUserNick[nLastindex]->SetColor(255, 255, 255, 255);
				m_pTextTrainingUserScore[nLastindex]->SetColor(255, 255, 255, 255);
				m_pTextTrainingHP[nLastindex]->SetColor(255, 255, 255, 255);					
			}
			else
			{
				m_pTextTrainingUserNick[nLastindex]->SetColor(200, 200, 200, 130);
				m_pTextTrainingUserScore[nLastindex]->SetColor(200, 200, 200, 130);
				m_pTextTrainingHP[nLastindex]->SetColor(200, 200, 200, 130);
			}

			nLastindex++;

			nCurindex++;
		}
	}

	if( nLastindex > 0)
	{
		m_pSpriteTrainingBG->SetEnable( true);
		REAL32 BG_start_x = (REAL32)(nStartPosX - 5) * width_ratio;
		REAL32 BG_start_y =  (REAL32)(nStartPosY- 8);
		REAL32 BG_Width =  (REAL32)(BG_start_x + ((nStartPosX + 160) * width_ratio));
		REAL32 BG_Height =  (REAL32)((nOffsetPosY)* (nLastindex)* height_ratio);
		m_pSpriteTrainingBG->SetRect( 0, BG_start_x, BG_start_y, BG_Width, BG_Height);
	}
}

void CHud_TrainingScore::ShowRankMarkOnTraining(INT32 nSlot,INT32 nRank)
{
	I3ASSERT(0 <= nSlot && MAX_ALL_USER > nSlot);
	I3ASSERT((nRank >= 0 && nRank < RANK_51+1) || GameUI::is_over_hero_rank(nRank));

	REAL32 fX = (REAL32)((nRank % GameGUI::RANK_MARK_HORZ_COUNT) * GameGUI::RANK_MARK_INTERVAL_SIZE);
	REAL32 fY = (REAL32)((nRank / GameGUI::RANK_MARK_HORZ_COUNT) * GameGUI::RANK_MARK_INTERVAL_SIZE);

	m_pSpriteTrainingRankMark[nSlot]->SetTextureCoord(0, fX, fY, fX + GameGUI::RANK_MARK_SIZE, fY + GameGUI::RANK_MARK_SIZE);
	m_pSpriteTrainingRankMark[nSlot]->SetEnable(0, true);
}



#if defined( ENABLE_BEST_RECORD)
void CHud_TrainingScore::UpdateTrainingRecord( REAL32 rDeltaSeconds)
{
	INT32 nStartPosX = 180;
	INT32 nStartPosY = 7 + 20;	

	// BG
	m_pSpriteTrainingRecordBG->SetEnable( true);
	REAL32 BG_start_x = (REAL32)(nStartPosX);
	REAL32 BG_start_y =  (REAL32)(nStartPosY);
	REAL32 BG_Width =  160.0f * i3GuiRoot::getWidthRatioBy768();
	REAL32 BG_Height =  53.0f * i3GuiRoot::getHeightRatioBy768();
	m_pSpriteTrainingRecordBG->SetRect( 0, BG_start_x, BG_start_y, BG_Width, BG_Height);

	// Stage Name
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_STAGE]->SetEnable( true);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_STAGE]->setPos( (UINT32)(BG_start_x + 5), (UINT32)(BG_start_y * i3GuiRoot::getHeightRatioBy768()) + 2);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_STAGE]->SetTextEllipsis( "포트아카바 [단체전]");

	// Record Score	
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_SCORE]->SetEnable( true);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_SCORE]->setPos( (UINT32)(BG_start_x + 5), (UINT32)(BG_start_y * i3GuiRoot::getHeightRatioBy768()) + 20);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_SCORE]->SetTextEllipsis( "랭킹 1위 점수 : 260000");

	// Nick
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_NICK]->SetEnable( true);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_NICK]->setPos( (UINT32)(BG_start_x + 5), (UINT32)(BG_start_y * i3GuiRoot::getHeightRatioBy768()) + 38);
	m_pTextTrainingRecord[HUD_TEXT_TRAINING_RECORD_NICK]->SetTextEllipsis( "닉네임 : 아에이오우아아");
}
#endif