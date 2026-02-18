#include	"pch.h"
#include	"UIBattlePopupSelectWeapon.h"

#include	"UIBattleFrame.h"
#include "UIImgSetCtrl.h"
#include "ItemImageMgr.h"
#include	"UIUtil.h"
#include	"BattleGui.h"
#include "../Shop.h"
#include "UIHudRespawnTimer.h"
#include "UIHudDefine.h"
#include "UIHudManager.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"
#include "ui/UIMath.h"
#include "UIHudUtil.h"
#include "GameGUI.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "i3Base/ColorString.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include <functional>
#include "ShopContext.h"

extern "C" 
{
	int UIBattlePopupSelectWeapon_BtnClick( LuaState * L)
	{
		ws::BPSW_BTN_ID btnID = (ws::BPSW_BTN_ID)i3Lua::GetIntegerArg( L, 1);
		UIBattlePopupSelectWeapon * p = static_cast<UIBattlePopupSelectWeapon*>(GameUI::GetBattlePopup(UBP_SELECT_WEAPON));
		if( p != nullptr)
			p->OnBtnClick( btnID );
		return 0;
	}
}

LuaFuncReg UIBattlePopupSelectWeapon_Glue[] = 
{
	{ "BtnClick", UIBattlePopupSelectWeapon_BtnClick},
	{nullptr, nullptr}
};

I3_CLASS_INSTANCE( UIBattlePopupSelectWeapon);//, UIBattlePopupBase );

UIBattlePopupSelectWeapon::UIBattlePopupSelectWeapon()
{
	m_bInitList = false;
	m_Is_Clicked_Ok_Btn = false;
	m_Focused_Category = 0;

	for(size_t i=WEAPON_SLOT_PRIMARY; i<WEAPON_SLOT_MISSION;++i)
	{
		m_ImageSet_List[i] = 0;
	}
	m_pImageSetPrevPrimary = m_pImageSetNextPrimary = 0;
}

UIBattlePopupSelectWeapon::~UIBattlePopupSelectWeapon()
{
}

//-----------------------------//
void	UIBattlePopupSelectWeapon::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate( pParent );

	AddScene("Scene/HUD/PointBlankRe_InGame_NewSelectWeapon.i3UIs", UIBattlePopupSelectWeapon_Glue);

	m_bInitList = true;
	m_Is_Clicked_Ok_Btn = false;
}

void UIBattlePopupSelectWeapon::_InitializeAtLoad( i3UIScene * pScene)
{
	UIBattlePopupBase::_InitializeAtLoad(pScene);

	ItemImageMgr* mgr = GetItemImageMgr();

	m_pImageSetPrevPrimary = mgr->CreateImgSetCtrl(pScene, "imgPrimWeaponPrev");
	m_pImageSetNextPrimary = mgr->CreateImgSetCtrl(pScene, "imgPrimWeaponNext");

	m_ImageSet_List[WEAPON_SLOT_PRIMARY] = mgr->CreateImgSetCtrl(pScene, "imgPrimWeaponCur");
	m_ImageSet_List[WEAPON_SLOT_SECONDARY] = mgr->CreateImgSetCtrl(pScene, "imgSubWeapon");
	m_ImageSet_List[WEAPON_SLOT_MELEE] = mgr->CreateImgSetCtrl(pScene, "imgMeleeWeapon");
	m_ImageSet_List[WEAPON_SLOT_THROWING1] = mgr->CreateImgSetCtrl(pScene, "imgThrowWeapon");
	m_ImageSet_List[WEAPON_SLOT_THROWING2] = mgr->CreateImgSetCtrl(pScene, "imgItem");

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "btn_Ok" );
	btn->set_disable_text_color(*GameGUI::GetColor(GCT_INVALID_C) );
}

void UIBattlePopupSelectWeapon::_ClearAtUnload( i3UIScene * pScene)
{
	UIBattlePopupBase::_ClearAtUnload(pScene);

	for(size_t i=WEAPON_SLOT_PRIMARY; i<WEAPON_SLOT_MISSION;++i)
	{
		if( m_ImageSet_List[i] ) m_ImageSet_List[i]->RestoreSizePos();
		I3_SAFE_RELEASE( m_ImageSet_List[i] );
	}

	if( m_pImageSetPrevPrimary ) m_pImageSetPrevPrimary->RestoreSizePos();
	I3_SAFE_RELEASE( m_pImageSetPrevPrimary );

	if( m_pImageSetNextPrimary ) m_pImageSetNextPrimary->RestoreSizePos();
	I3_SAFE_RELEASE( m_pImageSetNextPrimary );

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "btn_Ok" );
	btn->enable_btn();
}

bool	UIBattlePopupSelectWeapon::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr */ )
{
	if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
	{
		SetEnable(false);
		g_pFramework->SetGameKeyMode( GAMEKEY_MODE_GAME);
		return false;
	}

	if( UIPopupBase::OnEntranceStart( pArg1, pArg2 ) == false)
		return false;

	// 한소프트 7347. 교체창이 활성화 되어있으면 전적창은 출력하지 않음.
	if (UIBattleFrame::i()->GetHUD(UIHUD_RECORD_BOARD)->isEnable())
		UIBattleFrame::i()->EnableHUD(UIHUD_RECORD_BOARD, false);

	GetSlide()->ForceEnd();

	GameUI::SetMouseCenterPosition();

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_BLOCK);

	if( m_bInitList == true ) //리소폰 후에 한번 초기화 한다.
	{
		Init_Weapon_DB(); //현재 인게임 모드에서 사용 가능한 무기 data base를 만든다. (사용 가능 한 건만)
		
		//현재 내가 들고 있는 무기에 대한 db index 위치 처리
		const NET_CHARA_INFO * pMyCharaInfo = BattleSlotContext::i()->getNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());
		
		m_is_zero_durability_weapon_selected = false;

		for(INT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
		{
			T_ItemID itemID = pMyCharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i); //현재 내가 들고 있는 무기

			INT32 idx = Get_Index_inDB( itemID, (WEAPON_SLOT_TYPE)i ); //내가 들고 잇는 무기에 db 인덱스
			if( idx == -1 ) idx = 0; //특수전일 경우 -1이 리턴되며, 특수전이 아닐 경우 -1이 리턴되면 에러다.
				
			m_SelectInfo[i].backup = m_SelectInfo[i].curr = idx;

			if (true != m_is_zero_durability_weapon_selected)
			{
				m_is_zero_durability_weapon_selected = hu::is_zero_durability_weapon(
					m_WeaponDBList[i][idx]->_ui32ItemID,
					m_WeaponDBList[i][idx]->_TItemWareDBIdx);
			}
		}

		m_bInitList = false;
		m_Is_Clicked_Ok_Btn = false;
	}

	m_Focused_Category = WEAPON_SLOT_PRIMARY;
	for(INT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		if( Get_Count_inDB(i) != 0 )
		{
			m_Focused_Category = i;
			break;
		}
	}

	Init_UI();
	Update_UI();

	return true;
}

void UIBattlePopupSelectWeapon::OnLoadAllScenes()
{
	UIBattlePopupBase::OnLoadAllScenes();
}

void UIBattlePopupSelectWeapon::OnEntranceEnd()
{
	UIBattlePopupBase::OnEntranceEnd();
	
	const CHARA_INFO* main_chara_info = CCharaInfoContext::i()->GetMainCharaInfo();

	// 캐릭터 정보
	INT32 my_chara_slot = BattleSlotContext::i()->getMySlotIdx();
	UINT8 nick_color = BattleSlotContext::i()->getNickColorForSlot(my_chara_slot);
	
	i3::wstring wstr_nick = main_chara_info->m_CharaBasicInfo.m_strCharaName;

	const I3COLOR * pNickColor = GameUI::GetNickNameColor( nick_color );
	if( pNickColor != nullptr)
		::ColorString(wstr_nick, pNickColor->r, pNickColor->g, pNickColor->b, pNickColor->a );

//	INT32 skill_image_idx = main_chara_info->m_CharaSkillInfo.m_PrimarySkill;		//skill
//	INT32 skill_image_idx = -1; // 5576. GD팀 요청으로 병과 출력 기능 삭제

	//LuaState * L = _CallLuaFunction( "SetMyNick");
	//i3Lua::PushStringUTF16To8( L, wstr_nick.c_str());
	//i3Lua::PushInteger( L, skill_image_idx);
	//_EndLuaFunction( L, 2);
}


bool UIBattlePopupSelectWeapon::OnExitStart()
{
	UIBattlePopupBase::OnExitStart();

	HUD::instance()->SetParameter(HUD::RESPAWN_TIMER, HUD::RT_SEND_UNBLOCK);

	return true;
}

void UIBattlePopupSelectWeapon::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	i3UIButton* btn = static_cast<i3UIButton*>( GetScene(0)->FindChildByName("btn_Cancel") );
	btn->RemoveOnMouse();
}

bool UIBattlePopupSelectWeapon::OnKeyInput( i3InputDeviceManager * pInputMng)
{
	if( UIBattlePopupBase::OnKeyInput( pInputMng) == true)
		return true;

	i3InputKeyboard* pKeyboard = pInputMng->GetKeyboard();

	bool is_modify = false;

	if( pKeyboard->GetStrokeState(I3I_KEY_UP) == true ) { Move_Category(false); is_modify = true; }
	else if( pKeyboard->GetStrokeState(I3I_KEY_DOWN) == true ) { Move_Category(true); is_modify = true; }
	else if( pKeyboard->GetStrokeState(I3I_KEY_LEFT) == true ) { Move_Slot(false); is_modify = true; }
	else if( pKeyboard->GetStrokeState(I3I_KEY_RIGHT) == true ) { Move_Slot(true); is_modify = true; }
	else if( pKeyboard->GetStrokeState(I3I_KEY_ENTER) == true )
	{
		if( m_is_zero_durability_weapon_selected == false )
			OnOKButton();
	}

	if(is_modify)
	{
		Update_UI();
	}

	return false;
}

bool UIBattlePopupSelectWeapon::OnOKButton( void)
{
	m_Is_Clicked_Ok_Btn = true;
	for(INT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		m_SelectInfo[ i].backup = m_SelectInfo[i].curr;
	}

#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
	if( !g_pConfig->m_bNetworking )	// Local Mode
	{
		WeaponChange();
		g_pCharaManager->RespawnChara( BattleSlotContext::i()->getMySlotIdx(), true, true );
	}
#endif
	CBattleGui::i()->OnSelectWeaponClose();
	return true;
}

bool UIBattlePopupSelectWeapon::OnCloseButton( void)
{
	for(INT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		m_SelectInfo[ i].curr = m_SelectInfo[i].backup;
	}

	CBattleGui::i()->OnSelectWeaponClose();
	return true;
}

//-----------------------------//
void UIBattlePopupSelectWeapon::WeaponChange()
{
	if( m_bInitList == true ) return; //weapon list를 초기화가 필요하단건 무기 교체창을 아직 안 활성화 했단 이야기
	if( m_Is_Clicked_Ok_Btn == false ) return; //Ok 버튼을 한번도 클릭 안 했다.

	bool bChange = false;
	NET_CHARA_INFO * pMyCharaInfo = BattleSlotContext::i()->setNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());
	for(INT i = 0; i < MAX_WEAPON_CHECK_SLOT_COUNT; i++)
	{
		if( Get_Count_inDB(i) == 0 ) continue;

		T_ItemID weapon_item_id = pMyCharaInfo->getWeaponItemID( (WEAPON_SLOT_TYPE)i);
		T_ItemID select_item_id = (T_ItemID)Get_ID_inDB(m_SelectInfo[i].curr, (WEAPON_SLOT_TYPE) i);

		if( weapon_item_id != select_item_id )
		{
			T_ItemDBIdx idx = Get_WareIndex_inDB(select_item_id);
			pMyCharaInfo->setWeaponItemID( (WEAPON_SLOT_TYPE)i, select_item_id, idx);
			bChange = true;
		}
	}

	if( bChange)
		CBattleGui::i()->OnGameStart();
	else
		CBattleGui::i()->OnSelectWeaponClose();
}

void UIBattlePopupSelectWeapon::OnBtnClick( ws::BPSW_BTN_ID btnID)
{
	bool dir = false;

	switch( btnID)
	{
	case ws::BPSW_BTN_PRIM_PREV :	m_Focused_Category = WEAPON_SLOT_PRIMARY;	break;
	case ws::BPSW_BTN_PRIM_NEXT :	m_Focused_Category = WEAPON_SLOT_PRIMARY;	dir = true; break;

	case ws::BPSW_BTN_SUB_PREV :	m_Focused_Category = WEAPON_SLOT_SECONDARY;	break;
	case ws::BPSW_BTN_SUB_NEXT :	m_Focused_Category = WEAPON_SLOT_SECONDARY;	dir = true;	break;

	case ws::BPSW_BTN_MELEE_PREV :	m_Focused_Category = WEAPON_SLOT_MELEE;	break;
	case ws::BPSW_BTN_MELEE_NEXT :	m_Focused_Category = WEAPON_SLOT_MELEE;	dir = true;	break;

	case ws::BPSW_BTN_THROW_PREV :	m_Focused_Category = WEAPON_SLOT_THROWING1;	break;
	case ws::BPSW_BTN_THROW_NEXT :	m_Focused_Category = WEAPON_SLOT_THROWING1;	dir = true;	break;

	case ws::BPSW_BTN_ITEM_PREV :	m_Focused_Category = WEAPON_SLOT_THROWING2;	break;
	case ws::BPSW_BTN_ITEM_NEXT :	m_Focused_Category = WEAPON_SLOT_THROWING2;	dir = true;	break;
	}

	Move_Slot(dir);
	Update_UI();
}


//-----------------------------//
void UIBattlePopupSelectWeapon::Update_Slot(size_t idx, UIImgSetCtrl* p, INVEN_BUFFER* ib)
{
	i3::rc_wstring wstrName;
	i3::rc_wstring wstrClassName;
	bool			brepair_item = false;
	bool			bProgressBar = false;

	GameUI::GetWeaponString( wstrName, nullptr, &wstrClassName, ib->_ui32ItemID);

	REAL32 durable_rate = 0.f;
	brepair_item = usf::GetDurability(ib->_ui32ItemID, ib->_TItemWareDBIdx, durable_rate);

	if (UserInfoContext::i()->IsNewRepair())
	{
		bProgressBar = true;
	}

	if(idx < 3) //prim
	{
		LuaState * L = _CallLuaFunction( "SetPrimSlot");
		i3Lua::PushInteger( L, idx);
		i3Lua::PushStringUTF16To8( L, wstrName);
		i3Lua::PushStringUTF16To8( L, wstrClassName);
		i3Lua::PushBoolean(L, brepair_item);
		i3Lua::PushNumber(L, durable_rate);
		_EndLuaFunction( L, 5);
	}
	else //sub
	{
		LuaState * L = _CallLuaFunction( "SetSubSlot");
		i3Lua::PushInteger( L, idx-3);
		i3Lua::PushStringUTF16To8( L, wstrName.c_str(), wstrName.size());
		i3Lua::PushNumber(L, brepair_item ? durable_rate : 100.0f );
		_EndLuaFunction( L, 3);
	}

	GetItemImageMgr()->SelectImage( p, ib->_ui32ItemID);
}

void UIBattlePopupSelectWeapon::Update_UI()
{
	//포커스
	LuaState * L = _CallLuaFunction( "SetFocus");
	i3Lua::PushInteger( L, m_Focused_Category);
	_EndLuaFunction( L, 1);

	size_t size = m_WeaponDBList[WEAPON_SLOT_PRIMARY].size();
	if(size != 0 ) //중앙 : 0개 이상일 경우 표시한다.
	{
		INT32 idx = m_SelectInfo[WEAPON_SLOT_PRIMARY].curr;
		Update_Slot(0, m_ImageSet_List[WEAPON_SLOT_PRIMARY], m_WeaponDBList[WEAPON_SLOT_PRIMARY][idx]);
	}
	//if( size > 1 ) //오른쪽 : 1개 이상일 경우 표시된다.
	if (size == 2)
	{
		INT32 idx = ws::get_move_number(m_SelectInfo[WEAPON_SLOT_PRIMARY].curr, size, true);
		Update_Slot(1, m_pImageSetNextPrimary, m_WeaponDBList[WEAPON_SLOT_PRIMARY][idx]);
		Update_Slot(2, m_pImageSetPrevPrimary, m_WeaponDBList[WEAPON_SLOT_PRIMARY][idx]);
	}
	else if( size > 2 ) //왼쪽 : 2개 이상일 경우 표시된다.
	{
		INT32 prev_weapon_idx = ws::get_move_number(m_SelectInfo[WEAPON_SLOT_PRIMARY].curr, size, false);
		INT32 next_weapon_idx = ws::get_move_number(m_SelectInfo[WEAPON_SLOT_PRIMARY].curr, size, true);
		Update_Slot(1, m_pImageSetNextPrimary, m_WeaponDBList[WEAPON_SLOT_PRIMARY][next_weapon_idx]);
		Update_Slot(2, m_pImageSetPrevPrimary, m_WeaponDBList[WEAPON_SLOT_PRIMARY][prev_weapon_idx]);
	}

	//sub
	for(size_t i = WEAPON_SLOT_SECONDARY; i < MAX_CATEGORY_WEAPON; ++i)
	{
		if( m_WeaponDBList[i].empty() == true ) continue;

		INT32 idx = m_SelectInfo[i].curr;
		Update_Slot(i+2, m_ImageSet_List[i], m_WeaponDBList[i][idx]);
	}
}

void UIBattlePopupSelectWeapon::Init_UI()
{
	//prim
	size_t sizeWeaponPrimary = m_WeaponDBList[WEAPON_SLOT_PRIMARY].size();

	// hansoft 6871 : 아래 코드 주석 처리. 무기가 두 개라도 선택 화살표 모두 이용 가능토록.
	//LuaState * L = _CallLuaFunction( "EnablePrimArrow");
	//i3Lua::PushBoolean( L, (size < 2) ? false : true);
	//i3Lua::PushBoolean( L, (size > 2) ? true : false);
	//_EndLuaFunction( L, 2);

	if (sizeWeaponPrimary <= 1)
	{
		LuaState * L = _CallLuaFunction( "EnablePrimArrow");
		i3Lua::PushBoolean( L, false);
		i3Lua::PushBoolean( L, false);
		_EndLuaFunction( L, 2);
	}

	if(sizeWeaponPrimary == 0 )
	{
		m_ImageSet_List[0]->SetEnable(false);
		m_pImageSetNextPrimary->SetEnable(false);
		m_pImageSetPrevPrimary->SetEnable(false);
	}
	else if(sizeWeaponPrimary == 1)
	{
		m_pImageSetNextPrimary->SetEnable(false);
		m_pImageSetPrevPrimary->SetEnable(false);
	}
	//else if( size == 2)
	//{
	//	m_pImageSetPrevPrimary->SetEnable(false);
	//}

	//0은 중앙, 1은 오른쪽, 2는 왼쪽을 나타냄
	for(size_t i = sizeWeaponPrimary; i<=2; ++i) //오른쪽 까지 해야되서 i<=2로 설정함.
	{
		LuaState* L = _CallLuaFunction( "SetPrimSlot");
		i3Lua::PushInteger( L, i);	
		i3Lua::PushStringUTF16To8( L, L"");	
		i3Lua::PushStringUTF16To8( L, L"");	
		i3Lua::PushBoolean(L, false);
		_EndLuaFunction( L, 4);
	}

	//sub
	for(size_t i = WEAPON_SLOT_SECONDARY; i < MAX_CATEGORY_WEAPON; ++i)
	{
		size_t size = m_WeaponDBList[i].size();

		if( size < 2 )
		{
			m_ImageSet_List[i]->SetEnable( size == 0 ? false : true );

			LuaState * L = _CallLuaFunction( "DisableSubArrow");
			i3Lua::PushInteger( L, i-1);
			_EndLuaFunction( L, 1);

			L = _CallLuaFunction( "SetSubSlot");
			i3Lua::PushInteger( L, i-1);
			i3Lua::PushStringUTF16To8( L, L"");
			_EndLuaFunction( L, 2);
		}
	}

	// 내구도가 0인 무기 착용시, 처음 시작에서 착용은 되나 교체 및 확인 버튼은 비활성화.
	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName("btn_Ok");
	if (m_is_zero_durability_weapon_selected == true)
	{
		btn->disable_btn();
	}
	else
	{
		btn->enable_btn();
	}

	LuaState * L = _CallLuaFunction("SetNotice");
	i3Lua::PushInteger(L, m_is_zero_durability_weapon_selected);
	_EndLuaFunction(L, 1);
}

void UIBattlePopupSelectWeapon::Move_Category(bool dir) //false: up, true: down
{
	size_t i = 0;
	while( i < MAX_CATEGORY_WEAPON)
	{
		m_Focused_Category = ws::get_move_number( m_Focused_Category, MAX_CATEGORY_WEAPON, dir );

		if( m_WeaponDBList[m_Focused_Category].empty() == false )
			break;

		++i;
	}
}

void UIBattlePopupSelectWeapon::Move_Slot(bool dir) //false: left, true: right
{
	m_SelectInfo[m_Focused_Category].curr = ws::get_move_number( 
		m_SelectInfo[m_Focused_Category].curr, m_WeaponDBList[m_Focused_Category].size(), dir);

	for (int category = 0; category < MAX_CATEGORY_WEAPON; category++)
	{
		INT32 idx = m_SelectInfo[category].curr;

		m_is_zero_durability_weapon_selected = hu::is_zero_durability_weapon(
			m_WeaponDBList[category][idx]->_ui32ItemID,
			m_WeaponDBList[category][idx]->_TItemWareDBIdx);

		if (m_is_zero_durability_weapon_selected) break;
	}

	i3UIButtonComposed3* btn = (i3UIButtonComposed3*)GetScene(0)->FindChildByName( "btn_Ok" );
	if(m_is_zero_durability_weapon_selected == true)
	{
		btn->disable_btn();
	}
	else
	{
		btn->enable_btn();
	}

	LuaState * L = _CallLuaFunction( "SetNotice");
	i3Lua::PushInteger( L, m_is_zero_durability_weapon_selected);
	_EndLuaFunction( L, 1);
}

//- WeaponDB ------------------------//
void UIBattlePopupSelectWeapon::Init_Weapon_DB()
{
	for(INT32 i = 0; i < MAX_CATEGORY_WEAPON; i++)
	{
		const i3::vector<INVEN_BUFFER*>& db = CInvenList::i()->get_ref_list( INVEN::DB_WEAPON, i);
		m_WeaponDBList[i] = db;

		struct Fn : std::binary_function<INVEN_BUFFER*, UIBattlePopupSelectWeapon*, bool>
		{
			bool operator()(INVEN_BUFFER* p, UIBattlePopupSelectWeapon* pThis) const
			{
				// hansoft. 6833. 아래 주석 처리

				//true 를 반환 하면 삭제 된다.
				UINT8 authType = CShop::i()->GetAuthType(p->_ui32ItemID);
				if( ITEM_ATTR_TYPE_BUY == p->_ui8ItemType && authType == ITEM_TYPE_TIME)
					return true;

				UINT8 mode_flag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
				return item_def::is_locked_weapon(mode_flag, p->_ui32ItemID );
			}
		};

		i3::vector<INVEN_BUFFER*>::iterator it = std::remove_if(m_WeaponDBList[i].begin(), 
			m_WeaponDBList[i].end(), std::bind2nd(Fn(), this) );

		m_WeaponDBList[i].resize( it - m_WeaponDBList[i].begin() );
	}
}

namespace inner
{
	struct Fn_ItemId
	{
		UINT32 _ui32ItemID;
		Fn_ItemId(UINT32 i) : _ui32ItemID(i) {}

		bool operator()(INVEN_BUFFER* pBuffer) const
		{
			return (pBuffer->_ui32ItemID == _ui32ItemID);
		}
	};
}

size_t UIBattlePopupSelectWeapon::Get_Count_inDB(size_t idx) const //GetWeaponCount
{
	return m_WeaponDBList[idx].size();
}

size_t UIBattlePopupSelectWeapon::Get_Index_inDB(T_ItemID itemID, size_t idx) const
{
	const i3::vector<INVEN_BUFFER*>& db_list = m_WeaponDBList[idx];
	i3::vector<INVEN_BUFFER*>::const_iterator it = std::find_if(db_list.begin(), db_list.end(), inner::Fn_ItemId(itemID) );

	if( it == db_list.end() )
		return UINT_MAX;

	return it - db_list.begin();
}

T_ItemDBIdx UIBattlePopupSelectWeapon::Get_WareIndex_inDB(T_ItemID itemID)
{
/*
	내부 구현이 T_ItemDBIdx CInvenList::FindWareDBIdx_ByItemID( T_ItemID ItemId)
	위 함수와 동일함. 무기교체의 경우에만 사용되는 함수인데. 위 함수는 다른 아이템에서도 많이 사용되어
	임의로 무기슬롯으로 가져오게 할 수가 없어 이 함수 내부의 usage get 하는 함수를 교체하도록 함.
	추후 개선 필요하다고 생각됨.
*/
	INVEN::DB_TYPE dbType = INVEN::ItemID2DBType( itemID);
	I3ASSERT( dbType != INVEN::DB_COUPON);

	INT32 idx = CInvenList::i()->FindItemIndexByItemID( dbType, itemID, GetWeaponSlot_WeaponItemIDFunc( itemID));
	return CInvenList::i()->GetDBIdxByIndex( dbType, idx, GetWeaponSlot_WeaponItemIDFunc( itemID));
}

bool UIBattlePopupSelectWeapon::Get_Type_inDB(INT32 idx, WEAPON_SLOT_TYPE nUsageType, UINT8* Type, UINT32* Arg)
{
	const i3::vector<INVEN_BUFFER*>& db_list = m_WeaponDBList[nUsageType];

	*Type = db_list[idx]->_ui8ItemType;
	*Arg = db_list[idx]->_ui32ItemArg;

	return true;
}

T_ItemID UIBattlePopupSelectWeapon::Get_ID_inDB(INT32 idx, WEAPON_SLOT_TYPE nUsageType)
{
	const i3::vector<INVEN_BUFFER*>& db_list = m_WeaponDBList[nUsageType];
	return db_list[idx]->_ui32ItemID;
}

size_t ws::get_move_number(size_t curr, size_t max, bool dir)
{
	return uiMath::move_number( (INT32)curr, (INT32)0, (INT32)max-1, dir);
}