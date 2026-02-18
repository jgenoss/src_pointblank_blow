#include "pch.h"
#include "BattleGUI_Result.h"
#include "GuiNotifyReceiver.h"
#include "BattleGui.h"
#include "StageBattle.h"
#include "GameStateMgr.h"

I3_CLASS_INSTANCE( CBattleGUI_Result); //, i3GameObjBase);

CBattleGUI_Result::CBattleGUI_Result(void)
{
	INT32 i;

	m_pStaticResultBG = NULL;
	for( i=0; i<RESULT_STATIC_COUNT; i++)	m_pStaticResult[i] = NULL;

	for( i=0; i<MAX_TEAM_SLOT_COUNT; i++)
	{
		m_pEditBoxTop_ID[i] = NULL;
		m_pEditBoxTop_Kill[i] = NULL;
		m_pEditBoxTop_Death[i] = NULL;
		m_pEditBoxTop_Pino[i] = NULL;
		m_pEditBoxTop_Exp[i] = NULL;
		m_pStaticTop_Rank[i] = NULL;
		m_pStaticTop_Beginer[i] = NULL;
		m_pEditBoxTop_Mission[i] = NULL;
		m_pEditBoxTop_Score[i] = NULL;
		m_pStaticTop_Challenge[i] = NULL;
		m_pStaticTop_PC_Room[i] = NULL;
		m_pStaticTop_Item[i] = NULL;
		m_pStaticTop_Event[i] = NULL;
		m_pStaticTop_ClanMark[i] = NULL;
		m_pStaticTop_ESport[i] = NULL;

		m_pEditBoxBottom_ID[i] = NULL;
		m_pEditBoxBottom_Kill[i] = NULL;
		m_pEditBoxBottom_Death[i] = NULL;
		m_pEditBoxBottom_Pino[i] = NULL;
		m_pEditBoxBottom_Exp[i] = NULL;
		m_pStaticBottom_Rank[i] = NULL;
		m_pStaticBottom_Beginer[i] = NULL;
		m_pEditBoxBottom_Mission[i] = NULL;
		m_pEditBoxBottom_Score[i] = NULL;
		m_pStaticBottom_Challenge[i] = NULL;
		m_pStaticBottom_PC_Room[i] = NULL;
		m_pStaticBottom_Item[i] = NULL;
		m_pStaticBottom_Event[i] = NULL;
		m_pStaticBottom_ClanMark[i] = NULL;
		m_pStaticBottom_ESport[i] = NULL;
	}

	m_pButtonResultConfirm = NULL;

	m_pTeamBarTop = NULL;
	m_pTeamBarBottom = NULL;
	m_pTeamResultTop = NULL;
	m_pTeamResultBottm = NULL;
	m_pSelectBar = NULL;

	m_nWinResultKillCount = 0;
	m_nLoseResultKillCount = 0;

	
	for(i = 0; i < 11; i++)
	{
		m_pWinBarText[i] = NULL;
		m_pLoseBarText[i] = NULL;
	}

	m_bEndCalcResult = FALSE;

	m_resultState = RS_DRAW;

	for( i=0; i<8; i++)
	{
		m_TopPoint[ i ].Reset();
		m_BottomPoint[ i ].Reset();
	}

	m_nTopTotalPoint = 0;
	m_nBottomTotalPoint = 0;
	m_nMaxExpPerBattle = 0;

	m_pGUI = NULL;
	m_pStaticTop_LostWorldResult = NULL;
	m_pStaticBottom_LostWorldResult = NULL;
}

CBattleGUI_Result::~CBattleGUI_Result(void)
{
}

bool CBattleGUI_Result::Create( CBattleGui * pGUI, const i3::vector_set<STRCINFO*>* plist, i3GuiControl * pControl)
{
	INT32 i;

	m_pGUI = pGUI;

	LinkControlEx(*plist,  "Result_BG", m_pStaticResultBG, -1);

	LinkControlEx(*plist,  "Red_Score1", m_pStaticResult[RESULT_STATIC_RED_SCORE1] , -1);
	LinkControlEx(*plist,  "Red_Score2", m_pStaticResult[RESULT_STATIC_RED_SCORE2] , -1);
	LinkControlEx(*plist,  "Red_Score3", m_pStaticResult[RESULT_STATIC_RED_SCORE3], -1);

	LinkControlEx(*plist,  "Blue_Score1", m_pStaticResult[RESULT_STATIC_BLUE_SCORE1], -1);
	LinkControlEx(*plist,  "Blue_Score2", m_pStaticResult[RESULT_STATIC_BLUE_SCORE2], -1);
	LinkControlEx(*plist,  "Blue_Score3", m_pStaticResult[RESULT_STATIC_BLUE_SCORE3], -1);

	//탈출 미션 위쪽 escape 카운터
	LinkControlEx(*plist,  "LostWorld_ScoreTop1", m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE1], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreTop2", m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE2], -1);

	//탈출 미션 위쪽 Kill 카운터
	LinkControlEx(*plist,  "LostWorld_ScoreTop3", m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE3], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreTop4", m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreTop5", m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5], -1);
	
	//탈출 미션 아래쪽 escape 카운터
	LinkControlEx(*plist,  "LostWorld_ScoreBottom1", m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE1], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreBottom2", m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE2], -1);

	//탈출 미션 아래쪽 Kill 카운터
	LinkControlEx(*plist,  "LostWorld_ScoreBottom3", m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE3], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreBottom4", m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4], -1);
	LinkControlEx(*plist,  "LostWorld_ScoreBottom5", m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5], -1);


	INT32 offsetEscape = LocaleValue::GetInt32("WinPosition/EscapeMission_Escape");
	INT32 offsetPos = 0;

	for(int i = RESULT_STATIC_LOSTWORLD_TOPSCORE1; i <= RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5; ++i )
	{
		switch( i )
		{
		case RESULT_STATIC_LOSTWORLD_TOPSCORE1:	case RESULT_STATIC_LOSTWORLD_TOPSCORE2:	
		case RESULT_STATIC_LOSTWORLD_BOTTOMSCORE1:	case RESULT_STATIC_LOSTWORLD_BOTTOMSCORE2:
			offsetPos = offsetEscape;
			break;
		default:
			offsetPos = offsetEscape;
			break;
		}
		m_pStaticResult[i]->setPositionX( m_pStaticResult[i]->getPositionX() + offsetPos);
	}

	char szTemp[MAX_STRING_COUNT];

	//RED TEAM
	for( i = 0; i < MAX_TEAM_SLOT_COUNT; i++)
	{	
		sprintf_s(szTemp, "RedSlot_ID_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_ID[i], -1);

		sprintf_s(szTemp, "RedSlot_Kill_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Kill[i], -1);

		sprintf_s(szTemp, "RedSlot_Death_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Death[i], -1);

		sprintf_s(szTemp, "RedSlot_Pino_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Pino[i], -1);

		sprintf_s(szTemp, "RedSlot_Exp_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Exp[i], -1);

		sprintf_s(szTemp, "RedSlot_Rank_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_Rank[i], -1);

		sprintf_s(szTemp, "RedSlot_Beginer_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_Beginer[i], -1);

		sprintf_s(szTemp, "RedSlot_Mission_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Mission[i], -1);

		sprintf_s(szTemp, "RedSlot_Score_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxTop_Score[i], -1);

		sprintf_s(szTemp, "RedSlot_Challenge_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_Challenge[i], -1);

		sprintf_s(szTemp, "RedSlot_PC_Room_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_PC_Room[i], -1);

		sprintf_s(szTemp, "RedSlot_Item_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_Item[i], -1);

		sprintf_s(szTemp, "RedSlot_Event_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_Event[i], -1);

		sprintf_s(szTemp, "RedSlot_ClanMark_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_ClanMark[i], -1);

		sprintf_s(szTemp, "RedSlot_ESport_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticTop_ESport[i], -1);

	}

	//BLUE TEAM
	for(i=0; i<MAX_TEAM_SLOT_COUNT; i++)
	{	
		sprintf_s(szTemp, "BlueSlot_ID_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_ID[i], -1);

		sprintf_s(szTemp, "BlueSlot_Kill_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Kill[i], -1);

		sprintf_s(szTemp, "BlueSlot_Death_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Death[i], -1);

		sprintf_s(szTemp, "BlueSlot_Pino_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Pino[i], -1);

		sprintf_s(szTemp, "BlueSlot_Exp_%d", i+1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Exp[i], -1);

		sprintf_s(szTemp, "BlueSlot_Rank_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_Rank[i], -1);

		sprintf_s(szTemp, "BlueSlot_Beginer_%d", i + 1);
		LinkControlEx(*plist, 	szTemp, m_pStaticBottom_Beginer[i], -1);

		sprintf_s(szTemp, "BlueSlot_Mission_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Mission[i], -1);

		sprintf_s(szTemp, "BlueSlot_Score_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pEditBoxBottom_Score[i], -1);

		sprintf_s(szTemp, "BlueSlot_Challenge_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_Challenge[i], -1);

		sprintf_s(szTemp, "BlueSlot_PC_Room_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_PC_Room[i], -1);

		sprintf_s(szTemp, "BlueSlot_Item_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_Item[i], -1);

		sprintf_s(szTemp, "BlueSlot_Event_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_Event[i], -1);

		sprintf_s(szTemp, "BlueSlot_ClanMark_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_ClanMark[i], -1);

		sprintf_s(szTemp, "BlueSlot_ESport_%d", i + 1);
		LinkControlEx(*plist,  szTemp, m_pStaticBottom_ESport[i], -1);
	}

	LinkControlEx(*plist, "Result_Confirm",		m_pButtonResultConfirm,							GUI_ID_BATTLE_BUTTON_RESULT_CONFIRM );

	LinkControlEx(*plist, "TeamBarWin", m_pTeamBarTop, -1);
	LinkControlEx(*plist, "TeamBarLose", m_pTeamBarBottom, -1);
	LinkControlEx(*plist, "TeamResultTop", m_pTeamResultTop, -1);
	LinkControlEx(*plist, "TeamResultBottom", m_pTeamResultBottm, -1);
	LinkControlEx(*plist, "Result_SelectBar", m_pSelectBar, -1);
	//Key:EscapeMission 스코어보드 처리 
	LinkControlEx(*plist, "Result_WinLostWorld", m_pStaticTop_LostWorldResult, -1);
	LinkControlEx(*plist, "Result_LoseLostWorld", m_pStaticBottom_LostWorldResult, -1);
	for( i = 0; i < 11; i++)
	{
		char control[MAX_STRING_COUNT];

		sprintf_s(control, "WinBarText%d", i + 1);
		LinkControlEx(*plist, control, m_pWinBarText[i], -1);

		sprintf_s(control, "LoseBarText%d", i + 1);
		LinkControlEx(*plist, control, m_pLoseBarText[i], -1);
	}

	// Shadow 적용
	for( i=0; i<MAX_TEAM_SLOT_COUNT; i++)
	{
		m_pEditBoxTop_ID[i]->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Kill[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Death[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Pino[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Exp[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Mission[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxTop_Score[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);

		m_pEditBoxBottom_ID[i]->ReCreateTextEx( GetDefaultFontName(), FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Kill[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Death[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Pino[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Exp[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Mission[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
		m_pEditBoxBottom_Score[i]->ReCreateTextEx( GetDefaultFontName(), 16, GAME_FONT_DEFAULT_SIZE, 0, 0, FW_NORMAL, TRUE, FONT_SHADOW_QUALITY_LOW);
	}

	return true;
}


void CBattleGUI_Result::Init(void)
{
	INT32 i;

	m_pStaticResultBG->SetEnable( FALSE);

	m_pStaticResult[RESULT_STATIC_RED_SCORE1]->SetAllShapeEnable( FALSE);
	m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetAllShapeEnable( FALSE);
	m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetAllShapeEnable( FALSE);
	m_pStaticResult[RESULT_STATIC_BLUE_SCORE1]->SetAllShapeEnable( FALSE);
	m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetAllShapeEnable( FALSE);
	m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetAllShapeEnable( FALSE);
	
	m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable(0, TRUE);
	m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable(0, TRUE);

	for( i=0; i<MAX_TEAM_SLOT_COUNT; i++)
	{
		// Red Team
		m_pEditBoxTop_ID[i]->SetTextAlign( 0, 1);
		m_pEditBoxTop_Kill[i]->SetTextAlign( 1, 1);
		m_pEditBoxTop_Death[i]->SetTextAlign( 1, 1);
		m_pEditBoxTop_Pino[i]->SetTextAlign( 1, 1);		
		m_pEditBoxTop_Exp[i]->SetTextAlign( 1, 1);
		GameGUI::SetRankStatic(m_pStaticTop_Rank[i]);
		m_pStaticTop_Rank[i]->SetEnable(FALSE);
		m_pStaticTop_Beginer[i]->SetEnable(FALSE);
		m_pEditBoxTop_Mission[i]->SetTextAlign( 1, 1);
		m_pEditBoxTop_Score[i]->SetTextAlign(1, 1);
		m_pStaticTop_Challenge[i]->SetEnable(FALSE);
		m_pStaticTop_PC_Room[i]->SetEnable(FALSE);
		m_pStaticTop_Item[i]->SetEnable(FALSE);
		m_pStaticTop_Event[i]->SetEnable(FALSE);
		m_pStaticTop_ClanMark[i]->SetEnable(FALSE);
		m_pStaticTop_ESport[i]->SetAllShapeEnable(FALSE);
		m_pStaticTop_ESport[i]->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게

		// Blue Team
		m_pEditBoxBottom_ID[i]->SetTextAlign( 0, 1);
		m_pEditBoxBottom_Kill[i]->SetTextAlign( 1, 1);
		m_pEditBoxBottom_Death[i]->SetTextAlign( 1, 1);
		m_pEditBoxBottom_Pino[i]->SetTextAlign( 1, 1);
		m_pEditBoxBottom_Exp[i]->SetTextAlign( 1, 1);
		GameGUI::SetRankStatic(m_pStaticBottom_Rank[i]);
		m_pStaticBottom_Rank[i]->SetEnable(FALSE);
		m_pStaticBottom_Beginer[i]->SetEnable(FALSE);
		m_pEditBoxBottom_Mission[i]->SetTextAlign( 1, 1);
		m_pEditBoxBottom_Score[i]->SetTextAlign( 1, 1);
		m_pStaticBottom_Challenge[i]->SetEnable(FALSE);
		m_pStaticBottom_PC_Room[i]->SetEnable(FALSE);
		m_pStaticBottom_Item[i]->SetEnable(FALSE);
		m_pStaticBottom_Event[i]->SetEnable(FALSE);
		m_pStaticBottom_ClanMark[i]->SetEnable(FALSE);
		m_pStaticBottom_ESport[i]->SetAllShapeEnable(FALSE);
		m_pStaticBottom_ESport[i]->SetColor(I3GUI_CONTROL_STATE_NORMAL, 255, 255, 255, 120);		// 태그를 흐리게
	}


	for(i = 0; i < 11; i++)
	{
		m_pWinBarText[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		m_pLoseBarText[i]->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
	}
	
	m_pWinBarText[0]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_ORDER"));
	m_pWinBarText[1]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_CLAN"));
	m_pWinBarText[2]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_RANK"));
	m_pWinBarText[3]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_NICK"));
	m_pWinBarText[4]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_KILL"));
	m_pWinBarText[5]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_DEATH"));
	m_pWinBarText[6]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_MISSION"));
	m_pWinBarText[7]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_EXP"));
	m_pWinBarText[8]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_POINT"));	
	
	if (g_pGameContext->IsStageMaskTraining())
	{
		m_pWinBarText[9]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_SCORE"));
	}
	else
	{
		m_pWinBarText[9]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_CHALLENGE"));
	}

	m_pWinBarText[10]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_BONUS"));

	m_pLoseBarText[0]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_ORDER"));
	m_pLoseBarText[1]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_CLAN"));
	m_pLoseBarText[2]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_RANK"));
	m_pLoseBarText[3]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_NICK"));
	m_pLoseBarText[4]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_KILL"));
	m_pLoseBarText[5]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_DEATH"));
	m_pLoseBarText[6]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_MISSION"));
	m_pLoseBarText[7]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_EXP"));
	m_pLoseBarText[8]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_POINT"));

	if (g_pGameContext->IsStageMaskTraining())
	{
		m_pLoseBarText[9]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_SCORE"));
	}
	else
	{
		m_pLoseBarText[9]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_CHALLENGE"));
	}
	
	m_pLoseBarText[10]->SetText(GAME_STRING("STR_TBL_BATTLEGUI_RESULT_TEXT_BONUS"));


	m_pButtonResultConfirm->setClickAction( TRUE);
	m_pButtonResultConfirm->setInputMode(I3GUI_INPUT_MODE_STROKE);
	m_pButtonResultConfirm->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	m_pButtonResultConfirm->SetUserDefine(I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);
	m_pButtonResultConfirm->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 2);

	m_pTeamBarTop->SetAllShapeEnable(FALSE);
	m_pTeamBarBottom->SetAllShapeEnable(FALSE);
	m_pTeamResultTop->SetAllShapeEnable(FALSE);
	m_pTeamResultBottm->SetAllShapeEnable(FALSE);

	m_bEndCalcResult = FALSE;
}

INT32 CBattleGUI_Result::getWinnerTeam( void)
{
	if( m_bEndCalcResult)
	{
		return m_resultState;
	}

	return -1;
}
#include "ui/UIUtil.h"
void CBattleGUI_Result::SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	if( bFlag)
	{
		// Respawn
		if( nState == GAME_GUI_RESPAWN)
		{
			m_pStaticResultBG->SetEnable( FALSE);
			SetEnable( FALSE);
		}
		// Result
		else if( nState == GAME_GUI_RESULT)
		{
			g_pFramework->SetGameKeyMode( GAMEKEY_MODE_MENU);
			
			m_pStaticResultBG->SetEnable( TRUE);

			SetEnable( TRUE);
		}
		else if( nState == GAME_GUI_NONE)
		{
		}
	}
	else
	{
		m_pStaticResultBG->SetEnable( FALSE);
		SetEnable( FALSE);
	}
}

BOOL CBattleGUI_Result::OnInputForResult( i3InputKeyboard*pKeyboard)
{
	if (m_pButtonResultConfirm->isEnable())
	{
		if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
		{
			m_pGUI->OnResultConfrim();

			return TRUE;
		}
	}

	return FALSE;
}

#ifdef DEV_BUILD
	static REAL32 screenshotTime = 0.0f;
	static bool enableScreenshot = false;
#endif

void CBattleGUI_Result::ProcessResult( REAL32 rDeltaSeconds )
{
	// 점수를 결산합니다.(한번만 함!)
	if( !m_bEndCalcResult)
	{
		gstatemgr::i()->UpdateScore(false, rDeltaSeconds);

		CalcResult();

		if (RS_RED_WIN == m_resultState)
		{
			g_pSndMng->StartAnnounceMessage( GTA_REDTEAM_WIN);
		}
		else if (RS_BLUE_WIN == m_resultState)
		{
			g_pSndMng->StartAnnounceMessage( GTA_BLUETEAM_WIN);
		}

#ifdef DEV_BUILD
		if( LocaleValue::Enable("_AutoScreenShot") )
		{
			screenshotTime = 0.0f;
			enableScreenshot = true;
		}
#endif
	}

#ifdef DEV_BUILD
	if( LocaleValue::Enable("_AutoScreenShot") )
	{
		if( enableScreenshot && screenshotTime > 0.4f )
		{
			g_pFramework->SetGameKey(1, GAME_KEY_MAP_SCREENSHOT);
			enableScreenshot = false;
		}
		else
		{
			screenshotTime += rDeltaSeconds;
		}
	}
#endif
}

void CBattleGUI_Result::CalcResult()
{
	INT32 i = 0;
	char szTemp[256]= {0,};
	INT32 TopStartIdx = 0;
	INT32 BottomStartIdx = 0;

	m_nMaxExpPerBattle = MAX_EXP_PER_BATTLE;

	SCORE_INFO * pScoreInfo;
	SLOT_INFO * pSlotInfo;

	for(i=0; i < (SLOT_MAX_COUNT >> 1); i++)
	{
		m_TopPoint[ i ].Reset();
		m_BottomPoint[ i ].Reset();
	}

	for(i=0; i < SLOT_MAX_COUNT; i++)
	{	
		BOOL bIsNPC = FALSE;
		
		CGameCharaBase* pChara = g_pCharaManager->getCharaByNetIdx(i);

		if (pChara != NULL)
		{
			if( g_pGameContext->IsStageMaskTraining() )
			{
				if( (g_pGameContext->getMainSlotIdx() % 2) != (i % 2) )
				{
					bIsNPC = TRUE;
				}
			}
		}


		pSlotInfo = g_pGameContext->getSlotInfo(i);
		
		// 승리 팀
		if(m_resultState == i % 2 || (RS_DRAW == m_resultState && 0 == i % 2))
		{	
			if(g_pGameContext->getBattleEndUserFlag() & (0x01 <<i) || bIsNPC)
			{
				pScoreInfo = g_pGameContext->getIndividualScore( i );					

				// Index
				m_TopPoint[TopStartIdx].nIdx = i;
				// ID
				i3String::Copy(m_TopPoint[TopStartIdx].szNick, g_pGameContext->getNickForSlot(i), NET_NICK_NAME_SIZE );
				// NickName Color
				m_TopPoint[TopStartIdx].NickColor = g_pGameContext->getNickColorForSlot(i);
				//	Clan Idx
				m_TopPoint[TopStartIdx].nClanIdx = pSlotInfo->_clanidx;
				//	Clan Mark
				m_TopPoint[TopStartIdx].nClanMark = pSlotInfo->_clanMark;
				// Hero Kill Count
				m_TopPoint[TopStartIdx].nKillCount = pScoreInfo->m_KillCount;
				// Death Count
				m_TopPoint[TopStartIdx].nDeath = pScoreInfo->m_DeathCount;
				// Rank
				m_TopPoint[TopStartIdx].nRank = g_pGameContext->getSlotInfo(i)->_Rank;
				// Point
				m_TopPoint[TopStartIdx].nPoint = * g_pGameContext->GetBattlePoint( i);
				m_TopPoint[TopStartIdx].nBonusnPoint = *g_pGameContext->GetBattleEventPoint(i);
				// Exp
				m_TopPoint[TopStartIdx].nExp = * g_pGameContext->GetBattleExp( i);
				m_TopPoint[TopStartIdx].nBonusnExp = *g_pGameContext->GetBattleEventExp( i);
				m_nMaxExpPerBattle = MAX(m_nMaxExpPerBattle, *g_pGameContext->GetBattleExp( i));
				// Score
				m_TopPoint[TopStartIdx].nScore = * g_pGameContext->GetBattleScore(i);				

				m_TopPoint[TopStartIdx].nResultMark = *g_pGameContext->GetBattleResultIcon(i);

				//	E-Sport
				m_TopPoint[TopStartIdx].nESport = pSlotInfo->_eSportID;
		
				// Mission
				if (g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode()|| g_pGameContext->IsBossMissionMode())
				{
					m_TopPoint[TopStartIdx].nMission = *g_pGameContext->GetAccMissionCount(i);
				}
				else
				{
					m_TopPoint[TopStartIdx].nMission = 0;
				}

				if( g_pGameContext->getBattleChallengeFlag() & (0x01 <<i))
				{
					m_TopPoint[TopStartIdx].bChallenge = TRUE;
				}
				
				m_nWinResultKillCount += m_TopPoint[TopStartIdx].nKillCount;
				
				TopStartIdx++;
			}
		}
		// 패배팀
		else
		{
			if (g_pGameContext->getBattleEndUserFlag() & (0x01 <<i)  || bIsNPC)
			{
				pScoreInfo = g_pGameContext->getIndividualScore( i );

				// Index
				m_BottomPoint[BottomStartIdx].nIdx = i;
				// ID
				i3String::Copy(m_BottomPoint[BottomStartIdx].szNick, g_pGameContext->getNickForSlot(i), NET_NICK_NAME_SIZE );
				// NickName Color
				m_BottomPoint[BottomStartIdx].NickColor = g_pGameContext->getNickColorForSlot(i);
				//	Clan Idx
				m_BottomPoint[BottomStartIdx].nClanIdx = pSlotInfo->_clanidx;
				//	Clan Mark
				m_BottomPoint[BottomStartIdx].nClanMark = pSlotInfo->_clanMark;
				// Hero Kill Count
				m_BottomPoint[BottomStartIdx].nKillCount = pScoreInfo->m_KillCount;
				// Death Count
				m_BottomPoint[BottomStartIdx].nDeath = pScoreInfo->m_DeathCount;
				// Rank
				m_BottomPoint[BottomStartIdx].nRank = g_pGameContext->getSlotInfo(i)->_Rank;
				// Point
				m_BottomPoint[BottomStartIdx].nPoint = *g_pGameContext->GetBattlePoint( i);
				m_BottomPoint[BottomStartIdx].nBonusnPoint = *g_pGameContext->GetBattleEventPoint(i);
				// Exp				
				m_BottomPoint[BottomStartIdx].nExp = *g_pGameContext->GetBattleExp( i);
				m_BottomPoint[BottomStartIdx].nBonusnExp = *g_pGameContext->GetBattleEventExp( i);
				m_nMaxExpPerBattle = MAX(m_nMaxExpPerBattle, *g_pGameContext->GetBattleExp( i));
				// Score
				m_BottomPoint[BottomStartIdx].nScore = *g_pGameContext->GetBattleScore(i);				

				m_BottomPoint[BottomStartIdx].nResultMark = *g_pGameContext->GetBattleResultIcon(i); 

				//	E-Sport
				m_BottomPoint[BottomStartIdx].nESport = pSlotInfo->_eSportID;
		
				// Mission
				if (g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode() || g_pGameContext->IsBossMissionMode())
				{
					m_BottomPoint[BottomStartIdx].nMission = *g_pGameContext->GetAccMissionCount(i);
				}
				else
				{
					m_BottomPoint[BottomStartIdx].nMission = 0;
				}

				if( g_pGameContext->getBattleChallengeFlag() & (0x01 <<i))
				{
					m_BottomPoint[BottomStartIdx].bChallenge = TRUE;
				}
				m_nLoseResultKillCount += m_BottomPoint[BottomStartIdx].nKillCount;
				BottomStartIdx++;
			}
		}
	}

	// 정렬!!
	Sort( m_TopPoint, TopStartIdx);
	Sort( m_BottomPoint, BottomStartIdx);

	
	TopStartIdx = 0;
	BottomStartIdx = 0;

	for(i=0; i<16; i++)
	{
		// Top Team!
		if(i%2 == 0)
		{
			if(i3String::Compare( m_TopPoint[TopStartIdx].szNick, "") != 0)
			{				
				// ID
				m_pEditBoxTop_ID[TopStartIdx]->SetEnable(TRUE);		
				m_pEditBoxTop_ID[TopStartIdx]->SetTextEllipsis(m_TopPoint[TopStartIdx].szNick);
				m_pEditBoxTop_ID[TopStartIdx]->SetTextColor( 
					(COLOR*)&g_ColorCashItem[ m_TopPoint[TopStartIdx].NickColor ] );

				// Hero Kill Count
				sprintf_s(szTemp,"%d", m_TopPoint[TopStartIdx].nKillCount);
				m_pEditBoxTop_Kill[TopStartIdx]->SetEnable(TRUE);
				m_pEditBoxTop_Kill[TopStartIdx]->SetText(szTemp);

				// Death Count
				sprintf_s(szTemp,"%d", m_TopPoint[TopStartIdx].nDeath);
				m_pEditBoxTop_Death[TopStartIdx]->SetEnable(TRUE);
				m_pEditBoxTop_Death[TopStartIdx]->SetText(szTemp);

				// Pino				
				if( m_TopPoint[TopStartIdx].nBonusnPoint > 0) 
				{
					sprintf_s(szTemp, "%d(+%d)", m_TopPoint[TopStartIdx].nPoint, m_TopPoint[TopStartIdx].nBonusnPoint);
				}
				else
				{
					sprintf_s(szTemp, "%d", m_TopPoint[TopStartIdx].nPoint);
				}
				m_pEditBoxTop_Pino[TopStartIdx]->SetEnable(TRUE);
				m_pEditBoxTop_Pino[TopStartIdx]->SetText(szTemp);

				// Exp 화면에 경험치 찍히는 부분입니다.
				if( m_TopPoint[TopStartIdx].nBonusnExp > 0) 
				{
					sprintf_s(szTemp, "%d(+%d)", m_TopPoint[TopStartIdx].nExp, m_TopPoint[TopStartIdx].nBonusnExp);
				}
				else
				{
					sprintf_s(szTemp, "%d", m_TopPoint[TopStartIdx].nExp);
				}
				m_pEditBoxTop_Exp[TopStartIdx]->SetEnable(TRUE);
				m_pEditBoxTop_Exp[TopStartIdx]->SetText(szTemp);

				//초보 채널에 대한 처리 
				if(g_pGameContext->IsBeginnerChannel1())
				{
					m_pStaticTop_Rank[TopStartIdx]->SetEnable(FALSE);
					m_pStaticTop_Beginer[TopStartIdx]->SetEnable(TRUE);
				}
				else 
				{
					m_pStaticTop_Rank[TopStartIdx]->SetEnable(TRUE);
					m_pStaticTop_Rank[TopStartIdx]->SetShapeUVIndex(m_TopPoint[TopStartIdx].nRank);
					m_pStaticTop_Beginer[TopStartIdx]->SetEnable(FALSE);
				}

				// Mission
				sprintf_s(szTemp, "%d", m_TopPoint[TopStartIdx].nMission);
				m_pEditBoxTop_Mission[TopStartIdx]->SetEnable(TRUE);
				m_pEditBoxTop_Mission[TopStartIdx]->SetText(szTemp);

				if (g_pGameContext->IsStageMaskTraining())
				{
					sprintf_s(szTemp, "%d", m_TopPoint[TopStartIdx].nScore);
					m_pEditBoxTop_Score[TopStartIdx]->SetEnable(TRUE);
					m_pEditBoxTop_Score[TopStartIdx]->SetText(szTemp);

					m_pStaticTop_Challenge[TopStartIdx]->SetEnable(FALSE);
				}
				else
				{
					// 임무				
					if( m_TopPoint[TopStartIdx].bChallenge) m_pStaticTop_Challenge[TopStartIdx]->SetEnable( TRUE);
					else m_pStaticTop_Challenge[TopStartIdx]->SetEnable( FALSE);
				}

				// 자기자신의 인덱스라면 테두리를 칩니다.
				if( g_pGameContext->getMySlotIdx() == m_TopPoint[TopStartIdx].nIdx)
				{
					INT32 idxSlot = i / 2;
					m_pSelectBar->setPositionY(114.0f + (idxSlot * 26));

					ROOM_INFO_BASIC pBasic;
					
					g_pGameContext->getMyRoomInfo(&pBasic);

					UINT8 bWin = 0;
					if(m_resultState == RS_DRAW)
						bWin = RS_DRAW;
					else if(g_pGameContext->getMySlotIdx() % 2 == 0)
						bWin = 1;
					CCharaInfoContext::instance()->setMyLastRecord(
						true,
						bWin,
						pBasic._StageMask,
						pBasic._CurrentStageID,
						m_TopPoint[TopStartIdx].nKillCount,
						m_TopPoint[TopStartIdx].nDeath,
						m_TopPoint[TopStartIdx].nExp,
						m_TopPoint[TopStartIdx].nPoint,
						m_TopPoint[TopStartIdx].nBonusnExp,
						m_TopPoint[TopStartIdx].nPoint
						);
				}

				// PC Cafe
				SetResultMark( m_pStaticTop_PC_Room[TopStartIdx], m_TopPoint[TopStartIdx].nResultMark);

				//Item Mark
				if( m_TopPoint[TopStartIdx].nResultMark & RESULT_ICON_ITEM)
					m_pStaticTop_Item[TopStartIdx]->SetEnable(TRUE);
				else 
					m_pStaticTop_Item[TopStartIdx]->SetEnable(FALSE);

				//Event Mark 
				if( m_TopPoint[TopStartIdx].nResultMark & RESULT_ICON_EVENT)
					m_pStaticTop_Event[TopStartIdx]->SetEnable(TRUE);
				else 
					m_pStaticTop_Event[TopStartIdx]->SetEnable(FALSE);

				
				//	유저 E-Sport 태그


				switch( m_TopPoint[TopStartIdx].nESport)
				{
				case ESPORT_ID_LEVEL_BEGINNER1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_BEGINNER2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_BEGINNER3:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUBSTITUTE1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUBSTITUTE2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STARTER1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 5, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STARTER2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 6, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 7, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 8, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR3:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 9, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 10, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 11, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR3:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 12, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR4:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 13, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 14, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 15, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS3:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 16, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_LEGEND1:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 17, TRUE);
					}
					break;

				case ESPORT_ID_LEVEL_LEGEND2:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 18, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_HALLOFFAME:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 19, TRUE);
					}
					break;
				default:
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( FALSE);
					}
					break;
				}

			/*
				switch( m_TopPoint[TopStartIdx].nESport)
				{
				case ESPORT_ID:	// 유저의 E-Sport 태그
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( TRUE);
						m_pStaticTop_ESport[TopStartIdx]->SetShapeEnable(0, TRUE);
					}
					break;
				default:
					{
						m_pStaticTop_ESport[TopStartIdx]->SetEnable( FALSE);
					}
					break;
				}
				*/
			
				//	클랜마크
				if( m_TopPoint[TopStartIdx].nClanMark !=  CLAN_MARK_DEFAULT)
				{
					m_pStaticTop_ClanMark[TopStartIdx]->SetEnable(TRUE);					

#if defined(USE_CLAN_MARK)		
					::SetClanMarkTexEx(m_pStaticTop_ClanMark[TopStartIdx], CLAN_MARK_SMALL, m_TopPoint[TopStartIdx].nClanMark);	
#endif
				}
				else
				{
					m_pStaticTop_ClanMark[TopStartIdx]->SetEnable(FALSE);
				}

				TopStartIdx++;
			}
		}
		// Bottom Team!
		else
		{
			if(i3String::Compare( m_BottomPoint[BottomStartIdx].szNick, "") != 0)
			{	
				// ID
				m_pEditBoxBottom_ID[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_ID[BottomStartIdx]->SetTextEllipsis( m_BottomPoint[BottomStartIdx].szNick);
				m_pEditBoxBottom_ID[BottomStartIdx]->SetTextColor( 
					(COLOR*)&g_ColorCashItem[ m_BottomPoint[BottomStartIdx].NickColor ] );

				// Hero Kill Count
				sprintf_s(szTemp,"%d", m_BottomPoint[BottomStartIdx].nKillCount);
				m_pEditBoxBottom_Kill[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_Kill[BottomStartIdx]->SetText(szTemp);

				// Death Count
				sprintf_s(szTemp,"%d", m_BottomPoint[BottomStartIdx].nDeath);
				m_pEditBoxBottom_Death[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_Death[BottomStartIdx]->SetText(szTemp);

				// Pino
				if( m_BottomPoint[BottomStartIdx].nBonusnPoint > 0) 
				{
					sprintf_s(szTemp, "%d(+%d)", m_BottomPoint[BottomStartIdx].nPoint, m_BottomPoint[BottomStartIdx].nBonusnPoint);
				}
				else
				{
					sprintf_s(szTemp, "%d", m_BottomPoint[BottomStartIdx].nPoint);
				}
				m_pEditBoxBottom_Pino[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_Pino[BottomStartIdx]->SetText(szTemp);

				// Exp
				if( m_BottomPoint[BottomStartIdx].nBonusnExp > 0) 
				{
					sprintf_s(szTemp, "%d(+%d)", m_BottomPoint[BottomStartIdx].nExp, m_BottomPoint[BottomStartIdx].nBonusnExp);
				}
				else
				{
					sprintf_s(szTemp, "%d", m_BottomPoint[BottomStartIdx].nExp);
				}

				m_pEditBoxBottom_Exp[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_Exp[BottomStartIdx]->SetText(szTemp);

				//초보 채널에 대한 처리 
				if(g_pGameContext->IsBeginnerChannel1())
				{
					m_pStaticBottom_Rank[BottomStartIdx]->SetEnable(FALSE);
					m_pStaticBottom_Beginer[BottomStartIdx]->SetEnable(TRUE);
				}
				else 
				{
					m_pStaticBottom_Rank[BottomStartIdx]->SetEnable(TRUE);
					m_pStaticBottom_Rank[BottomStartIdx]->SetShapeUVIndex(m_BottomPoint[BottomStartIdx].nRank);
					m_pStaticBottom_Beginer[BottomStartIdx]->SetEnable(FALSE);
				}

				// Mission
				sprintf_s(szTemp, "%d", m_BottomPoint[BottomStartIdx].nMission);
				m_pEditBoxBottom_Mission[BottomStartIdx]->SetEnable(TRUE);
				m_pEditBoxBottom_Mission[BottomStartIdx]->SetText(szTemp);

				if (g_pGameContext->IsStageMaskTraining() )
				{
					sprintf_s(szTemp, "%d", m_BottomPoint[BottomStartIdx].nScore);
					m_pEditBoxBottom_Score[BottomStartIdx]->SetEnable(TRUE);
					m_pEditBoxBottom_Score[BottomStartIdx]->SetText(szTemp);

					m_pStaticBottom_Challenge[BottomStartIdx]->SetEnable(FALSE);
				}
				else
				{
					// 임무
					if( m_BottomPoint[BottomStartIdx].bChallenge) m_pStaticBottom_Challenge[BottomStartIdx]->SetEnable( TRUE);
					else m_pStaticBottom_Challenge[BottomStartIdx]->SetEnable( FALSE);
				}

				// 자기자신의 인덱스라면 테두리를 칩니다.
				if( g_pGameContext->getMySlotIdx() == m_BottomPoint[BottomStartIdx].nIdx)
				{
					INT32 idxSlot = i / 2;
					m_pSelectBar->setPositionY(418.0f + (idxSlot * 26));

					ROOM_INFO_BASIC pBasic;

					g_pGameContext->getMyRoomInfo(&pBasic);

					UINT8 bWin = 0;
					if(m_resultState == RS_DRAW)
						bWin = RS_DRAW;
					else if(g_pGameContext->getMySlotIdx() % 2 == 0)
						bWin = 1;
					CCharaInfoContext::instance()->setMyLastRecord(
						true,
						bWin,
						pBasic._StageMask,
						pBasic._CurrentStageID,
						m_BottomPoint[BottomStartIdx].nKillCount,
						m_BottomPoint[BottomStartIdx].nDeath,
						m_BottomPoint[BottomStartIdx].nExp,
						m_BottomPoint[BottomStartIdx].nPoint,
						m_BottomPoint[BottomStartIdx].nBonusnExp,
						m_BottomPoint[BottomStartIdx].nPoint
						);
				}
				
				// PC Cafe
				SetResultMark( m_pStaticBottom_PC_Room[BottomStartIdx], m_BottomPoint[BottomStartIdx].nResultMark);

				//Item Mark
				if( m_BottomPoint[BottomStartIdx].nResultMark & RESULT_ICON_ITEM)
					m_pStaticBottom_Item[BottomStartIdx]->SetEnable(TRUE);
				else 
					m_pStaticBottom_Item[BottomStartIdx]->SetEnable(FALSE);

				//Event Mark 
				if( m_BottomPoint[BottomStartIdx].nResultMark & RESULT_ICON_EVENT)
					m_pStaticBottom_Event[BottomStartIdx]->SetEnable(TRUE);
				else 
					m_pStaticBottom_Event[BottomStartIdx]->SetEnable(FALSE);

				//	유저 E-Sport 태그
				switch(  m_BottomPoint[BottomStartIdx].nESport)
				{
				case ESPORT_ID_LEVEL_BEGINNER1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_BEGINNER2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_BEGINNER3:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUBSTITUTE1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 3, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUBSTITUTE2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 4, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STARTER1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 5, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STARTER2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 6, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 7, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 8, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_STAR3:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 9, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 10, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 11, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR3:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 12, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_SUPERSTAR4:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 13, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 14, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 15, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_WORLDCLASS3:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 16, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_LEGEND1:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 17, TRUE);
					}
					break;

				case ESPORT_ID_LEVEL_LEGEND2:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 18, TRUE);
					}
					break;
				case ESPORT_ID_LEVEL_HALLOFFAME:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(GUI_SHAPE_OFFSET + 19, TRUE);
					}
					break;
				default:
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( FALSE);
					}
					break;
				}
				/*
				switch( m_BottomPoint[BottomStartIdx].nESport)
				{
				case ESPORT_ID:	// 유저의 E-Sport 태그
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( TRUE);
						m_pStaticBottom_ESport[BottomStartIdx]->SetShapeEnable(0, TRUE);
					}
					break;
				default:
					{
						m_pStaticBottom_ESport[BottomStartIdx]->SetEnable( FALSE);
					}
					break;
				}
				*/
			
				//	클랜마크
				if( m_BottomPoint[BottomStartIdx].nClanMark != CLAN_MARK_DEFAULT)
				{
					m_pStaticBottom_ClanMark[BottomStartIdx]->SetEnable(TRUE);

#if defined(USE_CLAN_MARK)
					::SetClanMarkTexEx(m_pStaticBottom_ClanMark[BottomStartIdx], CLAN_MARK_SMALL, m_BottomPoint[BottomStartIdx].nClanMark);					
#endif
				}				
				else
				{
					m_pStaticBottom_ClanMark[BottomStartIdx]->SetEnable(FALSE);
				}

				BottomStartIdx++;
			}
		}
	}

	if( g_pGameContext->IsDeathMode() )
	{
		SCORE_INFO * pScoreInfoTop		= g_pGameContext->getTeamScore( 0 );
		SCORE_INFO * pScoreInfoBottom	= g_pGameContext->getTeamScore( 1 );

		switch(m_resultState)
		{
		case RS_BLUE_WIN:
			m_nTopTotalPoint		= pScoreInfoBottom->m_KillCount;
			m_nBottomTotalPoint		= pScoreInfoTop->m_KillCount;
			break;
		default:			
			m_nTopTotalPoint		= pScoreInfoTop->m_KillCount;
			m_nBottomTotalPoint		= pScoreInfoBottom->m_KillCount;
			break;
		}

		
		// 최대 킬수 넘지 않도록 수정 - 훈련전에선 제한없다.
		if ( FALSE == g_pGameContext->IsStageMaskTraining() )
		{			
			UINT8 RoomSubType	= g_pGameContext->GetMyRoomSubType();
			m_nTopTotalPoint = MIN(m_nTopTotalPoint, (INT32)g_TeamKillCountTable[LONIBBLE(RoomSubType)]);
			m_nBottomTotalPoint = MIN(m_nBottomTotalPoint, (INT32)g_TeamKillCountTable[LONIBBLE(RoomSubType)]);
		}
	}
	else
	{
		switch(m_resultState)
		{
		case RS_BLUE_WIN:
			m_nTopTotalPoint	 = g_pGameContext->getBlueWinRound();
			m_nBottomTotalPoint	 = g_pGameContext->getRedWinRound();
			break;
		default:
			m_nTopTotalPoint	= g_pGameContext->getRedWinRound();
			m_nBottomTotalPoint = g_pGameContext->getBlueWinRound();
			break;
		}
	}

	// Top Team 총점수 표시
	{
		char szTemp[256] = {0,};
		i3String::ValueToStr(m_nTopTotalPoint, szTemp, sizeof(szTemp));
		INT32 len = i3String::Length((const char*)szTemp);
		
		char Temp[256] =  {0,};
		i3String::ValueToStr(m_nWinResultKillCount, Temp, sizeof(Temp));
//		INT32 Strlen = i3String::Length((const char*)Temp);
		
		m_pStaticResult[RESULT_STATIC_RED_SCORE1]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_RED_SCORE1]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetAllShapeEnable( FALSE);
		//Key:EscapeMission 스코어보드 처리
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE1]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE2]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE3]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetEnable( FALSE);

		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE1]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE2]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE3]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetAllShapeEnable( FALSE);
	

		switch(len)
		{
		case 1:			
			if( g_pGameContext->IsDeathMode())			
			{
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			}//Key:EscapeMission 스코어보드 처리
			else if (g_pGameContext->IsBossMissionMode())
			{
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				break;
			}
			else if (g_pGameContext->IsCrossCountMode())
			{
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			}
			else
			{
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable( 10, TRUE);
			}
			break;
		case 2:
			if (g_pGameContext->IsBossMissionMode())
			{
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			
			}
			else
			{
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			}
			break;
		case 3:
			if (g_pGameContext->IsBossMissionMode())
			{

				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE4]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_TOPSCORE5]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);
			}
			else
			{
				m_pStaticResult[RESULT_STATIC_RED_SCORE1]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE1]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE2]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_RED_SCORE3]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);
			}
			break;
		default:
			break;
		}


	}

	// Bottom Team 총점수 표시
	{
		char szTemp[256] = {0,};
		i3String::ValueToStr(m_nBottomTotalPoint, szTemp, sizeof(szTemp));
		INT32 len = i3String::Length((const char*)szTemp);

		char Temp[256] =  {0,};
		i3String::ValueToStr(m_nLoseResultKillCount, Temp, sizeof(Temp));
//		INT32 Strlen = i3String::Length((const char*)Temp); 


		m_pStaticResult[RESULT_STATIC_BLUE_SCORE1]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_BLUE_SCORE1]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetAllShapeEnable( FALSE);
		//Key:EscapeMission 스코어보드 처리
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE1]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE2]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE3]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetEnable( FALSE);
		
		
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE1]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE2]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE3]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetAllShapeEnable( FALSE);
		m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetAllShapeEnable( FALSE);
		
		
		switch(len)
		{
		case 1:			
			if( g_pGameContext->IsDeathMode() )
			{
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			
			}
			else if (g_pGameContext->IsBossMissionMode())			
			{	//Key:EscapeMission 스코어보드 처리
			
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);

			}
			else if (g_pGameContext->IsCrossCountMode())
			{
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
			}
			else
			{
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable( 10, TRUE);
			}
			break;
		case 2:
			if (g_pGameContext->IsBossMissionMode())
			{	//Key:EscapeMission 스코어보드 처리
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			}
			else
			{
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
			}
			break;
		case 3:
			if (g_pGameContext->IsBossMissionMode())
			{	//Key:EscapeMission 스코어보드 처리
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE3]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE4]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_LOSTWORLD_BOTTOMSCORE5]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);

			}
			else
			{
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE1]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetEnable( TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE1]->SetShapeEnable( (INT32)szTemp[0]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE2]->SetShapeEnable( (INT32)szTemp[1]-48, TRUE);
				m_pStaticResult[RESULT_STATIC_BLUE_SCORE3]->SetShapeEnable( (INT32)szTemp[2]-48, TRUE);
			}
			break;

		default:
			break;
		}
	}


	// 총 점수 결산
	m_pTeamBarTop->SetAllShapeEnable(FALSE);
	m_pTeamResultTop->SetAllShapeEnable(FALSE);
	m_pTeamBarBottom->SetAllShapeEnable(FALSE);		
	m_pTeamResultBottm->SetAllShapeEnable(FALSE);
	m_pStaticTop_LostWorldResult->SetEnable(FALSE);
	m_pStaticBottom_LostWorldResult->SetEnable(FALSE);

	switch(m_resultState)
	{
	case RS_RED_WIN:
		m_pTeamBarTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);		
		m_pTeamBarBottom->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);

		if( g_pGameContext->IsBossMissionMode() )
		{
			
			//Key:EscapeMission 스코어보드 처리
			m_pStaticTop_LostWorldResult->SetEnable(TRUE);
			m_pStaticBottom_LostWorldResult->SetEnable(TRUE);
			m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
			m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			
		}
		else
		{
			m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
			m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);

		}
		
		break;
	case RS_BLUE_WIN:
		m_pTeamBarTop->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);		
		m_pTeamBarBottom->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);

		if( g_pGameContext->IsBossMissionMode() )
		{
		//Key:EscapeMission 스코어보드 처리
			m_pStaticTop_LostWorldResult->SetEnable(TRUE);
			m_pStaticBottom_LostWorldResult->SetEnable(TRUE);
		
			m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
			m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);

		}
		else
		{
				m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);
				m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);

		}

		break;
	default:

		if( g_pGameContext->IsBossMissionMode() )
		{
			//Key:EscapeMission 스코어보드 처리
			m_pStaticTop_LostWorldResult->SetEnable(TRUE);
			m_pStaticBottom_LostWorldResult->SetEnable(TRUE);

			m_pTeamBarTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);		
			m_pTeamBarBottom->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
			m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);

		}
		else
		{

			m_pTeamBarTop->SetShapeEnable(GUI_SHAPE_OFFSET + 0, TRUE);		
			m_pTeamBarBottom->SetShapeEnable(GUI_SHAPE_OFFSET + 1, TRUE);
			m_pTeamResultTop->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);
			m_pTeamResultBottm->SetShapeEnable(GUI_SHAPE_OFFSET + 2, TRUE);


		}
		break;
	}

	//if(g_pGameContext->getMySlotIdx() == m_TopPoint[TopStartIdx].nIdx)
	//if(g_pGameContext->getMySlotIdx() == m_BottomPoint[TopStartIdx].nIdx)

	m_bEndCalcResult = TRUE;
}

void CBattleGUI_Result::Sort(RESULT_ARRY *pArr, INT32 num)
{
	INT32 i = 0, j = 0;
	RESULT_ARRY temp; 

	for( i=0; i< num-1; i++)
	{
		for( j=0; j< num-i-1; j++)
		{
			if( pArr[j].nKillCount < pArr[j+1].nKillCount)
			{
				i3mem::Copy( &temp, &pArr[j+1], sizeof(RESULT_ARRY));
				i3mem::Copy( &pArr[j+1], &pArr[j], sizeof(RESULT_ARRY));
				i3mem::Copy( &pArr[j], &temp, sizeof(RESULT_ARRY));
			}
			else if( pArr[j].nKillCount == pArr[j+1].nKillCount)
			{
				if( pArr[j].nDeath > pArr[j+1].nDeath)
				{
					i3mem::Copy( &temp, &pArr[j+1], sizeof(RESULT_ARRY));
					i3mem::Copy( &pArr[j+1], &pArr[j], sizeof(RESULT_ARRY));
					i3mem::Copy( &pArr[j], &temp, sizeof(RESULT_ARRY));
				}
			}
		}
	}
}

void CBattleGUI_Result::SetResultMark( i3GuiStatic * pStatic , UINT8 nResultMark)
{
	if( pStatic == NULL) return;

	pStatic->SetEnable( TRUE);
	pStatic->SetAllShapeEnable(FALSE);

	// 일반 PC방
	if( nResultMark & RESULT_ICON_PC)
	{		
		pStatic->SetShapeEnable( GUI_SHAPE_OFFSET + 0, TRUE);	
	}
	// 플러스 or 프리미엄 PC방
	else if( nResultMark & RESULT_ICON_PC_PLUS)
	{		
		pStatic->SetShapeEnable( GUI_SHAPE_OFFSET + 1, TRUE);	
	}
	// 유저 ID 패키지 (태국, +15%)
	else if( nResultMark & RESULT_ICON_PC_ID)
	{		
		pStatic->SetShapeEnable( GUI_SHAPE_OFFSET + 2, TRUE);	
	}
	else
	{
		pStatic->SetEnable( FALSE);
	}
}
