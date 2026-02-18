#include "pch.h"
#include "UIPopupColorNickname.h"
#include "UIUtil.h"
#include "ShopItemInfo.h"

#include "UserInfoContext.h"
#include "i3Base/ColorString.h"

#include "UIPhaseReadyRoom.h"
#include "UI/ItemConsumer.h"


static const REAL32 g_colorbox_width = 24.0f;
static const REAL32 g_colorbox_height = 10.0f;
static const REAL32 g_selectionbox_width = g_colorbox_width + 5.0f;
static const REAL32 g_selectionbox_height = g_colorbox_height + 3.4f;


I3_CLASS_INSTANCE(UIPopupColorNickname);


UIPopupColorNickname::UIPopupColorNickname() 
{
	m_selectionbox = nullptr;
	m_palette = nullptr;
	m_selectedColorBtn =nullptr;
	m_chatBox = nullptr;

	RegisterGameEventFunc(EVENT_ITEM_AUTH, &UIPopupColorNickname::ReceiveEvent_Item_Auth);
}

UIPopupColorNickname::~UIPopupColorNickname()
{
	I3_SAFE_RELEASE(m_selectionbox);
	i3::cu::for_each_safe_release_clear(m_colorboxes, &i3UIButton::Release);
}

void UIPopupColorNickname::OnCreate(i3GameNode* parent)
{
	UIPopupItemBase::OnCreate(parent);
	AddScene("Scene/Popup/PBRe_PopUp_Color_Nickname.i3UIs");
}

void UIPopupColorNickname::_InitializeAtLoad(i3UIScene* scene)
{
	UIPopupBase::_InitializeAtLoad(scene);

	m_palette = (i3UIFrameWnd*)scene->FindChildByName("palette_frame");
	m_chatBox = (i3UIEditBox*)scene->FindChildByName("eb_chat");

	_CreateSelectionbox(scene);
	_CreateColorboxes(scene);

	m_chatLogStrOnly.clear();
	UpdateNickname();
}

bool UIPopupColorNickname::OnEntranceStart(void* pArg1, void* pArg2)
{
	if(UIPopupItemBase::OnEntranceStart(pArg1, pArg2) == false) 
		return false;

	// ItemID, DBIndex는,UIPopupItemBase::_SetItemInfo()에서 세팅된다.
	I3ASSERT(m_ItemID != INVALID_ITEM_ID);
	I3ASSERT(m_AuthWareDBIndex != INVALID_WAREDB_INDEX);

	m_selectedColorBtn = nullptr;

	SyncIMEUI();
	UpdateNickname();

	return true;
}

bool UIPopupColorNickname::OnEnterMouse(i3UIControl* pControl)
{
	UIPopupItemBase::OnEnterMouse(pControl);

	if (_IsColorbox(pControl))
	{
		m_selectionbox->SetVisible(true);

		VEC3D* pos = pControl->getPos();
		m_selectionbox->setPos(pos->x - 2.5f, pos->y - 1.70f);
	}

	return true;
}

void UIPopupColorNickname::OnLeaveMouse(i3UIControl* pControl)
{
	UIPopupItemBase::OnLeaveMouse(pControl);

	if (_IsColorbox(pControl))
	{
		m_selectionbox->SetVisible(false);
	}
}

bool UIPopupColorNickname::OnClick(i3UIControl* pControl)
{
	UIPopupItemBase::OnEnterMouse(pControl);

	if(_IsColorbox(pControl))
	{
		if(i3::kind_of<i3UIButton*>(pControl))
		{
			m_selectedColorBtn = static_cast<i3UIButton*>(pControl);
			UpdateNickname();
		}
	}

	return true;
}

bool UIPopupColorNickname::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	bool bRv = UIPopupItemBase::OnEvent(event, pObj, param2, code);

	if( event == I3_EVT_IME)
	{
		switch( param2)
		{
		case EDIT_STATE_CLEAR :
		case EDIT_STATE_RETURN:
		case EDIT_STATE_CHANGE_COMPOSITION_CHAR :
		case EDIT_STATE_CHANGE_INPUT_CHAR :
			{
//				if( param2 == EDIT_STATE_RETURN)
//					m_pChatBox->GetIME()->SetInputText( L"", 0);
			}
			break;

		case EDIT_STATE_CHANGE_LANGUAGE :
		case EDIT_STATE_CHANGE_IME_MODE :
			{
				SyncIMEUI();
			}

			break;
		}
	}

	return bRv;
}

bool UIPopupColorNickname::OnKey_Enter( void)
{
	if(!m_chatBox)
		return false;

	i3::wstring chatStr(m_chatBox->getText());

	const WCHAR16* strChat = m_chatBox->getText();

	if(strChat && *strChat)
	{		
		m_chatLogStrOnly = i3::wstring(strChat);
		m_chatBox->SetText(L"");
		m_chatBox->SetIMEText(L"");

		UpdateNickname();
	}
			
	return false;
}

void UIPopupColorNickname::ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data& UserData)
{
	UIPopupItemBase::ProcessGameEvent(evt, arg, UserData);

	i3::unordered_map< GAME_EVENT, call_func >::iterator it;
	it = m_EventFuncMap.find(evt);

	if(it != m_EventFuncMap.end())
	{
		call_func & mf = it->second;
		(this->*mf)(arg, UserData);
	}
}

bool UIPopupColorNickname::OnOKButton( void)
{
	Commit();
	return true;
}

bool UIPopupColorNickname::OnCloseButton( void)
{
	return true;
}

void UIPopupColorNickname::UpdateNickname()
{
	LuaState * L = 0;
	i3::wstring nickName = UserInfoContext::i()->GetMyNickName();

	I3COLOR selectColor;
	UINT8 colorIndex;
	if(!GetSelectedColor(&colorIndex, &selectColor))
	{
		// 컬러가 선택되지않았다면 현재 NickColor를 사용합니다.
		USER_INFO_INVITEM_DATA invData;
		UserInfoContext::i()->GetMyUserInfoInvItemData(&invData);

		colorIndex = invData._ui8NickColor;

		selectColor = I3COLOR(0xff,0xff,0xff,0xff);
		I3ASSERT(colorIndex < m_colorboxes.size());

		if(colorIndex < m_colorboxes.size())
		{
			m_selectedColorBtn = m_colorboxes[colorIndex];
			GetSelectedColor(&colorIndex, &selectColor);
		}
	}

	ItemConsumer::instance()->m_nSelected = colorIndex;

	i3::wstring colorNickStr = nickName;
	::ColorString(colorNickStr, selectColor.r, selectColor.g, selectColor.b);

	//
	// 유저닉네임 보여주기
	//

	L = _CallLuaFunction("SetTxtColorNick");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, colorNickStr);
		UILua::EndLuaFunction(L, 1);
	}

	//
	// 레드팀/블루팀 Preview
	//

	L = _CallLuaFunction("SetTxtRedTeamBlueTeam");
	if(L != nullptr)
	{
		i3Lua::PushStringUTF16To8(L, colorNickStr);
		i3Lua::PushStringUTF16To8(L, colorNickStr);

		UILua::EndLuaFunction(L, 2);
	}


	// 11941: 닉네임 부분 대괄호에 변경된 색이 적용되지 않는 문제 수정
	
	i3::insert_string(colorNickStr, 0, L"[", i3::generic_string_size("["));
	i3::insert_string(colorNickStr, i3::generic_string_size(colorNickStr), L"]", i3::generic_string_size("]"));
	::ColorString(colorNickStr, selectColor.r, selectColor.g, selectColor.b);
	
	//
	// 채팅창
	//
	{
		L = _CallLuaFunction("SetTxtChatLog");
		if(L != nullptr)
		{	
			i3::wstring finalChatLog;

			if(m_chatLogStrOnly.empty())
				i3::sprintf(finalChatLog, L"%s %s", colorNickStr, GAME_STRING("STR_POPUP_COLORNICK_DEFAULT_CHATLOG"));
			else
				i3::sprintf(finalChatLog, L"%s %s", colorNickStr, m_chatLogStrOnly);

			i3Lua::PushStringUTF16To8(L, finalChatLog);
			UILua::EndLuaFunction(L, 1);
		}
	}	





}

bool UIPopupColorNickname::GetSelectedColor(UINT8* rstColorIndex, I3COLOR* rstColor) const
{
	if(!m_selectedColorBtn)
		return false;

	const char* nameIndex = m_selectedColorBtn->GetName();
	I3ASSERT(nameIndex);

	UINT8 colorIndex = atoi(nameIndex) & 0xff;

	if(rstColorIndex)
		*rstColorIndex = colorIndex;

	if(rstColor)
		*rstColor = *(m_selectedColorBtn->getColor());

	return true;
}

bool UIPopupColorNickname::Commit()
{
	if(!m_selectedColorBtn)
		return false;

	UINT8 colorIndex;
	bool ok = GetSelectedColor(&colorIndex);
	ItemConsumer::instance()->m_nSelected = colorIndex;

	if(ok)
	{		
		UINT8 ui8Size = sizeof(UINT8);
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &colorIndex);
	}

	return true;
}

void UIPopupColorNickname::SyncIMEUI()
{
	if(i3UI::getGlobalIme() == nullptr)
		return;

	INT32 nLangIndex = -1;

#ifdef LOCALE_KOREA
	i3Ime* pIME = i3UI::getGlobalIme();
	if(pIME->GetImeMode() == IME_MODE_NATIVE)
		nLangIndex = 0;
#endif

	LuaState * L = _CallLuaFunction("Language");
	if( L != nullptr)
	{
		i3Lua::PushInteger(L , nLangIndex);
		_EndLuaFunction( L, 1);
	}
}

void UIPopupColorNickname::_CreateSelectionbox(i3UIScene* scene)
{
	i3UITemplate* tmpl = i3UI::getUILibrary()->FindByName("ImageBox_UserColor_Over1");

	VEC3D pos;
	i3Vector::Set(&pos, 0.0f, 0.0f, 0.0f);

	VEC2D size;
	i3Vector::Set(&size, g_selectionbox_width, g_selectionbox_height);

	m_selectionbox = 
		(i3UIImageBoxEx*)i3UI::CreateControl(tmpl, &pos, &size, m_palette);

	m_selectionbox->setInputDisable(true);
	m_selectionbox->SetVisible(false);

	I3_SAFE_ADDREF(m_selectionbox);
}

void UIPopupColorNickname::_CreateColorboxes(i3UIScene* scene)
{
	i3UITemplate* tmpl = i3UI::getUILibrary()->FindByName("Button_Palette");

	const size_t num_row = ColorNickPalette::i()->GetNumOfRow();
	const size_t num_col = ColorNickPalette::i()->GetNumOfCol();

	const REAL32 frame_w = m_palette->getWidth();
	const REAL32 frame_h = m_palette->getHeight();
	
	const REAL32 box_w = g_colorbox_width;
	const REAL32 box_h = g_colorbox_height; 
	
	const REAL32 gap_w = 11.0f;
	const REAL32 gap_h = 5.0f;

	const REAL32 margin_left = (frame_w - ((box_w * num_col) + (gap_w * num_col-1))) / 2;
	const REAL32 margin_top = (frame_h - ((box_h * num_row) + (gap_h * num_row-1))) / 2;

	const REAL32 begin_x = margin_left;
	const REAL32 begin_y = margin_top;

	REAL32 x = 0.0f;
	REAL32 y = begin_y;

	m_colorboxes.reserve(num_row * num_col);
	i3::string nameStr;

	for (size_t row=0; row<num_row; row++)
	{
		x = begin_x;

		for (size_t col=0; col<num_col; col++)
		{
			VEC3D pos;
			i3Vector::Set(&pos, x, y, 0.0f);

			VEC2D size;
			i3Vector::Set(&size, box_w, box_h);

			i3UIButton* colorbox = (i3UIButton*)i3UI::CreateControl(tmpl, &pos, &size, m_palette);

			size_t numCol = ColorNickPalette::i()->GetNumOfCol();

			INT32 index = numCol * row + col;
			I3ASSERT(index >=0 && index < 256);

			i3::sprintf(nameStr, "%d", index);
			colorbox->SetName(nameStr);

			const I3COLOR& color = ColorNickPalette::i()->GetColor(row, col);
			colorbox->setColor(const_cast<I3COLOR*>(&color));

			I3_MUST_ADDREF(colorbox);
			m_colorboxes.push_back(colorbox);

			x += (box_w + gap_w);
		}

		y += (box_h + gap_h);
	}
}

bool UIPopupColorNickname::_IsColorbox(i3UIControl* pControl) const
{
	return i3::same_of<i3UIButton*>(pControl) && (pControl->getParent() == m_palette);
}

void UIPopupColorNickname::RegisterGameEventFunc( GAME_EVENT evt, call_func f)
{
	m_EventFuncMap.insert(i3::unordered_map< GAME_EVENT, call_func >::value_type( evt, f));
}

void UIPopupColorNickname::ReceiveEvent_Item_Auth(INT32 result, const i3::user_data&)
{
	if (EV_SUCCESSED( result))
	{ // 갯수제 아이템 인증
		UIPhaseReadyRoom::OpenPopupUsingCashItem();
	}
	else
	{
		/*네트워크 문제로 아이템 인증에 실패했습니다.\n 다시 시도해 주세요.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_GUI_READY_ITEM_IDENTYFY_FAIL_BY_NETWORK_PROBLEM"));
	}
}

LuaState * UIPopupColorNickname::_SetItemInfo( void)
{
	LuaState* L = UIPopupItemBase::_SetItemInfo();

	return L;
}

