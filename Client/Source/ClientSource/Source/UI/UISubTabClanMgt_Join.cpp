#include "pch.h"
#include "UISubTabClanMgt_Join.h"
#include "UIMainFrame.h"
#include "UIPhaseClanMember.h"
#include "UITabClanManagement.h"
#include "UIUtil.h"

#include "../CommunityContext.h"
#include "ClanDef.h"
#include "ClanContext.h"
#include "GameContextUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#define CLAN_SUBSCRIBER_CONTENTS_SIZE 35

I3_CLASS_INSTANCE( UISubTabClanMgt_Join);//, UISubTabClan_Mgt);

static UISubTabClanMgt_Join * GetThis()
{
	UIPhaseClanMember* pPhase = static_cast<UIPhaseClanMember*>(GameUI::GetPopup(UPW_CLAN_MEMBER));
	if( pPhase != nullptr)
	{
		UITabClanManagement* pClan = static_cast<UITabClanManagement*>(pPhase->GetTab( UTC_MGT));
		if( pClan != nullptr)
		{
			return static_cast<UISubTabClanMgt_Join *>(pClan->GetTab(USTC_MGT_JOIN));
		}
	}

	return nullptr;
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UISubTabClanMgt_Join_SelectBtn( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L , 1);
		INT32 i32Cell = i3Lua::GetIntegerArg(L , 2);

		if(i32Slot > -1)
		{
			UISubTabClanMgt_Join * pThis = GetThis();
			if( pThis != nullptr)
			{
				if(i32Cell == 4)
				{
					// 클랜 멤버 대기를 받지 않고 바로 클릭하면 아무 정보 없는 멤버 데이터가 들어갑니다.
					// 그래서 Wait가 끝나고 버튼 클릭이 되도록 수정합니다. 2014-12-23(Jinsik.kim)
					if( g_pFramework->GetClanContext()->isWaitMemberInfoReq() == false)
						pThis->OnJoinAccept(i32Slot);
				}
				else if(i32Cell == 5)
				{	
					pThis->OnJoinReject(i32Slot);
				}
			}
		}

		return 0;
	}
}

LuaFuncReg UISubTabClanMgt_Join_Glue[] = {
	{"SelectBtn",			UISubTabClanMgt_Join_SelectBtn	},
	{nullptr,					nullptr},
};

//------------------------------------------------------------------------------//


UISubTabClanMgt_Join::UISubTabClanMgt_Join()
{
	m_pClanContext = g_pFramework->GetClanContext();
}

UISubTabClanMgt_Join::~UISubTabClanMgt_Join()
{

}

void UISubTabClanMgt_Join::_OnUpdateSubscriber(REAL32 rDeltaSeconds)
{	
	//	가입 대기자 정보 업데이트
	m_pClanContext->UpdateClanSubscriber(rDeltaSeconds);
}

void UISubTabClanMgt_Join::_SortSubscriberList()
{
	//	가입 대기자 정렬 리스트 재구성
	INT32 nNewCount = m_pClanContext->GetMyClanSubscriberCount(); 

	if( nNewCount > MAX_REQUEST_COUNT ) 
	{
		I3TRACE("[StepClanMember] Clan subscribers are over count 100 [%d]\n", nNewCount); 

		nNewCount = MAX_REQUEST_COUNT;
	}

	m_vecManageSubscriber.clear();		
	m_vecManageSubscriber.reserve(nNewCount);

	for(INT32 i=0; i<nNewCount; i++)
	{
		CLAN_REQUEST_BASIC_INFO * pInfoSrc = m_pClanContext->GetMyClanSubscriber(i);
			
		if (pInfoSrc->_i64UID > 0)
		{
			// 여기서 경고오류 발생하면 i3엔진업데이트..(make_shared의 무항생성자에 대해서 new T()대신 new T변경)

			CLAN_REQUEST_BASIC_INFO_PTR info_ptr = i3::make_shared<CLAN_REQUEST_BASIC_INFO>();
			CLAN_REQUEST_BASIC_INFO* pInfoDest = info_ptr.get();
			i3mem::Copy(pInfoDest, pInfoSrc, sizeof(CLAN_REQUEST_BASIC_INFO));
			m_vecManageSubscriber.push_back(info_ptr);
		}
	}

	struct sort_fn
	{
		bool operator()(const CLAN_REQUEST_BASIC_INFO_PTR& lhs, const CLAN_REQUEST_BASIC_INFO_PTR& rhs) const
		{
			return lhs->_date > rhs->_date;			// 내림차순인듯 하다..
		}
	};

	i3::sort(m_vecManageSubscriber.begin(), m_vecManageSubscriber.end(), sort_fn());
}


void UISubTabClanMgt_Join::_UpdateSubscriberList(void)
{
	_SortSubscriberList();

	INT32 nCount = m_pClanContext->GetMyClanSubscriberCount();

	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		if (nCount < 9)
			nCount = 9;
	}
	else
	{
		if (nCount < 8)
			nCount = 8;
	}

	LuaState * L = _CallLuaFunction("ClearList");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, nCount);
		_EndLuaFunction( L, 1);
	}

	const INT32 count = (INT32)m_vecManageSubscriber.size();

	for(INT32 i = 0; i < nCount; i++)
	{
		if(i < (INT32)count)
		{
			CLAN_REQUEST_BASIC_INFO* pSubscriber = m_vecManageSubscriber[i].get();

			if( pSubscriber != nullptr)
			{
				if( pSubscriber->_i64UID > 0)
					_SetSlotData(i, pSubscriber);
				else
					_SetSlotData(i , nullptr);	
			}
			else
				_SetSlotData(i , nullptr);	
		}
		else
		{
			_SetSlotData(i , nullptr);		
		}
	}
}

void UISubTabClanMgt_Join::_SetSlotData(INT32 idx, CLAN_REQUEST_BASIC_INFO* pSubscriber)
{
	LuaState * L = _CallLuaFunction("SetJoinListInfo");

	if(pSubscriber != nullptr)
	{
		INT32 i32KillRate = GameContextUtil::getKillRate(pSubscriber->m_ui32Kill, pSubscriber->m_ui32Death);
		i3::stack_wstring wstrKDRate;
		
		i3::sprintf(wstrKDRate, L"%d%%", i32KillRate);

		i3Lua::PushInteger(L , idx);
		i3Lua::PushInteger(L , pSubscriber->_rank);
		i3Lua::PushStringUTF16To8(L , pSubscriber->_nick);
		i3Lua::PushStringUTF16To8(L , wstrKDRate);
		i3Lua::PushStringUTF16To8(L, pSubscriber->m_strContents);
		i3Lua::PushStringUTF16To8(L , L"0");
		i3Lua::PushUserData(L , (void*)CbSetSlotAcceptBtn);
		i3Lua::PushStringUTF16To8(L , L"0");
		i3Lua::PushUserData(L , (void*)CbSetSlotRejectBtn);
	}
	else
	{
		i3Lua::PushInteger(L , idx);
		i3Lua::PushInteger(L , -1);
		i3Lua::PushStringUTF16To8(L , L"");
		i3Lua::PushStringUTF16To8(L , L"");
		i3Lua::PushStringUTF16To8(L , L"");
		i3Lua::PushStringUTF16To8(L , L"-1");
		i3Lua::PushUserData(L , nullptr);
		i3Lua::PushStringUTF16To8(L , L"-1");
		i3Lua::PushUserData(L , nullptr);
	}

	_EndLuaFunction(L , 9);
}

/*virtual*/ void UISubTabClanMgt_Join::OnCreate(i3GameNode * pParent)
{
	UISubTabClan_Mgt::OnCreate(pParent);
	//LoadScene
	AddScene("Scene/Clan/PointBlankRe_Clan_Management_Join.i3UIs", UISubTabClanMgt_Join_Glue);
}

/*virtual*/ void UISubTabClanMgt_Join::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	UISubTabClan_Mgt::ProcessGameEvent(event,arg, UserData);
	if( event == EVENT_CLAN_MEMBER_LIST_N)
	{
		if(SUCCEEDED(arg))
		{
			//	맴버 리스트 재구성
			_UpdateSubscriberList();
		}
	}
}

/*virtual*/ void UISubTabClanMgt_Join::OnUpdate(REAL32 rDeltaSeconds)
{
	UISubTabClan_Mgt::OnUpdate(rDeltaSeconds);
	_OnUpdateSubscriber(rDeltaSeconds);
}

/*virtual*/ void UISubTabClanMgt_Join::OnEntranceEnd(void)
{
	UISubTabClan_Mgt::OnEntranceEnd();
}

/*virtual*/ bool UISubTabClanMgt_Join::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UISubTabClan_Mgt::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_UpdateSubscriberList();

	return true;	
}

/*virtual*/ void UISubTabClanMgt_Join::OnLoadAllScenes()
{
	UISubTabClan_Mgt::OnLoadAllScenes();
	m_vecManageSubscriber.clear();
	m_pClanContext->EnterClanSubscriber();
	if( g_pEnvSet->IsBaseOnUIVersion_1_0() )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_JoinList");
		pBox->getHeader()->SetHeaderFont("", 8 );
	}
	else if( g_pEnvSet->m_nUIVersion == UFV_1_1 )
	{
		i3UIListView_Box* pBox = (i3UIListView_Box*)GetScene(0)->FindChildByName("ListView_JoinList");
		pBox->getHeader()->SetHeaderFont("", 9 );
	}
}


void UISubTabClanMgt_Join::OnJoinAccept(INT32 i32Slot)
{
	if(m_pClanContext->IsEnter() == false)
		return;

	INT32 i32Count = (INT32)m_vecManageSubscriber.size();

	if(i32Slot >= i32Count)
		return;

	if( false == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) ) 
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

	}
	else
	{
		CLAN_REQUEST_BASIC_INFO * pSubscriber = m_vecManageSubscriber[i32Slot].get();
		m_pClanContext->ProcessIntromissionMember(&pSubscriber->_i64UID, 1);
	}
}

void UISubTabClanMgt_Join::OnJoinReject(INT32 i32Slot)
{
	if(m_pClanContext->IsEnter() == false)
		return;

	INT32 i32Count = (INT32)m_vecManageSubscriber.size();

	if(i32Slot >= i32Count)
		return;

	if( false == m_pClanContext->CheckMenagementAuth( m_pClanContext->GetMyStaffLevel(), CLAN_MENAGE_TYPE_ACCEPT ) )
	{
		//	"해당 기능에 대한 스태프 권한이 설정되어 있지 않습니다."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_MODIFY_NO_STAFF_FAIL"));

	}
	else
	{
		CLAN_REQUEST_BASIC_INFO* pSubscriber = m_vecManageSubscriber[i32Slot].get();
		m_pClanContext->ProcessRejectionMember(&pSubscriber->_i64UID, 1);
	}
}

void UISubTabClanMgt_Join::CbSetSlotAcceptBtn(i3UIListView_Cell * pCell, void * szData)
{
	SetSlotBtnData(pCell, GAME_RCSTRING("STBL_IDX_CLAN_SUBSCRIBER_INTROMISSION"));
}

void UISubTabClanMgt_Join::CbSetSlotRejectBtn(i3UIListView_Cell * pCell, void * szData)
{
	SetSlotBtnData(pCell, GAME_RCSTRING("STBL_IDX_CLAN_SUBSCRIBER_REJECTION"));
}

void UISubTabClanMgt_Join::SetSlotBtnData(i3UIListView_Cell * pCell, const i3::rc_wstring& wstrText)
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

		pCtrl->SetText(wstrText);

		if( g_pEnvSet->IsBaseOnUIVersion_1_0() == false )
		{
			COLOR color;
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);

			i3Color::Set(&color, (UINT8)6, 82, 149, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, color);

			i3Color::Set(&color, (UINT8)0, 51, 96, 255 ); 
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, color);
		}
		else
		{
			COLOR color;
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_IDLE, color);
		
			i3Color::Set(&color, (UINT8)205, 205, 207, 255);
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_ONMOUSE, color);

			i3Color::Set(&color, (UINT8)205, 205, 207, 255 ); 
			pCtrl->SetButtonTextColor(i3UIButtonComposed3::I3UIBUTTONCOMPOSED3_CR_SELECTED, color);
		}
		pCtrl->SetFont(pCtrl->getFontName(),STAFF_BUTTON_FONTSIZE);
	}
}
