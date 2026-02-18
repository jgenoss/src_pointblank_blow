#include "pch.h"

#if 지워질_것들

#include "BattleHUD_Dashboard_Boss.h"
#include "BattleHUD.h"
#include "GameStateMgr.h"
#include "ConfigRenderBattle.h"

I3_CLASS_INSTANCE( CHUD_Dashboard_Boss);//, CHUD_Dashboard);

const INT32		PANEL_CX	= 135;
const INT32		FONT_SZ		= 19;

CHUD_Dashboard_Boss::CHUD_Dashboard_Boss(void)
{
	INT32 i;

	m_pSprite		= NULL;

	for( i = 0; i < HUD_BSD_TEXT_COUNT; i++)
	{
		m_pTxt[i] = NULL;
	}
}

CHUD_Dashboard_Boss::~CHUD_Dashboard_Boss(void)
{
	INT32 i;

	I3_SAFE_NODE_RELEASE( m_pSprite);

	for( i = 0; i < HUD_BSD_TEXT_COUNT; i++)
	{
		I3_SAFE_NODE_RELEASE( m_pTxt[i]);
	}
}

bool CHUD_Dashboard_Boss::Create( CHud * pHUD)
{
	i3Node * pRoot;
	i3Viewer * pViewer		= g_pFramework->GetViewer();
	INT32	nScreenWidth	= pViewer->GetViewWidth();
	//INT32	nScreenHeight	= pViewer->GetViewHeight();
	INT32	x;

	m_pHUD = pHUD;

	pRoot = pHUD->getHUDRoot();

	m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD);

	// Score Frame
	{
		m_pSprite = i3Sprite2D::new_object();	
		m_pSprite->SetTexture( m_pTex );
		m_pSprite->Create( HUD_BSD_SPR_COUNT, TRUE, FALSE);

		INT32 nPosX = (nScreenWidth >> 1) - PANEL_CX;

		// Round 표시
		{
			// 왼쪽 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_ROUND_BACK_LEFT, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_ROUND_BACK_LEFT,		nPosX, 6,			42, 42,		881, 409);

			// 가운데 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_ROUND_BACK_CENTER, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_ROUND_BACK_CENTER,		nPosX + 42, 6,		42, 42,		926, 409);
			m_pSprite->SetRect( HUD_BSD_SPR_ROUND_BACK_CENTER, nPosX + 42.0f, 6.0f, 42.0f + 9.0f, 42.0f);

			// 오른쪽 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_ROUND_BACK_RIGHT, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_ROUND_BACK_RIGHT,		nPosX + 84 + 9, 6,		42, 42,		971, 409);

			// 슬래쉬 표시
			m_pSprite->SetEnable( HUD_BSD_SPR_ROUND_SLASH, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_ROUND_SLASH,			nPosX + 60, 6 + 11,		11, 20,		185, 452);
		}

		// Time 표시
		{
			nPosX = (nScreenWidth >> 1);

			// 왼쪽 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_TIME_BACK_LEFT, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_TIME_BACK_LEFT,			nPosX, 6,		42, 42,		881, 409);

			// 가운데 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_TIME_BACK_CENTER, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_TIME_BACK_CENTER,	nPosX + 42, 6,		42, 42,		926, 409);
			m_pSprite->SetRect( HUD_BSD_SPR_TIME_BACK_CENTER, nPosX + 42.0f, 6.0f, 42.0f + 9.0f, 42.0f);

			// 오른쪽 바탕
			m_pSprite->SetEnable( HUD_BSD_SPR_TIME_BACK_RIGHT, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_TIME_BACK_RIGHT,	nPosX + 84 + 9, 6,		42, 42,		971, 409);

			// 시계 마크
			m_pSprite->SetEnable( HUD_BSD_SPR_TIME_CLOCK, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_TIME_CLOCK,			nPosX + 17, 6 + 9,	26, 26,		462, 421);

			// Colon
			m_pSprite->SetEnable( HUD_BSD_SPR_TIME_COLON, TRUE);
			m_pSprite->Set( HUD_BSD_SPR_TIME_COLON,			nPosX + 78, 6 + 14,	5, 15,		176, 455);
		}

		pRoot->AddChild(m_pSprite);		
	}

	// HUD에서 사용하는 Text를 설정합니다.
	{
		x = (nScreenWidth >> 1);

		m_pTxt[HUD_BSD_TEXT_TIME_MIN] = i3TextNodeDX2::new_object();
		m_pTxt[HUD_BSD_TEXT_TIME_MIN]->Create( BATTLE_CHARA_SET, FONT_COUNT_32, FONT_SZ, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTxt[HUD_BSD_TEXT_TIME_MIN]->setSize( 40, 26);
		m_pTxt[HUD_BSD_TEXT_TIME_MIN]->setPos( x + 49 , 15);
		m_pTxt[HUD_BSD_TEXT_TIME_MIN]->SetColor( 211, 214, 214, 255);
		m_pTxt[HUD_BSD_TEXT_TIME_MIN]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		pRoot->AddChild( m_pTxt[HUD_BSD_TEXT_TIME_MIN]);

		m_pTxt[HUD_BSD_TEXT_TIME_SEC] = i3TextNodeDX2::new_object();
		m_pTxt[HUD_BSD_TEXT_TIME_SEC]->Create( BATTLE_CHARA_SET, FONT_COUNT_32, FONT_SZ, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTxt[HUD_BSD_TEXT_TIME_SEC]->setSize( 40, 26);
		m_pTxt[HUD_BSD_TEXT_TIME_SEC]->setPos( x + 88, 15);
		m_pTxt[HUD_BSD_TEXT_TIME_SEC]->SetColor( 211, 214, 214, 255);
		m_pTxt[HUD_BSD_TEXT_TIME_SEC]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		pRoot->AddChild( m_pTxt[HUD_BSD_TEXT_TIME_SEC]);

		x = (nScreenWidth >> 1) - PANEL_CX;
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND] = i3TextNodeDX2::new_object();
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->Create( BATTLE_CHARA_SET, FONT_COUNT_32, FONT_SZ, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->setSize( 40, 26);
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->setPos( x + 30, 15);
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->SetColor( 211, 214, 214, 255);
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		pRoot->AddChild( m_pTxt[HUD_BSD_TEXT_CUR_ROUND]);

		m_pTxt[HUD_BSD_TEXT_MAX_ROUND] = i3TextNodeDX2::new_object();
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->Create( BATTLE_CHARA_SET, FONT_COUNT_32, FONT_SZ, FW_NORMAL, FALSE, FALSE, ANTIALIASED_QUALITY);
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->setSize( 40, 26);
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->setPos( x + 77, 15);
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->SetColor( 211, 214, 214, 255);
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->SetAlign( TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		pRoot->AddChild( m_pTxt[HUD_BSD_TEXT_MAX_ROUND]);
	}

	return true;
}

void CHUD_Dashboard_Boss::Init(void)
{
	m_RedWinScore = 0;
	m_BlueWinScore = 0;

	// HUD Text 위치 초기화
	{
		{
			char goalText[MAX_STRING_COUNT];
			UINT8 RoomSubType = g_pGameContext->GetMyRoomSubType();

			INT32 round = LONIBBLE(RoomSubType);
			round = MINMAX(BATTLE_ROUND_TYPE_1, round, BATTLE_ROUND_TYPE_9);
			sprintf_s( goalText, "%dR", g_MissionRoundCountTable[round]);

			if ( g_pGameContext->IsStageMaskTraining() )
			{
				i3String::Copy(goalText, "∞", MAX_STRING_COUNT );
			}

			m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->SetText( goalText);
		}
	}
}

void CHUD_Dashboard_Boss::UpdateTime()
{
	char szTemp[16] = {0,};

	// Timer
	INT32 Minute = ((INT32) g_pGameContext->getBattleTime())/60;
	INT32 Second = ((INT32)g_pGameContext->getBattleTime())%60;

	if( Minute < 0) Minute = 0;
	if( Second < 0) Second = 0;

	if( (gstatemgr::i()->getStageState() == GAMESTATE_STATE_PRESTART_ROUND) ) {
		Minute = 0;
		Second = 0;
	}

	sprintf_s( szTemp, "%02d", Minute);
	m_pTxt[HUD_BSD_TEXT_TIME_MIN]->SetText( szTemp);

	sprintf_s( szTemp, "%02d", Second);
	m_pTxt[HUD_BSD_TEXT_TIME_SEC]->SetText( szTemp);
}

void CHUD_Dashboard_Boss::Update( REAL32 tm)
{
	//KEY : 김공룡
	// Score Sprite를 갱신합니다.	
	{
		char conv[128];

		sprintf_s( conv, "%dR", m_RedWinScore );
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->SetText(conv);
		sprintf_s( conv, "%dR", m_BlueWinScore );
		m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->SetText(conv);
	}
/*
	{ 
		char conv[128];

		sprintf_s( conv, "%dR", m_RedWinScore + m_BlueWinScore + 1);
		m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->SetText(conv);
	}
*/
	UpdateTime();
}

void CHUD_Dashboard_Boss::SetEnableWithoutNotice( BOOL bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	if( bFlag)
	{
	}
	else
	{
		if( nState == GAME_GUI_RESULT)
		{
			m_pSprite->SetEnable( FALSE);
			m_pTxt[HUD_BSD_TEXT_TIME_MIN]->SetEnable( FALSE);
			m_pTxt[HUD_BSD_TEXT_TIME_SEC]->SetEnable(FALSE);
			m_pTxt[HUD_BSD_TEXT_CUR_ROUND]->SetEnable(FALSE);
			m_pTxt[HUD_BSD_TEXT_MAX_ROUND]->SetEnable(FALSE);
		}
	}
}

#endif