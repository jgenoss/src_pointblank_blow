#include "pch.h"
#include "UIPopupMissionCardShop_V10.h"
#include "../CommunityContext.h"
#include "UI/UIMainFrame_V10.h"

#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"

#include "UserInfoContext.h"


I3_CLASS_INSTANCE( UIPopupMissionCardShop_V10);

static UIPopupMissionCardShop_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupMissionCardShop_V10*) pFrame->GetPopup( UPW_MISSIONSHOP );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupMissionCardShop_V10_BuyOK(LuaState * L)
	{
		GetThis()->BuyCardOK();
		return 0;
	}

	int UIPopupMissionCardShop_V10_BuyCancel(LuaState * L)
	{
		GetThis()->BuyCardCancel();
		return 0;
	}
}

LuaFuncReg UIPopupMissionCardShop_V10_Glue[] = {
	{"BuyCardOK",		UIPopupMissionCardShop_V10_BuyOK		},
	{"BuyCardCancel",	UIPopupMissionCardShop_V10_BuyCancel	},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UIPopupMissionCardShop_V10::UIPopupMissionCardShop_V10()
{
	m_CardSetType = 0;
}

UIPopupMissionCardShop_V10::~UIPopupMissionCardShop_V10()
{
}

void UIPopupMissionCardShop_V10::BuyCardOK()
{
	UINT8 cardSetID = (UINT8)m_CardSetType;
	GameEventSender::i()->SetEvent(EVENT_QUEST_BUY_CARD_SET, &cardSetID);
	GetMainFrame()->TogglePopup( UPW_MISSIONSHOP);
}

void UIPopupMissionCardShop_V10::BuyCardCancel()
{
	GetMainFrame()->TogglePopup( UPW_MISSIONSHOP);
}

void UIPopupMissionCardShop_V10::OnCardBuyAll( INT32 cardSetType)
{
	// 선택카드 출력
	_SetCardImage((ENUM_QUEST_CARDSET_TYPE)cardSetType);

	// 카드 이름
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(cardSetType);

	i3::rc_wstring wstrName;

	if (pSetInfo) wstrName = pSetInfo->wstrName;
	
	_SetCardTitle(wstrName);

	// 가격
//	UINT32 cardPricePoint = g_QuestCardSetBuyPoint[cardSetId];			// 어디있는지 몰라서 일단 출력은 나중으로 미룬다..
	UINT32 myCurrentPoint = UserInfoContext::i()->GetMyPoint();
	i3::stack_wstring wstrText;

	// 카드가격

	UINT32 goodsID = (pSetInfo) ? pSetInfo->goodsID : 0;
	GOODS_PRICE_INFO price_info;
	::memset(&price_info, 0, sizeof(price_info));
	CShop::i()->GetGoodsPrice(goodsID, price_info, PRICE_TYPE_SHOP_REAL);		// 그냥 실판매가격이면 충분함..

	i3::sprintf( wstrText, L"%u %s", price_info._Point, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
	_SetCardPrice(wstrText);

	// 소유금액
	i3::sprintf( wstrText, L"%u %s", myCurrentPoint, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
	_SetCardMymoney(wstrText);

	// 지불후 예상 차액
	INT32 remainPoint = (signed)myCurrentPoint - (signed)price_info._Point;		// 원래 이것이지만..가격을 현재는 알수없어 아랫것으로 테스트.
	
	if (0 <= remainPoint)
	{
		i3::sprintf( wstrText, L"{col:151, 211, 50, 255}%d{/col} %s", remainPoint, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
		{
			_SetCardConfirm(false);
		}
	}
	else
	{
		i3::sprintf( wstrText, L"{col:255, 48, 0, 255}%d{/col} %s", remainPoint, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
		{
			_SetCardConfirm(true);
		}
	}

	_SetCardMybalance(wstrText);
}

/*virtual*/ void UIPopupMissionCardShop_V10::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PointBlankRe_MyInfo_MissionCard_BuyNow.i3UIs",UIPopupMissionCardShop_V10_Glue);
	// Load Scene
}

/*virtual*/ bool UIPopupMissionCardShop_V10::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart() == false)
		return false;

	m_CardSetType = *static_cast<INT32*>(pArg1);
	OnCardBuyAll(m_CardSetType);
	SetEnable(true);

	return true;
}

void UIPopupMissionCardShop_V10::_SetCardImage(ENUM_QUEST_CARDSET_TYPE Idx)
{
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(Idx);

	INT32 imageIndex = ( pSetInfo ) ? pSetInfo->UITemplateIndex : -1;
	
	LuaState* L = _CallLuaFunction("SetCardImage");
	i3Lua::PushInteger(L, imageIndex);
	_EndLuaFunction(L,1);
}

void UIPopupMissionCardShop_V10::_SetCardTitle(const i3::wliteral_range& wstrText)
{
	LuaState* L = _CallLuaFunction("SetCardTitle");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}

void UIPopupMissionCardShop_V10::_SetCardPrice(const i3::wliteral_range& wstrText)
{
	LuaState* L = _CallLuaFunction("SetCardPrice");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}
void UIPopupMissionCardShop_V10::_SetCardMymoney(const i3::wliteral_range& wstrText)
{
	LuaState* L = _CallLuaFunction("SetCardMymoney");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}
void UIPopupMissionCardShop_V10::_SetCardMybalance(const i3::wliteral_range& wstrText)
{
	LuaState* L = _CallLuaFunction("SetCardMybalance");
	i3Lua::PushStringUTF16To8(L, wstrText);
	_EndLuaFunction(L, 1);
}

void UIPopupMissionCardShop_V10::_SetCardConfirm(bool Sig)
{
	LuaState* L = _CallLuaFunction("SetCardConfirm");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L,1);
}