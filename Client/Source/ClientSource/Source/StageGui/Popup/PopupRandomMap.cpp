#include "pch.h"
#include "PopupRandomMap.h"



//////////////////////////////////////////////////////////////////////////

I3_CLASS_INSTANCE(CPopupRandomMap, CPopupBase);


CPopupRandomMap::CPopupRandomMap( void )
{	
	
	m_pRandomMapWin = NULL;
	m_pRandomMapCaption= NULL;
	m_pRandomMapConfirm= NULL;
	m_pRandomMapCancel= NULL;
	for(INT32 i = 0 ; i < COUNT_CHECKBOX_MAX; i++)
	{
		m_bPrevRandom[i]= TRUE;
	}
//	 BombMatchList =0;
//	 DeathMatchList=0;
//	 DestroyMatchList=0;
	 m_IndexMap = 0;
	 m_IndexCheck= 0;
	 
	}

CPopupRandomMap::~CPopupRandomMap()
{	
	

}

void CPopupRandomMap::OnGuiNotify( I3GUI_CONTROL_NOTIFY* pNotify )
{
	switch(pNotify->m_nID)
	{
	case GCI_RRP_B_RANDOMMAP_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_OnRandomMapConfirm();
		break;

	case GCI_RRP_B_RANDOMMAP_CANCEL:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_OnRandomMapCancel();
		break;

	case GCI_RRP_B_CHECK_COUNT_CONFIRM:
		if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)
			_CountCheck();
		break;

	}
}

BOOL CPopupRandomMap::OnLinkControl( i3BinList * plist, i3GameNode * pControl )
{

	LinkControlEx(plist, "RandomMapWin", m_pRandomMapWin, -1);
	LinkControlEx(plist, "RandomMapCaption", m_pRandomMapCaption, -1);
	LinkControlEx(plist, "RandomMapConfirm", m_pRandomMapConfirm, GCI_RRP_B_RANDOMMAP_CONFIRM);
	LinkControlEx(plist, "RandomMapCancel", m_pRandomMapCancel, GCI_RRP_B_RANDOMMAP_CANCEL);
	LinkControlEx(plist, "CheckCountWin", m_pCheckCountWin, -1);	
	LinkControlEx(plist, "CheckCountConfirm", m_pCheckCountConfirm, GCI_RRP_B_CHECK_COUNT_CONFIRM);
	LinkControlEx(plist, "CheckCountMessage", m_pCheckCountEditBox, -1);
	
	for(UINT32 i = 0; i < COUNT_CHECKBOX_MAX; i++)
	{
		char strName[MAX_STRING_COUNT];
		sprintf_s(strName, "MapCheck%d", i);
		LinkControlEx(plist, strName, m_pMapCheck[i], -1);
		sprintf_s(strName, "MapName%d", i);
		LinkControlEx(plist, strName, m_pMapName[i], -1);
	}


	return FALSE;
}

void CPopupRandomMap::OnInitControl( void )
{
	INIT_CAPTION_BUTTON(m_pRandomMapConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/	
	INIT_CAPTION_BUTTON(m_pCheckCountConfirm, GAME_STRING("STR_TBL_GUI_LOBBY_CONFIRM_TEXT"));/*확인*/
	INIT_CAPTION_BUTTON(m_pRandomMapCancel, GAME_STRING("STR_TBL_GUI_LOBBY_CANCEL_TEXT"));/*취소*/

	for(int i =0; i<COUNT_CHECKBOX_MAX; i++)
	{
		INIT_CHECK_EX(m_pMapCheck[i]);
		m_pMapCheck[i]->setCheckBoxState( g_pGameContext->IsRandomMap(i));
		m_pMapCheck[i]->SetEnable(FALSE);

	}

	m_pCheckCountEditBox->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pCheckCountEditBox->SetText(GAME_STRING("STR_POPUP_RANDOMMAP_CHECK_COUNT"));
	m_pRandomMapCaption->SetTextAlign(TEXT_ATTR_ALIGN_MIDDLE, TEXT_ATTR_ALIGN_MIDDLE);
	m_pRandomMapCaption->SetText( GAME_STRING("STR_POPUP_RANDOMMAP_CONFIG"));

	m_pRandomMapWin->SetEnable(FALSE);
	m_pCheckCountWin->SetEnable(FALSE);
	CPopupBase::OnInitControl();
}

void CPopupRandomMap::_InitCheckState()
{	
	for(INT32 i =0; i < COUNT_CHECKBOX_MAX; i++)
	{
		m_pMapCheck[i]->setCheckBoxState(TRUE);
		m_bPrevRandom[i] = TRUE;
		//g_pGameContext->setRandomMap(i, true);
		
	}

	g_pGameContext->InitRandomCheck( true );
}

void CPopupRandomMap::OnOpenPopup( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/ )
{

	m_CurGameMode = static_cast<STAGE_TYPE>((INT32)pArg1);
	I3ASSERT(STAGE_TYPE_NA != m_CurGameMode);

	CPopupBase::OnOpenPopup( pArg1, pArg2);
	m_pRandomMapWin->SetEnable(TRUE);


	//체크박스 상태를 불러옵니다.
	for(INT32 i =0; i< COUNT_CHECKBOX_MAX; i++)
	{
		m_pMapCheck[i]->setCheckBoxState( g_pGameContext->IsRandomMap(i));
	}
	//맵타입이 바뀌면 초기화 체크박스를 초기화 합니다
	if(g_pGameContext->GetRandomStageType() != m_CurGameMode)
	{
		_InitCheckState();
	}

/*	g_pGameContext->ClearRandomMapIndexList();


	//m_IndexMapList에 타입별로 출력할 맵을 ADD합니다.
	for(INT32 i = STAGE_ORD_NA + 1; i < STAGE_ORD_MAX; i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)i);

		if ( isActiveStage(stageId))
		{	
			if(g_pGameContext->IsStageMaskTraining())
			{

				m_IndexMap = i;
#if defined(LOCALE_THAILAND)
				if(isChallengeDieHardStage(stageId))
#else
				if(isChallengeStage(stageId))
#endif
				{
					g_pGameContext->AddRandomMapIndex( m_IndexMap);
				}


			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && m_CurGameMode == STAGE_TYPE_ANNIHILATION||m_CurGameMode == getStageType(stageId))
			{
				m_IndexMap = i;

				if(getStageSlot(stageId)!=STAGE_SLOT_MODE_4VS4)
					g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (m_CurGameMode == STAGE_TYPE_ANNIHILATION)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_ANNIHILATION);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && m_CurGameMode == STAGE_TYPE_SUDDEN_DEATH||m_CurGameMode == getStageType(stageId))
			{
				m_IndexMap = i;

				if(getStageSlot(stageId)!=STAGE_SLOT_MODE_4VS4)
					g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (m_CurGameMode == STAGE_TYPE_SUDDEN_DEATH)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_SUDDEN_DEATH);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && m_CurGameMode == STAGE_TYPE_HEAD_HUNTER||m_CurGameMode == getStageType(stageId))
			{
				m_IndexMap = i;

				if(getStageSlot(stageId)!=STAGE_SLOT_MODE_4VS4)
					g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (m_CurGameMode == STAGE_TYPE_HEAD_HUNTER)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_HEAD_HUNTER);
				}
			}
			else if(STAGE_TYPE_DEATHMATCH == getStageType(stageId) && m_CurGameMode == STAGE_TYPE_HEAD_KILLER||m_CurGameMode == getStageType(stageId))
			{
				m_IndexMap = i;

				if(getStageSlot(stageId)!=STAGE_SLOT_MODE_4VS4)
					g_pGameContext->AddRandomMapIndex( m_IndexMap);

				if (m_CurGameMode == STAGE_TYPE_HEAD_KILLER)
				{
					stageId = (STAGE_ID)setStageSlot(stageId, STAGE_TYPE_HEAD_KILLER);
				}
			}

		}
	}
*/
	{
		if(!g_pGameContext->IsStageMaskTraining())
		{
			switch(m_CurGameMode)
			{
			case STAGE_TYPE_DEATHMATCH:
			case STAGE_TYPE_HEAD_HUNTER:
				_DeathMatchCheckBox();
				break;
			case STAGE_TYPE_BOMB:
				_BombCheckBox();
				break;
			case STAGE_TYPE_DESTROY:
				_DestroyCheckBox();
				break;
			case STAGE_TYPE_DEFENCE:
				_DestroyCheckBox();
				break;
			case STAGE_TYPE_ANNIHILATION:
			case STAGE_TYPE_SUDDEN_DEATH:
			case STAGE_TYPE_HEAD_KILLER:
				_DeathMatchCheckBox();	
				break;
			case STAGE_TYPE_ESCAPE:
				_BossCheckBox();
				break;
			default:	
				_DeathMatchCheckBox();
				break;
			}
		}
		else
		{
			_ChallengeMatchCheckBox();
		}
	}
	
	//체크박스 상태를 불러옵니다.
	/*for(INT32 i =0; i< COUNT_CHECKBOX_MAX; i++)
	{
		m_bPrevRandom[i] =	m_pMapCheck[i]->getCheckBoxState();
	}*/

	g_pGameContext->SetRandomStageType(m_CurGameMode);
}

// 한개도 체크하지 않은 상태에서 확인을 누르면 팝업이 뜨는데 그 팝업의 확인 버튼을 누를 경우
// 랜덤맵 체크 박스를 이전상태로 돌린다
void CPopupRandomMap::_CountCheck()
{
	for(INT32 i =0; i < COUNT_CHECKBOX_MAX; i++)
	{
		m_pMapCheck[i]->setCheckBoxState(m_bPrevRandom[i]);
		/*g_pGameContext->setRandomMap(i, (m_pMapCheck[i]->getCheckBoxState()?TRUE:FALSE));
		if(m_pMapCheck[i]->getCheckBoxState())
		{	
			m_IndexCheck = i;
			g_pGameContext->AddCheckCountList(m_IndexCheck);
		}*/
	}

	m_pCheckCountWin->SetEnable(FALSE);

}

void CPopupRandomMap::OnClosePopup( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/ )
{
	m_pRandomMapWin->SetEnable(FALSE);
	CPopupBase::OnClosePopup();
	
}
void CPopupRandomMap::OnExitKey( void )
{
	m_pRandomMapWin->SetEnable(FALSE);
	OnClosePopup();
}


BOOL CPopupRandomMap::OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code )
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{

			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				if (m_pRandomMapWin->isEnable())
				{
					_OnRandomMapCancel();
					return TRUE;
				}

			}
			if (pKeyboard->GetStrokeState(I3I_KEY_ENTER))
			{
				if (m_pRandomMapWin->isEnable())
				{
					_OnRandomMapConfirm();
					return TRUE;
				}
			}
			
		}
	}

	return CPopupBase::OnEvent(event, pObj, param2, code);
}
void CPopupRandomMap::_OnRandomMapConfirm()
{
	/*g_pGameContext->ClearCheckCountList();
	
	//맵의 체크상태를 m_CheckCountList에 ADD합니다.
	for(INT32 i = 0; i <COUNT_CHECKBOX_MAX; i++)
	{
		g_pGameContext->setRandomMap(i, (m_pMapCheck[i]->getCheckBoxState()?TRUE:FALSE));
		
		if(m_pMapCheck[i]->getCheckBoxState())
		{	
			m_IndexCheck = i;
			g_pGameContext->AddCheckCountList(m_IndexCheck);
		}
	}*/

	BOOL	bCheck	= FALSE;

	for( INT32 i = 0; i < COUNT_CHECKBOX_MAX; i++)
	{
		if( m_pMapCheck[i]->getCheckBoxState() )
		{
			bCheck	= TRUE;
			break;
		}
	}

	//체크값이 하나 이하면 체크카운트 윈도우 팝업합니다.
	//if(g_pGameContext->GetCheckCountListCount() < 1)
	if( bCheck == FALSE )
	{
		m_pCheckCountWin->SetEnable(TRUE);
	}
	else
	{
		_RunRandomMap();
		OnClosePopup();
		m_pRandomMapWin->SetEnable(FALSE);
	}
	
	
}
void CPopupRandomMap::_OnRandomMapCancel()
{			
	//캔슬할 경우 저장된 체크 상태로 되돌립니다.
	//for(INT32 i =0; i < COUNT_CHECKBOX_MAX; i++)
	//{
	//	m_pMapCheck[i]->setCheckBoxState(m_bPrevRandom[i]);
	//}
	//if(g_pGameContext->GetCheckCountListCount() < 1)
	//{
	//	m_pCheckCountWin->SetEnable(TRUE);
	//}
	//else
	{
		//_RunRandomMap();
		OnClosePopup();
		m_pRandomMapWin->SetEnable(FALSE);
	}
	
}
void CPopupRandomMap::_ChallengeMatchCheckBox()
{
	for(INT32 I =0 ; I<COUNT_CHECKBOX_MAX;I++)
	{	
		if(I<g_pGameContext->GetRandomMapIndexCount())
		{
			m_pMapCheck[I]->SetEnable(true);
		}
		else
		{			
			m_pMapCheck[I]->SetEnable(false);
			m_pMapCheck[I]->setCheckBoxState(FALSE);
		}
	}
	for(INT32 i =0; i < g_pGameContext->GetRandomMapIndexCount() ; i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)g_pGameContext->GetRandomMapIndex(i));
		char szTemp[256];

		getStageDisplayName(szTemp, stageId);
		m_pMapName[i]->SetText(szTemp);
		m_pMapCheck[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}

}
void CPopupRandomMap::_DeathMatchCheckBox()
{

	for(INT32 i =0 ; i<COUNT_CHECKBOX_MAX;i++)
	{	
		if(i<g_pGameContext->GetRandomMapIndexCount())
		{
			m_pMapCheck[i]->SetEnable(true);
		}
		else
		{			
			m_pMapCheck[i]->SetEnable(false);
			m_pMapCheck[i]->setCheckBoxState(FALSE);
		}
	}
	for(INT32 i =0; i < g_pGameContext->GetRandomMapIndexCount() ; i++)
	{
		
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)g_pGameContext->GetRandomMapIndex(i));
		char szTemp[256];

		getStageDisplayName(szTemp, stageId);
		m_pMapName[i]->SetText(szTemp);
		m_pMapCheck[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}

}

void CPopupRandomMap::_DestroyCheckBox()
{

	for(INT32 i =0 ; i<COUNT_CHECKBOX_MAX;i++)
	{	
		if(i<g_pGameContext->GetRandomMapIndexCount())
		{
			m_pMapCheck[i]->SetEnable(true);
		}
		else
		{			
			m_pMapCheck[i]->SetEnable(false);
			m_pMapCheck[i]->setCheckBoxState(FALSE);
		}
	}
	for(int i=0;i<g_pGameContext->GetRandomMapIndexCount();i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)g_pGameContext->GetRandomMapIndex(i));
		char szTemp[256];

		getStageDisplayName(szTemp, stageId);
		m_pMapName[i]->SetText(szTemp);
		m_pMapCheck[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);
	}
	
}
void CPopupRandomMap::_BossCheckBox()
{

	for(INT32 I =0 ; I<COUNT_CHECKBOX_MAX;I++)
	{
		if(I<g_pGameContext->GetRandomMapIndexCount())
			m_pMapCheck[I]->SetEnable(true);
		else
		{
			m_pMapCheck[I]->SetEnable(false);
			m_pMapCheck[I]->setCheckBoxState(FALSE);
		}
	}
	for(int i=0;i<g_pGameContext->GetRandomMapIndexCount();i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)g_pGameContext->GetRandomMapIndex(i));
		char szTemp[256];

		getStageDisplayName(szTemp, stageId);

		m_pMapName[i]->SetText(szTemp);
		m_pMapCheck[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	}

}
void CPopupRandomMap::_BombCheckBox()
{

	for(INT32 I =0 ; I<COUNT_CHECKBOX_MAX;I++)
	{
		if(I<g_pGameContext->GetRandomMapIndexCount())
			m_pMapCheck[I]->SetEnable(true);
		else
		{
			m_pMapCheck[I]->SetEnable(false);
			m_pMapCheck[I]->setCheckBoxState(FALSE);
		}
	}
	for(int i=0;i<g_pGameContext->GetRandomMapIndexCount();i++)
	{
		STAGE_ID stageId = getStageId((STAGE_ORDINAL_ID)g_pGameContext->GetRandomMapIndex(i));
		char szTemp[256];

		getStageDisplayName(szTemp, stageId);
		m_pMapName[i]->SetText(szTemp);
		m_pMapCheck[i]->setControlState(I3GUI_CONTROL_STATE_NORMAL);

	}

}
void CPopupRandomMap::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	//	CPopupBase::OnUpdate( rDeltaSeconds);
}

void CPopupRandomMap::_RunRandomMap()
{	
/*	g_pGameContext->ClearRandomMapList();

	switch(m_CurGameMode)
	{
	case STAGE_TYPE_DEATHMATCH:
	case STAGE_TYPE_ANNIHILATION:
	case STAGE_TYPE_SUDDEN_DEATH:
	case STAGE_TYPE_HEAD_HUNTER:
	case STAGE_TYPE_HEAD_KILLER:
		{
			for(INT32 i =0; i<g_pGameContext->GetRandomMapIndexCount(); i++)
			{
				if(m_pMapCheck[i]->getCheckBoxState())
				{
					DeathMatchList = g_pGameContext->GetRandomMapIndex(i);
					g_pGameContext->AddRandomMap(DeathMatchList);
				}
			}
		}
		break;

	case STAGE_TYPE_DESTROY:
	case STAGE_TYPE_DEFENCE:
		{
			for(INT32 i =0; i<g_pGameContext->GetRandomMapIndexCount(); i++)
			{
				if(m_pMapCheck[i]->getCheckBoxState())
				{

					DestroyMatchList = g_pGameContext->GetRandomMapIndex(i);
					g_pGameContext->AddRandomMap(DestroyMatchList);
				}
			}
		}
		break;

	case STAGE_TYPE_BOMB:
		{
			for(INT32 i =0; i<g_pGameContext->GetRandomMapIndexCount(); i++)
			{
				if(m_pMapCheck[i]->getCheckBoxState())
				{
					BombMatchList = g_pGameContext->GetRandomMapIndex(i);
					g_pGameContext->AddRandomMap(BombMatchList);
				}
			}
		}
		break;
	}
*/
	// 체크박스에서 체크된 맵만 리스트로 만들고 체크박스 리스트 저장
	INT32			nCount	= g_pGameContext->GetRandomMapIndexCount();
	vector<INT32>	vecChecked;

	for( INT32 i = 0; i < nCount; i++ )
	{
		if( m_pMapCheck[i]->getCheckBoxState() == TRUE ) vecChecked.push_back( i );
	}

	g_RandomMap.SetRandomMapList( &vecChecked );

	vecChecked.clear();
}
