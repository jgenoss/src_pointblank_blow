#include "pch.h"
#include "UIPhaseCreateChara.h"
#include "UIPopupNickNameSetting.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UICharaView.h"

#include "../GameGUI.h"
#include "../CommunityContext.h"

#include "i3Base/string/ext/generic_string_ncopy.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/format_string.h"

#include "GameLoginContext.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

extern "C" {

	int UIPhaseCreateChara_NextButton( LuaState * L)
	{
		UIPhaseCreateChara * pThis = static_cast<UIPhaseCreateChara*>(GameUI::GetPhase(UIPHASE_CREATE_CHARA));
		if( pThis != nullptr)
			pThis->Click_Next();
		return 0;
	}
	int UIPhaseCreateChara_BackButton( LuaState * L)
	{
		UIPhaseCreateChara * pThis = static_cast<UIPhaseCreateChara*>(GameUI::GetPhase(UIPHASE_CREATE_CHARA));
		if( pThis != nullptr)
			pThis->Click_Back();
		return 0;
	}

	int UIPhaseCreateChara_SelectChara( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);

		UIPhaseCreateChara * pThis = static_cast<UIPhaseCreateChara*>(GameUI::GetPhase(UIPHASE_CREATE_CHARA));
		if( pThis != nullptr)
			pThis->Click_Chara(idx);
		return 0;
	}
	int UIPhaseCreateChara_SelectClass( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		UIPhaseCreateChara * pThis = static_cast<UIPhaseCreateChara*>(GameUI::GetPhase(UIPHASE_CREATE_CHARA));
		if( pThis != nullptr)
			pThis->Click_Class( (CHARA_CLASS) (idx));
		return 0;
	}
}	// extern "C"


LuaFuncReg PhaseCreateChara_Glue[] = 
{
	{"NextButton",					UIPhaseCreateChara_NextButton},
	{"PreButton",					UIPhaseCreateChara_BackButton},
	{"SelectChara",					UIPhaseCreateChara_SelectChara},
	{"SelectClass",					UIPhaseCreateChara_SelectClass},
	{nullptr, nullptr}
};

//----------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPhaseCreateChara);//, UIPhaseBase);

UIPhaseCreateChara::UIPhaseCreateChara(void)
{
	m_CurrentClass = CHARA_CLASS_ASSAULT;

	m_CurrentCreateIndex = 0;

	m_PageState = CREATECHARA_STATE_CHARA;

	m_UsedSlotCount = 0;
	m_pmbCreateCharaWait = nullptr;
	
	i3mem::FillZero( m_nItemId, sizeof( m_nItemId));

	i3mem::FillZero( m_pCharaSlot, sizeof( m_pCharaSlot));

	Reset(&m_CharaInfo);
	Reset(&m_TempInfo);
	
	m_nItemId[0] = 601001;		// 레드불스
	m_nItemId[1] = 601003;		// 타란튤라
	m_nItemId[2] = 602002;		// 에시드폴
	m_nItemId[3] = 602004;		// 킨아이즈

	m_bSendCreate = false;

	RegisterGameEventFunc( EVENT_CREATE_CHARA,			&UIPhaseCreateChara::ReceiveGameEvent_Create_Character);
	RegisterGameEventFunc( EVENT_CREATE_NICK_NAME,		&UIPhaseCreateChara::ReceiveGameEvent_Create_NickName);
}

UIPhaseCreateChara::~UIPhaseCreateChara(void)
{
}

/*virtual*/ void	UIPhaseCreateChara::OnCreate( i3GameNode * pParent)
{
	UIPhaseBase::OnCreate( pParent);

	AddScene("Scene/Main/PointBlankRe_CreateChara.i3UIs", PhaseCreateChara_Glue);
}

void UIPhaseCreateChara::_SetButtonInfo( void)
{
	INT32 i;

	// 필요하지 않은 슬롯은 Disable처리

	m_UsedSlotCount = 0;
	for( i = 0; i < CREATECHARA_MAX_COUNT; i++)
	{
		if( m_nItemId[ i] == 0)
		{
			m_pCharaSlot[i]->EnableCtrl( false);
		}
		else
			m_UsedSlotCount++;
	}

	m_pCharaSlot[ m_CurrentCreateIndex]->addState( I3UI_STATE_DEACTIVATED);
	m_pCharaSlot[ m_CurrentCreateIndex]->setModifyState();

	for( i = 0; i < CREATECHARA_MAX_COUNT; i++)
	{
		if( i != m_CurrentCreateIndex)
		{
			m_pCharaSlot[i]->removeState( I3UI_STATE_DEACTIVATED);
			m_pCharaSlot[i]->setModifyState();
		}
	}

	LuaState* L = _CallLuaFunction(0, "SetCharaName");
	if(L != nullptr)
	{
		i3::rc_wstring wstr;
		for( i = 0; i < CREATECHARA_MAX_COUNT; i ++)
		{
			CShopContext::i()->GetGoodsName(wstr, 0, m_nItemId[i] );
			i3Lua::PushStringUTF16To8( L, wstr.c_str(), wstr.size() );
		}
		_EndLuaFunction(L, 4);
	}
}

void UIPhaseCreateChara::_SetDetailInfo( T_ItemID ItemID)
{
	i3::rc_wstring wstrName;
	i3::rc_wstring wstrDesc;

	const CCharaInfo * pCharaInfo = g_pCharaInfoDataBase->GetCharaDBInfo( ItemID);
	pCharaInfo->getName( wstrName);
	pCharaInfo->getDescription( wstrDesc);

	LuaState * L = _CallLuaFunction( "SetSelectCharaName");

	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wstrName.c_str(), wstrName.size() );
		_EndLuaFunction( L, 1);
	}

	_SetFlowEditBox(wstrDesc);
}

/*
void UIPhaseCreateChara::_SetFlowEditBox( i3::string & strIn)
{
	i3::wstring wstrDesc;

	i3::mb_to_utf16(strIn, wstrDesc);
	m_FlowEditBox.SetFlowTextData(wstrDesc);
	m_FlowEditBox.PlayEditTextFlow(TFD_VERTICAL, true);
}
*/

void	UIPhaseCreateChara::_SetFlowEditBox( const i3::rc_wstring & wstrIn)
{
	m_FlowEditBox.SetFlowTextData(wstrIn);
	m_FlowEditBox.PlayEditTextFlow(TFD_VERTICAL, true);
}

INT32 UIPhaseCreateChara::_FindParentButton( i3UIControl * pCtrl)
{
	if( pCtrl != nullptr)
	{
		i3UIControl * pParent = (i3UIControl*) pCtrl->getParent();
		if( pParent != nullptr)
		{
			if( i3::generic_is_iequal( pParent->GetName(), "btnChara0") )
			{
				return 0;
			}
			else if( i3::generic_is_iequal( pParent->GetName(), "btnChara1") )
			{
				return 1;
			}
			else if( i3::generic_is_iequal( pParent->GetName(), "btnChara2") )
			{
				return 2;
			}
			else if( i3::generic_is_iequal( pParent->GetName(), "btnChara3") )
			{
				return 3;
			}
		}
	}

	return -1;
}

/*virtual*/ void UIPhaseCreateChara::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPhaseBase::_InitializeAtLoad( pScene);

	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd0");
	I3ASSERT( m_pFrameWnd);

	m_pFrameClass = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWndClass");
	I3ASSERT( m_pFrameClass);

	m_pFrameChara = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWndChara");
	I3ASSERT( m_pFrameChara);

	m_FlowEditBox.SetFlowEditBox(static_cast<i3UIEditBox*>(pScene->FindChildByName("i3UIEditBox")), 6.0f, 0.2f);

	INT32 i;
	char szConv[ 128];
	for( i = 0; i < CREATECHARA_MAX_COUNT; i++)
	{
		i3::snprintf( szConv, sizeof( szConv), "btnChara%d", i);

		m_pCharaSlot[i] = (i3UIButton*) m_pFrameChara->FindChildByName( szConv);
		I3ASSERT( m_pCharaSlot[i] != nullptr);
	}

	REAL32 offX = (REAL32)(GameUI::AutoPosition::get_max_right()) - getX(m_pFrameWnd->getSize()) - 5.f;
	m_pFrameWnd->setPos(offX, getY(m_pFrameWnd->getPos()));

	CHARA_INFO info;
	info.Reset();
	UI_CHARA_INFO* pMyInfo = UserInfoContext::i()->SetMyUICharaInfo();

	CHARA_PARTS_INFO* partInfo = &info.m_CharaEquipInfo.m_CharPartsInfo;
	info.m_CharaBasicInfo.m_ui8CharaState = CHARA_STATE_CREATED;
	info.m_ui32BattleTime = 0;
	info.m_ui32SkillRelTime = 0;
	for( i = 0; i < CREATECHARA_MAX_COUNT; i++)
	{
		if( m_nItemId[i] != 0)
		{
			if( CCharaInfoContext::i()->FindCharaInfoIndexByItemID( m_nItemId[ i]) == -1)
			{
				// vv3 - check
				partInfo->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, m_nItemId[i]);
				partInfo->SetPartsIdx( CHAR_EQUIPMENT_PARTS_CHARA, i);

				const CCharaInfo * pCharaDBInfo = g_pCharaInfoDataBase->GetCharaDBInfo( m_nItemId[i]);
				if( pCharaDBInfo != nullptr)
				{
					i3::rc_wstring wstrCharaName;	pCharaDBInfo->getName( wstrCharaName );
					i3::safe_string_copy(info.m_CharaBasicInfo.m_strCharaName, wstrCharaName, NET_CHARA_NICK_NAME_SIZE);
				}
				CCharaInfoContext::i()->AddCharacterInfo( info );
			}
		}
	}
	CCharaInfoContext::i()->SetAllCharaData();
	pMyInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_ASSAULT);
	pMyInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, DEFAULT_WEAPON_HANDGUN);
	pMyInfo->setWeaponItemID( WEAPON_SLOT_MELEE, DEFAULT_WEAPON_KNIFE);
	pMyInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, DEFAULT_WEAPON_THROWING1);
	pMyInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, DEFAULT_WEAPON_THROWING2);


	CCharaInfoContext::i()->CalculateMultiSlotIdx();

	_SetButtonInfo();
	
	_SetDetailInfo( m_nItemId[ 0]);

	// CharaView에 전달
	SetStateButten();
}

/*virtual*/ void UIPhaseCreateChara::_ClearAtUnload( i3UIScene * pScene)
{
	UIPhaseBase::_ClearAtUnload( pScene);

	m_pFrameWnd = nullptr;
	m_pFrameClass = nullptr;
	m_pFrameChara = nullptr;

	m_FlowEditBox.Reset();

	for( INT32 i = 0; i < CREATECHARA_MAX_COUNT; i++)
		m_pCharaSlot[i] = nullptr;

	CharaViewMgr::getView()->RemoveViewChara( true);
}

/*virtual*/ void	UIPhaseCreateChara::OnEntranceEnd()
{
	UIPhaseBase::OnEntranceEnd();
	if (!UserInfoContext::i()->HaveNickName())
	{
		GetMainFrame()->TogglePopup(UPW_CREATENICKNAME);
	}
	else
	{
		Click_Chara(0);
	}
}

/*virtual*/ bool UIPhaseCreateChara::OnEnterMouse( i3UIControl * pControl)
{
	bool bRv = UIPhaseBase::OnEnterMouse( pControl);

	INT32 idx = _FindParentButton( pControl);
	if( idx != -1)
	{
		m_pCharaSlot[ idx]->OnEnterMouse( 0, 0, 0);
	}

	return bRv;
}

/*virtual*/ void UIPhaseCreateChara::OnLeaveMouse( i3UIControl * pControl)
{
	UIPhaseBase::OnLeaveMouse( pControl);

	INT32 idx = _FindParentButton( pControl);
	if( idx != -1)
	{
		m_pCharaSlot[ idx]->OnLeaveMouse();
	}
}

/*virtual*/ bool UIPhaseCreateChara::OnClick( i3UIControl * pControl)
{
	bool bRv = UIPhaseBase::OnClick( pControl);

	INT32 idx = _FindParentButton( pControl);
	if( idx != -1)
	{
		m_pCharaSlot[ idx]->OnClicked( 0, 0);
		//Click_Chara( idx);
	}

	return bRv;
}


/*virtual*/ void	UIPhaseCreateChara::OnExitEnd( void)
{
	if(m_pmbCreateCharaWait != nullptr)
	{
		GameUI::CloseMsgBox(m_pmbCreateCharaWait);
		m_pmbCreateCharaWait = nullptr;
	}

	UIPhaseBase::OnExitEnd();
}

/*virtual*/ void	UIPhaseCreateChara::OnUpdate(REAL32 rDeltaSeconds)
{
	UIPhaseBase::OnUpdate(rDeltaSeconds);

	if( GetMainFrame()->IsOpenedPopup(UPW_CREATENICKNAME)== false )
	{
		if( !CharaViewMgr::i()->isLoadWaiting() && IsInputDisable() )
		{
			SetInputDisable( false);
		}
	}

	m_FlowEditBox.OnUpdate(rDeltaSeconds);
}

void UIPhaseCreateChara::SetStateButten()
{
	LuaState* L = _CallLuaFunction(0, "SetStateButten");
	if(L != nullptr)
	{
		i3Lua::PushInteger( L, 0);
		i3Lua::PushInteger( L, m_PageState);
		_EndLuaFunction(L, 2);
	}
}

void			UIPhaseCreateChara::CbSameNick(void* pThis,INT32 nParam)
{
	g_pFramework->GetUIMainframe()->OpenPopup( UPW_CREATENICKNAME);
}

void			UIPhaseCreateChara::Click_Next()
{
	switch(m_PageState)
	{
	case CREATECHARA_STATE_CLASS:	// 캐릭 생성
		{
			if( m_bSendCreate == false && m_pmbCreateCharaWait == nullptr)
			{
				m_pmbCreateCharaWait = GameUI::MsgBox( MSG_TYPE_SYSTEM_WAIT, GAME_RCSTRING("STR_CREATECHARA_ING"));

				m_CharaInfo.m_ui8BuyType = GOODS_BUY_CHAR_FREE;
				I3ASSERT( m_CharaInfo.m_wstrCharaName.size() > 0);
				GameEventSender::i()->SetEvent(EVENT_SET_CREATECHARA, &m_CharaInfo);

				SetInputDisable( true);
				m_bSendCreate = true;
			}
		}
		break;
	case CREATECHARA_STATE_CHARA:
		m_PageState = CREATECHARA_STATE_CLASS;	// Class 선택으로 전환
		SetStateButten();

		Click_Class( m_CurrentClass);

		i3::rc_wstring wstr_ClassDesc;

		LuaState* L = _CallLuaFunction(0, "InitClass");
		if(L != nullptr)
		{
			i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP1"));
			i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
			i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP2"));
			i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
			i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP3"));
			i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
			i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP4"));
			i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
			i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP5"));
			i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
			_EndLuaFunction(L, 5);
		}
		break;
	}
}

void			UIPhaseCreateChara::Click_Back()
{
	switch(m_PageState)
	{
	case CREATECHARA_STATE_CLASS:
		{
			m_CharaInfo.m_TItemID = m_TempInfo.m_TItemID;

			m_PageState = CREATECHARA_STATE_CHARA;
			SetStateButten();
			_SetButtonInfo();
			Click_Chara( m_CurrentCreateIndex);
			Click_Class( CHARA_CLASS_ASSAULT);
		}
		break;
	case CREATECHARA_STATE_CHARA:
		{
			m_PageState = CREATECHARA_STATE_CLASS;
		
			Reset(&m_CharaInfo);		// ._bCheck --> false 내장..

			SetStateButten();

			i3::rc_wstring wstr_ClassDesc;

			Click_Class( m_CurrentClass);
			LuaState* L = _CallLuaFunction(0, "InitClass");
			if(L != nullptr)
			{
				i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP1"));
				i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
				i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP2"));
				i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
				i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP3"));
				i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
				i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP4"));
				i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
				i3::sprintf(wstr_ClassDesc, GAME_RCSTRING("STR_TBL_CLASS_EXP5"));
				i3Lua::PushStringUTF16To8(L, wstr_ClassDesc);
				_EndLuaFunction(L, 5);
			}
		}
		break;
	}
	Avatar * pavtar = g_pCharaManager->GetLobbyAvatar();
	if(pavtar)
	{
		pavtar->SetParts( m_CharaInfo.m_TItemID, true);
	}

}

void			UIPhaseCreateChara::Click_Chara(INT32 iIdx)
{
	CHARA_INFO* pInfo = (CHARA_INFO*)CCharaInfoContext::i()->GetMainCharaInfo();

	i3::rc_wstring wstrCharaName;	CShopContext::i()->GetGoodsName( wstrCharaName, 0, m_nItemId[iIdx]);

	i3::generic_string_ncopy( pInfo->m_CharaBasicInfo.m_strCharaName, wstrCharaName, NET_CHARA_NICK_NAME_SIZE);

	// vv3 - check
	CHARA_PARTS_INFO* partInfo = &pInfo->getEquipInfo()->m_CharPartsInfo;
	partInfo->SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, m_nItemId[iIdx]);

	UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
	if( pCharaView != nullptr)
	{
		UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(partInfo, TEAM_RED);
		UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(partInfo, TEAM_BLUE);
		pCharaView->DoUpdateCharacter();
	}
	SetInputDisable( true);

	_SetDetailInfo( m_nItemId[ iIdx]);


	m_CharaInfo.m_TItemID = m_nItemId[ iIdx];

	m_CharaInfo.m_wstrCharaName = pInfo->m_CharaBasicInfo.m_strCharaName;  // NET_CHARA_NICK_NAME_SIZE;

	m_CurrentCreateIndex = iIdx;
}


void UIPhaseCreateChara::Click_Class( CHARA_CLASS iIdx)
{
	UI_CHARA_INFO * pNetCharaInfo = UserInfoContext::i()->SetMyUICharaInfo();

	// 이부분 PEF로 빼야합니다.
	switch(iIdx)
	{
	case CHARA_CLASS_ASSAULT:		pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_ASSAULT);	break;
	case CHARA_CLASS_SMG:			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_SMG);	break;
	case CHARA_CLASS_SNIPER:		pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_SNIPER);	break;
	case CHARA_CLASS_MACHINE:		pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_MG);	break;
	case CHARA_CLASS_SHOTGUN:		pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, DEFAULT_WEAPON_SHOTGUN);	break;
	}

	UIFloatCharacterView * pCharaView = static_cast<UIFloatCharacterView *>(GameUI::GetFloating( UFW_CHARACTER));
	if( pCharaView != nullptr)
		pCharaView->DoUpdateCharacter();

	m_CharaInfo.m_ui8Class = (UINT8)iIdx;
	m_CurrentClass = (CHARA_CLASS)iIdx;

//	const i3::rc_wstring& wstrDesc = GAME_RCSTRING(i3::format_string("STR_TBL_CLASS_EXP%d", iIdx));
//	_SetFlowEditBox(wstrDesc);	
}

void UIPhaseCreateChara::RegisterGameEventFunc( GAME_EVENT evt, Func_UIPhaseCreateChara f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, Func_UIPhaseCreateChara >::value_type( evt, f));
}

void UIPhaseCreateChara::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
{
	UIPhaseBase::ProcessGameEvent( evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, Func_UIPhaseCreateChara >::iterator it;
	it = m_EventFuncMap.find( evt);

	if ( it != m_EventFuncMap.end() )
	{
		Func_UIPhaseCreateChara & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

void UIPhaseCreateChara::ReceiveGameEvent_Create_Character( INT32 arg, const i3::user_data&)
{
	if(m_pmbCreateCharaWait != nullptr)
	{
		GameUI::CloseMsgBox(m_pmbCreateCharaWait);
		m_pmbCreateCharaWait = nullptr;
	}

	if( EV_SUCCESSED(arg))
	{
		GetMainFrame()->MovePhase(UIPHASE_LOBBY);
	}
	else
	{
		GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_FAIL_NICK_E"), nullptr, EXIT_CALLBACK(CbExitNormal,true), this);
		SetInputDisable( false);
	}
}

void UIPhaseCreateChara::ReceiveGameEvent_Create_NickName( INT32 arg, const i3::user_data&)
{
	if( EV_SUCCESSED(arg))
	{
		UserInfoContext::i()->SetMyUserInfoRecord()->Reset();
		UserInfoContext::i()->SetMyNickName(GameLoginContext::i()->getUserNick());
		GetMainFrame()->ClosePopup( UPW_CREATENICKNAME);
		GetMainFrame()->MovePhase(UIPHASE_CREATE_CHARA);
		Click_Chara(0);
		
	}
	else
	{
		UIPopupNickNameSetting* pNick = static_cast<UIPopupNickNameSetting*>(GameUI::GetPopup(UPW_CREATENICKNAME));
		if( pNick != nullptr)
			pNick->ChangeTextMsg(arg);

		PlaySound( UISND_ERROR);
	}
}

