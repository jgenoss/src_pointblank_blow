#include "pch.h"
#include "UITabProfileDesignation_V11.h"
#include "UI/UIMainFrame_V11.h"
#include "UIPhaseProfile_V11.h"
#include "UI/UIUtil.h"

#include "Designation/DesigUtil.h"
#include "Designation/UserDesigInfo.h"
#include "Designation/DesigAttr.h"
#include "Designation/DesigSet.h"

#include "UI/UIShopFunction.h"
#include "../StepProfileDef.h"
#include "MedalContext.h"
#include "UI/UICharaInfoMgr.h"

#include "UserInfoContext.h"

//
// 인벤의 get_default_item을 여기에 어느정도 중복작성하고 수정 해둔다.. ( 2015.05.29.수빈)
// 인벤 네임스페이스에 있는 것을 직접 사용하기가 꺼림직한 상태임...
//

namespace
{
	// 이 부분은 V2 베레모쪽 장착 제한이 깨끗하게 완료가 되어 있지 않기 때문에 다소 불안정하다..(2015.05.29.수빈)
	// 해당 함수는 호칭에서만 사용합시다. (2015.07.29 양승천)
	void CheckUserTitlesBeret()
	{
		 //해제하는 호칭이 현재 착용하고 있는 베레모를 착용하기 위한
		 //필요 호칭이라면 현재 착용하고 있는 베레모를 벗긴다. (서버에서 알아서 벗긴다고 가정하고 여기서는
		 //노티파잉만 신경 쓰도록 한다...)

		// 해당 함수는 호칭에서만 사용합시다.
		CharaInfoMgr::instance()->list_up();

		// 현재 착용중인 베레모를 가져옵니다 (캐릭터별 동일)
		const ITEM_INFO & beret_info = CharaInfoMgr::instance()->get_current_equip_item(UserEquipInfo::SUE_BERET);
		if (false == DesigUtil::CheckWearable_MyItemByDesig(beret_info.m_TItemID))
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
}

static UITabProfileDesignation_V11 * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	UITabProfileDesignation_V11* pTab = static_cast<UIPhaseProfile_V11*>(pSubPhase)->GetDesignation();
	I3ASSERT( i3::same_of<UITabProfileDesignation_V11*>(pTab)); 

	return static_cast<UITabProfileDesignation_V11*>(pTab);
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfileDesignation_V11_Click(LuaState * L)
	{
		GetThis()->__UpdateInfoFromLuaGlue();
		return 0;
	}
	int UITabProfileDesignation_V11_MyDesigClick(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyDesigClickFromLuaGlue(idx);
		return 0;
	}
	int UITabProfileDesignation_V11_MyDesigDoubleClick(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyDesigDoubleClickFromLuaGlue(idx);
		return 0;
	}
	int UITabProfileDesignation_V11_Release(LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->__MyEquipReleaseFromLuaGlue(idx);
		return 0;
	}
}

LuaFuncReg UITabProfileDesignation_V11_Glue[] =
{
	{ "ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigClick		},
	{ "D_ClickMyDesigSlot",	UITabProfileDesignation_V11_MyDesigDoubleClick	},
	{ "UseDesignation",		UITabProfileDesignation_V11_Click				},
	{ "ClickXButton",		UITabProfileDesignation_V11_Release				},
	{ nullptr,		nullptr}
};

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
			GetThis()->__OnDesignationChange();
		}
	}
	
	
}


I3_CLASS_INSTANCE( UITabProfileDesignation_V11);

UITabProfileDesignation_V11::UITabProfileDesignation_V11() : m_vecGetDesignation(MAX_DEFAULT_USER_TITLE_COUNT)
{
	m_SelectedDesignationIdx = -1;
	m_OldSelectedDesignationIdx = -1;

	m_SelectedDesignationChangeSlotIdx = -1;
	m_SelectedDesignationReleaseSlotIdx = -1;
	m_MySlotIdx = -1;
	m_DesignationMode = DESIGNATION_MODE_GET;

	m_nCurShowingDesignationType = 0;

	// 이하 4개는 모두 포인터 배열로 그냥 memset으로 정리..
	::memset(m_pCheckBox, 0, sizeof(m_pCheckBox));				// 레퍼런스 관리는 이것만..
	::memset(m_pImageBox, 0, sizeof(m_pImageBox));
	::memset(m_pButtonImage, 0, sizeof(m_pButtonImage));
	::memset(m_dummyButton, 0, sizeof(m_dummyButton));

	m_nCurDesignationSlotCount = 0;
}

UITabProfileDesignation_V11::~UITabProfileDesignation_V11()
{
}


struct SetMyInfoDesignation_Image_Enable
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Enable");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Enable;
	
struct SetMyInfoDesignation_Image_Lock
{
	void operator()(i3UIScene* pScene, UINT32 idx, bool Sig) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Lock");
		i3Lua::PushInteger(L, idx);
		i3Lua::PushBoolean(L, Sig);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Lock;


struct SetMyInfoDesignation_Image_Index
{
	void operator()(i3UIScene* pScene, UINT32 slot_idx, UINT32 desigSetIndex) const
	{
		LuaState * L = UILua::CallLuaFunction(pScene, "SetMyInfoDesignation_Image_Index");
		i3Lua::PushInteger(L, slot_idx);
		i3Lua::PushInteger(L, desigSetIndex);
		UILua::EndLuaFunction(L, 2);
	}
} fnSetMyInfoDesignation_Image_Index;


void UITabProfileDesignation_V11::_Update_All( void)
{
	// 1. 슬롯 획득 체크..
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
		INT32 limitTitleCount = myDesigInfo->GetLimitCount();
	
		// 추가슬롯을 획득하였다면~!!  --> ???
		if ( limitTitleCount > m_nCurDesignationSlotCount )
		{
			m_nCurDesignationSlotCount = limitTitleCount;
			
			i3::stack_wstring wstrMsg;
			/*축하드립니다!!\n \n호칭 슬롯을 획득하여 호칭을 동시에\n사용할 수 있는 개수가 증가하였습니다.\n(최대 %d종의 호칭 사용 가능)*/
			i3::sprintf(wstrMsg, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SLOT_MESSAGE"), m_nCurDesignationSlotCount);
			GameUI::MsgBox( MSG_TYPE_GAME_OK,  wstrMsg);
		}
	}

	// 2. 획득 호칭 UI 표시 변경..
	{
		DesigUtil::AcquireGainedDesigSetsForUI( m_vecGetDesignation, MAX_DEFAULT_USER_TITLE_COUNT);

		COLOR tColor;	tColor.a = 255;
		const size_t num_GetDesig = m_vecGetDesignation.size();
		for (size_t i = 0 ; i < num_GetDesig ; ++i)
		{
			tColor.r = tColor.g = tColor.b = ( m_vecGetDesignation[i] ) ? 255 : 33;
			m_pImageBox[i]->setColor(&tColor);
			m_pButtonImage[i]->setColor(&tColor);
		}
	}

	// 3. 상세 정보 박스 변경들..
	_Update_DetailInfoBox();
	_Update_GetInfoBox();
	
	// 4. 내 정보 호칭 변경..
	if( IsReadyToUpdate() )
	{
		const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

		INT32 iLimitTitleCount = myDesigInfo->GetLimitCount();
		
		I3ASSERT( iLimitTitleCount <= MAX_EQUIP_USER_TITLE);
		
		for( INT32 i = 0; i < iLimitTitleCount; ++i)
		{
			fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
			fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,false);//Enable true, false
		}
		
		for( INT32 i = iLimitTitleCount; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
			fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,true);
		}

		// 호칭이미지 및 호칭이름.
		for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; ++i)
		{
			UINT8 myEquipDesigSet = myDesigInfo->GetEquipDesig(i);

			if( myEquipDesigSet > MAX_DEFAULT_USER_TITLE_COUNT)
				continue;
		
			const DesigSet* desigSet = DesigUtil::FindDesigSetByType(myEquipDesigSet);

			if (desigSet)
			{
				fnSetMyInfoDesignation_Image_Enable(GetScene(0), i, true);
				fnSetMyInfoDesignation_Image_Index(GetScene(0), i, desigSet->desigSetUIIndex );
			}
		}		
		
	}

	// 5. 장착된 호칭 강조.
	_Update_DesignationTree();					
}

void UITabProfileDesignation_V11::_Update_DetailInfoBox()
{
	if( IsReadyToUpdate() == false) return;

	i3::stack_wstring wstrTemp1;
	i3::stack_wstring wstrTemp2;
//	char szName_Explain[ MAX_STRING_COUNT];			// 이상하게 1.1에서는 Explain이 빠져있음..

	if( (m_SelectedDesignationIdx != -1) && (m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT))
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByUI(m_SelectedDesignationIdx);

		m_nCurShowingDesignationType = desigSet->desigSetType;
	
		// 이미지 인덱스 루아로 전달..
		{
			LuaState * L = _CallLuaFunction("SetDesignImage");
			i3Lua::PushInteger(L, m_SelectedDesignationIdx);
			_EndLuaFunction(L, 1);
		}
	
		// 호칭 이름 표시..
		{
			i3::sprintf( wstrTemp1, L"{col:80,150,215,255}%s{/col}\n{col:204,204,204,255}%s{/col}", desigSet->wstrName, desigSet->wstrNameE );
			LuaState * L = _CallLuaFunction("SetDesignTitle");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}
		
			// Plus Effect
		wstrTemp1.clear();		wstrTemp2.clear();

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
			LuaState * L = _CallLuaFunction("SetPlusEffectString");
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
			LuaState * L = _CallLuaFunction("SetMinusEffectString");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}
		
		// Add Effect  (호칭 기능인 경우도 있기 때문에 버릴수는 없을 것임..)
		// 일단 서버 정보에 기인하여 호칭 구매 가능 아이템 정보부터 입수한다..
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
			LuaState * L = _CallLuaFunction("SetAddEffectString");
			i3Lua::PushStringUTF16To8(L, wstrTemp1);
			_EndLuaFunction(L, 1);
		}

	}
	else
	{
		m_nCurShowingDesignationType = 0;

		// 이미지 인덱스 루아로 전달..
		{
			LuaState * L = _CallLuaFunction("SetDesignImage");
			i3Lua::PushInteger(L, -1);		// -1전달.. (아무것도 선택 안되는 것?)
			_EndLuaFunction(L, 1);
		}

		// 호칭 이름 표시 ( 비워준다..)
		{
			LuaState * L = _CallLuaFunction("SetDesignTitle");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetPlusEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetMinusEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}

		{
			LuaState * L = _CallLuaFunction("SetAddEffectString");
			i3Lua::PushStringUTF16To8(L, L"");
			_EndLuaFunction(L, 1);
		}
	}
}



void UITabProfileDesignation_V11::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
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


//
void UITabProfileDesignation_V11::__UpdateInfoFromLuaGlue()
{
	if( IsReadyToUpdate() == false) return;

	if( m_SelectedDesignationIdx >=0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
		__OnDoubleClickedDesignationButton();
		_Update_DesignationTree();	
	}
	else
	{
		GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_DESIGNATION_GET_SELECT"));
	}
}


/*virtual*/ bool UITabProfileDesignation_V11::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /*= I3_EVT_CODE_ACTIVATE*/ )
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
		bool bCheckBox = false;

		for(INT32 i=0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
		{
			if(m_pCheckBox[i] == pParam->m_pCtrl)
			{
				bCheckBox = true;
				break;
			}
		}
		
		if(	bCheckBox == false ) 	
		{
			if(pParam->m_nEvent == I3UI_EVT_CLICKED)
				I3TRACE(pParam->m_pCtrl->GetName());
			return UITabProfileBase_V11::OnEvent( event, pObj, param2, code);
		}

		if (pParam->m_nEvent == I3UI_EVT_BTN_PUSHED)
		{
			I3ASSERT(pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			if (m_SelectedDesignationIdx == curSel)		// 같은걸 눌렀을땐 셀렉션 유지해야됨..
			{
				m_pCheckBox[m_SelectedDesignationIdx]->addState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_SelectedDesignationIdx]->setModifyState(true);
			}
		}
		else
		if( pParam->m_nEvent == I3UI_EVT_DBL_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			__SetCurrentButton(curSel);

			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
		
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
			__OnDoubleClickedDesignationButton();
			_Update_DesignationTree();
		}
		else if( pParam->m_nEvent == I3UI_EVT_CLICKED)
		{
			I3ASSERT( pParam->m_pCtrl != nullptr);
			INT32 curSel = i3::atoi(pParam->m_pCtrl->GetName());

			__SetCurrentButton(curSel);

			I3TRACE("Old_Select_Idx : %d\n", m_OldSelectedDesignationIdx);
			I3TRACE("Current_Select_Idx : %d\n", m_SelectedDesignationIdx);
			if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
				m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
			}
			_Update_DetailInfoBox();
			_Update_GetInfoBox();
		}

	}

	return UITabProfileBase_V11::OnEvent( event, pObj, param2, code);
}

/*virtual*/ void UITabProfileDesignation_V11::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase_V11::OnCreate( pParent);

	char * pszPath = "Scene/Main/PointBlankRe_MyInfomation_Designation.i3UIs";
	AddScene( pszPath,UITabProfileDesignation_V11_Glue);
}

void UITabProfileDesignation_V11::_Update_GetInfoBox( void)
{	
	if( IsReadyToUpdate() == false) return;

	// 뱃지는 임무카드와 관련이 있기 때문에, 누락된 구현분이 있다면 임무카드 처리할때 같이 할 것...(2015.05.29.수빈)
	// 뱃지 표시..

	i3::stack_wstring wstrCount;

	{
		struct SetDesigAppendixString
		{
			void operator()( i3UIScene* pScene, BADGE_TYPE badgeType, const i3::stack_wstring& wstr) const
			{
				LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignAppendixString");
				i3Lua::PushInteger(L, badgeType);
				i3Lua::PushStringUTF16To8(L, wstr);;
				UILua::EndLuaFunction(L, 2);
			}
		} fnSetDesigAppendixString;

	
		// 약장
		UINT32 ribbonCount = MedalContext::i()->getMiniature();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), ribbonCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_RIBBON, wstrCount);
		
		// 휘장
		UINT32 InsigniaCount = MedalContext::i()->getInsignia();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), InsigniaCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_ENSIGN, wstrCount);
		
		// 훈장
		UINT32 orderCount = MedalContext::i()->getOrder();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), orderCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_MEDAL, wstrCount);
		//
		// 마스터 훈장
		UINT32 masterCount = MedalContext::i()->getMaster();
		i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_APPENDIX_BADGECOUNT_TEXT"), masterCount);
		fnSetDesigAppendixString(GetScene(0), BADGE_TYPE_MASTER, wstrCount);
	}

	struct SetDesignMedalCount
	{
		void operator()( i3UIScene* pScene, INT32 idx, const i3::stack_wstring& wstrCount) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignMedalCount");
			i3Lua::PushInteger(L, idx);
			i3Lua::PushStringUTF16To8(L, wstrCount);
			UILua::EndLuaFunction(L, 2);
		}
	} fnSetDesignMedalCount;

	struct SetNeedText
	{
		void operator()( i3UIScene* pScene, UINT32 idx, const i3::stack_wstring& wstrDesigName) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignNeedText");
			i3Lua::PushInteger(L, idx);
			i3Lua::PushStringUTF16To8(L, wstrDesigName);
			UILua::EndLuaFunction(L, 2);
		}
	} fnSetNeedText;

	struct SetDesignNeedRankText
	{
		void operator()( i3UIScene* pScene, const i3::stack_wstring& wstrNeedRank) const
		{
			LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignNeedRankText");
			i3Lua::PushStringUTF16To8(L, wstrNeedRank);
			UILua::EndLuaFunction(L, 1);
		}
	} fnSetDesignNeedRankText;

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

	if( m_SelectedDesignationIdx != -1)
	{
		struct SetDesignationSetCaption
		{
			void operator()( i3UIScene* pScene, UINT32 CState) const
			{
				LuaState * L = UILua::CallLuaFunction(pScene, "SetDesignationSetCaption");
				i3Lua::PushInteger(L, CState);
				UILua::EndLuaFunction(L, 1);
			}
		} fnSetDesignationSetCaption;

		{
			// 호칭 획득/사용/해제
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

		if( m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			const DesigSet* pDesigSet = DesigUtil::FindDesigSetByUI( m_SelectedDesignationIdx );
			INT32 desigSetType = pDesigSet->desigSetType;

			// 1.필요 표장 설정
		
			INT32 iCount[ BADGE_TYPE_COUNT] = {0,};
		
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
				fnSetDesignMedalCount(GetScene(0), i, wstrCount);
			}
				
			// 2. 필요 호칭 설정

			for( INT32 i = 0; i < 2; i++)
			{
				UINT8 reqDesigSetType = g_UserTitleRequirement[desigSetType]._userTitleId[i];

				const DesigSet* reqSet = DesigUtil::FindDesigSetByType( reqDesigSetType );
				
				if( reqSet )
					fnSetNeedText(GetScene(0), i+1, reqSet->wstrName);
				else
					fnSetNeedText(GetScene(0), i+1, L"----");

			}
	
			// 3. 필요 계급 설정
			i3::stack_wstring wstrRankText = GetCharaRank(g_UserTitleRequirement[desigSetType]._rank);
			fnSetDesignNeedRankText(GetScene(0), wstrRankText);

		}
	}
	else
	{
		for(INT32 i = 0; i < BADGE_TYPE_COUNT; ++i)
		{
			i3::sprintf( wstrCount, GAME_RCSTRING("STR_TBL_DESIGNATION_NEEDED_BADGECOUNT_TEXT"), 0);
			fnSetDesignMedalCount(GetScene(0), i, wstrCount);
		}

		fnSetNeedText(GetScene(0), 1, L"{col:204, 204, 204, 255}----{/col}");
		fnSetNeedText(GetScene(0), 2, L"{col:204, 204, 204, 255}----{/col}");
		fnSetDesignNeedRankText(GetScene(0), L"{col:204, 204, 204, 255}----{/col}");
		fnSetDesignationSetButton(GetScene(0), false);
	}

}

void UITabProfileDesignation_V11::__OnDoubleClickedDesignationButton()
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
		// 호칭 획득  OnDesignationGet();
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

bool UITabProfileDesignation_V11::_CheckDesignationEquip(void)
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

void UITabProfileDesignation_V11::__OnDesignationChange()
{
	//빈슬롯을 찾아야 한다.
	//빈슬롯의 인덱스를 받아온다.
	//3칸이 비어있다면 가장 좌측부터.
	//

	m_SelectedDesignationChangeSlotIdx = GetEmptySlot();
	
	if( m_SelectedDesignationChangeSlotIdx >= 0 && m_SelectedDesignationChangeSlotIdx < 3)
	{
		CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();
//		const INT32 numDesigSetType = DesigUtil::GetNumDesigSetByType();

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

void UITabProfileDesignation_V11::_Update_DesignationTree()
{
	if( IsReadyToUpdate() == false)return;

	// 획득한 호칭에 대하여는 항상 특정한 표시를 합니다.
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo();

	UINT8 TitleBuffer[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(TitleBuffer);

	//장착된 호칭 파란 테두리 효과 3개까지 유지됩니다.
	// OnActivate -> CheckBox 이벤트 핸들링을 막아놓음으로서 렌더링 스테이트값을 고정시킵니다. 추후에 다른컨트롤로 수정해야함(파란 테두리 유지 효과)
	
	

	bool deactviated_idx[MAX_DEFAULT_USER_TITLE_COUNT];
	memset(&deactviated_idx, 0, sizeof(deactviated_idx));
	


	for( INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		const DesigSet* desigSet = DesigUtil::FindDesigSetByType(TitleBuffer[i]);
		if (desigSet)
		{
			INT32 idxUI = desigSet->desigSetUIIndex;
			if (idxUI < MAX_DEFAULT_USER_TITLE_COUNT)
			{
				deactviated_idx[idxUI] = true;
			//	m_pCheckBox[idxUI]->OnActivate(false);
			}
		}
	}
	
	for (INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if (deactviated_idx[i] == true)
		{
			m_pCheckBox[i]->OnActivate(false);
		}
		else
		{
			m_pCheckBox[i]->OnActivate(true);
		}
	}


}


bool UITabProfileDesignation_V11::OnEntranceStart( void * pArg1, void * pArg2 )
{
	if( UITabProfileBase_V11::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	LuaState * L = _CallLuaFunction("InitUI");
	_EndLuaFunction(L,0);

	m_SelectedDesignationIdx = -1;
	// 호칭 이미지를 위치에 맞게 호출합니다.
	for(INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		m_pButtonImage[i]->SetShapeIdx(i);
	}

	for(INT32 i = 0; i < MAX_EQUIP_USER_TITLE; i++) //좌측 하단 장착칸의 이미지를 초기화 합니다.
	{
		fnSetMyInfoDesignation_Image_Enable(GetScene(0), i,false);
		fnSetMyInfoDesignation_Image_Lock(GetScene(0), i,true);
	}

	_Update_All();

	return true;
}


bool UITabProfileDesignation_V11::OnClick( i3UIControl * pControl)
{
	if( UITabProfileBase_V11::OnClick( pControl) == false)
		return false;

	if( i3::same_of<i3UICheckBox*>(pControl)) 
		PlaySound( UISND_CLICK);

	return true;
}

bool UITabProfileDesignation_V11::OnEnterMouse( i3UIControl * pControl)
{
	if( UITabProfileBase_V11::OnEnterMouse( pControl) == false)
		return false;

	if( i3::generic_is_iequal( pControl->GetNameString(), "i3UIButtonConfirm") )
		PlaySound( UISND_ENTER_MOUSE1);

	return true;
}

void UITabProfileDesignation_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UITabProfileBase_V11::_InitializeAtLoad( pScene);

	I3PRINTLOG(I3LOG_NOTICE, "ProfileDesignation Loading....\n");

	i3UIFrameWnd * pFrameWndIcon = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd3_MID");

	if( pFrameWndIcon != nullptr)
	{
		m_pCheckBoxTemplate = i3UI::getUILibrary()->FindByName("CheckBox_Designation");
		VEC3D* pos;
		char szCountName[ MAX_STRING_COUNT];
		INT32 Count1 = 0, Count2 = 0;
		INT32 ChildCount = pFrameWndIcon->GetChildCount();
		i3UIControl * pChild = (i3UIControl *)pFrameWndIcon->getFirstChild();
		
		for(INT32 i = 0; i < ChildCount ; i++)
		{
			if(i3::kind_of<i3UIImageBox*>(pChild)) 
			{
				m_pImageBox[Count1] = (i3UIImageBox *)pChild;
				++Count1;
			}
			else if(i3::kind_of<i3UIButtonImageSet*>(pChild)) 
			{
				m_pButtonImage[Count2] = (i3UIButtonImageSet *)pChild;
				m_pButtonImage[Count2]->SetViewOnly(true);
				++Count2;
			}
			pChild = (i3UIControl *)pChild->getNext();
		}

		pChild = (i3UIControl *)pFrameWndIcon->getFirstChild();
		Count1 = Count2 = 0;

		VEC3D tPos;

		for(INT32 i = 0; i < ChildCount ; i++)
		{
			if(i3::kind_of<i3UIButtonImageSet*>(pChild)) 
			{
				//체크박스 컨트롤 생성
				i3UICheckBox * pChkBox = (i3UICheckBox*) i3UI::CreateControl( m_pCheckBoxTemplate, &tPos, pChild->getSize(), pChild);

				i3::itoa(Count2, szCountName, _countof(szCountName));
				++Count2;
				pChkBox->SetName( szCountName);
				pChkBox->SetForceEnableInput(true);

				m_pCheckBox[Count1] = pChkBox;

				pos = pChild->getPos();
				m_pImageBox[Count1]->setPos(pos);
				m_pImageBox[Count1]->setSize(pChild->getSize());
				++Count1;

			}
			pChild = (i3UIControl *)pChild->getNext();
		}

	}

	//Lock(자물쇠)이미지와 호칭 장착/해체 이벤트를 위해 더미 이벤트 컨트롤 생성, 투명하게 처리한다.
	COLOR tColor;
	tColor.a = 0;
	tColor.r = tColor.g = tColor.b = 0;
	m_dummyButton[0] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton1");
	m_dummyButton[0]->setColor(&tColor);
	m_dummyButton[1] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton2");
	m_dummyButton[1]->setColor(&tColor);
	m_dummyButton[2] = (i3UIImageBox *)pScene->FindChildByName("i3UIDummyButton3");
	m_dummyButton[2]->setColor(&tColor);
	
	const CUserDesigInfo* userDesigInfo = DesigUtil::GetMyUserDesigInfo();
	m_nCurDesignationSlotCount = userDesigInfo->GetLimitCount();

}

//
void UITabProfileDesignation_V11::OnLoadAllScenes()
{
	GetSlide()->AddFrameWnds(GetScene(0));
}

void UITabProfileDesignation_V11::OnUnloadAllScenes()
{
}

void UITabProfileDesignation_V11::_ClearAtUnload( i3UIScene * pScene)
{
	for(INT32 i = 0; i < MAX_DEFAULT_USER_TITLE_COUNT; i++)
	{
		if( m_pCheckBox[ i] != nullptr)
		{
			i3UI::DeleteControl( m_pCheckBox[ i]);
			m_pCheckBox[ i] = nullptr;
		}
	}

	UITabProfileBase_V11::_ClearAtUnload( pScene);
}

void UITabProfileDesignation_V11::__SetCurrentButton(INT32 idx)
{
	if(idx != m_SelectedDesignationIdx)
	{
		m_OldSelectedDesignationIdx = m_SelectedDesignationIdx;
		m_SelectedDesignationIdx = idx;
	}
}

void UITabProfileDesignation_V11::__MyDesigClickFromLuaGlue(INT32 idx)
{
	m_MySlotIdx = idx;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);
	
	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[idx] );

	if (desigSet)
	{
		INT32 idxUI = desigSet->desigSetUIIndex;
		__SetCurrentButton( idxUI );
		if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
			m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
		}
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
	}
}

void UITabProfileDesignation_V11::__MyDesigDoubleClickFromLuaGlue(INT32 idx)
{
	m_MySlotIdx = idx;

	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);

	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[idx] );

	if (desigSet)
	{
		INT32 idxUI = desigSet->desigSetUIIndex;
		__SetCurrentButton( idxUI );

		if( m_OldSelectedDesignationIdx >= 0 && m_OldSelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
		{
			m_pCheckBox[m_OldSelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
			m_pCheckBox[m_OldSelectedDesignationIdx]->setModifyState(true);
		}
		_Update_DetailInfoBox();
		_Update_GetInfoBox();
		__OnDoubleClickedDesignationButton();
		_Update_DesignationTree();	
	}
	else
	{
		__OnDoubleClickedDesignationButton();
	}
}


void UITabProfileDesignation_V11::__MyEquipReleaseFromLuaGlue(INT32 slotindex)
{

	if( m_SelectedDesignationIdx >= 0 && m_SelectedDesignationIdx < MAX_DEFAULT_USER_TITLE_COUNT)
	{
		m_pCheckBox[m_SelectedDesignationIdx]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[m_SelectedDesignationIdx]->setModifyState(true);
	}
	
	const CUserDesigInfo* myDesigInfo = DesigUtil::GetMyUserDesigInfo(); 
	UINT8 myDesigEquipTypes[MAX_EQUIP_USER_TITLE];
	myDesigInfo->GetEquipDesig(myDesigEquipTypes);

	const DesigSet* desigSet = DesigUtil::FindDesigSetByType( myDesigEquipTypes[slotindex] );

	if (desigSet == nullptr)
	{
		m_SelectedDesignationIdx = -1;
		__UpdateInfoFromLuaGlue();
		return;
	}
	
	m_SelectedDesignationIdx = desigSet->desigSetUIIndex;
	__UpdateInfoFromLuaGlue();

}
