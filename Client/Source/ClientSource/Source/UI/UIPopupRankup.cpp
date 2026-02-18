#include "pch.h"
#include "UIPopupRankup.h"

#include "UIMainFrame.h"
#include "ItemImageMgr.h"
#include "UIShopFunction.h"
#include "UserInfoContext.h"
#include "ShopContext.h"
#include "UIUtil.h"

#include "i3Base/string/ext/mb_to_utf16.h"

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupRankup_Confirm( LuaState * L)
	{
		UIPopupRankup * pThis = static_cast<UIPopupRankup*>(GameUI::GetPopup( UPW_RANK_UP));
		if( pThis != nullptr)
			pThis->Confirm();
		return 0;
	}
}

LuaFuncReg UIPopupRankup_Glue[] =
{
	{"OK",			UIPopupRankup_Confirm	},
	{nullptr,				nullptr					}
};


I3_CLASS_INSTANCE( UIPopupRankup);//, UIPopupBase);

UIPopupRankup::UIPopupRankup()
{
	rank_ctrl = 0;
	m_iItemCount = 0;
}

UIPopupRankup::~UIPopupRankup()
{
}

void UIPopupRankup::Confirm()
{
	GetMainFrame()->ClosePopup(UPW_RANK_UP);
}

void	UIPopupRankup::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad( pScene);

	rank_ctrl = GetItemImageMgr()->CreateImgSetCtrl(pScene, "i3UIButtonImageSet0");
	gain_skill_buf_ctrl = (i3UIEditBox*) pScene->FindChildByName( "i3UIEditBox" );

	i3::stack_string wstr_frm, wstr_ctrl, wstr_text;

	for(size_t i=0;i<MAX_RANKUP_ITEM_COUNT; ++i)
	{
		i3::sprintf(wstr_frm, "Frm_Item%d", i+1);
		i3::sprintf(wstr_ctrl, "i3UIButtonImageSet%d", i+1);
		i3::sprintf(wstr_text, "i3UIStaticText%d", i+1);

		m_ItemContext[i].frm = (i3UIFrameWnd*) pScene->FindChildByName( wstr_frm.c_str() );
		m_ItemContext[i].ctrl = GetItemImageMgr()->CreateImgSetCtrl(pScene, wstr_ctrl.c_str());
		m_ItemContext[i].text = (i3UIStaticText*) pScene->FindChildByName( wstr_text.c_str());
	}
}

void	UIPopupRankup::_ClearAtUnload( i3UIScene * pScene)
{
	UIPopupBase::_ClearAtUnload( pScene);

	I3_SAFE_RELEASE(rank_ctrl);

	for(size_t i=0;i<MAX_RANKUP_ITEM_COUNT; ++i)
	{
		I3_SAFE_RELEASE(m_ItemContext[i].ctrl);
	}
}

/*virtual*/ void UIPopupRankup::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PBRe_PopUp_LVUp_new.i3UIs", UIPopupRankup_Glue);
}

/*virtual*/ bool UIPopupRankup::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	Set_UI();

	return true;
}


/*virtual*/ bool UIPopupRankup::OnExitStart( void)
{
	if( rank_ctrl == 0 ) return UIPopupBase::OnExitStart();

	rank_ctrl->RestoreSizePos();

	for(size_t i=0;i<MAX_RANKUP_ITEM_COUNT; ++i)
	{
		m_ItemContext[i].ctrl->RestoreSizePos();
	}


	if (m_iItemCount > 0)
	{
		if (UserInfoContext::i()->IsContentsEndabled(CONTENTS_GIFT))
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS"));
		else
			GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_UI_ATTENDANCE_SUCCESS2"));
	}

	return UIPopupBase::OnExitStart();
}


namespace inner
{
	INT32 get_position(UINT32 idx, UINT32 max_count)
	{
		//하드 코딩함..
		switch( max_count )
		{
		case 1: return 112; break;
		case 3:
			switch(idx)
			{
			case 0: return -112; break;
			case 1: return 112; break;
			case 2: return 339; break;
			}
			break;
		case 2: case 4:
			switch(idx)
			{
			case 0: return -216; break;
			case 1: return 0; break;
			case 2: return 219; break;
			case 3: return 439; break;
			}
			break;
		}

		return 0;
	}
}

void UIPopupRankup::Set_UI()
{
	// 닉네임
	//char szMyNick[NET_NICK_NAME_SIZE] = {0};

	i3::rc_wstring wstrMyNick = UserInfoContext::i()->GetMyNickName();
	
	// 진급 포인트 정보
	UINT32 rank = UserInfoContext::i()->GetRankUpRank();
	if (rank < 1) rank = 1;
	
	char szRank[MAX_STRING_COUNT];
	i3::snprintf(szRank, MAX_STRING_COUNT, "STBL_IDX_RANK_%d", rank);

	const i3::rc_wstring& wstr_curr_rank = GAME_RCSTRING(szRank);

	LuaState * L = _CallLuaFunction("SetLabel1");
	i3Lua::PushStringUTF16To8(L, wstr_curr_rank.c_str(), wstr_curr_rank.size());
	_EndLuaFunction(L,1);

	// 채팅창
	i3::rc_wstring wstr_rankUpChattingText;
	i3::sprintf(wstr_rankUpChattingText, 
		GAME_RCSTRING("STR_TBL_GUI_READY_NOTICE_PROMOTION1"), wstrMyNick, wstr_curr_rank);/*{col:255,120,0,255}%s님이 %s으로 진급하셨습니다. 축하 드립니다.!!{/col}*/
	g_pFramework->getChatBox()->PutSystemChatting(wstr_rankUpChattingText);

	if(g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		//스킬 혜택
		i3::snprintf(szRank, MAX_STRING_COUNT, "STR_UI_LVUP_POPUP_%d", rank);
		i3::wstring wstrRank;		i3::mb_to_utf16(szRank, wstrRank);

		const i3::rc_wstring& wstr_skill = GAME_RCSTRING(szRank);

		if( i3::generic_is_equal(wstrRank, wstr_skill) == false ) //스킬 혜택 내용이 없는것도 있다.
		{
			gain_skill_buf_ctrl->SetText(wstr_skill);
		}
		else
		{
			gain_skill_buf_ctrl->EnableCtrl(false);
		}
	}
	else
	{
		gain_skill_buf_ctrl->EnableCtrl(false);
	}

	//획득 아이템
	m_iItemCount = 0;
	const RANKUP_ITEM * pData1 = UserInfoContext::i()->GetUserRankUpItemList(rank-1);
	if(pData1 != nullptr)
	{
		for(UINT32 i=0; i<MAX_RANKUP_ITEM_COUNT; i++)
		{
			m_ItemContext[i].frm->EnableCtrl(false);

			if(pData1->m_ui32GoodsID[i] != 0)
			{
				UINT32 pitemid =  CShopContext::i()->ConvertGoodsID(pData1->m_ui32GoodsID[i]);
				GetItemImageMgr()->SelectImage(m_ItemContext[i].ctrl, pitemid);

				i3::rc_wstring wstrItemName;
				usf::GetShopItemName(wstrItemName, pitemid);
				m_ItemContext[i].text->SetText(wstrItemName);

				m_iItemCount++;	//현재 지급받을 무기 카운트
			}
		}
	}

	//아이템 자리 이동
	for(UINT32 i=0; i<m_iItemCount; i++)
	{
		m_ItemContext[i].frm->EnableCtrl(true);

		VEC3D* pos = m_ItemContext[i].frm->getPos();
		REAL32 x = (REAL32)inner::get_position(i, m_iItemCount);
		m_ItemContext[i].frm->setPos(x, pos->y);
	}

	i3::wstring wstr_next_item_str;
	const RANKUP_ITEM * pData2 = UserInfoContext::i()->GetUserRankUpItemList(rank);
	if(pData2 != nullptr)
	{
		i3::rc_wstring wstrItemName;

		for(INT32 i=0; i<MAX_RANKUP_ITEM_COUNT; i++)
		{
			if(pData2->m_ui32GoodsID[i] != 0)
			{
				UINT32 pitemid =  CShopContext::i()->ConvertGoodsID(pData2->m_ui32GoodsID[i]);

				usf::GetShopItemName(wstrItemName, pitemid);
				wstr_next_item_str.append(wstrItemName.begin(), wstrItemName.end());
				wstr_next_item_str.append(L", ");
			}
		}
		if( wstr_next_item_str.empty() == false )
			wstr_next_item_str.erase( wstr_next_item_str.size() - 2 ); //맨뒤 "," 제거
	}

	//다음 계급

	i3::snprintf(szRank, MAX_STRING_COUNT, "STBL_IDX_RANK_%d", rank+1);

	i3::stack_wstring wstr_next_rank;
	i3::sprintf( wstr_next_rank, GAME_RCSTRING("STR_TBL_UI_RANKUP_NEXT"), GAME_RCSTRING(szRank) );

	L = _CallLuaFunction("SetLabel3");
	i3Lua::PushStringUTF16To8(L, wstr_next_rank.c_str(), wstr_next_rank.size());
	i3Lua::PushStringUTF16To8(L, wstr_next_item_str.c_str(), wstr_next_item_str.size() );
	_EndLuaFunction(L,2);

	//계급
	rank_ctrl->SetImage(rank);
}