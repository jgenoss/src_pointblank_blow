#include "pch.h"
#include "UIPopupQuickJoin.h"
#include "QJSlot.h"
#include "../UIMainFrame.h"
#include "../QuickJoinContext.h"
#include "../UIPhaseLobby.h"
#include "../UIPhaseSelectServer.h"
#include "../UIUtil.h"

extern "C" 
{
}

LuaFuncReg QuickJoin_Glue[] =
{
	{ nullptr, nullptr }
};

I3_CLASS_INSTANCE(UIPopupQuickJoin);

UIPopupQuickJoin::UIPopupQuickJoin() : m_slot(new Qjoin::Slot(this))
{
}

UIPopupQuickJoin::~UIPopupQuickJoin() 
{
	I3_SAFE_DELETE( m_slot);
}

void UIPopupQuickJoin::OnCreate(i3GameNode* parent)
{
	UIPopupBase::OnCreate(parent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_QuickEnter.i3UIs", QuickJoin_Glue, true, false);

	m_slot->OnCreate(parent);
}

void UIPopupQuickJoin::_InitializeAtLoad(i3UIScene* scn)
{
	UIPopupBase::_InitializeAtLoad(scn);
}

void UIPopupQuickJoin::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_slot->OnLoadAllScenes();
}

void UIPopupQuickJoin::OnUpdate(REAL32 tm)
{
	UIPopupBase::OnUpdate(tm);

	m_slot->OnUpdate(tm);
}

void UIPopupQuickJoin::OnEntranceEnd()
{
	UIPopupBase::OnEntranceEnd();

	m_slot->RefreshAllStartButton();
}

void UIPopupQuickJoin::GetLeftTopPos(INT32& outLeft, INT32& outTop)
{
	i3UIFrameWnd* main_frame = (i3UIFrameWnd*)GetScene(0)->FindChildByName("frame_main");
	VEC3D* pos = main_frame->getAccumulatedPos();
	outLeft = (INT32)pos->x; outTop = (INT32)pos->y;
}

void UIPopupQuickJoin::ProcessGameEvent(GAME_EVENT event, INT32 Arg, const i3::user_data& UserData)
{
	UIPopupBase::ProcessGameEvent(event, Arg, UserData);

	UIMainFrame* frame = g_pFramework->GetUIMainframe();
	if (frame == 0) return;

	switch (event)
	{
	case EVENT_QUICKJOIN_START:
		{
			QuickJoinContext* ctx = QuickJoinContext::i();

			if (EVENT_ERROR_SUCCESS == ctx->getResult()) // 방 검색 성공, 해당 방으로 이동한다.
			{
				// PROTOCOL_ROOM_JOIN_ACK 에서 처리함.
			}
			else if (EVENT_ERROR_QUICK_JOIN_2ND_BEST == ctx->getResult()) // 차선책 팝업을 띄운다.
			{
				if (frame->IsOpenedPopup(UPW_QUICK_JOIN_SECOND_BEST) == false)
				{
					frame->OpenPopup(UPW_QUICK_JOIN_SECOND_BEST);
				}
			}
			else
			{
				GameUI::MsgBox(MSG_TYPE_ERROR, GAME_RCSTRING("STBL_IDX_EP_LOBBY_NOT_EXIST_ROOM"));
			}
		}
		break;

	default:
		break;
	}
}

bool UIPopupQuickJoin::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code )
{
	switch (event)
	{
	case I3_EVT_INPUT:
		{
			i3InputDeviceManager* mgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* keyboard = mgr->GetKeyboard();

			m_slot->OnKeyInput(keyboard);
		}
		break;

	default: break;
	}

	return UIPopupBase::OnEvent(event, pObj, param2, code);
}

bool UIPopupQuickJoin::OnCloseButton()
{
	UIPopupBase::OnCloseButton();
	
	UIPhaseLobby * pLobby = static_cast<UIPhaseLobby*>(GetMainFrame()->GetPhase(UIPHASE_LOBBY));
	if( pLobby != nullptr )
		pLobby->EnableAllowRefresh(true);

	return true;
}