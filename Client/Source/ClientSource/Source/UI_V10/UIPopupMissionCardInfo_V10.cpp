#include "pch.h"
#include "UIPopupMissionCardInfo_V10.h"
#include "../CommunityContext.h"
#include "UI/UIMainFrame_V11.h"

#include "UI/UIShopFunction.h"
#include "UI/ItemImageMgr.h"
#include "UI/UIMath.h"

#include "../ShopContext.h"

#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"


I3_CLASS_INSTANCE( UIPopupMissionCardInfo_V10);

static UIPopupMissionCardInfo_V10 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMissionCardInfo_V10*) pFrame->GetPopup( UPW_MISSIONINFO );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupMissionCardInfo_V10_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg UIPopupMissionCardInfo_V10_Glue[] = {
	{"Close",					UIPopupMissionCardInfo_V10_Confirm},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//




UIPopupMissionCardInfo_V10::UIPopupMissionCardInfo_V10() : m_CardSetType(QUEST_CARDSET_TYPE_NONE)
{
	m_ItemSetCtrl = 0;
}

UIPopupMissionCardInfo_V10::~UIPopupMissionCardInfo_V10()
{
}

/*virtual*/ void UIPopupMissionCardInfo_V10::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene("Scene/Popup/PointBlankRe_MyInfo_MissionCard_Detail.i3UIs", UIPopupMissionCardInfo_V10_Glue);

	//AddScene("Scene/Popup/PointBlankRe_MyInfo_MissionCard_Detail.i3UIs",	UIPopupMissionCardInfo_V10_Glue);
}


/*virtual*/ bool UIPopupMissionCardInfo_V10::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//상품 정보를 수집한다.
	CShopContext* pShopContext = CShopContext::i();

	pShopContext->BuildGoodsList();

	//i3UIScene * pScene = UIBase::GetScene("PointBlankRe_MyInfo_MissionCard_Detail");

	//LuaState* L = nullptr;
	i3UIButtonImageSet* btnset = nullptr;
	m_CardSetType = *static_cast<INT32*>(pArg1);
	
	// Card가 이벤트 일 경우
	if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
	{
		m_EventFrame->EnableCtrl(true);
		//L = _CallLuaFunction("GetEventImageSetCtrl"); //UILua::CallLuaFunction(pScene, "GetEventImageSetCtrl");
		btnset = (i3UIButtonImageSet*)GetScene(0)->FindChildByName("EventItemIcon");
	}
	else
	{
		m_DefaultFrame->EnableCtrl(true);
		//L = _CallLuaFunction("GetDefaultImageSetCtrl"); //UILua::CallLuaFunction(pScene, "GetDefaultImageSetCtrl");
		btnset = (i3UIButtonImageSet*)GetScene(0)->FindChildByName("DefaultItemIcon");
	}
	
	m_ItemSetCtrl = GetItemImageMgr()->CreateImgSetCtrl(btnset);
	
	//SetEnable(true);
	OnCardInfo();

	return true;
}

void	UIPopupMissionCardInfo_V10::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_ItemSetCtrl);
}

void UIPopupMissionCardInfo_V10::Confirm()
{
	GetMainFrame()->TogglePopup( UPW_MISSIONINFO);
}

void UIPopupMissionCardInfo_V10::_SetCardItem()
{

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	INT32 itemID = 0;
	UINT8 authType = 0;
	UINT32 authArg = 0;

	if (pSetInfo)
	{
		const i3::svector<MCardPrizeInfo_Mqf_ItemInfo>& vecPrizeItem = pSetInfo->prizeCompletion.vecPrizeItem;

		const INT32 numBonusItem = INT32(vecPrizeItem.size() );

		if ( numBonusItem > 0 )
		{
			itemID = MCardUtil::ConvertItemID_V10_TO_V20(vecPrizeItem[0].m_ui32ItemID);
			authType = vecPrizeItem[0].m_i8AuthType;
			authArg = vecPrizeItem[0].m_ui32Arg;
		}
	}

	if( itemID == 0 )
	{
		m_ItemSetCtrl->SetVisible(false);
		_SetCardString("SetDefaultPlusItem", L"");
	}
	else
	{
		m_ItemSetCtrl->SetVisible(true);
		GetItemImageMgr()->SelectImage(m_ItemSetCtrl, itemID);

		i3::stack_wstring wstrText;
		i3::rc_wstring wstrItemName;
			
		usf::GetShopItemName(wstrItemName, itemID);

		i3::wstring wstrBonusItemTimeCount;
		usf::GetShopPeriodCountString(itemID, authType, authArg, wstrBonusItemTimeCount);

		i3::sprintf( wstrText, L"%s [%s]", wstrItemName, wstrBonusItemTimeCount);

		if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
			_SetCardString("SetEventCardItemName", wstrText);
		else
			_SetCardString("SetDefaultPlusItem", wstrText);
	}
}

void UIPopupMissionCardInfo_V10::OnCardInfo()
{
	if( m_CardSetType >= QUEST_CARDSET_TYPE_COUNT)
		return;
	
	i3::stack_wstring wstrText;
	i3::stack_wstring wstrDay;

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	if(pSetInfo)
		wstrText = pSetInfo->wstrTitle;
	
	//이벤트 임무카드 기간 표시
	if( m_CardSetType == QUEST_CARDSET_TYPE_EVENT )
	{
		if (pSetInfo)
			wstrDay = pSetInfo->wstrEventPeriod;
		_SetCardString("SetEventCardName", wstrText);
		_SetCardString("SetEventCardDay", wstrDay);
	}
	else
	{
		// 기본 이름
		_SetCardString("SetDefaultCardName", wstrText);
	}

	UINT32 goodsID = (pSetInfo) ? pSetInfo->goodsID : 0;
	
	GOODS_PRICE_INFO price_info;
	::memset(&price_info, 0, sizeof(price_info));

	CShop::i()->GetGoodsPrice(goodsID, price_info, PRICE_TYPE_SHOP_REAL);		// 그냥 실판매가격이면 충분함..
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col}", price_info._Point);
	if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
		_SetCardString("SetEventCardPrice", wstrText);
	else
		_SetCardString("SetDefaultCardPrice", wstrText);

	INT32 cardSetPoint = (pSetInfo) ? pSetInfo->totPoint : 0;
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col}", cardSetPoint);
	if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
		_SetCardString("SetEventCardPoint", wstrText);
	else
		_SetCardString("SetDefaultCardPoint", wstrText);

	INT32 cardSetExp =   (pSetInfo) ? pSetInfo->totExp : 0;
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col}", cardSetExp);
	if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
		_SetCardString("SetEventCardExp", wstrText);
	else
		_SetCardString("SetDefaultCardExp", wstrText);

	if(m_CardSetType != QUEST_CARDSET_TYPE_EVENT)
	{
		// 아래 임무카드 보상 및 지급 표장
		//EXPORT_AVALIABLE_CODE_VALUE;
		INT32 bonusPoint = (pSetInfo) ? pSetInfo->prizeCompletion.point : 0;
		INT32 bonusExp = (pSetInfo) ? pSetInfo->prizeCompletion.exp : 0;

		// 최종 추가 보상 포인트/경험치
		i3::sprintf( wstrText, L"{col:255,151,49,255}+%d{/col}", bonusPoint);
		_SetCardString("SetDefaultPlusPoint", wstrText);

		i3::sprintf( wstrText, L"{col:255,151,49,255}+%d{/col}", bonusExp);
		_SetCardString("SetDefaultPlusExp", wstrText);

		// 지급 표장 하드코딩 언제 없애지...
		_SetCardBadge();

		INT32 iMasterCount = 0;
		iMasterCount += (pSetInfo) ? pSetInfo->bonusBadgeMaster : 0;

		i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col}", iMasterCount);
		_SetCardString("SetBadgeCount3", wstrText);
	}
	
	_SetCardImage();
	_SetCardItem();
}

/*virtual*/ void UIPopupMissionCardInfo_V10::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
	
	m_DefaultFrame = (i3UIFrameWnd *)pScene->FindChildByName("DefaultCardWnd");
	if(m_DefaultFrame != nullptr)
	{
		m_DefaultFrame->EnableCtrl(false);
	}

	m_EventFrame = (i3UIFrameWnd *)pScene->FindChildByName("EventCardWnd");
	if(m_EventFrame != nullptr)
	{
		m_EventFrame->EnableCtrl(false);
	}

	// CardImage들
	char szTemp[MAX_STRING_COUNT];

	for(INT32 i = 0; i < MAX_CARD_PER_CARDSET+1; i++)
	{
		sprintf_s(szTemp, "Card%d", i);
		m_CardSlot[i] = (i3UIButtonImageSet *)pScene->FindChildByName(szTemp);
		m_CardSlot[i]->SetViewOnly(true);
	}
	m_EventCardImage = (i3UIButtonImageSet *)pScene->FindChildByName("EventCardImage");	
	m_DefaultCardImage = (i3UIButtonImageSet *)pScene->FindChildByName("DefaultCardImage");	
}

void UIPopupMissionCardInfo_V10::_SetCardBadge()
{
	INT32 i;
	INT32 iRibbonCount = 0;
	INT32 iEnsignCount = 0;
	INT32 iMedalCount = 0;

	INT32 supplyTable[QUEST_CARDSET_TYPE_COUNT][ MAX_CARD_PER_CARDSET][2] =
	{
		{{0,}, },	//QUEST_CARDSET_TYPE_NONE
		{{0, 1}, {0, 2}, {0, 5}, {0, 3}, {0, 6}, {0, 9}, {0, 13}, {0, 4}, {0, 10}, {0, 14}},	//QUEST_CARDSET_TYPE_TUTORIAL

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_TUTORIAL
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_TUTORIAL

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_GOLD	//삭제 됨
		{{1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15}, {1, 18}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_ASSAULT
		{{1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 16}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_BACKUP
		{{1, 6}, {1, 7}, {1, 8}, {1, 9}, {1, 10}, {1, 17}, {1, 19}, {1, 20}, {2, 11}, {2, 12}},	//QUEST_CARDSET_TYPE_INFILTRATION
		{{2, 1}, {2, 2}, {2, 3}, {2, 8}, {2, 9}, {2, 10}, {2, 4}, {2, 5}, {2, 6}, {2, 13}},	//QUEST_CARDSET_TYPE_SPECIAL
		{{2, 3}, {2, 6}, {2, 10}, {2, 12}, {2, 11}, {2, 12}, {2, 13}, {2, 7}, {2, 14}, {2, 15}},	//QUEST_CARDSET_TYPE_DEFCON
		{{1, 14}, {1, 9}, {1, 4}, {1, 15}, {1, 10}, {1, 5}, {1, 18}, {1, 16}, {1, 17}, {1, 20}},	//QUEST_CARDSET_TYPE_COMMISSIONED_O
		{{2, 1}, {2, 2}, {2, 3}, {2, 8}, {2, 9}, {2, 10}, {2, 4}, {2, 5}, {2, 6}, {2, 13}},	//QUEST_CARDSET_TYPE_COMPANY_O
		{{2, 2}, {2, 5}, {2, 9}, {2, 3}, {2, 5}, {2, 8}, {2, 13}, {2, 7}, {2, 14}, {2, 15}},	//QUEST_CARDSET_TYPE_FIELD_O

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_EVENT

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_2ND
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_2ND

		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_DINO_3ND
		{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}},	//QUEST_CARDSET_TYPE_HUMAN_3ND
	};

	for( i = 0; i < MAX_CARD_PER_CARDSET; i++)
	{
		INT32 supplyMedalType = -1;
		INT32 supplyMedalId = -1;

		if (QUEST_CARDSET_TYPE_EVENT != m_CardSetType)
		{
			supplyMedalType = supplyTable[m_CardSetType][i][0];
			supplyMedalId = supplyTable[m_CardSetType][i][1];
		}

		if (supplyMedalId != 0)
		{
			if( supplyMedalType == 0)		iRibbonCount ++;
			else if (supplyMedalType == 1)	iEnsignCount ++;
			else							iMedalCount ++;
		}
	}	

	i3::stack_wstring wstrText;
	
	i3::sprintf(wstrText, L"{col:255,151,49,255}%d{/col}", iRibbonCount);
	_SetCardString("SetBadgeCount0", wstrText);
	
	i3::sprintf(wstrText, L"{col:255,151,49,255}%d{/col}", iEnsignCount);
	_SetCardString("SetBadgeCount1", wstrText);
	
	i3::sprintf(wstrText, L"{col:255,151,49,255}%d{/col}", iMedalCount);
	_SetCardString("SetBadgeCount2", wstrText);
}

void UIPopupMissionCardInfo_V10::_SetCardImage()
{
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);
	I3ASSERT_RETURN(pSetInfo);

	i3::stack_string strTemplate;
	i3::utf16_to_mb(pSetInfo->wstrUITemplateName, strTemplate);
	
	INT32 i = 0;
	for(i = 0 ; i < MAX_CARD_PER_CARDSET; i++)
	{
		((i3UIControl *)m_CardSlot[i])->SetTemplateByName(strTemplate.c_str());
		m_CardSlot[i]->SetShapeIdx(i);
	}
		
	INT32 imageIndex = pSetInfo->UITemplateIndex;
	
	m_CardSlot[i]->SetShapeIdx(imageIndex);
	if(m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
	{
		//((i3UIControl *)m_EventCardImage)->SetTemplateByName("ButtonImageSet_Item_Card_hs");
		m_EventCardImage->SetShapeIdx(imageIndex);
	}
	else
	{
		//((i3UIControl *)m_DefaultCardImage)->SetTemplateByName("ButtonImageSet_Item_Card_hs");
		m_DefaultCardImage->SetShapeIdx(imageIndex);
	}
}

void UIPopupMissionCardInfo_V10::_SetCardString(const char* luaName, const i3::wliteral_range& text)
{
	LuaState* L = _CallLuaFunction( luaName );
	if( L != nullptr )
	{
		i3Lua::PushStringUTF16To8(L, text);
		_EndLuaFunction(L, 1);
	}
}