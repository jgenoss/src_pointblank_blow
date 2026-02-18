#include "pch.h"
#include "UISubTabClanMgt_Control.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UITabClanManagement.h"
#include "ClanDef.h"
#include "../CommunityContext.h"
#include "ClanContext.h"

#include "UIPopupReceiveMail.h"
#include "ClanGameContext.h"
#include "../UI/UIClanFunction.h"

#include "UIUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/utf8_to_utf16.h"


#define CLAN_AUTHORITY_1 0x0001	//게시글 관리 권한
#define CLAN_AUTHORITY_2 0x0002	// 클랜원 제명 권한
#define CLAN_AUTHORITY_3 0x0004 // 클랜 초대 권한
#define CLAN_AUTHORITY_4 0x0008 //  가입관리 권한

I3_CLASS_INSTANCE( UISubTabClanMgt_Control);//, UISubTabClan_Mgt);


static UISubTabClanMgt_Control * GetThis()
{
	UIPhaseClanMember* pPhase = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		UITabClanManagement* pClan = static_cast<UITabClanManagement*>(pPhase->GetTab( UTC_MGT));
		if( pClan != nullptr)
		{
			return static_cast<UISubTabClanMgt_Control*>(pClan->GetTab(USTC_MGT_CONTROL));
		}
	}

	return nullptr;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UISubTabClanMgt_Control_Apply(LuaState * L)
	{
		GetThis()->Apply();
		return 0;
	}	
	int UISubTabClanMgt_Control_Grade(LuaState * L)
	{
		INT32 nValue = i3Lua::GetIntegerArg( L, 1);

		GetThis()->SetAgeRank((UINT8)nValue);
		return 0;
	}	
	int UISubTabClanMgt_Control_AgeUpper(LuaState * L)
	{
		INT32 nValue = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SetAgeUpper((UINT8)nValue);
		return 0;
	}	
	int UISubTabClanMgt_Control_AgeLower(LuaState * L)
	{
		INT32 nValue = i3Lua::GetIntegerArg( L, 1);
		GetThis()->SetAgeLower((UINT8)nValue);
		return 0;
	}	
}

LuaFuncReg UISubTabClanMgt_Control_Glue[] = {
	{"uiClick_Apply", UISubTabClanMgt_Control_Apply},
	{"uiClick_Grade", UISubTabClanMgt_Control_Grade},
	{"uiClick_AgeUpper", UISubTabClanMgt_Control_AgeUpper},
	{"uiClick_AgeLower", UISubTabClanMgt_Control_AgeLower},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UISubTabClanMgt_Control::UISubTabClanMgt_Control()
{
	
}

UISubTabClanMgt_Control::~UISubTabClanMgt_Control()
{
	
}




/*virtual*/ void UISubTabClanMgt_Control::OnCreate(i3GameNode * pParent)
{
	UISubTabClan_Mgt::OnCreate(pParent);

	AddScene("Scene/Clan/PointBlankRe_Clan_Management_Control.i3UIs", UISubTabClanMgt_Control_Glue);
}

/*virtual*/ bool UISubTabClanMgt_Control::OnEntranceStart( void * pArg1, void * pArg2)
{
	if (UISubTabClan_Mgt::OnEntranceStart(pArg1, pArg2) == false ) return false;

	UpdateDetailInfo();
	return true;
}


/*virtual*/ void UISubTabClanMgt_Control::_InitializeAtLoad( i3UIScene * pScene)
{
	UISubTabClan_Mgt::_InitializeAtLoad( pScene);

	ucf::ComboString wstrGrade; 
	
	i3::wstring temp = L"";

	

	wstrGrade<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT")
		<<GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_1")
		<<GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_2")
		<<GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_3")
		<<GAME_RCSTRING("STBL_IDX_CLAN_RANK_LIMIT_4");

	ucf::ComboString wstrAge1, wstrAge2; 

	wstrAge1<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UPPER_1")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UPPER_2")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UPPER_3");

	wstrAge2<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_UNLIMIT")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_LOWER_1")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_LOWER_2")
		<<GAME_RCSTRING("STBL_IDX_CLAN_AGE_LOWER_3");

	LuaState * L = _CallLuaFunction("SetComboBoxString");
	if ( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3::sprintf(temp, L"%s", i3::wstring(wstrGrade));
		i3Lua::PushStringUTF16To8( L, temp);						// 계급
	}
	else
	{
		i3Lua::PushStringUTF16To8( L, i3::wstring(wstrGrade));
	}
	
	if ( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3::sprintf(temp, L"%s", i3::wstring(wstrAge1));
		i3Lua::PushStringUTF16To8( L, temp);						// 나이
	}
	else
	{
		i3Lua::PushStringUTF16To8( L, i3::wstring(wstrAge1));
	}
	
	if ( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3::sprintf(temp, L"%s", i3::wstring(wstrAge2));
		i3Lua::PushStringUTF16To8( L, temp);	
	}
	else
	{
		i3Lua::PushStringUTF16To8( L, i3::wstring(wstrAge2));
	}
						
	_EndLuaFunction(L , 3);

	ZeroMemory(&m_Management_Control, sizeof(m_Management_Control));
}

/*virtual*/ void UISubTabClanMgt_Control::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UISubTabClan_Mgt::ProcessGameEvent( event, arg, UserData);

	switch( event )
	{
	case EVENT_CLAN_MANAGEMENT:
		{
			
		}
		break;
	}
}

/*virtual*/ void UISubTabClanMgt_Control::_ClearAtUnload( i3UIScene * pScene)
{
	UISubTabClan_Mgt::_ClearAtUnload( pScene);

}


void UISubTabClanMgt_Control::Apply()
{
 	m_Management_Control._authority = CLAN_STAFF_AUTH_NONE;
 
 	i3UICheckBox* check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox"));
 	if( check->isChecked() )
 		m_Management_Control._authority |= CLAN_STAFF_AUTH_BOARD;
 
 	check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox0"));
 	if( check->isChecked() )
 		m_Management_Control._authority |= CLAN_STAFF_AUTH_EJECT;
 
 	check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox1"));
 	if( check->isChecked() )
 		m_Management_Control._authority |= CLAN_STAFF_AUTH_INVITE;
 
 	check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox2"));
 	if( check->isChecked() )
 		m_Management_Control._authority |= CLAN_STAFF_AUTH_JOIN;
 
 	g_pFramework->GetClanContext()->ProcessManagementModify(&m_Management_Control);
}

void UISubTabClanMgt_Control::UpdateDetailInfo()
{
 	CLAN_DETAIL_INFO_ALL* pClanDetailInfoAll = ClanGameContext::i()->setMyClanDetailInfoAll();
 
 	//스테프 권한 설정
 	UINT8 staff = pClanDetailInfoAll->m_ClanDetailInfo.m_authstaff;
 	i3UICheckBox* check = nullptr;
 	if( staff & CLAN_STAFF_AUTH_BOARD )
 	{
 		check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox"));
 		check->setCheck(true);
 	}
 
 	if( staff & CLAN_STAFF_AUTH_EJECT )
 	{
 		check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox0"));
 		check->setCheck(true);
 	}
 
 	if( staff & CLAN_STAFF_AUTH_INVITE )
 	{
 		check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox1"));
 		check->setCheck(true);
 	}
 
 	if( staff & CLAN_STAFF_AUTH_JOIN )
 	{
 		check = static_cast<i3UICheckBox*>(GetScene(0)->FindChildByName("i3UICheckBox2"));
 		check->setCheck(true);
 	}
 
 	UINT8 rank = pClanDetailInfoAll->m_ClanDetailInfo.m_ranklock;
 	//계급 제한설정
 	
	INT32 nIndex = 0;
	if(rank >= RANK_46)
	{
		nIndex = 4;
	}
	else if (rank >= RANK_31)
	{
		nIndex = 3;
	}
	else if( rank >= RANK_17)
	{
		nIndex = 2;
	}
	else if ( rank >= RANK_04)
	{
		nIndex = 1;
	}
	else
	{
		nIndex = 0;
	}

	LuaState * L = _CallLuaFunction("SetRank");
	i3Lua::PushInteger( L, nIndex);				
	_EndLuaFunction(L , 1);

 	//나이 제한설정
	UINT8 age = pClanDetailInfoAll->m_ClanDetailInfo.m_hagelock;
	nIndex = 0;

	if(age > 20)
		nIndex = 3;	// 30세
	else if (age > 15)
		nIndex = 2;	// 20세
	else if (age > 0)
		nIndex = 1;	// 15세
	else
		nIndex = 0;

	L = _CallLuaFunction("SetAgeUpper");
	i3Lua::PushInteger( L, nIndex);				
	_EndLuaFunction(L , 1);

	age = pClanDetailInfoAll->m_ClanDetailInfo.m_lagelock;
	nIndex = 0;

	if(age > 20)
		nIndex = 3;	// 30세
	else if (age > 15)
		nIndex = 2;	// 20세
	else if (age > 0)
		nIndex = 1;	// 15세
	else
		nIndex = 0;

	L = _CallLuaFunction("SetAgeLower");
	i3Lua::PushInteger( L, nIndex);				
	_EndLuaFunction(L , 1);
}

void UISubTabClanMgt_Control::SetAgeRank(UINT8 rank)
{
 	UINT8 nIndex = 0;
 	if(rank >= 4)
 	{
 		nIndex = RANK_46;
 	}
 	else if (rank >= 3)
 	{
 		nIndex = RANK_31;
 	}
 	else if( rank >= 2)
 	{
 		nIndex = RANK_17;
 	}
 	else if ( rank >= 1)
 	{
 		nIndex = RANK_04;
 	}
 
 	m_Management_Control._limitRank = nIndex; 
	CLAN_DETAIL_INFO_ALL* pClanDetailInfoAll = ClanGameContext::i()->setMyClanDetailInfoAll();
	pClanDetailInfoAll->m_ClanDetailInfo.m_ranklock = nIndex;
}
void UISubTabClanMgt_Control::SetAgeUpper(UINT8 age)
{ 
 	UINT8 nIndex = 0;
 
 	if(age == 3)
 		nIndex = 30;	// 30세
 	else if (age == 2)
 		nIndex = 20;	// 20세
 	else if ( age == 1 )
 		nIndex = 15;	// 15세
	else
		nIndex = 0;
 
 	m_Management_Control._limitAgeHigh = nIndex;
	CLAN_DETAIL_INFO_ALL* pClanDetailInfoAll = ClanGameContext::i()->setMyClanDetailInfoAll();
	pClanDetailInfoAll->m_ClanDetailInfo.m_lagelock = nIndex;
}

void UISubTabClanMgt_Control::SetAgeLower(UINT8 age)
{ 
 	UINT8 nIndex = 0;

	if(age == 3)
		nIndex = 30;	// 30세
	else if (age == 2)
		nIndex = 20;	// 20세
	else if ( age == 1 )
		nIndex = 15;	// 15세
	else
		nIndex = 0;
 
 	m_Management_Control._limitAgeLow = nIndex; 
	CLAN_DETAIL_INFO_ALL* pClanDetailInfoAll = ClanGameContext::i()->setMyClanDetailInfoAll();
	pClanDetailInfoAll->m_ClanDetailInfo.m_hagelock = nIndex;
}