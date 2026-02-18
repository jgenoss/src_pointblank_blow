#include "pch.h"

#if 지워질_것들

#include "TrainingScoreBoard.h"
#include "StageBattle.h"

I3_CLASS_INSTANCE(CTrainingScoreBoard);//, i3GameObjBase);

CTrainingScoreBoard::CTrainingScoreBoard()
{
	INT32 i = 0;

	m_pSceneNode = i3Node::new_object();

	m_scoreBoardTexture = NULL;
	m_scoreBoardNode = NULL;

	for( i=0; i<TR_SCORE_BOARD_TEXT_COUNT; i++)
	{
		m_scoreTextNode[i] = NULL;
	}

	m_updateTimer = 0.0f;
	m_score = 0;
}

CTrainingScoreBoard::~CTrainingScoreBoard()
{
	INT32 i = 0;

	I3_SAFE_RELEASE(m_scoreBoardTexture);
	I3_SAFE_NODE_RELEASE(m_scoreBoardNode);

	for( i=0; i<TR_SCORE_BOARD_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE(m_scoreTextNode[i]);
	}
	
}

BOOL CTrainingScoreBoard::Create(i3Node *pParentNode, BOOL bEnable)
{
	INT32 i = 0;

	BOOL bResult = i3GameObjBase::Create(pParentNode, bEnable);

	if (!bResult)
	{
		return bResult;
	}


	m_scoreBoardTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);

	m_scoreBoardNode = i3Sprite2D::new_object();
	I3ASSERT(m_scoreBoardNode != NULL);
	m_scoreBoardNode->SetTexture(m_scoreBoardTexture);
	m_scoreBoardNode->Create(6, TRUE, FALSE, FALSE);

	m_scoreBoardNode->SetTextureCoord(0, 717.0f, 263.0f, 741.0f, 325.0f);
	m_scoreBoardNode->SetRect(0, 10.0f, 187.0f, 23.0f, 63.0f);
	m_scoreBoardNode->SetEnable(0, bEnable);

	m_scoreBoardNode->SetTextureCoord(1, 744.0f, 263.0f, 768.0f, 325.0f);
	m_scoreBoardNode->SetRect(1, 33.0f, 187.0f, 122.0f, 63.0f);
	m_scoreBoardNode->SetEnable(1, bEnable);

	m_scoreBoardNode->SetTextureCoord(2, 771.0f, 263.0f, 795.0f, 325.0f);
	m_scoreBoardNode->SetRect(2, 155.0f, 187.0f, 20.0f, 63.0f);
	m_scoreBoardNode->SetEnable(2, bEnable);


	m_scoreBoardNode->SetTextureCoord(3, 717.0f, 263.0f, 741.0f, 325.0f);
	m_scoreBoardNode->SetRect(3, 10.0f, 250.0f, 23.0f, 63.0f);
	m_scoreBoardNode->SetEnable(3, bEnable);

	m_scoreBoardNode->SetTextureCoord(4, 744.0f, 263.0f, 768.0f, 325.0f);
	m_scoreBoardNode->SetRect(4, 33.0f, 250.0f, 122.0f, 63.0f);
	m_scoreBoardNode->SetEnable(4, bEnable);

	m_scoreBoardNode->SetTextureCoord(5, 771.0f, 263.0f, 795.0f, 325.0f);
	m_scoreBoardNode->SetRect(5, 155.0f, 250.0f, 20.0f, 63.0f);
	m_scoreBoardNode->SetEnable(5, bEnable);

	GetNode()->AddChild(m_scoreBoardNode);


	for( i=0; i<TR_SCORE_BOARD_TEXT_COUNT; i++)
	{
		m_scoreTextNode[i] = i3TextNodeDX2::new_object();	
		I3ASSERT(m_scoreTextNode[i] != NULL);
				
		if( i == TR_SCORE_BOARD_TEXT_POINT_GET_KEY || i == TR_SCORE_BOARD_TEXT_LEVEL_KEY)
		{
			m_scoreTextNode[i]->Create( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE);
			m_scoreTextNode[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
			
			m_scoreTextNode[i]->setSize(155, 20);
			m_scoreTextNode[i]->SetColor(255, 255, 255, 255);	

			if( i==TR_SCORE_BOARD_TEXT_POINT_GET_KEY)
			{
				m_scoreTextNode[i]->setPos(15, 192);	
				m_scoreTextNode[i]->SetText(GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_SCORE_GET_POINT"));
			}
			else
			{
				m_scoreTextNode[i]->setPos(15, 254);	
				m_scoreTextNode[i]->SetText(GAME_STRING("STR_TBL_GUI_READY_DIFFICULTY"));
			}
		}
		else
		{
			if( i==TR_SCORE_BOARD_TEXT_POINT_GET)
			{
				m_scoreTextNode[i]->Create( BATTLE_CHARA_SET, FONT_COUNT_32, 32);
				m_scoreTextNode[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
				m_scoreTextNode[i]->setPos(15, 193);
				m_scoreTextNode[i]->SetColor(32, 171, 0, 255);	
				m_scoreTextNode[i]->setSize(155, 70);
			}			
			else if( i == TR_SCORE_BOARD_TEXT_LEVEL)
			{
				m_scoreTextNode[i]->Create( BATTLE_CHARA_SET, 8, 28);
				m_scoreTextNode[i]->setPos(15, 255);				
				m_scoreTextNode[i]->SetColor(255, 153, 51, 255);	
				m_scoreTextNode[i]->setSize(155, 70);
				m_scoreTextNode[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			}
			else if( i == TR_SCORE_BOARD_TEXT_LEVEL_TEXT)
			{
#if defined (LOCALE_RUSSIA)
				m_scoreTextNode[i]->Create( GetDefaultFontName(), 16, 12);
#elif(LOCALE_CHINA)
				m_scoreTextNode[i]->Create( GetDefaultFontName(), 16, GAME_FONT_SIZE_HUD_MISSION_NOTICE);
#else
				m_scoreTextNode[i]->Create( BATTLE_CHARA_SET, 16, 20);
#endif
				m_scoreTextNode[i]->setPos(15, 255);				
				m_scoreTextNode[i]->SetColor(255, 153, 51, 255);	
				m_scoreTextNode[i]->setSize(155, 70);
				m_scoreTextNode[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
			}
			else
			{
				I3ASSERT_0;
			}
		}

		m_scoreTextNode[i]->setAutoWrapEnable(TRUE);
		m_scoreTextNode[i]->SetEnable(bEnable);
		
		GetNode()->AddChild(m_scoreTextNode[i]);
	}


	return TRUE;
}

void CTrainingScoreBoard::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	char szTemp[256];
	char szTemp2[256];
	szTemp[0] = 0;
	szTemp2[0] = 0;
	
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	// 점수 표시
	if (m_updateTimer < 0.8f)
	{
		m_updateTimer += rDeltaSeconds;

		if (0.8f <= m_updateTimer)
		{
			char scoreText[MAX_STRING_COUNT];
		
			itoa(m_score, scoreText, 10);
			m_scoreTextNode[TR_SCORE_BOARD_TEXT_POINT_GET]->SetText(scoreText);
			m_scoreTextNode[TR_SCORE_BOARD_TEXT_POINT_GET]->SetColor(255, 211, 3, 255);			
		}
	}
	else if (m_updateTimer < 0.9f)
	{
		m_updateTimer += rDeltaSeconds;
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_POINT_GET]->SetColor(255, 255, 255, 255);
	}
	else
	{
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_POINT_GET]->SetColor(32, 171, 0, 255);
	}

	// Level표시
	switch(g_pGameContext->getCurrentDifficultyLevel())
	{
	case 10:
	case 9:
	case 8:
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL]->setPos(53, 255);
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL_TEXT]->setPos(83, 254);
		sprintf_s(szTemp, "%d", g_pGameContext->getCurrentDifficultyLevel());
		sprintf_s(szTemp2, GAME_STRING("STR_TBL_GUI_AI_LEVEL_HARD"));
		break;

	case 7:
	case 6:
	case 5:
	case 4:
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL]->setPos(44, 255);
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL_TEXT]->setPos(65, 254);
		sprintf_s(szTemp, "%d", g_pGameContext->getCurrentDifficultyLevel());
		sprintf_s(szTemp2, GAME_STRING("STR_TBL_GUI_AI_LEVEL_NORMAL"));
		break;

	case 3:
	case 2:
	case 1:
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL]->setPos(55, 255);
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL_TEXT]->setPos(78, 254);
		sprintf_s(szTemp, "%d", g_pGameContext->getCurrentDifficultyLevel());
		sprintf_s(szTemp2, GAME_STRING("STR_TBL_GUI_AI_LEVEL_EASY"));
		break;

	default:
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL]->setPos(70, 255);
		m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL_TEXT]->setPos(15, 255);
		sprintf_s(szTemp, "%d", g_pGameContext->getCurrentDifficultyLevel());
		break;
	}

	m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL]->SetText(szTemp);
	m_scoreTextNode[TR_SCORE_BOARD_TEXT_LEVEL_TEXT]->SetText(szTemp2);
}

void CTrainingScoreBoard::SetScore(INT32 score)
{
	m_score = score;
	m_updateTimer = 0.0f;
}
#endif