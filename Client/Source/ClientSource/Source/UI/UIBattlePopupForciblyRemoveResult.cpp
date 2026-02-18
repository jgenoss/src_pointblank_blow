#include "pch.h"
#include "UIBattlePopupForciblyRemove.h"

#include "UIBattledef.h"

#include "UIBattleFrame.h"
#include "UIHUDForciblyRemove.h"
#include "UIUtil.h"

//#include "StageBattle.h"
#include "CrosshairMgr.h"


static UIBattlePopupForciblyRemove * GetThis( void)
{
#if 잠시_사라져라
	UIBattleFrame * pFrame = UIBattleFrame::i();
	if( pFrame != NULL)
		return (UIBattlePopupForciblyRemove*) pFrame->GetPopup( UBP_FORCIBLYREMOVE);
#endif
	return NULL;
}


UIBattlePopupForciblyRemove* UIBattlePopupForciblyRemove::instance()
{
	return GetThis();
}

//--------------------------------------------------------------------------//
//								Lua Glue Function							//
//--------------------------------------------------------------------------//
extern "C" {
	int UIBattlePopupForciblyRemove_SelectUser( LuaState * L)
	{	
		INT32 slot = i3Lua::GetIntegerArg( L, 1);

		UIBattlePopupForciblyRemove * pThis = GetThis();
		if( pThis != NULL)
		{
			i3Lua::PushBoolean(L, pThis->SelectUser( slot));
			return 1;
		}

		return 0;
	}

	int UIBattlePopupForciblyRemove_SelectReason( LuaState * L)
	{
		INT32 reason = i3Lua::GetIntegerArg( L, 1);

		UIBattlePopupForciblyRemove * pThis = GetThis();
		if( pThis != NULL)
			pThis->SelectReason( static_cast<FORCEREMOVE_CAUSE>(reason));
		return 0;
	}
}

LuaFuncReg UIBattlePopupForciblyRemove_Glue[] = {
	{"SelectUser",					UIBattlePopupForciblyRemove_SelectUser	},
	{"SelectReason",				UIBattlePopupForciblyRemove_SelectReason	},
	{NULL,							NULL}
};

//--------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIBattlePopupForciblyRemove);//, UIBattlePopupBase);

UIBattlePopupForciblyRemove::UIBattlePopupForciblyRemove() : m_bPrev_CrossHairState(TRUE)
{
	m_SelectedUserIdx = -1;
	m_SelectedListboxIdx = -1;
	m_Reason = FORCEREMOVE_NONE;
}

UIBattlePopupForciblyRemove::~UIBattlePopupForciblyRemove()
{
	
}

void	UIBattlePopupForciblyRemove::_ClearUserNameList()
{
	m_UserNameList.clear();
}

void UIBattlePopupForciblyRemove::_Init( void)
{
	m_SelectedListboxIdx = -1;
	m_SelectedUserIdx = -1;

	m_Reason = FORCEREMOVE_CURSE;

	m_UserList.clear();
	_ClearUserNameList();

	int odd_even = g_pGameContext->getMainSlotIdx() % 2;
	for(INT32 i = 0; i < SLOT_MAX_COUNT; i++)
	{
		const char*  szNick = g_pGameContext->getNickForSlot(i);
		if( szNick == 0 ) continue;

		SLOT_INFO* pInfo = g_pGameContext->getSlotInfo(i);
		if( pInfo == 0 ) continue;

		bool add_user = false;
		if( pInfo->_State == SLOT_STATE_BATTLE && pInfo->_Rank != RANK_GAMEMASTER )
		{
			if( g_pGameContext->getMainSlotIdx() != i ) //본인을 제외한 모든 사람 추가
			{
				add_user = true;
				if( g_pGameContext->IsStageMaskTraining() && odd_even != i  % 2)	//챌린지 모드는 본인과 다른 팀은 AI 이다.
					add_user = false;
			}
		}

		if( add_user )
		{
			m_UserList.push_back(i);
			m_UserNameList.push_back(szNick);
		}
	}

	LuaState * L = 0;

	L = _CallLuaFunction( "SetSlotCount");
	i3Lua::PushInteger( L, (INT32)m_UserList.size());
	_EndLuaFunction( L, 1);

	for( size_t i = 0; i < m_UserList.size(); i++)
	{
		L = _CallLuaFunction( "SetSlotInfo");
		i3Lua::PushInteger( L, i);
		i3Lua::PushA2UTF8String( L,  m_UserNameList[i].c_str());
		_EndLuaFunction( L, 2);
	}
	
	char szComboList[ MAX_STRING_COUNT] = "";
	i3String::Format( szComboList, sizeof( szComboList), "%s;%s;%s;%s",
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_NOMANNER"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ILLEGALPROGRAM"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ABUSE"),
		GAME_STRING("STR_POPUP_FORCIBLYREMOVE_REASON_ETC") );

	L = _CallLuaFunction( "SetComboList");
	i3Lua::PushA2UTF8String( L, szComboList);
	_EndLuaFunction( L, 1);
}

/*virtual*/ void UIBattlePopupForciblyRemove::OnCreate( i3GameNode * pParent)
{
	UIBattlePopupBase::OnCreate( pParent);

	AddScene( "UIRe/PBRe_ForceExit.i3UIe", UIBattlePopupForciblyRemove_Glue, true, false);
}

/*virtual*/ bool UIBattlePopupForciblyRemove::OnKey_Escape( void)
{
#if 잠시_사라져라
	UIBattleFrame * pFrame = UIBattleFrame::i();
	I3ASSERT( pFrame);

	if( isEnable())
	{
		pFrame->TogglePopup( UBP_FORCIBLYREMOVE);
		return true;
	}
#endif

	return false;
}


bool UIBattlePopupForciblyRemove::OnEntranceStart( void * pArg1 , void * pArg2 )
{
	if( UIBattlePopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	// 이곳 대신 베이스클래스나 UIBattleFrame토글에서 처리도 가능하긴 한데.11.05.26.수빈.
	if(g_pFramework == NULL)	return false;

	m_bPrev_CrossHairState = CrosshairMgr::i()->isEnableCrossHair();
	CrosshairMgr::i()->enableCrossHair(FALSE); 

	_Init();

	return true;
}

void	 UIBattlePopupForciblyRemove::OnExitEnd( void)
{
	if(g_pFramework == NULL)
		return;

	CrosshairMgr::i()->enableCrossHair(m_bPrev_CrossHairState != FALSE);
	UIBattlePopupBase::OnExitEnd();
}


bool UIBattlePopupForciblyRemove::SelectUser( INT32 idx)
{
//	I3_BOUNDCHK( idx, m_UserList.GetCount() );
	if (idx >= (INT32)m_UserList.size() )
	{
		m_SelectedUserIdx = -1;
		m_SelectedListboxIdx = -1;
		return false;
	}

	m_SelectedListboxIdx = idx;
	m_SelectedUserIdx = m_UserList[ idx];
	I3_BOUNDCHK( m_SelectedUserIdx, SLOT_MAX_COUNT);
	return true;
	
}

void UIBattlePopupForciblyRemove::SelectReason( FORCEREMOVE_CAUSE cause)
{
	I3_BOUNDCHK( cause, FORCEREMOVE_MAX);

	m_Reason = cause;
}

bool UIBattlePopupForciblyRemove::OnOKButton( void)
{
	
	// 제일 좋은 처리는 아무것도 셀렉션하지 않을때 OK버튼을 disable시키고, 셀렉션이 들어오면 enable시키며,
	// 셀렉션이 디셀렉트될때 다시 OK버튼을 disable시키는 방식이 좋아보입니다....(11.06.02.수빈)
	// (이런 부분을 깔끔하게 처리하려면, 해당UI클래스에 이벤트함수 2개를 만드는 것이 좋아보임)

	// 그 외에 주의사항으로, 이 클래스 진행중에 강퇴유저가 나가고, 다른 유저가 그 슬롯을 차지한 경우 처리가 있습니다. (아직 미처리)
	// 이 경우, 실시간으로 목록을 갱신하는 부담을 가질수도 있고, OK처리 순간에 강퇴자 슬롯 변경시
	// 닉네임 비교하는 부담이 있을수 있어 고민중입니다...(후자쪽이 더 쉽다..)

// 우선은, 직접 셀렉션 하지 않는 이상, 강퇴유저를 선택하지 않은 것으로 간주하고, 아무것도 하지 않습니다.(11.06.02.수빈)
	if (m_SelectedUserIdx == -1 || m_SelectedListboxIdx == -1) 
	{
		// 11.06.02.수빈.todo
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_MSG_FORCIBLYREMOVE_SELECT_NOUSER"), GAME_STRING("STR_HUD_FORCIBLYREMOVE_SUBJECT") );
		return true;		// 그냥 닫아버린다... 
	}

	// 서버에 강제퇴장을 신청한다.

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( g_pConfig->m_bNetworking == FALSE)
	{
#if 잠시_사라져라
		UIBattleFrame::i()->EnableHUD( UIHUD_FORCIBLYREMOVE, true);
#endif
		return true;
	}
#endif

	// 셀렉션된 인덱스 이름과 강퇴자 닉네임이 같은지 비교해야합니다...	(그동안 슬롯이 비거나, 다른 사람이 채워질수 있다.)
	const char* szNick_From_Slot = g_pGameContext->getNickForSlot( m_SelectedUserIdx );
	const i3::rc_string& szNick_From_Listbox = m_UserNameList[ m_SelectedListboxIdx];

	if ( szNick_From_Listbox != szNick_From_Slot )
	{
		char strMsg[512];
		StringCchPrintf( strMsg, 512, 
			GAME_STRING("STR_MSG_FORCIBLYREMOVE_EARLY_CANCEL_NOUSER"), szNick_From_Listbox);
		GameUI::MsgBox( MSG_TYPE_GAME_OK, strMsg, GAME_STRING("STR_HUD_FORCIBLYREMOVE_SUBJECT") );
		return true;		// 그냥 닫아버린다... 
	}

	g_pGameContext->SetEventOnce( EVENT_KICK_VOTE_START, &m_SelectedUserIdx, &m_Reason);

	// 성공 실패 여부와 관계없이 제안자의 강퇴자 슬롯을 HUD에 넘깁니다...
	// 제안자의 경우, 패킷으로부터 닉네임과 사유 정보를 못얻습니다.
	// 이 창은 비활성될 것이므로, 이쪽 멤버변수값을 HUD쪽 멤버변수로 전달해서 넘깁니다.
//	UIHUDForciblyRemove* pHUD = static_cast<UIHUDForciblyRemove*>(UIBattleFrame::i()->GetHUD(UIHUD_FORCIBLYREMOVE));
//	pHUD->SetInfo_BySuggest( szNick_From_Slot, m_Reason);

	return true;
}

bool	UIBattlePopupForciblyRemove::OnCloseButton()
{
	return UIBattlePopupBase::OnCloseButton();
}


