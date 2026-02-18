#include "pch.h"
#include "UITabMailBox_Gift.h"

#include "UIPopupMailBox.h"
#include "UIPopupReceiveMail.h"
#include "MailBoxContext.h"
#include "UserInfoContext.h"
#include "UIUtil.h"
#include "UIShopFunction.h"
#include "UIItemInfoUtil.h"

#include "../Shop.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE( UITabMailBox_Gift);

static UITabMailBox_Gift * GetThis()
{
	UIPopupMailBox * pPopup = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
	if( nullptr != pPopup)
	{
		return static_cast<UITabMailBox_Gift*>(pPopup->GetTab(UIPopupMailBox::TAB_GIFT_LIST));
	}

	return nullptr;
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UITabMailBox_Gift_SelectMail( LuaState * L)
	{
		INT32 i32Slot = i3Lua::GetIntegerArg(L, 1, -1 );	
		INT32 i32Cell = i3Lua::GetIntegerArg(L, 2, -1);

		UITabMailBox_Gift * pThis = GetThis();
		if(nullptr != pThis)
		{
			if( i32Cell == UITabMailBox::LISTVIEW_CELL_RECEIVE)
				pThis->ReceiveItem(i32Slot);
			else
				pThis->SetSelectIdx(i32Slot);
		}

		return 0;
	}

	int UITabMailBox_Gift_ReadMail( LuaState * L)
	{
		INT32 nIndex = i3Lua::GetIntegerArg( L, 1, -1 );

		UITabMailBox_Gift * pThis = GetThis();
		if(nullptr != pThis)
			pThis->ReadMail(nIndex);

		return 0;
	}

	int UITabMailBox_Gift_SortMailList( LuaState * L )
	{
		INT32 i32SortType = i3Lua::GetIntegerArg( L, 1);

		UITabMailBox_Gift * pThis = GetThis();
		if(nullptr != pThis)
			pThis->SortMailList(i32SortType);

		return 0;
	}
}

LuaFuncReg UITabMailBox_Gift_Glue[] = {
	{"SelectMail",				UITabMailBox_Gift_SelectMail},
	{"ReadMail",				UITabMailBox_Gift_ReadMail},
	{"SortMailList",			UITabMailBox_Gift_SortMailList},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UITabMailBox_Gift::UITabMailBox_Gift() : m_i32SubjectWidth(0), m_i32EllipseWidth(0)
{
}

UITabMailBox_Gift::~UITabMailBox_Gift()
{
}

/*virtual*/ void UITabMailBox_Gift::_InitializeAtLoad(i3UIScene * pScene)
{
	UITabMailBox::_InitializeAtLoad(pScene);

	if( nullptr == pScene )
		return;

	m_pListView = (i3UIListView_Box*) pScene->FindChildByName("ListView_Gift");

	if( nullptr != m_pListView )
	{
		i3UIListView_Item* pItem = m_pListView->getBoundItem();
		i3UIListView_Cell* pCell = nullptr;

		if( nullptr != pItem )
			pCell = pItem->getCell( UITabMailBox::LISTVIEW_CELL_SUBJECT );

		if( nullptr != pCell )
		{
			i3UIFont* font	= pCell->ObtainUIFont();
			m_i32SubjectWidth = INT32(pCell->getWidth() ) - pCell->getTextSpaceX() * 2;	

			if( nullptr == font )
				return;

			if( nullptr == m_hdc )
				::ReleaseDC(g_hWnd, m_hdc);

			SIZE sizeExt;

			m_hdc = ::GetDC( g_hWnd );

			::SelectObject( m_hdc, font->getFontHandle() );
			::GetTextExtentPoint32A( m_hdc, "....", 3, &sizeExt );

			m_i32EllipseWidth= sizeExt.cx;
		}
	}
}

/*virtual*/ void UITabMailBox_Gift::_ClearMailList(void)
{
	LuaState *  L = _CallLuaFunction( "ClearListViewBox" );
	if(nullptr != L )
	{
		INT32 i32NoteCnt = GetNoteCount();

		i3Lua::PushInteger( L, i32NoteCnt );
		_EndLuaFunction( L, 1 );
	}
}

/*virtual*/ void UITabMailBox_Gift::_SetMailData(INT32 idx, const NOTE_RECEIVE * pNote, bool bMailStateOnly)
{
	if( pNote == nullptr)
		return;

	LuaState * L = nullptr;

	// 쪽지 상태 < 미확인 = 0 / 확인 = 1 >
	INT32 i32MailState = pNote->header._state == NS_UNREAD ? 0 : 1;

	// 수취 상태 < 미확인 = 0 / 확인&미수취 = 1 / 수취 = 2 > 
	GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);
	INT32 i32GiftState = _GetReceiveState( static_cast<INT32>(pGift->_eDeliver));

	if( false == bMailStateOnly )
	{
		i3::wstring wstrMessage;
		if( 0 < pNote->body._messageSize )
		{
			wstrMessage = pNote->body._message;

			_CalculateEllipseText( wstrMessage, m_i32SubjectWidth, m_i32EllipseWidth );
		}
		
		i3::stack_wstring wstrPeriod;
		_GetExpirePeriod(wstrPeriod, pGift->_ui32Expire);

		L = _CallLuaFunction( "SetGiftDataAll");
		if( nullptr != L )
		{
			i3Lua::PushInteger( L, idx );
			i3Lua::PushInteger( L, 0 );
			i3Lua::PushInteger( L, i32MailState );
			i3Lua::PushStringUTF16To8( L, pNote->body._sender);
			i3Lua::PushStringUTF16To8( L, wstrMessage);
			i3Lua::PushStringUTF16To8( L, wstrPeriod);
			i3Lua::PushInteger( L, i32GiftState );
			_EndLuaFunction( L, 7 );
		}
	}
	else
	{
		i3::stack_wstring wstrPeriod;
		_GetExpirePeriod(wstrPeriod, pGift->_ui32Expire);

		L = _CallLuaFunction( "SetGiftState");
		if( nullptr != L )
		{
			i3Lua::PushInteger( L, idx );
			i3Lua::PushInteger( L, i32MailState );
			i3Lua::PushStringUTF16To8( L, wstrPeriod );
			i3Lua::PushInteger( L, i32GiftState );
			_EndLuaFunction( L, 4 );
		}
	}
}

/*virtual*/ void UITabMailBox_Gift::_SetMailTextColor(INT32 idx, const NOTE_RECEIVE * pNote)
{
	COLOR Color;

	GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);

	switch(pGift->_eDeliver)
	{
	case TYPE_GIFT_DELIVER_NORMAL	: i3Color::Set(&Color, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);	break;
	case TYPE_GIFT_DELIVER_READ:	  i3Color::Set(&Color, (UINT8)136, (UINT8)136, (UINT8)139, (UINT8)255); break;
	case TYPE_GIFT_DELIVER_RECEIVED	: i3Color::Set(&Color, (UINT8)136, (UINT8)136, (UINT8)139, (UINT8)255);	break;
	default							: i3Color::Set(&Color, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);	break;
	}

	LuaState * L = _CallLuaFunction("SetMailTextColor");

	if( nullptr != L )
	{
		i3Lua::PushInteger(L, idx);
		i3Lua::PushColor(L , &Color);
		_EndLuaFunction(L, 5);
	}
}

/*virtual*/ void UITabMailBox_Gift::_SetSelectAll(bool bSelect)
{
	LuaState * L = _CallLuaFunction( "SetSelectAll");

	if( nullptr != L )
	{
		i3Lua::PushBoolean( L, bSelect ? true : false );
		_EndLuaFunction( L, 1 );
	}
}

/*virtual*/ NOTE_RECEIVE * UITabMailBox_Gift::_FindNoteData(INT32 idx)
{
	return const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetNoteGiftNote( idx ));
}

/*virtual*/ INT32 UITabMailBox_Gift::_GetNoteDataCount(void)
{
	return MailBoxContext::i()->GetNoteGiftCount();
}

/*virtual*/ INT32 UITabMailBox_Gift::_GetMailType(NOTE_RECEIVE & Note)
{
	return UIPopupReceiveMail::GIFT;
}

/*virtual*/ void UITabMailBox_Gift::_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring& wstrItemInfo)
{
	wstrItemInfo = GAME_RCSTRING("STR_POPUP_UNKNOWN_GIFT");

	//	선물하기 Note에서 pNote->body._user 포인터는 GIFT_BUFFER 구조체 포인터임.
	GIFT_BUFFER * pGift = (GIFT_BUFFER*) pNote->body._user;
	SHOP_GOODS_LIST * pGoodsList = CShop::i()->GetGoodsListData( pGift->_ui32GoodsID );

	if( pGoodsList == nullptr )
		return;


	if( pGoodsList->_ui8ItemCount > 1 )
	{
		bool isPeriod = false;
		INT32 i32PeriodIdx = 0;

		for(INT32 j = 0; j < pGoodsList->_ui8ItemCount; j++)
		{
			if (ITEM_TYPE_TIME == pGoodsList->_ppItemData[j]->_ui8AuthType)
			{
				isPeriod = true;
				i32PeriodIdx = j;
				break;
			}
		}

		//	getInfo는 keyItem으로 가져와야 정확한 info를 얻을 수 있다. (그렇지않으면 nullptr일 수 있다.)
		ShopPackageGoodsInfo* pInfo = g_pShopItemInfoDataBase->GetPackageGoodsInfo( pGift->_ui32GoodsID );
		I3ASSERT( pInfo != nullptr);
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[i32PeriodIdx];

		if( isPeriod )
		{
			UINT32 day = ConvertSecToUseDay(ui32ItemArg);
			// 이름: %s\n사용기간: %d일
			wstrItemInfo = i3::format_string( GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"), pInfo->GetScriptName(), day );
		}
		else
		{
			T_ItemID ItemID = pGoodsList->_ppItemData[i32PeriodIdx]->_ui32ItemID;
			
			i3::rc_wstring wstrTemp;

			//	유료 아이템 Goods에는 pGoods->_AuthArg에 모두 갯수가 있기 때문에 예외 처리가 필요 komet
			switch( (ITEM_TYPE) GET_ITEM_TYPE(ItemID) )
			{	
			//	지속성
			case	ITEM_TYPE_MAINTENANCE	: wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME"); break;		
			//	소모성
			case	ITEM_TYPE_WRAP_PERIOD	:
			case	ITEM_TYPE_WRAP_COUNT	: wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME"); break;
			default							: return;
			}

			wstrItemInfo = i3::format_string(wstrTemp, pInfo->GetScriptName(), ui32ItemArg);							
		}
	}
	else
	{
		UINT8 ui8AuthType = pGoodsList->_ppItemData[0]->_ui8AuthType;
		UINT32 ui32ItemArg = pGoodsList->_pui32ItemArg[0] ; 
		T_ItemID ItemID = pGoodsList->_ppItemData[0]->_ui32ItemID;

		if( GET_ITEM_TYPE(ItemID) == ITEM_TYPE_SALECOUPON )
		{
#if defined (USE_SALE_COUPON)
			SALE_COUPON * pCoupon = CShop::i()->GetCouponData(ItemID);
			
			i3::rc_wstring wstrCouponName;				// TODO : 서버 협업 요망..(2014.07.16.수빈)
			i3::mb_to_utf16(pCoupon->m_strCouponName, wstrCouponName);
			wstrItemInfo = wstrCouponName;
#endif		
		}
		else
		{
			i3::rc_wstring wstrTemp;
			i3::rc_wstring wstrItemName;		
			usf::GetShopItemName(wstrItemName, ItemID);
		
			I3ASSERT( ui8AuthType != 0 );
			switch( ui8AuthType )
			{
			case ITEM_TYPE_COUNT :
				{
					if( item_def::get_category(ItemID) == item_def::CTG_ITEM )
					{
						if (GET_ITEM_TYPE(ItemID) != ITEM_TYPE_WRAP_COUNT)
						{
							UINT32 ui32ItemTerm = GET_ITEM_SUBTYPE(ItemID);
							if (ui32ItemTerm > 0)
								wstrItemName = i3::format_string(GAME_RCSTRING("STR_INVEN_CASH_ITEM_TERM"), wstrItemName, ui32ItemTerm);
						}
					}

					wstrTemp = GAME_RCSTRING("STR_POPUP_EXPENDABLES_GIFT_NAME");
				}
				break;
			case ITEM_TYPE_TIME :
				{
					wstrTemp = GAME_RCSTRING("STR_POPUP_MAINTENANCE_GIFT_NAME");
					ui32ItemArg = (INT32) ConvertSecToUseDay(ui32ItemArg);	//	인증기간 (일)로 변환
				}
				break;
			}

			wstrItemInfo = i3::format_string(wstrTemp, wstrItemName, ui32ItemArg);
		}	
	}
}

/*virtual*/ INT32 UITabMailBox_Gift::_GetNoteIdx(const NOTE_RECEIVE * pNote)
{
	const GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);
	return pGift->_i32GiftWareDBIdx;
}

/*virtual*/ void UITabMailBox_Gift::_SendEvent(int action, int * list, int count)
{
	if( MailBoxContext::i()->IsWaitResult() )
		return;

	MailBoxContext::i()->ResetGiftReqList();

	GAME_EVENT evt = _GetEvent(action);

	UINT8 ui8Type  = _GetGiftAuthType(action);
	UINT8 ui8Count = 0;
	T_GiftDBIdx GiftIdx[ MAX_SHOP_GOODS_BUY_COUNT ];

	if(ui8Type == INIT_UINT8)
		return;

	INT32 req_cnt = 0;

	while(req_cnt < count)
	{
		i3mem::FillZero( GiftIdx, sizeof(GiftIdx));
		ui8Count = 0;

		for(INT32 idx = 0 ; idx < MAX_SHOP_GOODS_BUY_COUNT ; ++idx, ++req_cnt)
		{
			if(req_cnt == count)
				break;

			GiftIdx[idx] = list[req_cnt]; 
			ui8Count++;
		}

		if(ui8Count > 0 && evt > EVENT_NOTHING)
		{
			if(GameEventSender::i()->SetEvent( evt, &ui8Type, &ui8Count, GiftIdx) == true)
			{
				MailBoxContext::i()->BuildGiftReqList( ui8Type, GiftIdx, ui8Count);
				i3UI::setDisableUIInput(true);
			}				
		}
	}
}

/*virtual*/ GAME_EVENT UITabMailBox_Gift::_GetEvent(int action)
{
	switch(action)
	{
	case ACTION_READ	:	return EVENT_AUTH_GIFT_GOODS;
	case ACTION_DELETE	:	return EVENT_AUTH_GIFT_GOODS;
	case ACTION_RECEIVE	:	return EVENT_AUTH_GIFT_GOODS;
	}

	return EVENT_NOTHING;
}

INT32 UITabMailBox_Gift::_GetReceiveState(const INT32 i32Deliver)
{
	INT32 i32GiftState = GIFT_UNREAD;
	switch(i32Deliver)
	{
	case TYPE_GIFT_DELIVER_NORMAL	: i32GiftState = GIFT_UNREAD;	break;
	case TYPE_GIFT_DELIVER_READ		: i32GiftState = GIFT_READED;	break;
	case TYPE_GIFT_DELIVER_RECEIVED	: i32GiftState = GIFT_RECEIVE;	break;
	default							: i32GiftState = GIFT_UNREAD;	break;
	}

	return i32GiftState;
}

void UITabMailBox_Gift::_GetExpirePeriod(i3::stack_wstring & wstrOut, const UINT32 ui32Expire)
{
	UINT32 ui32SecTerm = UserInfoContext::i()->GetInventorySecTime(ui32Expire);
	ui32SecTerm -= ITEM_TERM_ADJUST_VALUE; // 시간값 보정

	if( 0 > static_cast<INT32>(ui32SecTerm) )
		ui32SecTerm = 0;

	UINT32 ui32Day = ConvertSecToUseDay(ui32SecTerm);

	i3::sprintf(wstrOut, GAME_RCSTRING("STR_POPUP_WHAT_DAY"), ui32Day);

}

UINT8 UITabMailBox_Gift::_GetGiftAuthType(int action)
{
	UINT8 ui8AuthType = INIT_UINT8;

	switch(action)
	{
	case ACTION_READ	: ui8AuthType = TYPE_SHOP_GIFT_READ;	break;
	case ACTION_DELETE	: ui8AuthType = TYPE_SHOP_GIFT_DELETE;	break;
	case ACTION_RECEIVE	: ui8AuthType = TYPE_SHOP_GIFT_AUTH;	break;
	}

	return ui8AuthType;
}

/*virtual*/ void UITabMailBox_Gift::OnCreate( i3GameNode * pParent)
{
	UITabMailBox::OnCreate( pParent);
	AddScene( "Scene/Popup/PBRe_PopUp_MessageBox_Tab_Gift.i3UIs", UITabMailBox_Gift_Glue);
}