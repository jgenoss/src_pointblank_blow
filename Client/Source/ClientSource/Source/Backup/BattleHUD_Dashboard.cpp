#include "pch.h"

#if 지워질_것들

#include "BattleHUD_Dashboard.h"
#include "BattleHUD.h"
#include "MovingText.h"
#include "ConfigRenderBattle.h"

I3_CLASS_INSTANCE( CHUD_Dashboard);//, i3GameObjBase);

CHUD_Dashboard::CHUD_Dashboard(void)
{
	INT32 i;

	m_RedWinScore		= 0;
	m_BlueWinScore		= 0;
	m_iTempBlueScore =0;
	m_iTempRedScore	=0;


	m_pTex					= NULL;
	m_pSpriteBattleInfo		= NULL;
	m_pSpriteBattleInfoDino		= NULL;

	for( i = 0; i < HUD_TEXT_COUNT; i++)
	{
		m_pText[i] = NULL;
	}

	m_pMovingText = NULL;
	m_pHUD = NULL;
	m_pTexDino =  NULL;
}

CHUD_Dashboard::~CHUD_Dashboard(void)
{
	INT32 i;

	I3_SAFE_RELEASE( m_pTex);
	I3_SAFE_RELEASE( m_pTexDino);

	I3_SAFE_NODE_RELEASE(m_pSpriteBattleInfo);
	I3_SAFE_NODE_RELEASE(m_pSpriteBattleInfoDino);

	for( i = 0; i < HUD_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pText[i]);
	}
	I3_SAFE_RELEASE(m_pMovingText);
}

bool CHUD_Dashboard::Create( CHud * pHUD)
{
	INT32 i;
	i3Node * pRoot;
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	//INT32	nScreenHeight	= pViewer->GetViewHeight();

	m_pHUD = pHUD;

	pRoot = pHUD->getHUDRoot();

	
	if(g_pGameContext->IsTutorialMode())
	{
		m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
	}
	else
	{
		m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);
		m_pTexDino = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);
	}
	

	
	// Score Frame
	{
		REAL32 fOffsetY_Clan = 0.0f;

		if( g_pGameContext->GetBattleChannel())
		{
			fOffsetY_Clan = 18.0f;
		}
		
		m_pSpriteBattleInfoDino = i3Sprite2D::new_object();	
		m_pSpriteBattleInfoDino->SetTexture( m_pTexDino );


		m_pSpriteBattleInfo = i3Sprite2D::new_object();	
		m_pSpriteBattleInfo->SetTexture( m_pTex );
		m_pSpriteBattleInfo->Create( HUD_BATTLE_INFO_COUNT, TRUE, TRUE);
		m_pSpriteBattleInfoDino->Create( HUD_BATTLE_INFO_DINO_COUNT, TRUE, TRUE);
		
		INT32 nPosX = (nScreenWidth >> 1) - (266 >> 1);
		INT32 nPosTimeX = (nScreenWidth >> 1) - (74 >> 1);
		m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_SCORE, TRUE);
		m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_SCORE, (REAL32)nPosX, (fOffsetY_Clan+6.0f), (266.0f), (43.0f));//65
		m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_SCORE, 255,255,255,255);
		
		//Time		
		m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_TIME, TRUE);
		m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_TIME, (REAL32)nPosTimeX, (fOffsetY_Clan+50.0f), (72.0f), (19.0f));//65
		m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_TIME, 255,255,255,255);

		if(g_pGameContext->IsTutorialMode())
		{
			m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_SCORE, (6.0f), (217.0f), (270.0f), (260.0f));//159
			m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_TIME, (98.0f), (259.0f), (172.0f), (280.0f));//159

		}
		else
		{
			m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_SCORE, (4.0f), (116.0f), (270.0f), (159.0f));//159
			m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_TIME, (98.0f), (160.0f), (170.0f), (179.0f));//159
		}

		

		if(g_pGameContext->IsTutorialMode() == false)
		{
			// 자신이 속한팀 표시
			{
				if( g_pGameContext->getMyTeam() & CHARACTER_TEAM_BLUE)
				{
					m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_TEAM_RED, FALSE);
					m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_TEAM_BLUE, TRUE);
				}
				else
				{
					m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_TEAM_BLUE, FALSE);
					m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_TEAM_RED, TRUE);
				}

				m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_TEAM_RED, (REAL32)(nPosX + 16.0f), (REAL32)(fOffsetY_Clan+13.5f), (74.0f), (27.0f));
				m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_TEAM_RED, 255,255,255,255);
				m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_TEAM_RED, 252.0f, 3.0f, 326.0f, 30.0f);

				m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_TEAM_BLUE, (REAL32)(nPosX + 175.0f), (REAL32)(fOffsetY_Clan+13.5f), (74.0f), (27.0f));
				m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_TEAM_BLUE, 255,255,255,255);
				m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_TEAM_BLUE, 252.0f, 36.0f, 326.0f, 63.0f);
			}
		}
		// EscapeMission
		{
			if(g_pGameContext->IsBossMissionMode())
			{

				m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_ESCAPEARROW, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, TRUE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, TRUE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, TRUE);
				
			}
			else
			{
				m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_ESCAPEARROW, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, FALSE);


			}
			

			if(g_pGameContext->IsBossMissionMode())
			{

				//탈출미션 

				m_pSpriteBattleInfoDino->SetRect( HUD_BATTLE_INFO_ESCAPESHILD_0,  (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));//65
				m_pSpriteBattleInfoDino->SetTextureCoord( HUD_BATTLE_INFO_ESCAPESHILD_0, (542.0f), (155.0f), (570.0f), (185.0f));//159
				m_pSpriteBattleInfoDino->SetColor( HUD_BATTLE_INFO_ESCAPESHILD_0, 255,255,255,255);

				//탈출미션 

				m_pSpriteBattleInfoDino->SetRect( HUD_BATTLE_INFO_ESCAPESHILD_1,  (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));//65
				m_pSpriteBattleInfoDino->SetTextureCoord( HUD_BATTLE_INFO_ESCAPESHILD_1, (582.0f), (155.0f), (610.0f), (185.0f));//159
				m_pSpriteBattleInfoDino->SetColor( HUD_BATTLE_INFO_ESCAPESHILD_1, 255,255,255,255);

				//탈출미션 

				m_pSpriteBattleInfoDino->SetRect( HUD_BATTLE_INFO_ESCAPESHILD_2,  (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));//65
				m_pSpriteBattleInfoDino->SetTextureCoord( HUD_BATTLE_INFO_ESCAPESHILD_2, (622.0f), (155.0f), (650.0f), (185.0f));
				m_pSpriteBattleInfoDino->SetColor( HUD_BATTLE_INFO_ESCAPESHILD_2, 255,255,255,255);

				m_pSpriteBattleInfoDino->SetRect( HUD_BATTLE_INFO_ESCAPESHILD_3,  (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));//65
				m_pSpriteBattleInfoDino->SetTextureCoord( HUD_BATTLE_INFO_ESCAPESHILD_3, (662.0f), (155.0f), (690.0f), (185.0f));
				m_pSpriteBattleInfoDino->SetColor( HUD_BATTLE_INFO_ESCAPESHILD_3, 255,255,255,255);

				m_pSpriteBattleInfoDino->SetRect( HUD_BATTLE_INFO_ESCAPESHILD_4,  (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));//65
				m_pSpriteBattleInfoDino->SetTextureCoord( HUD_BATTLE_INFO_ESCAPESHILD_4, (702.0f), (155.0f), (740.0f), (185.0f));
				m_pSpriteBattleInfoDino->SetColor( HUD_BATTLE_INFO_ESCAPESHILD_4, 255,255,255,255);


				m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_ESCAPEARROW, (REAL32)(nPosX+121.0f), (REAL32)(fOffsetY_Clan+13.5f), (24.0f), (27.0f));
				m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_ESCAPEARROW, 255,255,255,255);
				m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_ESCAPEARROW, 157.0f, 416.0f, 181.0f, 442.0f);

			}

		}

		// Clan
		if( g_pGameContext->GetBattleChannel())		
		{
			m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_CLAN, TRUE);
			m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_CLAN, (REAL32)nPosX, (0.0f), (265.0f), (31.0f));
			m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_CLAN, 255,255,255,255);
			m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_CLAN, (511.0f), (246.0f), (776.0f), (281.0f));
		}

		pRoot->AddChild(m_pSpriteBattleInfo);	
		pRoot->AddChild(m_pSpriteBattleInfoDino);		
	}

	// HUD에서 사용하는 Text를 설정합니다.
	{
		for(i=0; i<HUD_TEXT_COUNT; i++)
		{
			m_pText[i] = i3TextNodeDX2::new_object();

			switch(i)
			{
			case HUD_TEXT_TIME:
				m_pText[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 19, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
				m_pText[i]->setSize(83, 26);
				break;
			case HUD_TEXT_COLON:
				m_pText[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 19, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
				m_pText[i]->setSize(83, 26);
				break;
			case HUD_TEXT_INFO_RED:
				m_pText[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 25, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
				m_pText[i]->setSize(75, 40);
				break;
			case HUD_TEXT_INFO_BLUE:
				m_pText[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 25, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
				m_pText[i]->setSize(75, 40);
				break;
			case HUD_TEXT_INFO_GOAL:
				m_pText[i]->Create(BATTLE_CHARA_SET, FONT_COUNT_32, 23, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
				m_pText[i]->setSize(75, 40);
			}

			m_pText[i]->setPos(0, 0);

			if (HUD_TEXT_INFO_GOAL == i)
			{
				m_pText[i]->SetColor(205, 205, 50, 255);
			}
			else
			{
				m_pText[i]->SetColor(180, 180, 180, 255);
			}

			m_pText[i]->SetAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			pRoot->AddChild( m_pText[i]);
		}
	}

	m_pMovingText = CMovingText::new_object();
	m_pMovingText->Create(pRoot);
	I3_GAMENODE_ADDCHILD(this, m_pMovingText);


	return true;
}

void CHUD_Dashboard::Init(void)
{
	m_RedWinScore = 0;
	m_BlueWinScore = 0;


	g_pGameContext->setEscapeBlock(TOUCHDOWN_BLOCK_STATE_INIT);
	g_pGameContext->SetEscapeBlocktime(0.0f);

	// HUD Text 위치 초기화
	{
		INT32 nOffsetY_Clan = 0;

		if( g_pGameContext->GetBattleChannel())
		{
			nOffsetY_Clan = 18;
		}

		i3Viewer * pViewer		= g_pFramework->GetViewer();
		INT32	nScreenWidth	= pViewer->GetViewWidth();
		INT32	nScreenHeight	= pViewer->GetViewHeight();

		INT32 nPosX = 0;
		INT32 nPosY = 0;

		nPosX = (nScreenWidth >> 1) - (84 >> 1);
		nPosY = nScreenHeight - 96;

		m_pText[HUD_TEXT_TIME]->setPos( nPosX, 48+nOffsetY_Clan);
		m_pText[HUD_TEXT_COLON]->setPos( nPosX, 45+nOffsetY_Clan);
		
		{
			POINT2D ptCenter = g_pFramework->GetCenterPosition(266, 0);

			m_pText[HUD_TEXT_INFO_RED]->setPos(ptCenter.x + 15, 8+nOffsetY_Clan);
			m_pText[HUD_TEXT_INFO_BLUE]->setPos(ptCenter.x + 175, 8+nOffsetY_Clan);

			
			char goalText[MAX_STRING_COUNT];
			UINT8 RoomSubType = g_pGameContext->GetMyRoomSubType();

			if (g_pGameContext->IsKillMode())
			{
				INT32 killCount = LONIBBLE(RoomSubType);
				killCount = MINMAX(BATTLE_KILL_TYPE_60, killCount, BATTLE_KILL_TYPE_160);
				sprintf_s(goalText, "%d", g_TeamKillCountTable[killCount]);
			}
			else
			{
				INT32 round = LONIBBLE(RoomSubType);
				round = MINMAX(BATTLE_ROUND_TYPE_1, round, BATTLE_ROUND_TYPE_9);				
				sprintf_s(goalText, "%dR", g_MissionRoundCountTable[round]);
			}

			m_pText[HUD_TEXT_INFO_GOAL]->setPos(ptCenter.x + 96, 9+nOffsetY_Clan);
		
			if ( g_pGameContext->IsStageMaskTraining() )
			{
		
				m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_LIMIT, TRUE);
				m_pSpriteBattleInfo->SetRect( HUD_BATTLE_INFO_LIMIT, (REAL32)(ptCenter.x + 118), (REAL32) (20+nOffsetY_Clan), (24.0f), (12.0f));//65
				m_pSpriteBattleInfo->SetColor( HUD_BATTLE_INFO_LIMIT, 255,255,255,255);
				m_pSpriteBattleInfo->SetTextureCoord( HUD_BATTLE_INFO_LIMIT, (40.0f), (164.0f), (64.0f), (176.0f));//159


			}
			else if (g_pGameContext->IsBossMissionMode())
			{
				m_pSpriteBattleInfo->SetEnable( HUD_BATTLE_INFO_ESCAPEARROW, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, TRUE);
			}
			else if(g_pGameContext->IsTutorialMode())
			{
				// 아무것도 하지 않음
			}
			else
			{
				m_pText[HUD_TEXT_INFO_GOAL]->SetText(goalText);
			}
		}
	}
}

void CHUD_Dashboard::SetScore( INT32 red, INT32 blue)
{
	m_RedWinScore = red;
	m_BlueWinScore = blue;
}

void CHUD_Dashboard::UpdateTime()
{
	char szTemp[16] = {0,};

	// Timer
	INT32 Minute = ((INT32) g_pGameContext->getBattleTime()+1)/60;
	INT32 Second = ((INT32)g_pGameContext->getBattleTime()+1)%60;

	if( Minute < 0) Minute = 0;
	if( Second < 0) Second = 0;

		

	if(g_pGameContext->getBattleTime()<0.1f)
		Second = 0;
	

	sprintf_s( szTemp, "%02d   %02d", Minute, Second);
	m_pText[HUD_TEXT_TIME]->SetText( szTemp);
	m_pText[HUD_TEXT_COLON]->SetText(":");
}

void CHUD_Dashboard::Update( REAL32 tm)
{
	// Score Sprite를 갱신합니다.	
	{ 
		char strScore[16];

		if(g_pGameContext->IsTutorialMode() == false && g_pGameContext->IsBossMissionMode()== false)
		{
			sprintf_s(strScore, "%03d", m_RedWinScore);
			m_pText[HUD_TEXT_INFO_RED]->SetText(strScore);

			sprintf_s(strScore, "%03d", m_BlueWinScore);
			m_pText[HUD_TEXT_INFO_BLUE]->SetText(strScore);
		}

		if(g_pGameContext->IsBossMissionMode())
		{
/*
			sprintf_s(strScore, "%03d", m_RedWinScore);
			m_pText[HUD_TEXT_INFO_RED]->SetText(strScore);

			sprintf_s(strScore, "%03d", m_BlueWinScore);
			m_pText[HUD_TEXT_INFO_BLUE]->SetText(strScore);
*/
			// 점수 표시
			if (m_updateTimer < 0.8f)
			{
				m_updateTimer += tm;

				if (0.8f <= m_updateTimer)
				{
//					char scoreText[MAX_STRING_COUNT];

					itoa(m_iRedScore, strScore, 10);
					m_pText[HUD_TEXT_INFO_RED]->SetText(strScore);
					if (m_iRedScore != m_iTempRedScore)
					{
						m_pText[HUD_TEXT_INFO_RED]->SetColor(255, 211, 3, 255);		
					}
					
					itoa(m_iBlueScore, strScore, 10);
					m_pText[HUD_TEXT_INFO_BLUE]->SetText(strScore);
					if (m_iBlueScore != m_iTempBlueScore)
					{
						m_pText[HUD_TEXT_INFO_BLUE]->SetColor(255, 211, 3, 255);		
					}
					
					m_iTempRedScore=m_iRedScore;
					m_iTempBlueScore=m_iBlueScore;
				}
			}
			else if (m_updateTimer < 0.9f)
			{
				m_updateTimer += tm;
				
				if (m_iRedScore != m_iTempRedScore)
				{
					m_pText[HUD_TEXT_INFO_RED]->SetColor(32, 171, 0, 255);
				}
				if (m_iBlueScore != m_iTempBlueScore)
				{
					m_pText[HUD_TEXT_INFO_BLUE]->SetColor(32, 171, 0, 255);
				}
			
			}
			else
			{
				m_pText[HUD_TEXT_INFO_RED]->SetColor(255, 255, 255, 255);
				m_pText[HUD_TEXT_INFO_BLUE]->SetColor(255, 255, 255, 255);
			}






			g_pGameContext->UpdateEscapeBlockTime(tm);
					

			switch(g_pGameContext->getEscapeBlock())
			{

			case TOUCHDOWN_BLOCK_STATE_INIT :
					

				if( g_pGameContext->getEscapeBlocktime() > -6)
				{
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, TRUE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, FALSE);

				}
				else if( g_pGameContext->getEscapeBlocktime() > -13)
				{
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, TRUE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, FALSE);

				}
				else if (g_pGameContext->getEscapeBlocktime() > -19)
				{
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, TRUE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, FALSE);

				}
				else if(g_pGameContext->getEscapeBlocktime() > -26)
				{
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, FALSE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, TRUE);
					m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, FALSE);

				}



				break;


			case TOUCHDOWN_BLOCK_STATE_FULL:
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_0, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_1, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_2, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_3, FALSE);
				m_pSpriteBattleInfoDino->SetEnable( HUD_BATTLE_INFO_ESCAPESHILD_4, TRUE);
				g_pGameContext->SetEscapeBlocktime(0.0f);
				g_pGameContext->setEscapeBlock(TOUCHDOWN_BLOCK_STATE_INIT);

			}
#ifdef CC_DINO
		else if (g_pGameContext->IsCrossCountMode())
		{
			UpdateRound();
		}
#endif

		}
	}

	UpdateTime();
}

void CHUD_Dashboard::UpdateRound()
{
	char goalText[MAX_STRING_COUNT] = {0,};
	sprintf_s(goalText, "%dR", g_pGameContext->getCurrentRound());
	m_pText[HUD_TEXT_INFO_GOAL]->SetText(goalText);
}

void CHUD_Dashboard::SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	if( bFlag)
	{
	}
	else
	{
		if( nState == GAME_GUI_RESULT)
		{
			m_pSpriteBattleInfoDino->SetEnable( FALSE);
			m_pSpriteBattleInfo->SetEnable( FALSE);
			m_pText[HUD_TEXT_TIME]->SetEnable( FALSE);
			m_pText[HUD_TEXT_COLON]->SetEnable(FALSE);
			m_pText[HUD_TEXT_INFO_RED]->SetEnable(FALSE);
			m_pText[HUD_TEXT_INFO_BLUE]->SetEnable(FALSE);
			m_pText[HUD_TEXT_INFO_GOAL]->SetEnable(FALSE);
		}
	}
}



void CHUD_Dashboard::SetEscapeScore(INT32 RedScore, INT32 BlueScore)
{
	INT32 AddScore;
	INT32 mySlotIdx = g_pGameContext->getMySlotIdx();

	//기존스코어와 달라진게 없을 경우	
	if (RedScore == m_iRedScore && m_iBlueScore == BlueScore)
	{
		m_iRedScore = RedScore;
		m_iBlueScore = BlueScore;
	}//기존스코어와 변경된 사항이 있는경우
	else
	{
		//레드 스코어가 변경된 경우
		if (RedScore != m_iRedScore)
		{	
			AddScore = RedScore-m_iRedScore;
			if( g_pGameContext->IsRedTeam( mySlotIdx) )
			{
				if (g_pGameContext->GetTouchDownSlotIdx() == mySlotIdx ||g_pGameContext->GetEscapeMyKill())
				{
					m_pHUD->SetEscapeScore(TRUE,AddScore);
				}
			}
			m_iRedScore = RedScore;
		}
		
		//블루 스코어가 변경된 경우
		if (BlueScore != m_iBlueScore)
		{	
			AddScore = BlueScore-m_iBlueScore;
			if(!g_pGameContext->IsRedTeam( mySlotIdx))
			{
				if (g_pGameContext->GetTouchDownSlotIdx() == mySlotIdx ||g_pGameContext->GetEscapeMyKill())
				{
					m_pHUD->SetEscapeScore(FALSE,AddScore);
				}
			}
	
			m_iBlueScore = BlueScore;
		}

		m_updateTimer = 0.0f;
	}
}
#endif