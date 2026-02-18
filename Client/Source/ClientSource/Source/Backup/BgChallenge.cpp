#include "pch.h"

#if 지워질_것들

#include "BgChallenge.h"
#include "GlobalVariables.h"
#include "QuestManager.h"


I3_CLASS_INSTANCE(CBgChallenge);//, CGuiNotifyReceiver);

CBgChallenge::CBgChallenge()
{
	m_pChallengeWinMin = NULL;
	m_pChallengeCaptionMin = NULL;

	m_pChallengeWin = NULL;
	m_pChallengeCaption = NULL;
	
	for(INT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		m_ChallengeGroup[i]._pMark = NULL;
		m_ChallengeGroup[i]._pText = NULL;
	}

	m_isMinimize = FALSE;
}

CBgChallenge::~CBgChallenge()
{
	// do nothing
}

void CBgChallenge::OnUpdate(RT_REAL32 rDeltaSeconds)
{
#if defined ENABLE_MISSION_CARD
	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		m_ChallengeGroup[i]._pMark->SetAllShapeEnable(FALSE);		
		m_ChallengeGroup[i]._pText->SetText("");
	}

	//
	// 죄송합니다. 리사이징 하드코딩합니다.. ㅠㅠ (영권)
	//

	INT32 nRealSlot = 0;//빈 퀘스트는 넘어가도록

	CQuestManager * pManager = g_pGameContext->getQuestManager(true);
	if (pManager != NULL && pManager->isQuestActivated())
	{
		i3Sprite2D * pSprite = m_ChallengeGroup[0]._pMark->getSprite();
		I3ASSERT( pSprite != NULL);
		VEC3D	vPos;
		pSprite->GetLeftTop(0, &vPos);

		REAL32	NextLineStartY = getY( &vPos) - 8.0f;
		char	strTemp[MAX_STRING_COUNT];

		// 출력할 임무 설명 텍스트를 모두 저장합니다.
		vector<string>	texts;
		for ( INT32 i = 0 ; i < MAX_CHALLENGE_COUNT ; ++i )
		{
			CQuestBase * pQuest = pManager->getActiveQuest( i);
			if ( pQuest == NULL )
				continue;
			pQuest->OnQueryHUDDesc( strTemp, sizeof( strTemp));
			texts.push_back(strTemp);
		}

		// 라인 브레이크가 있으면 스트링을 분할합니다.
		// 각 줄마다 문자열의 가로길이를 구하기 위함입니다.
		vector<string>	newtexts;
		for ( vector<string>::iterator iter = texts.begin() ; iter != texts.end() ;  )
		{
			size_t idx = iter->find('\n');
			if ( idx == string::npos )
			{
				++iter;
				continue;
			}

			string	newstr = iter->substr(idx+1);
			iter->erase(idx);

			newtexts.push_back(newstr);
		}

		// 텍스트를 한 컨테이너로 합칩니다.
		texts.insert(texts.end(), newtexts.begin(), newtexts.end());

		//  컬러 키워드를 모두 삭제합니다.
		for ( vector<string>::iterator iter = texts.begin() ; iter != texts.end() ; ++iter )
		{
			GameGUI::RemoveColorKeyword(*iter);
		}


		// 모든 문자열중 최대 가로길이를 구합니다.
		i3TextNodeDX2 * pTextNode2 = m_ChallengeGroup[0]._pText->getTextNodeDX();
		I3ASSERT( pTextNode2 != NULL);
		i3TextAttrDX2 * pTexAttr2 = pTextNode2->getTextAttrDX();
		I3ASSERT( pTexAttr2 != NULL);

		INT32 MaxTextWidth = 0;
		for ( vector<string>::iterator iter = texts.begin() ; iter != texts.end() ; ++iter )
		{
			const char * str = iter->c_str();
			INT32 TextWidth = pTexAttr2->CalcTextWidth(str);
			MaxTextWidth = max(MaxTextWidth, TextWidth);
		}
		// 350으로 제한을 둔 이유는 350이 넘으면 우측창을 덮기때문입니다.
		MaxTextWidth = min(MaxTextWidth, 320);

		for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
		{
			CQuestBase * pQuest = pManager->getActiveQuest( i);

			if( pQuest == NULL)
			{
				continue;
			}

			pQuest->OnQueryHUDDesc( strTemp, sizeof( strTemp));

			ChallengeControlGroup * pCGroup = &m_ChallengeGroup[nRealSlot];
			i3TextNodeDX2 * pTextNode2	= pCGroup->_pText->getTextNodeDX();	I3ASSERT( pTextNode2 != NULL);
			i3TextAttrDX2 * pTexAttr2	= pTextNode2->getTextAttrDX();		I3ASSERT( pTexAttr2 != NULL);
			pTexAttr2->SetWidth(MaxTextWidth);

			pCGroup->_pText->SetText( strTemp);
			pCGroup->_pMark->SetAllShapeEnable(FALSE);

			switch( pQuest->OnQueryState())
			{
			case QST_NOT_STARTED:	pCGroup->_pMark->SetShapeEnable(0, TRUE);	break;
			case QST_DOING:			pCGroup->_pMark->SetShapeEnable(1, TRUE);	break;
			case QST_COMPLETED:		pCGroup->_pMark->SetShapeEnable(2, TRUE);	break;
			}


			// 위치를 다시 잡는 이유는 임무내용 오른쪽에 화살표 컨트롤과 Y위치를 맞추기위함입니다.
			// 안맞추면 어긋나게됩니다.
			pTextNode2->setPos( pTextNode2->getPosX(), (UINT32) NextLineStartY);

			{
				pSprite = pCGroup->_pMark->getSprite();
				pSprite->SetRect( 0, getX( &vPos), NextLineStartY, 15, 14);
				pSprite->SetRect( 1, getX( &vPos), NextLineStartY, 15, 14);
				pSprite->SetRect( 2, getX( &vPos), NextLineStartY, 15, 14);
			}

			NextLineStartY += ((pTexAttr2->getTextHeight() + 1) * pTexAttr2->getVisibleLineCount());
			NextLineStartY += 6;


			nRealSlot++;
		}


		const REAL32 NewWidth = REAL32(MaxTextWidth + 70) * i3GuiRoot::getGuiResolutionWidth();
		GameGUI::ResizingForm	Form;
		Form.Set(0,0,0);
		Form.Set(0,1,1);
		Form.Set(0,2,2);
		Form.Set(1,0,3);
		Form.Set(1,1,8);
		Form.Set(1,2,4);
		Form.Set(2,0,5);
		Form.Set(2,1,6);
		Form.Set(2,2,7);
		//	0	1	2
		//	3	8	4
		//	5	6	7
		GameGUI::ResizingControl(m_pChallengeWin, &Form, NewWidth, NextLineStartY);

		Form.Reset();
		Form.Set(0,0,0);
		Form.Set(0,1,1);
		Form.Set(0,2,2);
		Form.Set(2,0,5);
		Form.Set(2,1,6);
		Form.Set(2,2,7);

		GameGUI::ResizingControl(m_pChallengeWinMin, &Form, NewWidth, 0);


		{
			//
			// 캡션 크기를 재조정합니다.
			//

			// 캡션 shape은 1개입니다.

			INT32 CaptionWidth = MaxTextWidth + 70;//m_pChallengeWinMin->getWidth();
			{
				INT32 Height = m_pChallengeCaption->getHeight();
				m_pChallengeCaption->setSize(CaptionWidth, Height);
				m_pChallengeCaption->SetTextRectSize(CaptionWidth, Height);
			}
			{
				INT32 Height = m_pChallengeCaptionMin->getHeight();
				m_pChallengeCaptionMin->setSize(CaptionWidth, Height);
				m_pChallengeCaptionMin->SetTextRectSize(CaptionWidth, Height);
			}

		}
	}

	
	if (nRealSlot == 0)
	{
		m_pChallengeWin->SetEnable(FALSE);
		m_pChallengeWinMin->SetEnable(FALSE);
	}
	else
	{
		if (m_isMinimize)
		{
			m_pChallengeWinMin->SetEnable(TRUE);
			m_pChallengeWin->SetEnable(FALSE);
		}
		else
		{
			m_pChallengeWinMin->SetEnable(FALSE);
			m_pChallengeWin->SetEnable(TRUE);
		}
	}

#endif

	CGuiNotifyReceiver::OnUpdate(rDeltaSeconds);
}

BOOL CBgChallenge::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	return FALSE;
}

void CBgChallenge::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
	case GCI_BC_E_CAPTION:
		if (pNotify->m_nEvent == I3GUI_EDIT_NOTIFY_CLICKED) ToggleMinimize();
		break;
	}
}

BOOL CBgChallenge::OnLinkControl( const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl)
{	
	LinkControlEx(*plist, "bcChallengeWinMin", m_pChallengeWinMin, -1);
	LinkControlEx(*plist, "bcChallengeCaptionMin", m_pChallengeCaptionMin, GCI_BC_E_CAPTION);
	m_pChallengeCaptionMin->SetText(GAME_STRING("STR_TBL_GUI_READY_CHALLENGE_INFO1"));
	m_pChallengeCaptionMin->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	LinkControlEx(*plist, "bcChallengeWin", m_pChallengeWin, -1);
	LinkControlEx(*plist, "bcChallengeCaption", m_pChallengeCaption, GCI_BC_E_CAPTION);
	m_pChallengeCaption->SetText(GAME_STRING("STR_TBL_GUI_READY_CHALLENGE_INFO2"));
	m_pChallengeCaption->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

	for(INT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		char controlName[MAX_STRING_COUNT];

		sprintf_s(controlName, "bcChallengeMark%d", i + 1);
		LinkControlEx(*plist, controlName, m_ChallengeGroup[i]._pMark, -1);
		
		sprintf_s(controlName, "bcChallengeText%d", i + 1);
		LinkControlEx(*plist, controlName, m_ChallengeGroup[i]._pText, -1);		
	}

	return FALSE;
}

void CBgChallenge::OnInitControl(void)
{
	m_pChallengeWinMin->SetEnable(FALSE);
	m_pChallengeWin->SetEnable(FALSE);

	INIT_WIN_CAPTION(m_pChallengeCaptionMin, GAME_STRING("STR_TBL_GUI_READY_CHALLENGE_INFO1")); // ▼ 임무 알림 ▼
	INIT_WIN_CAPTION(m_pChallengeCaption, GAME_STRING("STR_TBL_GUI_READY_CHALLENGE_INFO2")); // ▲ 임무 알림 ▲


	for(INT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		m_ChallengeGroup[i]._pMark->SetAllShapeEnable(FALSE);
		m_ChallengeGroup[i]._pMark->SetShapeEnable(0, TRUE);

		m_ChallengeGroup[i]._pText->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);			
		m_ChallengeGroup[i]._pText->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));
		m_ChallengeGroup[i]._pText->SetTextSpace( 0, 3);	
	}

	CGuiNotifyReceiver::OnInitControl();
}

void CBgChallenge::OnOpen(void)
{
#if defined ENABLE_MISSION_CARD
	if (m_isMinimize)
	{
		m_pChallengeWinMin->SetEnable(TRUE);
		m_pChallengeWin->SetEnable(FALSE);
	}
	else
	{
		m_pChallengeWinMin->SetEnable(FALSE);
		m_pChallengeWin->SetEnable(TRUE);		
	}

	SetEnable(TRUE);
#endif
}

void CBgChallenge::OnClose(void)
{
#if defined ENABLE_MISSION_CARD
	m_pChallengeWin->SetEnable(FALSE);
	m_pChallengeWinMin->SetEnable(FALSE);
	SetEnable(FALSE);
#endif
}


void CBgChallenge::ToggleMinimize(void)
{
	m_isMinimize = !m_isMinimize;

	if (m_isMinimize)
	{
		m_pChallengeWinMin->SetEnable(TRUE);
		m_pChallengeWin->SetEnable(FALSE);
	}
	else
	{
		m_pChallengeWinMin->SetEnable(FALSE);
		m_pChallengeWin->SetEnable(TRUE);
	}
}
#endif