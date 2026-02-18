#include "pch.h"

#include "UIOptionControl.h"
#include "UIUtil.h"
#include "UIMainFrame.h"

#include "EnvInfoContext.h"

I3_CLASS_INSTANCE( UIOptionControl);

namespace
{
	UIOptionControl* g_this = 0;
	i3UIFrameWnd * g_main_frame = 0;
	REAL32 g_accumulate_time = 0.f;
	bool g_str_on_off = false;

	//클래스 생성시에 default_value() 가 호출된다. 여기서 ui 로딩이 되기 전에 update_keyitem_list()가 호출 된다면
	//크래쉬가 발생된다. 실제로 키값을 초기화 할 때 slot 갱신을 해야 된다.
	bool g_is_ui_loaded = false;	
	bool g_isAudioSlot = false;

	enum { MOUSE, SLOT1, SLOT2, SLOT3, };
	enum { MOUSE_NONE = -1 };	//마우스 키 값 상태
	enum ADJ 
	{
		ADJ_NONE_STATE = 0, ADJ_INPUT_STATE, ADJ_MSG_STATE,	//키 값 조정 상태
		ADJ_NONE_STR = 0, ADJ_INPUT_STR,	//키 값 스트링 상태
	};
	ADJ g_adjust_state = ADJ_NONE_STATE;
	ADJ g_adjust_str = ADJ_NONE_STR;

	struct SlotIndex
	{
		int col; 	int row;	//list container 의 인덱스
		int ctrl_row;	//slot control idx
	} g_slot_idx, g_dupl_slot_idx;
	GAME_KEY_STRUCT g_key_value, g_none_value;
}

namespace
{
	bool enable_Keymenu( INT32 menu );
	bool is_exception_keycode(I3I_KEY keycode);
}

extern "C" 
{
	int option_control_click( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clicked( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_control_glue[] =
{
	{"Click",		option_control_click},
	{nullptr,				nullptr}
};

/************************//************************/
void UIOptionControl::clicked(int idx1, int idx2)
{
	switch(idx1)
	{
	case MOUSE: m_bEnableMouseInvert = idx2 != 0; apply_lua(); break;
	case SLOT1: case SLOT2:
			start_adjust_key(idx1, idx2);
			update_keyitem_list();
		break;
	case SLOT3:	m_Scroll.SetFocus();	break;
	}
}

/************************//************************/

UIOptionControl::UIOptionControl() : UIOptionBase(), m_idxStartSlot(0)
{
	g_this = this;
	g_is_ui_loaded = false;

	m_ScrollCallback.SetObj(this);
	m_Scroll.SetCallback( &m_ScrollCallback );

	mPEdit.SetRange(0.1f, 1.2f);
	m_PECallback.SetObj(this);
	mPEdit.SetCallback(&m_PECallback);

	g_none_value._nDiviceType = -1;
	g_none_value._nState = 0;

	init_keyitem_list();
}

UIOptionControl::~UIOptionControl()
{
}

/************************//************************/
void UIOptionControl::_InitializeAtLoad( i3UIScene * pScene )
{
	UIOptionBase::_InitializeAtLoad(pScene);

	bool isctr1 = i3::generic_is_iequal(pScene->getScriptFile(), i3::string("script\\PointBlankRe_Scroll.lua"));

	if( isctr1 == true )
		return;

	UIPopupBase* parent = (UIPopupBase*)getParent();
	mPEdit.SetControl( parent, pScene, "i3UIProgressBar", "i3UIButton", "i3UIEditBox1" );
	mPEdit.Enable(true);

	for(int i=0; i<MAX_TITLE; ++i) 
	{
		char str_wnd[20] = "";		char str_st[20] = "";
		wsprintf( str_wnd, "i3UIFrmWnd%d", i+10);	wsprintf( str_st, "i3UISt%d", i);
		init_control_slot(pScene, m_TitleSlot[i], str_wnd, str_st );
	}

	for(int i=0; i<COL; ++i)
	{
		for(int j=0; j<ROW; ++j)
		{
			char str_wnd[20] = ""; char str_st[20] = ""; char str_btn[20] = "";
			wsprintf( str_wnd, "i3UIFrm%d%d", i,j);	wsprintf( str_st, "i3UISt%d%d", i,j);	wsprintf( str_btn, "i3UIBt%d%d", i,j);
			init_control_slot(pScene, m_EditSlot[i][j], str_wnd, str_st, str_btn);
		}
	}

	g_main_frame = static_cast<i3UIFrameWnd*>( pScene->FindChildByName("i3UIFrameWnd6") );

	m_idxStartSlot = 0;

	init_keyitem_list();
	update_keyitem_list();
}

void UIOptionControl::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Control.i3UIs", option_control_glue);

	m_Scroll.OnCreate( (UIBase*)this );
	m_Scroll.RegisterScene("Scene/Main/PointBlankRe_Scroll.i3UIs");
}

void UIOptionControl::show_scn()
{
	SetVisible(true);
	GetScene(0)->Attach(false);

	m_Scroll.GetScene()->Attach(false);

	m_idxStartSlot = 0;
	load_value();
	update_keyitem_list();
	m_Scroll.SetFocus();
}

bool UIOptionControl::is_empty_keyitem()
{
	return duplicate_key(g_none_value._nState, g_none_value._nDiviceType);
}

void UIOptionControl::OnLoadAllScenes()
{
	g_adjust_state = ADJ_NONE_STATE;
	g_adjust_str = ADJ_NONE_STR;

	m_Scroll.OnLoadAllScenes();

	i3::pack::RECT rect;
	g_main_frame->getAccumulatedRect(&rect); 
	m_Scroll.SetPosition( (UINT32)rect.right - 20, (UINT32)rect.top + 51 );

	if (g_pEnvSet->IsBaseOnUIVersion_1_0())
		m_Scroll.SetSize(17, 265);	
	else
		m_Scroll.SetSize(9, 265);

	int max = ( mKeyItemList[0].size() > mKeyItemList[1].size() ) ? mKeyItemList[0].size() : mKeyItemList[1].size();
	m_Scroll.SetRange( max - ROW + 1);
	m_Scroll.SetPage(1);
	m_Scroll.SetTick(1);
	m_Scroll.SetFocus();

	m_idxStartSlot = 0;
	g_is_ui_loaded = true;
}

void	UIOptionControl::OnUnloadAllScenes()
{
	m_Scroll.OnUnloadAllScenes();
}

bool UIOptionControl::OnKey_Enter()
{
	if(isVisible() == false) return 0;
	mPEdit.OnKey_Enter();
	return 0;
}

bool UIOptionControl::OnKey_Escape()
{
	if(isVisible() == false) return 0;
	mPEdit.OnKey_Escape();
	return 0;
}

void UIOptionControl::OnUpdate( REAL32 rDeltaSeconds)
{
	UIOptionBase::OnUpdate(rDeltaSeconds);

	if(isVisible() == false) return;

	mPEdit.OnUpdate(rDeltaSeconds);

	if( g_adjust_state == ADJ_INPUT_STATE )		//control key 변경때...
	{
		g_accumulate_time += rDeltaSeconds;
		display_adjust_str();

		I3I_KEY stroke_key = GameUI::GetKeyboardStroke();
		if( stroke_key != I3I_KEY_UNKNOWN )
		{
			g_key_value._nDiviceType = GAME_DEVICE_KEYBOARD;
			g_key_value._nState = stroke_key;

#if defined( I3_DEBUG ) || defined( BUILD_RELEASE_QA_VERSION) || defined(LOCALE_BRAZIL)
			if (is_exception_keycode(stroke_key) ||
				deactivated_key((UINT32)stroke_key, GAME_DEVICE_KEYBOARD))	//제외 키, Deactivated Key
			{
				if (stroke_key == I3I_KEY_ESC)	//키 조정을 취소 한다.
					finish_adjust_key();
			}
#else
			if (is_exception_keycode(stroke_key))
			{
				if (stroke_key == I3I_KEY_ESC)	//키 조정을 취소 한다.
					finish_adjust_key();
			}
#endif
			
			else if( duplicate_key((UINT32)stroke_key, GAME_DEVICE_KEYBOARD) )
			{
				g_adjust_state = ADJ_MSG_STATE;
				/*해당 키는 사용 중입니다. 그래도 교체하시겠습니까?*/
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, 
					GAME_RCSTRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"),
					nullptr, MAKE_CALLBACK(cb_change_keyitem), this);
			}
			else
			{
				//변경
				mKeyItemList[g_slot_idx.col][g_slot_idx.row].value = g_key_value;
				UIPopupOption::instance()->setKeyItem_value((UI_KEYCTRL_LIST_TYPE)g_slot_idx.col, g_slot_idx.row, g_key_value);
				finish_adjust_key();
			}
			update_keyitem_list();
			return;
		}

		int stroke_mouse = GameUI::GetMouseStroke();
		if( stroke_mouse != MOUSE_NONE )
		{
			g_key_value._nDiviceType = GAME_DEVICE_MOUSE;
			g_key_value._nState = stroke_mouse;

			if( duplicate_key((UINT32)stroke_mouse, GAME_DEVICE_MOUSE) )
			{
				g_adjust_state = ADJ_MSG_STATE;
				/*해당 키는 사용 중입니다. 그래도 교체하시겠습니까?*/
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, 
					GAME_RCSTRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"),
					nullptr, MAKE_CALLBACK(cb_change_keyitem), this);
			}
			else
			{
				mKeyItemList[g_slot_idx.col][g_slot_idx.row].value = g_key_value;
				UIPopupOption::instance()->setKeyItem_value((UI_KEYCTRL_LIST_TYPE)g_slot_idx.col, g_slot_idx.row, g_key_value);
				finish_adjust_key();
			}
			update_keyitem_list();
			return;
		}
	}
}

bool	UIOptionControl::OnEvent( UINT32 event, i3ElementBase * pObj,  UINT32 param2, I3_EVT_CODE code)
{
	if( UIOptionBase::OnEvent(event, pObj, param2, code) == true )
		return true;

	return m_Scroll.OnEvent(event, pObj, param2, code);
}

/************************//************************/
void UIOptionControl::default_value()
{
	m_bEnableMouseInvert = false;
	m_rMouseSensivity = 0.5f; //0.5f 값이다.

	UIPopupOption::instance()->setKeyItem_default();

	for(int i=0; i<COL; i++)
	{
		const size_t _size = mKeyItemList[i].size();
		for(size_t j=0; j<_size; j++)
		{
			mKeyItemList[i][j].set_default();
		}
	}

	if( g_is_ui_loaded ) update_keyitem_list();
}

void UIOptionControl::load_value()
{
	m_bEnableMouseInvert = g_pEnvSet->m_bEnableMouseInvert;
	m_rMouseSensivity = g_pEnvSet->m_rMouseSensivity;

	load_src_key_value();
}

void UIOptionControl::save_value()
{
	if( is_modified_basic() )
	{
		g_pEnvSet->m_bEnableMouseInvert = m_bEnableMouseInvert;
		g_pEnvSet->m_rMouseSensivity = m_rMouseSensivity;
		EnvInfoContext::i()->AddNetEnvSetBasicChange(BASIC);
	}
	if( !is_modified_key() ) return;

	save_key_value_to_src();
}

void UIOptionControl::apply_game()
{

}

void UIOptionControl::apply_lua()
{
	LuaState * L = 0;

	L = _CallLuaFunction( "SetValue");
	i3Lua::PushInteger( L, m_bEnableMouseInvert);
	UILua::EndLuaFunction(L , 1);

	mPEdit.SetNumber(m_rMouseSensivity);
}

bool UIOptionControl::is_modified()
{
	if( is_modified_basic() == true ) return true;
	if( is_modified_key() == true ) return true;
	return false;
}

bool UIOptionControl::is_modified_basic()
{
	if( m_bEnableMouseInvert != g_pEnvSet->m_bEnableMouseInvert )		return true;
	if( fabs(m_rMouseSensivity - g_pEnvSet->m_rMouseSensivity) > 0.00999f  )		return true;
	return false;
}
bool UIOptionControl::is_modified_key()
{
	for(int i=0;i<UI_KEYCTRL_TYPE_MAX;++i)
	{
		int size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		for(int j = 0; j<size;++j)
		{
			if( UIPopupOption::instance()->getKeyItem_slotType((UI_KEYCTRL_LIST_TYPE)i, j) == ITEM )
			{
				if( UIPopupOption::instance()->isMatchValue((UI_KEYCTRL_LIST_TYPE)i, j) == false ) 
					return true;
			}
		}
	}
	return false;
}

/************************//************************/
void UIOptionControl::start_adjust_key(int idx1, int idx2)
{
	if( idx2 < 0 || idx2 >= ROW) return;		//ROW

	g_pFramework->m_EnableCapture = false;		//동영상 녹화 금지

	g_accumulate_time = 0.f;
	g_str_on_off = true;

	g_adjust_state = ADJ_INPUT_STATE;
	g_adjust_str = ADJ_INPUT_STR;

	g_slot_idx.col = idx1 - 1;
	g_slot_idx.row = m_idxStartSlot + idx2;
	g_slot_idx.ctrl_row = idx2;

	UIPopupBase* parent = (UIPopupBase*)getParent();
	parent->SetInputDisable(true);
	mPEdit.Enable(false);
}

void UIOptionControl::finish_adjust_key()
{
	g_pFramework->m_EnableCapture = true;	//동영상 녹화 활성화

	g_accumulate_time = 0.f;
	g_str_on_off = true;
	
	g_adjust_state = ADJ_NONE_STATE;
	g_adjust_str = ADJ_NONE_STR;

	UIPopupBase* parent = (UIPopupBase*)getParent();
	parent->SetInputDisable(false);
	mPEdit.Enable(true);
	
	m_Scroll.SetFocus();
}


/************************//************************/
void UIOptionControl::cb_scroll( INT32 inx, INT32 old_idx )
{
	m_idxStartSlot = inx;
	update_keyitem_list();
}

void UIOptionControl::cb_edit_change( REAL32 value )
{
	m_rMouseSensivity = value;	//floorf( value * 100.f ) * 0.01f + 0.0000001f;
}

/**********************//**********************/

void UIOptionControl::init_keyitem_list()
{

	for(int i=0 ;i<COL; i++)
	{
		// i 번째의 리스트가 이미 있다면 더 추가되어선 안됨
		const size_t _checkSize = mKeyItemList[i].size();
		if(_checkSize != 0) continue;

		if(!UIPopupOption::instance()->isInitKeyItem()) return;

		int size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		for(int j=0; j<size; j++)
		{
			mKeyItemList[i].push_back( UIPopupOption::instance()->getKeyItem((UI_KEYCTRL_LIST_TYPE)i, j));
		}
	}
}

void UIOptionControl::hide_title_slot(int col)
{
	m_TitleSlot[col].wnd->EnableCtrl(false);
}
void UIOptionControl::show_title_slot(int col, int idx, const KeyItem& i)
{
	m_TitleSlot[col].wnd->EnableCtrl(true);
	m_TitleSlot[col].st_ctrl->SetText(i.wstr_name);

	VEC3D* e_pos = m_EditSlot[col][idx].wnd->getPos();
	VEC3D* t_pos = m_TitleSlot[col].wnd->getPos();

	m_TitleSlot[col].wnd->setPos(t_pos->x, e_pos->y);
}

void UIOptionControl::hide_item_slot(int col, int idx)
{
	m_EditSlot[col][idx].wnd->EnableCtrl(false);
}

void UIOptionControl::display_adjust_str()
{
	i3::rc_wstring wstr;

	if(g_accumulate_time > 0.503f)
	{
		g_str_on_off = !g_str_on_off; g_accumulate_time = 0.f;
	}
	
	if(g_str_on_off) 
		wstr = GAME_RCSTRING("STR_TBL_GUI_ETC_SC_INPUT");

	m_EditSlot[g_slot_idx.col][g_slot_idx.ctrl_row].btn_ctrl->SetText( wstr );
}

void UIOptionControl::cb_change_keyitem(void* pThis,INT32 nParam)
{
	// 인게임일 경우에는 UIMainFrame이 없기 때문에 넘어갑니다.
	// 조건 : 인게임이 아닐때, Option Popup이 안켜져 있을때에는 세팅을 하지 않습니다.
	if( g_pFramework->IsBattleStage() == false && g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_OPTION) == false )
		return;

	if( nParam == MBR_OK)
	{
		UIPopupOption::instance()->setKeyItem_value((UI_KEYCTRL_LIST_TYPE)g_slot_idx.col, g_slot_idx.row, g_key_value);
		g_this->mKeyItemList[g_slot_idx.col][g_slot_idx.row].value = g_key_value;
		
		I3ASSERT(g_dupl_slot_idx.col <UI_KEYCTRL_TYPE_MAX);
		if(!g_isAudioSlot)
		{
			g_this->mKeyItemList[g_dupl_slot_idx.col][g_dupl_slot_idx.row].value = g_none_value;
			UIPopupOption::instance()->setKeyItem_value((UI_KEYCTRL_LIST_TYPE)g_dupl_slot_idx.col, g_dupl_slot_idx.row, g_none_value);
		}
		else
		{
			UIPopupOption::instance()->setKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, g_dupl_slot_idx.row, g_none_value);
		}
	}

	g_this->finish_adjust_key();
	g_this->update_keyitem_list();
}

void UIOptionControl::show_item_slot(int col, int idx, const KeyItem& i)
{
	m_EditSlot[col][idx].wnd->EnableCtrl(true);
	m_EditSlot[col][idx].st_ctrl->SetText(i.wstr_name);

#if defined( I3_DEBUG ) || defined( BUILD_RELEASE_QA_VERSION) || defined(LOCALE_BRAZIL)
	if (!m_EditSlot[col][idx].btn_ctrl->isActivated())
	{
		m_EditSlot[col][idx].btn_ctrl->setInputDisable(false);
		m_EditSlot[col][idx].btn_ctrl->removeState(I3UI_STATE_DEACTIVATED);
		m_EditSlot[col][idx].btn_ctrl->setModifyState();
	}
#endif
	
	if(g_adjust_state == ADJ_INPUT_STATE && 
		g_slot_idx.col == col && g_slot_idx.ctrl_row == idx )
		display_adjust_str();
	else
		m_EditSlot[col][idx].btn_ctrl->SetText( GameUI::GetKeyboardString( (GAME_KEY_STRUCT*)&i.value ) );	
}

void UIOptionControl::update_keyitem_list()
{
	// 키 아이템 정보 최신화
	load_src_key_value();
	
	//Title
	for(int i=0;i<MAX_TITLE;++i)
	{
		hide_title_slot( i );
	}

	int title_idx = 0;
	int ItemListsize = mKeyItemList[0].size();
	for(int j = 0; j<ROW;++j)
	{
		int idx = m_idxStartSlot + j;
		if(ItemListsize <= idx ) continue;

		if( mKeyItemList[0][idx].type == TITLE )
			show_title_slot( title_idx++, j, mKeyItemList[0][idx] );
	}

	//Item
	for(int i=0;i<COL;++i)
	{
		int size = mKeyItemList[i].size();

		for(int j = 0; j<ROW;++j)
		{
			int idx = m_idxStartSlot + j;

			hide_item_slot( i, j );
			if( size <= idx ) continue;

			if( mKeyItemList[i][idx].type == ITEM )
			{
				show_item_slot( i, j, mKeyItemList[i][idx] );

#if defined( I3_DEBUG ) || defined( BUILD_RELEASE_QA_VERSION) || defined(LOCALE_BRAZIL)
				if (mKeyItemList[i][idx].menu == GAME_KEY_MAP_CHAT)
				{
					m_EditSlot[i][j].btn_ctrl->setInputDisable(true);
					m_EditSlot[i][j].btn_ctrl->setState(I3UI_STATE_DEACTIVATED);
					m_EditSlot[i][j].btn_ctrl->setModifyState();
				}
#endif

			}
		}
	}
}
/************************//************************/
void UIOptionControl::load_src_key_value()
{
	for(int i=0;i<COL;++i)
	{
		const size_t _size = mKeyItemList[i].size();
		for(size_t j = 0; j<_size;++j)
		{
			if( mKeyItemList[i][j].type == ITEM )
			{
				mKeyItemList[i][j].value = UIPopupOption::instance()->getKeyItem_value((UI_KEYCTRL_LIST_TYPE)i, j);
			}
		}
	}
}
void UIOptionControl::save_key_value_to_src()
{
	bool diff = false;
	bool tmp = false;
	
	for(size_t i =0; i< UI_KEYCTRL_TYPE_MAX; i++)
	{
		const size_t _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		I3ASSERT(_size != 0);
		for(size_t j=0; j<_size; j++)
		{
			if(UIPopupOption::instance()->getKeyItem_slotType((UI_KEYCTRL_LIST_TYPE)i, j) == ITEM)
			{
				tmp = UIPopupOption::instance()->isMatchValue((UI_KEYCTRL_LIST_TYPE)i, j);
				if(!tmp)
					UIPopupOption::instance()->saveKeyItem((UI_KEYCTRL_LIST_TYPE)i, j);

				if(!diff && !tmp) diff = !tmp;
			}
		}
	}

	g_pEnvSet->CheckKeyMap();
	if(diff) EnvInfoContext::i()->AddNetEnvSetBasicChange(KEY);
}
/************************//************************/
void UIOptionControl::init_control_slot(i3UIScene * pScene, static_slot & slot, const char* wnd, const char* st)
{
	i3GameNode* node1 = pScene->FindChildByName( wnd );
	i3GameNode* node2 = pScene->FindChildByName( st );

	if(!node1 || !node2)
	{
		I3PRINTLOG(I3LOG_FATAL, "여기 없음 진행 못 함.. PBRe_Pref_Control.i3UIs 파일 확인");
	}

	slot.wnd = (i3UIFrameWnd*)node1;
	slot.st_ctrl = (i3UIStaticText*)node2;
}

void UIOptionControl::init_control_slot(i3UIScene * pScene, edit_slot & slot, const char* wnd, const char* st, const char* btn)
{
	i3GameNode* node1 = pScene->FindChildByName( wnd );
	i3GameNode* node2 = pScene->FindChildByName( st );
	i3GameNode* node3 = pScene->FindChildByName( btn );

	if(!node1 || !node2 || !node3)
	{
		I3PRINTLOG(I3LOG_FATAL, "여기 없음 진행 못 함.. PBRe_Pref_Control,i3Uie 파일 확인");
	}

	slot.wnd = (i3UIFrameWnd*)node1;
	slot.st_ctrl = (i3UIStaticText*)node2;
	slot.btn_ctrl = (i3UIButton*)node3;
}

bool UIOptionControl::deactivated_key(UINT32 key, INT8 device_type)
{
	for (int i = 0; i < UI_KEYCTRL_TYPE_MAX; ++i)	 // UI_KEYCTRL_TYPE_MAX : enum
	{
		int _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		I3ASSERT(_size != 0);

		bool bSameCol = false;

		if (g_slot_idx.col == i && i != UI_KEYCTRL_TYPE_AUDIO)
			bSameCol = true;

		for (int j = 0; j < _size; ++j)
		{
			// Key type이 title일 경우는 중복검사 하지 않음 
			if (UIPopupOption::instance()->getKeyItem_slotType((UI_KEYCTRL_LIST_TYPE)i, j) == TITLE) continue;
			// 현재 Key Item과 중복검사 하지 않음
			if (bSameCol && g_slot_idx.row == j) 	continue;

			if (UIPopupOption::instance()->isDuplicateKey(key, device_type, (UI_KEYCTRL_LIST_TYPE)i, j))
			{
				if (i == UI_KEYCTRL_TYPE_AUDIO)
				{	// audio tab에서의 처리
					g_dupl_slot_idx.row = j;
					g_isAudioSlot = true;

					if (m_EditSlot[i][j].btn_ctrl->isActivated() == false)
						return true;
				}
				else
				{	// gamekey tab에서의 처리
					g_dupl_slot_idx.col = i;	g_dupl_slot_idx.row = j;	//중복된 위치
					g_isAudioSlot = false;

					if (m_EditSlot[i][j].btn_ctrl->isActivated() == false || 
						mKeyItemList[i][j].menu == GAME_KEY_MAP_CHAT)
						return true;
				}
			}
		}
	}

	return false;
}
bool UIOptionControl::duplicate_key(UINT32 key, INT8 device_type)
{
	for(int i = 0; i < UI_KEYCTRL_TYPE_MAX; ++i )	 // UI_KEYCTRL_TYPE_MAX : enum
	{
		int _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		I3ASSERT(_size != 0);

		bool bSameCol = false;
		
		if( g_slot_idx.col == i && i != UI_KEYCTRL_TYPE_AUDIO)
			bSameCol = true;

		for(int j = 0; j<_size; ++j)
		{
			// Key type이 title일 경우는 중복검사 하지 않음 
			if( UIPopupOption::instance()->getKeyItem_slotType((UI_KEYCTRL_LIST_TYPE)i, j) == TITLE ) continue;
			// 현재 Key Item과 중복검사 하지 않음
			if( bSameCol && g_slot_idx.row == j ) 	continue;

			if( UIPopupOption::instance()->isDuplicateKey( key, device_type, (UI_KEYCTRL_LIST_TYPE)i, j))
			{
				if (i == UI_KEYCTRL_TYPE_AUDIO)
				{	// audio tab에서의 처리
					g_dupl_slot_idx.row = j;	
					g_isAudioSlot = true;
					return true;
				}
				else
				{	// gamekey tab에서의 처리
					g_dupl_slot_idx.col = i;	g_dupl_slot_idx.row = j;	//중복된 위치
					g_isAudioSlot= false; 
					return true;
				}
			}
		}
	}

	return false;
}

/**********************//**********************/
namespace
{
	bool enable_Keymenu( INT32 menu )
	{
		switch( menu)
		{
		case	GAME_KEY_MAP_VIDEOCAPTURE :		return (LocaleValue::Enable("EnableVideoCapture") == true) ? true : false;
		case	GAME_KEY_MAP_INGAME_SHOP :		return (LocaleValue::Enable("FieldShop") == true) ? true : false;
#if defined(USE_INGAME_REPORT)
		case	GAME_KEY_MAP_INGAME_REPORT :	return true;
#endif
		case	GAME_KEY_MAP_TOGGLE_MAP :	return true;
		}
		return true;
	}

	bool is_exception_keycode(I3I_KEY keycode)
	{
		if( keycode == I3I_KEY_BREAK || keycode == I3I_KEY_WIN || keycode == I3I_KEY_NUMLOCK ||
			keycode == I3I_KEY_PRTSC || keycode == I3I_KEY_F1 || keycode == I3I_KEY_ESC )
			return true;

		return false;
	}
}
