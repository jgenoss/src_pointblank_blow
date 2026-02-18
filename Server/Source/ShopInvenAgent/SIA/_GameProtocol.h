#ifndef __GAME_PROTOCOL_H__
#define __GAME_PROTOCOL_H__

/**--------------------------------------------------------------
FileName        : _GameProtocol.h
Description     : 
	이파일은 게임서버와 통신할 게임아이템 과금 프로토콜을 정의한다.
	아래 정의된 Packet 타입과 Structure는 해당 게임사와 충분히 협의
	하여 각 게임별로 정의하여야 한다.
Caution         : 
	게임프로토콜 정의시에 Structure Header에 해당하는 부분에 패킷길이
	패킷타입, 응답코드를 순서대로 정의하고 short 타입으로 정의한다.
	패킷길이 : short   ReqLen;		
	패킷타입 : short	ReqType;	
	응답코드 : short	RetCode;

Copyright 2001-2008 by PayLetter Inc. All rights reserved.
Author          : soohyunk@payletter.com, 2008-06-25
Modify History  : 
---------------------------------------------------------------*/

// 게임서버/빌링GTX서버 상호 명령어 정의
// Packet Type 정의
#define GTX_PK_V11_TYPE_GETBALANCE					10	// 잔액조회 요청/응답
#define GTX_PK_V11_TYPE_INSPOLICYEVENT				12	// 정책이벤트캐시 지급 요청/응답
#define GTX_PK_V11_TYPE_PURCHASEITEM				20	// 아이템구매 
#define GTX_PK_V11_TYPE_GIFTITEM					21	// 아이템선물 
#define GTX_PK_V11_TYPE_CNLPURCHASE					22	// 아이템구매 취소
#define GTX_PK_V11_TYPE_REGCOUPON					30	// 쿠폰 등록
#define GTX_PK_V11_TYPE_CNLCOUPON					31	// 쿠폰 등록 취소
#define GTX_PK_V11_TYPE_HEALTH_CHECK				40	// GTX Health Check

// GTX 프로토콜
#pragma pack(1)

// Health Check 구조체
typedef struct _GTX_PK_V11_HEALTH_CHECK
{
	WORD	ReqLen;		// IN	1
	WORD	ReqType;	// IN	2
	DWORD	ReqKey;		// IN	3

	WORD	RetCode;	// OUT	4
} GTX_PK_V11_HEALTH_CHECK, *PGTX_PK_V11_HEALTH_CHECK;

// 잔액조회 구조체
typedef struct _GTX_PK_V11_GETBALANCE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

    DWORD	UserNo;				// IN	5
	char    UserID[50+1];		// IN   6
	
	DWORD	RealCash;			// OUT	7
	DWORD	BonusCash;			// OUT	8
	char	RetMsg[256+1];		// OUT  9
} GTX_PK_V11_GETBALANCE, *PGTX_PK_V11_GETBALANCE;

// 정책이벤트캐시 지급 구조체
typedef struct _GTX_PK_V11_INSPOLICYEVENT
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

    DWORD	UserNo;				// IN	5
	char	UserID[50+1];		// IN   6
	DWORD	ClientIP;			// IN	7
    DWORD	CashAmt;			// IN	8
	DWORD	EventID;			// IN   9
	char	ExpireYMD[8+1];		// IN	10
	
	DWORD	RealCash;			// OUT	11
	DWORD	BonusCash;			// OUT	12
	DWORD	RetCashAmt;			// OUT	13
	char	RetExpireYMD[8+1];	// OUT	14
	char	RetMsg[256+1];		// OUT  15
} GTX_PK_V11_INSPOLICYEVENT, *PGTX_PK_V11_INSPOLICYEVENT;

// 아이템 구매 구조체
typedef struct _GTX_PK_V11_PURCHASEITEM
{
	WORD	ReqLen;					// IN	1
	WORD	ReqType;				// IN	2
	DWORD	ReqKey;					// IN	3
	WORD	RetCode;				// OUT	4

    DWORD	UserNo;					// IN	5
	DWORD	ClientIP;				// IN	6
	char	ItemID[256+1];			// IN	7
	char	ItemCnt[128+1];			// IN	8
	char	ItemUnitPrice[256+1];	// IN	9
	char	CouponDCPrice[256+1];	// IN	10
	char	ChargePrice[256+1];		// IN	11
	WORD	GameServerNo;			// IN	12
	DWORD	WorldNo;				// IN	13
	char	UserID[50+1];			// IN	14
    char    CharacterID[50+1];		// IN	15    
											
	DWORD	RealCash;				// OUT	16
	DWORD	BonusCash;				// OUT	17
	DWORD	ChargedCashAmt;			// OUT	18
	char	ChargeNo[512+1];		// OUT	19
	char	EventItemID[256+1];		// OUT  20
	char	EventChargeNo[512+1];	// OUT  21
	char	RetMsg[256+1];			// OUT  22
} GTX_PK_V11_PURCHASEITEM, *PGTX_PK_V11_PURCHASEITEM;

// 아이템 선물 구조체
typedef struct _GTX_PK_V11_GIFTITEM
{
	WORD	ReqLen;					// IN	1
	WORD	ReqType;				// IN	2
	DWORD	ReqKey;					// IN	3
	WORD	RetCode;				// OUT	4

    DWORD	UserNo;					// IN	5
	DWORD	RUserNo;				// IN	6
	DWORD	ClientIP;				// IN	7
	char	ItemID[256+1];			// IN	8
	char	ItemCnt[128+1];			// IN	9
	char	ItemUnitPrice[256+1];	// IN	10
	char	CouponDCPrice[256+1];	// IN	11
	char	ChargePrice[256+1];		// IN	12
	WORD	GameServerNo;			// IN	13
	DWORD	WorldNo;				// IN	14
	char	UserID[50+1];			// IN	15
    char    CharacterID[50+1];		// IN	16
	char    RUserID[50+1];			// IN	17
	char    RCharacterID[50+1];		// IN	18

	DWORD	RealCash;				// OUT	19
	DWORD	BonusCash;				// OUT	20
	DWORD	ChargedCashAmt;			// OUT	21
	char	ChargeNo[512+1];		// OUT	22
	char	EventItemID[256+1];		// OUT  23
	char	EventChargeNo[512+1];	// OUT  24
	char    RetMsg[256+1];			// OUT  25
} GTX_PK_V11_GIFTITEM, *PGTX_PK_V11_GIFTITEM;

// 구매취소 구조체
typedef struct _GTX_PK_V11_CNLPURCHASE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

    DWORD	UserNo;				// IN	5
	char	UserID[50+1];		// IN	6
	char	ChargeNo[512+1];	// IN	7

	DWORD	RealCash;			// OUT	8
	DWORD	BonusCash;			// OUT	9
	DWORD	CanceledCashAmt;	// OUT	10
	char	RetMsg[256+1];		// OUT  11
} GTX_PK_V11_CNLPURCHASE, *PGTX_PK_V11_CNLPURCHASE;

// 쿠폰등록 구조체
typedef struct _GTX_PK_V11_REGCOUPON
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

    DWORD	UserNo;				// IN	5
	DWORD	ClientIP;			// IN	6
	WORD	GameServerNo;		// IN	7
	DWORD	WorldNo;			// IN	8
	char	UserID[50+1];		// IN	9
    char    CharacterID[50+1];	// IN	10
	char	CouponNo[19+1];		// IN	11

	DWORD	RealCash;			// OUT	12
	DWORD	BonusCash;			// OUT	13
	char	ItemID[20+1];		// OUT	14
	WORD	ItemCnt;			// OUT	15
	char	ChargeNo[20+1];		// OUT	16
	char	RetMsg[256+1];		// OUT	17
} GTX_PK_V11_REGCOUPON, *PGTX_PK_V11_REGCOUPON;

// 쿠폰 등록 취소 구조체
typedef struct _GTX_PK_V11_CNLCOUPON
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

    DWORD	UserNo;				// IN	5
	char	UserID[50+1];		// IN   6
	char	CouponNo[19+1];		// IN	7

	char	RetMsg[256+1];		// OUT	8
} GTX_PK_V11_CNLCOUPON, *PGTX_PK_V11_CNLCOUPON;

#define GTX_PK_V11_HEALTH_CHECK_SZ				sizeof(GTX_PK_V11_HEALTH_CHECK)
#define GTX_PK_V11_GETBALANCE_SZ				sizeof(GTX_PK_V11_GETBALANCE)
#define GTX_PK_V11_INSPOLICYEVENT_SZ			sizeof(GTX_PK_V11_INSPOLICYEVENT)
#define GTX_PK_V11_PURCHASEITEM_SZ				sizeof(GTX_PK_V11_PURCHASEITEM)
#define GTX_PK_V11_GIFTITEM_SZ					sizeof(GTX_PK_V11_GIFTITEM)
#define GTX_PK_V11_CNLPURCHASE_SZ				sizeof(GTX_PK_V11_CNLPURCHASE)
#define GTX_PK_V11_REGCOUPON_SZ					sizeof(GTX_PK_V11_REGCOUPON)
#define GTX_PK_V11_CNLCOUPON_SZ					sizeof(GTX_PK_V11_CNLCOUPON)

#endif
