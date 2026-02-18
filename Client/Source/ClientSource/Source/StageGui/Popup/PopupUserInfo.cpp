#include "pch.h"
#include "GlobalVariables.h"
#include "PopupUserInfo.h"
#include "CommunityContext.h"
#include "Record.h"

I3_CLASS_INSTANCE( CPopupUserInfo, CPopupBase);

CPopupUserInfo::CPopupUserInfo(void)
{
	m_pRoot = NULL;
	m_pCaption = NULL;

	m_pNicknameKey = NULL;
	m_pNicknameValue = NULL;

	m_pSeasonRecordKey = NULL;
	m_pSeasonRecordValue = NULL;
	m_pSeasonKillDeath = NULL;
	m_pSeasonHeadshot = NULL;

	m_pTotalRecordKey = NULL;
	m_pTotalRecordValue = NULL;
	m_pTotalKillDeath = NULL;
	m_pTotalHeadshot = NULL;

	m_pPositionKey = NULL;
	m_pPositionValue = NULL;

	for(INT32 i=0; i<2; i++)
		m_pButton[i] = NULL;

	m_pClose = NULL;

	m_CurrentUser.Reset();
	m_bDone = FALSE;
}

CPopupUserInfo::~CPopupUserInfo(void)
{
}

BOOL CPopupUserInfo::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, "UserInfoPopup", m_pRoot, -1);

	LinkControlEx(plist,  "UserInfoPopupCaption", m_pCaption, -1);
	
	LinkControlEx(plist,  "UserInfoPopupNicknameKey", m_pNicknameKey, -1);
	LinkControlEx(plist,  "UserInfoPopupNicknameValue", m_pNicknameValue, -1);

	LinkControlEx(plist,  "UserInfoPopupSeasonRecordKey", m_pSeasonRecordKey, -1);
	LinkControlEx(plist,  "UserInfoPopupSeasonRecord", m_pSeasonRecordValue, -1);
	LinkControlEx(plist,  "UserInfoPopupSeasonKillDeath", m_pSeasonKillDeath, -1);
	LinkControlEx(plist,  "UserInfoPopupSeasonHeadshot", m_pSeasonHeadshot, -1);

	LinkControlEx(plist,  "UserInfoPopupTotalRecordKey", m_pTotalRecordKey, -1);
	LinkControlEx(plist,  "UserInfoPopupTotalRecord", m_pTotalRecordValue, -1);
	LinkControlEx(plist,  "UserInfoPopupTotalKillDeath", m_pTotalKillDeath, -1);
	LinkControlEx(plist,  "UserInfoPopupTotalHeadshot", m_pTotalHeadshot, -1);

	LinkControlEx(plist,  "UserInfoPopupPositionKey", m_pPositionKey, -1);
	LinkControlEx(plist,  "UserInfoPopupPositionValue", m_pPositionValue, -1);

	LinkControlEx(plist,  "UserInfoPopupButton1", m_pButton[0], -1);
	LinkControlEx(plist,  "UserInfoPopupButton2", m_pButton[1], GCI_USER_INFO_B_ADD_FRIEND);
	LinkControlEx(plist,  "UserInfoPopupClose", m_pClose, GCI_USER_INFO_B_CLOSE);

	return FALSE;
}

void CPopupUserInfo::OnInitControl(void)
{
	m_pRoot->SetEnable(FALSE);

	INIT_WIN_CAPTION( m_pCaption, GAME_STRING("STR_POPUP_DETAIL_INFO"));	//"상세정보"

	INIT_STATIC_CAPTION( m_pNicknameKey, GAME_STRING("STR_POPUP_NICKNAME"));	//	"닉네임"
	INIT_STATIC_CAPTION( m_pSeasonRecordKey, GAME_STRING("STR_POPUP_SEASON_HISTORY"));	//	"시즌전적"
	INIT_STATIC_CAPTION( m_pTotalRecordKey, GAME_STRING("STR_POPUP_TOTAL_HISTORY"));	//	"통산전적"
	INIT_STATIC_CAPTION( m_pPositionKey, GAME_STRING("STR_POPUP_CUR_POSITION"));	//	"현재위치"

	INIT_VALUE_CAPTION( m_pNicknameValue, "");
	INIT_VALUE_CAPTION( m_pSeasonRecordValue, "");
	INIT_VALUE_CAPTION( m_pSeasonKillDeath, "");
	INIT_VALUE_CAPTION( m_pSeasonHeadshot, "");
	INIT_VALUE_CAPTION( m_pTotalRecordValue, "");
	INIT_VALUE_CAPTION( m_pTotalKillDeath, "");
	INIT_VALUE_CAPTION( m_pTotalHeadshot, "");
	INIT_VALUE_CAPTION( m_pPositionValue, "");

	INIT_CAPTION_BUTTON(m_pButton[0], "");
	INIT_CAPTION_BUTTON(m_pButton[1], GAME_STRING("STR_POPUP_ADD_FRIEND"));		//	"친구추가"
	INIT_CAPTION_BUTTON(m_pClose, GAME_STRING("STR_POPUP_CLOSE"));		//	"닫기"

	m_pButton[0]->SetEnable( FALSE);
	m_pPositionKey->SetEnable( FALSE);
	m_pPositionValue->SetEnable( FALSE);

}

void CPopupUserInfo::OnGameEvent( INT32 event, INT32 arg)
{
	//	이벤트 결과를 받아 처리한다.
	/*switch( event )
	{
		
	}*/
}

void CPopupUserInfo::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{    
	switch(pNotify->m_nID) 
	{
		case GCI_USER_INFO_B_ADD_FRIEND:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnAddFriend();
			break;
		case GCI_USER_INFO_B_CLOSE:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)		OnExitKey();
			break;
	}
}

void CPopupUserInfo::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);		//	enable

	if( pArg1)
	{
		i3mem::Copy( &m_CurrentUser, (LOBBY_USER_INFO_LIST*) pArg1, sizeof( LOBBY_USER_INFO_LIST));
	}

	_SetRecord(NULL);

	m_pRoot->SetEnable(TRUE);

	//	친구 버튼
	{
		BOOL bFriend = FALSE;

		for(UINT32 i = 0; i < g_pCommunity->GetFriendCount(); i++)
		{
			const FRIEND_NODE* node = g_pCommunity->GetViewFriend(i);
			//if(NULL == node) continue;

			if (0 == i3String::Compare(m_CurrentUser._Nick, node->_nick))
			{
				bFriend = TRUE;
				break;
			}
		}

		if (g_pGameContext->IsMyNickName(m_CurrentUser._Nick))
		{
			bFriend = TRUE;
		}

		if( bFriend)
		{
			DEACTIVATE_BUTTON( m_pButton[1]);
		}
		else
		{
			ACTIVATE_BUTTON( m_pButton[1]);
		}
	}
}

void CPopupUserInfo::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pRoot->SetEnable(FALSE);

	m_CurrentUser.Reset();
	m_bDone = FALSE;
}

void CPopupUserInfo::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate( rDeltaSeconds);

	//if( !m_bDone)
	{
		if( i3String::Compare( g_pCommunity->GetCommnunityInfoNick(), m_CurrentUser._Nick) == 0)
		{
			const USER_INFO_RECORD* pRecord = g_pCommunity->GetCommunityRecord();

			_SetRecord( pRecord);

			m_bDone = TRUE;
		}
	}
}

void CPopupUserInfo::OnPreGuiNotify(void)
{
	
}

void CPopupUserInfo::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupUserInfo::OnAddFriend(void)
{
	char nick[MAX_STRING_COUNT];
	
	if (g_pCommunity->GetFriendCount() >= FRIEND_MAX_COUNT)
	{
		STRING_POPUP( GAME_STRING("STR_POPUP_MESSAGE_NO_MORE_FRIEND"));		//	"더이상 친구를 추가할 수 없습니다."
		return;
	}

	if( m_CurrentUser._Session <= 0)		
		return;

	i3String::Copy(nick, m_CurrentUser._Nick, MAX_STRING_COUNT );

	g_pCommunity->InsertFriend(nick);
}

void CPopupUserInfo::_SetRecord( const USER_INFO_RECORD * pRecord)
{
#ifndef USE_MODE_RECORD
	if( pRecord != NULL)
	{
		char record[MAX_STRING_COUNT] = "";
		char killDeath[MAX_STRING_COUNT] = "";
		char headshot[MAX_STRING_COUNT] = "";

		m_pNicknameValue->SetEnable(TRUE);
		m_pNicknameValue->SetTextEllipsis((char*)g_pCommunity->GetCommnunityInfoNick());

		m_pSeasonRecordValue->SetEnable(TRUE);
		m_pSeasonKillDeath->SetEnable(TRUE);
		m_pSeasonHeadshot->SetEnable(TRUE);
		m_pTotalRecordValue->SetEnable(TRUE);
		m_pTotalKillDeath->SetEnable(TRUE);
		m_pTotalHeadshot->SetEnable(TRUE);

		// 시즌 기록
		{
			//	"%d전 %d승 %d패"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_smatch, pRecord->_swin, pRecord->_slose);

			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_skillcount, pRecord->_sdeath, g_pGameContext->getKillRate(pRecord->_skillcount, pRecord->_sdeath));
			
			//	"HEADSHOT: %d%%"
			INT32 sheadshotPercent = SeasonHeadShotPercent(pRecord);
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), sheadshotPercent);

			m_pSeasonRecordValue->SetText(record);
			m_pSeasonKillDeath->SetText(killDeath);
			m_pSeasonHeadshot->SetText(headshot);
		}

		// 토탈 기록
		{
			//	"%d전 %d승 %d패"
			sprintf_s(record, GAME_STRING("STR_POPUP_WHAT_RECORDING_HISTORY"), pRecord->_match, pRecord->_win, pRecord->_lose);

			//	"KILL/DEATH: %d/%d"
			sprintf_s(killDeath, GAME_STRING("STR_POPUP_KILL_AND_DEATH"), pRecord->_killcount, pRecord->_death, g_pGameContext->getKillRate(pRecord->_killcount, pRecord->_death));

			//	"HEADSHOT: %d%%"
			INT32 headshotPercent =	TotalHeadShotPercent(pRecord);			
			sprintf_s(headshot, GAME_STRING("STR_POPUP_HEADSHOT"), headshotPercent);

			m_pTotalRecordValue->SetText(record);
			m_pTotalKillDeath->SetText(killDeath);
			m_pTotalHeadshot->SetText(headshot);
		}     
	}
	else
	{
		m_pNicknameValue->SetEnable(FALSE);
		m_pSeasonRecordValue->SetEnable(FALSE);
		m_pSeasonKillDeath->SetEnable(FALSE);
		m_pSeasonHeadshot->SetEnable(FALSE);
		m_pTotalRecordValue->SetEnable(FALSE);
		m_pTotalKillDeath->SetEnable(FALSE);
		m_pTotalHeadshot->SetEnable(FALSE);
		m_pPositionValue->SetEnable(FALSE);
	}
#endif
}
