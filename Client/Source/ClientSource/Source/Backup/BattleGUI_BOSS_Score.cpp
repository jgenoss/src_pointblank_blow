#include "pch.h"
#include "BattleGUI_BOSS_Score.h"
#include "BattleGui.h"
#include "BattleHUD_Define.h"
#include "BattleHud.h"
#include "ConfigRenderBattle.h"

#include "GuiNotifyReceiver.h"
#include "i3Base/string/ext/snprintf.h"

I3_CLASS_INSTANCE( CBattleGUI_BOSS_Score); //, i3GameObjBase);

typedef struct _tagEditBoxInitInfo
{
	char			m_szName[128];
	INT32			m_idxStrInit;
	INT32			m_MaxLength;
	INT32			m_FontPt;
	INT32			m_AlignHorz;
	INT32			m_AlignVert;
} GUI_EDITBOX_INIT;

static GUI_EDITBOX_INIT		s_STATIC_TEXT_INIT[] =
{
	//   Control Name                StringTable Index                 MaxLength  FontPoint    Align(H)      Align(V)
	{	"Team_Boss/Header_Type",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Boss/Header_Clan",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Boss/Header_Class",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Boss/Header_Nick",		-1,		6,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Boss/Header_KillCount",	-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Boss/Header_Point",		-1,		9,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },

	{	"Team_Union/Header_Rank",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Union/Header_Clan",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Union/Header_Class",		-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Union/Header_Nick",		-1,		6,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Union/Header_KillCount",	-1,		4,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Team_Union/Header_Point",		-1,		9,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },

	{	"",						-1,										0,		0,		0,						0}
};

CBattleGUI_BOSS_Score::CBattleGUI_BOSS_Score(void)
{

	s_STATIC_TEXT_INIT[0].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_TYPE");
	s_STATIC_TEXT_INIT[1].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_CLAN");
	s_STATIC_TEXT_INIT[2].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_RANK");
	s_STATIC_TEXT_INIT[3].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_NICK");
	s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_KILL");
	s_STATIC_TEXT_INIT[5].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_POINT");

	s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_ORDER");
	s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_CLAN");
	s_STATIC_TEXT_INIT[8].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_RANK");
	s_STATIC_TEXT_INIT[9].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_NICK");
	s_STATIC_TEXT_INIT[10].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_KILL");
	s_STATIC_TEXT_INIT[11].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_SCORE_TEXT_POINT");
	
	m_pGUI = NULL;
	m_bVisible = false;
	m_bFirstRound = true;

	m_pTex = NULL;

	i3mem::FillZero( m_pED_Boss, sizeof(m_pED_Boss));
	i3mem::FillZero( m_pED_Union, sizeof(m_pED_Union));
	i3mem::FillZero( m_pST_Ping, sizeof(m_pST_Ping));
	i3mem::FillZero( m_pST_RankMark, sizeof(m_pST_RankMark));

	i3mem::FillZero( m_pST_Clan, sizeof(m_pST_Clan));
	i3mem::FillZero( m_pTip, sizeof(m_pTip));
}

CBattleGUI_BOSS_Score::~CBattleGUI_BOSS_Score(void)
{
	// m_pED_Boss는 해제할 필요없음.
	// m_pED_Union은 해제할 필요없음.
	// m_pED_Quest는 해제할 필요없음.

	I3_SAFE_RELEASE( m_pTex);

	// m_pST_Ping는 해제할 필요없음.
	// m_pST_RankMark는 해제할 필요없음.
}

i3GuiEditBox * CBattleGUI_BOSS_Score::_InitEditBox( const char * pszPath, INT32 maxLength, INT32 pt, INT32 alignX, INT32 alignY, const char * pszText)
{
	i3GuiEditBox * pCtrl;

	pCtrl = (i3GuiEditBox *) m_pPanel->FindControlByPath( pszPath);
	I3ASSERT( pCtrl != NULL);

	pCtrl->ReCreateTextEx( GetDefaultFontName(), maxLength, pt);
	pCtrl->SetTextAlign( alignX, alignY);

	if( pszText != NULL)
		pCtrl->SetText( pszText);

	return pCtrl;
}

void CBattleGUI_BOSS_Score::Create( CBattleGui * pGUI)
{
	i3GuiRoot * pGUIRoot;
	char conv[64];
	INT32 i;

	m_pGUI = pGUI;
	pGUIRoot = m_pGUI->getGUIRoot();

	pGUIRoot->ADDControlByGUI( "Gui/Project/BOSS_Score.gui");

	m_pPanel = (i3GuiStatic *) pGUIRoot->GetControlByName( "BOSS_Score");
	I3ASSERT( m_pPanel);
	
	m_pPanel->SetEnable( FALSE);
	m_pPanel->SetPosToCenter();

	// Static Text 초기화
	{
		for( i = 0; s_STATIC_TEXT_INIT[i].m_szName[0] != 0; i++)
		{
			const char * pszText = NULL;
			const GUI_EDITBOX_INIT * pInfo = &s_STATIC_TEXT_INIT[i];

			if( pInfo->m_idxStrInit != -1)
				pszText = GAME_STRING_BY_HANDLE(pInfo->m_idxStrInit);

			_InitEditBox( (char *) pInfo->m_szName, pInfo->m_MaxLength, pInfo->m_FontPt, 
							s_STATIC_TEXT_INIT[i].m_AlignHorz, s_STATIC_TEXT_INIT[i].m_AlignVert, pszText);
		}
	}

	// Rank Text Init
	{
		// Boss Team
		for( i = 0; i < DINO_TOTAL_CNT; i++)
		{
			// Rank (구분)
			sprintf_s( conv, "Team_Boss/Slot%d/Type", i);
			m_pED_Boss[i][RANK_EDBOX_RANK] = _InitEditBox( conv, 32, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			// 닉네임
			sprintf_s( conv, "Team_Boss/Slot%d/Nick", i);
			m_pED_Boss[i][RANK_EDBOX_NICK] = _InitEditBox( conv, 32, 9, TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			// Kill
			sprintf_s( conv, "Team_Boss/Slot%d/KillCount", i);
			m_pED_Boss[i][RANK_EDBOX_KILLCOUNT] = _InitEditBox( conv, 4, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			// Point
			sprintf_s( conv, "Team_Boss/Slot%d/Point", i);
			m_pED_Boss[i][RANK_EDBOX_POINT] = _InitEditBox( conv, 8, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		}

		// Union Team
		for( i = 0; i < UNION_TOTAL_CNT; i++)
		{
			// Rank (구분)
			sprintf_s( conv, "Team_Union/Slot%d/Rank", i);
			m_pED_Union[i][RANK_EDBOX_RANK] = _InitEditBox( conv, 32, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			// 닉네임
			sprintf_s( conv, "Team_Union/Slot%d/Nick", i);
			m_pED_Union[i][RANK_EDBOX_NICK] = _InitEditBox( conv, 32, 9, TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);

			// Kill
			sprintf_s( conv, "Team_Union/Slot%d/KillCount", i);
			m_pED_Union[i][RANK_EDBOX_KILLCOUNT] = _InitEditBox( conv, 4, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

			// Point
			sprintf_s( conv, "Team_Union/Slot%d/Point", i);
			m_pED_Union[i][RANK_EDBOX_POINT] = _InitEditBox( conv, 8, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		}
	}

	// Ping 관련
	{
		m_pTex = crb::i()->LoadHudTexture( HUD_TEXTURE_SCORE);

		_InitPing( m_pTex);
	}

	// 계급
	{	
		INT32 idxBase;


		// BOSS Team
		for( i = 0; i < DINO_TOTAL_CNT; i++)
		{
			// 계급
			sprintf_s( conv, "Team_Boss/Slot%d/Class", i);

			m_pST_RankMark[i] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
			I3ASSERT( m_pST_RankMark[i] != NULL);

			// 클랜
			sprintf_s( conv, "Team_Boss/Slot%d/Clan", i);
			m_pST_Clan[i] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
			I3ASSERT( m_pST_Clan[i] != NULL);
		}

		// Union Team
		{
			idxBase = DINO_TOTAL_CNT;

			for( i = 0; i < UNION_TOTAL_CNT; i++)
			{
				sprintf_s( conv, "Team_Union/Slot%d/Class", i);
				m_pST_RankMark[i + idxBase] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
				I3ASSERT( m_pST_RankMark[i + idxBase] != NULL);

				sprintf_s( conv, "Team_Union/Slot%d/Clan", i);
				m_pST_Clan[i + idxBase] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
				I3ASSERT( m_pST_Clan[i + idxBase] != NULL);
			}
		}
	}

	// Tip
	{
		m_pED_Tip = _InitEditBox( "Tip/Text", 256, 9, TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);
		m_pED_Tip->getTextNodeDX()->SetTextSpace( 0, 2);

		for( i = 0; i < BOSS_TIP_CNT; i++)
		{
			char szTemp[256];
			_snprintf(szTemp, 256, "STR_TBL_INGAME_HUD_BOSS_TIP%d", i);
			m_pTip[i] = GAME_STRING(szTemp);
		}

		_SetTipText();
	}

	// Server Info
	{
		char strMode[MAX_STRING_COUNT] = "";
		char szServer[64] = "";

		ROOM_INFO_BASIC	roomInfo;
		g_pGameContext->getMyRoomInfo(&roomInfo);

		m_pED_ServerInfo = _InitEditBox( "ServerInfo", 64, 9, TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);

		// _땡빵코드 성백 2008 0507
		if( roomInfo._StageID !=0 )
			i3::safe_string_copy( strMode, getStageTypeName( (STAGE_ID) roomInfo._StageID).c_str(), MAX_STRING_COUNT );
		else
			i3::safe_string_copy( strMode, "Stage", MAX_STRING_COUNT );

		INT32 idxServer = g_pGameContext->GetServerNumber();

		i3::snprintf( szServer, 64, "%03d", idxServer);

		// 수락 여부 확인		
		i3::snprintf(conv, 64, GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_WHERE_AM_I"), strMode, szServer, roomInfo._Idx + 1);

	//	const char * str = GAME_STRING("STR_TBL_INGAME_HUD_NOTICE_WHERE_AM_I");		/*%s %d번서버의 %d번방*/
	//	std::string fmt = BOOST_FORMAT3( str, strMode, szServer, roomInfo._Idx + 1);
	//	i3::safe_string_copy( conv, fmt.c_str(), 64 );

		m_pED_ServerInfo->SetText( conv);
	}
}

void CBattleGUI_BOSS_Score::_InitText( i3GuiControl * pCtrl)
{
	
}

void CBattleGUI_BOSS_Score::_UpdateScore(void)
{
	INT32		dinoList[DINO_TOTAL_CNT];
	INT32		dinoCount = 0;
	INT32		humanList[UNION_TOTAL_CNT];
	INT32		humanCount = 0;
	INT32 i,	idx;
	CGameCharaBase * pChara = NULL;
	SCORE_INFO * pScoreInfo = NULL;
	SLOT_INFO * pSlotInfo = NULL;
	char	conv[128];
	bool	bTRex = false;

	for( i = 0; i < DINO_TOTAL_CNT; i++)		dinoList[i] = -1;
	for( i = 0; i < UNION_TOTAL_CNT; i++)		humanList[i] = -1;

	// 전체 Character에 대한 정보를 읽어, BOSS Team과 Union Team을 구분한다.
	for( i = 0; i < SLOT_MAX_COUNT; i++)
	{
		pChara = g_pCharaManager->getCharaByNetIdx( i);
		if( pChara == NULL) continue;

		if( pChara->IsDino())
		{

			if( pChara->IsRaptorBase())
			{
				I3_BOUNDCHK( dinoCount, sizeof(dinoList) - 1);
				dinoList[ 1+dinoCount] = i;						// T-Rex는 항상 0번 Slot을 사용한다.
				dinoCount++;
			}
			
			else if( pChara->IsRex())
			{
				dinoList[ 0] = i;
				bTRex = true;
			}
			
			else {
				I3ASSERT_0;
			}
		}
		else
		{
			I3_BOUNDCHK( humanCount, sizeof(humanList));
			humanList[ humanCount] = i;
			humanCount++;
		}
		
	}

	if( bTRex)		dinoCount++;				// T-Rex분 만큼 증가.
	else			dinoList[0] = -1;


	_HidePingAll();
	_HideRankMarkAll(); 
	_HideClanMark();

	// BOSS Team
	{
		// Dinos
		for( i = 0; i < DINO_TOTAL_CNT; i++)
		{
			idx = dinoList[i];

			if( idx != -1)
			{
				pChara = g_pCharaManager->getCharaByNetIdx( idx);
				pSlotInfo = g_pGameContext->getSlotInfo( idx);
				pScoreInfo = g_pGameContext->getIndividualScore( idx);

				// 구분 
				if( pChara->IsRex())
					m_pED_Boss[i][RANK_EDBOX_RANK]->SetText( (char *) GAME_STRING("STR_TBL_INGAME_HUD_DINO_TYPE_TREX"));
				else
					m_pED_Boss[i][RANK_EDBOX_RANK]->SetText( (char *) GAME_STRING("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR"));

				// Nick
				m_pED_Boss[i][RANK_EDBOX_NICK]->SetText( (char *) g_pGameContext->getNickForSlot( idx));
				

				// Kill Count
				sprintf_s( conv, "%d", pScoreInfo->m_KillCount);
				m_pED_Boss[i][RANK_EDBOX_KILLCOUNT]->SetText( conv);
		// 여기 다시 확인해봐야 합니다 KEY : 김공룡
		//		sprintf_s( conv, "%d", g_pGameContext->getMissionManager()->getDestructionInfo()->_nUserDamage[idx]);

				m_pED_Boss[i][RANK_EDBOX_POINT]->SetText( conv);

				_ShowPing( i, idx);
				_ShowRankMark( i, pSlotInfo->_Rank);
				_ShowClanMark( i, pSlotInfo);
			}
			else
			{
				m_pED_Boss[i][RANK_EDBOX_RANK]->SetText( "");
				m_pED_Boss[i][RANK_EDBOX_NICK]->SetText( "");
				m_pED_Boss[i][RANK_EDBOX_KILLCOUNT]->SetText( "");
				m_pED_Boss[i][RANK_EDBOX_POINT]->SetText( "");
			}
		}
	}

	// Union Team
	{
		// Humans
		for( i = 0; i < UNION_TOTAL_CNT; i++)
		{
			idx = humanList[i];

			if( idx != -1)
			{
				pSlotInfo = g_pGameContext->getSlotInfo( idx);
				pScoreInfo = g_pGameContext->getIndividualScore( idx);

				// Ranking
				sprintf_s( conv, "%d", i + 1);
				m_pED_Union[i][RANK_EDBOX_RANK]->SetText( (char *) conv);

				// Nick
				m_pED_Union[i][RANK_EDBOX_NICK]->SetText( (char *) g_pGameContext->getNickForSlot( idx));

				// Kill Count
				sprintf_s( conv, "%d", pScoreInfo->m_KillCount);
				m_pED_Union[i][RANK_EDBOX_KILLCOUNT]->SetText( conv);


//				sprintf_s( conv, "%d", (INT32)((REAL32)(g_pGameContext->getMissionManager()->getDestructionInfo()->_nUserDamage[idx])));
//				m_pED_Union[i][RANK_EDBOX_POINT]->SetText( conv);

				_ShowPing( i + DINO_TOTAL_CNT, idx);
				_ShowRankMark( i + DINO_TOTAL_CNT, pSlotInfo->_Rank);
				_ShowClanMark( i + DINO_TOTAL_CNT, pSlotInfo);
			}
			else
			{
				m_pED_Union[i][RANK_EDBOX_RANK]->SetText( "");
				m_pED_Union[i][RANK_EDBOX_NICK]->SetText( "");
				m_pED_Union[i][RANK_EDBOX_KILLCOUNT]->SetText( "");
		//		m_pED_Union[i][RANK_EDBOX_POINT]->SetText( "");
			}
		}
	}
}

void CBattleGUI_BOSS_Score::ShowScore( bool bVisible, REAL32 tm)
{
	if( bVisible)
	{
		if( m_bVisible == false)
		{
			//GlobalFunc::PB_TRACE("CBattleGUI_BOSS_Score::ShowScore 1");
			m_pPanel->SetPosToCenter( -1, -1);			// 가운데 정렬 계산에서 세로 크기를 조정.
			FLUSH_CONTROL(m_pPanel);
			m_pPanel->SetEnable( TRUE);
		}

		_UpdateScore();

		OnUpdate( tm);
	}
	else
	{
		if( m_bVisible == true)
		{
			m_pPanel->SetEnable( FALSE);
		}
	}

	m_bVisible = bVisible;
}

void CBattleGUI_BOSS_Score::_InitPing(i3Texture* pTexture)
{
	char conv[256];
	INT32 i, idx = 0;

	// BOSS Team Ping
	for( i = 0; i < DINO_TOTAL_CNT; i++, idx++)
	{
		sprintf_s( conv, "Team_Boss/Slot%d/Ping", i);
		m_pST_Ping[idx] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
		I3ASSERT( m_pST_Ping[idx] != NULL);

		_ShowPing( idx, -1);
	}

	// Union Team 0
	for( i = 0; i < UNION_TOTAL_CNT; i++, idx++)
	{
		sprintf_s( conv, "Team_Union/Slot%d/Ping", i);
		m_pST_Ping[ idx] = (i3GuiStatic *) m_pPanel->FindControlByPath( conv);
		I3ASSERT( m_pST_Ping[ idx] != NULL);

		_ShowPing( idx, -1);
	}
}

void CBattleGUI_BOSS_Score::_ShowPing(INT32 nSlot,INT32 idxNet)
{
	INT32 nPingLevel;

	I3_BOUNDCHK( nSlot, MAX_ALL_USER);

	if( idxNet == -1)
	{
		// Initialize
		nPingLevel = 3;
	}
	else if( idxNet == g_pGameContext->getMainSlotIdx())
	{
		// Host다.
		nPingLevel = 6;
	}
	else
	{
		nPingLevel = max( 1, g_pGameContext->GetPing( idxNet));
	}
	
	INT32 U = 0;
	INT32 V = 112;

	switch(nPingLevel)
	{
	case 1:	U = 84;	break;
	case 2:	// fall through
	case 3:	U = 77;	break;
	case 4:	// fall through
	case 5:	U = 70;	break;
	}
				
	INT32	i;

	if( nPingLevel > 5)
	{
		// 호스트다
		for( i = 0; i < MAX_PING_LEVEL; i++)
		{
			m_pST_Ping[ nSlot]->SetShapeEnable( i, FALSE);
		}

		m_pST_Ping[ nSlot]->SetShapeEnable( 5, TRUE);		// 호스트용 Shape
	}
	else
	{
		for( i = 0; i < MAX_PING_LEVEL; i++)
		{
			m_pST_Ping[ nSlot]->SetShapeTextureCoord( i, U, V, 4, 15);
			m_pST_Ping[ nSlot]->SetShapeEnable( i, i < nPingLevel);
		}

		m_pST_Ping[ nSlot]->SetShapeEnable( 5, FALSE);		// 호스트용 Shape
	}

	m_pST_Ping[ nSlot]->SetEnable( TRUE);
}

void CBattleGUI_BOSS_Score::_HidePingAll(void)
{	
	for(INT32 i = 0; i < MAX_ALL_USER; i++)
	{
		m_pST_Ping[i]->SetEnable(FALSE);
	}
}

void CBattleGUI_BOSS_Score::_ShowRankMark(INT32 nSlot,INT32 nRank)
{
	I3ASSERT(0 <= nSlot && MAX_ALL_USER > nSlot);
	I3ASSERT((nRank >= 0 && nRank < RANK_51+1) || (nRank == RANK_GAMEMASTER) || (nRank == RANK_NEWBIE));

	if(g_pGameContext->IsBeginnerChannel1())
		nRank = RANK_NEWBIE;

	REAL32 fX = (REAL32)((nRank % 10) * 24);
	REAL32 fY = (REAL32)((nRank / 10) * 24);

	m_pST_RankMark[nSlot]->SetShapeTextureCoord( 0, (INT32)fX, (INT32)fY, (INT32)(22.0f), (INT32)(22.0f));
	m_pST_RankMark[nSlot]->SetShapeEnable( 0, TRUE);
}

void CBattleGUI_BOSS_Score::_HideRankMarkAll(void)
{
	INT32 i;

	for( i = 0; i < MAX_ALL_USER; i++)
	{
		m_pST_RankMark[i]->SetShapeEnable( 0, FALSE);
	}
}

void CBattleGUI_BOSS_Score::_ShowClanMark(INT32 nSlot, SLOT_INFO * pSlotInfo)
{
	I3_BOUNDCHK( nSlot, MAX_ALL_USER);

	if(pSlotInfo->_clanidx > 0 ) {
		m_pST_Clan[nSlot]->SetShapeEnable(0, TRUE);
		//	Clan mark
		#if defined( USE_CLAN_MARK)
		::SetClanMarkTex( (i3GuiStatic *) m_pST_Clan[nSlot], pSlotInfo);
		#endif
	}
	else {
		m_pST_Clan[nSlot]->SetShapeEnable( 0, FALSE);
	}
}

void CBattleGUI_BOSS_Score::_HideClanMark(void)
{
	for(INT32 i=0; i<MAX_ALL_USER; i++)
	{
		m_pST_Clan[i]->SetShapeEnable( 0, FALSE);
	}
}

void CBattleGUI_BOSS_Score::_SetTipText(void)
{
	INT32 idx = i3Math::Rand() % BOSS_TIP_CNT;

	m_pED_Tip->SetText( m_pTip[ idx]);
}

void CBattleGUI_BOSS_Score::SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, BOOL bDeath)
{
	if( bFlag)
	{
		// Respawn
		switch( nState)
		{
			case GAME_GUI_PRESTART_ROUND :
				if( m_bFirstRound)
				{
					_SetTipText();
					m_bFirstRound = false;
				}
				break;
			default :
				break;
		}
	}
	else {
		switch( nState)
		{
			case GAME_GUI_ROUND_START:
				m_bFirstRound = true;
				break;
			default :
				break;
		}
	}
}
