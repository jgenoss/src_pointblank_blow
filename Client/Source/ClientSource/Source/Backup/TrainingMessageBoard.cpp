#include "pch.h"

#if 지워질_것들

#include "TrainingMessageBoard.h"
#include "StageBattle.h"

I3_CLASS_INSTANCE(CMessageBoard);//, i3GameObjBase);

CMessageBoard::CMessageBoard()
{
	m_pSceneNode = i3Node::new_object();

	m_state = TMBS_IDLE;
	m_stateTimer = 0.0f;
	m_currentMessage = TM_NONE;
	m_team = TEAM_RED;

	m_messageBoardTexture = NULL;
	m_messageBoardNode = NULL;
	m_messageTextNode = NULL;

	m_eMessageBoardType = MBT_NOT_SET;

	m_TextNodeForTutorial = NULL;
}

CMessageBoard::~CMessageBoard()
{
	I3_SAFE_RELEASE(m_messageBoardTexture);
	I3_SAFE_NODE_RELEASE(m_TextNodeForTutorial);
	I3_SAFE_NODE_RELEASE(m_messageBoardNode);
	I3_SAFE_NODE_RELEASE(m_messageTextNode);
}

BOOL CMessageBoard::Create(MESSAGE_BOARD_TYPE eType, i3Node *pParentNode, BOOL bEnable)
{
	BOOL bResult = i3GameObjBase::Create(pParentNode, bEnable);

	if (!bResult)
	{
		return bResult;
	}

	
	INT32 screenHight = g_pFramework->GetViewer()->GetViewHeight();
	REAL32 boardPositionX = screenHight - 120.0f;


	m_messageBoardTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD2);

	m_messageBoardNode = i3Sprite2D::new_object();
	I3ASSERT(m_messageBoardNode != NULL);
	m_messageBoardNode->SetTexture(m_messageBoardTexture);
	m_messageBoardNode->Create(5, TRUE, FALSE, FALSE);
	
	m_messageBoardNode->SetTextureCoord(0, 2.0f, 348.0f, 22.0f, 454.0f);
	m_messageBoardNode->SetRect(0, 20.0f, boardPositionX, 20.0f, 106.f);

	m_messageBoardNode->SetTextureCoord(1, 25.0f, 348.0f, 45.0f, 454.0f);
	m_messageBoardNode->SetRect(1, 40.0f, boardPositionX, 400.0f, 106.f);

	m_messageBoardNode->SetTextureCoord(2, 48.0f, 348.0f, 68.0f, 454.0f);
	m_messageBoardNode->SetRect(2, 440.0f, boardPositionX, 20.0f, 106.f);

	m_messageBoardNode->SetTextureCoord(3, 473.0f, 263.0f, 593.0f, 369.0f);
	m_messageBoardNode->SetRect(3, 20.0f, boardPositionX, 119.0f, 106.0f);

	m_messageBoardNode->SetTextureCoord(4, 595.0f, 263.0f, 715.0f, 369.0f);
	m_messageBoardNode->SetRect(4, 20.0f, boardPositionX, 119.0f, 106.0f);

	GetNode()->AddChild(m_messageBoardNode);


	
	m_messageTextNode = i3TextNodeDX2::new_object();
	I3ASSERT(m_messageTextNode != NULL);
	m_messageTextNode->Create( GetDefaultFontName(), FONT_COUNT_512, GAME_FONT_DEFAULT_SIZE);
	m_messageTextNode->setAutoWrapEnable(TRUE);
	m_messageTextNode->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
	m_messageTextNode->setPos(145, (UINT32)boardPositionX + 5);
#if defined(LOCALE_CHINA)
	m_messageTextNode->SetTextSpace(0, 1);
	m_messageTextNode->setSize(315, 98);
#else
	m_messageTextNode->SetTextSpace(0, 5);
	m_messageTextNode->setSize(315, 96);
#endif
	m_messageTextNode->SetColor(255, 255, 255, 255);
	m_messageTextNode->SetEnable(FALSE);
	GetNode()->AddChild(m_messageTextNode);


	if(g_pGameContext->IsTutorialMode())
	{
		m_TextNodeForTutorial = i3TextNodeDX2::new_object();
		I3ASSERT(m_TextNodeForTutorial != NULL);
		m_TextNodeForTutorial->Create( GetDefaultFontName(), FONT_COUNT_256, GAME_FONT_DEFAULT_SIZE);
		m_TextNodeForTutorial->setAutoWrapEnable(TRUE);
		m_TextNodeForTutorial->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_TextNodeForTutorial->setPos(15, (UINT32)(screenHight - 190.0f));
		m_TextNodeForTutorial->SetTextSpace(0, 5);
		m_TextNodeForTutorial->setSize(315, 96);
		m_TextNodeForTutorial->SetColor(255, 255, 255, 255);
		m_TextNodeForTutorial->SetEnable(TRUE);
		m_TextNodeForTutorial->SetText(GAME_STRING("STR_TBL_TUTORIAL_DESC_KEY_HUD_ON_OFF"));
		GetNode()->AddChild(m_TextNodeForTutorial);
	}

	InitMessageText();
	SetMessageBoardType(eType);

	return bResult;
}

void CMessageBoard::InitMessageText()
{
	m_szTrainingTableText[TM_20_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT14");		
	m_szTrainingTableText[TM_40_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT15");		
	m_szTrainingTableText[TM_60_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT16");		
	m_szTrainingTableText[TM_80_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT17");		
	m_szTrainingTableText[TM_100_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT18");		
	m_szTrainingTableText[TM_120_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT19");		
	m_szTrainingTableText[TM_140_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT20");		
	m_szTrainingTableText[TM_160_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT21");		
	m_szTrainingTableText[TM_180_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT22");		
	m_szTrainingTableText[TM_200_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT23");
	m_szTrainingTableText[TM_5_DEATH] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT3");		
	m_szTrainingTableText[TM_20_DEATH] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT2");	
	m_szTrainingTableText[TM_5_CHAIN_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT6");	
	m_szTrainingTableText[TM_20_CHAIN_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT7");	
	m_szTrainingTableText[TM_PIERCING_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT8");
	m_szTrainingTableText[TM_MASS_KILL] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT9");
	m_szTrainingTableText[TM_CHAIN_SLUGGER] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT13");
	m_szTrainingTableText[TM_CHAIN_STOPPER] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT1");	
	m_szTrainingTableText[TM_BATTLE_START] = GAME_STRING("STR_TBL_INGAME_HUD_TRAINING_TEXT24");

	for(INT32 i=0; i<MAX_TUTORIAL_MESSAGE; i++)
	{
		char szTemp[MAX_STRING_LENGTH];
		_snprintf(szTemp, MAX_STRING_LENGTH, "STR_TBL_INGAME_HUD_TUTORIAL_TEXT%d", i);
		m_szTutorialTableText[i] = GAME_STRING(szTemp);		
	}
}

void CMessageBoard::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);

	switch(m_state)
	{
	case TMBS_IDLE:
		// Do nothing
		break;
	case TMBS_START:
		{
			m_stateTimer += rDeltaSeconds;

			if (TRAINING_MESSAGE_START_DELAY <= m_stateTimer)
			{
				_ChangeState(TMBS_PRINT);
			}
		}
		break;
	case TMBS_PRINT:
		{
			m_stateTimer += rDeltaSeconds;

			const char* tableText = _GetMessageText();
			if(tableText == NULL)
			{
				return;
			}
			INT32 letterCount = (INT32)(m_stateTimer * TRAINING_MESSAGE_SPEED_PER_SECONDS);

			char outputText[MAX_STRING_COUNT*3];

			_LCopy(outputText, tableText, letterCount);
			m_messageTextNode->SetText(outputText);

			// 원본과 출력문자열이 같으면 출력 완료
			if (0 == i3String::Compare(tableText, outputText))
			{
				_ChangeState(TMBS_WAIT);
			}
		}
		break;
	case TMBS_WAIT:
		{
			m_stateTimer += rDeltaSeconds;

			if(m_eMessageBoardType == MBT_TRAINING)
			{
				if (TRAINING_MESSAGE_END_DELAY <= m_stateTimer)
				{
					_ChangeState(TMBS_IDLE);
				}
			}
			
		}
		break;
	}
}

void CMessageBoard::SetTeam(TEAM_TYPE teamType)
{
	I3ASSERT(m_messageBoardNode != NULL);
	
	m_team = teamType;
}

void CMessageBoard::SetMessage(INT32 message)
{	
	//I3ASSERT(TM_20_KILL <= message && message < MAX_TRAINING_MESSAGE);

	if(m_eMessageBoardType == MBT_TRAINING)
	{
		// 출력되는 메시지가 없다면, 새 메시지 설정
		if (m_state == TMBS_IDLE)
		{
			m_currentMessage = message;
			_ChangeState(TMBS_START);
		}
	}
	else if(m_eMessageBoardType == MBT_TUTORIAL)
	{
		m_currentMessage = message;
		_ChangeState(TMBS_START);
	}
	else
	{
		I3ASSERT_0;
	}
	
}

void CMessageBoard::SetEnableBoardText(BOOL bFlag)
{
	m_messageBoardNode->SetEnable(bFlag);
	m_messageTextNode->SetEnable(bFlag);
}

void CMessageBoard::_ChangeState(TRAINING_MESSAGE_BOARD_STATE state)
{
	switch(state)
	{
	case TMBS_IDLE:		
		GlobalFunc::PB_TRACE("TMBS_IDLE");
		m_messageBoardNode->SetEnable(FALSE);
		m_messageTextNode->SetEnable(FALSE);
		break;
	case TMBS_START:
		{
			//GlobalFunc::PB_TRACE("TMBS_START");

			m_messageBoardNode->SetEnable(0, TRUE);
			m_messageBoardNode->SetEnable(1, TRUE);
			m_messageBoardNode->SetEnable(2, TRUE);

			if (m_team == TEAM_RED)
			{
				m_messageBoardNode->SetEnable(3, TRUE);
				m_messageBoardNode->SetEnable(4, FALSE);
			}
			else
			{
				m_messageBoardNode->SetEnable(3, FALSE);
				m_messageBoardNode->SetEnable(4, TRUE);
			}

			m_stateTimer = 0.0f;
		}
		break;
	case TMBS_PRINT:
		//GlobalFunc::PB_TRACE("TMBS_PRINT");
		m_messageTextNode->SetText("");
		m_messageTextNode->SetEnable(TRUE);
		m_stateTimer = 0.0f;
		break;
	case TMBS_WAIT:
		//GlobalFunc::PB_TRACE("TMBS_WAIT");
		m_stateTimer = 0.0f;
		break;
	default:
		//GlobalFunc::PB_TRACE("default");
		I3ASSERT_0;
		break;
	}

	m_state = state;
}

const char* CMessageBoard::_GetMessageText(void)
{
	if (m_currentMessage == TM_NONE)
	{
		return NULL;
	}
	
	const char* pszString;

	if(m_eMessageBoardType == MBT_TRAINING)
	{
		I3_BOUNDCHK( m_currentMessage, MAX_TRAINING_MESSAGE);
		pszString = m_szTrainingTableText[m_currentMessage];
	}
	else if(m_eMessageBoardType == MBT_TUTORIAL)
	{
		I3_BOUNDCHK( m_currentMessage, MAX_TUTORIAL_MESSAGE);
		pszString = m_szTutorialTableText[m_currentMessage];
	}
	else
	{
		I3ASSERT_0;
		return NULL;
	}

	return pszString;
}

INT32 CMessageBoard::_LLength(const char* string)
{
	I3ASSERT(string != NULL);

	INT32 letterCount = 0;

	while(*string)
	{
		INT32 code = i3String::WhatIsCode((char*)string, 0);
		
		// 한글 첫바이트면 뒷바이트는 뛰어넘는다.
		if (code == 1)
		{
			string ++;
		}

		string++;
		letterCount++;
	}

	return letterCount;
}

INT32 CMessageBoard::_LCopy(char* dest, const char* src, INT32 letterCount)
{
	I3ASSERT(dest != NULL);
	I3ASSERT(src != NULL);
	I3ASSERT(0 <= letterCount);

	INT32 length = 0;

	for(INT32 i = 0; i < letterCount && *src; i++)
	{		
		INT32 code = i3String::WhatIsCode((char*)src, 0);

		// 한글 첫바이트면 한바이트 더 복사한다.
		if (code == 1)
		{
			*dest++ = *src++;
			length++;
		}

		*dest++ = *src++;
		length++;		
	}

	*dest = '\0';

	return length;
}

#endif