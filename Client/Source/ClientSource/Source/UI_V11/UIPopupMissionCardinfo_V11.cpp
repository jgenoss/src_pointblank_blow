#include "pch.h"
#include "UIPopupMissionCardInfo_V11.h"
#include "../CommunityContext.h"
#include "UI/UIMainFrame_V11.h"

#include "UI/UIShopFunction.h"
#include "UI/ItemImageMgr.h"
#include "UI/UIMath.h"

#include "../ShopContext.h"

#include "MissionCard/MCardUtil.h"
#include "MissionCard/MCardSetInfo.h"



I3_CLASS_INSTANCE( UIPopupMissionCardInfo_V11);

static UIPopupMissionCardInfo_V11 * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	return (UIPopupMissionCardInfo_V11*) pFrame->GetPopup( UPW_MISSIONINFO );
}

//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UIPopupMissionCardInfo_V11_Confirm(LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}

	int UIPopupMissionCardInfo_V11_Next(LuaState * L)
	{
		GetThis()->CardMove(1);
		return 0;
	}

	int UIPopupMissionCardInfo_V11_Prev(LuaState * L)
	{
		GetThis()->CardMove(-1);
		return 0;
	}
}

LuaFuncReg UIPopupMissionCardInfo_Glue[] = {
	{"CardInfo_Confirm",	UIPopupMissionCardInfo_V11_Confirm},
	{"CardInfo_Next",		UIPopupMissionCardInfo_V11_Next},
	{"CardInfo_Prev",		UIPopupMissionCardInfo_V11_Prev},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//




UIPopupMissionCardInfo_V11::UIPopupMissionCardInfo_V11() : m_CardSetType(QUEST_CARDSET_TYPE_NONE)
{
	m_ItemSetCtrl = 0;
	m_EventCardIndex = 0;
}

UIPopupMissionCardInfo_V11::~UIPopupMissionCardInfo_V11()
{
}

/*virtual*/ void UIPopupMissionCardInfo_V11::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene("Scene/Popup/PointBlankRe_MyInfo_MissionCard_Detail.i3UIs",UIPopupMissionCardInfo_Glue);
	// Load Scene
}


/*virtual*/ bool UIPopupMissionCardInfo_V11::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	//상품 정보를 수집한다.
	CShopContext* pShopContext = CShopContext::i();

	pShopContext->BuildGoodsList();

	i3UIScene * pScene = UIBase::GetScene("PointBlankRe_MyInfo_MissionCard_Detail");

	LuaState* L = nullptr;
	i3UIButtonImageSet* btnset = nullptr;
	L = UILua::CallLuaFunction(pScene, "GetImageSetCtrl");
	if( L != nullptr )
	{
		btnset = reinterpret_cast< i3UIButtonImageSet* >(UILua::ReturnLuaPointer(L, 0));
		m_ItemSetCtrl = GetItemImageMgr()->CreateImgSetCtrl(btnset);
	}

	m_CardSetType = *static_cast<INT32*>(pArg1);
	
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	//Event Card일 경우 복수로 아이템을 받는다.
	
	INT32 EventCardMaxCount = 1;

	if (pSetInfo != nullptr)
	{
		EventCardMaxCount =	INT32( pSetInfo->prizeCompletion.vecPrizeItem.size() );	
	}

//	INT32 EventCardMaxCount = (pSetInfo) ? INT32( pSetInfo->vecBonusItem.size() ) : 1;
	
	L = UILua::CallLuaFunction(pScene, "EnableCardNext");
	if( L != nullptr )
	{
		i3Lua::PushBoolean(L, EventCardMaxCount > 1 ? true : false);
		_EndLuaFunction(L, 1);
	}

	m_EventCardIndex = 0;

	SetEnable(true);
	OnCardInfo();

	return true;
}

void	UIPopupMissionCardInfo_V11::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_ItemSetCtrl);
}

void UIPopupMissionCardInfo_V11::Confirm()
{
	GetMainFrame()->TogglePopup( UPW_MISSIONINFO);
}

void UIPopupMissionCardInfo_V11::CardMove(int move_type)
{
	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	//Event Card일 경우 복수로 아이템을 받는다.

	INT32 EventCardMaxCount = 1;
	if (pSetInfo != nullptr)
	{
		EventCardMaxCount =	INT32( pSetInfo->prizeCompletion.vecPrizeItem.size() );	
	}

//	INT32 EventCardMaxCount = (pSetInfo) ? INT32( pSetInfo->vecBonusItem.size() ) : 1;

	m_EventCardIndex += move_type;
	m_EventCardIndex = uiMath::roundlimited(m_EventCardIndex, (INT32)0, EventCardMaxCount-1);
	_SetCardItem();
}

void UIPopupMissionCardInfo_V11::_SetCardItem()
{

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	INT32 itemID = 0;
	UINT8 authType = 0;
	UINT32 authArg = 0;

	if (pSetInfo)
	{
		const i3::svector<MCardPrizeInfo_Mqf_ItemInfo>& vecPrizeItem = pSetInfo->prizeCompletion.vecPrizeItem;

		const INT32 numBonusItem = INT32(vecPrizeItem.size() );

		if ( m_EventCardIndex < numBonusItem)
		{
			itemID = MCardUtil::ConvertItemID_V10_TO_V20(vecPrizeItem[m_EventCardIndex].m_ui32ItemID);
			authType = vecPrizeItem[m_EventCardIndex].m_i8AuthType;
			authArg = vecPrizeItem[m_EventCardIndex].m_ui32Arg;
		}
	}

	if( itemID == 0 )
	{
		VEC2D *PrizeSize, *MainSize;
		VEC3D *PrizePos, *ButtonPos;

		OnShapeChange();
		PrizePos = m_PrizeFrame->getPos();
		ButtonPos = m_CloseButton->getPos();
		PrizeSize = m_PrizeFrame->getSize();
		MainSize = m_MainFrame->getSize();
		MainSize->y -= PrizeSize->y;
		ButtonPos->y = PrizePos->y;

		m_CloseButton->setPos(ButtonPos);
		m_MainFrame->setSize(MainSize);
	}
	else
	{

		GetItemImageMgr()->SelectImage(m_ItemSetCtrl, itemID);

		i3::stack_wstring wstrText;
		i3::rc_wstring wstrItemName;
		
		usf::GetShopItemName(wstrItemName, itemID);

		i3::wstring wstrBonusItemTimeCount;
		usf::GetShopPeriodCountString(itemID, authType, authArg, wstrBonusItemTimeCount);

		i3::sprintf( wstrText, L"%s [%s]", wstrItemName, wstrBonusItemTimeCount);

		_SetCardString("SetCardPrizeName", wstrText);

		i3::rc_wstring wstrItemDesc;
		usf::GetShopItemDescription(wstrItemDesc, itemID);
		i3::sprintf( wstrText, L"{col:255,255,255,255}%s{/col}", wstrItemDesc);
		_SetCardString("SetCardPrizeDesc", wstrText);
	}
}

void UIPopupMissionCardInfo_V11::OnCardInfo()
{
	if( m_CardSetType >= QUEST_CARDSET_TYPE_COUNT)
		return;


	
	INT32 iMasterCount = 0;
	
	i3::stack_wstring wstrText;

	const MCardSetInfo* pSetInfo = MCardUtil::FindMCardSetInfoBySetType(m_CardSetType);

	if (pSetInfo)
	{
		wstrText = pSetInfo->wstrTitle;

		//이벤트 임무카드 기간 표시
		if (m_CardSetType == QUEST_CARDSET_TYPE_EVENT)
		{
			wstrText += L'\n';
			wstrText += pSetInfo->wstrEventPeriod;
		}
	}

	_SetCardString("SetCardName", wstrText);
	
	UINT32 goodsID = (pSetInfo) ? pSetInfo->goodsID : 0;
	
	GOODS_PRICE_INFO price_info;
	::memset(&price_info, 0, sizeof(price_info));

	CShop::i()->GetGoodsPrice(goodsID, price_info, PRICE_TYPE_SHOP_REAL);		// 그냥 실판매가격이면 충분함..
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col}", price_info._Point);
	_SetCardString("SetCardValue", wstrText);

	INT32 cardSetPoint = (pSetInfo) ? pSetInfo->totPoint : 0;
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col} %s", cardSetPoint, GAME_RCSTRING("STBL_IDX_SHOP_MONEY"));
	_SetCardString("SetCardPoint", wstrText);

	INT32 cardSetExp =   (pSetInfo) ? pSetInfo->totExp : 0;
	i3::sprintf( wstrText, L"{col:255,151,49,255}%d{/col} EXP", cardSetExp);
	_SetCardString("SetCardExp", wstrText);

	iMasterCount += (pSetInfo) ? pSetInfo->bonusBadgeMaster : 0;

	// 아래 임무카드 보상 및 지급 표장
	//EXPORT_AVALIABLE_CODE_VALUE;
	INT32 bonusPoint = (pSetInfo) ? pSetInfo->prizeCompletion.point : 0;
	INT32 bonusExp = (pSetInfo) ? pSetInfo->prizeCompletion.exp : 0;

	// 지급표장에 대해서 하드코딩 되어 있습니다.		// 이 하드코딩은 어느 세월에 고칠지 모르겠음( 2015.06.23.수빈)
	// 현재, pef와 mqf 모두에 안들어있는걸 봐서는 언젠가 mqf쪽이나 서버 패킷 정보로 추가될 가능성이 있어보임..
	// 놀랍게도 1.1버전은 리본,엔사인,메달에 대한 정보를 출력하고 있지 않다..
/*
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
*/

	// 최종 추가 보상 포인트/경험치
	i3::sprintf( wstrText, L"{col:255,151,49,255}+%d{/col}", bonusPoint);
	_SetCardString("SetCardFinalPoint", wstrText);

	i3::sprintf( wstrText, L"{col:255,151,49,255}+%d{/col}", bonusExp);
	_SetCardString("SetCardFinalExp", wstrText);

	_SetCardItem();
}

void UIPopupMissionCardInfo_V11::OnShapeChange()
{
	i3UIControl * pChild = (i3UIControl *)m_PrizeFrame->getFirstChild();
	INT32 ChildCount = m_PrizeFrame->GetChildCount();

	for(INT32 i = 0; i < ChildCount ; i++)
	{
		pChild->EnableCtrl(false);
		pChild = (i3UIControl *)pChild->getNext();
	}
	m_PrizeFrame->EnableCtrl(false);
}

/*virtual*/ void UIPopupMissionCardInfo_V11::_InitializeAtLoad(i3UIScene * pScene)
{
	UIPopupBase::_InitializeAtLoad(pScene);
	m_MainFrame = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd0");
	m_PrizeFrame = (i3UIFrameWnd *)pScene->FindChildByName("i3UIFrameWnd");
	m_CloseButton = (i3UIButton *)pScene->FindChildByName("Button_Close");
}

void UIPopupMissionCardInfo_V11::_SetCardString(const char* luaName, const i3::wliteral_range& text)
{
	LuaState* L = _CallLuaFunction( luaName );
	if( L != nullptr )
	{
		i3Lua::PushStringUTF16To8(L, text);
		_EndLuaFunction(L, 1);
	}
}