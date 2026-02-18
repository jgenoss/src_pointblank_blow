#ifndef __USER_EVENT_H__
#define __USER_EVENT_H__

class CUserEventInfo
{
	EVENT_ITEM_NEW			m_stNewUser;															// 신규 유저 아이템
	EVENT_ITEM_LOGIN		m_stLoginUser[ EVENTITEM_LOGIN_SLOT_COUNT ];							// 로그인 유저 아이템

public:
	CUserEventInfo();
	~CUserEventInfo();

	void					Reset();

	BOOL					SetLoginUser( DATE32 dt32StartDate, UINT8 ui8Slot, UINT8 ui8GoodsCount, T_GoodsID* pTGoodsID );
	BOOL					SetNewUser( T_GoodsID TGoodsID );

	BOOL					GetLoginUserEventGoods( DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn );
	BOOL					GetLoginUserEventItem( DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem );

	BOOL					GetNewUserEvent( UINT8* pui8ItemCount, ST_ITEM* pItem );
};

class CUserEventLocale
{
	CUserEventInfo			m_stEventLocale[ USER_EVENT_COUNT ];

public:
	CUserEventLocale();
	~CUserEventLocale();

	void					Reset();

	BOOL					SetLoginUser( UINT8 ui8LocaleCode, DATE32 dt32StartDate, UINT8 ui8Slot, UINT8 ui8GoodsCount, T_GoodsID* pTGoodsID )
	{
		if( USER_EVENT_COUNT <= ui8LocaleCode )	return FALSE;

		return m_stEventLocale[ ui8LocaleCode ].SetLoginUser( dt32StartDate, ui8Slot, ui8GoodsCount, pTGoodsID );
	}
	BOOL					SetNewUser( UINT8 ui8LocaleCode, T_GoodsID TGoodsID )
	{
		if( USER_EVENT_COUNT <= ui8LocaleCode )	return FALSE;

		return m_stEventLocale[ ui8LocaleCode ].SetNewUser( TGoodsID );
	}

	BOOL					GetLoginUserEventGoods( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn )
	{
		return m_stEventLocale[ ui8LocaleCode ].GetLoginUserEventGoods( pdt32EventSendDate, pGiftIn );
	}

	BOOL					GetLoginUserEventItem( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem )
	{
		return m_stEventLocale[ ui8LocaleCode ].GetLoginUserEventItem( pdt32EventSendDate, pui8ItemCount, pItem );
	}

	BOOL					GetNewUserEvent( UINT8 ui8LocaleCode, UINT8* pui8ItemCount, ST_ITEM* pItem )
	{
		return m_stEventLocale[ ui8LocaleCode ].GetNewUserEvent( pui8ItemCount, pItem );
	}
};

class CUserEventManager
{
	INT32					m_i32UseBufferIdx;
	INT32					m_i32BackBufferIdx;					// Next Buffer Idx 체크가 많아 변수를 따로 둠

	CUserEventLocale		m_stUserEvent[ 2 ];					// Double Buffer 를 위한 2개 배열

	UINT16*					m_pui16LoginUserCount;				// 로그인 유저 개수			- 정보 저장 용
	LOGIN_USER_EVENT*		m_pstLoginUser;						// 로그인 유저 아이템		- 정보 저장 용

	UINT16*					m_pui16NewUserCount;				// 로그인 유저 개수			- 정보 저장 용
	NEW_USER_EVENT*			m_pstNewUser;						// 신규 유저 아이템		- 정보 저장 용

public:
	CUserEventManager();
	~CUserEventManager();
	
	void					Update( DATE32 dt32NowDate );

	void					SwapBuffer();

	void					SetLoginUserInfo( UINT16* pui16LoginUserCount, LOGIN_USER_EVENT* pstLoginUser )
	{
		m_pui16LoginUserCount	= pui16LoginUserCount;
		m_pstLoginUser			= pstLoginUser;;
	}

	void					SetNewUserInfo( UINT16* pui16NewUserCount, NEW_USER_EVENT* pstNewUser )
	{
		m_pui16NewUserCount		= pui16NewUserCount;
		m_pstNewUser			= pstNewUser;
	}

	BOOL					GetLoginUserEventGoods( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, SEND_GIFT* pGiftIn )
	{
		return m_stUserEvent[ m_i32UseBufferIdx ].GetLoginUserEventGoods( ui8LocaleCode, pdt32EventSendDate, pGiftIn );
	}

	BOOL					GetLoginUserEventItem( UINT8 ui8LocaleCode, DATE32* pdt32EventSendDate, UINT8* pui8ItemCount, ST_ITEM* pItem )
	{
		return m_stUserEvent[ m_i32UseBufferIdx ].GetLoginUserEventItem( ui8LocaleCode, pdt32EventSendDate, pui8ItemCount, pItem );
	}

	BOOL					GetNewUserEvent( UINT8 ui8LocaleCode, UINT8* pui8ItemCount, ST_ITEM* pItem )
	{
		return m_stUserEvent[ m_i32UseBufferIdx ].GetNewUserEvent( ui8LocaleCode, pui8ItemCount, pItem );
	}
};

#endif