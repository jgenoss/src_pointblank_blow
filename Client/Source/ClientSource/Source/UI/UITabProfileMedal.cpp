#include 	"pch.h"
#include 	"UITabProfileMedal.h"

#include 	"UIMainFrame.h"
#include 	"UIPhaseProfile.h"
#include	"MedalManager.h"
#include	"ItemImageMgr.h"
#include	"UIShopFunction.h"
#include	"i3Base/itl/algorithm/sort.h"
#include	"i3Base/string/stack_string.h"
#include	"i3Base/string/ext/sprintf.h"
#include	"UI/UIUtil.h"
#include "UserInfoContext.h"
#include "MedalContext.h"
#include "UI/UIBlink.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

static UITabProfileMedal* GetThis( void)
{
	UIPhaseProfile* pProfile = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
	if( pProfile != nullptr)
		return static_cast<UITabProfileMedal*>(pProfile->GetTab( UIPhaseProfile::TAB_MEDAL ));
	return nullptr;
}

extern "C" {
	int PhaseProfileMedal_ChangeTab(LuaState * L)
	{
		INT32 tab = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeTab( (MEDALTAB) tab );
		return 0;
	}
	int PhaseProfileMedal_SelectSort(LuaState * L)
	{
		INT32 sorttype = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeSortType(sorttype);
		return 0;
	}
	int PhaseProfileMedal_ClickLevel(LuaState * L)
	{
		INT32 level = i3Lua::GetIntegerArg( L, 1 );
		bool bEvent = (i3Lua::GetBooleanArg( L, 2 )==true)?true:false;
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeLevel(level, bEvent);
		return 0;
	}
	int PhaseProfileMedal_GetReward(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->GetReward(idx);
		return 0;
	}
	int PhaseProfileMedal_NotifySelected(LuaState * L)
	{
		INT32 selected = i3Lua::GetIntegerArg( L, 1 );
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->NotifySelected(selected);
		return 0;
	}
	int PhaseProfileMedal_NotifySelectedClear(LuaState * L)
	{
		INT32 selected = i3Lua::GetIntegerArg( L, 1 );
		if(selected >= 0 && selected < MAX_NOTIFY_MADAL_COUNT)
		{
			UITabProfileMedal * pTab = GetThis();
			if( pTab != nullptr)
				pTab->NotifySelectedClear(selected);
		}
		return 0;
	}
	int PhaseProfileMedal_NotifyAllClear(LuaState * L)
	{
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->NotifyAllClear();
		return 0;
	}
	int MedalFrame_Wheel(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->UpdateScroll(idx);
		return 0;
	}
	int PhaseProfileMedal_PushNotifyMedal(LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->_PushNotifyMedal(idx);
		return 0;
	}
	int PhaseProfileMedal_MoveRewardPage(LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->MoveRewardPage(idx);
		return 0;
	}
	int PhaseProfileMedal_ChangeRewardPage(LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->ChangeRewardPage(idx);
		return 0;
	}
	int PhaseProfileMedal_AllGetReward(LuaState* L)
	{
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->AllGetReward();
		return 0;
	}
	int PhaseProfileMedal_SetMedalScrollFocus(LuaState* L)
	{
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SetMedalScrollFocus();
		return 0;
	}
	int PhaseProfileMedal_SelectedMedalSlot(LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SelectedMedalSlot(idx);
		return 0;
	}
	int	PhaseProfileMedal_OpenToolTip(LuaState* L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1, -1);
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->SetRewardToolTip(idx);
		return 0;
	}
	int	PhaseProfileMedal_CloseToolTip(LuaState* L)
	{
		UITabProfileMedal * pTab = GetThis();
		if( pTab != nullptr)
			pTab->CloseRewardToolTip();
		return 0;
	}
}

LuaFuncReg PhaseProfileMedal_Glue[] =
{
	{"ChangeTab",			PhaseProfileMedal_ChangeTab				},		// 메달 탭 클릭
	{"SelectSortType",		PhaseProfileMedal_SelectSort			},		// 메달 정렬 타입 선택
	{"ClickLevel",			PhaseProfileMedal_ClickLevel			},		// 메달 레벨 클릭
	{"GetReward",			PhaseProfileMedal_GetReward				},		// 메달 클리어 보상
	{"AllGetRewardMedal",	PhaseProfileMedal_AllGetReward			},
	{"NotifySelected",		PhaseProfileMedal_NotifySelected		},		// 메달 알리미 선택
	{"NotifySelectedClear",	PhaseProfileMedal_NotifySelectedClear	},		// 메달 알리미 선택 해제
	{"NotifyAllClear",		PhaseProfileMedal_NotifyAllClear		},		// 메달 알리미 전부 해제
	{"MedalFrame_Wheel",	MedalFrame_Wheel			},		// 메달 스크롤 이벤트
	{"PushNotifyMedal",		PhaseProfileMedal_PushNotifyMedal		},
	{"MoveRewardPage",		PhaseProfileMedal_MoveRewardPage		},		// 보상 메달 리스트 페이지 변경
	{"ChangeRewardPage",	PhaseProfileMedal_ChangeRewardPage		},
	{"SetMedalScrollFocus",	PhaseProfileMedal_SetMedalScrollFocus	},
	{"SelectedMedalSlot",	PhaseProfileMedal_SelectedMedalSlot		},
	{"OpenToolTip",			PhaseProfileMedal_OpenToolTip			},
	{"CloseToolTip",		PhaseProfileMedal_CloseToolTip			},
	{nullptr, nullptr}
};

extern "C"
{
	// 슬롯 클릭 이벤트
	int Click_MedalSlot(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UITabProfileMedal * pThis = GetThis();
		if( pThis != nullptr)
			pThis->Click_MedalSlot(idx);
		return 0;
	}

	// 더블 클릭(현재는 기능에 없음)
	int DoubleClick_MedalSlot(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UITabProfileMedal * pThis = GetThis();
		if( pThis != nullptr)
			pThis->DoubleClick_MedalSlot(idx);
		return 0;
	}

	int Check_MedalSlot(LuaState * L)
	{
		INT32 idx		= i3Lua::GetIntegerArg(L, 1);
		INT32 checked	= i3Lua::GetIntegerArg(L, 2);
		UITabProfileMedal * pThis = GetThis();
		if( pThis != nullptr)
			pThis->Check_MedalSlot(idx, (checked < 0) ? true:false);
		return 0;
	}

	//마우스 Wheel 이벤트
	int Wheel_MedalSlot(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UITabProfileMedal * pThis = GetThis();
		if( pThis != nullptr)
			pThis->UpdateWheel(idx);
		return 0;
	}

};

LuaFuncReg PhaseProfileMedalSlot_Glue[] =
{
	{ "Click_MedalSlot",		Click_MedalSlot			},
	{ "DoubleClick_MedalSlot",	DoubleClick_MedalSlot	},
	{ "Check_MedalSlot",		Check_MedalSlot			},
	{ "MedalSlot_Wheel",		Wheel_MedalSlot			},
	{ nullptr, nullptr }
};

I3_CLASS_INSTANCE( UITabProfileMedal);//, UITabProfileBase );

UITabProfileMedal::UITabProfileMedal() //: m_pBlinkLVButtons(nullptr)
{
	m_eTab = TAB_NONE;
	::memset(m_apImgSetCtrl, 0, sizeof(UIImgSetCtrl*) * Medal_SLOT_TOTAL );
	::memset(m_pMedalDetailButton, 0, sizeof(i3UIButtonComposed3*) * 4 );
	m_viewIdx = -1;
	m_selectedIdx = -1;
	m_slotCallback.SetObj(this);	
	//m_SlotSet.SetCallback(&m_slotCallback);
	//m_SlotSet.SetEnableMouseOver(true);
	m_pMedalScroll = nullptr;
	m_pComboBox = nullptr;
	m_pRewardToolTip = nullptr;
	m_gameEventLevel = 0;
	m_CurPos = 0;
	m_OldPos = -1;
	m_SelectedSlotIdx = -1;
	m_bScrollFirst = true;
	m_RewardMedalPage = 0;
	m_RewardMedalCurPage = 1;
	m_RewardMedalCount = 0;
	m_pviewList = CMedalManager::i()->GetViewList();
	m_prewardList = CMedalManager::i()->GetRewardList();
	InitViewList();
}

UITabProfileMedal::~UITabProfileMedal()
{
}

void	UITabProfileMedal::OnCreate( i3GameNode * pParent )
{
	UITabProfileBase::OnCreate( pParent );
	AddScene( "Scene/Main/PointBlankRe_Profile_Medal.i3UIs", PhaseProfileMedal_Glue );	// 메달 MainLua 등록

	// 슬롯셋 클래스 크기설정(행*렬)
	//m_SlotSet.OnCreate(this, MEDAL_SLOT_COL, MEDAL_SLOT_ROW);						// 디파인 변경으로 가로*세로 크기 변경 가능
	//m_SlotSet.RegisterSlotScene("Scene/Main/PointBlankRe_Profile_Medal_Slot.i3UIs", PhaseProfileMedalSlot_Glue, true);	//메달 슬롯Lua 등록
}

void	UITabProfileMedal::OnLoadAllScenes()
{
	UITabProfileBase::OnLoadAllScenes();

	// 슬롯셋 부모 프레임 등록
	//LuaState* L = UILua::CallLuaFunction(GetScene(0), "GetSlotDummy");
	//i3UIControl* dummy = reinterpret_cast<i3UIControl*>(UILua::ReturnLuaPointer(L, 0));
	//m_SlotSet.OnLoadAllScenes( dummy );

	m_pMedalScroll	= (i3UIScrollBar*)GetScene(0)->FindChildByName("i3UIMedal_Scroll");
	I3ASSERT( nullptr != m_pMedalScroll );

	m_pComboBox		= (i3UIComboBox*)GetScene(0)->FindChildByName("i3UIMedal_Combo");
	I3ASSERT( nullptr != m_pComboBox );

	m_pRewardToolTip	= (i3UIToolTip*)GetScene(0)->FindChildByName("RewardToolTip");
	I3ASSERT( nullptr != m_pRewardToolTip );

	m_pMedalDetailButton[0] = (i3UIButtonComposed3*)GetScene(0)->FindChildByName("MedalDetailInfoButton0");
	I3ASSERT( nullptr != m_pMedalDetailButton[0] );

	m_pMedalDetailButton[1] = (i3UIButtonComposed3*)GetScene(0)->FindChildByName("MedalDetailInfoButton1");
	I3ASSERT( nullptr != m_pMedalDetailButton[1] );

	m_pMedalDetailButton[2] = (i3UIButtonComposed3*)GetScene(0)->FindChildByName("MedalDetailInfoButton2");
	I3ASSERT( nullptr != m_pMedalDetailButton[2] );

	m_pMedalDetailButton[3] = (i3UIButtonComposed3*)GetScene(0)->FindChildByName("MedalDetailInfoButton3");
	I3ASSERT( nullptr != m_pMedalDetailButton[3] );

	//m_pBlinkLVButtons = new UIBlinkLVButtons(GetScene(0));
	//MEMDUMP_NEW( m_pBlinkLVButtons, sizeof(UIBlinkLVButtons));
}

void	UITabProfileMedal::OnUnloadAllScenes()
{
	UITabProfileBase::OnUnloadAllScenes();

	m_pMedalScroll = nullptr;
	m_pComboBox = nullptr;
	m_pRewardToolTip = nullptr;

	//I3_SAFE_DELETE( m_pBlinkLVButtons);
}

bool	UITabProfileMedal::OnEntranceStart( void * pArg1, void * pArg2 )
{
	if( false == UITabProfileBase::OnEntranceStart( pArg1, pArg2 ) )
	{
		return false;
	}

	m_viewIdx = -1;
	m_selectedIdx = -1;
	m_RewardMedalCurPage = 1;
	m_SelectedSlotIdx = -1;
	m_CurPos = 0;
	CMedalManager::i()->SetSortType(MEDAL_SORT_DEFAULT);

	CMedalManager::i()->setWaitResult(false);

	InitTab();
	InitNotify();
	InitSortType();

	ChangeTab(TAB_BASIC);
	UpdateRewardMedalList();

	LuaState * L = _CallLuaFunction("__init");
	if (L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}

	return true;
}

void	UITabProfileMedal::OnEntranceEnd()
{
	UITabProfileBase::OnEntranceEnd();

	static_cast<UIPhaseProfile *>(GetMainFrame()->GetPopup(UPW_PROFILE))->setEscape(true);
	//m_SlotSet.SetVisible(true);

	m_RewardToolTipHeight = getY(m_pRewardToolTip->getPos());
	m_pRewardToolTip->setPos( getX( m_pRewardToolTip->getPos()) + 40.f, m_RewardToolTipHeight ); 

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

bool	UITabProfileMedal::OnExitStart()
{
	bool bRes = UITabProfileBase::OnExitStart();

	if( bRes )	
		ChangeTab(TAB_NONE);
	
	GameEventSender::i()->SetEvent(EVENT_MEDAL_NOTIFY_REGISTER, (void *)m_NotifyInfo);
	return bRes;
}

void	UITabProfileMedal::OnExitEnd()
{
	UITabProfileBase::OnExitEnd();
}

void	UITabProfileMedal::OnUpdate(REAL32 rDeltaSeconds)
{
	UITabProfileBase::OnUpdate(rDeltaSeconds);
	if(m_pComboBox != nullptr && m_pComboBox->isListBoxUnfolded() == false)
	{
	//	m_SlotSet.OnUpdate();
	}

	// 레벨업 체크
	if( 0 < UserInfoContext::i()->GetRankUpRank())
	{
		GetMainFrame()->OpenPopup(UPW_RANK_UP);
		UserInfoContext::i()->ResetRankUp();
	}

//	if (m_pBlinkLVButtons)
	//	m_pBlinkLVButtons->OnUpdate(rDeltaSeconds);

	CMedalManager::i()->UpdateRewardResultWait(rDeltaSeconds);
}

void	UITabProfileMedal::ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_MEDAL_GET_ALL_REWARD :
		{
			if( CMedalManager::i()->getMedalRewardResultCount() > 0)
			{
				for (INT32 i = 0; i < CMedalManager::i()->getMedalRewardResultCount(); i++)
				{
					const MedalRewardResult * pResult = CMedalManager::i()->getMedalRewardResult(i);

					if( pResult->_success )
					{
						//GetMainFrame()->OpenPopup( UPW_MEDAL_REWARD );
						//UpdateMedalInfo(m_gameEventLevel);
						const MedalCompleteInfo info = CMedalManager::i()->getMedalRewardResult(i)->_info;
						CMedalManager::i()->PopRewardMedal(&info);
					}
					else
					{
						GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_QUEST_REWARD_GETERROR"));
					}
				}
				UpdateRewardMedalList();

				CMedalManager::i()->EndRewardResultWait();
				
				// 보상 획득 요청을 보냈던 메달 정보가 모두 제거되면
				// 보상을 다 받은 것으로 알림 팝업을 띄운다
				if (CMedalManager::i()->getRewardMedalListCount() < 1)
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_GIFT_TAKEALL"));
				
			}
		}
		break;
	case EVENT_MEDAL_GET_REWARD :
		{
			if( CMedalManager::i()->getMedalRewardResultCount() > 0)
			{
				const MedalRewardResult * pResult = CMedalManager::i()->getMedalRewardResult(0);

				if( pResult->_success )
				{
					GetMainFrame()->OpenPopup( UPW_MEDAL_REWARD );
					//UpdateMedalInfo(m_gameEventLevel);
				}
				else
				{
					GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_QUEST_REWARD_GETERROR"));
				}
			}
			UpdateRewardMedalList();

			CMedalManager::i()->EndRewardResultWait();
		}
		break;
	}
}

// 바꿀꺼 없을 듯
void	UITabProfileMedal::InitNotify()
{
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		NOTIFY_MEDAL * pMedal = CMedalManager::i()->getNotifyMedal(i);
		if(pMedal->m_ui8MedalType == MEDAL_TYPE_NORMAL)
			m_NotifyInfo[i].m_ui8MedalType = MEDAL_SET_TYPE_NORMAL;
		else
			m_NotifyInfo[i].m_ui8MedalType = (UINT8)CMedalManager::i()->getMedalSetTypeByIndex(pMedal->m_ui16Idx);

		m_NotifyInfo[i].m_ui16Idx		= pMedal->m_ui16Idx;
		m_NotifyInfo[i].m_ui16Count		= pMedal->m_ui16Count;
	}
	LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetNotifyMedal");
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		UINT8 _type = m_NotifyInfo[i].m_ui8MedalType;
		UINT16 _idx = m_NotifyInfo[i].m_ui16Idx;
		i3Lua::PushInteger(L, CMedalManager::i()->getImageIdx(_type, _idx));
	}
	i3Lua::PushInteger(L, -1);
	UILua::EndLuaFunction(L, 5);
}

void	UITabProfileMedal::InitTab()
{
	LuaState * L = UILua::CallLuaFunction(GetScene(0), "SetTabState");
	i3Lua::PushBoolean(L, true);
	i3Lua::PushBoolean(L, true);
	i3Lua::PushBoolean(L, true);
	//i3Lua::PushBoolean(L, false);

	//i3Lua::PushBoolean(L, CMedalManager::i()->hasNew(MEDAL_SET_TYPE_NORMAL));
	//i3Lua::PushBoolean(L, CMedalManager::i()->hasNew(MEDAL_SET_TYPE_EVENT));
	//i3Lua::PushBoolean(L, CMedalManager::i()->hasNew(MEDAL_SET_TYPE_DAILY));
	//i3Lua::PushBoolean(L, CMedalManager::i()->hasNew(MEDAL_SET_TYPE_CLAN));
	_EndLuaFunction(L, 3);
}

// 얘도 바꿀꺼 없을 듯
void	UITabProfileMedal::InitSortType()
{
	CMedalManager::i()->SetSortType(MEDAL_SORT_DEFAULT);
	i3::stack_wstring	wstrSort;
	i3::rc_wstring		wstr_sortName;

	for(INT32 i=0; i < MEDAL_SORT_MAX; i++)
	{
		CMedalManager::i()->GetSortTypeStr( wstr_sortName, (MEDAL_SORT_TYPE)i );
		wstrSort += wstr_sortName;
		wstrSort += L";";
	}

	LuaState * L = UILua::CallLuaFunction(GetScene(0), "SetMedalSortType");
	i3Lua::PushBoolean(L, true);
	i3Lua::PushStringUTF16To8(L, wstrSort);
	_EndLuaFunction(L, 2);
}

void	UITabProfileMedal::SetAchievementRate()
{
	i3::rc_wstring	wstrRateTitle;
	i3::rc_wstring	wstrRate;

	i3::sprintf(wstrRateTitle, GAME_STRING("STR_MEDAL_COUNT"));
	i3::sprintf(wstrRate, L" (%.1f", CMedalManager::i()->GetAchievementRate());
	wstrRateTitle += wstrRate;
	wstrRate = L"%)";
	wstrRateTitle += wstrRate;

	LuaState * L = _CallLuaFunction("SetAchievementRate");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrRateTitle);
		_EndLuaFunction(L, 1);
	}
}

/*void	UITabProfileMedal::InitLVButtonState()
{
	LuaState * L = UILua::CallLuaFunction(GetScene(0), "SetLVButtonState");
	i3Lua::PushInteger(L, m_eTab);
	_EndLuaFunction(L, 1);
}*/

void	UITabProfileMedal::InitScroll()
{
	m_pMedalScroll->EnableCtrl( true );
	m_pMedalScroll->setTickSize( 1 );

	if (m_pviewList->size() < 5)
		m_pMedalScroll->setScrollRange( 0, 1);
	else
		m_pMedalScroll->setScrollRange( 0, m_pviewList->size());
//	if(m_pviewList->size() % MEDAL_SLOT_ROW == 0)
//		m_pMedalScroll->setScrollRange( 0, m_pviewList->size() / MEDAL_SLOT_ROW);
//	else
//		m_pMedalScroll->setScrollRange( 0, (m_pviewList->size() / MEDAL_SLOT_ROW) + 1);
	m_pMedalScroll->setPageSize( 4 );
	m_pMedalScroll->setScrollPos( 0 );
}

void	UITabProfileMedal::ChangeTab(MEDALTAB eTab )
{
	if(eTab == TAB_NONE)
		return;
	m_eTab = eTab;
	m_bScrollFirst = true;
	m_SelectedSlotIdx = -1;
	//InitLVButtonState();
	UpdateMedalTab();
	CheckSlotEnable();
	Click_MedalSlot(0);
	setMedalTab((INT32)eTab);
	
	SetMedalScrollFocus();
}

void	UITabProfileMedal::ChangeSortType(INT32 sorttype)
{
	if( sorttype == CMedalManager::i()->GetSortType() )		return;
	CMedalManager::i()->SetSortType(sorttype);
	m_bScrollFirst = true;
	UpdateMedalTab();

	SetMedalScrollFocus();
}

void	UITabProfileMedal::ChangeLevel(INT32 level, bool inout)
{
	m_gameEventLevel = (UINT8)(level&0xFF);
	UpdateMedalInfo((UINT8)(level&0xFF), inout);
}

/*bool	UITabProfileMedal::SetSelIdx(INT32 idx)
{
	if(m_selectedIdx - getSlotRow() >= 0 && m_selectedIdx - getSlotRow() < Medal_SLOT_TOTAL)
	{
		//i3UIScene* scn = m_SlotSet.GetSlotScene(m_selectedIdx - getSlotRow());
		//if (scn)
	//	{
		//	LuaState* L = UILua::CallLuaFunction(scn, "SetSlotSelection");
		//	i3Lua::PushBoolean(L, false);
		//	UILua::EndLuaFunction(L, 1);
	//	}
	}

	m_viewIdx = -1;

	//INT32 active_slot_count	= m_SlotSet.GetActiveSlotCount();

	//if( idx >= active_slot_count)		// 유효하지 않으면 셀렉션true는 하지 않음..
	//	return false;

	if( (UINT32)idx < m_pviewList->size())
	{
		//LuaState* L = nullptr;
		m_viewIdx = idx;
		m_selectedIdx = m_viewIdx + getSlotRow();
		MedalViewInfo * pMedalInfo = (*m_pviewList)[m_selectedIdx];

		//i3UIScene* scn = m_SlotSet.GetSlotScene(idx);
		//if (scn)
		//{
		//	L = UILua::CallLuaFunction(scn, "SetSlotSelection");
		//	i3Lua::PushBoolean(L, true);
	//		UILua::EndLuaFunction(L, 1);
//		}

		pMedalInfo->setNew(false);

	//	L = UILua::CallLuaFunction(scn, "SetMedalNew");
//		i3Lua::PushBoolean(L, false);//
//		UILua::EndLuaFunction(L, 1);

	//	L = UILua::CallLuaFunction(GetScene(0), "SetMedalNewTab");
	//	i3Lua::PushInteger(L, (INT32)m_eTab);
	//	i3Lua::PushBoolean(L, CMedalManager::i()->hasNew(m_eTab));
	//	UILua::EndLuaFunction(L, 2);
	}
	return true;
}*/

void	UITabProfileMedal::MoveRewardPage(INT32 idx)
{
	// 페이지가 없거나 1개일 경우에는 페이지 이동이 없음
	if (m_RewardMedalPage == 1 || m_RewardMedalPage == 0)
		return;

	// Page Up
	if (idx == 1)
	{
		if(m_RewardMedalCurPage < m_RewardMedalPage)
		{
			m_RewardMedalCurPage += 1;
			SetRewardMedalPage();
		}
		else
			return;
	}
	// Page Down
	else if (idx == -1)
	{
		if(m_RewardMedalCurPage > 1)
		{
			m_RewardMedalCurPage -= 1;
			SetRewardMedalPage();
		}
		else
			return;
	}
	else
		I3TRACE("잘못된 값. 들어오면 안되는 값임...\n");

	SetRewardPageControl();
	SetMedalScrollFocus();
}

void	UITabProfileMedal::ChangeRewardPage(INT32 idx)
{
	// 총 Page가 1일 경우 바꿀 필요 없음
	if(m_RewardMedalPage == 1 || m_RewardMedalPage == 0)
		return;

	if (m_RewardMedalPage == 2)
	{
		if(idx == m_RewardMedalCurPage || m_RewardMedalPage < idx)
			return;

		if (idx == 1)
			m_RewardMedalCurPage = 1;
		else
			m_RewardMedalCurPage = 2;
			
		SetRewardMedalPage();
	}
	else
	{
		// 현재 Page가 1이거나 마지막일 경우 Select된 위치가 가운데가 아니기 때문에 따로 처리해야함
		if (m_RewardMedalCurPage == 1)
		{
			if (idx == 1)
				return;

			m_RewardMedalCurPage += (idx - 1);
			SetRewardMedalPage();
		}
		else if (m_RewardMedalCurPage == m_RewardMedalPage)
		{
			if (idx == 3)
				return;

			m_RewardMedalCurPage -= (3 - idx);
			SetRewardMedalPage();
		}
		else
		{
			if (idx == 2)
				return;

			m_RewardMedalCurPage += (idx - 2);
			SetRewardMedalPage();
		}
	}

	SetRewardPageControl();
	SetMedalScrollFocus();
}

void	UITabProfileMedal::SetRewardToolTip(INT32 idx)
{
	INT32 _MedalIndex = m_pMedalScroll->getScrollPos() + idx;

	m_selectedIdx = idx;

	MedalViewInfo * pMedalInfo = (*m_pviewList)[_MedalIndex];

	UpdateToolTip(idx);

	switch(idx)
	{
	case 0:
		if(pMedalInfo->medal_tabIdx == MEDAL_SET_TYPE_NORMAL)
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight - 18.f);
		else
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 3.f);
		break;
	case 1:
		if(pMedalInfo->medal_tabIdx == MEDAL_SET_TYPE_NORMAL)
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 44.f);
		else
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 102.f);
		break;
	case 2:
		if(pMedalInfo->medal_tabIdx == MEDAL_SET_TYPE_NORMAL)
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 92.f);
		else
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 201.f);
		break;
	case 3:
		if(pMedalInfo->medal_tabIdx == MEDAL_SET_TYPE_NORMAL)
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 150.f);
		else
			m_pRewardToolTip->setPos( getX(m_pRewardToolTip->getPos()),m_RewardToolTipHeight + 300.f);
		break;
	}

}

void	UITabProfileMedal::UpdateToolTip(INT32 idx)
{
	if (idx == -1)
		return;

	INT32 _MedalIndex = m_pMedalScroll->getScrollPos() + idx;

	MedalViewInfo * pMedalInfo = (*m_pviewList)[_MedalIndex];

	i3::rc_wstring wstrReward;
	i3::rc_wstring wstrTemp;

	INT32 _MedalMaxLevel = 4;
	if(pMedalInfo->medal_tabIdx != MEDAL_SET_TYPE_NORMAL)
		_MedalMaxLevel = 1;

	wstrReward = GAME_RCSTRING("STR_SHOP_MENU_ITEM_COMPENSATION");
	wstrReward += L"\n                                   \n";

	for(INT32 i = 0; i < _MedalMaxLevel; i++)
	{
		i3::sprintf(wstrTemp, L"Lv. %d\n", i + 1);
		wstrReward += wstrTemp;

		const MQF_REWARD_INFO * pRewardInfo = pMedalInfo->getReward( static_cast<UINT8>(i) );

		// 포인트
		if ( pRewardInfo->m_ui32RewardPoint != 0 )
		{
			i3::itoa(pMedalInfo->getReward((UINT8)i)->m_ui32RewardPoint, wstrTemp);
			wstrReward += wstrTemp;
			wstrReward += L" Point\n";
		}

		// 경험치
		if ( pRewardInfo->m_ui32RewardExp != 0 )
		{
			i3::itoa(pMedalInfo->getReward((UINT8)i)->m_ui32RewardExp, wstrTemp);
			wstrReward += wstrTemp;
			wstrReward += L" Exp";
		}

		// 아이템
		if ( pRewardInfo->m_ui32RewardItemID != 0)
		{
			wstrReward += L"\n";
			usf::GetShopItemName(wstrTemp, pRewardInfo->m_ui32RewardItemID );
			wstrReward += wstrTemp;
		}

		if(i != _MedalMaxLevel - 1)
			wstrReward += L"\n\n";
	}

	LuaState * L = _CallLuaFunction("SetRewardToolTip");
	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, wstrReward);
		_EndLuaFunction(L, 1);
	}
}

void	UITabProfileMedal::CloseRewardToolTip()
{
	LuaState * L = _CallLuaFunction("CloseRewardToolTip");
	if (L != nullptr)
	{
		_EndLuaFunction(L, 0);
	}
}

void	UITabProfileMedal::NotifySelected(INT32 selected)
{
	LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetNotifyMedal");
	
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		UINT8 _type = m_NotifyInfo[i].m_ui8MedalType;
		UINT16 _idx = m_NotifyInfo[i].m_ui16Idx;
		i3Lua::PushInteger(L, CMedalManager::i()->getImageIdx(_type, _idx));
	}
	
	i3Lua::PushInteger(L, selected);
	UILua::EndLuaFunction(L, 5);
}

void	UITabProfileMedal::NotifySelectedClear(INT32 selected)
{
	if( selected < 0 || selected >= MAX_NOTIFY_MADAL_COUNT) return;

	m_NotifyInfo[selected].m_ui8MedalType	= 0;
	m_NotifyInfo[selected].m_ui16Idx		= 0;
	m_NotifyInfo[selected].m_ui16Count		= 0;

	LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetNotifyMedal");
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		UINT8 _type = m_NotifyInfo[i].m_ui8MedalType;
		UINT16 _idx = m_NotifyInfo[i].m_ui16Idx;
		i3Lua::PushInteger(L, CMedalManager::i()->getImageIdx(_type, _idx));
	}
	i3Lua::PushInteger(L, -1);
	UILua::EndLuaFunction(L, 5);

	CheckSlotEnable();
	SetMedalScrollFocus();
	//m_SlotSet.UpdateList(Medal_SLOT_TOTAL, 0);
}

void	UITabProfileMedal::NotifyAllClear()
{
	for(INT32 i=0; i < MAX_NOTIFY_MADAL_COUNT; i++)
	{
		m_NotifyInfo[i].m_ui8MedalType	= 0;
		m_NotifyInfo[i].m_ui16Idx		= 0;
		m_NotifyInfo[i].m_ui16Count		= 0;
	}

	if(m_eTab == TAB_BASIC)
	{
		for(INT32 i=0; i< MEDAL_SLOT_COL*MEDAL_SLOT_ROW; i++)
		{
			//i3UIScene* scn = m_SlotSet.GetSlotScene(i);
			//if (scn)
			//{
			//	LuaState* L = UILua::CallLuaFunction(scn, "SetCheckBoxLock");
		//		i3Lua::PushBoolean(L, false);
	//			UILua::EndLuaFunction(L, 1);
//
//				L = UILua::CallLuaFunction(scn, "SetCheckBox");
//				i3Lua::PushInteger(L, -1);//
//				UILua::EndLuaFunction(L, 1);
//			}
		}
	}

	LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetNotifyMedal");
	i3Lua::PushInteger(L, -1);
	i3Lua::PushInteger(L, -1);
	i3Lua::PushInteger(L, -1);
	i3Lua::PushInteger(L, -1);
	i3Lua::PushInteger(L, -1);
	UILua::EndLuaFunction(L, 5);

	SetMedalScrollFocus();
}

void	UITabProfileMedal::AllGetReward()
{
	if(CMedalManager::i()->isWaitResult() )
		return;

	if(m_prewardList->size() == 0)
		return;

	INT32 packetcount = m_prewardList->size() / 20 + 1;
	UINT32 rewardcnt = 0;

	CMedalManager::i()->ClearMedalRewardResultList();

	for (INT32 cnt = 0; cnt < packetcount; cnt++)
	{
		PACKET_H_GET_MEDAL_REWARD_REQ	ReqHeader;

		ReqHeader.m_ui8Count = (UINT8)(m_prewardList->size() - rewardcnt);

		if (ReqHeader.m_ui8Count == 0)
			return;

		MEDAL_REWARD * pReqData = (MEDAL_REWARD*)i3MemAlloc(sizeof(MEDAL_REWARD) * ReqHeader.m_ui8Count);
		i3mem::FillZero( pReqData, sizeof(MEDAL_REWARD) * ReqHeader.m_ui8Count );

		for (INT32 _SendRewardCnt = 0; _SendRewardCnt < 20; _SendRewardCnt++)
		{
			if (rewardcnt == m_prewardList->size())
				continue;

			MedalViewInfo * pMedalInfo = (*m_prewardList)[rewardcnt];
			CMedalManager::i()->PushRewardMedal(*pMedalInfo);

			const MedalCompleteInfo * pInfo = CMedalManager::i()->getRewardMedal(rewardcnt);
			CMedalManager::i()->BuildMedalRewardPacket(&pReqData[_SendRewardCnt], pInfo);

			rewardcnt++;
		}

		GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_REWARD, &ReqHeader, pReqData);

		I3MEM_SAFE_FREE(pReqData);
	}

	UpdateRewardMedalList();

	CMedalManager::i()->StartRewardResultWait();
	CMedalManager::i()->ClearRewardMedalList();

	SetMedalScrollFocus();
}

void	UITabProfileMedal::GetReward(INT32 idx)
{
	if(CMedalManager::i()->isWaitResult() )
		return;

	INT32 selectedIndex = ((m_RewardMedalCurPage - 1) * 3) + idx;

	if( selectedIndex < 0 || (size_t)selectedIndex >= m_prewardList->size()) return;

	if( CMedalManager::i()->isWaitResult() )	
		return;

	// Medal Result List 초기화
	CMedalManager::i()->ClearMedalRewardResultList();
	
	MedalViewInfo * pMedalInfo = (*m_prewardList)[selectedIndex];

	MEDAL_SET_TYPE eType = static_cast<MEDAL_SET_TYPE>(pMedalInfo->medal_tabIdx);
	UINT16 ui16Idx = pMedalInfo->medal_Idx;
	MEDAL_LEVEL eLevel = static_cast<MEDAL_LEVEL>(pMedalInfo->getLevel()); // 서버에서는 1~4 까지 유효값

	// Reward Popup창에 같은 정보로 보여주기 위한 정보
	CMedalManager::i()->setCurrentRewardInfo(eLevel, pMedalInfo);

	MedalCompleteInfo CompleteInfo;
	CompleteInfo.SetMedalCompleteInfo(eType, ui16Idx, eLevel);

	PACKET_H_GET_MEDAL_REWARD_REQ PacketHeader;
	PacketHeader.m_ui8Count = 1;

	MEDAL_REWARD PacketData;
	// 패킷 데이터 설정 및 Medal Result List에 Push
	CMedalManager::i()->BuildMedalRewardPacket(&PacketData, &CompleteInfo);

	GameEventSender::i()->SetEvent(EVENT_MEDAL_GET_REWARD, &PacketHeader, &PacketData);
	I3PRINTLOG(I3LOG_NOTICE,"--SEND_MEDAL_REWARD--");

	UpdateRewardMedalList();

	CMedalManager::i()->StartRewardResultWait();

	SetMedalScrollFocus();
}

INT32	UITabProfileMedal::getSlotRow()
{
	INT32 count = 0;
	count = m_pMedalScroll->getScrollPos() * MEDAL_SLOT_ROW; 
	if(count < 0)
		count = 0;
	return count;
}

void	UITabProfileMedal::UpdateMedalInfo(UINT8 _lv, bool inout)
{
	LuaState * L = nullptr;
	if( m_selectedIdx < 0 || (size_t)m_selectedIdx >= m_pviewList->size()) return;

	MedalViewInfo * pMedalInfo = (*m_pviewList)[m_selectedIdx];
	UINT8 lv = 0;
	if(_lv == 0xff)
	{
		lv = pMedalInfo->getCurrentLevel();
		if(pMedalInfo->medal_tabIdx != MEDAL_SET_TYPE_NORMAL && _lv >= MEDAL_LEVEL_1)
			lv = MEDAL_LEVEL_NONE;
	}
	else
		lv = _lv;

	SetMedalPushed(lv);
	if(inout == true)
	{
		L = UILua::CallLuaFunction(GetScene(0), "SetLVPushed");
		if(MEDAL_LEVEL_MAX == lv)
			lv = MEDAL_LEVEL_MAX - 1;
		i3Lua::PushInteger(L, lv);
		_EndLuaFunction(L, 1);
	}

	UpdateBlinkingLVButtons();
}

void	UITabProfileMedal::UpdateBlinkingLVButtons()
{
	// 통으로 안쓰기 때문에 통으로 주석건다. (2015.03.24.수빈)
	/*
	MedalViewInfo* pMedalInfo = (*m_pviewList)[m_selectedIdx];

	for (UINT8 i=0; i<MEDAL_LEVEL_MAX; ++i)
	{
		INT32 curr_count = pMedalInfo->getMedalProcessCnt() - pMedalInfo->getLevelProcessCnt(i);
		INT32 goal_count = pMedalInfo->getLevelProcessCnt(i + 1) - pMedalInfo->getLevelProcessCnt(i);

		if(curr_count >= goal_count) 
			curr_count = goal_count;
		if(curr_count < 0) 
			curr_count = 0;
		if(goal_count < 0) 
			goal_count = 0;

	//	if (!pMedalInfo->isGetReward(i) && MedalContext::i()->GetReceivedMyMedal() && curr_count == goal_count)
	//		m_pBlinkLVButtons->Enable(static_cast<MEDAL_LEVEL>(i));
	//	else
	//		m_pBlinkLVButtons->Disable(static_cast<MEDAL_LEVEL>(i));
	}
	*/
}

void	UITabProfileMedal::SetMedalPushed(UINT8 _lv)
{
	if( m_selectedIdx < 0 || (size_t)m_selectedIdx >= m_pviewList->size()) return;

	MedalViewInfo * pMedalInfo = (*m_pviewList)[m_selectedIdx];
	i3::stack_wstring wstr_medalDescStr;
	i3::stack_wstring wstr_medalCntStr;

	i3::rc_wstring medalReward[MAX_MADAL_REWARD_COUNT_PER_LEVEL];

	UINT8 lv = 0;
	if(_lv == 0xff)
		lv = pMedalInfo->getCurrentLevel();
	else
		lv = _lv;
	INT32 begin_cnt= pMedalInfo->getMedalProcessCnt() - pMedalInfo->getLevelProcessCnt(lv);
	INT32 dest_cnt = 0;
	const MQF_REWARD_INFO * pReward = nullptr;
	if(lv == MEDAL_LEVEL_MAX)
	{
		dest_cnt = pMedalInfo->getLevelProcessCnt(lv);
		pReward = pMedalInfo->getReward(MEDAL_LEVEL_MAX-1);
	}
	else
	{
		dest_cnt = pMedalInfo->getLevelProcessCnt(lv + 1) - pMedalInfo->getLevelProcessCnt(lv);
		pReward = pMedalInfo->getReward(lv);
	}
	if(begin_cnt >= dest_cnt)
		begin_cnt = dest_cnt;
	if(begin_cnt < 0)
		begin_cnt = 0;

	i3::sprintf(wstr_medalDescStr, GAME_QUEST_RCSTRING(pMedalInfo->getMedalDesc()), dest_cnt);
	i3::sprintf(wstr_medalCntStr, L"%d/%d", begin_cnt, dest_cnt);

	// 기획의도에 따라 우선순위를 소유권, 마스터훈장, MQF파일 보상 순으로 출력합니다.
	if ( pMedalInfo->getRestrictionItem() != 0 &&
		 pReward->m_ui32RewardItemID == 0 &&
		 lv == MEDAL_LEVEL_MAX - 1 )			// 소유권
	{
		i3::rc_wstring wstrRestrictString;
		usf::GetShopItemName( wstrRestrictString, pMedalInfo->getRestrictionItem());

		i3::stack_wstring str = wstrRestrictString;
		str += L' ';
		str += GAME_RCSTRING("STR_MEDAL_QWEAPON_UNLOCK");
		medalReward[0] = str; 

	}
	else if (pReward->m_ui32RewardMaster > 0)	// 마스터훈장
	{
		i3::sprintf(medalReward[0], L"%s", GAME_RCSTRING("STR_MEDAL_MASTERMEDAL"));
	}
	else										// MQF 파일 보상
	{
		usf::GetShopItemName( medalReward[0], pReward->m_ui32RewardItemID );
	}

	if ( pReward->m_ui32RewardPoint > 0 )		// 0 이하일 경우엔 스트링 출력 x
	{
		i3::sprintf(medalReward[1], L"Point +%d", pReward->m_ui32RewardPoint);
	}

	if ( pReward->m_ui32RewardExp > 0 )			// 0 이하일 경우엔 스트링 출력 x
	{	
		i3::sprintf(medalReward[2], L"Exp +%d", pReward->m_ui32RewardExp);
	}

	LuaState * L = UILua::CallLuaFunction(GetScene(0), "SetMedalInfo");
	i3Lua::PushInteger(L, pMedalInfo->getImageIdx());
	i3Lua::PushBoolean(L, !pMedalInfo->isAcquired());
	i3Lua::PushStringUTF16To8(L, pMedalInfo->getMedalName());
	i3Lua::PushStringUTF16To8(L, wstr_medalDescStr);
	i3Lua::PushNumber(L, pMedalInfo->getLevelRate(lv));
	i3Lua::PushStringUTF16To8(L, wstr_medalCntStr);
	for(INT32 i=0; i<MAX_MADAL_REWARD_COUNT_PER_LEVEL; i++)
		i3Lua::PushStringUTF16To8(L, medalReward[i].c_str(), medalReward[i].size() );

	if (lv < MEDAL_LEVEL_MAX)
	{
		i3Lua::PushBoolean(L,
			!pMedalInfo->isGetReward(lv) &&
			MedalContext::i()->GetReceivedMyMedal() &&
			begin_cnt == dest_cnt
			);
	}
	else
	{
		i3Lua::PushBoolean(L, true);			// true던 false든 나도 모르겠다...어차피 BOUNDCHK대상 구현임..
		I3ASSERT(lv < MEDAL_LEVEL_MAX);
	}

	_EndLuaFunction(L, 10);

}

void	UITabProfileMedal::SetRewardMedalPage()
{
	for (UINT32 i = 0; i < 3; i++)
	{
		INT32 medal_index = ((m_RewardMedalCurPage - 1) * 3) + i;

		if (i < m_prewardList->size() - ((m_RewardMedalCurPage - 1) * 3) && m_RewardMedalCurPage != 0)
		{
			MedalViewInfo * pMedalInfo = (*m_prewardList)[medal_index];

			I3ASSERT(pMedalInfo != nullptr);

//			const MQF_REWARD_INFO * pReward = nullptr;

			i3::rc_wstring medalReward[MAX_MADAL_REWARD_COUNT_PER_LEVEL];

			const MQF_REWARD_INFO * pReward = pMedalInfo->getReward(pMedalInfo->getLevel() - 1);

//			if(pMedalInfo->getLevel() == MEDAL_LEVEL_MAX)
//				pReward = pMedalInfo->getReward(pMedalInfo->getLevel() - 1);
//			else
//				pReward = pMedalInfo->getReward(pMedalInfo->getLevel() - 1);

			// 기획의도에 따라 우선순위를 소유권, 마스터훈장, MQF파일 보상 순으로 출력합니다.
			if ( pMedalInfo->getRestrictionItem() != 0 &&
				pReward->m_ui32RewardItemID == 0 &&
				pMedalInfo->getLevel() == MEDAL_LEVEL_MAX - 1 )			// 소유권
			{
				i3::rc_wstring wstrRestrictString;
				usf::GetShopItemName( wstrRestrictString, pMedalInfo->getRestrictionItem());

				i3::stack_wstring str = wstrRestrictString;
				str += L' ';
				str += GAME_RCSTRING("STR_MEDAL_QWEAPON_UNLOCK");
				medalReward[0] = str; 

			}
			else if (pReward->m_ui32RewardMaster > 0)	// 마스터훈장
			{
				i3::sprintf(medalReward[0], L"%s", GAME_RCSTRING("STR_MEDAL_MASTERMEDAL"));
			}
			else										// MQF 파일 보상
			{
				usf::GetShopItemName( medalReward[0], pReward->m_ui32RewardItemID );
			}

			if ( pReward->m_ui32RewardPoint > 0 )		// 0 이하일 경우엔 스트링 출력 x
			{
				i3::sprintf(medalReward[1], L"Point +%d", pReward->m_ui32RewardPoint);
			}

			if ( pReward->m_ui32RewardExp > 0 )			// 0 이하일 경우엔 스트링 출력 x
			{	
				i3::sprintf(medalReward[2], L"Exp +%d", pReward->m_ui32RewardExp);
			}

			i3::stack_wstring wstrMedalName = pMedalInfo->getMedalName();
			i3::stack_wstring wstrLevel;
			i3::sprintf(wstrLevel, L" Lv. %d", pMedalInfo->getLevel());
			wstrMedalName += wstrLevel;

			LuaState* L = _CallLuaFunction("SetRewardMedalSlot");
			if (L != nullptr)
			{
				i3Lua::PushInteger(L, i+1);
				i3Lua::PushStringUTF16To8(L, wstrMedalName);
				i3Lua::PushInteger(L, pMedalInfo->getImageIdx());
				for(INT32 cnt=0; cnt < MAX_MADAL_REWARD_COUNT_PER_LEVEL; cnt++)
					i3Lua::PushStringUTF16To8(L, medalReward[cnt].c_str(), medalReward[cnt].size() );
				_EndLuaFunction(L, 6);
			}
		}
		else
		{
			LuaState* L = _CallLuaFunction("CloseRewardMedalSlot");
			if (L != nullptr)
			{
				i3Lua::PushInteger(L, i+1);
				_EndLuaFunction(L, 1);
			}
		}

	}

}

void	UITabProfileMedal::SetRewardMedalSlot(INT32 idx, INT32 rewardmedal)
{

}

void	UITabProfileMedal::SetRewardPageControl()
{
	LuaState* L = _CallLuaFunction("SetControlNumbering");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_RewardMedalPage);
		i3Lua::PushInteger(L, m_RewardMedalCurPage);
		_EndLuaFunction(L, 2);
	}
}

void	UITabProfileMedal::SelectedMedalSlot(INT32 idx)
{
	m_SelectedSlotIdx = m_pMedalScroll->getScrollPos() + idx;

	_SetDetailInfoSlot();

	SetMedalScrollFocus();
}

void	UITabProfileMedal::UpdateRewardMedalList()
{
	InitRewardList();
	CMedalManager::i()->MakeRewardList();
	SetAchievementRate();

	m_RewardMedalCount = m_prewardList->size();
	m_RewardMedalPage = m_RewardMedalCount/3;

	if (m_RewardMedalCount%3 > 0)
		m_RewardMedalPage += 1;

	if (m_RewardMedalPage < m_RewardMedalCurPage)
		m_RewardMedalCurPage = m_RewardMedalPage;

	LuaState* L = _CallLuaFunction("SetRewardMedalCount");
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, m_RewardMedalCount);
		_EndLuaFunction(L, 1);
	}

	SetRewardMedalPage();
	SetRewardPageControl();
}

void	UITabProfileMedal::UpdateMedalTab()
{
	m_viewIdx = -1;		// Selection 초기화
	m_selectedIdx = -1;
	InitViewList();
	MakeViewList();		// ViewList를 다시 구성(해당 탭과 카테고리에 따라...)
	InitScroll();		// ViewList 구성 갯수에 따라 스크롤 변경

	_SetDetailInfoSlot();
	//m_SlotSet.UpdateList(Medal_SLOT_TOTAL, 0);
	//UpdateMedalInfo();
	g_pFramework->SetFocus(m_pMedalScroll);
}

void	UITabProfileMedal::MakeViewList()
{
	CMedalManager::i()->MakeViewList(m_eTab);
}

void	UITabProfileMedal::OnLoadSlot(INT32 idx, i3UIScene* scn)
{
	if( scn != nullptr)
	{
		scn->setDepth(4);
		LuaState* L = UILua::CallLuaFunction(scn, "SetSlotIndex");
		i3Lua::PushInteger(L, idx);
		UILua::EndLuaFunction(L, 1);
	}
}

void	UITabProfileMedal::OnUnloadSlot(INT32 idx, i3UIScene* scn)
{
	if (m_apImgSetCtrl[idx])
	{
		m_apImgSetCtrl[idx]->RestoreSizePos();
		I3_SAFE_RELEASE(m_apImgSetCtrl[idx]);
	}
}

void	UITabProfileMedal::OnEnterSlot(INT32 idx, i3UIScene* scn)
{
	INT32 slotIdx = idx + getSlotRow();
	if(slotIdx == m_selectedIdx)
		return;

	if( scn != nullptr)
	{
		LuaState* L;
		L = UILua::CallLuaFunction(scn, "SetMedalMouseOver");
		i3Lua::PushBoolean(L, true);
		UILua::EndLuaFunction(L, 1);
	}

}

void	UITabProfileMedal::OnLeaveSlot(INT32 idx, i3UIScene* scn)
{
	INT32 slotIdx = idx + getSlotRow();
	if(slotIdx == m_selectedIdx)
		return;

	if( scn != nullptr)
	{
		LuaState* L;
		L = UILua::CallLuaFunction(scn, "SetMedalMouseOver");
		i3Lua::PushBoolean(L, false);
		UILua::EndLuaFunction(L, 1);
	}
}

void	UITabProfileMedal::OnEnabledSlot(INT32 idx, i3UIScene* scn)
{
	INT32 slotIdx = idx + getSlotRow();
	if(slotIdx < 0)
		return;

	if( scn != nullptr)
	{
		LuaState* L;

		L = UILua::CallLuaFunction(scn, "SetMedalSlotEnable");
		INT32 _size = m_pviewList->size();
		if(slotIdx >= _size)
		{
			i3Lua::PushBoolean(L, false );	
			UILua::EndLuaFunction(L, 1);
			return;
		}
		else
		{
			i3Lua::PushBoolean(L, true );	
			UILua::EndLuaFunction(L, 1);
		}

		MedalViewInfo * pMedalInfo = (*m_pviewList)[slotIdx];

		i3::stack_wstring wstr_conLv;
		UINT8 lv = pMedalInfo->getCurrentLevel();
		if(pMedalInfo->medal_tabIdx != MEDAL_SET_TYPE_NORMAL && lv >= MEDAL_LEVEL_1)
			lv = MEDAL_LEVEL_NONE;
		UINT8 lvStr = lv;
		
		if(lv < MEDAL_LEVEL_MAX)
			lvStr += 1;
		i3::sprintf(wstr_conLv, L"Lv.%d", lvStr);

		L = UILua::CallLuaFunction(scn, "GetSlotProgress");
		i3UIProgressBar* pProgress = reinterpret_cast<i3UIProgressBar*>(UILua::ReturnLuaPointer(L, 0));
		if(m_OldPos != m_CurPos)
			pProgress->setAnimate(true);
		else if(m_CurPos == 0 && pProgress->isAnimating())
			pProgress->setAnimate(true);
		else
			pProgress->setAnimate(false);

		L = UILua::CallLuaFunction(scn, "SetMedalSlotInfo");
		i3Lua::PushInteger(L, pMedalInfo->getImageIdx());
		i3Lua::PushBoolean(L, !pMedalInfo->isAcquired());
		if(m_OldPos != m_CurPos && m_CurPos != m_pMedalScroll->getScrollLimit() - 1 || m_bScrollFirst == true)
		{
			i3Lua::PushNumber(L, pMedalInfo->getLevelRate(lv));
		}
		else
			i3Lua::PushNumber(L, -1);
		i3Lua::PushStringUTF16To8(L, wstr_conLv);
		bool isNew = pMedalInfo->isNew();
		i3Lua::PushBoolean(L, isNew);
		i3Lua::PushInteger(L, isChecked(pMedalInfo->medal_tabIdx, pMedalInfo->medal_Idx) == true ? 1:0);
		UILua::EndLuaFunction(L, 6);

		bool bSelect = (slotIdx == m_selectedIdx);

		L = UILua::CallLuaFunction(scn, "SetSlotSelection");
		i3Lua::PushBoolean(L, bSelect );	
		UILua::EndLuaFunction(L, 1);

		// 일반 퀘스트만 알리미 등록 가능
		{
			L = UILua::CallLuaFunction(scn, "SetCheckBoxLock");
			if( pMedalInfo->medal_tabIdx == MEDAL_SET_TYPE_NORMAL)
				i3Lua::PushBoolean(L, false);
			else
				i3Lua::PushBoolean(L, true);
			UILua::EndLuaFunction(L, 1);
		}
	}

}

void	UITabProfileMedal::OnDisabledSlot(INT32 idx, i3UIScene* scn)
{
}

bool	UITabProfileMedal::isChecked(INT32 eTab, UINT16 idx)
{
	for(INT32 i=0; i < MAX_NOTIFY_MADAL_COUNT; i++){
		if(m_NotifyInfo[i].m_ui8MedalType == (UINT8)eTab &&
			m_NotifyInfo[i].m_ui16Idx == idx)
		{
			return true;
		}
	}
	return false;
}

void	UITabProfileMedal::CheckSlotEnable()
{
	if(m_eTab != TAB_BASIC)
		return;

	INT32 activeCount = 0;
	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	{
		if( m_NotifyInfo[i].m_ui16Idx != 0 )
			activeCount++;
	}

	if( activeCount == MAX_NOTIFY_MADAL_COUNT )
	{
		for(INT32 i=0; i< MEDAL_SLOT_COL*MEDAL_SLOT_ROW; i++)
		{
			//i3UIScene* scn = m_SlotSet.GetSlotScene(i);
			INT32 idx = getSlotRow()+i;

			if( (UINT32) idx >= m_pviewList->size())
				return;

		//	if(isChecked(m_eTab, (*m_pviewList)[idx]->medal_Idx))
		//	{
			//	LuaState* L = UILua::CallLuaFunction(scn, "SetCheckBoxLock");
			//	i3Lua::PushBoolean(L, false);
			//	UILua::EndLuaFunction(L, 1);
			//	continue;
		//	}
			//if (scn)
			//{
			//	LuaState* L = UILua::CallLuaFunction(scn, "SetCheckBoxLock");
			//	i3Lua::PushBoolean(L, true);
			//	UILua::EndLuaFunction(L, 1);
			//}
		}
	}
	else
	{
		for(INT32 i=0; i< MEDAL_SLOT_COL*MEDAL_SLOT_ROW; i++)
		{
			//i3UIScene* scn = m_SlotSet.GetSlotScene(i);
			INT32 idx = getSlotRow()+i;
			if( (UINT32) idx >= m_pviewList->size())
				return;
			if(isChecked(m_eTab, (*m_pviewList)[idx]->medal_Idx))
				continue;
			
			//if (scn)
			//{
		//		LuaState* L = UILua::CallLuaFunction(scn, "SetCheckBoxLock");
	//			if(CMedalManager::i()->isActivated(MEDAL_SET_TYPE_NORMAL, (*m_pviewList)[idx]->medal_viewIdx))
	//				i3Lua::PushBoolean(L, false);
	//			else
//					i3Lua::PushBoolean(L, true);/
//
//				UILua::EndLuaFunction(L, 1);
//			}
		}
	}
}

void	UITabProfileMedal::SetMedalScrollFocus()
{
	if( m_pMedalScroll )
	{
		i3UIManager* pMgr = i3UI::getManager();

		if( pMgr )
			pMgr->SetFocusControl( m_pMedalScroll );
	}
}

void	UITabProfileMedal::_SetDetailInfoSlot()
{
	for (UINT32 i = 0; i < MEDAL_DETAIL_INFO_MAX_SLOT; i++)
	{
		INT32 medal_index = m_CurPos + i;

		if (m_pviewList->size() > i)
		{
			i3::stack_wstring wstr_medalDescStr;
			i3::stack_wstring wstr_medalCntStr;

			MedalViewInfo * pMedalInfo = (*m_pviewList)[medal_index];

			INT32 dest_cnt = 0;
			UINT8 lv = pMedalInfo->getCurrentLevel();

			if(lv == MEDAL_LEVEL_MAX)
			{
				dest_cnt = pMedalInfo->getLevelProcessCnt(lv);
			}
			else
			{
				dest_cnt = pMedalInfo->getLevelProcessCnt(lv + 1) - pMedalInfo->getLevelProcessCnt(lv);
			}

			INT32 begin_cnt= pMedalInfo->getMedalProcessCnt() - pMedalInfo->getLevelProcessCnt(lv);

			if(begin_cnt >= dest_cnt || lv == MEDAL_LEVEL_MAX)
				begin_cnt = dest_cnt;
			if(begin_cnt < 0)
				begin_cnt = 0;

			i3::stack_wstring wstrMedalName = pMedalInfo->getMedalName();
			i3::stack_wstring wstrLevel;
			INT32 medal_level = (INT32)pMedalInfo->getCurrentLevel();
			
			bool quest_complete = false;
			if (medal_level >= (INT32)pMedalInfo->getMaxLevel())			//퀘스트가 완료 됬거나, 완료 되기 직전이다.
			{
				medal_level = (INT32)pMedalInfo->getMaxLevel();
				if ( dest_cnt == begin_cnt ) quest_complete = true;			//목표 카운트와 시작 카운트(진행도인듯) 같을 때 완료한 쾌스트로 판단한다.
			}
			else medal_level += 1;											//완료되지 않은 쾌스트 레벨 처리. +1 이유는 medal_level 이 0부터 시작되고 LV은 표시시 1로 시작.
			
			i3::sprintf(wstrLevel, L" Lv. %d", medal_level);
			wstrMedalName += wstrLevel;

			i3::sprintf(wstr_medalDescStr, GAME_QUEST_RCSTRING(pMedalInfo->getMedalDesc()), dest_cnt);

			if( quest_complete ) i3::sprintf(wstr_medalCntStr, L"%s", GAME_RCSTRING("STR_TBL_PROFILE_COMPLETE_TEXT") );
			else				 i3::sprintf(wstr_medalCntStr, L"%d/%d", begin_cnt, dest_cnt);

			LuaState* L  = _CallLuaFunction("SetMedalDetailInfoSlot");
			if ( L != nullptr)
			{
				i3Lua::PushInteger(L, i+1);
				i3Lua::PushInteger(L, pMedalInfo->getCurrentLevel());
				i3Lua::PushInteger(L, pMedalInfo->getImageIdx());
				i3Lua::PushStringUTF16To8(L, wstrMedalName);
				i3Lua::PushStringUTF16To8(L, wstr_medalDescStr);
				i3Lua::PushStringUTF16To8(L, wstr_medalCntStr);
				i3Lua::PushNumber(L, pMedalInfo->getLevelRate(pMedalInfo->getCurrentLevel()));
				if (medal_index == m_SelectedSlotIdx)
					i3Lua::PushBoolean(L, true);
				else
					i3Lua::PushBoolean(L, false);
				_EndLuaFunction(L, 8);
			}
		}
		else
		{
			LuaState* L = _CallLuaFunction("CloseMedalDetailInfoSlot");
			if ( L != nullptr)
			{
				i3Lua::PushInteger(L, i+1);
				_EndLuaFunction(L, 1);
			}
		}
	}
}

void	UITabProfileMedal::SetComboEditBoxText(INT32 sortType)
{
	LuaState * L = UILua::CallLuaFunction(GetScene(0), "SetComboEditBoxText");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, sortType);
		UILua::EndLuaFunction(L, 1);
	}
}

void UITabProfileMedal::setMedalTab(INT32 idx)
{
	LuaState * L = UILua::CallLuaFunction(GetScene(0), "setMedalTab");
	if(L != nullptr)
	{
		i3Lua::PushInteger(L, idx);
		UILua::EndLuaFunction(L, 1);
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////// 밑에부터 입력함수 입니다 ////////////////////////////
//////////////////////////////////////////////////////////////////////////

void	UITabProfileMedal::Click_MedalSlot(INT32 idx)
{
	//if(UITabProfileMedal::SetSelIdx(idx) == true)
	//{
	//	UpdateMedalInfo();
	//}
}

void	UITabProfileMedal::DoubleClick_MedalSlot(INT32 idx)
{
	// 안써용~아직은..;
}

void	UITabProfileMedal::_PushNotifyMedal(INT32 idx)
{
	for(INT32 i = 0; i < MAX_NOTIFY_MADAL_COUNT; i++)
	{

		for(INT32 j = 0; j < MAX_NOTIFY_MADAL_COUNT; j++)
		{
			if (m_NotifyInfo[i].m_ui8MedalType == (UINT8)m_eTab && m_NotifyInfo[i].m_ui16Idx == (*m_pviewList)[idx + m_CurPos]->medal_Idx)
				return;
		}

		if (m_NotifyInfo[i].m_ui8MedalType == 0 && m_NotifyInfo[i].m_ui16Idx == 0)
		{
			m_NotifyInfo[i].m_ui8MedalType	= (UINT8)m_eTab;
			m_NotifyInfo[i].m_ui16Idx		= (*m_pviewList)[idx + m_CurPos]->medal_Idx;
			m_NotifyInfo[i].m_ui16Count		= 0;
			break;
		}
	}

	LuaState* L = _CallLuaFunction("SetNotifyMedal");
	if( L != nullptr)
	{
		for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
		{
			UINT8 _type = m_NotifyInfo[i].m_ui8MedalType;
			UINT16 _idx = m_NotifyInfo[i].m_ui16Idx;
			i3Lua::PushInteger(L, CMedalManager::i()->getImageIdx(_type, _idx));
		}
		i3Lua::PushInteger(L, -1);
		_EndLuaFunction(L, 5);
	}

	SetMedalScrollFocus();
}

void	UITabProfileMedal::Check_MedalSlot(INT32 idx, bool checked)
{
	//INT32 active_slot_count	= m_SlotSet.GetActiveSlotCount();

	//if (idx <= -1 || idx >= active_slot_count)		// 유효하지 않으면 셀렉션true는 하지 않음..
	//	return;

	//i3UIScene* scn = m_SlotSet.GetSlotScene(idx);
	//if (scn)
	//{
	//	if(checked == true)		// UI 체크버튼을 눌렀을 경우.
	//	{
	//		for(INT32 i=0; i < MAX_NOTIFY_MADAL_COUNT; i++)
	//		{
	//			if(m_NotifyInfo[i].m_ui8MedalType == 0 && m_NotifyInfo[i].m_ui16Idx == 0)
	//			{
	//				m_NotifyInfo[i].m_ui8MedalType	= (UINT8)m_eTab;
	//				m_NotifyInfo[i].m_ui16Idx		= (*m_pviewList)[idx + getSlotRow()]->medal_Idx;
	//				m_NotifyInfo[i].m_ui16Count		= 0;
	//				break;
	//			}
	//		}
	//	}
	//	else					// UI 체크버튼을 풀었을 경우.
	//	{
	//		for(INT32 i=0; i < MAX_NOTIFY_MADAL_COUNT; i++)
	//		{
	//			if(m_NotifyInfo[i].m_ui8MedalType == (UINT8)m_eTab && m_NotifyInfo[i].m_ui16Idx == (*m_pviewList)[idx + getSlotRow()]->medal_Idx)
	//			{
	//				m_NotifyInfo[i].m_ui8MedalType	= 0;
	//				m_NotifyInfo[i].m_ui16Idx		= 0;
	//				m_NotifyInfo[i].m_ui16Count		= 0;
	//				break;
	//			}
	//		}
	//	}

	//	LuaState* L = UILua::CallLuaFunction(GetScene(0), "SetNotifyMedal");
	//	for(INT32 i=0; i<MAX_NOTIFY_MADAL_COUNT; i++)
	//	{
	//		UINT8 _type = m_NotifyInfo[i].m_ui8MedalType;
	//		UINT16 _idx = m_NotifyInfo[i].m_ui16Idx;
	//		i3Lua::PushInteger(L, CMedalManager::i()->getImageIdx(_type, _idx));
	//	}
	//	i3Lua::PushInteger(L, -1);
	//	UILua::EndLuaFunction(L, 5);
	//}
	CheckSlotEnable();
}


void	UITabProfileMedal::UpdateWheel(INT32 nDelta)
{
	if( m_pMedalScroll )
	{
		int nTerm	= 0 < nDelta ? -1 : 1;
		m_OldPos = m_pMedalScroll->getScrollPos();
		if(m_OldPos < 0)
			m_OldPos = 0;
		int nPos	= m_pMedalScroll->getScrollPos() + nTerm * m_pMedalScroll->getTickSize();
		if(nPos < 0)
			nPos = 0;
		m_CurPos = nPos;
		m_bScrollFirst = false;

		m_pMedalScroll->setScrollPos( nPos );
		g_pFramework->SetFocus(m_pMedalScroll);
	}

	UpdateScroll(m_CurPos);
}

void	UITabProfileMedal::UpdateScroll(INT32 idx)
{
	//m_SlotSet.UpdateList(Medal_SLOT_TOTAL, 0);

	I3TRACE("#ScrollPos : %d\n", idx);

	m_CurPos = idx;

	if (m_pMedalScroll)
	{
		if (m_CurPos > 0 && m_CurPos < m_pMedalScroll->getTickSize())
		{
			m_CurPos = m_pMedalScroll->getTickSize();
		}
	}

	UpdateToolTip(m_selectedIdx);

	//m_pMedalScroll->setScrollPos(m_pMedalScroll->getScrollPos() + m_CurPos);

	_SetDetailInfoSlot();

	//CheckSlotEnable();
}

/*UIBlinkLVButtons::UIBlinkLVButtons(i3UIScene* pScene) 
{
	const char* pLvBtnCtrlNames[MEDAL_LEVEL_MAX] = 
		{"i3UIButton_Lv1", "i3UIButton_Lv2", "i3UIButton_Lv3", "i3UIButton_Lv4"};

	for (INT32 i=0; i<MEDAL_LEVEL_MAX; i++)
	{
		m_pUIBlink[i] = new UIBlink(static_cast<i3UIControl*>(pScene->FindChildByName(pLvBtnCtrlNames[i])), 0.25f);
		MEMDUMP_NEW( m_pUIBlink[i], sizeof(UIBlink));
	}
}

UIBlinkLVButtons::~UIBlinkLVButtons() 
{
	for (INT32 i=0; i<MEDAL_LEVEL_MAX; i++)
		I3_SAFE_DELETE(m_pUIBlink[i]);
}

void UIBlinkLVButtons::OnUpdate(REAL32 tm)
{
	for (INT32 i=0; i<MEDAL_LEVEL_MAX; i++)
	{
		m_pUIBlink[i]->OnUpdate(tm);
	}
}

void UIBlinkLVButtons::Enable(const MEDAL_LEVEL lv)
{
	I3ASSERT(0 <= lv && lv < MEDAL_LEVEL_MAX);

	m_pUIBlink[lv]->Start();

	// 깜빡이는 다른 버튼과의 깜빡임 동기화.
	for (INT32 i=0; i<MEDAL_LEVEL_MAX; i++)
	{
		if (lv == i) continue;
		if (m_pUIBlink[i]->IsOn())
			m_pUIBlink[i]->Start();
	}
}

void UIBlinkLVButtons::Disable(const MEDAL_LEVEL lv)
{
	I3ASSERT(0 <= lv && lv < MEDAL_LEVEL_MAX);

	m_pUIBlink[lv]->Stop();
}*/