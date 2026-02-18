#include "pch.h"

#include "UIOptionAudio.h"
#include "UIUtil.h"
#include "UIClanFunction.h"

#include "EnvInfoContext.h"
#include "EnvSet.h"
#include "UIMainFrame.h"

#include "LocaleValue.h"

I3_CLASS_INSTANCE( UIOptionAudio);//, UIOptionBase );

namespace
{
	UIOptionAudio* g_this = 0;

	enum { BGM, EFFECT, MESSAGE, LANGUAGE, RADIOMESSAGE };
	enum { TEAM, REQUEST, REPORT, };

	REAL32 g_accumulate_time = 0.f;
	bool g_str_on_off = false;

	bool g_is_ui_loaded = false;

	enum ADJ 
	{
		ADJ_NONE_STATE = 0, ADJ_INPUT_STATE, ADJ_MSG_STATE,	//키 값 조정 상태
		ADJ_NONE_STR = 0, ADJ_INPUT_STR,	//키 값 스트링 상태
	};
	ADJ g_adjust_state = ADJ_NONE_STATE;
	ADJ g_adjust_str = ADJ_NONE_STR;

	UINT g_KeySlotIdx;
	GAME_KEY_STRUCT g_key_value;
	GAME_KEY_STRUCT g_none_value;

	enum { MAX_CONTROLKEY_COUNT = 3};

	GAME_KEY_MAP g_ControlKey_List[MAX_CONTROLKEY_COUNT] = {
		GAME_KEY_MAP_RADIOMESSAGE1, GAME_KEY_MAP_RADIOMESSAGE2, GAME_KEY_MAP_RADIOMESSAGE3	};

	struct SlotIndex
	{
		int col; 	int row;	//list container 에 인덱스
		int ctrl_row;	//slot control idx
	} g_dupl_slot_idx, g_ctrl_slot_idx;	// 중복키 체크를 위해 추가
}

namespace
{
	bool is_exception_keycode(I3I_KEY keycode);
}

extern "C" 
{
	int option_audio_click( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clicked( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_audio_glue[] =
{
	{"Click",		option_audio_click},
	{nullptr,				nullptr}
};

/************************//************************/
void UIOptionAudio::clicked(int idx1, int idx2)
{
	switch(idx1)
	{
	case BGM: case EFFECT:
		{
			bool enable = (idx2) ? false : true;
			mVolume[idx1].enable = enable;
			mVolume[idx1].edit.Enable( enable );
			apply_game();
		}
		break;

	case MESSAGE:	m_bEnableRadioMessage = idx2 != 0; apply_lua(); break;
	case LANGUAGE:	m_nRadioMessageType = idx2; apply_lua(); break;
	case RADIOMESSAGE:
		start_adjust_key(idx2); 	update_keyitem_list();
		break;
	}
}

/************************//************************/

UIOptionAudio::UIOptionAudio() : UIOptionBase()
{
	g_this = this;

	m_Callback1.SetObj(this);
	mVolume[BGM].edit.SetCallback(&m_Callback1);
	mVolume[BGM].edit.SetRange(0, 100);

	m_Callback2.SetObj(this);
	mVolume[EFFECT].edit.SetCallback(&m_Callback2);
	mVolume[EFFECT].edit.SetRange(0, 100);

	g_none_value._nDiviceType = -1;
	g_none_value._nState = 0;
}

UIOptionAudio::~UIOptionAudio()
{
}


/************************//************************/
void UIOptionAudio::_InitializeAtLoad( i3UIScene * pScene )
{
	UIOptionBase::_InitializeAtLoad(pScene);

	UIPopupBase* parent = (UIPopupBase*)getParent();
	mVolume[BGM].edit.SetControl( parent, pScene, "i3UIProgressBar", "i3UIButton1", "i3UIEditBox0" );
	mVolume[BGM].edit.Enable( mVolume[BGM].enable);
	mVolume[EFFECT].edit.SetControl( parent, pScene, "i3UIProgressBar0", "i3UIButton2","i3UIEditBox1" );
	mVolume[EFFECT].edit.Enable( mVolume[EFFECT].enable);

	if (mVolume[EFFECT].enable)
		mVolume[EFFECT].edit.SetNumber((int)mVolume[EFFECT].vol, true);

	if ( mVolume[BGM].enable )
		mVolume[BGM].edit.SetNumber((int)mVolume[BGM].vol, true);

	for(size_t i=0; i< MAX_KEY; ++i)
	{
		char str_st[20] = ""; char str_wnd[20] = "";	char str_btn[20] = "";
		wsprintf( str_wnd, "i3UIFramWnd%d", i+1); wsprintf( str_st, "i3UIStaticText%d", i+2);	wsprintf( str_btn, "i3UIBt%d", i+4);
		init_control_slot(pScene, m_EditSlot[i], str_wnd, str_st, str_btn );
	}
	
	// RadioButton On off
	i3UIButton * pRadioRightButton = (i3UIButton*) pScene->FindChildByName( "btnRadioLangKor");
	if(LocaleValue::Enable("EnableMultiRadioLanguage") == false)
	{
		if(pRadioRightButton != nullptr)
		{
			m_nRadioMessageType = 0;
			g_pEnvSet->m_nRadioMessageType = m_nRadioMessageType;
			pRadioRightButton->EnableCtrl(false);
		}
	}


	init_keyitem_list(); // KeyItem은 환경설정 팝업이 활성화 된 이후에 생성됩니다.
	update_keyitem_list();
}

/*virtual*/ void UIOptionAudio::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Audio.i3UIs", option_audio_glue);
}

void UIOptionAudio::OnLoadAllScenes()
{
	g_KeySlotIdx = 0;
	g_is_ui_loaded = true;
	update_keyitem_list();
}

bool UIOptionAudio::OnKey_Enter()
{
	if(isVisible() == false) return 0;

	for(int i=0; i<MAX_VOL;++i)
		mVolume[i].edit.OnKey_Enter();

	return 0;
}

bool UIOptionAudio::OnKey_Escape()
{
	if(isVisible() == false) return 0;

	for(int i=0; i<MAX_VOL;++i)
		mVolume[i].edit.OnKey_Escape();

	return 0;
}

void UIOptionAudio::OnUpdate( REAL32 rDeltaSeconds)
{
	if(isVisible() == false) return;

	for(int i=0; i<MAX_VOL;++i)
		mVolume[i].edit.OnUpdate(rDeltaSeconds);

	if ( g_adjust_state == ADJ_INPUT_STATE )
	{
		g_accumulate_time += rDeltaSeconds;
		display_adjust_str();

		I3I_KEY stroke_key = GameUI::GetKeyboardStroke();
		if ( stroke_key != I3I_KEY_UNKNOWN)
		{
			g_key_value._nDiviceType = GAME_DEVICE_KEYBOARD;
			g_key_value._nState = stroke_key;

			if( is_exception_keycode(stroke_key) )	//제외 키
			{
				if( stroke_key == I3I_KEY_ESC )	//키 조정을 취소 한다.
					finish_adjust_key();
			}
			else if ( duplicate_key((UINT32)stroke_key, GAME_DEVICE_KEYBOARD))	// 중복 키가 존재하는지
			{
				g_adjust_state = ADJ_MSG_STATE;
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"), nullptr, MAKE_CALLBACK(cb_change_keyitem), this);
			}
			else
			{
				I3ASSERT(g_KeySlotIdx < mKeyItemList.size());
				mKeyItemList[g_KeySlotIdx].value = g_key_value;
				UIPopupOption::instance()->setKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, g_KeySlotIdx, g_key_value);
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
				GameUI::MsgBox( MSG_TYPE_GAME_QUERY, GAME_RCSTRING("STBL_IDX_EP_OPTION_DUPLICATE_KEY"), nullptr, MAKE_CALLBACK(cb_change_keyitem), this);
			}
			else
			{
				I3ASSERT(g_KeySlotIdx < mKeyItemList.size());
				mKeyItemList[g_KeySlotIdx].value = g_key_value;
				UIPopupOption::instance()->setKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, g_KeySlotIdx, g_key_value);
				finish_adjust_key();
			}
			update_keyitem_list();
			return;
		}
	}
}

/************************//************************/
void UIOptionAudio::cb_edit_change1( INT32 value )
{
	mVolume[BGM].vol = value;
	apply_game();
}

void UIOptionAudio::cb_edit_change2( INT32 value )
{
	mVolume[EFFECT].vol = value;
	apply_game();
}

/************************//************************/
void UIOptionAudio::init_keyitem_list()
{
	// 리스트가 이미 존재한다면 더 추가되어선 안됨
	const size_t _checksize = mKeyItemList.size();
	if(_checksize != 0) return;

	// UIPopupOption의 KeyItem의 존재 여부. 존재하지 않는다면 여기서도 추가되어선 안됨
	if(!UIPopupOption::instance()->isInitKeyItem()) return;

	for(int i=0 ;i<MAX_CONTROLKEY_COUNT; i++)
	{
		mKeyItemList.push_back( UIPopupOption::instance()->getKeyItem(UI_KEYCTRL_TYPE_AUDIO, i));
	}
}
void UIOptionAudio::init_control_slot(i3UIScene * pScene, edit_slot & slot, const char* wnd, const char* st, const char* btn)
{
	i3GameNode* node1 = pScene->FindChildByName( wnd );
	i3GameNode* node2 = pScene->FindChildByName( st );
	i3GameNode* node3 = pScene->FindChildByName( btn );

	if(!node2 || !node3)
	{
		I3PRINTLOG(I3LOG_FATAL, "여기 없음 진행 못 함.. PBRe_Pref_Control,i3Uie 파일 확인");
	}

	slot.wnd = (i3UIFrameWnd*)node1;
	slot.st_ctrl = (i3UIStaticText*)node2;
	slot.btn_ctrl = (i3UIButton*)node3;
}

void UIOptionAudio::start_adjust_key(int idx)
{
	g_pFramework->m_EnableCapture = false;		//동영상 녹화 금지

	g_accumulate_time = 0.f;
	g_str_on_off = true;

	g_adjust_state = ADJ_INPUT_STATE;
	g_adjust_str = ADJ_INPUT_STR;

	g_KeySlotIdx = idx;

	UIPopupBase* parent = (UIPopupBase*)getParent();
	parent->SetInputDisable(true);
}

void UIOptionAudio::finish_adjust_key()
{
	g_pFramework->m_EnableCapture = true;	//동영상 녹화 활성화

	g_accumulate_time = 0.f;
	g_str_on_off = true;

	g_adjust_state = ADJ_NONE_STATE;
	g_adjust_str = ADJ_NONE_STR;

	UIPopupBase* parent = (UIPopupBase*)getParent();
	parent->SetInputDisable(false);
}

void UIOptionAudio::update_keyitem_list()
{
	// load_value(); // 키 아이템 정보를 최신화

	const size_t _size = mKeyItemList.size();
	for(size_t i=0; i<_size; ++i)
	{
		if (_size <= g_KeySlotIdx) continue;

		show_item_slot(i , mKeyItemList[i]);
	}
}

void UIOptionAudio::show_item_slot(UINT idx, const KeyItem& i)
{
	m_EditSlot[idx].wnd->EnableCtrl(true);
	m_EditSlot[idx].st_ctrl->SetText(i.wstr_name);

	if(g_adjust_state == ADJ_INPUT_STATE && g_KeySlotIdx == idx )
		display_adjust_str();
	else
		m_EditSlot[idx].btn_ctrl->SetText( GameUI::GetKeyboardString( (GAME_KEY_STRUCT*)&i.value ) );	
}

void UIOptionAudio::display_adjust_str()
{
	i3::rc_wstring wstr;

	if (g_accumulate_time > 0.503f)
	{
		g_str_on_off = !g_str_on_off;
		g_accumulate_time = 0.f;
	}

	if(g_str_on_off) 
		wstr = GAME_RCSTRING("STR_TBL_GUI_ETC_SC_INPUT");

	m_EditSlot[g_KeySlotIdx].btn_ctrl->SetText( wstr );
}

void UIOptionAudio::hide_item_slot(int idx)
{
	m_EditSlot[idx].wnd->EnableCtrl(false);
}

void UIOptionAudio::cb_change_keyitem(void* pThis,INT32 nParam)
{
	// 인게임일 경우에는 UIMainFrame이 없기 때문에 넘어갑니다.
	// 조건 : 인게임이 아닐때, Option Popup이 안켜져 있을때에는 세팅을 하지 않습니다.
	if( g_pFramework->IsBattleStage() == false && g_pFramework->GetUIMainframe()->IsOpenedPopup(UPW_OPTION) == false )
		return;

	if( nParam == MBR_OK)
	{
		UIPopupOption::instance()->setKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, g_KeySlotIdx, g_key_value);
		g_this->mKeyItemList[g_KeySlotIdx].value = g_key_value;		

		I3ASSERT(g_dupl_slot_idx.col <UI_KEYCTRL_TYPE_MAX);
		if(g_dupl_slot_idx.col == UI_KEYCTRL_TYPE_AUDIO) // 현재(Audio)탭
		{
			UIPopupOption::instance()->setKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, g_dupl_slot_idx.row, g_none_value);
			g_this->mKeyItemList[g_dupl_slot_idx.row].value = g_none_value;		
		}
		else	// Ctrl 탭
		{		
			UIPopupOption::instance()->setKeyItem_value((UI_KEYCTRL_LIST_TYPE)g_ctrl_slot_idx.col, g_ctrl_slot_idx.row, g_none_value);
		}
	}
	g_this->finish_adjust_key();
	g_this->update_keyitem_list();
}

bool UIOptionAudio::is_empty_keyitem()
{
	return duplicate_key(g_none_value._nState, g_none_value._nDiviceType);
}
/************************//************************/
void UIOptionAudio::show_scn()
{
	SetVisible(true);
	GetScene(0)->Attach(false);

	load_value();
	update_keyitem_list();

	// Radio Sound 우측 버튼(한국어) 제거용도
	i3UIButton * pRadioRightButton = (i3UIButton*) GetScene(0)->FindChildByName( "btnRadioLangKor");
	if(LocaleValue::Enable("EnableMultiRadioLanguage") == false)
	{
		if(pRadioRightButton != nullptr)
		{
			m_nRadioMessageType = 0;
			g_pEnvSet->m_nRadioMessageType = m_nRadioMessageType;
			pRadioRightButton->EnableCtrl(false);
		}
	}
}

void UIOptionAudio::default_value()
{
	m_bEnableRadioMessage = true;

#if defined( LOCALE_JAPAN )
	m_nRadioMessageType = LocaleValue::Enable("EnableMultiRadioLanguage") ? 1 : 0;
#else
	m_nRadioMessageType = 0;
#endif // LOCALE_JAPAN

	mVolume[BGM].enable = true;
	mVolume[BGM].vol = 60;
	mVolume[EFFECT].enable = true;
	mVolume[EFFECT].vol = 100;

	UIPopupOption::instance()->setKeyItem_default();

	const size_t _size = UIPopupOption::instance()->getKeyListSize(UI_KEYCTRL_TYPE_AUDIO);
	for(size_t i=0; i<_size; i++)
		mKeyItemList[i].set_default();

	if(g_is_ui_loaded) update_keyitem_list();
}

void UIOptionAudio::load_value()
{	
	m_bEnableRadioMessage = g_pEnvSet->m_bEnableRadioMessage;
	m_nRadioMessageType = g_pEnvSet->m_nRadioMessageType;
	
	mVolume[BGM].vol = (int)g_pEnvSet->m_fVolBGM;
	mVolume[BGM].enable = (g_pEnvSet->m_bEnableBGM) ? true : false;

	mVolume[EFFECT].vol = (int)g_pEnvSet->m_fVolEffect;
	mVolume[EFFECT].enable = (g_pEnvSet->m_bEnableEffect) ? true : false;

	// KeyItem이 존재하지 않은 경우, 이상 실행되어선 안됨
	if (!UIPopupOption::instance()->isInitKeyItem() || mKeyItemList.size() <= 0) return;

	const size_t _size = UIPopupOption::instance()->getKeyListSize(UI_KEYCTRL_TYPE_AUDIO);	
	
	I3ASSERT(mKeyItemList.size() <= _size);

	for(size_t i=0;i<_size;++i)
	{
		mKeyItemList[i].value = UIPopupOption::instance()->getKeyItem_value(UI_KEYCTRL_TYPE_AUDIO, i);
	}
}

void UIOptionAudio::save_value()
{
	if( is_modified_basic() )
	{
		g_pEnvSet->m_bEnableRadioMessage = m_bEnableRadioMessage;
		g_pEnvSet->m_nRadioMessageType = m_nRadioMessageType;
		g_pEnvSet->m_bEnableBGM = mVolume[BGM].enable;
		g_pEnvSet->m_fVolBGM = (REAL32)mVolume[BGM].vol;
		g_pEnvSet->m_bEnableEffect = mVolume[EFFECT].enable;
		g_pEnvSet->m_fVolEffect = (REAL32)mVolume[EFFECT].vol; 
		EnvInfoContext::i()->AddNetEnvSetBasicChange(BASIC);
	}

	if( !is_modified_key() ) return;

	bool diff = false;
	bool tmp = false;

	for(size_t i =0; i< UI_KEYCTRL_TYPE_MAX; i++)
	{
		const size_t _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
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

void UIOptionAudio::apply_game()
{
	//::InitSound();	// hansoft 9921. 서버로부터 받아온 세팅 정보를 적용. 

	if(g_pSndMng->IsPlayBGM() == false && !g_pFramework->IsBattleStage())
	{
		g_pSndMng->StartBGM("Sound/UI/PB_BGM_NO4_2.ogg", true, true, 1.f);
	}

	UINT8 bgm = (mVolume[BGM].enable) ? (UINT8)mVolume[BGM].vol : 0;
	UINT8 effect = (mVolume[EFFECT].enable) ? (UINT8)mVolume[EFFECT].vol : 0;

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();

	if( pSndCtx != nullptr )
	{
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_BGM,  bgm);
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, effect);
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_VOICE, effect);
	}
}

void UIOptionAudio::apply_lua()
{
	//bgm, effect
	for (int i = 0; i < MAX_VOL; ++i)
	{
		if (mVolume[i].edit.GetNumber() != (int)mVolume[i].vol)
			mVolume[i].edit.SetNumber(mVolume[i].vol);
	}

	//mute
	LuaState * L = 0;
	L = _CallLuaFunction( "SetMute");
	for(int i=0; i<MAX_VOL;++i)
		i3Lua::PushBoolean( L, mVolume[i].enable );
	UILua::EndLuaFunction(L , MAX_VOL);

	//message
	L = _CallLuaFunction("setValue");
	i3Lua::PushInteger(L, m_bEnableRadioMessage);
	i3Lua::PushInteger(L, m_nRadioMessageType);
	UILua::EndLuaFunction(L , 2);

	//language
//	ucf::ComboString cs;
//	switch( GetCodePage() )
//	{
//	case I3_LANG_CODE_KOREAN:
//		cs	<< GAME_RCSTRING("STR_POPUP_ENGLISH_LANGUAGE")	/*영문*/
//			<< GAME_RCSTRING("STR_POPUP_NATIVE_LANGUAGE");	/*한글*/
//		break;
//
//	default:
//#if defined (LOCALE_RUSSIA)
//		cs	<< GAME_RCSTRING("STR_POPUP_RUSSIA_LANGUAGE");	/*러시아어*/
//#elif defined(LOCALE_TURKEY)
//		cs	<< GAME_RCSTRING("STR_POPUP_NATIVE_LANGUAGE")	/*터키어*/
//			<< GAME_RCSTRING("STR_POPUP_ENGLISH_LANGUAGE");	/*영문*/
//#else
//		cs	<< GAME_RCSTRING("STR_POPUP_ENGLISH_LANGUAGE");	/*영문*/
//#endif
//		break;
//	}
//
//	const i3::wstring& cs_str = cs;
//
//	L = _CallLuaFunction("SetFilterList");
//	i3Lua::PushStringUTF16To8(L, cs_str.c_str(), cs_str.size() );
//	i3Lua::PushInteger(L, m_nRadioMessageType);
//	UILua::EndLuaFunction(L , 2);
}

bool UIOptionAudio::is_modified()
{
	if( is_modified_basic() == true ) return true;
	if( is_modified_key() == true ) return true;
	return false;
}
bool UIOptionAudio::is_modified_basic()
{
	if(g_pEnvSet->m_bEnableRadioMessage != m_bEnableRadioMessage) return true;
	if(g_pEnvSet->m_nRadioMessageType != m_nRadioMessageType) return true;

	if(g_pEnvSet->m_bEnableBGM != (bool)mVolume[BGM].enable) return true;
	if(g_pEnvSet->m_fVolBGM != (REAL32)mVolume[BGM].vol) return true;

	if(g_pEnvSet->m_bEnableEffect != (bool)mVolume[EFFECT].enable) return true;
	if(g_pEnvSet->m_fVolEffect != (REAL32)mVolume[EFFECT].vol) return true;

	return false;
}
bool UIOptionAudio::is_modified_key()
{
	for(int i=0; i<UI_KEYCTRL_TYPE_MAX; i++)
	{
		size_t _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		for(size_t j=0; j<_size; j++)
		{
			if(UIPopupOption::instance()->isMatchValue((UI_KEYCTRL_LIST_TYPE)i, j) == false)
				return true;
		}
	}
	return false;
}

bool UIOptionAudio::duplicate_key(UINT32 key, INT8 device_type)
{
	for(int i=0; i<UI_KEYCTRL_TYPE_MAX; i++)
	{
		const size_t _size = UIPopupOption::instance()->getKeyListSize((UI_KEYCTRL_LIST_TYPE)i);
		I3ASSERT(_size != 0);

 		for(size_t j=0; j<_size; j++)
		{
			if(g_KeySlotIdx == j) continue;

			if(UIPopupOption::instance()->isDuplicateKey(key, device_type, (UI_KEYCTRL_LIST_TYPE)i ,j))
			{
				if(i == UI_KEYCTRL_TYPE_AUDIO)
				{
					g_dupl_slot_idx.col = i; g_dupl_slot_idx.row = j;
					return true;
				}
				else
				{
					g_dupl_slot_idx.col = i;
					g_ctrl_slot_idx.col = i; g_ctrl_slot_idx.row = j;
					return true;
				}
			}
		}
	}

	return false;
}

namespace
{
	bool is_exception_keycode(I3I_KEY keycode)
	{
		if( keycode == I3I_KEY_BREAK || keycode == I3I_KEY_WIN || keycode == I3I_KEY_NUMLOCK ||
			keycode == I3I_KEY_PRTSC || keycode == I3I_KEY_F1 || keycode == I3I_KEY_ESC ||
			keycode == I3I_KEY_1 || keycode == I3I_KEY_2 || keycode == I3I_KEY_3 || keycode == I3I_KEY_4 || 
			keycode == I3I_KEY_5 || keycode == I3I_KEY_6 || keycode == I3I_KEY_7 || keycode == I3I_KEY_8 ||
			keycode == I3I_KEY_9 || keycode == I3I_KEY_0)
			return true;

		return false;
	}
}
