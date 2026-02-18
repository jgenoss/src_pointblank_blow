#include "pch.h"
#include "UIMainFrame.h"
#include "UIPopupClanNameSearch.h"

#include "UIUtil.h"
#include "i3Base/string/ext/utf8_to_utf16.h"

I3_CLASS_INSTANCE( UIPopupClanNameSearch);


extern "C" 
{
	int UIPBRe_PopUp_NameCheck_Btn( LuaState * L)
	{
		UIPopupClanNameSearch* pPopup = static_cast<UIPopupClanNameSearch*>(GameUI::GetPopup(UPW_CLAN_NAME_CHECK));
		if( pPopup != nullptr)
			pPopup->_OverLappingCheck(L);
		return 0;
	}

	int UIPBRe_PopUp_NameCheck_BtnOK( LuaState * L)
	{
		UIPopupClanNameSearch* pPopup = static_cast<UIPopupClanNameSearch*>(GameUI::GetPopup(UPW_CLAN_NAME_CHECK));
		if( pPopup != nullptr)
			pPopup->_OverLappingOK();
		return 0;
	}

	int UIPBRe_PopUp_NameCheck_getEditBoxText(LuaState * L)
	{
		UIPopupClanNameSearch* pPopup = static_cast<UIPopupClanNameSearch*>(GameUI::GetPopup(UPW_CLAN_NAME_CHECK));
		if( pPopup != nullptr)
		{
			char szName[MAX_STRING_COUNT];
			i3Lua::GetStringArg(L, 1, szName, MAX_STRING_COUNT);
			i3::utf8_to_utf16(szName, pPopup->m_wstrClanName);		
		}
		return 0;
	}
}

LuaFuncReg UIPBRe_PopUp_NameCheck_Glue[] =
{
	{ "NameCheck", UIPBRe_PopUp_NameCheck_Btn },
	{ "getEditBoxText", UIPBRe_PopUp_NameCheck_getEditBoxText },
	{ "BtnOK", UIPBRe_PopUp_NameCheck_BtnOK	},
	{nullptr, nullptr}
};

UIPopupClanNameSearch::UIPopupClanNameSearch(void)
{
	m_bPopupTC = true;
	m_pSerach = nullptr;

	
	m_bCheckedName = false;
	m_bCheckedAddress = false;
	m_bWaitingIsDuplicateName = false;
	m_bWaitingIsDuplicateAddress = false;

	RegisterGameEventFunc( EVENT_CLAN_DUPLICATE_NAME,				&UIPopupClanNameSearch::ReceiveGameEvent_DuplicateName);
	RegisterGameEventFunc( EVENT_CLAN_DUPLICATE_ADDRESS,			&UIPopupClanNameSearch::ReceiveGameEvent_DuplicateAddress);
}

UIPopupClanNameSearch::~UIPopupClanNameSearch(void)
{
}

/*virtual*/ void UIPopupClanNameSearch::OnCreate( i3GameNode * pParent)
{
	UIPopupClanBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PBRe_PopUp_NameCheck.i3UIs", UIPBRe_PopUp_NameCheck_Glue, true, false);
}

/*virtual*/ void	UIPopupClanNameSearch::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupClanBase::_InitializeAtLoad( pScene);
}

void UIPopupClanNameSearch::__OverLappingOK()
{
	UIPopupClanBase::_CheckNickNameOverLap(m_wstrClanName);

	if( !m_bCheckedName)
	{
		//	"클랜명 중복검사를 해주십시오."
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_PLEASE_CHECK_NAME"));
		return;
	}

	m_bCheckedName = false;

	SearchInfo stInfo;
	stInfo.bDuplicateAddress = false;
	stInfo.bDuplicateName = false;
	stInfo.wstrClanName = m_wstrClanName;
	stInfo.wstrClanURL = m_wstrURL;

	GetMainFrame()->TogglePopup(UPW_CLAN_CREATE, &stInfo);
	GetMainFrame()->TogglePopup(UPW_CLAN_NAME_CHECK);
}

void UIPopupClanNameSearch::_OverLappingOK()
{
	LuaState* L = _CallLuaFunction( "GetEditBox");
	_EndLuaFunction( L, 0);

	__OverLappingOK();
}

void UIPopupClanNameSearch::__OverLappingCheck()
{
	UIPopupClanBase::_CheckNickNameOverLap( m_wstrClanName);
	g_pFramework->GetClanContext()->SendOverlappingClanName(m_wstrClanName);
}

void UIPopupClanNameSearch::_OverLappingCheck( LuaState * L)
{
//	m_bCheckingName = true; 

	L = _CallLuaFunction( "GetEditBox");
	_EndLuaFunction( L, 0);
	
	__OverLappingCheck();
}

bool UIPopupClanNameSearch::_CheckClanAddressOverLap( LuaState * L)
{
#ifdef CLAN_ADDRESS_ENABLE
	L = _CallLuaFunction( "GetEditBox");
	_EndLuaFunction( L, 0);

	if( m_bWaitingIsDuplicateAddress) 
		return false;

	if(!UIPopupClanBase::_CheckClanAddressOverLap( m_szName) )
	{
	//	m_pSerach->SetText(_T(""));
		return false;
	}

	//	클랜명 중복 체크에 영향 받지 않도록
	if(!m_bWaitingIsDuplicateName)
		m_bCheckedAddress = false;
	
	m_bWaitingIsDuplicateAddress = true;

	return true;

#else
	m_bCheckedAddress = true;

	return true;
#endif
}

bool UIPopupClanNameSearch::_CheckClanAddressValidate(const char * pszText)
{
#ifdef CLAN_ADDRESS_ENABLE
	if(!UIPopupClanBase::_CheckClanAddressValidate(pszText))
	{
		m_bCheckedAddress = false;
		return false;
	}

	return true;
#else
	// 클랜 주소 Check 여부
	m_bCheckedAddress = true;

	return true;
#endif
}

bool UIPopupClanNameSearch::_OnPopupCheckOverlapping( const i3::rc_wstring& wstrName, bool bDuplicate)
{
	i3::stack_wstring wstrPopMsg;
	
	//	"은/는 이미 사용중이거나 사용할수 없는 이름입니다. \n 다른 이름을 사용해 주십시오."
	
	if(!UIPopupClanBase::_OnPopupCheckOverlapping(wstrPopMsg, wstrName, bDuplicate))
	{
		LuaState * L = _CallLuaFunction("Duplicate");
		i3Lua::PushStringUTF16To8(L, wstrPopMsg);
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction(L, 2);
		return false;
	}

	LuaState * L = _CallLuaFunction("Duplicate");
	i3Lua::PushStringUTF16To8(L, wstrPopMsg);
	i3Lua::PushBoolean(L, true);
	_EndLuaFunction(L, 2);
	return true;
}

bool	UIPopupClanNameSearch::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	if(pArg1 != nullptr)
	{
		SearchInfo * stInfo =  static_cast<SearchInfo*>(pArg1);

		LuaState * L = _CallLuaFunction("setString");
		i3Lua::PushBoolean(L, true);
		_EndLuaFunction(L, 1);
		_OnPopupCheckOverlapping( stInfo->wstrClanName, stInfo->bDuplicateName);
	}
	
	return true;
}

void UIPopupClanNameSearch::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupClanNameSearch::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPopupClanBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)( arg, UserData);
	}
}

void UIPopupClanNameSearch::ReceiveGameEvent_DuplicateName( INT32 arg, const i3::user_data&)
{
	//	클랜 이름 중복 체크
//	char szTemp[MAX_STRING_LENGTH] = "";

	LuaState * L = _CallLuaFunction("GetEditBox");
	_EndLuaFunction(L,0);
	if( arg == 0)
	{
		m_bCheckedName = true;
		_OnPopupCheckOverlapping(m_wstrClanName, !m_bCheckedName);
	}
	else
	{
		m_bCheckedName = false;

		//	"은/는 이미 사용중이거나 사용할수 없는 이름입니다.\n 다른 이름을 선택해 주세요."
		i3::stack_wstring wstrTemp;

		i3::sprintf( wstrTemp, L"\n {col:255, 255, 255, 255} %s {/col} \n \n %s" , 
			m_wstrClanName, GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_NAME"));

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrTemp);
		_OnPopupCheckOverlapping(m_wstrClanName, !m_bCheckedName);
	}

	m_bWaitingIsDuplicateName = false;
}

void UIPopupClanNameSearch::ReceiveGameEvent_DuplicateAddress( INT32 arg, const i3::user_data&)
{

	LuaState * L = _CallLuaFunction("GetEditBox");
	_EndLuaFunction(L,0);

	if( arg == 0)
	{
		m_bCheckedAddress = true;
		i3::stack_wstring wstrStackTemp;
		UIPopupClanBase::_OnPopupCheckOverlappingAddress(wstrStackTemp, m_wstrClanName, false);	// 뭘하자는건지 이해 안감..(2014.07.14.수빈)	
	}
	else
	{
		m_bCheckedAddress = false;

		//	"은/는 이미 사용중이거나 사용할수 없는 주소입니다.\n 다른 주소를 선택해 주세요."
		i3::rc_wstring wstrTemp;
		i3::sprintf( wstrTemp, L"\n {col:255, 255, 255, 255} %s {/col} \n \n %s", m_wstrClanName, GAME_RCSTRING("STBL_IDX_CLAN_ALREADY_USED_AZIT"));

		GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrTemp);

		i3::stack_wstring wstrStackTemp;
		UIPopupClanBase::_OnPopupCheckOverlappingAddress(wstrStackTemp, m_wstrClanName, true);		// 뭘하자는건지 이해 안감..(2014.07.14.수빈)
	}

	m_bWaitingIsDuplicateAddress = false;
}