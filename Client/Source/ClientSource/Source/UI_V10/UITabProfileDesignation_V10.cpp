#include "pch.h"
#include "UIPhaseProfile_V10.h"
#include "UITabProfileDesignation_V10.h"
#include "UI/UIMainFrame_V10.h"
#include "UIPhaseProfile_V10.h"
#include "UI/UIUtil.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UserInfoContext.h"

#include "UI/UIShopFunction.h"
#include "../StepProfileDef.h"
#include "MedalContext.h"
#include "UI/UICharaInfoMgr.h"


static UITabProfileDesignation_V10 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	UITabProfileDesignation_V10* pTab = static_cast<UIPhaseProfile_V10*>(pSubPhase)->GetDesignation();
	I3ASSERT( i3::same_of<UITabProfileDesignation_V10*>(pTab)); 

	return static_cast<UITabProfileDesignation_V10*>(pTab);
}

namespace
{
	INT32 GetEmptySlot()
	{
		const CUserDesigInfo*	myDesigInfo = DesigUtil::GetMyUserDesigInfo();

		UINT8 desigSetTypes[MAX_EQUIP_USER_TITLE];
		myDesigInfo->GetEquipDesig(desigSetTypes);

		for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
		{
			const DesigSet* pDesigSet = DesigUtil::FindDesigSetByType(desigSetTypes[i]);
			if (pDesigSet == nullptr)
				return i;
		}

		return -1;
	}

	// 이 부분은 V2 베레모쪽 장착 제한이 깨끗하게 완료가 되어 있지 않기 때문에 다소 불안정하다..(2015.05.29.수빈)
	// 해당 함수는 호칭에서만 사용합시다. (2015.07.29 양승천)
	void CheckUserTitlesBeret()
	{
		//해제하는 호칭이 현재 착용하고 있는 베레모를 착용하기 위한
		//필요 호칭이라면 현재 착용하고 있는 베레모를 벗긴다. (서버에서 알아서 벗긴다고 가정하고 여기서는
		//노티파잉만 신경 쓰도록 한다...)

		CharaInfoMgr::instance()->list_up();

		// 현재 착용중인 베레모를 가져옵니다 (캐릭터별 동일)
		const ITEM_INFO & beret_info = CharaInfoMgr::instance()->get_current_equip_item(UserEquipInfo::SUE_BERET);
		if(false == DesigUtil::CheckWearable_MyItemByDesig(beret_info.m_TItemID) )
		{
			ItemInfoConvert item_convert(beret_info);
		
			for (INT32 team = 0; team < TEAM_COUNT; ++team)
			{
				UINT8 chara_slot_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx(static_cast<TEAM_TYPE>(team));
				const CHARA_PARTS_INFO * parts_info = CCharaInfoContext::i()->GetCharaPartsInfoBySlotIdx(chara_slot_idx);
				T_ItemID chara_item_id = parts_info->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

				CHARA_PARTS_INFO parts;
				const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
				pMyParts->CopyTo(&parts);
				
				ITEM_INFO default_item_info = item_def::get_default_item(chara_item_id, EQUIP::ePART_BERET);
				parts.SetItemInfo(CHAR_EQUIPMENT_PARTS_BERET, default_item_info);
				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&parts, team);
			}

			CharaInfoMgr::instance()->change_equip_item(item_convert, beret_info);
		}

		// 무기는 별도의 착용 제한은 없으나, 기존의 구조가 무기도 고려하여, 일단 제작은 해둡니다.
		// 필요 시 주석 해제 후 사용하시면 됩니다. 2015.07.29 양승천
		//const ItemInfoList & weapon_list = CharaInfoMgr::instance()->get_current_equip_weapon();
		//for( INT32 idx = 0 ; idx < item_def::WT_MAX ; ++idx)
		//{
		//	const ITEM_INFO & weapon_info = weapon_list[item_def::WT_PRIMARY];

		//	if( DesigUtil::CheckWearable_MyItemByDesig(weapon_info.m_TItemID) )
		//	{
		//		ItemInfoConvert item_convert(weapon_info);
		//		CharaInfoMgr::instance()->change_equip_item(item_convert, weapon_info);
		//	}
		//}

		// 장착 정보가 변경된 경우에는, 장비 변경 패킷을 전달합시다.
		if( CharaInfoMgr::instance()->send_event_change_equip() == true )
		{
			// 장비하신 베레모가 있을 경우 자동으로 해제됩니다.
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_PROFILE_VELEMO_RELEASE"));		
			// 게임스트링이 추가되어야 할 것 같다...일단 방치..
			//	if( g_pFramework->EquipCharacterToMyInven(0, CHARACTER_CLASS_ITEM, 0, true))
			//			PlaySound( UISND_ITEM_EQUIP);
		}
	}
}

namespace
{
	static void CbDesignationGet(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			INT32 ShowingDesignation = GetThis()->GetCurShowingDesigType();
			GameEventSender::i()->SetEvent(EVENT_DESIGNATION_GET, &ShowingDesignation );
		}
	}

	static void CbDesignationRelese(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			UINT8 RemoveTitleSlotIndex = (UINT8)GetThis()->GetSelectedDesignationReleaseSlotIdx();

			// 해제하기 때문에 DesignationID 는0으로 넣어준다.
			//CheckUserTitlesBeret(RemoveTitleSlotIndex, 0);

			GameEventSender::i()->SetEvent( EVENT_DESIGNATION_RELEASE, &RemoveTitleSlotIndex);
		}
	}

	static void CbDesignationChange(void* pThis, INT32 nParam)
	{
		if (nParam == MBR_OK)
		{
			// GetThis()->__OnDesignationChange();
			UIMainFrame* pFrame = g_pFramework->GetUIMainframe();
			pFrame->OpenPopup(UPW_DESIG_SELECT);
		}
	}


}

struct SetMyInfoDesignation_Image_Enable_V10
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Enable");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Enable_V10;

struct SetMyInfoDesignation_Image_Lock_V10
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Lock");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Lock_V10;

struct SetMyInfoDesignation_Image_Index_V10
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, UINT32 desigSetIndex) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Index");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushInteger(L, desigSetIndex);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Index_V10;

struct SetMyInfoDesignation_Text_V10
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, i3::rc_wstring wstrTitle) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Text");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushStringUTF16To8(L, wstrTitle.c_str());
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Text_V10;


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

extern "C" {
	int UITabProfileDesignation_V10_Click(LuaState * L)
	{
		GetThis()->__UpdateInfoFromLuaGlue();
		return 0;
	}
	//int UITabProfileDesignation_V10_MyDesigClick(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyDesigClickFromLuaGlue(idx);
	//	return 0;
	//}
	//int UITabProfileDesignation_V11_MyDesigDoubleClick(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyDesigDoubleClickFromLuaGlue(idx);
	//	return 0;
	//}
	//int UITabProfileDesignation_V11_Release(LuaState * L)
	//{
	//	INT32 idx = i3Lua::GetIntegerArg(L, 1);
	//	GetThis()->__MyEquipReleaseFromLuaGlue(idx);
	//	return 0;
	//}
}

LuaFuncReg UITabProfileDesignation_V10_Glue[] =
{
	//{ "ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigClick		},
	//{ "D_ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigDoubleClick	},
	{ "UseDesignation",		UITabProfileDesignation_V10_Click				},
	//{ "ClickXButton",		UITabProfileDesignation_V11_Release				},
	{ nullptr,		nullptr}
};


I3_CLASS_INSTANCE( UITabProfileDesignation_V10);

UITabProfileDesignation_V10::UITabProfileDesignation_V10() : m_vecGetDesignation(MAX_DEFAULT_USER_TITLE_COUNT)
{
	::memset(m_pButtonImage, 0, sizeof(m_pButtonImage));
	::memset(m_pShapeImage, 0, sizeof(m_pShapeImage));
	m_nCurShowingDesignationType = 0;
	m_nCurDesignationSlotCount = 1;
}

UITabProfileDesignation_V10::~UITabProfileDesignation_V10()
{
}

void UITabProfileDesignation_V10::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	switch( event)
	{
	case EVENT_DESIGNATION_GET :
		{
			// 획득 호칭
			INT32 result = arg;
			switch( result )
			{
			case EVENT_ERROR_SUCCESS:
				{
					// 호칭 획득시 보상 및 알림창
					i3::rc_wstring wstrMessage;
					DesigUtil::Update_Recv_EVENT_DESIGNATION_GET( wstrMessage );
					GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMessage);
				}
				break;
			case EVENT_ERROR_USER_TITLE_REQUIREMENT_FAIL:
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_NETWORK_NOT_MATCH_BUY_DESIGNATION"));/*호칭 교환 조건을 만족하지 않습니다.*/
				break;
			case EVENT_ERROR_USER_TITLE_ALREADY_GAIN_FAIL :
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_NETWORK_ALREADY_GET_DESIGNATION"));/*이미 획득한 호칭입니다.*/
				break;
			}
			_Update_All();
		}
		break;
	case EVENT_DESIGNATION_CHANGE:
	case EVENT_DESIGNATION_RELEASE :
		{
			CheckUserTitlesBeret();
			_Update_All();
		}
		break;
	case EVENT_GET_USERINFO :
		_Update_All();
		break;
	}
}



/*virtual*/ bool UITabProfileDesignation_V10::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		//	/*
		//	----------------------------------------필독---------------------------------------------
		//	이벤트는 호칭 버튼만 받습니다. 나머지 UI 이벤트는 루아로...
		//	임시로 스테이트값 호칭창에 맞게 변형(기존 체크박스의 State를 생각하시면 안되욧)
		//	엔진단의 UIControl은 사용안함. 호칭버튼의 Press상태(내가 장착하고 있는 호칭) 때문입니다.
		//	I3UI_CHECKBUTTON_SHAPE_DISABLED_CHECKED			-> Press 상태 + OnMouse(진진파랑)
		//	I3UI_CHECKBUTTON_SHAPE_DISABLED_UNCHECKED;		-> Press 상태(진파랑) or 테두리
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED_ONMOUSE	-> 셀렉트 상태(파랑) + OnMouse(테두리)
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED			-> 셀렉트 상태(파랑)
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED_ONMOUSE	-> 노멀 상태 + OnMouse -> 튀어나오는 효과
		//	I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED			-> 노멀 상태 -> 효과 무
		//	-----------------------------------------------------------------------------------------
		//	*/

		// *************** Shape Idx Info
		// 0 : 획득하지 않은 호칭 Normal 
		// 1 : 획득하지 않은 호칭 MouseOver
		// 2 : 획득하지 않은 호칭 Selected
		// 3 : 획득한 호칭 Normal
		// 4 : 획득한 호칭 MouseOver
		// 5 : 획득한 호칭 Selected
		// *************** Shape Idx Info

		bool bCheckBox = false;


		if(_GetDesigIdx(pParam->m_pCtrl) >= 0)
		{
			bCheckBox = true;
		}

		if(	bCheckBox == false )
		{
			if(pParam->m_nEvent == I3UI_EVT_CLICKED)
				I3TRACE(pParam->m_pCtrl->GetName());
			return true;
		}

		// MouseOver 
		if( pParam->m_nEvent == I3UI_EVT_ENTERMOUSE)
		{
			INT32 idx = _GetDesigIdx( pParam->m_pCtrl);
			// 인덱스 범위 체크는 아래 함수 내부에서 진행
			_ChangeButtonImgState(I3UI_EVT_ENTERMOUSE, idx);
		}

		if( pParam->m_nEvent == I3UI_EVT_LEAVEMOUSE)
		{
			INT32 idx = _GetDesigIdx( pParam->m_pCtrl);
			_ChangeButtonImgState(I3UI_EVT_LEAVEMOUSE, idx);
		}

		if( pParam->m_nEvent == I3UI_EVT_DBL_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			__SetCurrentButton( _GetDesigIdx( pParam->m_pCtrl) );

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				_ChangeButtonImgState(I3UI_EVT_DBL_CLICKED, m_SelectedDesignationIdx);
				m_pButtonImage[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pButtonImage[m_OldSelectedDesignationIdx]->setModifyState(true);
			}

			_Update_DetailInfoBox();
			_Update_GetInfoBox();
			__OnDoubleClickedDesignationButton();
			_Update_DesignationTree();	
		}
		else if( pParam->m_nEvent == I3UI_EVT_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			__SetCurrentButton( _GetDesigIdx( pParam->m_pCtrl) );

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				_ChangeButtonImgState(I3UI_EVT_CLICKED, m_SelectedDesignationIdx);
				m_pButtonImage[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pButtonImage[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
		}

	}


	return UITabProfileBase_V10::OnEvent( event, pObj, param2, code);
}

/*virtual*/ void UITabProfileDesignation_V10::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase_V10::OnCreate( pParent);
	
	AddScene( "Scene/Main/PointBlankRe_MyInfomation_Designation.i3UIs", UITabProfileDesignation_V10_Glue);
}



bool UITabProfileDesignation_V10::OnEntranceStart( void * pArg1, void * pArg2 )
{
	// 여기서 미리 받아두어야, 호칭 슬롯을 새로 획득하게 되었을 때에만 축하 팝업을 띄워줄 수 있습니다
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
	INT32 limitTitleCount = myDesigInfo->GetLimitCount();

	m_nCurDesignationSlotCount = limitTitleCount;

	if( UITabProfileBase_V10::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	return true;
}


bool UITabProfileDesignation_V10::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V10::OnClick( pControl) == false)
		return false;

	//if( i3::same_of<i3UICheckBox*>(pControl))
	//	PlaySound( UISND_CLICK);

	return true;
}

bool UITabProfileDesignation_V10::OnEnterMouse( i3UIControl * pControl)
{
	//if( UITabProfileBase_V10::OnEnterMouse( pControl) == false)
	//	return false;

	return true;
}

/*virtual*/void UITabProfileDesignation_V10::_InitializeAtLoad( i3UIScene * pScene)
{
	I3ASSERT( pScene != nullptr);

	m_pScene = pScene;

	m_pDesigFmw = (i3UIFrameWnd *)pScene->FindChildByName("Fmw_Designation");
	I3ASSERT( m_pDesigFmw !=  nullptr );

	// 우측 정보 관련 UIControl 담기
	m_pHavedMedal[0] = (i3UIStaticText*)pScene->FindChildByName("TextMiniHave");	// 약장
	m_pHavedMedal[1] = (i3UIStaticText*)pScene->FindChildByName("TextInsHave");		// 휘장
	m_pHavedMedal[2] = (i3UIStaticText*)pScene->FindChildByName("TextMedalCurrent");	// 훈장
	m_pHavedMedal[3] = (i3UIStaticText*)pScene->FindChildByName("TextMasterHave");	// 마스터훈장

	m_pNeedMedal[0] = (i3UIStaticText*)pScene->FindChildByName("TextMiniNeed");	// 약장
	m_pNeedMedal[1] = (i3UIStaticText*)pScene->FindChildByName("TextInsNeed");	// 휘장
	m_pNeedMedal[2] = (i3UIStaticText*)pScene->FindChildByName("TextMedalNeeds");	// 훈장 
	m_pNeedMedal[3] = (i3UIStaticText*)pScene->FindChildByName("TextMasterNeed");	// 마스터훈장 

	m_pNeedTitle[0] = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context00");
	m_pNeedTitle[1] = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context01");

	m_pNeedRank = (i3UIStaticText*)pScene->FindChildByName("TextNeedDes_context02");

	// 호칭 트리 UIControl 담기
	i3UIFrameWnd* pFmwIcon =	(i3UIFrameWnd *)pScene->FindChildByName("FmwDesView");
	i3::rc_wstring wstrCtrlName;
	if(pFmwIcon != nullptr)
	{
		INT32 fmwViewCnt =  pFmwIcon->GetChildCount();
		i3UIControl * pChild = (i3UIControl *)pFmwIcon->getFirstChild();

		for(INT32 i=0; i<fmwViewCnt ; ++i)
		{
			INT32 fmwIconCnt =  pChild->GetChildCount();
			i3UIControl * pChild2 = (i3UIControl *)pChild->getFirstChild();

			for(INT32 j=0; j<fmwIconCnt ; ++j)
			{

				if(i3::kind_of<i3UIButtonImageSet*>(pChild2))
				{
					i3::sprintf(wstrCtrlName, L"Imgbtn%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(),wstrCtrlName ))
					{
						m_pButtonImage[i] = (i3UIButtonImageSet*)pChild2;
					}

					i3::sprintf(wstrCtrlName, L"ImgShape%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(),wstrCtrlName ))
					{
						m_pShapeImage[i] = (i3UIButtonImageSet*)pChild2;
					}
				}
				else if (i3::kind_of<i3UIImageBox*>(pChild2))
				{
					i3::sprintf(wstrCtrlName, L"ImgEquipFlag%02d", i);
					if(i3::generic_is_iequal( pChild2->GetNameString(), wstrCtrlName))
						m_pGainDesigFlag[i] = (i3UIImageBox*)pChild2;

				}

				pChild2 = (i3UIControl *)pChild2->getNext();
			}

			pChild = (i3UIControl *)pChild->getNext();
		}

	}

	// 빈 정보로 초기화
	_InitTabInfo();
	// 전체 정보 갱신
	_Update_All();

	GetSlide()->AddFrameWnds(GetScene(0));

	const CUserDesigInfo* userDesigInfo = DesigUtil::GetMyUserDesigInfo();
	m_nCurDesignationSlotCount = userDesigInfo->GetLimitCount();
}

void	UITabProfileDesignation_V10::_InitTabInfo()
{
	// 호칭 트리 Shape 설정
	for(INT32 i = 0 ; i<MAX_DEFAULT_USER_TITLE_COUNT; ++i)
	{
		if(m_pShapeImage[i] != nullptr)
		{
			m_pShapeImage[i]->SetShapeIdx(i);
			m_pShapeImage[i]->SetViewOnly(true);
			m_pGainDesigFlag[i]->EnableCtrl(true);
		}
	}

	// 좌측 팝업 정보 설정
	LuaState* L = _CallLuaFunction("setMyInfo");

	if (L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, UserInfoContext::i()->GetMyNickName().c_str());
		i3Lua::PushInteger(L, UserInfoContext::i()->GetMyRank());
		_EndLuaFunction(L, 2);
	}

	// 우측 정보 초기화
	// 보유 약장  /  필요 약장 정보
	i3::stack_wstring wstrCount;

	// 약장
	UINT32 ribbonCount = MedalContext::i()->getMiniature();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
	m_pHavedMedal[0]->SetText(wstrCount.c_str());
	// 휘장
	UINT32 InsigniaCount = MedalContext::i()->getInsignia();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
	m_pHavedMedal[1]->SetText(wstrCount.c_str());
	// 훈장
	UINT32 orderCount = MedalContext::i()->getOrder();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
	m_pHavedMedal[2]->SetText(wstrCount.c_str());
	// 마스터 훈장
	UINT32 masterCount = MedalContext::i()->getMaster();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
	m_pHavedMedal[3]->SetText(wstrCount.c_str());

	for(INT32 i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), 0);
		m_pNeedMedal[i]->SetText(wstrCount.c_str());
	}
}

void UITabProfileDesignation_V10::_ChangeButtonImgState(I3UI_EVT _evt, INT32 idx)
{
	if( idx < 0 || idx >= MAX_DEFAULT_USER_TITLE_COUNT) 
	{
		return;
	}
	
	// 호칭을 획득하지 않은 상태
	if(m_pButtonImage[idx]->GetShapeIdx() < 3)
	{
		switch(_evt)
		{
		case I3UI_EVT_ENTERMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 2)
				m_pButtonImage[idx]->SetShapeIdx(1);
			break;
		case I3UI_EVT_LEAVEMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 2)
				m_pButtonImage[idx]->SetShapeIdx(0);
			break;
		case I3UI_EVT_CLICKED:
			m_pButtonImage[idx]->SetShapeIdx(2);			
			break;
		}
	}
	else
	{
		switch(_evt)
		{
		case I3UI_EVT_ENTERMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 5)
				m_pButtonImage[idx]->SetShapeIdx(4);
			break;
		case I3UI_EVT_LEAVEMOUSE:
			if(m_pButtonImage[idx]->GetShapeIdx() != 5)
				m_pButtonImage[idx]->SetShapeIdx(3);
			break;
		case I3UI_EVT_CLICKED:
			m_pButtonImage[idx]->SetShapeIdx(5);
			break;
		}
	}

	// 인덱스 m_OldSelectedDesignationIdx는 바깥에서 체크
	if(_evt == I3UI_EVT_CLICKED)
	{
		if(m_pButtonImage[m_OldSelectedDesignationIdx]->GetShapeIdx() < 3 )
			m_pButtonImage[m_OldSelectedDesignationIdx]->SetShapeIdx(0); 
		else
			m_pButtonImage[m_OldSelectedDesignationIdx]->SetShapeIdx(3); 
	}

}

void UITabProfileDesignation_V10::OnLoadAllScenes()
{
	//GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileDesignation_V10::OnUnloadAllScenes()
{
}

void UITabProfileDesignation_V10::_ClearAtUnload( i3UIScene * pScene)
{
	//UITabProfileBase_V10::_ClearAtUnload( pScene);
}

void UITabProfileDesignation_V10::__OnDoubleClickedDesignationButton()
{
	if( m_vecGetDesignation[m_SelectedDesignationIdx])
	{
		if( _CheckDesignationEquip()) // 해당 호칭이 장착 여부 확인
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationRelese), this);	//선택한 호칭을 해제하시겠습니까?
		else
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_EQUIP_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationChange), this);	//선택한 호칭을 장착하시겠습니까?
	}
	else
	{	
		switch( m_DesignationMode)
		{
		case DESIGNATION_MODE_GET:
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationGet), this);/*선택한 호칭을 획득하시겠습니까?*/
			break;
		case DESIGNATION_MODE_CHANGE:
			//	m_pDesignationChangeBG->SetEnable( true);
			break;
		case DESIGNATION_MODE_RELEASE:
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_STRING("STR_TBL_DESIGNATION_RELEASE_ANSWER_MESSAGE"), nullptr, MAKE_CALLBACK(CbDesignationRelese), this);/*선택한 호칭을 해제하시겠습니까?*/
			break;
		default:
			break;
		}
	}
}

void UITabProfileDesignation_V10::__OnDesignationChange(INT32 targetSlotIdx)
{


	m_SelectedDesignationChangeSlotIdx = targetSlotIdx;
	CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx <myDesigInfo->GetLimitCount() )
	{

		UINT8 myEquipDesigTypes[MAX_EQUIP_USER_TITLE];
		myDesigInfo->GetEquipDesig(myEquipDesigTypes);

		bool DesigSetAlreadyUsed = false;

		const DesigSet* pDesigSetSelected = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

		if ( pDesigSetSelected != nullptr )
		{

			for (INT32 i = 0 ; i < MAX_EQUIP_USER_TITLE ; ++i)
			{
				if ( i == m_SelectedDesignationChangeSlotIdx)
					continue;

				if (myEquipDesigTypes[i] == pDesigSetSelected->desigSetType)
				{
					DesigSetAlreadyUsed = true;
					break;
				}
			}
		}

		if( DesigSetAlreadyUsed )
		{
			//다른 슬롯에 동일한 계열의 호칭을 사용하고 있어 사용에 실패하였습니다.
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_TBL_DESIGNATION_FAIL_ANOTHER_USED"));
		}
		else
		{
			if ( pDesigSetSelected != nullptr )
			{
				UINT8 ui8DesigSlot = (UINT8)m_SelectedDesignationChangeSlotIdx;
				UINT8 DesignationType = (UINT8)pDesigSetSelected->desigSetType;
			//	CheckUserTitlesBeret(m_SelectedDesignationChangeSlotIdx, DesignationType);

				GameEventSender::i()->SetEvent( EVENT_DESIGNATION_CHANGE, &ui8DesigSlot, &DesignationType);
			}
		}
	}
	else
	{
		//사용하실수 있는 슬롯이 없습니다.\n호칭 해제후 다시 등록해 주십시오.
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_FAIL_ANOTHER_USED"));	
	}

	m_DesignationMode = DESIGNATION_MODE_GET;
	m_SelectedDesignationChangeSlotIdx = -1;
}

bool UITabProfileDesignation_V10::_CheckDesignationEquip(void)
{
	const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

	if ( desigSet == nullptr )
		return false;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 desigSetType = (UINT8)desigSet->desigSetType;
	INT32 slotIndex = myDesigInfo->FindDesigEquipSlot(desigSetType);

	if( slotIndex != -1)
	{
		m_SelectedDesignationReleaseSlotIdx = slotIndex;
		return true;
	}

	return false;
}



INT32	UITabProfileDesignation_V10::_GetDesigIdx(i3UIControl* pCtrl)
{
	if(	i3::same_of<i3UIButton*>(pCtrl))
	{
		i3::rc_string wstrName = pCtrl->GetNameString();
		wstrName.erase( 0, 10 ); 

		return ::atoi(wstrName.c_str());
	}
	return -1;
}



void UITabProfileDesignation_V10::__SetCurrentButton(INT32 idx)
{
	if(idx != m_SelectedDesignationIdx)
	{
		m_OldSelectedDesignationIdx = m_SelectedDesignationIdx;
		m_SelectedDesignationIdx = idx;
	}
}

//////////////////////////////////////////////////////////////////////////
//						Update Infomation								//
//////////////////////////////////////////////////////////////////////////

void	UITabProfileDesignation_V10::_Update_All()
{
	// 호칭 슬롯 확인
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
		INT32 limitTitleCount = myDesigInfo->GetLimitCount();

		// 추가슬롯을 획득하였다면~!!  --> ???
		if ( limitTitleCount > m_nCurDesignationSlotCount )
		{
			m_nCurDesignationSlotCount = limitTitleCount;

			if(limitTitleCount != 0)
			{
				i3::stack_wstring wstrMsg;
				/*축하드립니다!!\n \n호칭 슬롯을 획득하여 호칭을 동시에\n사용할 수 있는 개수가 증가하였습니다.\n(최대 %d종의 호칭 사용 가능)*/
				i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SLOT_MESSAGE"), m_nCurDesignationSlotCount);
				GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);
			}
		}
	}


	DesigUtil::AcquireGainedDesigSetsForUI( m_vecGetDesignation, MAX_DEFAULT_USER_TITLE_COUNT);

	// 내 정보 호칭 변경
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	INT32 iLimitTitleCount = myDesigInfo->GetLimitCount();

	I3ASSERT( iLimitTitleCount <= MAX_EQUIP_USER_TITLE);

	for( INT32 i = 0; i < iLimitTitleCount; ++i)
	{
		fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i,false);
		fnSetMyInfoDesignation_Image_Lock_V10(m_pScene, i,false);//Enable true, false
	}

	for( INT32 i = iLimitTitleCount; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i,false);
		fnSetMyInfoDesignation_Image_Lock_V10(m_pScene, i,true);
	}

	// 호칭이미지 및 호칭이름.
	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
	{
		// 먼저 초기화
		fnSetMyInfoDesignation_Text_V10(m_pScene, i, L"");

		UINT8 myEquipDesigSet = myDesigInfo->GetEquipDesig(i);

		if( myEquipDesigSet > MAX_DEFAULT_USER_TITLE_COUNT)
			continue;

		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(myEquipDesigSet);

		if (desigSet != nullptr)
		{
			fnSetMyInfoDesignation_Image_Enable_V10(m_pScene, i, true);
			fnSetMyInfoDesignation_Image_Index_V10(m_pScene, i, desigSet->desigSetUIIndex );
			fnSetMyInfoDesignation_Text_V10(m_pScene, i, desigSet->wstrName);
		}
	}

	// 상세 정보 변경
	_Update_GetInfoBox();

	// 호칭 트리 업데이트
	_Update_DesignationTree();
}

void	UITabProfileDesignation_V10::_Update_DetailInfoBox()
{
	if(m_SelectedDesignationIdx < 0)
	{
		m_nCurShowingDesignationType = 0;
		return;
	}
	// Detail Info
	const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);
	if (desigSet == nullptr) return;

	m_nCurShowingDesignationType = desigSet->desigSetType;

	i3::stack_wstring wstrTemp1;
	i3::stack_wstring wstrTemp2;

	wstrTemp1.clear();		wstrTemp2.clear();

	// Plus Effect
	for(INT32 i = 0; i < 2; ++i)
	{
		INT32 desigIndex = desigSet->plusEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr ) 
		{
			if (i == 0)
				i3::sprintf(wstrTemp1, L"{col:80,150,215,255}%s{/col}", attr->GetText() );
			else
				i3::sprintf(wstrTemp2, L"%s\n{col:80,150,215,255}%s{/col}", wstrTemp1, attr->GetText() );
		}
		else
		{
			if( i == 0)	
				i3::sprintf( wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR1"));/*{col:80,150,215,255}없음{/col}*/
		}
	}

	if ( !wstrTemp2.empty() )
		wstrTemp1 = wstrTemp2;

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetPlusEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Minus Effect
	wstrTemp1.clear();		wstrTemp2.clear();

	for(INT32 i=0 ; i < 2 ; ++i)
	{
		INT32 desigIndex = desigSet->minusEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr ) 
		{
			if ( i == 0)
				wstrTemp1 = attr->GetText(); 
			else
				i3::sprintf(wstrTemp2, L"%s\n%s", wstrTemp1, attr->GetText() );
		}
		else
		{
			if( i == 0)	
				i3::sprintf(wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR2"));/*{col:205,100,50,255}없음{/col}*/
		}
	}

	if ( !wstrTemp2.empty() )
		wstrTemp1 = wstrTemp2;

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetMinusEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Add Effect

	const i3::vector<UINT32>* pvecItemID = DesigUtil::FindItemIDsFromDesigType( (UINT8)desigSet->desigSetType );

	const INT32 numItemID = (pvecItemID) ? (INT32)pvecItemID->size() : 0;

	bool	UseSlotEffect[2];
	wstrTemp1.clear();		wstrTemp2.clear();

	i3::stack_wstring wstrAddEffect(L"INVALID");
	i3::rc_wstring wstrItemName(L"INVALID");

	for(INT32 i = 0; i< 2; ++i )
	{	
		UseSlotEffect[i] = false;

		INT32 desigIndex = desigSet->addEffect[i];

		const DesigAttr* attr = DesigUtil::FindDesigAttr(desigIndex);

		if( attr )		// (plus, minus와 달리 0인 경우 허용)..
		{

			if (desigIndex == 0 || attr->IsPurchasable() )
			{
				if ( i < numItemID )
				{
					usf::GetShopItemName(wstrItemName, (*pvecItemID)[i]);
					if ( !wstrItemName.empty() )
					{
						UseSlotEffect[i] = true;
						i3::sprintf(wstrAddEffect, GAME_RCSTRING("STR_HEADER_AUTHORITY_WEAPON"), wstrItemName);
					}
				}
			}
			else		// 호칭 기능일 것임...
			{
				wstrAddEffect =	attr->GetText(); 
				UseSlotEffect[i] = true;
			}
		}

		if(i==0 && UseSlotEffect[i] == true)
		{	//	첫번째 호칭 효과
			i3::sprintf( wstrTemp1, L"{col:204,204,204,255}%s{/col}", wstrAddEffect);
		}
		else if(i==1 && UseSlotEffect[i] == true)
		{	//	두번째 호칭 효과
			i3::sprintf( wstrTemp2, L"%s\n{col:204,204,204,255}%s{/col}", wstrTemp1, wstrAddEffect);
		}	
	}

	if (!wstrTemp2.empty())
		wstrTemp1 = wstrTemp2;

	// 호칭 효과 없음
	if(!UseSlotEffect[0] && !UseSlotEffect[1])
	{
		i3::sprintf( wstrTemp1, GAME_RCSTRING("STR_TBL_DESIGNATION_NONE_WITH_COLOR3"));/*{col:204,204,204,255}없음{/col}*/	
	}	

	{
		LuaState * L = UILua::CallLuaFunction(m_pScene, "SetAddEffectString");
		i3Lua::PushStringUTF16To8(L, wstrTemp1);
		_EndLuaFunction(L, 1);
	}

	// Title Text/UI Shape Setting
	{
		LuaState* L = UILua::CallLuaFunction(m_pScene, "SetDesigDetailInfo");

		if(L != nullptr)
		{
			i3Lua::PushStringUTF16To8(L, desigSet->wstrName);
			i3Lua::PushInteger(L, m_SelectedDesignationIdx);
			_EndLuaFunction(L, 2);

		}
	}
}

void	UITabProfileDesignation_V10::_Update_GetInfoBox()
{
	i3::stack_wstring wstrCount;
	const DesigSet* pDesigSet = DesigUtil::FindDesigSetByUI( m_SelectedDesignationIdx );
	if(pDesigSet == nullptr) return;

	INT32 desigSetType = pDesigSet->desigSetType;

	// 보유한 표장 설정

	// 약장
	UINT32 ribbonCount = MedalContext::i()->getMiniature();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
	m_pHavedMedal[0]->SetText(wstrCount.c_str());

	// 휘장
	UINT32 InsigniaCount = MedalContext::i()->getInsignia();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
	m_pHavedMedal[1]->SetText(wstrCount.c_str());

	// 훈장
	UINT32 orderCount = MedalContext::i()->getOrder();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
	m_pHavedMedal[2]->SetText(wstrCount.c_str());
	//
	// 마스터 훈장
	UINT32 masterCount = MedalContext::i()->getMaster();
	i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
	m_pHavedMedal[3]->SetText(wstrCount.c_str());

	// 필요한 표장 설정
	INT32 iCount[ 4] = {0,};
	for(INT32 i = 0; i < USER_TITLE_REQUIREMENT_COUNT; ++i)
	{
		switch( g_UserTitleRequirement[desigSetType]._ribbon[i]._type)
		{
		case UTR_RIBBON:	iCount[ 0] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_ENSIGN:	iCount[ 1] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_MEDAL:		iCount[ 2] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_MASTER:	iCount[ 3] += g_UserTitleRequirement[desigSetType]._ribbon[i]._count;	break;
		case UTR_NOTHING:		break;
		}
	}

	for(INT32 i = 0; i < BADGE_TYPE_COUNT; i++)
	{
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), iCount[ i]);
		m_pNeedMedal[i]->SetText(wstrCount.c_str());
	}

	// 필요 호칭 설정
	for( INT32 i = 0; i < 2; i++)
	{
		UINT8 reqDesigSetType = g_UserTitleRequirement[desigSetType]._userTitleId[i];

		const DesigSet* reqSet = DesigUtil::FindDesigSetByType( reqDesigSetType );

		if( reqSet )
			m_pNeedTitle[i]->SetText(reqSet->wstrName.c_str());
		else
			m_pNeedTitle[i]->SetText("----");
	}

	// 필요 계급 설정
	i3::stack_wstring wstrRankText = GetCharaRank(g_UserTitleRequirement[desigSetType]._rank);
	m_pNeedRank->SetText(wstrRankText.c_str());


	/////////////////////////////////////////////////////////////////////////
	// // 호칭 사용버튼 Enable On/Off
	struct SetDesignationSetButton
	{
		void operator()( i3UIScene* pScene, bool bEnable) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignSetEnable");
			i3Lua::PushBoolean(L, bEnable);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignationSetButton;

	struct SetDesignationSetCaption
	{
		void operator()( i3UIScene* pScene, UINT32 CState) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignationSetCaption");
			i3Lua::PushInteger(L, CState);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignationSetCaption;

	// 호칭 획득/사용/해제
	{
		if(m_SelectedDesignationIdx == -1)
		{
			fnSetDesignationSetButton( GetScene(0), false);
			return;
		}
		fnSetDesignationSetButton(GetScene(0), true);

		if( !m_vecGetDesignation[m_SelectedDesignationIdx])
		{
			m_DesignationMode = DESIGNATION_MODE_GET;
			fnSetDesignationSetCaption(GetScene(0), 1);						/*호칭 획득*/
		}
		else if( m_vecGetDesignation[m_SelectedDesignationIdx])
		{
			if( _CheckDesignationEquip())
			{
				m_DesignationMode = DESIGNATION_MODE_RELEASE;
				fnSetDesignationSetCaption(GetScene(0), 2);					/*호칭 해제*/
			}
			else
			{
				m_DesignationMode = DESIGNATION_MODE_CHANGE;
				fnSetDesignationSetCaption(GetScene(0), 3);					/*호칭 사용*/
			}
		}
	}
}



void	UITabProfileDesignation_V10::_Update_DesignationTree()
{
//	if( IsReadyToUpdate() == false) return;

	// 획득한 호칭에 대하여는 항상 특정한 표시를 합니다.
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 TitleBuffer[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(TitleBuffer);

	//장착된 호칭 파란 테두리 효과 3개까지 유지됩니다.
	// OnActivate -> CheckBox 이벤트 핸들링을 막아놓음으로서 렌더링 스테이트값을 고정시킵니다. 추후에 다른컨트롤로 수정해야함(파란 테두리 유지 효과)
	for( INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pButtonImage[i]->SetShapeIdx(0);
		if(m_vecGetDesignation[i])
		{
			m_pGainDesigFlag[i]->EnableCtrl(false);
		}
		
	}

	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(TitleBuffer[i]);
		if (desigSet)
		{
			INT32 idxUI = desigSet->desigSetUIIndex;
			if (idxUI < MAX_DEFAULT_USER_TITLE_COUNT)
				m_pButtonImage[idxUI]->SetShapeIdx(4);
		}
	}

}

// 여기가 클릭하는 부분. V1 참고해서 수정. 
void	UITabProfileDesignation_V10::__UpdateInfoFromLuaGlue()
{
	if( m_SelectedDesignationIdx >=0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{

		__OnDoubleClickedDesignationButton();

		//__OnDoubleClickedDesignationButton();
		//_Update_DetailInfoBox();
		//_Update_GetInfoBox();		
		//_Update_DesignationTree();
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SELECT"));
	}
}
