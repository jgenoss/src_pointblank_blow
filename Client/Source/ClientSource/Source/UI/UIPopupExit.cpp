#include "pch.h"
#include "UIPopupExit.h"

#include "UIMainFrame.h"
#include "UIUtil.h"
#include "UIShopFunction.h"
#include "ItemImageMgr.h"
#include "UserInfoContext.h"
#include "ShopContext.h"

#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( UIPopupExit);

LuaFuncReg UIExitPopup_Glue[] = {
	{nullptr,						nullptr},
};

//------------------------------------------------------------------------------//
UIPopupExit::UIPopupExit()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
	
	for(size_t i=0;i<MAX_PRESENT;++i)
	{
		m_ImgBox[i] = nullptr;			// 릭이 있는것 같아 추가..(2013.04.09.수빈. 종료시...OnUnload호출이 안됨)
	}
}

UIPopupExit::~UIPopupExit()
{
	for(size_t i=0;i<MAX_PRESENT;++i)
	{
		I3_SAFE_RELEASE(m_ImgBox[i]);			// 릭이 있는것 같아 추가..(2013.04.09.수빈. 종료시...OnUnload호출이 안됨)
	}
}

/*virtual*/ void UIPopupExit::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PBRe_PopUp_Todayend.i3UIs", UIExitPopup_Glue);
}

/*virtual*/ bool UIPopupExit::OnEntranceStart( void * pArg1, void * pArg2)
{
	 if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	SetLabel();
	SetRank();
	SetPresent();

	GetSlide()->ForceEnd();

	return true;
}
 
/*virtual*/ void UIPopupExit::OnLoadAllScenes()
{
	UIPopupBase::OnLoadAllScenes();

	m_ImgBox[0] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet");
	m_ImgBox[1] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet0");
	m_ImgBox[2] = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet1");
	m_ImgBox[3] =  GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet2");
}

/*virtual*/ void UIPopupExit::OnUnloadAllScenes()
{
	UIPopupBase::OnUnloadAllScenes();

	for(size_t i=0;i<MAX_PRESENT;++i)
	{
		m_ImgBox[i]->RestoreSizePos();
		I3_SAFE_RELEASE(m_ImgBox[i]);
	}
}

bool UIPopupExit::OnOKButton( void)
{
	GameEventSender::i()->SetEvent( EVENT_GAME_END ); // 준영. 로그아웃 요청 패킷 전송
	g_pFramework->GotoNextStage("Exit");
	return true;
}

/*virtual*/ bool UIPopupExit::OnPlayClickSound(i3UIControl * pCtrl)
{
	if( i3::same_of<i3UIButtonComposed3*>(pCtrl) )
	{
		if(i3::generic_is_iequal(pCtrl->GetName(), "i3UIButtonOk") )
		{
			// 확인 버튼
			return PlaySound(UISND_ITEM_CLICK_OK);
		}
		else if(i3::generic_is_iequal(pCtrl->GetName(), "i3UIButtonCancel") )
		{
			// 취소 버튼
			return PlaySound(UISND_ITEM_CLICK_CANCEL);
		}
	}

	return UIPopupBase::OnPlayClickSound(pCtrl);
}


//------------------------------------------------------------------------------//
void UIPopupExit::SetLabel()
{
	USER_INFO_BASIC MyUserInfoBasic;
	UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

	i3::stack_wstring wstr_title;		// 강제 형변환 과정 존재함..(2014.07.14.수빈)
	i3::sprintf(wstr_title, GAME_RCSTRING("STR_TBL_UI_EXIT_TODAY_HEAD"), MyUserInfoBasic.m_strNick ); /*[{col:255, 255, 0, 255}%s{/col}]님의 오늘의 전적*/

	const USER_INFO_DAILY * pInfo = UserInfoContext::i()->GetUserInfoDaily(); 

	i3::stack_wstring wstr_score;
	i3::sprintf( wstr_score, GAME_RCSTRING("STR_TBL_UI_EXIT_TODAY_RECORD_VALUE"), pInfo->_match, pInfo->_win, pInfo->_draw, pInfo->_lose); /*%d판 %d승 %무 %d패*/

	INT32 kill_death_rate;
	if(pInfo->_death == 0 && pInfo->_killcount == 0) 
		kill_death_rate = 0;
	else
		kill_death_rate = (INT32)((pInfo->_killcount / (REAL32)(pInfo->_killcount + pInfo->_death) * 100.0f) + 0.5);

	i3::stack_wstring wstr_kd_rate;
	i3::sprintf( wstr_kd_rate, GAME_RCSTRING("STR_TBL_UI_EXIT_TODAY_KILLDEATH"), pInfo->_killcount, pInfo->_death, kill_death_rate ); /*%d Kill %d Death %d%%*/

	LuaState * L = _CallLuaFunction("SetLabel");
	i3Lua::PushStringUTF16To8(L, wstr_title);
	i3Lua::PushStringUTF16To8(L, wstr_score);
	i3Lua::PushStringUTF16To8(L, wstr_kd_rate);

	
	i3::stack_wstring wstr_stack;	
	
	i3::itoa(pInfo->_headshot, wstr_stack);
	i3Lua::PushStringUTF16To8(L, wstr_stack );		

	i3::itoa(pInfo->_exp, wstr_stack);
	i3Lua::PushStringUTF16To8(L, wstr_stack );

	i3::itoa(pInfo->_point, wstr_stack);
	i3Lua::PushStringUTF16To8(L, wstr_stack );

	i3::itoa(pInfo->_playtime, wstr_stack);
	i3Lua::PushStringUTF16To8(L, wstr_stack );
	_EndLuaFunction(L, 7);
}

void UIPopupExit::SetRank()
{
	INT32 curr_rank = UserInfoContext::i()->GetRealRank();

	//장성급, 영웅,GM은 경험치로 계산을 못 함
	//inner::set_gain_point 함수와 내용이 같음
	if( GameUI::is_over_hero_rank(curr_rank) == true || GameUI::is_hero_rank(curr_rank, true) == true) //장성급 & 영웅 & GM 계급일 경우
	{
		LuaState * L = _CallLuaFunction("SetRank");
		i3Lua::PushInteger(L, curr_rank);
		i3Lua::PushStringUTF16To8(L, GetCharaRank(curr_rank));

		i3Lua::PushInteger(L, -1); 
		i3Lua::PushStringUTF16To8(L, L"");
		i3Lua::PushInteger(L, 100);
		i3Lua::PushStringUTF16To8(L, L"100%");
		_EndLuaFunction(L, 6);
	}
	else
	{
		USER_INFO_BASIC MyUserInfoBasic;
		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);

		INT32 nCurrentExp = MAX(MyUserInfoBasic.m_ui32Exp - GameUI::GetMaxExp(curr_rank), 0); //음수가 나올 수 있다.(웹툴에서 계급만 수정할 경우, 해당 계급에 exp가 정상적이지 않음)
		//INT32 nMaxExp = GameUI::GetMaxExp(curr_rank + 1) - GameUI::GetMaxExp(curr_rank);
		//if( nMaxExp == 0)
		//	nMaxExp = 1;
		INT32 nExpRate = GameUI::GetExpRate(curr_rank, nCurrentExp);

		//nExpRate = MINMAX(0, nExpRate, 100);

		i3::stack_wstring wstrRateText;
		i3::sprintf(wstrRateText, L"%d%%", nExpRate);

		LuaState * L = _CallLuaFunction("SetRank");
		i3Lua::PushInteger(L, curr_rank);
		i3Lua::PushStringUTF16To8(L, GetCharaRank(curr_rank));
		i3Lua::PushInteger(L, curr_rank+1);
		i3Lua::PushStringUTF16To8(L, GetCharaRank(curr_rank+1));
		i3Lua::PushInteger(L, nExpRate);
		i3Lua::PushStringUTF16To8(L, wstrRateText);
		_EndLuaFunction(L, 6);
	}
}

void UIPopupExit::SetPresent()
{
	INT32 wRank = UserInfoContext::i()->GetRealRank();
	const RANKUP_ITEM * pData = UserInfoContext::i()->GetUserRankUpItemList(wRank);

	LuaState * L = _CallLuaFunction("SetPresent");
	for(INT32 i=0; i < MAX_PRESENT; i++)
	{
		if(pData != nullptr && pData->m_ui32GoodsID[i] != 0)
		{
			T_ItemID itemid =  CShopContext::i()->ConvertGoodsID(pData->m_ui32GoodsID[i]);
			GetItemImageMgr()->SelectImage(m_ImgBox[i], itemid );

			i3::rc_wstring wstr;
			usf::GetShopItemName(wstr, itemid);
			i3Lua::PushStringUTF16To8(L, wstr.c_str(), wstr.size());
		}
		else
		{
			m_ImgBox[i]->SetEnable(false);
			i3Lua::PushStringUTF16To8(L, L"--------");
		}
	}

	_EndLuaFunction(L, MAX_PRESENT);
}