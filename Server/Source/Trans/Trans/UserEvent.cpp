#include "pch.h"
#include "UserEvent.h"

CUserEventInfo::CUserEventInfo()
{
}

CUserEventInfo::~CUserEventInfo()
{
}

void CUserEventInfo::Reset()
{
	m_stNewUser.m_ui8ItemCount = 0;
	for( INT32 i = 0 ; i < EVENTITEM_LOGIN_SLOT_COUNT ; i++ )
	{
		m_stLoginUser[i].m_ui8ItemCount = 0;
	}
}

BOOL CUserEventInfo::SetLoginUser( DATE32 dt32StartDate, UINT8 ui8Slot, UINT8 ui8GoodsCount, T_GoodsID* pTGoodsID )
{
	if( EVENTITEM_LOGIN_SLOT_COUNT <= ui8Slot )	return FALSE;

	ST_ITEM*			pInsertItem;

	EVENT_ITEM_LOGIN*	pLoginUser = &m_stLoginUser[ ui8Slot ];
	
	for( INT32 i32GoodsCount = 0 ; i32GoodsCount < ui8GoodsCount ; i32GoodsCount++ )
	{
		if( g_pConfig->IsContentsEnabled( CONTENTS_GIFT ) )
		{
			if( MAX_SHOP_GOODS_BUY_COUNT > (pLoginUser->m_ui8ItemCount+1) )
			{
				pLoginUser->m_stItem_Goods[ pLoginUser->m_ui8ItemCount ].m_ui32GoodsID = pTGoodsID[ i32GoodsCount ];
				pLoginUser->m_ui8ItemCount++;
			}
		}
		else
		{
			if( MAX_SHOP_DB_ITEM_COUNT > (pLoginUser->m_ui8ItemCount+1) )
			{
				pInsertItem = &pLoginUser->m_stItem_Item[ pLoginUser->m_ui8ItemCount ];
				g_pShop->GetItemInfo_byGoodsID( pTGoodsID[ i32GoodsCount ], &pInsertItem->m_TItemID, &pInsertItem->m_TItemArg );
				pLoginUser->m_ui8ItemCount++;
			}
		}
	}

	pLoginUser->m_dt32SendDate	= dt32StartDate;
	return TRUE;
}

BOOL CUserEventInfo::SetNewUser( T_GoodsID TGoodsID )
{
	ST_ITEM* pInsertItem;
	if( MAX_SHOP_DB_ITEM_COUNT > (m_stNewUser.m_ui8ItemCount + 1) )
	{
		pInsertItem = &m_stNewUser.m_stItem[ m_stNewUser.m_ui8ItemCount ];
		g_pShop->GetItemInfo_byGoodsID( TGoodsID, &pInsertItem->m_TItemID, &pInsertItem->m_TItemArg );
		m_stNewUser.m_ui8ItemCount++;
	}
	return TRUE;
}

BOOL CUserEventInfo::GetLoginUserEventGoods( DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn )
{
	pGiftIn->m_ui8GiftCount = 0;
	EVENT_ITEM_LOGIN* pstLoginUser;
	for( INT32 i = 0 ; i < EVENTITEM_LOGIN_SLOT_COUNT ; i++ )
	{
		pstLoginUser = &m_stLoginUser[i];

		if( (0 == pstLoginUser->m_ui8ItemCount) || 
			(pdt32EventSendDate[i] == pstLoginUser->m_dt32SendDate) )
		{
			pdt32EventSendDate[i] = (UINT32)0;
			continue;		// 이미 지급 받았습니다.	저장을 줄이기 위해 0으로 세팅합니다.
		}

		pdt32EventSendDate[i]	= pstLoginUser->m_dt32SendDate;
		i3mem::Copy( &pGiftIn->m_aGift[ pGiftIn->m_ui8GiftCount ], pstLoginUser->m_stItem_Goods, sizeof(S2MO_RECV_GIFT)*pstLoginUser->m_ui8ItemCount );
		pGiftIn->m_ui8GiftCount			+= pstLoginUser->m_ui8ItemCount;
	}

	if( 0 == pGiftIn->m_ui8GiftCount )	return FALSE;

	return TRUE;
}

BOOL CUserEventInfo::GetLoginUserEventItem( DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem )
{
	*pui8ItemCount = 0;
	EVENT_ITEM_LOGIN* pstLoginUser;
	for( INT32 i = 0 ; i < EVENTITEM_LOGIN_SLOT_COUNT ; i++ )
	{
		pstLoginUser = &m_stLoginUser[i];

		if( (0 == pstLoginUser->m_ui8ItemCount) || 
			(pdt32EventSendDate[i] == pstLoginUser->m_dt32SendDate) )
		{
			pdt32EventSendDate[i] = (UINT32)0;
			continue;		// 이미 지급 받았습니다.	저장을 줄이기 위해 0으로 세팅합니다.
		}

		pdt32EventSendDate[i]	= pstLoginUser->m_dt32SendDate;
		i3mem::Copy( &pItem[ *pui8ItemCount ], pstLoginUser->m_stItem_Item, sizeof(ST_ITEM)*pstLoginUser->m_ui8ItemCount );
		*pui8ItemCount			+= pstLoginUser->m_ui8ItemCount;
	}

	if( 0 == *pui8ItemCount )	return FALSE;
	
	return TRUE;
}

BOOL CUserEventInfo::GetNewUserEvent( UINT8* pui8ItemCount, ST_ITEM* pItem )
{
	if( 0 == m_stNewUser.m_ui8ItemCount )	return FALSE;

	*pui8ItemCount	= m_stNewUser.m_ui8ItemCount;
	i3mem::Copy( pItem, m_stNewUser.m_stItem, sizeof( ST_ITEM ) * m_stNewUser.m_ui8ItemCount ) ;
	//pItem			= m_stNewUser.m_stItem;

	return TRUE;
}

CUserEventLocale::CUserEventLocale()
{
}

CUserEventLocale::~CUserEventLocale()
{
}

void CUserEventLocale::Reset()
{
	for( INT32 i = 0 ; i < USER_EVENT_COUNT ; i++ )
	{
		m_stEventLocale[i].Reset();
	}
}

CUserEventManager::CUserEventManager()
{
	m_i32UseBufferIdx	= 0;
	m_i32BackBufferIdx	= 1;
}

CUserEventManager::~CUserEventManager()
{
}

void CUserEventManager::SwapBuffer()
{
	INT32 i32Temp		= m_i32UseBufferIdx;
	m_i32UseBufferIdx	= m_i32BackBufferIdx;
	m_i32BackBufferIdx	= i32Temp;
}

void CUserEventManager::Update( DATE32 dt32NowDate )
{
	CUserEventLocale* pEventLocale = &m_stUserEvent[ m_i32BackBufferIdx ];

	pEventLocale->Reset();
	
	INT32					i;	

	////////////////////////////////////////////////////////////
	// Login User
	LOGIN_USER_EVENT*		pLoginEvent;
	for( i = 0 ; i < *m_pui16LoginUserCount ; i++ )
	{
		pLoginEvent = &m_pstLoginUser[i];		// 이벤트 아이템 지급 
			
		if( dt32NowDate > pLoginEvent->m_dt32EndDate )			continue;
		if( dt32NowDate < pLoginEvent->m_dt32StartDate )		continue;

		pEventLocale->SetLoginUser( pLoginEvent->m_ui8LocaleCode, pLoginEvent->m_dt32StartDate, pLoginEvent->m_ui8Slot, pLoginEvent->m_ui8GoodsCount, pLoginEvent->m_ui32GoodsID );
	}

	////////////////////////////////////////////////////////////
	// New User
	NEW_USER_EVENT*		pNewEvent;
	for( i = 0 ; i < *m_pui16NewUserCount ; i++ )
	{
		pNewEvent = &m_pstNewUser[i];

		if( dt32NowDate > pNewEvent->m_dt32EndDate )			continue;
		if( dt32NowDate < pNewEvent->m_dt32StartDate )			continue;

		pEventLocale->SetNewUser( pNewEvent->m_ui8LocaleCode, pNewEvent->m_ui32GoodsID );
	}

	SwapBuffer();
}