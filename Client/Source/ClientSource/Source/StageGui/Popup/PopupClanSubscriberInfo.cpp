#include "pch.h"
#include "GameDefine.h"
#include "ClanDef.h"
#include "GlobalVariables.h"
#include "PopupClanSubscriberInfo.h"

I3_CLASS_INSTANCE( CPopupClanSubscriberInfo, CPopupBase);

CPopupClanSubscriberInfo::CPopupClanSubscriberInfo(void)
{
	m_pClanSubscriberInfoGui = NULL;

	m_pClanSubscriberInfoCaption = NULL;

	m_pClanSubscriberInfoPrev = NULL;
	m_pClanSubscriberInfoNext = NULL;
	m_pClanSubscriberInfoOk = NULL;

	m_pClanSubscriberInfoRank = NULL;
	m_pClanSubscriberInfoReceiverStatic = NULL;
	m_pClanSubscriberInfoReceiverValue = NULL;
	m_pClanSubscriberInfoNickStatic = NULL;
	m_pClanSubscriberInfoNickValue = NULL;
	m_pClanSubscriberInfoWinStatic = NULL;
	m_pClanSubscriberInfoWinValue = NULL;
	m_pClanSubscriberInfoScoreStatic = NULL;
	m_pClanSubscriberInfoScoreValue = NULL;
	m_pClanSubscriberInfoMessageStatic = NULL;
	m_pClanSubscriberInfoMessageValue = NULL;
	m_pClanSubscriberInfoPageValue = NULL;

	m_nPage = 0;

	m_pClanContext = g_pFramework->GetClanContext();
	I3ASSERT(m_pClanContext);
}

CPopupClanSubscriberInfo::~CPopupClanSubscriberInfo(void)
{
}
	
void CPopupClanSubscriberInfo::OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify)
{
	switch(pNotify->m_nID)
	{
		case GCI_CLAN_B_POPUP_SUBSCRIBER_PREV:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnPrevPage();
			break;
		case GCI_CLAN_B_POPUP_SUBSCRIBER_NEXT:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnNextPage();
			break;
		case GCI_CLAN_B_POPUP_SUBSCRIBER_OK:
			if (I3GUI_BUTTON_NOTIFY_CLICKED == pNotify->m_nEvent)	OnExitKey();
			break;
	}
}

BOOL CPopupClanSubscriberInfo::OnLinkControl(i3BinList * plist, i3GameNode * pControl)
{
	LinkControlEx(plist, 	"clanSubscriberInfoPopup", m_pClanSubscriberInfoGui, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupCaption", m_pClanSubscriberInfoCaption, -1);

	LinkControlEx(plist,  "clanSubscriberInfoPopupPrev", m_pClanSubscriberInfoPrev, GCI_CLAN_B_POPUP_SUBSCRIBER_PREV);
	LinkControlEx(plist,  "clanSubscriberInfoPopupNext", m_pClanSubscriberInfoNext, GCI_CLAN_B_POPUP_SUBSCRIBER_NEXT);
	LinkControlEx(plist,  "clanSubscriberInfoPopupOk", m_pClanSubscriberInfoOk, GCI_CLAN_B_POPUP_SUBSCRIBER_OK);

	LinkControlEx(plist,  "clanSubscriberInfoPopupRankImage", m_pClanSubscriberInfoRank, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupReceiverStatic", m_pClanSubscriberInfoReceiverStatic, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupReceiverValue", m_pClanSubscriberInfoReceiverValue, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupNickStatic", m_pClanSubscriberInfoNickStatic, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupNickValue", m_pClanSubscriberInfoNickValue, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupWinStatic", m_pClanSubscriberInfoWinStatic, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupWinValue", m_pClanSubscriberInfoWinValue, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupScoreStatic", m_pClanSubscriberInfoScoreStatic, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupScoreValue", m_pClanSubscriberInfoScoreValue, -1);	
	LinkControlEx(plist,  "clanSubscriberInfoPopupMessageStatic", m_pClanSubscriberInfoMessageStatic, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupMemoEdit", m_pClanSubscriberInfoMessageValue, -1);
	LinkControlEx(plist,  "clanSubscriberInfoPopupPageValue", m_pClanSubscriberInfoPageValue, -1);

	return FALSE;
}

void CPopupClanSubscriberInfo::OnInitControl(void)
{
	char temp[256] = "";

	INIT_WIN_CAPTION( m_pClanSubscriberInfoCaption, GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_INFO"));		//	"클랜 가입 대기자 정보"

	INIT_CAPTION_BUTTON( m_pClanSubscriberInfoPrev, GAME_STRING("STBL_IDX_CLAN_PREV"));	// "이전"
	INIT_CAPTION_BUTTON( m_pClanSubscriberInfoNext, GAME_STRING("STBL_IDX_CLAN_NEXT"));	// "다음"
	INIT_CAPTION_BUTTON( m_pClanSubscriberInfoOk, GAME_STRING("STBL_IDX_CLAN_BUTTON_OK"));		// "확인"

	INIT_STATIC_CAPTION(m_pClanSubscriberInfoReceiverStatic, "To.");

	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_MEMBER_NAME"));	//	"닉네임 :"
	INIT_STATIC_CAPTION(m_pClanSubscriberInfoNickStatic, temp);

	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_WIN_SCORE"));	//	"승수 :"
	INIT_STATIC_CAPTION(m_pClanSubscriberInfoWinStatic, temp); 

	sprintf_s(temp, "%s :", GAME_STRING("STBL_IDX_CLAN_USER_RECORD"));	//	"전적 :"
	INIT_STATIC_CAPTION(m_pClanSubscriberInfoScoreStatic, temp);

	INIT_STATIC_CAPTION(m_pClanSubscriberInfoMessageStatic, GAME_STRING("STBL_IDX_CLAN_SUBSCRIBER_NOTE"));	//	"가입 신청 메세지"
	INIT_STATIC_CAPTION2(m_pClanSubscriberInfoPageValue, "");

	INIT_VALUE_CAPTION(m_pClanSubscriberInfoReceiverValue, "");
	INIT_VALUE_CAPTION(m_pClanSubscriberInfoNickValue, "");
	INIT_VALUE_CAPTION(m_pClanSubscriberInfoWinValue, "");
	INIT_VALUE_CAPTION(m_pClanSubscriberInfoScoreValue, "");	

	m_pClanSubscriberInfoMessageValue->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
	m_pClanSubscriberInfoMessageValue->SetTextColor(GetColor(GCT_DEFAULT));
	m_pClanSubscriberInfoMessageValue->SetText("");
	m_pClanSubscriberInfoMessageValue->SetTextSpace( 0, 5);
	m_pClanSubscriberInfoMessageValue->SetTextAutoWrap( TRUE);

	m_pClanSubscriberInfoGui->SetEnable(FALSE);

	DEACTIVATE_BUTTON(m_pClanSubscriberInfoPrev);
	DEACTIVATE_BUTTON(m_pClanSubscriberInfoNext);

	m_pClanSubscriberInfoPrev->OnUpdate(0.0f);

	CPopupBase::OnInitControl();
}

void CPopupClanSubscriberInfo::OnGameEvent( INT32 event, INT32 arg)
{
}

void CPopupClanSubscriberInfo::SetInputDisable(BOOL bDisable)
{
	m_pClanSubscriberInfoGui->setInputDisable(bDisable);
}

BOOL CPopupClanSubscriberInfo::GetInputDisable(void)
{
	return m_pClanSubscriberInfoGui->getInputDisable();
}

void CPopupClanSubscriberInfo::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	char temp[MAX_STRING_COUNT] = "";
	BOOL bValidContents = FALSE;

	CPopupBase::OnUpdate( rDeltaSeconds);

	CLAN_REQUEST_DETAIL_INFO * pInfo = m_pClanContext->GetMyClanSubscriberDetail();

	if( IsMultiPage())
	{
		m_pClanSubscriberInfoPageValue->SetEnable(TRUE);

		sprintf_s(temp, "%d / %d Page",  m_nPage + 1, m_pClanContext->GetIndexBufferCount());
		m_pClanSubscriberInfoPageValue->SetText(temp);
	}
	else
	{
		m_pClanSubscriberInfoPageValue->SetEnable(FALSE);
	}

	if(pInfo)
	{
		if(pInfo->_i64UID != 0)
		{
			//	TO.
			SetSubscriberReceiver(g_pGameContext->getMyClanDetailInfo()->_name);
	
			//	닉네임
			SetSubscriberUser( pInfo->_nick, pInfo->_rank);

			//	승수
			SetSubscriberWin( pInfo->_kill, pInfo->_death);

			//	전적
			SetSubscriberScore( pInfo->_match, pInfo->_win, pInfo->_lose);

			//	신청 내용
			SetSubscriberMessage(pInfo->_contents);

			bValidContents = TRUE;
		}
	}

	if( !bValidContents)
	{
		SetSubscriberReceiver("");
		SetSubscriberUser("", 0);
		SetSubscriberWin(0, 0);
		SetSubscriberScore(0, 0, 0);
		SetSubscriberMessage("");
	}
}

void CPopupClanSubscriberInfo::OnPreGuiNotify(void)
{
	CPopupBase::OnPreGuiNotify();
}

void CPopupClanSubscriberInfo::OnExitKey(void)
{
	OnClosePopup();
}

void CPopupClanSubscriberInfo::OnWheelUpdate(INT32 scrollby)
{
	CPopupBase::OnWheelUpdate( scrollby);
}	

void CPopupClanSubscriberInfo::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup( pArg1, pArg2);

	m_pClanSubscriberInfoGui->SetEnable( TRUE);

	//	서버에 가입 대기자 정보 요청	
	TUID dbIdx = m_pClanContext->GetIndexBuffer(0);

	m_pClanContext->RequsetSubscriberInfo(dbIdx);

	//	여러 페이지가 있을 경우
	if( g_pFramework->GetClanContext()->GetIndexBufferCount() > 1)
	{	
		m_pClanSubscriberInfoPrev->SetEnable(TRUE);
		m_pClanSubscriberInfoNext->SetEnable(TRUE);

		DEACTIVATE_BUTTON(m_pClanSubscriberInfoPrev);
		ACTIVATE_BUTTON(m_pClanSubscriberInfoNext);
	}
	else
	{
		m_pClanSubscriberInfoPrev->SetEnable(FALSE);
		m_pClanSubscriberInfoNext->SetEnable(FALSE);
	}

	m_nPage = 0;
}

void CPopupClanSubscriberInfo::OnClosePopup(void * pArg1, void * pArg2)
{
	m_pClanSubscriberInfoGui->SetEnable( FALSE);

	CPopupBase::OnClosePopup( pArg1, pArg2);
}

void CPopupClanSubscriberInfo::OnNextPage(void)
{
	if( m_nPage < m_pClanContext->GetIndexBufferCount() - 1)
	{
		m_nPage++;

		I3_BOUNDCHK(m_nPage, m_pClanContext->GetIndexBufferCount());

		TUID dbIdx = m_pClanContext->GetIndexBuffer(m_nPage);

		m_pClanContext->RequsetSubscriberInfo(dbIdx);

		//	이전 페이지 버튼 활성
		ACTIVATE_BUTTON(m_pClanSubscriberInfoPrev);

		//	다음 페이지 버튼 비활성
		if( m_nPage >= m_pClanContext->GetIndexBufferCount() - 1)
			DEACTIVATE_BUTTON(m_pClanSubscriberInfoNext);
	}
}

void CPopupClanSubscriberInfo::OnPrevPage(void)
{
	if( m_nPage > 0)
	{
		m_nPage--;

		I3_BOUNDCHK(m_nPage, m_pClanContext->GetIndexBufferCount());

		TUID dbIdx = m_pClanContext->GetIndexBuffer(m_nPage);

		m_pClanContext->RequsetSubscriberInfo(dbIdx);

		//	다음 페이지 버튼 활성
		ACTIVATE_BUTTON(m_pClanSubscriberInfoNext);

		//	이전 페이지 버튼 비활성
		if( m_nPage <= 0)
			DEACTIVATE_BUTTON(m_pClanSubscriberInfoPrev);
	}
}

BOOL CPopupClanSubscriberInfo::IsMultiPage(void)
{
	return (m_pClanContext->GetIndexBufferCount() > 1);
}

void CPopupClanSubscriberInfo::SetSubscriberReceiver(char * pszName)
{
	m_pClanSubscriberInfoReceiverValue->SetText( pszName);
}

void CPopupClanSubscriberInfo::SetSubscriberUser(char * pszName, UINT8 nRank)
{
	m_pClanSubscriberInfoNickValue->SetTextEllipsis( pszName);

	if( pszName != NULL && strlen(pszName) > 0)
	{
		m_pClanSubscriberInfoRank->SetEnable(TRUE);
		SetRankImage(m_pClanSubscriberInfoRank, nRank);
	}
	else
	{
		m_pClanSubscriberInfoRank->SetEnable(FALSE);
	}
}

void CPopupClanSubscriberInfo::SetSubscriberWin(UINT32 nKill, UINT32 nDeath)
{
	char temp[MAX_STRING_COUNT] = "";

	sprintf_s(temp, "%u Kill / %u Death",  nKill, nDeath);
	m_pClanSubscriberInfoWinValue->SetText(temp);
}

void CPopupClanSubscriberInfo::SetSubscriberScore(UINT32 nMatch, UINT32 nWin, UINT32 nLose)
{
	char temp[MAX_STRING_COUNT] = "";

	//	"%u전 %u승 %u패"
	//	"%d전 %d승 %d패"
	sprintf_s(temp, GAME_STRING("STBL_IDX_CLAN_DECLARE_SCORE"),  (INT32)nMatch, (INT32)nWin, (INT32)nLose);
	m_pClanSubscriberInfoScoreValue->SetText(temp);
}

void CPopupClanSubscriberInfo::SetSubscriberMessage(char * pszMessage)
{
	m_pClanSubscriberInfoMessageValue->SetText(pszMessage);
}
