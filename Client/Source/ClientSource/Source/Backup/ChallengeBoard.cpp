#include "pch.h"

#if 지워질_것들

#include "ChallengeBoard.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "TutorialMode.h"
#include "GameMode.h"

I3_CLASS_INSTANCE(CChallengeBoard);//, i3GameObjBase);

CChallengeBoard::CChallengeBoard()
{
	m_pSceneNode = i3Node::new_object();

	m_pTexture = NULL;
	m_pSpriteBoard = NULL;
	m_pSprite = NULL;

	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		m_pText[i] = NULL;
	}

//	m_IngChallegeCount = 0;

//	memset( &m_IngChallegeVale, 0, sizeof( m_IngChallegeVale ) );
//	memset( &m_IngChallengeClass, 0, sizeof( m_IngChallengeClass ) );
}

CChallengeBoard::~CChallengeBoard()
{
	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
        I3_SAFE_RELEASE(m_pText[i]);
	}

	I3_SAFE_RELEASE(m_pSprite);
	I3_SAFE_RELEASE(m_pSpriteBoard);
	I3_SAFE_RELEASE(m_pTexture);
}

BOOL CChallengeBoard::Create(i3Node* pParentNode,BOOL bEnable)
{
	if (FALSE == i3GameObjBase::Create(pParentNode, bEnable))
	{
		return FALSE;
	}

	// 텍스처 생성
	m_pTexture = crb::i()->LoadHudTexture(HUD_TEXTURE_HUD);

	// 보드 설정
	m_pSpriteBoard = i3Sprite2D::new_object();
	m_pSpriteBoard->Create(CBF_MAX, TRUE, FALSE);
	m_pSpriteBoard->SetTexture(m_pTexture);

	m_pSpriteBoard->SetTextureCoord(CBF_BOARD_TOP, 515.0f, 148.0f, 680.0f, 177.0f);
	m_pSpriteBoard->SetTextureCoord(CBF_BOARD_MIDDLE, 515.0f, 179.0f, 680.0f, 206.0f);
	m_pSpriteBoard->SetTextureCoord(CBF_BOARD_BOTTOM, 515.0f, 209.0f, 680.0f, 238.0f);
	m_pSpriteBoard->SetRect(CBF_BOARD_TOP, 4.0f, 188.0f, 166.0f, 29.0f);
	m_pSpriteBoard->SetRect(CBF_BOARD_MIDDLE, 4.0f, 217.0f, 166.0f, 40.0f * (MAX_CHALLENGE_COUNT - 1.0f));
	m_pSpriteBoard->SetRect(CBF_BOARD_BOTTOM, 4.0f, 217.0f + (40.0f * (MAX_CHALLENGE_COUNT - 1.0f)), 166.0f, 29.0f);

	m_pSpriteBoard->SetEnable(bEnable);

	GetNode()->AddChild(m_pSpriteBoard);
	

	// 스프라이트 설정
	m_pSprite = i3Sprite2D::new_object();
	m_pSprite->Create(MAX_CHALLENGE_COUNT, TRUE, FALSE);
	m_pSprite->SetTexture(m_pTexture);
	
	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{	
		_InitTextureCoord(i);
		_InitSpriteRect(i);
		m_pSprite->SetEnable(i, FALSE);
	}

	GetNode()->AddChild(m_pSprite);


	// 텍스트 설정
	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		VEC3D vecPos;
		m_pSprite->GetLeftTop(i, &vecPos);

		m_pText[i] = i3TextNodeDX2::new_object();
		m_pText[i]->Create( GetDefaultFontName(), MaxBoardString, GAME_FONT_SIZE_HUD_CHALLENGE_TEXT, 0, FALSE, FALSE, DEFAULT_QUALITY, TRUE, FONT_SHADOW_QUALITY_NORMAL);
		m_pText[i]->setAutoWrapEnable(TRUE);
		m_pText[i]->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pText[i]->setPos((UINT32)(vecPos.x + 18), (UINT32)(vecPos.y + 2));
		m_pText[i]->SetTextSpace( 0, 2);
		m_pText[i]->setSize((UINT32)(m_pSpriteBoard->GetSize(0)->x - vecPos.x - 18), 100);

		m_pText[i]->SetEnable(bEnable);
		
		GetNode()->AddChild(m_pText[i]);
	}
	
	////////////////////////////////////////////////////////////////////////////////////
	//제목 만들기 게임중간에 변경없음..
	//1 총기에 대한 부분만 검색합니다. 
	/*
	m_IngChallegeCount = 0; 
	char	strTemp[256]; 
	for(UINT32 i = 0; i < 2; i++) //
	{
		INT8 IngClassIdx = g_pGameContext->getMyChallengeIngInfo()->getClassIngIdx( i); 

		if( IngClassIdx != CHALLENGE_FINISH_CLASS)
		{
			if(i == 1)	i3String::Copy(strTemp,GAME_STRING("STR_TBL_INGAME_HUD_CHALLENGE_BOARD_TEXT"));//연속 
			else		strTemp[0] = 0; 

			UINT32 b = (g_pGameContext->getMyChallengeInfo()->getData( i, IngClassIdx) & 0x00FF0000) >> 16;
			UINT32 c = (g_pGameContext->getMyChallengeInfo()->getData( i, IngClassIdx) & 0x0000FF00) >> 8; 
			
			if(b != 0)strcat( strTemp, g_strCHG_Con_Weapon[b]);	
			if(c != 0)strcat( strTemp, g_strCHG_Con_Sp1[c]);		

			//Set 
			m_pText[m_IngChallegeCount + CB_FIRST]->SetText(strTemp);
			m_IngChallengeClass[m_IngChallegeCount]		= (UINT8)i; 
			m_IngChallegeVale[m_IngChallegeCount]		= g_pGameContext->getMyChallengeInfo()->getData( i, IngClassIdx);
			m_IngChallegeCount++;
		}
	}

	if( g_pGameContext->IsBombMissionMode() || g_pGameContext->IsAnnihilationMode())
	{//3번 미션보드 임무 
		INT8 IngClassIdx = g_pGameContext->getMyChallengeIngInfo()->getClassIngIdx( 3);
		if( IngClassIdx != CHALLENGE_FINISH_CLASS)
		{			
			UINT32 a = (g_pGameContext->getMyChallengeInfo()->getData( 3, IngClassIdx) & 0x00FF0000) >> 24;
			UINT32 b = (g_pGameContext->getMyChallengeInfo()->getData( 3, IngClassIdx) & 0x00FF0000) >> 16;
			UINT32 c = (g_pGameContext->getMyChallengeInfo()->getData( 3, IngClassIdx) & 0x0000FF00) >> 8; 


			//if( (b == STAGE_TEAM_UNKNOWN) || (b == g_pGameContext->getMyRoomInfo()->_MapIdx) )
			//{
			//	strTemp[0] = 0; 
			//	if(a != 0)strcat( strTemp, g_strCHG_Con_Mission_Mode[a]);	
			//	if(b != 0)strcat( strTemp, g_pStageNameMission[b]);	
			//	if(c != 0)strcat( strTemp, g_strCHG_Con_Sp3[c]);		

			//	m_pText[m_IngChallegeCount + CB_FIRST]->SetText(strTemp);			
			//	m_IngChallengeClass[m_IngChallegeCount]		= 3; 
			//	m_IngChallegeVale[m_IngChallegeCount]		= g_pGameContext->getMyChallengeInfo()->getData(3, IngClassIdx);
			//	m_IngChallegeCount++;
			//}

		}
	}
	else
	{//2번 데스모드 임무 
		INT8 IngClassIdx = g_pGameContext->getMyChallengeIngInfo()->getClassIngIdx( 2);
		if( IngClassIdx != CHALLENGE_FINISH_CLASS)
		{
			UINT32 b = (g_pGameContext->getMyChallengeInfo()->getData(2, IngClassIdx) & 0x00FF0000) >> 16;
			UINT32 c = (g_pGameContext->getMyChallengeInfo()->getData(2, IngClassIdx) & 0x0000FF00) >> 8; 


			//if( (b == STAGE_TEAM_UNKNOWN) || (b == g_pGameContext->getMyRoomInfo()->_MapIdx) )
			//{
			//	strTemp[0] = 0; 
			//	if(b != 0)strcat( strTemp, g_pStageNameTeam[b]);	
			//	if(c != 0)strcat( strTemp, g_strCHG_Con_Sp2[c]);		

			//	m_pText[m_IngChallegeCount + CB_FIRST]->SetText(strTemp);	
			//	m_IngChallengeClass[m_IngChallegeCount]		= 2; 
			//	m_IngChallegeVale[m_IngChallegeCount]		= g_pGameContext->getMyChallengeInfo()->getData(2, IngClassIdx);
			//	m_IngChallegeCount++;
			//}
		}
	}
	*/

	Change(); 
	return TRUE;
}



void CChallengeBoard::Change(void)
{
	char	strTemp[MAX_STRING_COUNT];
	CQuestManager * pManager = g_pGameContext->getQuestManager();

	INT32 nRealSlot = 0;//빈 퀘스트는 넘어가도록

	VEC3D	vecPos;
	REAL32	x, y;

	m_pSprite->GetLeftTop( 0, &vecPos);
	x = vecPos.x;
	y = vecPos.y;

	if( pManager != NULL && pManager->isQuestActivated())
	{
		for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
		{
			CQuestBase * pQuest = pManager->getActiveQuest( i);

			if(g_pGameContext->IsTutorialMode())
			{
				char szTemp[256];

				switch(i)
				{
				case 0:			strncpy(szTemp, GAME_STRING("STR_TBL_TUTORIAL_MISSION_MOVE"), 256);					break;
				case 1:			strncpy(szTemp, GAME_STRING("STR_TBL_TUTORIAL_MISSION_SHOOT"), 256);				break;
				case 2:			strncpy(szTemp, GAME_STRING("STR_TBL_TUTORIAL_MISSION_SPECIAL"), 256);				break;
				case 3:			strncpy(szTemp, GAME_STRING("STR_TBL_TUTORIAL_MISSION_TIME_ATTACK"), 256);			break;
				}
				
				if( gmode::i() )
				{

					if ( gmode::i()->GetTutorialMode() != NULL)
					{
						CTutorialMode* mode = gmode::i()->GetTutorialMode();
						_snprintf(strTemp, MAX_STRING_COUNT, "%s {col:206,206,206,255}%d/%d{/col} \n", szTemp, 
							 mode->GetMissionCompleteCount(i), mode->GetMissionMaxCount(i));
						m_pText[nRealSlot]->SetText( strTemp);
						_InitTextureCoord(nRealSlot, mode->IsMissionComplete(i));
					}
					else
					{
						I3ASSERT_0;
					}
					
				}
			}
			else
			{
				if( pQuest == NULL)
					continue;

				pQuest->OnQueryHUDDesc( strTemp, sizeof( strTemp));

				m_pText[nRealSlot]->SetText( strTemp);

				switch( pQuest->OnQueryState())
				{
				case QST_NOT_STARTED :		_InitTextureCoord(nRealSlot, CBT_NONE);	break;
				case QST_DOING :			_InitTextureCoord(nRealSlot, CBT_CHALLENGING);	break;
				case QST_COMPLETED :		_InitTextureCoord(nRealSlot, CBT_COMPLATE);	break;
				}
			}
			

			//이부분의 맹점은 Apply가 한번이상은 되어야 제대로 표시가 된다는 점입니다.textattr->_ArrangeText()가 한번이상 호출되어야 제대로 나옴 - raja - 
			if( nRealSlot >= 1)
			{
				m_pSprite->SetRect( nRealSlot, x, y, 15, 16);

				m_pText[nRealSlot]->setPos((UINT32)(x + 18), (UINT32)(y + 2));
			}

			{
				REAL32 height;

				height  = (REAL32)(m_pText[nRealSlot]->getTextAttrDX()->getTextHeight() + 1) * m_pText[nRealSlot]->getTextAttrDX()->getVisibleLineCount();
	/*
				if( height > m_pText[nRealSlot]->getHeight())
				{
					m_pText[nRealSlot]->setHeight( (UINT32)height + 10);
				}
	*/
				y += height + 6;
			}

			// 보드틀이 활성화 되있다면, 블랫도 활성화
			m_pSprite->SetEnable(nRealSlot, TRUE);
			nRealSlot++;
		}
	}

	for( UINT32 i = nRealSlot; i < MAX_CHALLENGE_COUNT; ++i)
	{
		// 보드틀이 활성화 되있다면, 블랫도 활성화
		m_pSprite->SetEnable(i, FALSE);
		m_pText[i]->SetText( "");
	}

	if( nRealSlot <= 0)
	{
		m_pSpriteBoard->SetEnable( FALSE);
	}
	else
	{
		m_pSpriteBoard->SetEnable( TRUE);

		REAL32 Height = y - vecPos.y;

		m_pSpriteBoard->SetRect(CBF_BOARD_MIDDLE, 4.0f, 217.0f, 166.0f, Height);
		m_pSpriteBoard->SetRect(CBF_BOARD_BOTTOM, 4.0f, 217.0f + Height, 166.0f, 9.0f);
	}
}

void CChallengeBoard::OnActivate(void)
{
	m_pSpriteBoard->SetEnable( TRUE);

	for(UINT32 i = 0; i < MAX_CHALLENGE_COUNT; i++)
	{
		m_pText[i]->SetText("");
		m_pText[i]->SetEnable(TRUE);
	}
}

//BOOL CChallengeBoard::IsComplate(UINT32 idxChallenge)
//{
//	/*
//	if (idxChallenge >= m_IngChallegeCount) return FALSE;
//
//	UINT32 d = m_IngChallegeVale[idxChallenge] & 0x000000FF;
//	UINT8  NowCount = g_pGameContext->getMyChallengeIngInfo()->getSlotSaveData( m_IngChallengeClass[idxChallenge]);
//
//	
//	return NowCount >= g_iCHG_Con_Count[d];
//	*/
//	return FALSE; 
//	
//
//}

//INT32 CChallengeBoard::GetChallengeCount(UINT32 idxChallenge)
//{
////	if (idxChallenge >= m_IngChallegeCount) return 0;
////	return g_pGameContext->getMyChallengeIngInfo()->getSlotSaveData( m_IngChallengeClass[idxChallenge]);
//
//	return 0; 
//}

void CChallengeBoard::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	i3GameObjBase::OnUpdate(rDeltaSeconds);
	// Do

	//Change();
}

void CChallengeBoard::_InitTextureCoord(INT32 challengIdx,ChallengeBulletType bulletType)
{
	RECT uvTable[MAX_CHALLENGE_COUNT] = 
	{
		{515, 281, 529, 296},		
		{515, 281, 529, 296},
		{515, 281, 529, 296},
		{515, 281, 529, 296}
	};

	switch(bulletType)
	{
	case CBT_CHALLENGING:
		uvTable[challengIdx].left += 17;
		uvTable[challengIdx].right += 17;
		break;
	case CBT_COMPLATE:
		uvTable[challengIdx].left += 34;
		uvTable[challengIdx].right += 34;
		break;
	default:
		// Do nothing
		break;
	}

	m_pSprite->SetTextureCoord(challengIdx, (REAL32)uvTable[challengIdx].left, (REAL32)uvTable[challengIdx].top, (REAL32)uvTable[challengIdx].right, (REAL32)uvTable[challengIdx].bottom);
}

void CChallengeBoard::_InitSpriteRect(INT32 challengIdx)
{
	// ! 주의 - right, bottom을 width, height로 사용합니다
	RECT rectTable[MAX_CHALLENGE_COUNT] = 
	{
		{  6,  29,  15,  16},
		{  6,  64,  15,  16},
		{  6,  99,  15,  16},
		{  6, 134,  15,  16}
	};

	// 화면 offset 적용
	rectTable[challengIdx].left += 4;
	rectTable[challengIdx].top += 188;

	m_pSprite->SetRect(challengIdx, (REAL32)rectTable[challengIdx].left, (REAL32)rectTable[challengIdx].top, (REAL32)rectTable[challengIdx].right, (REAL32)rectTable[challengIdx].bottom);
}
#endif