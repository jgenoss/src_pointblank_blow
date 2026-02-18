#ifndef __PAY_LETTER_H__
#define __PAY_LETTER_H__

// Network Byte Order 적용하는 경우 아래처럼 define한다.
// Network Byte Order 사용하지 않는 경우 Comment 처리해야 한다.

//================= Network Byte Order !!!!! ==================//
#define	APPLY_BYTE_ORDERING
//================= Network Byte Order !!!!! ==================//

// 게임서버/빌링GTX서버 상호 명령어 정의
// Packet Type 정의
#define GTX_PK_V09_TYPE_GETBALANCE					10	// 잔액조회 요청/응답
#define GTX_PK_V09_TYPE_PURCHASEITEM				20	// 아이템구매 
#define GTX_PK_V09_TYPE_CNLPURCHASE					40	// 아이템구매 취소
#define	GTX_PK_V09_TYPE_REGCOUPON					50	// 쿠폰 등록 요청
#define	GTX_PK_V09_TYPE_CNLCOUPON					60	// 쿠폰 등록 취소 요청
#define GTX_PK_V09_TYPE_HEALTH_CHECK				70	// GTX Health Check

// 동일 내용이지만 버전 구분을 위해 별도로 define합니다
#define GTX_PK_V12_TYPE_GETBALANCE					10	// 잔액조회 요청/응답
#define GTX_PK_V12_TYPE_PURCHASEITEM				20	// 아이템구매 
#define GTX_PK_V12_TYPE_CNLPURCHASE					40	// 아이템구매 취소
#define	GTX_PK_V12_TYPE_REGCOUPON					50	// 쿠폰 등록 요청
#define	GTX_PK_V12_TYPE_CNLCOUPON					60	// 쿠폰 등록 취소 요청
#define GTX_PK_V12_TYPE_HEALTH_CHECK				70	// GTX Health Check

// GTX 프로토콜
#pragma pack( push, 1 )

// Health Check 구조체
typedef struct _GTX_PK_V12_HEALTH_CHECK
{
	WORD	ReqLen;		// IN	1
	WORD	ReqType;	// IN	2
	DWORD	ReqKey;		// IN	3
	WORD	RetCode;	// OUT	4
} GTX_PK_V12_HEALTH_CHECK, *PGTX_PK_V12_HEALTH_CHECK;

// 잔액조회 구조체
typedef struct _GTX_PK_V12_GETBALANCE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

	char	UserNo[128+1];		// IN	5
	DWORD	RealCash;			// OUT	6
	DWORD	BonusCash;			// OUT	7
	DWORD   Mileage;			// OUT  8
	__int64	GameUID;			// OUT  9
	WORD	SiteCode;			// IN   10
} GTX_PK_V12_GETBALANCE, *PGTX_PK_V12_GETBALANCE;


// 아이템 구매 구조체
typedef struct _GTX_PK_V12_PURCHASEITEM
{
	WORD	ReqLen;					// IN	1
	WORD	ReqType;				// IN	2
	DWORD	ReqKey;					// IN	3
	WORD	RetCode;				// OUT	4

	char	UserNo[128 + 1];		// IN	5
	DWORD	ClientIP;				// IN	6
	DWORD	ItemID;					// IN	7
	WORD	ItemCnt;				// IN	8
	char	ItemName[100 + 1];		// IN	9
	DWORD	ChargeAmt;				// IN   10

	DWORD	RealCash;				// OUT	11
	DWORD	BonusCash;				// OUT	12
	DWORD	Mileage;				// OUT  13 
	DWORD	ChargedAmt;				// OUT	14
	__int64	ChargeNo;				// OUT	15
	WORD	SiteCode;				// IN   16
} GTX_PK_V12_PURCHASEITEM, *PGTX_PK_V12_PURCHASEITEM;


// 구매취소 구조체
typedef struct _GTX_PK_V12_CNLPURCHASE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

	char	UserNo[20 + 1];		// IN	5
	__int64	ChargeNo;			// IN	6

	DWORD   CanceledAmt;		// OUT  7
	DWORD	RealCash;			// OUT	8
	DWORD	BonusCash;			// OUT	9
	DWORD	Mileage;			// OUT	10
	WORD	SiteCode;			// IN   11
} GTX_PK_V12_CNLPURCHASE, *PGTX_PK_V12_CNLPURCHASE;

// 쿠폰 등록 요청 구조체
typedef struct _GTX_PK_V12_REGCOUPON
{
	WORD		ReqLen;				// IN	메시지길이(sizeof(GTX_PK_REGISTERCOUPON))
	WORD		ReqType;			// IN	메시지ID(50: 쿠폰 등록 요청)
	DWORD		ReqKey;				// IN	요청메시지 고유번호(메시지번호)
	WORD		RetCode;			// IN/OUT	요청처리결과(0: 성공)

	char		UID[128+1];			// IN	사용자의 고유키 UID는 AES-128로 암호화되어 전송됨.
	char		UserID[50+1];		// IN	사용자 아이디
	char		UserName[100+1];	// IN	사용자 명
	char		BirthDay[8+1];		// IN	생년월일(YYYYMMDD)
	char		EmailAddr[100+1];	// IN	사용자 이메일

	char		MemberRegDate[19+1];// IN	멤버수비 가입일(ex:2014-03-18 13:44:00)
	char		CharacterID[50+1];	// IN	사용자의 현재 게임 캐릭터 명. 없을 수도 있음.(없을 때는 null string)
	DWORD		ClientIP;			// IN	사용자 PC의 IP주소
	char		CouponNo[19+1];		// IN	쿠폰번호(XXXX-XXXX-XXXX-XXXX 형식) "-"는 보내도 되고 안 보내도 됨.
	WORD        UseLocation;		// IN	쿠폰사용처(0: ALL, 2: PC방) *PC방인 경우는 2로, 나머지 경우는 모두 0으로 설정. 추후 코드가 추가될 경우 별도 공지.

	DWORD		CashReal;			// OUT	사용자의 실캐시 잔액

	DWORD		CashBonus;			// OUT	사용자의 보너스캐시 잔액
	__int64		BillKeyNo;			// OUT	쿠폰이 정상 등록되어 지급된 캐시번호 또는 아이템 지급에 대한 과금번호(CouponType=1인 경우 캐시번호,CouponType=2인 경우 과금번호)
	WORD		CouponType;			// OUT	쿠폰의 종류(1-무료 캐시 지급, 2-무료 아이템 지급)
	DWORD		CashAmt;			// OUT	CouponType=1인 경우 지급된 무료 캐시금액
	char		GameItemID[256+1];	// OUT	CouponType=2인 경우 지급된 GameItemID(지급되는 아이템이 복수개일 경우 "^"로 구분)

	char		GameItemCnt[256+1];	// OUT	CouponType=2인 경우 지급된 GameItemID의 개수(지급되는 아이템이 복수개일 경우 "^"로 구분)
	WORD		SiteCode;			// IN	사이트코드 (1:PB, 2:STEAM)
} GTX_PK_V12_REGCOUPON, *PGTX_PK_V12_REGCOUPON;

// 쿠폰 등록 취소 요청 구조체
typedef struct _GTX_PK_V12_CNLCOUPON
{
	WORD		ReqLen;				// IN	메시지길이(sizeof(GTX_PK_CANCELCOUPON))
	WORD		ReqType;			// IN	메시지ID(60:쿠폰 등록 취소요청)
	DWORD		ReqKey;				// IN	요청메시지 고유번호(메시지번호)
	WORD		RetCode;			// IN/OUT	요청처리결과(0:성공)

	char		UID[128+1];			// IN	사용자의 고유키(UID는 AES-128로 암호화되어 전송됨)
	char		CouponNo[19+1];		// IN	등록 취소할 쿠폰 번호(XXXX-XXXX-XXXX-XXXX 형식) "-"는 보내도 되고 안 보내도 됨.
	WORD		SiteCode;			// IN	사이트코드 (1:PB, 2:STEAM)
} GTX_PK_V12_CNLCOUPON, *PGTX_PK_V12_CNLCOUPON;

//////////////////////////////////////////////////////////////////////////
//			기존 페이레터 연동용 패킷 구조체								//
//////////////////////////////////////////////////////////////////////////

// 쿠폰 등록 요청 구조체
typedef struct _GTX_PK_REGCOUPON
{
	WORD		ReqLen;					// IN	메시지길이(sizeof(GTX_PK_REGISTERCOUPON))
	WORD		ReqType;				// IN	메시지ID(50: 쿠폰 등록 요청)
	DWORD		ReqKey;					// IN	요청메시지 고유번호(메시지번호)
	WORD		RetCode;				// IN/OUT	요청처리결과(0: 성공)

	char		UID[128 + 1];			// IN	사용자의 고유키 UID는 AES-128로 암호화되어 전송됨.
	char		UserID[50 + 1];			// IN	사용자 아이디
	char		UserName[100 + 1];		// IN	사용자 명
	char		BirthDay[8 + 1];		// IN	생년월일(YYYYMMDD)
	char		EmailAddr[100 + 1];		// IN	사용자 이메일

	char		MemberRegDate[19 + 1];	// IN	멤버수비 가입일(ex:2014-03-18 13:44:00)
	char		CharacterID[50 + 1];	// IN	사용자의 현재 게임 캐릭터 명. 없을 수도 있음.(없을 때는 null string)
	DWORD		ClientIP;				// IN	사용자 PC의 IP주소
	char		CouponNo[19 + 1];		// IN	쿠폰번호(XXXX-XXXX-XXXX-XXXX 형식) "-"는 보내도 되고 안 보내도 됨.
	WORD        UseLocation;			// IN	쿠폰사용처(0: ALL, 2: PC방) *PC방인 경우는 2로, 나머지 경우는 모두 0으로 설정. 추후 코드가 추가될 경우 별도 공지.

	DWORD		CashReal;				// OUT	사용자의 실캐시 잔액

	DWORD		CashBonus;				// OUT	사용자의 보너스캐시 잔액
	__int64		BillKeyNo;				// OUT	쿠폰이 정상 등록되어 지급된 캐시번호 또는 아이템 지급에 대한 과금번호(CouponType=1인 경우 캐시번호,CouponType=2인 경우 과금번호)
	WORD		CouponType;				// OUT	쿠폰의 종류(1-무료 캐시 지급, 2-무료 아이템 지급)
	DWORD		CashAmt;				// OUT	CouponType=1인 경우 지급된 무료 캐시금액
	char		GameItemID[256 + 1];	// OUT	CouponType=2인 경우 지급된 GameItemID(지급되는 아이템이 복수개일 경우 "^"로 구분)

	char		GameItemCnt[256 + 1];	// OUT	CouponType=2인 경우 지급된 GameItemID의 개수(지급되는 아이템이 복수개일 경우 "^"로 구분)
	WORD		SiteCode;				// IN	사이트코드 (1:PB, 2:STEAM)
} GTX_PK_REGCOUPON, *PGTX_PK_REGCOUPON;

// 쿠폰 등록 취소 요청 구조체
typedef struct _GTX_PK_CNLCOUPON
{
	WORD		ReqLen;				// IN	메시지길이(sizeof(GTX_PK_CANCELCOUPON))
	WORD		ReqType;			// IN	메시지ID(60:쿠폰 등록 취소요청)
	DWORD		ReqKey;				// IN	요청메시지 고유번호(메시지번호)
	WORD		RetCode;			// IN/OUT	요청처리결과(0:성공)

	char		UID[128 + 1];			// IN	사용자의 고유키(UID는 AES-128로 암호화되어 전송됨)
	char		CouponNo[19 + 1];		// IN	등록 취소할 쿠폰 번호(XXXX-XXXX-XXXX-XXXX 형식) "-"는 보내도 되고 안 보내도 됨.
	WORD		SiteCode;			// IN	사이트코드 (1:PB, 2:STEAM)
} GTX_PK_CNLCOUPON, *PGTX_PK_CNLCOUPON;

// 하트비트 구조체 ( 필리핀 때문에 추가 )
typedef struct _BOQGTX_HEALTH_CHECK
{
	unsigned short	ReqLen;
	unsigned short	ReqType;
	unsigned int	ReqKey;
	unsigned short	RetCode;
}
BOQGTX_HEALTH_CHECK, *PBOQGTX_HEALTH_CHECK;


// 잔액조회 구조체 - 터키
typedef struct _BOQGTX_GETBALANCE
{
	unsigned short	ReqLen;			// IN
	unsigned short	ReqType;		// IN	
	unsigned int	ReqKey;			// IN
	unsigned short	RetCode;		// IN/OUT

    char            UID[128+1];		// IN
	unsigned int    RealCash;		// OUT
	unsigned int    BonusCash;		// OUT
	unsigned int    Mileage;		// OUT
	unsigned __int64	GameUID;		// OUT
}
BOQGTX_GETBALANCE, *PBOQGTX_GETBALANCE;

// 아이템 구매 요청 - 터.
typedef struct _BOQGTX_PURCHASEITEM
{
	unsigned short	ReqLen;			// IN
	unsigned short	ReqType;		// IN
	unsigned int	ReqKey;			// IN
	unsigned short	RetCode;		// IN/OUT

    char     		UID[128+1];		// IN
	unsigned int	ClientIP;		// IN
	unsigned int	ItemID;			// IN
	unsigned short	ItemCnt;		// IN
	char	        ItemName[100+1];// IN
	unsigned int	ChargeAmt;		// IN
	
	unsigned int	RealCash;		// OUT
	unsigned int	BonusCash;		// OUT
	unsigned int	Mileage;		// OUT
	unsigned int	ChargedAmt;		// OUT
	__int64	        ChargeNo;		// OUT
}
BOQGTX_PURCHASEITEM, *PBOQGTX_PURCHASEITEM;

// 아이템 구매 취소 요청 - 터키.
typedef struct _BOQGTX_CANCELPURCHASE
{
	unsigned short	ReqLen;		// IN
	unsigned short	ReqType;	// IN
	unsigned int	ReqKey;		// IN
	unsigned short	RetCode;	// IN/OUT

	char	        UID[128+1];	// IN
	__int64	        ChargeNo;	// IN
	unsigned int	RealCash;	// OUT
	unsigned int	BonusCash;	// OUT
	unsigned int	Mileage;	// OUT
}
BOQGTX_CANCELPURCHASE, *PBOQGTX_CANCELPURCHASE;

///////////////// 중남미용 페이레터 구조체//////////////////

// Health Check 구조체
typedef struct _GTX_PK_V09_HEALTH_CHECK
{
	WORD	ReqLen;		// IN	1
	WORD	ReqType;	// IN	2
	DWORD	ReqKey;		// IN	3
	WORD	RetCode;	// OUT	4
} GTX_PK_V09_HEALTH_CHECK, *PGTX_PK_V09_HEALTH_CHECK;

// 잔액조회 구조체
typedef struct _GTX_PK_V09_GETBALANCE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

	char	UserNo[20 + 1];		// IN	5
	DWORD	RealCash;			// OUT	6
	DWORD	BonusCash;			// OUT	7
	DWORD   Mileage;			// OUT  8
} GTX_PK_V09_GETBALANCE, *PGTX_PK_V09_GETBALANCE;


// 아이템 구매 구조체
typedef struct _GTX_PK_V09_PURCHASEITEM
{
	WORD	ReqLen;					// IN	1
	WORD	ReqType;				// IN	2
	DWORD	ReqKey;					// IN	3
	WORD	RetCode;				// OUT	4

	char	UserNo[20 + 1];			// IN	5
	DWORD	ClientIP;				// IN	8
	DWORD	ItemID;					// IN	9
	WORD	ItemCnt;				// IN	10
	char	ItemName[100 + 1];		// IN	11
	DWORD	ChargeAmt;				// IN   12

	DWORD	RealCash;				// OUT	16
	DWORD	BonusCash;				// OUT	17
	DWORD	Mileage;				// OUT  18 
	DWORD	ChargedAmt;				// OUT	19
	__int64	ChargeNo;				// OUT	20
} GTX_PK_V09_PURCHASEITEM, *PGTX_PK_V09_PURCHASEITEM;


// 구매취소 구조체
typedef struct _GTX_PK_V09_CNLPURCHASE
{
	WORD	ReqLen;				// IN	1
	WORD	ReqType;			// IN	2
	DWORD	ReqKey;				// IN	3
	WORD	RetCode;			// OUT	4

	char	UserNo[20 + 1];		// IN	5
	__int64	ChargeNo;			// IN	6

	DWORD   CanceledAmt;
	DWORD	RealCash;			// OUT	7
	DWORD	BonusCash;			// OUT	8
	DWORD	Mileage;			// OUT	9
} GTX_PK_V09_CNLPURCHASE, *PGTX_PK_V09_CNLPURCHASE;

////////////////////////////////////////////////////////////

#pragma pack( pop )

// PK_HEAD_SIZE는 ReqLen 길이로 한다.
#define GTX_PK_V12_HEAD_SZ						(sizeof(WORD)*2)

#define GTX_PK_V12_HEALTH_CHECK_SZ				sizeof(GTX_PK_V12_HEALTH_CHECK)
#define GTX_PK_V12_GETBALANCE_SZ				sizeof(GTX_PK_V12_GETBALANCE)
#define GTX_PK_V12_PURCHASEITEM_SZ				sizeof(GTX_PK_V12_PURCHASEITEM)
#define GTX_PK_V12_CNLPURCHASE_SZ				sizeof(GTX_PK_V12_CNLPURCHASE)

#define GTX_PK_V09_HEAD_SZ						(sizeof(WORD)*2)

#define GTX_PK_V09_HEALTH_CHECK_SZ				sizeof(GTX_PK_V09_HEALTH_CHECK)
#define GTX_PK_V09_GETBALANCE_SZ				sizeof(GTX_PK_V09_GETBALANCE)
#define GTX_PK_V09_PURCHASEITEM_SZ				sizeof(GTX_PK_V09_PURCHASEITEM)
#define GTX_PK_V09_CNLPURCHASE_SZ				sizeof(GTX_PK_V09_CNLPURCHASE)

#define GTX_ERR_CODE_SUCC					0	// 성공
#define GTX_ERR_CODE_100					100	// 잔액 부족				(아이템 구매 요청에서만 쓰임)
#define GTX_ERR_CODE_200					200	// 존재하지 않는 사용자		(아이템 구매 요청에서만 쓰임, 잔액 조회 요청의 경우는 존재하지 않는 사용자라 하더라도 에러코드는 성공으로 전달되며 실캐시 및 보너스캐시의 잔액이 모두 0으로 설정된다)
#define GTX_ERR_CODE_210					210	// 존재하지 않는 과금번호	(아이템 구매 취소 요청에서만 쓰임)
#define GTX_ERR_CODE_211					211	// 존재하지 않는 쿠폰번호	(쿠폰 등록 요청에서만 쓰임)
#define GTX_ERR_CODE_212					212	// 이미 사용한 쿠폰번호		(쿠폰 등록 요청에서만 쓰임)
#define GTX_ERR_CODE_300					300	// 빌링 시스템 에러
#define GTX_ERR_CODE_400					400	// 빌링서버 Internal Error
#define GTX_ERR_CODE_401					401	// 빌링서버 Internal Error2(Packet Rejected)
#define GTX_ERR_CODE_500					500	// 이미 취소된 건입니다.
#define GTX_ERR_CODE_900					900 // 유효하지 않은 입력 값
#define GTX_ERR_CODE_UNKNOWN				999	// 알수없는 에러.



#endif
