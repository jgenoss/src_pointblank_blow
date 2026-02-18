#include "pch.h"
#include "UISubTabClanMgt_Member.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UITabClanManagement.h"
#include "UIUtil.h"

#include "ClanDef.h"
#include "../CommunityContext.h"
#include "../StageBattle/NetworkContext.h"
#include "ClanContext.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( UISubTabClanMgt_Member);

static UISubTabClanMgt_Member * GetThis()
{
	UIPhaseClanMember* pPhase = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		UITabClanManagement* pClan = static_cast<UITabClanManagement*>(pPhase->GetTab( UTC_MGT));
		if( pClan != nullptr)
		{
			return static_cast<UISubTabClanMgt_Member *>(pClan->GetTab(USTC_MGT_GRADE));
		}
	}

	return nullptr;
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UISubTabClanMgt_Member_SelectCell( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		INT32 i32Cell = i3Lua::GetIntegerArg(L , 2);
		
		if(i32Slot > -1)
		{
			if(i32Cell == 4)
			{
				UISubTabClanMgt_Member * pThis = GetThis();
				if( pThis != nullptr)
					pThis->ChangeUserLevel(i32Slot);
			}
		}

		return 0;
	}	

	int UISubTabClanMgt_Member_SelectComboItem( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		INT32 i32ComboIdx = i3Lua::GetIntegerArg(L , 2);

		if(i32Slot > -1 && i32ComboIdx > -1)
		{
			UISubTabClanMgt_Member * pThis = GetThis();
			if( pThis != nullptr)
				pThis->SelectComboItem(i32Slot, i32ComboIdx);
		}

		return 0;
	}	
}

LuaFuncReg UISubTabClanMgt_Member_Glue[] = {
	{"SelectCell",			UISubTabClanMgt_Member_SelectCell},
	{"SelectComboItem",		UISubTabClanMgt_Member_SelectComboItem},
	{nullptr,					nullptr},
};

//------------------------------------------------------------------------------//


UISubTabClanMgt_Member::UISubTabClanMgt_Member()
{
	m_pClanContext = g_pFramework->GetClanContext();
}

UISubTabClanMgt_Member::~UISubTabClanMgt_Member()
{

}

void UISubTabClanMgt_Member::_UpdateMemberList(void)
{
	//  멤버 리스트 정보 재구성
	_UpdateSortClanMemberList();

	UINT32 nCount = m_vecManageMember.size();


	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		if (nCount < 9)
		{
			nCount = 9;
		}
	}
	else
	{
		if (nCount < 8)
		{
			nCount = 8;
		}
	}


	LuaState * L = _CallLuaFunction("ClearList");

	if( L != nullptr)
	{
		i3Lua::PushInteger(L, nCount);
		_EndLuaFunction( L, 1);
	}


	for(size_t i = 0; i < nCount; i++)
	{
		if( i < m_vecManageMember.size())
		{
			const CLAN_MEMBER_BASIC_INFO * pMember = m_vecManageMember[i].get();
			// 슬롯 데이터 설정
			_SetClanMgtMemberInfo( i, pMember);
		}
		else
		{
			_SetClanMgtMemberInfo(i , nullptr);
		}
	}
}

void UISubTabClanMgt_Member::_SetClanMgtMemberInfo(INT32 nSlot, const CLAN_MEMBER_BASIC_INFO * pMember)
{
	LuaState * L = _CallLuaFunction("SetMemberInfo");

	if(L != nullptr)
	{
		if(pMember != nullptr)
		{
			// 닉네임 컬러
			const I3COLOR * pNickColor = GameUI::GetNickNameColor( pMember->_color);

			//	멤버 직급
			i3::wstring wstrLevel;
			GetMemberLevelName2( pMember->_level, wstrLevel);

			// 직급 컬러
			COLOR Color;

			switch(pMember->_level)
			{
			case CLAN_MEMBER_LEVEL_MASTER:
				i3Color::Set(&Color, (UINT8)205, 205, 207, 255);
				break;
			case CLAN_MEMBER_LEVEL_STAFF:
				i3Color::Set(&Color, (UINT8)0,	255, 0, 255);
				break ;
			case CLAN_MEMBER_LEVEL_REGULAR:
			default:
				i3Color::Set(&Color, (UINT8)214, 186, 147, 255);
				break;
			}

			i3Lua::PushInteger( L, nSlot);
			i3Lua::PushInteger( L, pMember->_rank);				// 계급
			i3Lua::PushStringUTF16To8( L, pMember->_nick);		// 닉네임		
			i3Lua::PushColor( L, pNickColor);							// 닉네임 컬러
			i3Lua::PushStringUTF16To8( L, wstrLevel);						// 직급
			i3Lua::PushColor( L, &Color);								// 직급 컬러
			i3Lua::PushInteger( L, m_vecMemberGradeInfo[nSlot] - 1);	// Combo CurSel
			i3Lua::PushUserData(L , (void*)CbSetSlotComboData);			// Combo CallBack
			i3Lua::PushStringUTF16To8( L, L"0");							// Btn Enable
			i3Lua::PushUserData(L , (void*)CbSetOKBtn);					// Btn CallBack
		}
		else
		{
			COLOR Color;
			i3Color::Set(&Color, (UINT8)214, 186, 147, 255);

			i3Lua::PushInteger( L, nSlot);
			i3Lua::PushInteger( L, -1);			// 계급
			i3Lua::PushStringUTF16To8( L, L"");	// 닉네임
			i3Lua::PushColor( L, &Color);		// 닉네임 컬러
			i3Lua::PushStringUTF16To8( L, L"");	// 직급
			i3Lua::PushColor( L, &Color);		// 직급 컬러
			i3Lua::PushInteger(L , -1);			// Combo CurSel
			i3Lua::PushUserData(L , nullptr);		// Combo CallBack			
			i3Lua::PushStringUTF16To8( L, L"-1");	// Btn Enable
			i3Lua::PushUserData(L , nullptr);		// Btn CallBack
		}

		_EndLuaFunction(L , 16);
	}
}

void UISubTabClanMgt_Member::_UpdateClanManagementMemberPhase(REAL32 rDeltaSeconds)
{
	CLAN_DETAIL_INFO * pClanDetailInfo = m_pClanContext->GetMyClanDetailInfo();
	I3ASSERT( pClanDetailInfo);

	//	클랜원 정보 업데이트
	m_pClanContext->UpdateClanMember(rDeltaSeconds);
}

void UISubTabClanMgt_Member::_UpdateSortClanMemberList(void)
{
	INT32 nNewCount = m_pClanContext->GetMyClanMemberCount(); 

	m_vecManageMember.clear();
	m_vecManageMember.reserve(nNewCount);

	//	클랜원 정렬 리스트 재구성

	for(INT32 i=0; i< nNewCount; i++)
	{
		const CLAN_MEMBER_BASIC_INFO * pSrcInfo = m_pClanContext->GetMyClanMember(i);

		if( pSrcInfo->_i64UID > 0)
		{
			CLAN_MEMBER_BASIC_INFO_PTR info_ptr = i3::make_shared<CLAN_MEMBER_BASIC_INFO>();
			CLAN_MEMBER_BASIC_INFO* pDestInfo = info_ptr.get();

			i3mem::Copy( pDestInfo, pSrcInfo, sizeof(CLAN_MEMBER_BASIC_INFO));
			m_vecManageMember.push_back(info_ptr);
		}
	}	
	
	// 클랜 등급으로 결정
	struct cbSortMgtMember
	{
		bool operator()(const CLAN_MEMBER_BASIC_INFO_PTR& lhs, const CLAN_MEMBER_BASIC_INFO_PTR& rhs) const
		{
			return lhs->_level < rhs->_level;
		}
	};

	i3::sort(m_vecManageMember.begin(), m_vecManageMember.end(), cbSortMgtMember());

	// 멤버 등급으로 정렬 후, 변경용 변수를 설정
	const size_t numMember = m_vecManageMember.size();
	m_vecMemberGradeInfo.resize(numMember);
	for (size_t i = 0 ; i < numMember; ++i)
	{
		m_vecMemberGradeInfo[i] = m_vecManageMember[i]->_level;
	}

}

void UISubTabClanMgt_Member::_ChangeMemberLevel(CLAN_MEMBER_LEVEL_TYPE eType, INT64 i64UID)
{	
#if defined(TEST_CLAN_MEMBER_LIST)
	GameUI::MsgBox( MSG_TYPE_GAME_OK, "TEST_CLAN_MEMBER_LIST");
	return;
#endif

	if( m_pClanContext->GetMyStaffLevel() != CLAN_MEMBER_LEVEL_MASTER ) 
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_REQUIRE_MASTER"));	//	"마스터 권한이 필요합니다."
		return;
	}

	switch(eType)
	{
	case CLAN_MEMBER_LEVEL_MASTER:		//	마스터 위임	
		{
			m_pClanContext->ProcessCommissionMaster(&i64UID, 1);
		}			 
		break;
	case CLAN_MEMBER_LEVEL_STAFF:		//	스텝 임명
		{
			m_pClanContext->ProcessAppointMember(&i64UID, 1);
		}
		break;
	case CLAN_MEMBER_LEVEL_REGULAR:		//	일반으로 강등
		{
			m_pClanContext->ProcessDemoteMember(&i64UID, 1);
		}
		break;
	}
}

void UISubTabClanMgt_Member::_DismissalMember(INT64 i64UID)
{
	if( false == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_DEPORTATION ) ) 
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));
	}
	else
	{
		m_pClanContext->ProcessDismissalMember(&i64UID, 1);
	}
}

bool UISubTabClanMgt_Member::_CheckMemberInfo(CLAN_MEMBER_BASIC_INFO * pInfo)
{
	if(pInfo->_i64UID > 0)
	{
		T_UID i64MyUID = NetworkContext::i()->UID;

		// 자기자신의 정보가 아니고 클랜 마스터의 정보가 아닐 경우에 true 리턴 
		if(pInfo->_i64UID != i64MyUID && pInfo->_level != (UINT8)CLAN_MEMBER_LEVEL_MASTER)
		{
			return true;
		}
	}
	
	GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_CLAN_MANAGEMENT_ERROR_ME")); 	// 올바른 권한이 아닙니다.
	return false;
}

/*virtual*/ void UISubTabClanMgt_Member::OnCreate(i3GameNode * pParent)
{
	UISubTabClan_Mgt::OnCreate(pParent);
	//LoadScene
	AddScene("Scene/Clan/PointBlankRe_Clan_Management_Grade.i3UIs", UISubTabClanMgt_Member_Glue);
}

/*virtual*/ void UISubTabClanMgt_Member::OnLoadAllScenes()
{
	UISubTabClan_Mgt::OnLoadAllScenes();

	m_vecManageMember.clear();
	m_vecMemberGradeInfo.clear();

	//	서버에 클랜 멤버 리스트 요청
	m_pClanContext->EnterClanMember();

	if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_UserGradeList");
		pBox->getHeader()->SetHeaderFont("", 8 );
	}
	else if( g_pEnvSet->m_nUIVersion == UFV_1_1 )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_UserGradeList");
		pBox->getHeader()->SetHeaderFont("", 9 );
	}
}

/*virtual*/ bool UISubTabClanMgt_Member::OnEntranceStart(void * pArg1 /* = nullptr  */, void * pArg2 /* = nullptr */ )
{
	if( UISubTabClan_Mgt::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_UpdateMemberList();

	return true;
}

/*virtual*/ void UISubTabClanMgt_Member::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UISubTabClan_Mgt::ProcessGameEvent(event,arg, UserData);

	if( event == EVENT_CLAN_MEMBER_LIST)
	{
		if( EV_SUCCESSED(arg))
		{
			_UpdateMemberList();
		}
	}
}

/*virtual*/ void UISubTabClanMgt_Member::OnUpdate(REAL32 rDeltaSeconds)
{
	UISubTabClan_Mgt::OnUpdate( rDeltaSeconds);
	_UpdateClanManagementMemberPhase(rDeltaSeconds);
}

void UISubTabClanMgt_Member::SelectComboItem(INT32 i32Slot, INT32 i32ComboIdx)
{
	m_vecMemberGradeInfo[i32Slot] = (UINT8) i32ComboIdx + 1;

// 	LuaState * L = _CallLuaFunction("SetComboItemIdx");
// 
// 	i3Lua::PushInteger(L, i32Slot);
// 	i3Lua::PushInteger(L, i32ComboIdx);
// 	_EndLuaFunction(L , 2);
}

void UISubTabClanMgt_Member::ChangeUserLevel(INT32 i32Slot)
{
	if(m_pClanContext->IsEnter() == false || m_vecManageMember.size() <= (UINT32)i32Slot)
		return;

	CLAN_MEMBER_BASIC_INFO* pMember = m_vecManageMember[i32Slot].get();

	// 변경 가능한 유저인지 체크
	if(_CheckMemberInfo(pMember) == false)
		return;

	if(m_vecMemberGradeInfo[i32Slot] != pMember->_level)
	{
		switch(m_vecMemberGradeInfo[i32Slot])
		{
		case 1:							// 마스터 위임
		case 2:							// 스태프 임명
		case 3:							// 일반회원 강등	
			{
				_ChangeMemberLevel((CLAN_MEMBER_LEVEL_TYPE) m_vecMemberGradeInfo[i32Slot], (INT64)pMember->_i64UID);
			}
			break;
		case 4:							// 강제제명
			{
				_DismissalMember((INT64)pMember->_i64UID);
			}
			break;
		default: break;
		}
	}
}

void UISubTabClanMgt_Member::CbSetSlotComboData(i3UIListView_Cell * pCell, void * szData)
{
	i3UIComboBox * pCtrl = reinterpret_cast<i3UIComboBox *>(pCell->getCellChild());
	
	if(pCtrl != nullptr)
	{
		i3UIEditBox * pEdit = pCtrl->getEditBox();

		pCtrl->SetEditBoxReadOnly(true);
		if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
		{
			pCtrl->SetButtonhHeight(17);
			pCtrl->setSize(138, STAFF_COMBO_HEIGHT);
			pEdit->SetFont(pEdit->getFontName(),8);
			REAL32 CtrlPosX = (pCell->getWidth() - pCtrl->getWidth()) * 0.5f ;
			REAL32 CtrlPosY = (pCell->getHeight() - 17) * 0.5f;

			pCtrl->setPos(CtrlPosX, CtrlPosY);
			pCtrl->FoldListBox();
			i3UIListBox* pListbox = pCtrl->getListBox();
			pListbox->SetFont(pEdit->getFontName(),8);
			COLOR color;
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pListbox->setColor(&color);
		}
		else
		{
			pCtrl->SetButtonhHeight(STAFF_COMBO_BUTTON_SIZE);
			pCtrl->setSize(STAFF_COMBO_WIDTH, STAFF_COMBO_HEIGHT);
			pEdit->SetFont(pEdit->getFontName(),STAFF_COMBO_FONTSIZE);
			REAL32 CtrlPosX = (pCell->getWidth() - pCtrl->getWidth()) * 0.5f ;
			REAL32 CtrlPosY = (pCell->getHeight() - STAFF_COMBO_BUTTON_SIZE) * 0.5f;

			pCtrl->setPos(CtrlPosX, CtrlPosY);
			pCtrl->FoldListBox();
		}
		

		pEdit->SetTextAlign(I3UI_TEXT_ALIGN_X_LEFT, I3UI_TEXT_ALIGN_Y_MIDDLE);

 		COLOR color;
 		i3Color::Set(&color, (UINT8)205, 205, 207, 255);
 		pEdit->SetTextColor(&color);
 		pEdit->SetTextSpace(6, 0);

		pCtrl->SetTextColor(&color);
	}
}

void UISubTabClanMgt_Member::CbSetOKBtn(i3UIListView_Cell * pCell, void * szData)
{
	i3UIButtonComposed3 * pCtrl = reinterpret_cast<i3UIButtonComposed3*>(pCell->getCellChild());

	if(pCtrl != nullptr)
	{
		if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
		{
			pCtrl->setSize(56, STAFF_BUTTON_HEIGHT);
		}
		else
		{
			pCtrl->setSize(STAFF_BUTTON_WIDTH, STAFF_BUTTON_HEIGHT);
		}
		
		REAL32 BtnPosX = (pCell->getWidth() - pCtrl->getWidth()) * 0.5f;
		REAL32 BtnPosY = (pCell->getHeight() - STAFF_BUTTON_HEIGHT) * 0.5f;
		pCtrl->setPos(BtnPosX, BtnPosY);			

		pCtrl->SetText(GAME_RCSTRING("STBL_IDX_CLAN_BUTTON_OK"));

		COLOR color;
		if (g_pEnvSet->IsBaseOnUIVersion_1_0() == false)
		{
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);

			i3Color::Set(&color, (UINT8) 6, 82, 149, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, color);

			i3Color::Set(&color, (UINT8) 0, 51, 96, 255 );
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, color);
			pCtrl->SetFont(pCtrl->getFontName(),STAFF_BUTTON_FONTSIZE);
		}
		else
		{
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);

			i3Color::Set(&color, (UINT8) 205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, color);

			i3Color::Set(&color, (UINT8) 205, 205, 207, 255 );
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, color);
			pCtrl->SetFont(pCtrl->getFontName(),8);
		}

		
	}
}

