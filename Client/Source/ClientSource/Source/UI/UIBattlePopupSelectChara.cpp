#include "pch.h"
#include "UIBattlePopupSelectChara.h"

#include "UIBattleFrame.h"

#include "../../StageBattle.h"
#include "../BattleGui.h"

#include "UIShopFunction.h"
#include "UIInvenInfoMgr.h"

#include "UIHudRespawnTimer.h"
#include "UIHudDefine.h"
#include "UIHudManager.h"
#include "UIUtil.h"

#include "../../Shop.h"
#include "../InvenUtil.h"

#include "MyRoomInfoContext.h"

//#if 0
//Lus Glue Function

extern "C" {
	int			Slot_Prev( LuaState * L)
	{
		UIBattlePopupSelectChara * pThis = static_cast<UIBattlePopupSelectChara*>(GameUI::GetBattlePopup(UBP_SELECT_CHARA));
		if( pThis != nullptr)
			pThis->MoveSlot( UIBattlePopupSelectChara::PREV);
		return 0;
	}

	int			Slot_Next( LuaState * L)
	{
		UIBattlePopupSelectChara * pThis = static_cast<UIBattlePopupSelectChara*>(GameUI::GetBattlePopup(UBP_SELECT_CHARA));
		if( pThis != nullptr)
			pThis->MoveSlot( UIBattlePopupSelectChara::NEXT);
		return 0;
	}

	int			Select_Ok( LuaState * L)
	{
		UIBattlePopupSelectChara * pThis = static_cast<UIBattlePopupSelectChara*>(GameUI::GetBattlePopup(UBP_SELECT_CHARA));
		if( pThis != nullptr)
			pThis->SelectOk();
		return 0;
	}

	int			Select_Cancel( LuaState * L)
	{
		UIBattlePopupSelectChara * pThis = static_cast<UIBattlePopupSelectChara*>(GameUI::GetBattlePopup(UBP_SELECT_CHARA));
		if( pThis != nullptr)
			pThis->SelectCancel();
		return 0;
	}
};

LuaFuncReg UIIngameSelectChara_Glue[] = 
{
	{"ClickPrev",				Slot_Prev},
	{"ClickNext",				Slot_Next},
	{"ClickOk",					Select_Ok},
	{"ClickCancel",				Select_Cancel},
	{ nullptr,						nullptr}
};




I3_CLASS_INSTANCE( UIBattlePopupSelectChara);

UIBattlePopupSelectChara::UIBattlePopupSelectChara(/*CBattleGui* pGui*/) :
m_nCurrentIdx(-1), m_SelectedIdx(-1)
{
	for( UINT8 cnt = 0; cnt < MAX; cnt++)
	{
		m_pSlot[cnt] = new CCharacterSlot();
		MEMDUMP_NEW( m_pSlot[cnt], sizeof( CCharacterSlot));
	}
}

UIBattlePopupSelectChara::~UIBattlePopupSelectChara(void)
{
	for( UINT8 cnt = 0; cnt < MAX; cnt++)
	{
		I3_SAFE_DELETE( m_pSlot[cnt]);
	}
}

/*virtual*/ void UIBattlePopupSelectChara::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/HUD/PointBlankRe_InGame_SelectChara.i3UIs", UIIngameSelectChara_Glue);

	m_List.clear();
	m_SelectedIdx = m_nCurrentIdx = -1;

	const COMMON_PARTS_INFO* pCommonInfo = CCharaInfoContext::i()->GetCommonPartsInfo();
	T_ItemID dino_id = pCommonInfo->m_Info[CHAR_EQUIPMENT_COMMON_DINO].m_TItemID;

	ItemInfoList List;
	inven_util::get_dino_inven_list( List);

	for( size_t cnt = 0; cnt < List.size(); ++cnt)
	{
		if( _IsAvailble( List[cnt].m_TItemID))
		{	
			//장비 한 아이템 인덱스.
			if( List[cnt].m_TItemID == dino_id)
			{
				m_SelectedIdx = m_nCurrentIdx = m_List.size();
			}
			m_List.push( List[cnt]);
		}
	}
}

/*virtual*/	void UIBattlePopupSelectChara::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_BLOCK);
}

/*virtual*/ bool UIBattlePopupSelectChara::OnEntranceStart( void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UIPopupBase::OnEntranceStart(pArg1, pArg2) == false) return false;
	
	GameUI::SetMouseCenterPosition();

	m_nCurrentIdx = m_SelectedIdx;
	_Refresh();

	return true;
}

/*virtual*/	bool UIBattlePopupSelectChara::OnExitStart()
{
	UIBattlePopupBase::OnExitStart();

	//슬라이드 아웃 하는 동안 다시 (T키) Enable(true) 할 경우 안켜지기 때문에 슬라이드 안함.
	//if( UIBattlePopupBase::OnExitStart() == false) return false;

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_UNBLOCK);

	// 한소프트 7347. 교체창이 활성화 되어있으면 전적창은 출력하지 않음.
	if (UIBattleFrame::i()->GetHUD(UIHUD_RECORD_BOARD)->isEnable())
		UIBattleFrame::i()->EnableHUD(UIHUD_RECORD_BOARD, false);

	return true;
}

/*virtual*/	bool UIBattlePopupSelectChara::OnKey_Enter()
{
	if( SelectOk()) return true;

	return UIBattlePopupBase::OnKey_Enter();
}

/*virtual*/ bool UIBattlePopupSelectChara::OnKey_Escape()
{
	if( SelectCancel()) return true;

	return UIBattlePopupBase::OnKey_Escape();
}


/*virtual*/ bool UIBattlePopupSelectChara::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIBattlePopupBase::OnKeyInput(pInputMng) == true) return true;
	
	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();
	
	if( pKeyboard->GetStrokeState(I3I_KEY_LEFT))
	{
		MoveSlot(PREV);
	}

	if( pKeyboard->GetStrokeState(I3I_KEY_RIGHT))
	{
		MoveSlot(NEXT);
	}

	return false;
}

/*virtual*/ void UIBattlePopupSelectChara::ProcessGameEvent( GAME_EVENT Event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent( Event, Arg, UserData);
}

/*virtual*/	void UIBattlePopupSelectChara::OnUpdate( REAL32 rDeltaSeconds)
{
	UIPopupBase::OnUpdate( rDeltaSeconds);
}

bool UIBattlePopupSelectChara::_IsAvailble( INT32 nItemId)
{
	bool bAvailable = true;

	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT))
	{//Cross Counter 모드 일 경우 탈출 미션 공룡은 사용 할 수 없음.
		INT32 nNumber = GET_ITEM_NUMBER(nItemId);
		if( _IsEscapeModeDino( CHARA_RES_ID(nNumber)))
			return false;
	}

	//내구도일 경우 내구도확인
	REAL32	rRate = 0.0f;
	if( usf::GetDurability( nItemId, 0, rRate))
	{
		bAvailable = (rRate > 0.f);			//내구도 캐릭터의 내구도가 0일 경우 사용 못함.
	}

	// [12.11.21 woori.han]
	// 기간제 캐릭터 미사용 상태인지 확인
	UINT8	Type	= 0;
	UINT32	Arg		= 0;
	
	if( inven::GetItemTypeArg( nItemId, &Type, &Arg))
	{
		CShop * pShop = CShop::i(); 
		if (pShop == nullptr)
		{
			return bAvailable;
		}

		UINT8 authType = pShop->GetAuthType(nItemId);

		if (ITEM_ATTR_TYPE_BUY == Type && ITEM_TYPE_TIME == authType)
		{
			bAvailable = false;	// 기간제 캐릭터가 미사용일 경우 사용 못함
		}
	}

	return bAvailable;
}

void UIBattlePopupSelectChara::_Refresh()
{
	//Info
	INT32 nCharaDinoCount = m_List.size();

	INT32 nItemId;
	INT8 nHalf = (INT8)(MAX * 0.5f);
	INT32 nMax = (MAX > nCharaDinoCount) ? (MAX) : (nCharaDinoCount);

	INT32 nSlotIdx = m_nCurrentIdx;
	for( INT8 cnt = 0; cnt < nHalf; ++cnt)
		nSlotIdx = LOOP_DECREASE( nSlotIdx, 0, nMax - 1);

	for( UINT8 cnt = 0; cnt < MAX; ++cnt)
	{
		if( nSlotIdx >= nCharaDinoCount)
		{
			m_pSlot[cnt]->SetInfo(nullptr);
		}
		else
		{
			nItemId = m_List[nSlotIdx].m_TItemID;
			const CCharaInfo* pInfo = g_pCharaInfoDataBase->GetCharaDBInfo( nItemId);

			m_pSlot[cnt]->SetInfo( pInfo, nItemId);
		}

		//UI Refresh
		_RefreshSlot( cnt, m_pSlot[cnt]);

		if( cnt == CURRENT)
		{
			_RefreshDesc( m_pSlot[cnt]);
			_RefreshSkill( m_pSlot[cnt]);
		}

		nSlotIdx = LOOP_INCREASE( nSlotIdx, 0, nMax - 1);
	}	
}

void UIBattlePopupSelectChara::_RefreshSlot( INT8 iSlotIdx, CCharacterSlot* pSlot)
{	
	LuaState* L = nullptr;
	if( pSlot->IsEmpty())
	{ 
		L = _CallLuaFunction("EmptySlot");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, iSlotIdx);
			_EndLuaFunction(L, 1);
		}
	}
	else
	{
		L = _CallLuaFunction("UpdateSlot");
		if( L != nullptr)
		{
			CCharaInfo* pInfo = const_cast<CCharaInfo*>(pSlot->GetInfo());

			i3::rc_wstring wstrName;
			REAL32 fDurabilityRatio = 0.f;
			bool bDurability = usf::GetDurability( pSlot->GetItemId(), 0, fDurabilityRatio );
			
			pInfo->getName(wstrName);

			//구 UI는 ShapeName으로 UI 설정.(템플릿이 다름)
			static const bool bUseShapeName = false;
			wchar_t wszShape[64] = {0,};
			
			i3Lua::PushInteger(L, iSlotIdx);
			i3Lua::PushBoolean(L, bUseShapeName);
		
			if( bUseShapeName)
				i3Lua::PushStringUTF16To8(L, wszShape);				// TODO : 확인 요망한다.  szShape에 들어가는 값이 없는 상태이다...(2014.07.23.수빈)
			else
				i3Lua::PushInteger(L, pInfo->GetUIShapeIndex());
					
			i3Lua::PushStringUTF16To8(L, wstrName);
			i3Lua::PushBoolean(L, bDurability);
			i3Lua::PushNumber(L, fDurabilityRatio);

			_EndLuaFunction(L, 6);
		}
	}
}

void UIBattlePopupSelectChara::_RefreshDesc( CCharacterSlot* pSlot)
{
	if( pSlot->IsEmpty()) return;

	LuaState* L = _CallLuaFunction("UpdateDesc");
	if( L != nullptr)
	{
		CCharaInfo* pInfo = const_cast<CCharaInfo*>(pSlot->GetInfo());
		i3::rc_wstring wstrName;
		i3::rc_wstring wstrDesc;
	
		pInfo->getName(wstrName);
		pInfo->getDescription( wstrDesc);

		i3Lua::PushStringUTF16To8(L, wstrName.c_str(), wstrName.size() );
		i3Lua::PushStringUTF16To8(L, wstrDesc.c_str(), wstrDesc.size() );
		_EndLuaFunction(L, 2);
	}
}

void UIBattlePopupSelectChara::_RefreshSkill( CCharacterSlot* pSlot)
{
	if( pSlot->IsEmpty()) return;

	INT32 nSkillShapeIndx[SKILL_MAX] = {-1,};
	i3::rc_wstring awstrSkillDesc[SKILL_MAX];

	CCharaInfo* pInfo = const_cast<CCharaInfo*>(pSlot->GetInfo());

	for( UINT8 cnt = 0; cnt < SKILL_MAX; ++cnt)
	{
		nSkillShapeIndx[cnt] = pInfo->getShapeIndexCCSkill( cnt);
		pInfo->getCCSkillDescription( awstrSkillDesc[cnt], cnt);

		LuaState* L = _CallLuaFunction("UpdateSkill");
		if( L != nullptr)
		{
			i3Lua::PushInteger(L, cnt);
			i3Lua::PushInteger(L, nSkillShapeIndx[cnt]);
			i3Lua::PushStringUTF16To8(L, awstrSkillDesc[cnt]);
			_EndLuaFunction(L, 3);
		}
	}
}

void UIBattlePopupSelectChara::ResetFocus()
{
	g_pFramework->ResetFocus();
	i3UIManager* pUIManager = g_pFramework->getUIManager();

	if( pUIManager != nullptr)
		pUIManager->resetOnMouseTarget();

	i3Viewer* pViewer = g_pFramework->GetViewer();
	SetFocus( pViewer->GetWindowHandle());
}

void UIBattlePopupSelectChara::MoveSlot( INT8 nType)
{
	INT32 nCharaDinoCount = m_List.size();

	INT32 nMax = (MAX > nCharaDinoCount) ? (MAX) : (nCharaDinoCount);
	INT32 nSlotIdx = -1;

	switch( nType)
	{
	case PREV:
		nSlotIdx = (INT8)LOOP_DECREASE( m_nCurrentIdx, 0, nMax - 1);
		break;

	case NEXT:
		nSlotIdx = (INT8)LOOP_INCREASE( m_nCurrentIdx, 0, nMax - 1);
		break;

	default:
		I3ASSERT_0;
		break;
	}
	
	if( nSlotIdx < nCharaDinoCount && nSlotIdx >= 0)
	{
		m_nCurrentIdx = nSlotIdx;
		_Refresh();
	}
}

bool UIBattlePopupSelectChara::SelectOk()
{
	ResetFocus();
	CBattleGui::i()->OnGameStart();
	
	INT32 nCharaDinoCount = m_List.size();
	
	//케릭터 정보 저장
	if( m_nCurrentIdx < nCharaDinoCount && m_nCurrentIdx >= 0)
	{
		//COMMON_PARTS_INFO* pCommonInfo = CCharaInfoContext::i()->SetCommonPartsInfo();
		//pCommonInfo->m_Dino = m_List[m_nCurrentIdx];

		COMMON_PARTS_INFO* pActiveCommonInfo = CCharaInfoContext::i()->GetActiveCommonPartsInfo();
		pActiveCommonInfo->m_Info[CHAR_EQUIPMENT_COMMON_DINO] = m_List[m_nCurrentIdx];
		
		m_SelectedIdx = m_nCurrentIdx;

		CBattleGui::i()->OnSelectWeaponClose();
		return true;
	}

	I3ASSERT(m_nCurrentIdx < nCharaDinoCount && m_nCurrentIdx >= 0);
	return false;
}

bool UIBattlePopupSelectChara::SelectCancel()
{
	ResetFocus();
	CBattleGui::i()->OnSelectWeaponClose();

	return true;
}
//#endif