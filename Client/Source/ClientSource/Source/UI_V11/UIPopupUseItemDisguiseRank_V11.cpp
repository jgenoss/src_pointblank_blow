#include "pch.h"
#include "UI_V11/UIPopupUseItemDisguiseRank_V11.h"
#include "UI_V11/UIFloatCharacterView_V11.h"

#include "UI/UIMainFrame_V11.h"
#include "UI/UIUtil.h"
#include "UI/ItemConsumer.h"

#include "UserInfoContext.h"

I3_CLASS_INSTANCE( UIPopupUseItemDisguiseRank_V11);//, UIPopupUseItemBase);

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//

static UIPopupUseItemDisguiseRank_V11* GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupUseItemDisguiseRank_V11*) pFrame->GetPopup( UPW_DISGUISE_RANK);
}

extern "C" { 

	int UIPopup_DisguiseRank_SelectRank_V11( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->SelectRank(idx);
		return 0;
	}

	int UIPopup_DisguiseRank_Confirm_V11( LuaState * L)
	{
		GetThis()->OnOKButton();
		return 0;
	}

	int UIPopup_DisguiseRank_Close_V11( LuaState * L)
	{
		GetThis()->ClosePopup();
		return 0;
	}
}

LuaFuncReg PBRe_PopUp_FakeLevel_V11_Glue[] =
{
	{"Confirm",			UIPopup_DisguiseRank_Confirm_V11 },
	{"ClosePopup",		UIPopup_DisguiseRank_Close_V11 },
	{"SelectRank",		UIPopup_DisguiseRank_SelectRank_V11 },
	{ NULL,						NULL }
};

UIPopupUseItemDisguiseRank_V11::UIPopupUseItemDisguiseRank_V11()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupUseItemDisguiseRank_V11::~UIPopupUseItemDisguiseRank_V11()
{
}

/*virtual*/ void UIPopupUseItemDisguiseRank_V11::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupUseItemBase::_InitializeAtLoad( pScene);
}

/*virtual*/ void UIPopupUseItemDisguiseRank_V11::OnEntranceEnd( void)
{
	UIPopupUseItemBase::OnEntranceEnd();

	Init();

	// LoadScene이 2번 호출되서... 일단 강제로 다 꺼줌
	LuaState * L = _CallLuaFunction("AllRankVisible");
	if (L != NULL)
	{
		_EndLuaFunction(L, 0);
	}

	if (GET_ITEM_NUMBER(m_ItemID) == CASHITEM_GROUP_DISGUISE_RANK_APRIL_FOOL)
		SetAbleToSelectRank(m_CurRank, MAX_DISGUISE_RANK_COUNT);
	else
		SetAbleToSelectRank(m_CurRank, BOUNDS_DISGUISE_RANK);
}

/*virtual*/ bool UIPopupUseItemDisguiseRank_V11::OnExitStart( void)
{
	UIPopupBase::OnExitStart();

	m_SelectedDisguiseRank = -1;

	return true;
}

void UIPopupUseItemDisguiseRank_V11::Init()
{
	m_CurRank = UserInfoContext::i()->GetMyRank();

	VEC2D pPos;
	i3Vector::Zero(&pPos);

	GetRankPos(m_CurRank, &pPos);

	char szTemp[256];
	i3::rc_wstring wstrCurRank;
	i3::rc_wstring wstrDisguiseRank;

	GetRankName(m_CurRank, szTemp);

	// 현재 계급 스트링
	i3::sprintf(wstrCurRank, L"%s%s", GAME_STRING("STR_POPUP_CURRENT_RANK"), GAME_STRING(szTemp));
	// 위장 계급 스트링
	i3::sprintf(wstrDisguiseRank, L"%s%s", GAME_STRING("STR_POPUP_DISGUISE_RANK"), GAME_STRING(szTemp));

	LuaState * L = _CallLuaFunction("InitRankState");
	if (L  != NULL)
	{
		i3Lua::PushStringUTF16To8(L, wstrCurRank);
		i3Lua::PushInteger(L, (INT32)pPos.x);
		i3Lua::PushInteger(L, (INT32)pPos.y);
		i3Lua::PushStringUTF16To8(L, wstrDisguiseRank);
	}
	_EndLuaFunction(L, 4);
}

/*virtual*/ LuaState * UIPopupUseItemDisguiseRank_V11::_SetItemInfo( void)
{
	LuaState * L = UIPopupUseItemBase::_SetItemInfo();

	i3::rc_wstring wstrItemExplanation;

	g_pShopItemInfoDataBase->GetItemDescription( m_ItemID, wstrItemExplanation);

	if( L != NULL)
	{
		// 현재 계급
		i3Lua::PushStringUTF16To8( L, wstrItemExplanation);
		_EndLuaFunction( L, 2);
	}

	return L;
}

/*virtual*/ void UIPopupUseItemDisguiseRank_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupUseItemBase::OnCreate( pParent);

	AddScene( "UIRe1/Scene/Popup/PBRe_PopUp_FakeLevel.i3UIs", PBRe_PopUp_FakeLevel_V11_Glue);
}

/*virtual*/ bool UIPopupUseItemDisguiseRank_V11::OnOKButton( void)
{
	INT32 select = 55;
	INT32 Rank = UserInfoContext::i()->GetMyRank();

	if(Rank == m_SelectedDisguiseRank)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_DISGUISE_NAME"));
		return false;
	}

	if (m_SelectedDisguiseRank == -1)
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_FAKERANK_NON_CHECK_POPUP"));
		return false;
	}

	select = m_SelectedDisguiseRank;
	m_SelectedDisguiseRank = -1;

	SetInputDisable( true);

	//	위장 계급을 보낸다.
	UINT8 ui8Size = sizeof(INT32);
	GameEventSender::i()->SetEvent( EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &select);

	return true;
}

void UIPopupUseItemDisguiseRank_V11::SelectRank( INT32 idx)
{
	INT32 Rank = UserInfoContext::i()->GetMyRank();

	if (Rank == idx)
		return;

	I3TRACE("Select Rank Index : %d\n",idx);

	m_SelectedDisguiseRank = idx;

	VEC2D pPos;
	i3Vector::Zero(&pPos);

	GetRankPos(m_SelectedDisguiseRank, &pPos);

	char szTemp[256];

	GetRankName(m_SelectedDisguiseRank, szTemp);

	// 위장 계급 스트링
	i3::rc_wstring wstr;
	i3::sprintf(wstr, L"%s%s", GAME_STRING("STR_POPUP_DISGUISE_RANK"), GAME_STRING(szTemp));

	LuaState * L = _CallLuaFunction("SetSelectedRank");
	if (L != NULL)
	{
		i3Lua::PushStringUTF16To8(L, wstr);
		i3Lua::PushInteger(L, (INT32)pPos.x);
		i3Lua::PushInteger(L, (INT32)pPos.y);
	}
	_EndLuaFunction(L, 3);
}

void UIPopupUseItemDisguiseRank_V11::GetRankPos(INT32 rankIdx, VEC2D* pPos)
{
	// 랭크 인덱스가 0부터인지 확인 필요 //

	// 랭크의 인덱스로 이미지를 셋팅할 좌표를 구한다

	INT32 column = rankIdx % 13;
	INT32 row = rankIdx / 13;

	REAL32 xPos = (REAL32)(column * 24);
	REAL32 yPos = (REAL32)(row * 24);

	xPos += 25;
	yPos += 13;

	i3Vector::Set(pPos, xPos, yPos);
}

void UIPopupUseItemDisguiseRank_V11::GetRankName(INT32 idx, char* name)
{
	// 랭크의 이름을 얻어옴
	_snprintf(name, 256, "STBL_IDX_RANK_%d", idx);
}

void UIPopupUseItemDisguiseRank_V11::ClosePopup()
{
	m_SelectedDisguiseRank = -1;
	GetMainFrame()->ClosePopup(UPW_DISGUISE_RANK);
}

void UIPopupUseItemDisguiseRank_V11::SetAbleToSelectRank(INT32 idx, INT32 bounds)
{
	// 훈련병일 경우 시작이 정확한지 확인

	INT32 begin = idx - bounds;
	INT32 end = idx + bounds;

	if (begin < 0)
		begin = 0;

	if (end > MAX_DISGUISE_RANK_COUNT)
		end = MAX_DISGUISE_RANK_COUNT + 1;

	LuaState * L = _CallLuaFunction("AbleToSelectRank");
	if (L  != NULL)
	{
		i3Lua::PushInteger(L, begin);
		i3Lua::PushInteger(L, end);
	}
	_EndLuaFunction(L, 2);
}