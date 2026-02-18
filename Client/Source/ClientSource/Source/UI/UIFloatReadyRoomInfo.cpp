#include "pch.h"
#include "UIFloatReadyRoomInfo.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../TextSet.h"
#include "../GameGUI.h"
#include "../StageBattle/AIModeContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#include "i3Base/string/ext/mb_to_utf16.h"

#include "ClientStageInfoUtil.h"
#include "RandomMapContext.h"
#include "ClientStageInfoContext.h"

namespace ReadyRoomInfo
{
	FloatingListCtrl::FloatingListCtrl()
	{
	}

	FloatingListCtrl::~FloatingListCtrl()
	{ 
		update_func_map.clear();
		change_func_map.clear();
	}

	void FloatingListCtrl::ClearListBox( INT32 cnt)
	{
		UIFloatReadyRoomInfo * pThis = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
		if( pThis != nullptr)
			pThis->ClearFloatingListBox( cnt);
	}

	void FloatingListCtrl::SetListBoxSlot( INT32 idx, const i3::wliteral_range& wNameRng)
	{
		UIFloatReadyRoomInfo * pThis = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
		if( pThis != nullptr)
			pThis->SetFloatingListBoxItem( idx, wNameRng);
	}

	void FloatingListCtrl::ChangeItem( INT32 idx)
	{
		if(idx >= (INT32)GetListSize())
			return;

		STAGE_MODE type = MyRoomInfoContext::i()->getStage()->GetStageMode();

		i3::vector_map<STAGE_MODE, change_func>::iterator itr = change_func_map.find( type);
		if( itr != change_func_map.end())
			(this->*(itr->second))( idx);
		else
			ChangeItem_Defatult( idx);
	}

	void FloatingListCtrl::ChangeItem_Defatult( INT32 idx)
	{
		ROOM_INFO_BASIC InfoBasic;
		MyRoomInfoContext::i()->getMyRoomInfo(&InfoBasic);

		UINT32 ui32SelStageID = (UINT32)GetListBoxValue(idx);
		if(InfoBasic._StageID != ui32SelStageID)
		{
			InfoBasic._StageID = ui32SelStageID;
			if(MyRoomInfoContext::i()->IsRandomMap())
			{
				InfoBasic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE(InfoBasic._InfoFlag, false);
			}
			GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOMINFO_N, &InfoBasic);
		}
	}

	void FloatingListCtrl::ChangeItem_Domi( INT32 idx)
	{
		ROOM_INFO_BASIC InfoBasic;
		MyRoomInfoContext::i()->getMyRoomInfo(&InfoBasic);

		/*if( InfoBasic._i16BeginRound != GetListBoxValue(idx))
		{
			InfoBasic._i16BeginRound = static_cast<INT16>(GetListBoxValue(idx));
			
			GameEventSender::i()->SetEvent( EVENT_CHANGE_ROOMINFO_N, &InfoBasic);
			//ACK 받은 후 방정보 세팅.
		}*/
	}




	void FloatingListCtrl::UpdateList()
	{
		STAGE_MODE type = MyRoomInfoContext::i()->getStage()->GetStageMode();

		i3::vector_map<STAGE_MODE, update_func>::iterator itr = update_func_map.find( type);
		if( itr != update_func_map.end())
			(this->*(itr->second))();
		else
			UpdateList_Default();
	}

	void FloatingListCtrl::UpdateList_Default()
	{
		m_List = CStageInfoUtil::GetStageList(MyRoomInfoContext::i()->getStage()->m_i32RuleIdx);
		
		size_t stage_count = 0;
		stage_count = m_List.GetStageListSize();

		if( stage_count < 10)
			stage_count = 10;

		ClearListBox( stage_count);

		i3::stack_wstring wstrBuff;

		for(size_t i = 0 ; i < stage_count ; ++i)
		{  
			if(i < m_List.GetStageListSize() )
				wstrBuff = CStageInfoUtil::GetMapData(GetListBoxValue(i)->m_ui32StageID)->getDisplayName();
			else
				wstrBuff.clear();

			SetListBoxSlot(i , wstrBuff);
		}
	}

	void FloatingListCtrl::UpdateList_Domi()
	{
		return;
	}
};

//------------------------------------------------------------------------------//
//									Lua Glue									//
//------------------------------------------------------------------------------//
extern "C" {

	//int UIFloatReadyRoomInfo_OpenMapList( LuaState * L)
	//{
	//	UIFloatReadyRoomInfo * pThis = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
	//	if( pThis != nullptr)
	//		pThis->ToggleMapListWnd();
	//	return 0;
	//}

	//int UIFloatReadyRoomInfo_ClickAnotherWnd( LuaState * L)
	//{
	//	i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(i3Lua::GetUserDataArg(L , 1));
	//	UIFloatReadyRoomInfo * pThis = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
	//	if( pThis != nullptr)
	//		pThis->CloseMapListWnd(pCtrl);
	//	return 0;
	//}

	//int UIFloatReadyRoomInfo_ChangeStage( LuaState * L)
	//{
	//	INT32 nSlotIdx = i3Lua::GetIntegerArg(L , 1);
	//	UIFloatReadyRoomInfo * pThis = static_cast<UIFloatReadyRoomInfo *>(GameUI::GetFloating( UFW_READYROOMINFO));
	//	if( pThis != nullptr)
	//		pThis->ChangeStage(nSlotIdx);
	//	return 0;
	//}
}

LuaFuncReg UIFloatReadyRoomInfo_Glue[] = {
	//{"OpenMapList",				UIFloatReadyRoomInfo_OpenMapList},
	//{"ClickAnotherWnd",			UIFloatReadyRoomInfo_ClickAnotherWnd},
	//{"ChangeStage",				UIFloatReadyRoomInfo_ChangeStage},
	{nullptr,						nullptr}
};

I3_CLASS_INSTANCE( UIFloatReadyRoomInfo);//, UIFloatingBase);

UIFloatReadyRoomInfo::UIFloatReadyRoomInfo()
{
	m_AlignedPos = UIFLOATING_POS_RIGHTBOTTOM;
	m_bFixedWindow = true;

	m_bOpenMapList = false;

#if defined( I3_DEBUG)
	SetName( "ReadyRoomInfo");
#endif

	RegisterGameEventFunc( EVENT_CHANGE_READYROOM_INFO,		&UIFloatReadyRoomInfo::ReceiveGameEvent_Update_MapInfo);
	RegisterGameEventFunc( EVENT_TRANSFER_HOST,				&UIFloatReadyRoomInfo::ReceiveGameEvent_Update_MapInfo);
	RegisterGameEventFunc( EVENT_M_RECEIVE_USER_SLOT,		&UIFloatReadyRoomInfo::ReceiveGameEvent_Update_MapInfo);
}

UIFloatReadyRoomInfo::~UIFloatReadyRoomInfo()
{
}

/*virtual*/ void UIFloatReadyRoomInfo::_InitializeAtLoad(i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	m_pMainWnd[0] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_MapInfo");
	//m_pMainWnd[1] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_ML_Btn");
	//m_pMainWnd[2] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_MapList");
	m_pMainWnd[1] = (i3UIFrameWnd*)pScene->FindChildByName("FrameWnd_Event");

}

bool UIFloatReadyRoomInfo::_BeHost(void)
{
	return BattleSlotContext::i()->getMainSlotIdx() == BattleSlotContext::i()->getMySlotIdx();
}

void UIFloatReadyRoomInfo::_ToggleMapListWnd( bool bOpenMapList)
{
	//LuaState * L = _CallLuaFunction("SetMapList");

	//if( L != nullptr)
	//{
	//	i3Lua::PushBoolean(L , bOpenMapList == true ? true : false);
	//	_EndLuaFunction(L , 1);

	//	_RemoveOnMouseState();
	//}

	if(bOpenMapList)
	{
		_SetFocusMapListWnd();
		
		//_UpdateMapList();
		m_FloatingList.UpdateList();
	}

	UINT32 ev = bOpenMapList ? UIEVENT_SET_FOCUS_POPUP : UIEVENT_KILL_FOCUS_POPUP;
	GetMainFrame()->OnEvent( ev, this, 0, I3_EVT_CODE_ACTIVATE);
}

void UIFloatReadyRoomInfo::_RemoveOnMouseState( void)
{
	//LuaState * L = _CallLuaFunction("GetMapListViewBox");

	//if( L != nullptr)
	//{
	//	i3UIListView_Box * pLVBox = reinterpret_cast<i3UIListView_Box *>(_ReturnLuaPointer(L , 0));

	//	I3ASSERT(pLVBox);

	//	i3UIListView_Item * pItem = nullptr;
	//	for(size_t i = 0 ; i < pLVBox->getItemList().size() ; ++i)
	//	{
	//		pItem = pLVBox->getItemList()[i];
	//		pItem->removeState(I3UI_STATE_ONMOUSE);
	//	}
	//}
}

void UIFloatReadyRoomInfo::_SetFocusMapListWnd( void)
{
	// 맵 리스트를 Open하는 경우에는 포커스를 해당 윈도우로 이동
	//LuaState *	L = _CallLuaFunction("GetMapListViewBox");

	//if( L != nullptr)
	//{
	//	i3UIControl * pCtrl = reinterpret_cast<i3UIControl*>(_ReturnLuaPointer(L, 0));
	//	g_pFramework->SetFocus(pCtrl);
	//}
}

/*virtual*/ void UIFloatReadyRoomInfo::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);

	// load scene
	AddScene( "Scene/Main/PointBlankRe_ReadyRoom_MapInfo.i3UIs", UIFloatReadyRoomInfo_Glue, true, false);
}

/*virtual*/ bool UIFloatReadyRoomInfo::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;	

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	m_bOpenMapList = false;

	UpdateMapInfo();
	
	_ToggleMapListWnd( false);

	return true;
}

/*virtual*/void	UIFloatReadyRoomInfo::OnEntranceEnd( void)
{
	UIFloatingBase::OnEntranceEnd();

	ROOM_INFO_BASIC Info;
	MyRoomInfoContext::i()->getMyRoomInfo(&Info);

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

/*virtual*/ bool UIFloatReadyRoomInfo::OnExitStart( void)
{
	if( UIFloatingBase::OnExitStart() == false)
		return false;

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	return true;
}

bool UIFloatReadyRoomInfo::OnRevive( i3RenderContext * pCtx)
{
	if( UIFloatingBase::OnRevive( pCtx) == false)
		return false;

	enum { Right_Dec = 8 };

	for(size_t i=0;i<UserListMaxFrm;++i)
	{
		VEC3D* pos = m_pMainWnd[i]->getPos(); VEC2D* size = m_pMainWnd[i]->getSize();
		m_pMainWnd[i]->setPos( (REAL32) (GameUI::AutoPosition::get_max_right() - size->x - Right_Dec), pos->y );
	}

	return true;
}

void UIFloatReadyRoomInfo::UpdateMapInfo( void)
{
	_UpdateMapInfoNormal();
}

void UIFloatReadyRoomInfo::_UpdateMapInfoNormal( void)
{
	LuaState * L;

	// 방정보를 불러온다
	ROOM_INFO_BASIC Info;
	ROOM_INFO_ADD	InfoAdd;

	if( IsReadyToUpdate() == false)return;

	MyRoomInfoContext::i()->getMyRoomInfo(&Info);
	MyRoomInfoContext::i()->getMyRoomInfoAdd(&InfoAdd);
	
	if( Info._StageID == 0)return;

	// 맵 이미지 인덱스 설정

	INT32 ImageIdx = 0;
	i3::rc_wstring wstrMapName;
	i3::stack_wstring wstrRoomInfo;
	bool bWeaponLimit = true;
	
	// 스테이지 설정
	{
		//맵 인덱스
		if (MyRoomInfoContext::i()->IsRandomMap())
		{
			ImageIdx = g_pFramework->GetRandomMapIdx();
			wstrMapName = GAME_RCSTRING("STR_POPUP_RANDOMMAP");
		}
		else
		{
			ImageIdx = g_pFramework->GetMapShapeIdx(StageID::GetStageUID(Info._StageID));
			GameUI::GetStageMapName(MyRoomInfoContext::i()->IsRandomMap(), (STAGE_ID)Info._StageID, wstrMapName);
		}		
	}

	{
		UINT32 gameType				= InfoAdd._SubType & 0x0F;
		UINT32 gameTime				= InfoAdd._SubType & 0xF0;

		i3::rc_wstring wstrModeName;
		i3::rc_wstring wstrVictoryCondition;
		i3::rc_wstring wstrGameTime;

		// 모드 이름
		wstrModeName = CStageInfoUtil::GetRuleData(Info._StageID)->getRuleName();

		// 승리 조건 (AI 전은 시작 난이도로 표시)
		{
			if( (MyRoomInfoContext::i()->getStage()->IsAiMode()) )
			{
				wstrVictoryCondition = TextSet::AIModeLevel(AIModeContext::i()->StartLevel);
			}
			else
			{
				wstrVictoryCondition = TextSet::VictoryCondition(StageID::GetStageMode(Info._StageID), gameType);
			}
		}

		// 제한시간
		{
			wstrGameTime = TextSet::GameTime(gameTime);
		}

		i3::sprintf(wstrRoomInfo, L"%s (%s / %s)", wstrModeName, wstrGameTime, wstrVictoryCondition);
	}

	// 무기 제한
	{
		// bWeaponLimit == false 일 경우 제한된 무기가 있는것입니다.
		bWeaponLimit &= TESTBIT(Info._WeaponFlag, LOCK_PRIMARY) ? true : false;
		bWeaponLimit &= TESTBIT(Info._WeaponFlag, LOCK_SECONDARY)? true : false;
		bWeaponLimit &= TESTBIT(Info._WeaponFlag, LOCK_MELEE)? true : false;
		bWeaponLimit &= TESTBIT(Info._WeaponFlag, LOCK_THROW)? true : false;	
	}

	// 랜덤맵 개선 시스템을 사용하지 않으면 기존 랜덤맵 방식을 쓴다.
	if (true != ClientStageInfoContext::i()->IsUseRandomMapRenewalSystem())
	{
		// 서버로부터 받은 랜덤 스테이지 리스트 적용.
		RandomMapContext::i()->TakeOverStageList();
	}
	
	L = _CallLuaFunction("setMapInfo");

	if( L != nullptr)
	{
		i3Lua::PushInteger(L , ImageIdx);
		i3Lua::PushStringUTF16To8(L, wstrMapName);
		i3Lua::PushStringUTF16To8(L, wstrRoomInfo);
		i3Lua::PushInteger(L , InfoAdd._LockInterEnter);
		i3Lua::PushInteger(L , bWeaponLimit == false ? 1 : 0);
		i3Lua::PushInteger(L , InfoAdd._LockObserver > 0 ? 1 : 0);
		i3Lua::PushInteger(L , InfoAdd._LockTeamBalance > 0 ? 1 : 0);
		i3Lua::PushBoolean(L , _BeHost()); // [TFT]스테이지 창 작업 중 강제 off 되어버림.
		_EndLuaFunction(L ,8);
	}

	L = _CallLuaFunction("setEventMap");
	if( L != nullptr)
	{
		if( MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON) ) 
		{
			// 이벤트 맵
			i3Lua::PushBoolean(L , true);
			i3Lua::PushStringUTF16To8(L, CStageInfoUtil::GetRuleData(Info._StageID)->getRuleName());
			_EndLuaFunction(L ,2);
		}
		else
		{
			i3Lua::PushBoolean(L , false);
			_EndLuaFunction(L ,1);
		}
	}
}

void UIFloatReadyRoomInfo::_UpdateDomiRoundInfo( void)
{
	if( IsReadyToUpdate() == false)
		return;

	INT32 ImageIdx = 0;

	i3::rc_wstring wstrRoomInfo;

	// 스테이지 설정
	ImageIdx = g_pFramework->GetRandomMapIdx();
	wstrRoomInfo = CStageInfoUtil::GetMapData(MyRoomInfoContext::i()->getStage()->m_ui32StageID)->getDisplayName();

	LuaState * L = _CallLuaFunction("setDomiRoundInfo");
	if( L != nullptr)
	{
		wchar_t DomiRound[64] = {0,};

		i3Lua::PushInteger(L , ImageIdx);
		i3Lua::PushStringUTF16To8(L, DomiRound);
		i3Lua::PushStringUTF16To8(L, wstrRoomInfo);
		i3Lua::PushBoolean(L , _BeHost());
		_EndLuaFunction(L ,4);
	}
}

void UIFloatReadyRoomInfo::ToggleMapListWnd( void)
{
	// 토글을 위하여 사용합니다.
	m_bOpenMapList = !m_bOpenMapList;

	_AttachToTopScreen();
	_ToggleMapListWnd(m_bOpenMapList);
}

void UIFloatReadyRoomInfo::CloseMapListWnd(i3UIControl * pCtrl)
{
	i3UIControl * pCurCtrl = reinterpret_cast<i3UIControl*>(g_pFramework->getFocus());

	if(pCurCtrl == pCtrl->getParent())
		return;

	// 버튼을 눌러서 Close 하는 경우에는 Kill Focus로 오는 처리를 무시합니다.
	//LuaState * L = _CallLuaFunction("GetBtnFrameWnd");
	//if( L != nullptr)
	//{
	//	i3UIFrameWnd * pCheckFrame = reinterpret_cast<i3UIFrameWnd * >(_ReturnLuaPointer(L , 0));

	//	if(pCurCtrl == pCheckFrame)
	//		return;
	//}

	m_bOpenMapList = false;

	_ToggleMapListWnd( false);
}

void UIFloatReadyRoomInfo::ChangeStage( INT32 i32Idx)
{
	m_FloatingList.ChangeItem( i32Idx);
	ToggleMapListWnd();
}

void UIFloatReadyRoomInfo::ClearFloatingListBox( INT32 cnt)
{
	//LuaState *	L = _CallLuaFunction("ClearMapListBox");
	//if( L != nullptr)
	//{
	//	i3Lua::PushInteger(L, cnt);
	//	_EndLuaFunction( L, 1);
	//}
}

void UIFloatReadyRoomInfo::SetFloatingListBoxItem( INT32 idx, const i3::wliteral_range& wNameRng)
{
	//LuaState * L = _CallLuaFunction("SetStageSlot");
	//if(L != nullptr)
	//{
	//	i3Lua::PushInteger(L, idx);
	//	i3Lua::PushStringUTF16To8(L, wNameRng);
	//	_EndLuaFunction(L , 2);
	//}
}

void UIFloatReadyRoomInfo::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

/*virtual*/ void UIFloatReadyRoomInfo::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data & UserData)
{
	UIFloatingBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIFloatReadyRoomInfo::ReceiveGameEvent_Update_MapInfo( INT32, const i3::user_data&)
{
	UpdateMapInfo();
}
