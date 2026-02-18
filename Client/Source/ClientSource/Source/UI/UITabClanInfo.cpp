#include "pch.h"
#include "UITabClanInfo.h"
#include "UIMainFrame.h"
#include "UIPhaseBase.h"
#include "UIPhaseSubBase.h"
#include "UIPhaseClanMember.h"
#include "ClanDef.h"
#include "../CommunityContext.h"
#include "ClanContext.h"
#include "UIUtil.h"

#include "../GameGUI.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/substr.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE( UITabClanInfo);//, UITabClan_Normal);

static UITabClanInfo * GetThis()
{
	UIPhaseClanMember* pPhase = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		return static_cast<UITabClanInfo*>(pPhase->GetTab(UTC_INFO));
	}
	return nullptr;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

}

LuaFuncReg UITabClanInfo_Glue[] = {
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UITabClanInfo::UITabClanInfo()
{
	m_hdc = nullptr;
}

UITabClanInfo::~UITabClanInfo()
{
	if( nullptr != m_hdc )
	{
		::ReleaseDC(g_hWnd, m_hdc);
	}
}

/*virtual*/ void UITabClanInfo::_InitializeAtLoad( i3UIScene* pScene )
{
	UITabClan_Normal::_InitializeAtLoad(pScene);

	{
		i3UIStaticText * pCtrl = (i3UIStaticText *) pScene->FindChildByName("StaticText_PreMatchMember");
		i3UIFont* font	= pCtrl->ObtainUIFont();

		if( nullptr == font )
		{
			return;
		}

		if( nullptr == m_hdc )
			::ReleaseDC(g_hWnd, m_hdc);

		SIZE sizeExt;

		m_hdc = ::GetDC( g_hWnd );

		::SelectObject( m_hdc, font->getFontHandle() );

		::GetTextExtentPoint32A(m_hdc, "가나다라", 8, &sizeExt );
		m_nSubjectWidth = sizeExt.cx;

		::GetTextExtentPoint32A( m_hdc, "..", 2, &sizeExt );
		m_nEllipseWidth= sizeExt.cx;
	}
}
/*virtual*/ void UITabClanInfo::_ClearAtUnload( i3UIScene* pScene )
{
	if( nullptr != m_hdc )
		::ReleaseDC(g_hWnd, m_hdc);

	m_hdc = nullptr;

	UITabClan_Normal::_ClearAtUnload(pScene);
}

void UITabClanInfo::_SetClanInfoNotice() // 공지사항
{
	CLAN_DETAIL_INFO * pClanDetailInfo = nullptr;

	if(isVisitUser() == true)
		pClanDetailInfo = ClanGameContext::i()->getClanDetailInfo();
	else
		pClanDetailInfo = ClanGameContext::i()->getMyClanDetailInfo();

	I3ASSERT( pClanDetailInfo != nullptr);

	LuaState * L = _CallLuaFunction("SetNotice");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, pClanDetailInfo->m_announce);
		_EndLuaFunction(L,1);
	}


	if( GetScene(0) )
	{
		i3UIEditBox* pEdit = static_cast<i3UIEditBox*>(GetScene(0)->FindChildByName("Edit_Notice"));
		pEdit->getScrollBar()->setScrollPos(0);
	}
}

void UITabClanInfo::_SetEliteMember(INT32 idx, const CLAN_MEMBER_BASIC_INFO * pInfo)
{
	LuaState * L = _CallLuaFunction("SetEliteMember");

	if( L != nullptr)
	{
		if(pInfo != nullptr)
		{
			i3Lua::PushInteger(L , idx);
			i3Lua::PushStringUTF16To8(L, pInfo->_nick);
			i3Lua::PushInteger(L , pInfo->_rank);
			i3Lua::PushColor(L ,  GameUI::GetNickNameColor(pInfo->_color));
		}
		else
		{
			i3Lua::PushInteger(L , idx);
			i3Lua::PushStringUTF16To8( L, L"--");
			i3Lua::PushInteger( L, -1);
			i3Lua::PushColor( L, GameGUI::GetColor(GCT_DEFAULT));
		}
	
		_EndLuaFunction(L,7);
	}
}

void UITabClanInfo::_UpdateEliteMember(void) // 클랜 공로자 표시
{
	// 클랜 공로자 표시
	const CLAN_MEMBER_BASIC_INFO * pTopMember[UI_CBMS_MAX];	

	INT32 i;

	for( i = 0; i < UI_CBMS_MAX; ++i)
	{
		pTopMember[i] = nullptr;
	}

	if( ClanGameContext::i()->GetMyClanMemberMaxCount() > 0 )
	{
		CLAN_DETAIL_INFO * pClanDetailInfo = nullptr;
		CLAN_TOP_MEMBER * pClanTopMember = nullptr;

		if(isVisitUser() == true)
		{
			pClanDetailInfo = ClanGameContext::i()->getClanDetailInfo();
			pClanTopMember = ClanGameContext::i()->getClanTopMember();
		}
		else
		{
			pClanDetailInfo = ClanGameContext::i()->getMyClanDetailInfo();
			pClanTopMember = ClanGameContext::i()->getMyClanTopMember();
		}


		for( i = 0 ; i < pClanDetailInfo->m_currentPerson ; i++ )
		{
			const CLAN_MEMBER_BASIC_INFO * pMember = ClanGameContext::i()->getMyClanMember(i);
			if ( pMember == nullptr )
				continue;

				if( pClanTopMember->m_topexp != 0 && pClanTopMember->m_topexp			== pMember->_i64UID )	pTopMember[UI_CBMS_EXP]			= pMember;
				if( pClanTopMember->m_topkill != 0 && pClanTopMember->m_topkill			== pMember->_i64UID )	pTopMember[UI_CBMS_KILL_COUNT]	= pMember;
				if( pClanTopMember->m_tophead != 0 && pClanTopMember->m_tophead			== pMember->_i64UID )	pTopMember[UI_CBMS_HEAD_COUNT]	= pMember;
				if( pClanTopMember->m_topconnect != 0 && pClanTopMember->m_topconnect	== pMember->_i64UID )	pTopMember[UI_CBMS_CONNECTING]	= pMember;
				if( pClanTopMember->m_topwin != 0 && pClanTopMember->m_topwin			== pMember->_i64UID )	pTopMember[UI_CBMS_WIN_COUNT]	= pMember;

		}
	}

	for( i = 0 ; i < UI_CBMS_MAX ; ++i)
	{
		_SetEliteMember(i , pTopMember[i]);
	}

}

void UITabClanInfo::_SetPrevMatchMemeberNick(i3::stack_wstring& out_wstr, const i3::wliteral_range& wNickRng)
{
	
//	char szTemp[NET_NICK_NAME_SIZE] = {0};

//	i3::safe_string_copy(szTemp, pszNick, NET_NICK_NAME_SIZE);
	i3::stack_wstring wstrNickTemp = wNickRng;

	int nLength = wstrNickTemp.size();
	int nResultLen = 0;

	SIZE sizeEffective;
	
	::GetTextExtentExPointW(m_hdc, wstrNickTemp.c_str(), nLength, m_nSubjectWidth, &nResultLen, NULL, &sizeEffective );

	if(nResultLen < nLength)
	{
		// 글자가 많은 경우...Ellipse길이를 빼고 다시 처리한다..
		::GetTextExtentExPointW( m_hdc, wstrNickTemp.c_str(), nLength, m_nSubjectWidth - m_nEllipseWidth, &nResultLen, NULL, &sizeEffective );
		i3::substr(wstrNickTemp, wstrNickTemp, 0, nResultLen);
		i3::generic_string_cat(wstrNickTemp, L"..");
	}

	if(i3::generic_string_size(out_wstr) == 0)
		out_wstr = wstrNickTemp;
	else
		i3::generic_string_cat(out_wstr, wstrNickTemp);
}

void UITabClanInfo::_UpdateClanNews(void) // 클랜 뉴스
{
	CLAN_NEWS * pClanNews = nullptr;
	
	if(isVisitUser() == true)
		pClanNews = ClanGameContext::i()->getClanNews();
	else
		pClanNews = ClanGameContext::i()->getMyClanNews();

	// 가입 승인 대기 인원
	i3::rc_wstring wstrReqPeople;

	if(pClanNews->m_ClanRequestCount > 0)
		i3::sprintf(wstrReqPeople, GAME_RCSTRING("STBL_IDX_CLAN_DECLARE_PERSON"), pClanNews->m_ClanRequestCount );	// %d 명
	else
		wstrReqPeople = GAME_RCSTRING("STR_POPUP_NONE_VALUE");	// 없음

	// 최근 클랜전 결과
	i3::rc_wstring wstrMatchResult;
	
	switch(pClanNews->m_ui8WinLose)
	{
	case 1:
		wstrMatchResult = GAME_RCSTRING("STBL_IDX_CLAN_DRAW");	// 무승부
		break;
	case 2:
		wstrMatchResult = GAME_RCSTRING("STBL_IDX_CLAN_LOSE");	// 패
		break;
	case 3:
		wstrMatchResult = GAME_RCSTRING("STBL_IDX_CLAN_WIN");		// 승
		break;
	default:
		wstrMatchResult = GAME_RCSTRING("STR_POPUP_NONE_VALUE");	// 없음.
		break;		
	}

	// 최근 클랜전 참여 멤버
	i3::stack_wstring wstrPrevMatchMember;

	CLAN_DETAIL_INFO * pClanDetailInfo = ClanGameContext::i()->getMyClanDetailInfo();
	
	for(INT32 i = 0 ; i < CLAN_TEAM_MEMBER_COUNT ; ++i)
	{
		if(pClanNews->m_i64PlayerUID[i] == 0)
			continue;

		if(i3::generic_string_size(wstrPrevMatchMember) > 0)
		{
			i3::generic_string_cat(wstrPrevMatchMember, L", ");
		}
		
		for(INT32 idx = 0 ; idx < pClanDetailInfo->m_currentPerson ; ++idx )
		{
			const CLAN_MEMBER_BASIC_INFO * pMember = ClanGameContext::i()->getMyClanMember(idx);

			if ( pMember == nullptr )
				continue;

			if(pClanNews->m_i64PlayerUID[i] == (T_UID)pMember->_i64UID)
			{
				i3::wliteral_range wstrNick = pMember->_nick;

				_SetPrevMatchMemeberNick(wstrPrevMatchMember, wstrNick);		
			}
		}
	}

	LuaState * L = _CallLuaFunction("SetNewsInfo");

	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrReqPeople);
		i3Lua::PushStringUTF16To8(L, wstrMatchResult);
		i3Lua::PushStringUTF16To8(L, wstrPrevMatchMember);
		_EndLuaFunction(L , 3);
	}
}

void UITabClanInfo::_RequestClanDetailInfo(void)
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();

	if(isVisitUser() == true)
	{
		//pClanCxt->SendClanDetailInfoNEvent(ClanGameContext::i()->getClanDetailInfo()->m_ui32CID, 1);
	}
	else
	{
		// 클랜원인 경우, 자기 자신의 클랜 Idx를 던져줍니다.
		USER_INFO_BASIC Info;
		UserInfoContext::i()->GetMyUserInfoBasic(&Info);

		pClanCxt->SendClanDetailInfoNEvent(Info.m_ui32Clanidx, 0);
	}
}


/*virtual*/ bool UITabClanInfo::OnEntranceStart( void * pArg1, void * pArg2 )
{
	if( UITabClan_Normal::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_SetClanInfoNotice();
	_UpdateEliteMember();
	_UpdateClanNews();

	_RequestClanDetailInfo();

	return true;
}

/*virtual*/ bool UITabClanInfo::OnExitStart( void )
{
	return UITabClan_Normal::OnExitStart();
}

/*virtual*/void UITabClanInfo::OnCreate(i3GameNode * pParent)
{
	UITabClan_Normal::OnCreate(pParent);
	
	AddScene("Scene/Clan/PointBlankRe_Clan_Info.i3UIs", UITabClanInfo_Glue);
}

/*virtual*/ void UITabClanInfo::OnUpdate( REAL32 rDeltaSeconds)
{
	UITabClan_Normal::OnUpdate( rDeltaSeconds);

	//	클랜원 정보 업데이트
	if( g_pFramework->GetClanContext() != nullptr)
		g_pFramework->GetClanContext()->UpdateClanMember(rDeltaSeconds);
}

/*virtual*/ void UITabClanInfo::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UITabClan_Normal::ProcessGameEvent(event, arg, UserData);

	switch(event)
	{
	case EVENT_CLAN_DETAIL_INFO :
		{
			_SetClanInfoNotice();
			_UpdateEliteMember();
			_UpdateClanNews();
		}
		break;
	case EVENT_CLAN_MEMBER_LIST :
		{
			_UpdateEliteMember();
			_UpdateClanNews();
		}
		break;	
	}
}

void UITabClanInfo::OnLoadAllScenes()
{
	UITabClan_Normal::OnLoadAllScenes();


	if (g_pEnvSet->m_nUIVersion != UFV_1_5 && 
		GetMainFrame()->GetPopup(UPW_CLAN_MEMBER)->isSliding() )
	{
		m_slide.AddFrameWnds(GetScene(0), UISlide::R_TO_L);
		m_slide.SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_slide.SetTargetTime(true, 1.f / UI_SLIDEOUT_SLOP);
	}
}