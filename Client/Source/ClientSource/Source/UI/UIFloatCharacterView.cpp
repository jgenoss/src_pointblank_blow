#include "pch.h"
#include "UIFloatCharacterView.h"

#include "UIMainFrame.h"
#include "UICharaView.h"
#include "UISideMenu.h"
#include "UICharaInfoMgr.h"
#include "UIUtil.h"

#include "UITopMenu_V15.h"
#include "UIPhaseProfile.h"

#include "../CharaInfoContext.h"
#include "../GameCharaActionContext.h"
#include "../GameCharaAnimContext.h"
#include "../GameCharaWeaponContext.h"


#include "UIShopFunction.h"
#include "PurchaseMgr.h"

#include "UserInfoContext.h"
#include "BattleSlotContext.h"
#include "UISMenu_Ability.h"
#include "BuyUtil.h"
#include "ShopContext.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#include "UISignal.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

I3_CLASS_INSTANCE( UIFloatCharacterView);//, UIFloatingBase);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" { 

	int UIFloatCharacterView_ChangeTeam( LuaState * L)
	{
		UIFloatCharacterView * pThis = static_cast<UIFloatCharacterView*>(GameUI::GetFloating( UFW_CHARACTER));
		if( pThis != nullptr)
			pThis->ToggleTeam();
		return 0;
	}
	
	int UIFloatCharacterView_ChangeChara( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);
		UIFloatCharacterView * pThis = static_cast<UIFloatCharacterView*>(GameUI::GetFloating( UFW_CHARACTER));
		if( pThis != nullptr)
			pThis->SetChangeChara(btn);
		return 0;
	}

	int UIFloatCharacterView_Repair( LuaState * L)
	{
		UIFloatCharacterView * pThis = static_cast<UIFloatCharacterView*>(GameUI::GetFloating( UFW_CHARACTER));
		if( pThis != nullptr)
			pThis->RepairAll();
		return 0;
	}

	int UIFloatCharacterView_ClickSkillIcon( LuaState * L)
	{
		INT32 btn = i3Lua::GetIntegerArg( L, 1);
		UIFloatCharacterView * pThis = static_cast<UIFloatCharacterView*>(GameUI::GetFloating( UFW_CHARACTER));
		if( pThis != nullptr)
			pThis->ClickSkillIcon( btn);
		return 0;
	}
}

LuaFuncReg FloatCharacterView_Glue[] =
{
	{"ChangeTeam",		UIFloatCharacterView_ChangeTeam},
	{"ChangeChara",		UIFloatCharacterView_ChangeChara},
	{"Repair",			UIFloatCharacterView_Repair},
	{"ClickSkillIcon",	UIFloatCharacterView_ClickSkillIcon},
	{ nullptr,				nullptr}
};

//------------------------------------------------------------------------------//

UIFloatCharacterView::UIFloatCharacterView() : m_iCurrentCharaIdx(-1), m_TeamType(TEAM_RED)
	, m_ChangeCharaTimer(0.f), m_pSettedUIFrame(nullptr)
	, m_pDurabilityWnd(nullptr), m_pDurability(nullptr), m_pRepairBtn(nullptr)
	, m_InterpolateTimeForBtnVisible(0.f), m_DraggingTime(0.f), m_pPreCharaBtn(nullptr), m_pNextCharaBtn(nullptr)
	, m_bVisbleBtn(false), m_bDummyChara(false), m_bMouseClicked(false), m_pPlaneShape(nullptr)
	, m_bCenterFrame(false), m_bChangingTeam(false), m_ChangingTeamTimer(0.f), m_bChangeChara(false), m_bSkillSlot(false)
	, m_bChangeTeam(false), m_bInputDisable(false), m_UpdatableItemID(0)
{
	m_SigConnection += UISignal::FinishRepairAllItem.connect(i3::bind(&UIFloatCharacterView::cb_finish_repair_all_item, this, i3::_1));

//	UI::insert_cb("finish_repair_all_item", this, &UIFloatCharacterView::cb_finish_repair_all_item);

	m_bFixedWindow = true;
	m_AlignedPos = UIFLOATING_POS_LEFT;

#if defined( I3_DEBUG)
	SetName( "InvenCharaView");
#endif


	RegisterGameEventFunc( EVENT_INVEN_CHANGE_CHARA_EQUIP,		&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);
	RegisterGameEventFunc( EVENT_SKILL_SLOT_LEARN,				&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);
	RegisterGameEventFunc( EVENT_TEAM_CHANGE,					&UIFloatCharacterView::ReceiveGameEvent_Team_Change);
	RegisterGameEventFunc( EVENT_TEAM_BALANCE,					&UIFloatCharacterView::ReceiveGameEvent_Team_Balance);
	RegisterGameEventFunc( EVENT_CHANGE_MAINCHARA_FROM_ROOM,	&UIFloatCharacterView::ReceiveGameEvent_Change_MainCharacterFromRoom);
	RegisterGameEventFunc( EVENT_CHANGE_ROOM_OPTION_N,			&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);
	RegisterGameEventFunc( EVENT_SET_CHARA,						&UIFloatCharacterView::ReceiveGameEvent_SetCharacter);
	RegisterGameEventFunc( EVENT_REVERT_CHARA,					&UIFloatCharacterView::ReceiveGameEvent_RevertCharacter);
	RegisterGameEventFunc( EVENT_REVERT_PARTS,					&UIFloatCharacterView::ReceiveGameEvent_RevertParts);
	RegisterGameEventFunc( EVENT_REPAIR,						&UIFloatCharacterView::ReceiveGameEvent_Repair);
	
	RegisterGameEventFunc( EVENT_DELETE_CHARA,					&UIFloatCharacterView::ReceiveGameEvent_DeleteCharacter);
	RegisterGameEventFunc( EVENT_SKILL_EQUIP_CHANGE,			&UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter);

	RegisterUIEventFunc( UIEVENT_CHANGE_AVATAR,					&UIFloatCharacterView::UIEvent_Change_Avatar);
	RegisterUIEventFunc( UIEVENT_CHANGE_WEAPON,					&UIFloatCharacterView::UIEvent_Change_Weapon);
	RegisterUIEventFunc( UIEVENT_CHANGE_PARTS,					&UIFloatCharacterView::UIEvent_Change_Parts);
	RegisterUIEventFunc( UIEVENT_EQUIP_PARTS,					&UIFloatCharacterView::UIEvent_Equip_Parts);
	RegisterUIEventFunc( UIEVENT_UPDATE_CHARA_SLOT,				&UIFloatCharacterView::UIEvent_Update_Chara_Slot);
}

UIFloatCharacterView::~UIFloatCharacterView()
{
//	UI::remove_cb(this);
	I3_SAFE_RELEASE( m_pPlaneShape);
}

/*virtual*/ void UIFloatCharacterView::_InitializeAtLoad( i3UIScene * pScene)
{
	UIFloatingBase::_InitializeAtLoad( pScene);

	// main window
	m_pFrameWnd = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd3");
	m_OriginalFrameWidth = getX( m_pFrameWnd->getSize());
	
	m_pSettedUIFrame = (i3UIFrameWnd*) pScene->FindChildByName( "i3UIFrameWnd");
	I3ASSERT( m_pSettedUIFrame != nullptr);

	m_pDurabilityWnd = (i3UIFrameWnd*) pScene->FindChildByName( "fwdRepair");
	I3ASSERT( m_pDurabilityWnd != nullptr);
	
	m_pDurability = (i3UIProgressBar*) pScene->FindChildByName( "prgbDurability");
	I3ASSERT( m_pDurability != nullptr);
	m_pDurability->EnableCtrl( false);

	m_pRepairBtn = (i3UIButton*) pScene->FindChildByName( "btnRepair");
	I3ASSERT( m_pRepairBtn != nullptr);
	m_pRepairBtn->EnableCtrl( false);

	m_pPreCharaBtn = (i3UIButton*) pScene->FindChildByName( "i3UIButtonPre");
	I3ASSERT( m_pPreCharaBtn != nullptr);
	m_pNextCharaBtn = (i3UIButton*) pScene->FindChildByName( "i3UIButtonNext");
	I3ASSERT( m_pNextCharaBtn != nullptr);

	m_pTeamRedWnd = (i3UIFrameWnd*) pScene->FindChildByName( "FrmTeamViewRed");
	I3ASSERT( m_pTeamRedWnd != nullptr);
	m_pBtnTeamSmallRed = (i3UIButtonImageSet*) m_pTeamRedWnd->FindChildByName( "BtnTeamSmallRed");
	I3ASSERT( m_pBtnTeamSmallRed != nullptr);
	m_pBtnTeamLargeRed = (i3UIButtonImageSet*) m_pTeamRedWnd->FindChildByName( "BtnTeamLargeRed");
	I3ASSERT( m_pBtnTeamLargeRed != nullptr);

	m_pTeamBlueWnd = (i3UIFrameWnd*) pScene->FindChildByName( "FrmTeamViewBlue");
	I3ASSERT( m_pTeamBlueWnd != nullptr);
	m_pBtnTeamSmallBlue = (i3UIButtonImageSet*) m_pTeamBlueWnd->FindChildByName( "BtnTeamSmallBlue");
	I3ASSERT( m_pBtnTeamSmallBlue != nullptr);
	m_pBtnTeamLargeBlue = (i3UIButtonImageSet*) m_pTeamBlueWnd->FindChildByName( "BtnTeamLargeBlue");
	I3ASSERT( m_pBtnTeamLargeBlue != nullptr);

	setX( &m_PosBtnSmall, getX( m_pBtnTeamSmallBlue->getPos()));
	setY( &m_PosBtnSmall, getY( m_pBtnTeamSmallBlue->getPos()));
	setZ( &m_PosBtnSmall, m_pBtnTeamSmallBlue->getWidth());

	setX( &m_PosBtnLarge, getX( m_pBtnTeamLargeBlue->getPos()));
	setY( &m_PosBtnLarge, getY( m_pBtnTeamLargeBlue->getPos()));
	setZ( &m_PosBtnLarge, m_pBtnTeamLargeBlue->getWidth());

	m_bVisbleBtn = false;
	_UpdateButtonVisible( 1.f);
	_SetCharaClass();

	m_slide.ClearFrameWndList();

	// Set Chara View Info
	{

			
		
	}
}

/*virtual*/ void UIFloatCharacterView::_ClearAtUnload( i3UIScene * pScene)
{
	ClearAtUnloadChara();

	m_pSettedUIFrame = nullptr;
	m_pDurabilityWnd = nullptr;
	m_pDurability = nullptr;
	m_pRepairBtn = nullptr;
	m_pPreCharaBtn = nullptr;
	m_pNextCharaBtn = nullptr;

	UIFloatingBase::_ClearAtUnload( pScene);
}

void UIFloatCharacterView::_SetCharaClass()
{
	INT32 team = (m_TeamType==TEAM_RED)?0:1;

	const CHARA_SLOT_SKILL* pSkillInfo = CCharaInfoContext::i()->GetMainCharaSkill();

	INT32 skill0 = -1;
	INT32 skill1 = -1;
	INT32 skill2 = -1;

	if( pSkillInfo != nullptr)
	{
		if( pSkillInfo->m_ui8CommonSkillLv > 0)
			skill0 = pSkillInfo->m_CommonSkill;
		if( pSkillInfo->m_ui8MainSkillLv > 0)
			skill1 = pSkillInfo->m_MainSkill;
		if( pSkillInfo->m_ui8AssistSkillLv > 0)
			skill2 = pSkillInfo->m_AssistSkill;
	}

	LuaState* L = _CallLuaFunction(0, "SetClassState");
	if( L != nullptr)
	{
		i3Lua::PushInteger( L, team);
		i3Lua::PushInteger( L, skill0);
		i3Lua::PushInteger( L, skill1);
		i3Lua::PushInteger( L, skill2);
		_EndLuaFunction(L, 4);
	}
}

//------------------------------------------------------------------------------//
//								Game Event Functions							//
//------------------------------------------------------------------------------//
bool UIFloatCharacterView::OnEvent( UINT32 evt, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	i3::unordered_map< UIEVENT, ui_func >::iterator it;
	it = m_UIFuncMap.find( (UIEVENT)evt);

	if ( it != m_UIFuncMap.end() )
	{
		ui_func & mf = it->second;
		return (this->*mf)(pObj, param2, code);
	}

	return UIFloatingBase::OnEvent( evt, pObj, param2, code);
}
/*virtual*/ void UIFloatCharacterView::OnCreate( i3GameNode * pParent)
{
	UIFloatingBase::OnCreate( pParent);
	
	// Load Scene
	AddScene( "Scene/Main/PointBlankRe_CharaWindow.i3UIs", FloatCharacterView_Glue, true, false);	

	m_pPlaneShape = i3PhysixShapeSet::new_object();
	VEC3D vUp;

	i3Vector::Set( &vUp, 0.f, 1.f, 0.f);
	
	m_pPlaneShape->CreatePlaneShape( &vUp, 0.f, CGRP_WORLD_ATTACK, false);
}

void UIFloatCharacterView::OnUpdate( REAL32 rDeltaSeconds)
{
	i3GameNode::OnUpdate( rDeltaSeconds);

	_UpdateButtonVisible( rDeltaSeconds);

	if( m_bMouseClicked)
	{
		m_DraggingTime += rDeltaSeconds;
		if( m_DraggingTime > 0.2f)
		{
			m_bVisbleBtn = false;
		}
	}

	if( m_bUpdatedCharaPos )
	{
		UpdateCharaPosition();
	}

	__UpdateChangingTeamUI( rDeltaSeconds);
}

void UIFloatCharacterView::ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData)
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

void UIFloatCharacterView::__UpdateChangingTeamUI( REAL32 tm)
{
	if( m_bChangingTeam)
	{
		m_ChangingTeamTimer += tm;
		if( m_ChangingTeamTimer > 0.1f)	// 0.1초간 애니메이션
		{
			m_bChangingTeam = false;
			m_ChangingTeamTimer = 0.1f;
		}

		VEC3D start, end, Small, Large;

		i3Vector::Copy( &start, &m_PosBtnLarge);
		i3Vector::Copy( &end, &m_PosBtnSmall);

		i3Vector::Lerp( &Small, &start, &end, m_ChangingTeamTimer * 10.f);
		i3Vector::Lerp( &Large, &end, &start, m_ChangingTeamTimer * 10.f);

		if( m_TeamType == TEAM_RED)
		{
			m_pBtnTeamSmallRed->setPos( getX(&Small), getY(&Small));
			m_pBtnTeamSmallRed->setSize( getZ(&Small), getZ(&Small));
			m_pBtnTeamLargeRed->setPos( getX(&Large), getY(&Large));
			m_pBtnTeamLargeRed->setSize( getZ(&Large), getZ(&Large));
		}
		else
		{
			m_pBtnTeamSmallBlue->setPos( getX(&Small), getY(&Small));
			m_pBtnTeamSmallBlue->setSize( getZ(&Small), getZ(&Small));
			m_pBtnTeamLargeBlue->setPos( getX(&Large), getY(&Large));
			m_pBtnTeamLargeBlue->setSize( getZ(&Large), getZ(&Large));
		}
	}
}

void UIFloatCharacterView::UpdateCharaPosition( void)
{
	VEC3D vPos;
	i3Camera * pCam = g_pFramework->getSgContext()->getCurrentCamera();
	if( pCam != nullptr && m_bCenterFrame)
	{
		// 카메라의 로컬좌표
		VEC2D vPoint;

		m_pPlaneShape->modifyDynamicState( I3_PHYSIX_STATE_KINEMATIC, I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RESPONSE);
		
		MATRIX invMx;
		REAL32 dx, dy;
		VEC3D pos1, pos2;
		INT32 w, h;

		w = g_pFramework->GetViewer()->GetViewWidth();
		h = g_pFramework->GetViewer()->GetViewHeight();

		REAL32 scale = h / 768.f;

		i3Vector::Set( &vPoint, getX( m_pFrameWnd->getAccumulatedPos()), getY( m_pFrameWnd->getAccumulatedPos()));

		i3Vector::AddX( &vPoint, (i3UI::getManager()->getDisplayOffsetX() + (m_pFrameWnd->getWidth() * 0.5f)) );
		i3Vector::AddY( &vPoint, (i3UI::getManager()->getDisplayOffsetY() + m_pFrameWnd->getHeight() - 40.f));

		vPoint.y *= scale;
		
		{
			MATRIX tmx;
			i3Matrix::Mul( &tmx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
			i3Matrix::Inverse( &invMx, nullptr, &tmx);
		}
		//	i3Matrix::Inverse( &invMx, nullptr, pCam->GetViewMatrixAttr()->GetMatrix());

		dx = (vPoint.x / w * 2.0f) - 1.0f;
		dy = (vPoint.y / h * 2.0f) - 1.0f;

		i3Vector::Set( &pos1, dx, -dy, 0.0f);
		i3Vector::Set( &pos2, dx, -dy, 1.0f);

		i3Vector::TransformCoord( &pos1, &pos1, &invMx);
		i3Vector::TransformCoord( &pos2, &pos2, &invMx);

		// 우선 Raycasting을 시도한다.
		{
			I3_PHYSIX_HIT_RESULT * pResult;
			i3CollideeLine line;

			line.SetStart( &pos1);
			line.SetEnd( &pos2);
			pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);

			if( pResult != nullptr)
			{
				i3Vector::Copy( &vPos, & pResult->m_Intersect);
			}
			else
			{
				i3Vector::Set( &vPos, 0.f, 0.f, 0.6f);
			}
		}
	}
	else
	{
		i3Vector::Set( &vPos, 0.f, 0.f, 0.6f);
	}

	CharaViewMgr::getView()->SetCharaPosition( &vPos);

	m_bUpdatedCharaPos = false;
}

void UIFloatCharacterView::SetInputDisable( bool bDisable)
{
	m_bInputDisable = bDisable;
	_SetUIState( m_bChangeChara, m_bChangeTeam, m_bSkillSlot);
	_UpdateDurability();
}

void UIFloatCharacterView::_UpdateDurability( void)
{
	REAL32 progress_bar_val = 1.f;

	if ( CheckViewRepairUI() && !m_bInputDisable )
	{
		INT32 null_item_count = 0;
		INT32	Count = 0;
		REAL32 durability = 0.f;

		UINT8 chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
		for(INT32 i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
		{
			T_ItemID itemID = CharaInfoMgr::instance()->find(chara_idx, item_def::SCTG_WEAPON, i).m_TItemID;
			if( itemID == 0) { ++null_item_count; continue; }

			REAL32 dur = 1.0f;
			if( usf::GetDurability( itemID, 0, dur ))
			{
				durability += dur;
				Count++;
			}
		}

		if( null_item_count ==MAX_WEAPON_CHECK_SLOT_COUNT ) return;

		if( Count > 0)
		{
			progress_bar_val = min( durability / (REAL32)Count, 1.f);
			progress_bar_val = i3Math::Clamp(progress_bar_val, 0.f, 1.f);
		}

		I3COLOR durability_color;
		m_pDurability->setColor( &durability_color );
		if( progress_bar_val >= 0.9f )		
			durability_color.SetARGB(REPAIR_COLOR_BEST);  
		else if( progress_bar_val >= 0.5f )	
			durability_color.SetARGB(REPAIR_COLOR_NORMAL);
		else if( progress_bar_val >= 0.1f )	
			durability_color.SetARGB(REPAIR_COLOR_BAD);  
		else								
			durability_color.SetARGB(REPAIR_COLOR_BROKEN);

		m_pDurability->setColor( &durability_color );
		m_pDurability->setProgress( progress_bar_val );

		m_pDurabilityWnd->EnableCtrl( true);
	}
	else
	{
		m_pDurabilityWnd->EnableCtrl( false);
	}
	
	m_InterpolateTimeForBtnVisible = 0.f;

	if( progress_bar_val != 1.f )
		m_pRepairBtn->removeState( I3UI_STATE_DEACTIVATED);
	else
		m_pRepairBtn->addState( I3UI_STATE_DEACTIVATED);
				
	m_pRepairBtn->setModifyState();
}

void UIFloatCharacterView::RevertChara( REVERT_TYPE type)
{
	if( m_iCurrentCharaIdx != -1)
	{
		// 현재 설정된 아바타의 무기로 설정해준다.
		// 현재 장착된 파츠로 설정
		UISideMenu * pSide = UISideMenu::instance();
		if( pSide != nullptr)
		{
			SMenuAbility* ability = static_cast<SMenuAbility*>(pSide->getSubSidemenu(SIDEMENU_BTN_ABILITY));
			ability->SetMainInfoUpdateType(AMUT_REVERT);


			if( type & REVERT_TYPE_WEAPON)
			{
				const CHARA_WEAPON_INFO * pWeaponInfo = CCharaInfoContext::i()->GetCharaWeaponInfo( m_iCurrentCharaIdx);
				if( pWeaponInfo != nullptr)
				{
					// vv3 - check
					for( INT32 i = 0 ; i < CHAR_EQUIPMENT_WEAPON_COUNT ; ++ i)
					{
						UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID( (WEAPON_SLOT_TYPE)i, pWeaponInfo->m_Info[i].m_TItemID);
					}
					ability->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo(pWeaponInfo->m_Info[CHAR_EQUIPMENT_WEAPON_PRIMARY].m_TItemID, true));
				}
			}

			if( type & REVERT_TYPE_EQUIP)
			{
				const CHARA_PARTS_INFO * pPartsInfo = CCharaInfoContext::i()->GetCharaPartsInfo( m_iCurrentCharaIdx);
				if( pPartsInfo != nullptr)
				{
					UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( pPartsInfo );
					UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( pPartsInfo , TEAM_BLUE );
					ability->applyParts(const_cast<CHARA_PARTS_INFO*>(pPartsInfo));
				}

				// 다른 탭으로 이동시 Preview 설정된 모든 것을 초기화 한다.
				// 하지만 Part 하위 탭 이동시에는 무기만 리셋하므로 Equip을 되돌릴때 preview를 돌린다.
				buy::ClearPreview();
			
			}
		}
	}

	DoUpdateCharacter( false, WEAPON_SLOT_PRIMARY);
}

bool UIFloatCharacterView::CheckViewRepairUI(void)
{
	// 캐릭터 생성창은 Repair UI 사용 안함
	if (GetMainFrame()->GetCurrentPhaseType() == UIPHASE_CREATE_CHARA)
		return false;

	// 무기 수리를 사용하지 않는 국가 (Client 판단)
	if (LocaleValue::Enable("EnableRepairWeapon") == false)
		return false;

	// 무기 수리를 사용하지 않는 국가 (Server 판단)
	//if (UserInfoContext::i()->IsNewRepair() == false)
		//return false;

	// 기본캐릭터가 등록되지 않은 경우에 사용 안함.
	if (CCharaInfoContext::i()->GetCharaInfo(m_iCurrentCharaIdx) == nullptr)
		return false;

	//기프트샵에서 제거한다
	if (GetMainFrame()->GetCurrentSubPhaseType() == USP_GIFTSHOP)
		return false;

	bool equipped_durability_item = false;

	const UI_CHARA_INFO * pUICharaInfo = UserInfoContext::i()->GetMyUICharaInfo();

	if (pUICharaInfo != nullptr)
	{
		CShopContext * pShopCxt = CShopContext::i();

		// 내구제 파츠를 장착중인지 체크
		const CHARA_PARTS_INFO * equip_parts_info = pUICharaInfo->getCharaParts();

		for (INT32 idx = 0; idx < CHAR_EQUIPMENT_PARTS_COUNT; ++idx)
		{
			if (pShopCxt->IsDurabilityItem(equip_parts_info->GetPartsItemID(static_cast<CHAR_EQUIPMENT_PARTS>(idx))))
			{
				equipped_durability_item = true;
				break;
			}
		}

		if (equipped_durability_item == false)
		{
			// 내구제 무기를 장착중인지 체크
			for (INT32 idx = 0; idx < WEAPON_SLOT_COUNT; ++idx)
			{
				if (pShopCxt->IsDurabilityItem(pUICharaInfo->getWeaponItemID(static_cast<WEAPON_SLOT_TYPE>(idx))))
				{
					equipped_durability_item = true;
					break;
				}
			}
		}
	}

	return equipped_durability_item;
}

void UIFloatCharacterView::_UpdateButtonVisible( REAL32 tm)
{
	COLOR color;

	if( BattleSlotContext::i()->GetMySlotIdxEx() != -1)
	{
		const SLOT_INFO * pSlotInfo = BattleSlotContext::i()->getSlotInfo( BattleSlotContext::i()->GetMySlotIdxEx() );
		if( pSlotInfo->_State >= SLOT_STATE_READY)
			m_bVisbleBtn = false;
	}

	if( m_bVisbleBtn )
	{
		m_InterpolateTimeForBtnVisible += tm * 5.f;
		if( m_InterpolateTimeForBtnVisible >= 1.f)
		{
			m_InterpolateTimeForBtnVisible = 1.f;
		}
		
		if( m_bChangeChara)
		{
			i3Color::Copy( &color, m_pPreCharaBtn->getColor());
			color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
			m_pPreCharaBtn->setColor( &color);

			i3Color::Copy( &color, m_pNextCharaBtn->getColor());
			color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
			m_pNextCharaBtn->setColor( &color);

			if (m_pPreCharaBtn->isEnable() == false)
			{
				m_pPreCharaBtn->EnableCtrl(true);
				m_pNextCharaBtn->EnableCtrl(true);
			}
		}

		if( CheckViewRepairUI() )
		{
			i3Color::Copy( &color, m_pDurability->getColor());
			color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
			m_pDurability->setColor( &color);

			i3Color::Copy( &color, m_pRepairBtn->getColor());
			color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
			m_pRepairBtn->setColor( &color);

			if ( (m_pDurabilityWnd->isEnable() == false))
			{
				m_pDurabilityWnd->EnableCtrl(true);
				m_pDurability->EnableCtrl(true);
				m_pRepairBtn->EnableCtrl(true);
			}
		}
		else
		{
			if (m_pDurabilityWnd->isEnable() == true)
			{
				m_pDurabilityWnd->EnableCtrl(false);
			}
		}		
	}
	else
	{
		m_InterpolateTimeForBtnVisible -= tm * 5.f;
		if( m_InterpolateTimeForBtnVisible <= 0.f)
		{
			m_InterpolateTimeForBtnVisible = 0.f;

			if( m_pPreCharaBtn->isEnable() == true)
			{
				m_pPreCharaBtn->EnableCtrl( false);
				m_pNextCharaBtn->EnableCtrl( false);
			}

			if( m_pDurabilityWnd->isEnable() == true)
			{
				m_pDurabilityWnd->EnableCtrl( false);
				m_pDurability->EnableCtrl( false);
				m_pRepairBtn->EnableCtrl( false);
			}
		}
		else
		{
			if( m_bChangeChara)
			{
				i3Color::Copy( &color, m_pPreCharaBtn->getColor());
				color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
				m_pPreCharaBtn->setColor( &color);

			
				i3Color::Copy( &color, m_pNextCharaBtn->getColor());
				color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
				m_pNextCharaBtn->setColor( &color);

				if (m_pPreCharaBtn->isEnable() == false )
				{
					m_pPreCharaBtn->EnableCtrl(true);
					m_pNextCharaBtn->EnableCtrl(true);
				}
			}

			if( CheckViewRepairUI() )
			{
				i3Color::Copy( &color, m_pDurability->getColor());
				color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
				m_pDurability->setColor( &color);
			
				i3Color::Copy( &color, m_pRepairBtn->getColor());
				color.a = (UINT8) (m_InterpolateTimeForBtnVisible * 255.f);
				m_pRepairBtn->setColor( &color);

				if (m_pDurabilityWnd->isEnable() == false)
				{
					m_pDurabilityWnd->EnableCtrl(true);
					m_pDurability->EnableCtrl(true);
					m_pRepairBtn->EnableCtrl(true);
				}
			}
			else
			{
				if (m_pDurabilityWnd->isEnable() == true)
				{
					m_pDurabilityWnd->EnableCtrl(false);
				}
			}
		}
	}
}

/*virtual*/ bool UIFloatCharacterView::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIFloatingBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
		CharaViewMgr::i()->SetCharaViewInfo(this, m_pSettedUIFrame);
	else
	{
		UITopMenu * pTopMenu = g_pFramework->GetUIMainframe()->GetTopMenu();
		I3ASSERT(pTopMenu != nullptr);
		CharaViewMgr::i()->SetCharaViewInfo(this, pTopMenu->Get3DRoot());
	}

	if( CCharaInfoContext::i()->HasBasicChara() == false)
	{	// 초기 캐릭터가 없으면 기본 캐릭터를 설정한다.
		m_iCurrentCharaIdx = 0;
		RevertChara();
		Avatar * pAvatar = (Avatar*) g_pCharaManager->GetLobbyAvatar();
		if( pAvatar != nullptr)
			i3Matrix::Identity( pAvatar->getSceneObject()->GetMatrix());
	}
	
	return true;
}


void	UIFloatCharacterView::OnEntranceEnd()
{
	UIFloatingBase::OnEntranceEnd();

	DoUpdateCharacter();

	// Key : User Guide System
	{
		UserGuideMgr::i()->OnLoadGuideTargetInfo(this);
	}
}

bool UIFloatCharacterView::OnExitStart( void)
{
	CharaViewMgr::i()->ResetCharaViewInfo(this);

	if( UIFloatingBase::OnExitStart() == false)
		return false;

	ClearAtUnloadChara();

	m_slide.ForceEnd();	//slide 행동을 강제한다.

	return true;
}

void UIFloatCharacterView::PlayCeremonyCharacter( void)
{
	CharaViewMgr::getView()->PlayCharaAnim_Ceremony();
}

void UIFloatCharacterView::SelectChara( T_ItemDBIdx chara_db_idx)
{
	UINT8 slot_idx = CCharaInfoContext::i()->find_slot_idx_by_DBIdx( chara_db_idx );

	if( GetMainFrame()->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	{
		if( GetMainFrame()->GetCurrentSubPhaseType() == USP_NONE )
		{
			REAL32 rElapsedTime = g_pFramework->GetAccTime() - m_ChangeCharaTimer;
			if( rElapsedTime < 1.f) return;

			m_ChangeCharaTimer = g_pFramework->GetAccTime();

			GameEventSender::i()->SetEvent( EVENT_CHANGE_MAINCHARA_FROM_ROOM, (void*)slot_idx);
			return;
		}
	}

	{
		INT32 list_idx = CCharaInfoContext::i()->find_slot_idx( slot_idx );
		UpdateCharaSlot( static_cast<UINT8>(list_idx) );
	}
}

void UIFloatCharacterView::UpdateCharaSlot( INT32 charaSlot)
{
	if( charaSlot == -1 )
		charaSlot = CCharaInfoContext::i()->GetMainCharaIdx();

	if( m_iCurrentCharaIdx == charaSlot)
		return;

	m_iCurrentCharaIdx = charaSlot;

	CCharaInfoContext::i()->SetMainCharaIdx( m_iCurrentCharaIdx);
	INT32 chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
	const CHARA_INFO * info = CCharaInfoContext::i()->GetCharaInfoBySlotIdx(static_cast<UINT8>(chara_idx));

	Avatar * pavtar = g_pCharaManager->GetLobbyAvatar();

	if( pavtar != nullptr && info != nullptr)
	{
		if( pavtar->getCharaInfo()->getLoadSlotIndex() == -1)
		{
			SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));

			if( GetMainFrame()->GetCurrentSubPhaseType() == USP_INVENTORY)
			{	// 인벤토리서 아바타 교체시 설정
				CharaInfoMgr * mCharaMgr = CharaInfoMgr::instance();
				for(INT32 i = WEAPON_SLOT_PRIMARY; i <  WEAPON_CHARA_SLOT_MAX; i++)
				{
					ITEM_INFO item = mCharaMgr->find( chara_idx, item_def::SCTG_WEAPON, i);
					
					UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID( (WEAPON_SLOT_TYPE)i, item.m_TItemID);
				}
			}
			else
			{
				// 현재 설정된 아바타의 무기로 설정해준다.
				const CHARA_WEAPON_INFO * pWeaponInfo = CCharaInfoContext::i()->GetCharaWeaponInfo( chara_idx);

					// vv3 - check
				for (INT32 k = 0; k < CHAR_EQUIPMENT_WEAPON_COUNT; ++k)
				{
					UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID((WEAPON_SLOT_TYPE)k, pWeaponInfo->m_Info[k].m_TItemID);
				}


				ability->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo(pWeaponInfo->m_Info[CHAR_EQUIPMENT_WEAPON_PRIMARY].m_TItemID, true));
			}
			
			//2014-10-15 인벤토리에서는 해당 정보를 사용하지 않도록 변경합니다. ENG_김대영.
			if( GetMainFrame()->GetCurrentSubPhaseType() != USP_INVENTORY ) // CCharaInfoContext 캐릭터 정보 사용
			{
				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( CCharaInfoContext::i()->GetMainCharaPartsInfo());
				UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( CCharaInfoContext::i()->GetMainCharaPartsInfo(), TEAM_BLUE);
				ability->applyParts(CCharaInfoContext::i()->GetMainCharaPartsInfo());
			}
			else //CharaInfoMgr 캐릭터 정보 사용
			{
				const ItemInfoList & list = CharaInfoMgr::instance()->get_item_info_list(chara_idx, item_def::SCTG_PARTS);
				size_t size = list.size();

				for (int team = 0; team < TEAM_COUNT; team++)
				{
					CHARA_PARTS_INFO * pPartsInfo = const_cast<CHARA_PARTS_INFO*>(UserInfoContext::i()->SetMyUICharaInfo()->getCharaParts(team));
					for (size_t i = 0; i < size; ++i)
					{
						pPartsInfo->SetItemInfo((CHAR_EQUIPMENT_PARTS)(i + 1), list[i]);
					}

					pPartsInfo->m_Info[CHAR_EQUIPMENT_PARTS_CHARA] = CCharaInfoContext::i()->GetMainCharaPartsInfo()->m_Info[CHAR_EQUIPMENT_PARTS_CHARA];

					if( team == m_TeamType)
						ability->applyParts(pPartsInfo);
				}
			}

			ability->SetMainInfoUpdateType(AMUT_SLOTCHARA_CHANGED);

			DoUpdateCharacter();
		}
	}

	// CharaInfoMgr의 현재 캐릭터 슬롯을 변경
	CharaInfoMgr::instance()->m_current_state.slot_idx = CCharaInfoContext::i()->GetMainCharaIdx();
	CharaInfoMgr::instance()->m_current_state.chara_idx = CCharaInfoContext::i()->GetMainCharaSlotIdx();
	CharaInfoMgr::instance()->build_current_equip_info();

	UISignal::UpdateCartBoard();		// 카트 내용 변경

//	UI::call_cb("update_cart_board");	//카트 내용 변경
	UISideMenu::instance()->smenu_click_equip( false, 2 );	//왼쪽 메뉴 장비 정보 변경

	UISignal::UpdateSlot9(true, true);		//현재 캐릭에 맞게 내용 변경 상점 내용 변경

//	UI::call_cb( "update_slot9", (void*) true, (void*) true);	//현재 캐릭에 맞게 내용 변경 상점 내용 변경

	// 상점의 preview 슬롯의 내용을 초기화
	buy::ClearPreview();
	UISignal::UpdatePreviewSlot();
}

void UIFloatCharacterView::_SetUIState( bool bChangeChara, bool bChangeTeam, bool bSkillSlot)
{
	LuaState* L = _CallLuaFunction(0, "SetControlstate");
 	if(L != nullptr)
 	{
		i3Lua::PushBoolean( L, bChangeChara && !m_bInputDisable);
		i3Lua::PushBoolean( L, bSkillSlot);
		_EndLuaFunction(L, 2);

		_UpdateFrameWndPos();
 	}

	SetInputStateTeamChange( bChangeTeam && !m_bInputDisable);
}

void UIFloatCharacterView::_UpdateFrameWndPos( void)
{
	if (g_pEnvSet->m_nUIVersion == UFV_1_1)
		return;

	if( m_bCenterFrame) 
	{
		m_pFrameWnd->setSize( m_OriginalFrameWidth + (i3UI::getManager()->getDisplayOffsetX()*2.f), getY( m_pFrameWnd->getSize()));

		REAL32 offX = (g_pViewer->GetViewWidth()/2) - 400.f - (i3UI::getManager()->getDisplayOffsetX()*2.f);
		m_pFrameWnd->setDirectPos( offX, getY( m_pFrameWnd->getPos()));
	}
	else
	{
		m_pFrameWnd->setDirectPos( static_cast<REAL32>(-i3UI::getManager()->getDisplayOffsetX()), getY( m_pFrameWnd->getPos()));
		m_pFrameWnd->setSize( m_OriginalFrameWidth + (i3UI::getManager()->getDisplayOffsetX()*2.f), getY( m_pFrameWnd->getSize()));
	}

	m_pSettedUIFrame->setSize( getX( m_pFrameWnd->getSize()), getY( m_pSettedUIFrame->getSize()) );

	i3UIControl * pChild = static_cast<i3UIControl*>(m_pSettedUIFrame->getFirstChild());
	while( pChild != nullptr)
	{
		pChild->setDirectPos( getX( pChild->getTempPos()) + (i3UI::getManager()->getDisplayOffsetX()), getY( pChild->getPos()) );
		pChild = static_cast<i3UIControl*>(pChild->getNext());
	}

	m_pDurabilityWnd->setDirectPos( getX( m_pDurabilityWnd->getTempPos()) + i3UI::getManager()->getDisplayOffsetX(), getY( m_pDurabilityWnd->getPos()) );
}

void UIFloatCharacterView::DoUpdateCharacter( bool bDummyChara, WEAPON_SLOT_TYPE mustUpdateWeapon)
{
	bool bBtnChangeChara = false;
	bool bBtnChangeTeam = false;
	bool bUICenter = false;			// Frame Pos (true = Center , false = Left)
	bool bNickVisible = true;

	UIPHASE ePhase = GetMainFrame()->GetCurrentPhaseType();
	UISUBPHASE eSubPhase = GetMainFrame()->GetCurrentSubPhaseType();

	switch (ePhase)
	{
		case UIPHASE_CREATE_CHARA:
			bBtnChangeChara = false;
			bBtnChangeTeam = true;
			bUICenter = true;
			bNickVisible = false;
			m_bSkillSlot = false;
			break;
		case UIPHASE_CLANLOBBY:
		case UIPHASE_LOBBY:
			bBtnChangeChara = true;
			bBtnChangeTeam = true;
			bUICenter = false;
			m_bSkillSlot = true;

			switch(eSubPhase)
			{
			case USP_INVENTORY:
				bBtnChangeChara = true;
				bBtnChangeTeam = true;
				bUICenter = false;
				break;
			case USP_SHOP:
				bBtnChangeChara = true;
				bBtnChangeTeam = true;
				bUICenter = false;
				break;
			}
			break;
		case UIPHASE_READY_ROOM:
			bBtnChangeChara = true;
			bBtnChangeTeam = false;
			bUICenter = false;
			m_bSkillSlot = true;

			switch(eSubPhase)
			{
			case USP_INVENTORY:
				bBtnChangeChara = true;
				bBtnChangeTeam = true;
				bUICenter = false;
				break;
			case USP_SHOP:
				bBtnChangeChara = true;
				bBtnChangeTeam = true;
				bUICenter = false;
				break;
			}
			break;

		default: 
			break;
	}

	// Character Create에서 NetCharaInfo를 참조한다.
	// 캐릭터가 한마리면 채인지버튼 off
	if( CCharaInfoContext::i()->GetHasCharacterCount() == 1)
		bBtnChangeChara = false;
	
	const CHARA_INFO * info = CCharaInfoContext::i()->GetCharaInfo(m_iCurrentCharaIdx);

	// 이것이 nullptr이라면 기본캐릭터가 등록이 안되어 있다.
	if( info == nullptr)
	{
		m_bChangeChara = false;
		m_bChangeTeam = false;
		m_bCenterFrame = bUICenter;
		_SetUIState( false, false, m_bSkillSlot);
		_SetCharaClass();
		_UpdateDurability();
		return;
	}

	// 대기방이다. 팀을 구별하자!!
	if( BattleSlotContext::i()->GetMySlotIdxEx() > -1 &&
		(ePhase == UIPHASE_READY_ROOM && eSubPhase == USP_NONE))
	{
		SetChangeTeam( (BattleSlotContext::i()->GetMySlotIdxEx()%2==0) ? TEAM_RED:TEAM_BLUE);
	}

	{
		if( UserInfoContext::i()->SetMyUICharaInfo()->getCharaParts( m_TeamType )->IsValid() == false)
			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &info->m_CharaEquipInfo.m_CharPartsInfo, m_TeamType );

		_SetCharaClass();
	}

	m_bDummyChara = bDummyChara;

//SetClassstate
	m_bChangeChara = bBtnChangeChara;
	m_bChangeTeam = bBtnChangeTeam;
	m_bCenterFrame = bUICenter;
	_SetUIState( bBtnChangeChara, bBtnChangeTeam, m_bSkillSlot);

	CharaViewMgr::i()->UpdateCharacter(BattleSlotContext::i()->GetMySlotIdxEx(), true, mustUpdateWeapon, m_TeamType, m_pFrameWnd );

	_UpdateDurability();

	m_bUpdatedCharaPos = true;

	g_pFramework->OnEvent( UIEVENT_UPDATE_ABILITY, this, 0, I3_EVT_CODE_ACTIVATE);
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_INVEN_REFRESH);
}

void UIFloatCharacterView::ChangeOtherUserChara(INT32 i32SlotIdx)
{
	if( i32SlotIdx != BattleSlotContext::i()->getMySlotIdx())
	{	// 다른 유저의 슬롯이면 이벤트 요청
		GameEventSender::i()->SetEvent( EVENT_GET_USER_EQUIPMENT_INFO, &i32SlotIdx);
	}
	else
	{	// 자신의 캐릭터 업데이트
		DoUpdateCharacter();
	}
}

void UIFloatCharacterView::UpdateCharacter( INT32 slot)
{
	CharaViewMgr::i()->UpdateCharacter(slot, true, WEAPON_SLOT_UNKNOWN, (slot%2==0)?TEAM_RED:TEAM_BLUE, m_pFrameWnd);
}

VIEW_CHARA_INFO * UIFloatCharacterView::getViewCharaInfo( void)
{
	return CharaViewMgr::getView()->getViewCharaInfo();
}

void UIFloatCharacterView::ClearAtUnloadChara()
{
	CharaViewMgr::getView()->RemoveViewChara(true);
}

namespace inner
{
	void repair_all(void* arg, INT32 nParam)
	{
		if( nParam == MBR_OK)
		{
			UIFloatCharacterView* chara_view = static_cast<UIFloatCharacterView*>(arg);

			//캐릭터가 장착한 아이템(무기류)에 대해서 서버와 클라 정보가 다를 경우, 무기 변경 패킷을 보낸 후에 전체 수리를 해야된다.
			if( CharaInfoMgr::instance()->is_changed_equip_item( CCharaInfoContext::i()->GetMainCharaSlotIdx(), item_def::SCTG_WEAPON) == true )
			{
				if( UISideMenu::instance() != nullptr)
				{
					UISideMenu::instance()->set_change_equip_from_repair_all();

					//장비 변경 교체 한다.
					UISideMenu::instance()->send_change_equip();
				}
			}
			else
			{
				chara_view->SendRepairEvent(true); //장비 교환할 필요가 없고, 바로 전체 수리한다.
			}
		}
	}
}

void UIFloatCharacterView::RepairAll( void)
{
	AllRepairInfo info;

	bool enable_repair = PurchaseMgr::i()->IsAllRepairPossible(&info);

	if( enable_repair == false )
	{
		/*소지 금액이 부족하여 수리할 수 없습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_3"), &GAME_RCSTRING("STR_POPUP_REPAIR_CAPTION"));
	}
	else
	{
		/*수리 비용 %d(%s)\n수리하시겠습니까?*/
		i3::rc_wstring wstr;
	
		if(info.total_cash_cost != 0 && info.total_point_cost != 0 ) //포인트, 캐쉬 둘 다 있는 경우
		{
			i3::sprintf(wstr, L"%s %d(%s), %d(%s)\n%s", GAME_RCSTRING("STR_POPUP_REPAIR_PRICE"), 
				info.total_point_cost, GAME_RCSTRING("STR_SHOP_INFO_POINT"), info.total_cash_cost, GAME_RCSTRING("STR_SHOP_INFO_CASH"),
				GAME_RCSTRING("STR_POPUP_REPAIR_1") );
		}
		else if(info.total_cash_cost == 0) //포인트만 있는 경우
		{
			i3::sprintf(wstr, L"%s %d(%s)\n%s", GAME_RCSTRING("STR_POPUP_REPAIR_PRICE"), 
				info.total_point_cost, GAME_RCSTRING("STR_SHOP_INFO_POINT"),
				GAME_RCSTRING("STR_POPUP_REPAIR_1") );
		}
		else//캐쉬만 있는 경우
		{
			i3::sprintf(wstr, L"%s %d(%s)\n%s", 	GAME_RCSTRING("STR_POPUP_REPAIR_PRICE"),	
				info.total_cash_cost, GAME_RCSTRING("STR_SHOP_INFO_CASH"),
				GAME_RCSTRING("STR_POPUP_REPAIR_1") );
		}
		
		GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr, &GAME_RCSTRING("STR_POPUP_REPAIR_CAPTION"), MAKE_CALLBACK(inner::repair_all), this);
	}
}

void UIFloatCharacterView::SendRepairEvent( bool change_equip_result )
{
	if(change_equip_result == false) //장비 교환이 실패한 경우
	{
		/*{col:210,0,0,255}장비 불가{/col}*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_POPUP_REPAIR_REQ_FAIL"), &GAME_RCSTRING("STR_INVEN_CANNOT_EQUIP"));
		return;
	}

	PurchaseMgr::i()->RepairAll(this);   // "finish_repair_all_item", 
}

void	UIFloatCharacterView::SetChangeChara(INT32 istate, bool rotation)
{
	const CHARA_INFO * pCharaInfo;
	INT32 charaCount = CCharaInfoContext::i()->GetHasCharacterCount();

	INT32 idx = CCharaInfoContext::i()->GetMainCharaIdx() +istate;
	
	if( rotation == false )
	{
		idx = istate;
	}
	else
	{
		if(idx < 0)
		{
			idx = charaCount-1;
		}
		else if( idx >= charaCount)
		{
			idx = 0;
		}
	}

	pCharaInfo = CCharaInfoContext::i()->GetCharaInfo( idx);

	T_ItemDBIdx chara_db_idx =  pCharaInfo->getPartsInfo()->GetPartsIdx(CHAR_EQUIPMENT_PARTS_CHARA);

	SelectChara( chara_db_idx);
}

void	UIFloatCharacterView::ToggleTeam(void)
{
	SetChangeTeam( (m_TeamType==TEAM_RED) ? TEAM_BLUE:TEAM_RED);
	DoUpdateCharacter( m_bDummyChara);
}

void	UIFloatCharacterView::SetChangeTeam( TEAM_TYPE teamType)
{	
	if( m_TeamType != teamType)
	{
		m_TeamType = teamType;

		m_bChangingTeam = true;
		m_ChangingTeamTimer = 0.f;
		//
		//INT32 charaSlot = CCharaInfoContext::i()->GetMainCharaIdx(m_TeamType);
		//const CHARA_INFO * info = CCharaInfoContext::i()->GetCharaInfo(charaSlot);
		//CCharaInfoContext::i()->SetTeamSlotByCharaID(info->getPartsInfo()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA));
		//m_iCurrentCharaIdx = charaSlot;
		//UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts(&info->m_CharaEquipInfo.m_CharPartsInfo, m_TeamType);
	}
}

void UIFloatCharacterView::OnMovePhase( UIPHASE phase)
{
	RevertChara();
}

void UIFloatCharacterView::OnMoveSubPhase( UISUBPHASE phase)
{
	RevertChara();
}

bool UIFloatCharacterView::OnEnterMouse( i3UIControl * pControl)
{
	bool bRv = UIFloatingBase::OnEnterMouse( pControl);

	//I3TRACE( "EnterMouse : %s\n", pControl->GetName());

	if( m_bMouseClicked == false && m_bInputDisable == false)
	{
		m_bVisbleBtn = true;
	}
	
	return bRv;
}

void UIFloatCharacterView::OnLeaveMouse( i3UIControl * pControl)
{
	UIFloatingBase::OnLeaveMouse( pControl);

	//I3TRACE( "LeaveMouse : %s\n", pControl->GetName());
	
	if( pControl == m_pFrameWnd || pControl == m_pSettedUIFrame)
	{
		m_bMouseClicked = false;
		m_DraggingTime = 0.f;

		m_bVisbleBtn = false;
	}
}

void UIFloatCharacterView::OnDragStart( i3UIControl * pControl, INT32 x, INT32 y)
{
	m_bMouseClicked = true;
	CharaViewMgr::i()->SetMouseOldPos(x, y);	
}

void UIFloatCharacterView::OnDragging( i3UIControl * pControl, INT32 x, INT32 y)
{
	if( m_bMouseClicked == false)
		return;

	if( m_bInputDisable)
		return;

	CharaViewMgr::i()->RotateCharacter(x, y);
}

void UIFloatCharacterView::OnDragEnd( i3UIControl * pControl, INT32 x, INT32 y)
{
	m_bMouseClicked = false;
	m_DraggingTime = 0.f;

	if( m_bInputDisable == false)
	{
		if( pControl == m_pFrameWnd || pControl == m_pSettedUIFrame)
		{
			I3UI_PICK_INFO info;

			info.init( (REAL32) x, (REAL32) y);
			if( pControl->HitTest( &info) )
			{
				m_bVisbleBtn = true;
			}
		}
	}
}

bool UIFloatCharacterView::OnRevive( i3RenderContext * pCtx)
{
	if( UIFloatingBase::OnRevive( pCtx) == false)
		return false;

	_UpdateFrameWndPos();

	m_bUpdatedCharaPos = true;
	return true;
}

void UIFloatCharacterView::cb_finish_repair_all_item(bool bArg)
{
	if (bArg == false) return;

//	if( (BOOL)arg1 == FALSE ) return;

//	UI::call_cb( "finish_repair_item", (BOOL*)TRUE);
	UISignal::FinishRepairItem(true);

	_UpdateDurability();
}

void UIFloatCharacterView::ClickSkillIcon( INT32 icon)
{
	if( GetMainFrame() == nullptr)
		return;

	UITopMenu_V15 * pMenu = GetMainFrame()->GetTopMenu_V15();

	pMenu->Clicked( 4);	// 4번은 Profile

	UIPhaseProfile * pProfilePopup = static_cast<UIPhaseProfile*>(GameUI::GetPopup( UPW_PROFILE));
	if( pProfilePopup != nullptr)
		pProfilePopup->SetCurrentTab(UIPhaseProfile::TAB_SKILL);
}

void UIFloatCharacterView::SetInputStateTeamChange( bool bEnable)
{
	m_pTeamRedWnd->setInputDisable( !bEnable);
	m_pTeamBlueWnd->setInputDisable( !bEnable);
}


void UIFloatCharacterView::Update_CharaView_Parts(const i3::vector<ITEM_INFO>&list)
{
	size_t size = list.size();
	for(size_t i=0;i<size;++i)
	{
		// vv3 - check 일단 이건 하나로 수정.
		CHARA_PARTS_INFO * pPartsInfo = const_cast<CHARA_PARTS_INFO*>(UserInfoContext::i()->SetMyUICharaInfo()->getCharaParts());
		pPartsInfo->SetItemInfo( (CHAR_EQUIPMENT_PARTS)(i+1), list[i]);

		SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
		ability->applyParts(pPartsInfo);
	}

	DoUpdateCharacter();
}

#include "BuyUtil.h"

void UIFloatCharacterView::Update_CharaView_Parts_Preview()
{
	//원본 parts item( item_info_list ) 에 preview 에 등록된 item 을 결합해서 chara_view를 업데이트 한다.
	ItemInfoList item_info_list = CharaInfoMgr::instance()->get_item_info_list( CCharaInfoContext::i()->GetMainCharaSlotIdx(), item_def::SCTG_PARTS);

	i3::vector<ITEM_INFO> item_list = item_info_list.get_list();
	if (item_list.size() > 0)
		buy::UpdateItemInfoListWithPreview(item_list);
	
	Update_CharaView_Parts(item_list);
}

void UIFloatCharacterView::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIFloatCharacterView::ReceiveGameEvent_DoUpdateCharacter( INT32, const i3::user_data&)
{
	DoUpdateCharacter();
}

void UIFloatCharacterView::ReceiveGameEvent_Team_Change( INT32 arg, const i3::user_data&)
{
	if( arg == BattleSlotContext::i()->getMySlotIdx())
	{	
		SetChangeTeam( (BattleSlotContext::i()->getMySlotIdx()%2 == 0)? TEAM_RED:TEAM_BLUE );
		DoUpdateCharacter();
	}
}

void UIFloatCharacterView::ReceiveGameEvent_Team_Balance( INT32 arg, const i3::user_data&)
{
	if( g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() >= 0 && g_pFramework->GetUIMainframe()->GetCurrentSubPhaseType() <= USP_GIFTSHOP )
		return;

	SetChangeTeam( (BattleSlotContext::i()->getMySlotIdx()%2 == 0)? TEAM_RED:TEAM_BLUE );
	DoUpdateCharacter();
}

void UIFloatCharacterView::ReceiveGameEvent_Change_MainCharacterFromRoom( INT32 arg, const i3::user_data&)
{
	if( arg > -1)
	{
		//2014-07-17 ENG팀 김대영
		//함수의 의도와 맞게 클라용 캐릭터 인덱스를 사용 하도록 변경 함.
		INT32 idx = CCharaInfoContext::i()->find_slot_idx((UINT8)arg);
		I3_BOUNDCHK( idx, 255);
		UpdateCharaSlot( static_cast<UINT8>(idx));
	}
}

void UIFloatCharacterView::ReceiveGameEvent_DeleteCharacter( INT32 arg, const i3::user_data&)
{
	UpdateCharaSlot();
}

void UIFloatCharacterView::ReceiveGameEvent_SetCharacter(INT32 arg, const i3::user_data&)
{
	SelectChara(arg);
}

void UIFloatCharacterView::ReceiveGameEvent_RevertCharacter(INT32 arg, const i3::user_data&)
{
	RevertChara(arg);
}

void UIFloatCharacterView::ReceiveGameEvent_RevertParts(INT32 arg, const i3::user_data&)
{
	RevertChara(arg);
	Update_CharaView_Parts_Preview();
}

void UIFloatCharacterView::ReceiveGameEvent_Repair(INT32 arg, const i3::user_data&)
{
	bool bchange = (arg != 0);
	SendRepairEvent(bchange);
}

void UIFloatCharacterView::RegisterUIEventFunc( UIEVENT evt, ui_func f)
{
	m_UIFuncMap.insert(i3::unordered_map< UIEVENT, ui_func >::value_type( evt, f));
}

bool UIFloatCharacterView::UIEvent_Change_Avatar( i3ElementBase*, UINT32, I3_EVT_CODE)
{
	DoUpdateCharacter( m_bDummyChara);
	return true;
}

bool UIFloatCharacterView::UIEvent_Change_Weapon( i3ElementBase* pObj, UINT32 param, I3_EVT_CODE)
{
	WEAPON_SLOT_TYPE mustUpdateSlot = WEAPON_SLOT_UNKNOWN;

	SMenuAbility* menu_ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));

	if( pObj != nullptr && pObj == g_pFramework )
	{
		// 무기 정보 불러오기
		CWeaponInfo* pInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON::ItemID2Class( param), WEAPON::ItemID2ResID( param));
		if( pInfo == nullptr)
		{
			I3PRINTLOG(I3LOG_WARN,  "무기 정보가 없어요 ItemID %d", param);
			return false;
		}

		// Avatar에 장착
		ITEM_TYPE ItemType = (ITEM_TYPE) GET_ITEM_TYPE( param);
		switch( ItemType)
		{
		case ITEM_TYPE_PRIMARY:		mustUpdateSlot = WEAPON_SLOT_PRIMARY;	break;
		case ITEM_TYPE_SECONDARY:	mustUpdateSlot = WEAPON_SLOT_SECONDARY;	break;
		case ITEM_TYPE_MELEE:		mustUpdateSlot = WEAPON_SLOT_MELEE;		break;
		case ITEM_TYPE_THROWING1:	mustUpdateSlot = WEAPON_SLOT_THROWING1;	break;
		case ITEM_TYPE_THROWING2:	mustUpdateSlot = WEAPON_SLOT_THROWING2;	break;
		}

		UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID( mustUpdateSlot, param);
		SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
		ability->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo( param, true));
	}
	else if( param != 0)
	{
		CharaViewMgr::i()->SetParam(param);
		CharaInfoMgr::CurrentState * state = (CharaInfoMgr::CurrentState *) param;
		UserInfoContext::i()->SetMyUICharaInfo()->setWeaponItemID( (WEAPON_SLOT_TYPE) state->slot_idx, state->info.m_TItemID);
		menu_ability->applyWeapon(g_pWeaponInfoDataBase->getWeaponInfo( state->info.m_TItemID, true));
	}

	menu_ability->SetMainInfoUpdateType(AMUT_PREVIEW_WEAPON);
			
	DoUpdateCharacter( false, mustUpdateSlot);
	return true;
}

bool UIFloatCharacterView::UIEvent_Change_Parts( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE)
{
	bool bDummyChara = false;

	if( pObj != nullptr && pObj == g_pFramework )
	{
		// 정보 설정	
		// vv3 - check 팀별로 셋팅은 하는데 
		for( int team = 0 ; team < TEAM_COUNT ; team++ )
		{
			CHARA_PARTS_INFO parts;
			const CHARA_PARTS_INFO * pMyParts = UserInfoContext::i()->GetMyUICharaInfo()->getCharaParts(team);
			pMyParts->CopyTo( &parts);

			UINT32 ui32ItemType = GET_ITEM_TYPE( param);			
			T_ItemID ItemID = param;
			if( param < DEFAULT_PARTS_ITEM_ID_MIN && GET_ITEM_NUMBER( param) == 0  )
				ItemID = 0;	// 아이템 넘버가 0인 경우, 장착 해제입니다.

			switch( ui32ItemType )
			{
				// vv3 - check
			case ITEM_TYPE_DINO :
				{
					COMMON_PARTS_INFO commonParts;
					commonParts.SetPartsItemID( ItemID );
					UserInfoContext::i()->SetMyUICharaInfo()->setCommonParts( &commonParts);
				}
				break;
			case ITEM_TYPE_CHARA :		
				{
					parts.SetPartsItemID( CHAR_EQUIPMENT_PARTS_CHARA, ItemID);	
					bDummyChara = true;	
				}
				break;
			case ITEM_TYPE_BERET :		
				{
					parts.SetPartsItemID( EQUIP::GetCommonPartToBeret(), ItemID);	
					bDummyChara = true;	
				}
				break;

			default:
				if( ui32ItemType < ITEM_TYPE_HEADGEAR || ui32ItemType > ITEM_TYPE_SKIN ) 
				{
					I3ASSERT_0;
					return false;
				}
				UINT32 ui32Parts = ui32ItemType - ITEM_TYPE_CHARA;
				parts.SetPartsItemID( (CHAR_EQUIPMENT_PARTS)ui32Parts, ItemID);
				break;
			}

			//2014-06-17 버그 9506 ENG_김대영
			//캐릭터 뷰에서 상의, 하반신, 글러브를 교환하여 갱신하면 캐릭터 자체의 회전량이 초기화 됩니다.
			//그에 맞춰서 UI내의 회전량도 초기화 하였습니다.
			if(	ui32ItemType == ITEM_TYPE_UPPER ||
				ui32ItemType == ITEM_TYPE_LOWER ||
				ui32ItemType == ITEM_TYPE_GLOVE  )
			{
				CharaViewMgr::i()->SetCharaRotateTheta(0.0f);
			}
			//2014-06-17

			SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
			ability->applyParts( &parts);
			ability->SetMainInfoUpdateType(AMUT_PREVIEW_PARTS);

			UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( &parts, team);
		}

		m_UpdatableItemID = param;
	}

	DoUpdateCharacter( bDummyChara);
	return true;
}

bool UIFloatCharacterView::UIEvent_Equip_Parts( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE)
{
	if( param != 0)
	{
		CharaInfoMgr::CurrentState * state = (CharaInfoMgr::CurrentState *) param;
		ITEM_INFO item = state->info;

		// vv3 - check
		CHAR_EQUIPMENT_PARTS part = CHAR_EQUIPMENT_PARTS_CHARA;
		if( state->slot_idx > item_def::PT_BERET )				// slot_idx는 음수가 불가능하므로 < 0보다 작은 것은 제거..
		{
			I3PRINTLOG(I3LOG_FATAL,  "insert code!");
		}
		else
		{
			part = CHAR_EQUIPMENT_PARTS(state->slot_idx + 1);
		}
		
		CHARA_PARTS_INFO * pPartsInfo = nullptr;
		
		for(int i = 0 ; i < TEAM_COUNT ; i++)
		{
			pPartsInfo = UserInfoContext::i()->SetMyUICharaInfo()->setCharaParts( i );
			pPartsInfo->SetItemInfo( part, item);
		}

		SMenuAbility* ability = static_cast<SMenuAbility*>(UISideMenu::instance()->getSubSidemenu(SIDEMENU_BTN_ABILITY));
		ability->applyParts(pPartsInfo);
	}

	DoUpdateCharacter();
	return true;
}

bool UIFloatCharacterView::UIEvent_Update_Chara_Slot( i3ElementBase * pObj, UINT32 param, I3_EVT_CODE)
{
	if( param < (UINT32)CCharaInfoContext::i()->GetHasCharacterCount() )
	{
		UpdateCharaSlot( param);
		return true;
	}

	return false;
}
