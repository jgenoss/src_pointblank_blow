#include "pch.h"
#include "UIPhaseNonClanMember.h"
#include "UIMainFrame.h"
#include "GlobalVariables.h"
#include "ClanDef.h"
#include "ClanMark.h"
#include "UIClanFunction.h"

#include "UIUtil.h"
#include "TextSet.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "ClanGameContext.h"
#include "UserInfoContext.h"
#include "UISlide.h"



///////////////////////////////////////////////////////////////////////////////

extern "C"
{
	int UIPhaseNonClanMember_Close( LuaState * L)
	{
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->OnEscapeKey();
		return 0;
	}

	int UIPhaseNonClanMember_JoinClan( LuaState * L)
	{
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->JoinClan();
		return 0;
	}

	int UIPhaseNonClanMember_CreateClan( LuaState * L)
	{
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->CreateClan();
		return 0;
	}

	int UIPhaseNonClanMember_SearchClan( LuaState * L)
	{
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->SearchClan();
		return 0;
	}

	int UIPhaseNonClanMember_ClickClanSlot( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->ClickClanSlot(i32Slot);
		return 0;
	}

	int UIPhaseNonClanMember_VisitClan( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->VisitClan(i32Slot);
		return 0;
	}

	int UIPhaseNonClanMember_ClanListScroll( LuaState * L)
	{
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->ClanListScroll();
		return 0;
	}
	
	/*
	int UIPhaseNonClanMember_ClanListWheel( LuaState * L)
	{
		INT32 zDelta = i3Lua::GetIntegerArg( L, 1);
		UIPhaseNonClanMember * pThis = static_cast<UIPhaseNonClanMember*>(GameUI::GetPopup(UPW_CLAN_NON_MEMBER));
		if( pThis != nullptr)
			pThis->ClanListWheel(zDelta);
		return 0;
	}
	*/
}

LuaFuncReg	UIPhaseNonClanMember_Glue[] =
{
	{ "ManualClose",	UIPhaseNonClanMember_Close},
	{ "JoinClan",		UIPhaseNonClanMember_JoinClan},
	{ "CreateClan",		UIPhaseNonClanMember_CreateClan},
	{ "SearchClan",		UIPhaseNonClanMember_SearchClan},
	{ "ClickClanSlot",	UIPhaseNonClanMember_ClickClanSlot},
	{ "VisitClan",		UIPhaseNonClanMember_VisitClan},
	{ "ClanListScroll",	UIPhaseNonClanMember_ClanListScroll},
	/*{ "ClanListWheel",	UIPhaseNonClanMember_ClanListWheel},*/
	{ nullptr,				nullptr }
};

I3_CLASS_INSTANCE( UIPhaseNonClanMember);//, UIPhaseClanBase);

UIPhaseNonClanMember::UIPhaseNonClanMember()
{	
	i3mem::FillZero(m_ClanSlot, sizeof(UIClanSlot) * CLAN_SLOT_COUNT);

	m_pClanListScroll = nullptr;
	m_pDetailClanMark = nullptr;

	m_i32CurSelClanIdx = 0;

	RegisterGameEventFunc( EVENT_M_CLAN_MEMBER_INFO,			&UIPhaseNonClanMember::ReceiveGameEvent_MemberInfo);
	RegisterGameEventFunc( EVENT_CLAN_LEAVE,					&UIPhaseNonClanMember::ReceiveGameEvent_Leave);
	RegisterGameEventFunc( EVENT_CLAN_CREATION_INFO,			&UIPhaseNonClanMember::ReceiveGameEvent_CreationInfo);
	RegisterGameEventFunc( EVENT_CLAN_LIST_DETAIL_INFO,			&UIPhaseNonClanMember::ReceiveGameEvent_ClanList_DetailInfo);
	RegisterGameEventFunc( EVENT_CLAN_DETAIL_INFO,				&UIPhaseNonClanMember::ReceiveGameEvent_DetailInfo);
}

UIPhaseNonClanMember::~UIPhaseNonClanMember()
{
	m_pClanListScroll = nullptr;
	m_pDetailClanMark = nullptr;
}

/////// protected Member Func //////////

/*virtual*/ void UIPhaseNonClanMember::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPhaseClanBase::_InitializeAtLoad(pScene);

	// Init Clan Slot
	{
		char szWndName[ MAX_NAME ];

		for( int i = 0; i < CLAN_SLOT_COUNT; ++i )
		{
			i3::snprintf( szWndName, MAX_NAME, "FrameWnd_ClanSlot%d", i );
			i3UIFrameWnd* pWnd = (i3UIFrameWnd*) pScene->FindChildByName( szWndName );

			bool bRes = m_ClanSlot[i].Init( pWnd, i);
			I3ASSERT(bRes);
		}
	}

	// Scroll Bar
	{
		m_pClanListScroll = (i3UIScrollBar *) pScene->FindChildByName("Scroll_ClanList");
		m_pClanListScroll->setTickSize( 1 );
	}

	// Detail ClanMark
	m_pDetailClanMark = (i3UIImageBoxEx *) pScene->FindChildByName("ImgBox_ClanMark11");

	// Search Edit Box
	m_pSearchEdit = (i3UIEditBox *) pScene->FindChildByName("Edit_SearchClan");
}

/*virtual*/ void UIPhaseNonClanMember::_ClearAtUnload(i3UIScene * pScene)
{
	for( INT32 i = 0; i < CLAN_SLOT_COUNT; ++i )
	{
		m_ClanSlot[i].Init( nullptr, i);
	}

	m_pClanListScroll = nullptr;
	m_pDetailClanMark = nullptr;

	UIPhaseClanBase::_ClearAtUnload(pScene);
}

/*virtual*/ void UIPhaseNonClanMember::OnLoadAllScenes(void)
{
	UIPhaseClanBase::OnLoadAllScenes();

	for(int i = 0 ; i < CLAN_SLOT_COUNT ; ++i)
	{
		m_ClanSlot[i].SetSlotData(nullptr, -1);
	}

	_InitScrollBar();

	if (g_pEnvSet->m_nUIVersion != UFV_1_5)
	{
		m_slide.AddFrameWnds(GetScene(0), UISlide::R_TO_L);
		m_slide.SetTargetTime(false, 1.f / UI_SLIDEOUT_SLOP);
		m_slide.SetTargetTime(true, 1.f / UI_SLIDEOUT_SLOP);
	}
}

void UIPhaseNonClanMember::_InitScrollBar(void)
{
	if( m_pClanListScroll == nullptr )
		return;

	INT32 i32ClanCnt = g_pFramework->GetClanContext()->GetClanListCount();

	if(i32ClanCnt > CLAN_SLOT_COUNT )
	{
		// 홀수인 경우가 있기 때문에 i32MaxScrollPos = Count / 2 + Count % 2
		INT32 i32MaxScrollPos = (INT32)((i32ClanCnt / 2) + (i32ClanCnt % 2)) ;
		m_pClanListScroll->EnableCtrl( true );
		m_pClanListScroll->setPageSize( 4 );
		m_pClanListScroll->setScrollRange( 0, i32MaxScrollPos);
	}
	else
	{
		m_pClanListScroll->setScrollPos( 0 );
		m_pClanListScroll->EnableCtrl( false);		
	}
}

void	UIPhaseNonClanMember::_UpdateClanList(void)
{
	INT32 i32ClanCnt = g_pFramework->GetClanContext()->GetClanListCount();
	INT32 idx = 0;

	if(m_pClanListScroll == nullptr)
		return;

	for(int i = 0 ; i < CLAN_SLOT_COUNT ; i++)
	{
		idx = (m_pClanListScroll->getScrollPos() * 2) + i;

		if(idx < i32ClanCnt)
		{
			SORT_CLAN_INFO * pSortClanInfo = g_pFramework->GetClanContext()->GetClanList(idx);

			m_ClanSlot[i].SetSlotData(pSortClanInfo, m_i32CurSelClanIdx);
		}
		else
		{
			m_ClanSlot[i].SetSlotData(nullptr, -1);
		}
	}

	_UpdateSlotPushFlag();
}

void UIPhaseNonClanMember::_UpdateClanDetailInfo(void)
{
	LuaState * L = _CallLuaFunction("ClanDetailInfo"); 

	if(m_i32CurSelClanIdx > 0)
	{
		// 클랜이 선택되어 있을땐 정보 표시
		CLAN_DETAIL_INFO * pInfo = ClanGameContext::i()->getClanDetailInfo();

		///////// 표시할 정보 설정합니다. /////////////////

		// 클랜 등급
		i3::wstring wstrClanLevel;
		GetClanFullName(wstrClanLevel, pInfo->m_exp, pInfo->m_currentPerson);

		// 클랜 경험치 Rate
		i3::wstring wstrExpRate;		INT32 i32ExpRate = 0;
		_SetClanExpRate(wstrExpRate, i32ExpRate ,pInfo->m_exp);

		// 클랜 포인트 & 클랜 순위
		i3::stack_wstring wstrClanPoint;
		i3::sprintf( wstrClanPoint, L"%d (%d%s)", 
			(INT32) (pInfo->GetClanPoint()), pInfo->m_clanRank, GAME_RCSTRING("STBL_IDX_CLAN_LADDER") );

		// 클랜 멤버 수
		i3::stack_wstring wstrPeople;
		i3::sprintf( wstrPeople, L"%d/%d", pInfo->m_currentPerson, pInfo->m_maxPerson);

		i3::wstring wstrRecord;
		i3::wstring wstrRecordRate;
		_SetClanTotalRecord(wstrRecord, wstrRecordRate, pInfo);

		// 클랜 마크 설정
		_SetClanMark(m_pDetailClanMark, pInfo->m_mark);

		/////////////////////////////////////////////////////
		i3Lua::PushStringUTF16To8( L, pInfo->m_name);							// 클랜명		
		i3Lua::PushInteger( L,  pInfo->m_expRank);								// 클랜 등급
		i3Lua::PushStringUTF16To8( L, wstrClanLevel);								// 클랜 등급 명칭 - 없음
		i3Lua::PushNumber( L, i32ExpRate * 0.01f);								// 클랜 경험치 Value
		i3Lua::PushStringUTF16To8( L, wstrExpRate);									// 클랜 경험치 Rate
		i3Lua::PushStringUTF16To8( L, wstrClanPoint);								// 클랜 포인트 & 순위
		i3Lua::PushStringUTF16To8(L, pInfo->m_masterNick);
		i3Lua::PushStringUTF16To8( L, wstrPeople);									// 클랜 인원
		i3Lua::PushStringUTF16To8( L, wstrRecord);			// 클랜 전적
		i3Lua::PushStringUTF16To8( L, wstrRecordRate);	// 클랜 승률
	}
	else
	{
		_SetClanMark(m_pDetailClanMark, CLAN_MARK_FLAG_NONE);

		// 클랜이 선택이 안되있으면 디폴트 정보
		const i3::rc_wstring& game_str = GAME_RCSTRING("STBL_IDX_CLAN_UNDECIDED");

		i3Lua::PushStringUTF16To8( L, game_str.c_str(), game_str.size()  );	// 클랜명
		i3Lua::PushInteger( L, -1);												// 클랜 등급
		i3Lua::PushStringUTF16To8( L, L"");										// 클랜 등급 명칭 - 없음
		i3Lua::PushInteger( L, 0);												// 클랜 경험치 Value
		i3Lua::PushStringUTF16To8( L, L"");										// 클랜 경험치 text
		i3Lua::PushStringUTF16To8( L, L"-");									// 클랜 포인트
		i3Lua::PushStringUTF16To8( L, L"-");									// 마스터닉네임
		i3Lua::PushStringUTF16To8( L, L"-");									// 클랜 인원
		i3Lua::PushStringUTF16To8( L, L"-");									// 클랜 전적
		i3Lua::PushStringUTF16To8( L, L"-");									// 클랜 승률

	}
	_EndLuaFunction(L , 10);
}

void UIPhaseNonClanMember::_UpdateClanIntroFlow(REAL32 rDeltaSeconds)
{
	for(INT32 i = 0 ; i < CLAN_SLOT_COUNT ; ++i)
	{
		m_ClanSlot[i].OnUpdate(rDeltaSeconds);
	}
}

void UIPhaseNonClanMember::_UpdateSlotPushFlag(void)
{
	if( m_i32CurSelClanIdx > 0)
	{
		INT32 slot_idx = 0;

		for( slot_idx = 0 ; slot_idx < CLAN_SLOT_COUNT ; ++slot_idx )
		{
			if( m_ClanSlot[slot_idx].isSameClanIdx(m_i32CurSelClanIdx) )
				break;
		}

		LuaState * L = _CallLuaFunction("SetSlotPushFlag");
		i3Lua::PushInteger(L, slot_idx);
		_EndLuaFunction(L, 1);
	}
	else
	{
		LuaState * L = _CallLuaFunction("ResetSlotPushFlag");
		_EndLuaFunction(L, 0);
	}
}



/////// public Member Func //////////

/*virtual*/ void UIPhaseNonClanMember::OnCreate(i3GameNode * pParent)
{
	UIPhaseClanBase::OnCreate(pParent);

	AddScene("Scene/Clan/PointBlankRe_Clan_List.i3UIs", UIPhaseNonClanMember_Glue, true, false);
}

/*virtual*/ bool UIPhaseNonClanMember::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if(UIPhaseClanBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	m_i32CurSelClanIdx = 0;
	_UpdateClanDetailInfo();

	return true;
}

/*virtual*/ void UIPhaseNonClanMember::OnEntranceEnd( void)
{
	g_pFramework->GetClanContext()->EnterClanList(true);
	g_pFramework->GetClanContext()->InitSortClanList();

	UIPhaseClanBase::OnEntranceEnd();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}

	m_pSearchEdit->SetText(L"");

	// 가입 버튼 비활성화 상태로 시작.

	LuaState* L = _CallLuaFunction("ActivateJoinButton");
	i3Lua::PushBoolean(L, false);
	_EndLuaFunction(L, 1);

	if(m_pClanListScroll)
	{
		i3UIManager* pMgr = i3UI::getManager();
		if( pMgr )
			pMgr->SetFocusControl( m_pClanListScroll );
	}
}

void UIPhaseNonClanMember::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPhaseClanBase::OnUpdate( rDeltaSeconds);

	I3ASSERT( g_pFramework->GetClanContext() != nullptr);

	//  Client 클랜리스트 정보
	if( g_pFramework->GetClanContext()->IsUpdatedClanList())
	{
		_InitScrollBar();
		_UpdateClanList();
		g_pFramework->GetClanContext()->SetUpdatedClanList( false);
	}

	_UpdateClanIntroFlow(rDeltaSeconds);
	
	// 클랜 리스트를 업데이트한다.
	g_pFramework->GetClanContext()->UpdateClanList(rDeltaSeconds);
}


bool UIPhaseNonClanMember::OnKeyInput(i3InputDeviceManager * pInputMng)
{
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	
	if( pKeyboard->GetStrokeState( I3I_KEY_ENTER) )
	{
		OnKey_Enter();
		PlaySound( UISND_ITEM_SELECTING);
	}
	
	if( GameUI::IsMouseStroked(I3I_MOUSE_WHEELUP) == true )
	{
		ClanListWheel( (INT32)pInputMng->GetMouse()->GetDeltaZ() );
	}
	else if( GameUI::IsMouseStroked(I3I_MOUSE_WHEELDOWN) == true )
	{
		ClanListWheel( (INT32)pInputMng->GetMouse()->GetDeltaZ() );
	}

	return UIPopupBase::OnKeyInput(pInputMng);
}

bool UIPhaseNonClanMember::OnKey_Enter( void)
{

	SearchClan();	
	return true;

}

void UIPhaseNonClanMember::Leave( void)
{
	if (g_pEnvSet->IsBaseOnUIVersion_1_0() || g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		GetMainFrame()->ExclusedEnterSubPhase(USP_TEMP);
	}
	else
	{
		UIPhaseClanBase::Leave();

		CClanContext* pClanContext = g_pFramework->GetClanContext();
		if( pClanContext->IsEnter())
		{
			pClanContext->LeaveServer();
		}
	}
}

/*vurtial*/ bool UIPhaseNonClanMember::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButton*>(pCtrl) && 
		i3::generic_is_iequal(pCtrl->GetName() , "Btn_Close")  )
	{
		return PlaySound(UISND_ITEM_CLICK_CANCEL);
	}

	return UIPhaseClanBase::OnPlayClickSound(pCtrl);
}

void UIPhaseNonClanMember::ClickClanSlot( INT32 i32Slot)
{
	// 실제 클랜 리스트 상 인덱스입니다.
	INT32 i32RealSlot = (m_pClanListScroll->getScrollPos() * 2) + i32Slot;

	// 실제 인덱스가 클랜 수보다 크면 리턴
	if(i32RealSlot >= g_pFramework->GetClanContext()->GetClanListCount())
		return;

	SORT_CLAN_INFO * pSortClanInfo = g_pFramework->GetClanContext()->GetClanList( i32RealSlot);
	INT32 i32ClanIdx = pSortClanInfo->_info._idx;

	// 이미 선택된 클랜을 눌렀다면 처리 안함.
	if(i32ClanIdx == m_i32CurSelClanIdx)
		return;

	for(int i = 0 ; i < CLAN_SLOT_COUNT ; i++)
	{
		m_ClanSlot[i].SelectSlot(-1);
	}

	m_i32CurSelClanIdx = i32ClanIdx;
	m_ClanSlot[i32Slot].SelectSlot(m_i32CurSelClanIdx);

	PACKET_CS_NEW_CLAN_LIST_DETAIL_INFO_REQ ReqPacket;

	ReqPacket.m_ui32CUID = m_i32CurSelClanIdx;
	ReqPacket.m_ui8ClanType = 1;	// 0 은 MyDetail 정보 1은 Detail 정보

	GameEventSender::i()->SetEvent( EVENT_CLAN_LIST_DETAIL_INFO, &ReqPacket);

	// 가입 버튼 활성화.
	{
		LuaState * L = _CallLuaFunction("ActivateJoinButton");
		i3Lua::PushBoolean(L, true);
		_EndLuaFunction(L, 1);
	}

	_UpdateSlotPushFlag();
}

void UIPhaseNonClanMember::ClanListScroll( void)
{
	_UpdateClanList();
}

void UIPhaseNonClanMember::ClanListWheel(INT32 zDelta)
{
	INT32 i32ClanCount = g_pFramework->GetClanContext()->GetClanListCount();

	if( i32ClanCount > CLAN_SLOT_COUNT )
	{
		if( m_pClanListScroll )
		{
			int nPos = m_pClanListScroll->getScrollPos() - (int)(zDelta / 120);
			nPos = MINMAX( 0, nPos, i32ClanCount - CLAN_SLOT_COUNT );

			m_pClanListScroll->setScrollPos( nPos );
		}

		_UpdateClanList();
	}	
}

void UIPhaseNonClanMember::SearchClan( void)
{

	i3::wliteral_range strKeyword = m_pSearchEdit->getTextRange();

	if(g_pFramework->GetClanContext()->SearchClanListView( strKeyword) )
	{
		m_i32CurSelClanIdx = 0;
		_InitScrollBar();
		_UpdateClanList();
		_UpdateClanDetailInfo();
	}

	LuaState* L = _CallLuaFunction("ActivateJoinButton");
	i3Lua::PushBoolean(L, false);
	_EndLuaFunction(L, 1);
}

// 클랜 가입
void UIPhaseNonClanMember::JoinClan( void)
{
	if(g_pFramework->GetClanContext()->IsEnter() == false)
		return;

	if( m_i32CurSelClanIdx <= 0)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_CHOOSE_CLAN"));		// 가입하려는 클랜을 선택해주세요
		return;
	}

	// 클랜 가입이 가능한 상태일때, 가입 조건이 맞는지 서버에 요청
	if(_CheckClanRequsetEnable() == true)
	{
		//	클랜에 가입하기 전에 가입조건에 맞는지 먼저 서버에 요청합니다.
		g_pFramework->GetClanContext()->SendRequestJoining( m_i32CurSelClanIdx);
	}
}


// 클랜 생성
void UIPhaseNonClanMember::CreateClan( void)
{
	if(g_pFramework->GetClanContext()->IsEnter() == false)
		return;

	if(_CheckClanRequsetEnable() == true)
	{
		if ( ! g_pFramework->GetClanContext()->HasClanCreationInfo() )
		{
			GameEventSender::i()->SetEvent(EVENT_CLAN_CREATION_INFO);
		}
	}	
}

void UIPhaseNonClanMember::VisitClan( INT32 i32Slot)
{
	CClanContext * pClanCxt = g_pFramework->GetClanContext();

	// 실제 클랜 리스트 상 인덱스입니다.
	INT32 i32RealSlot = (m_pClanListScroll->getScrollPos() * 2) + i32Slot;

	// 실제 인덱스가 클랜 수보다 크면 리턴
	if(i32RealSlot >= pClanCxt->GetClanListCount())
		return;

	SORT_CLAN_INFO * pSortClanInfo = pClanCxt->GetClanList( i32RealSlot);

	pClanCxt->SendClanDetailInfoNEvent(pSortClanInfo->_info._idx, 1);
	pClanCxt->SetClanVisit(true);
}

void UIPhaseNonClanMember::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert( i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPhaseNonClanMember::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPhaseClanBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)( arg, UserData);
	}
}

/*virtual*/ bool UIPhaseNonClanMember::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pNotify = (I3UI_CONTROL_NOTIFY*) param2;

		i3UIButton * pButton = i3::same_cast<i3UIButton*>(pNotify->m_pCtrl);

		if( pButton != nullptr)
		{
			i3UIStaticText * pTextCtrl = i3::same_cast<i3UIStaticText*>(pButton->FindChildByType(i3UIStaticText::static_meta()));
			if( pTextCtrl != nullptr)
			{
				COLOR color;
				if( g_pEnvSet->IsBaseOnUIVersion_1_0() == false )
				{
					if( pButton->isSelected() )					i3Color::Set(&color, (UINT8)0, 51, 96, 255);
					if( pButton->isState( I3UI_STATE_ONMOUSE) )	i3Color::Set(&color, (UINT8)6, 82, 149, 255);
					else										i3Color::Set(&color, (UINT8)184, 216, 255, 255);
				}
				else
				{
					i3Color::Set(&color, (UINT8)191, 191, 191, 255);
				}

				pTextCtrl->SetTextColor(&color);
			}
		}
	}

	return UIBase::OnEvent( event, pObj, param2, code);
}

void UIPhaseNonClanMember::ReceiveGameEvent_MemberInfo( INT32 arg, const i3::user_data&)
{
	g_pFramework->GetClanContext()->ResetMatchResultContext();
	GetMainFrame()->ClosePopup(UPW_CLAN_NON_MEMBER);
	GetMainFrame()->ClosePopup(UPW_MAILBOX);
	GetMainFrame()->OpenPopup(UPW_CLAN_MEMBER);
	GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_GUI_MESSAGER_CLAN_INVITE_ACCEPT")); // 가입 승인
}

void UIPhaseNonClanMember::ReceiveGameEvent_Leave( INT32 arg, const i3::user_data&)
{
	OnLeave();
}

void UIPhaseNonClanMember::ReceiveGameEvent_CreationInfo( INT32 arg, const i3::user_data&)
{
	UINT32 MyPoint = UserInfoContext::i()->GetMyPoint();
	UINT32 wRank = UserInfoContext::i()->GetRealRank();

	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		return;
	}

	if( MyPoint > RequirementPoint && wRank >= RequirementRank)
	{
		// 클랜 생성화면 호출
		GetMainFrame()->OpenPopup(UPW_CLAN_CREATE);
		return;
	}

	if(MyPoint < RequirementPoint && wRank < RequirementRank)
	{
		//	"죄송합니다. 클랜 생성에 필요한 계급+에\n도달하지 못하였거나 포인트가 부족합니다.\n다음에 다시 시도해 주세요."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANNOT_CREATING_CLAN"));
	}
	else if( MyPoint < RequirementPoint)
	{ 
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_NO_POINT"));		//	"포인트가 부족합니다."
	}
	else if( wRank < RequirementRank)
	{
#if defined(LOCALE_TAIWAN)
		i3::stack_wstring wstr_temp; 
		i3::rc_wstring wstr_temp1;

		i3::sprintf(wstr_temp, L"{col:253,167,0,255}%s{/col}", GetCharaRank( RequirementRank));
		i3::sprintf(wstr_temp1, GAME_RCSTRING("STBL_IDX_CLAN_NEED_HIGH_RANK"), wstr_temp);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, wstr_temp1);
#else

		i3::rc_wstring str = i3::format_string(L"{col:253,167,0,255}%s{/col} %s", 
			GetCharaRank( RequirementRank), GAME_RCSTRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK, str);

#endif
	}
	g_pFramework->GetClanContext()->ResetClanCreationInfo();
}

void UIPhaseNonClanMember::ReceiveGameEvent_ClanList_DetailInfo( INT32 arg, const i3::user_data&)
{
	if(EV_SUCCESSED(arg))
		_UpdateClanDetailInfo();
}

void UIPhaseNonClanMember::ReceiveGameEvent_DetailInfo( INT32 arg, const i3::user_data&)
{
	if(EV_SUCCESSED(arg))
	{
		if(GetMainFrame()->IsOpenedPopup(UPW_CLAN_MEMBER) == false)
		{
			g_pFramework->GetClanContext()->ResetMatchResultContext();
			GetMainFrame()->ClosePopup(UPW_CLAN_NON_MEMBER);
			GetMainFrame()->OpenPopup(UPW_CLAN_MEMBER);
		}
	}
}
