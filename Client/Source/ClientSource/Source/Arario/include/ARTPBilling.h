#ifndef __ARTPBILLING_H__
#define __ARTPBILLING_H__

#include "AuthFunctor.h"
#include "ARTPCommon.h"
#include "ARTPBillingError.h"

class ArParam : public Param
{
public:
	ARTP_HEADER* m_pInfo;
	ArParam( ARTP_HEADER* pInfo ) : m_pInfo(pInfo)
	{		
	}

private:
	ArParam() :	m_pInfo(0)
	{		
	}
};

struct BillingPktIndex 
{
	enum {
		VALUE0,			//0..
		VALUE1,
		VALUE2,
		REQRET,
	};

	enum 
	{
		VALUE4 = REQRET,
		VALUE5,
	};

	enum {
		RCS_INDEX = VALUE5,
		RCS,
	};
};

struct BillingPktSubindex {
	enum {
		VALUE0 = 0,
		VALUE1 = 10,
		VALUE2,
		VALUE3,
		VALUE4,
	};

	enum 
	{
		VALUE5 = VALUE4,
		VALUE6,
	};
	
	enum {
		RCS_INDEX = VALUE6,
		RCS_ERROR, 
		RCS_GETRIOCASH,
		RCS_USERIOCASH,
		RCS_ROLLBACK,
		RCS_USERIOCASH_CF,
		RCS_USERIOCASH_CF_MULTIPLE,
	};

};

// align as 1byte
#include <PshPack1.h>

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! define length
//!
typedef struct tARTP_RCS_ROOT : public ARTP_HEADER
{
	enum
	{
		db_member_len = 32,
		db_item_len = 50,
		db_gamecode_len = 8,
		db_servercode_len = 8,
		db_hashcode_len = 32,
		db_paytype_len = 8,
		db_moneytype_len = 1,
	};

	//enum
	//{
	//	member_len = db_member_len + 1, 
	//	item_len = db_item_len + 1, 
	//	gamecode_len = db_gamecode_len + 1,
	//	servercode_len = db_servercode_len + 1,		
	//	reserved_len = 100,
	//	hashcode_len = db_hashcode_len + 1,		
	//	paytype_len = db_paytype_len + 1,
	//};

	enum
	{
		member_len = db_member_len, 
		item_len = db_item_len, 
		gamecode_len = db_gamecode_len,
		servercode_len = db_servercode_len,		
		reserved_len = 100,
		hashcode_len = db_hashcode_len,		
		paytype_len = db_paytype_len,
		moneytype_len = db_moneytype_len,
	};	

	// user info
	LONG m_usn;
	WCHAR m_memberID[member_len];
} ARTP_RCS_ROOT;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Get RioCash
//!
typedef struct tARTP_RCS_GETRIOCASH : public ARTP_RCS_ROOT
{

} ARTP_RCS_GETRIOCASH;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Use RioCash
//!
typedef struct tARTP_RCS_USERIOCASH  : public ARTP_RCS_ROOT
{	
	INT m_clientRemainder;
	WCHAR m_itemID[item_len];
	WCHAR m_gameCode[gamecode_len];
	WCHAR m_serverID[servercode_len];	
	LONGLONG m_characterID;	
	WCHAR m_reserved[reserved_len];	
} ARTP_RCS_USERIOCASH;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! roll back
//!
typedef struct tARTP_RCS_ROLLBACK  : public ARTP_RCS_ROOT
{	
	WCHAR m_hashcode[hashcode_len];
} ARTP_RCS_ROLLBACK;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Result from server
//!
typedef struct tARTP_RCS_RESULT: public ARTP_RCS_ROOT
{
	// result
	UINT m_result;
	UINT m_subResult;
} ARTP_RCS_RESULT;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Result from server
//!
typedef struct tARTP_RET_RCS_GETRIOCASH : public ARTP_RCS_RESULT
{	
	INT m_cash;	
} ARTP_RET_RCS_GETRIOCASH;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Result from server
//!
typedef struct tARTP_RET_RCS_USERIOCASH : public ARTP_RCS_RESULT
{		
	INT m_remainder;
	WCHAR m_itemID[item_len];	
	WCHAR m_hashcode[hashcode_len];
} ARTP_RET_RCS_USERIOCASH;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Result from rollback
//!
typedef struct tARTP_RET_RCS_ROLLBACK : public ARTP_RCS_RESULT
{
	INT m_cash;
	WCHAR m_hashcode[hashcode_len];
} ARTP_RET_RCS_ROLLBACK;

////========================================================================================================================////
////==============Start CF Block       =====================================================================================////
////========================================================================================================================////
//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Use RioCash2 FOR CF
//!
typedef struct tARTP_RCS_USERIOCASH_CF  : public ARTP_RCS_USERIOCASH
{
	__int64 nInvenSRL;
} ARTP_RCS_USERIOCASH_CF;

//+--------------------------------------------------------------------------------------------------------------------
//! use at rcs
//! Result from server (CF)
//!
typedef struct tARTP_RET_RCS_USERIOCASH_CF : public ARTP_RET_RCS_USERIOCASH
{
	__int64 nInvenSRL;
} ARTP_RET_RCS_USERIOCASH_CF;


// 2015.1.6 스마일게이트 요청 작업
// 기존 UseRioCash(캐쉬차감) 함수에 아이템 개수 배수 변수(INT m_item_multiple)를 추가하여 신규 함수와 신규 프로토콜을 생성 
// request function
// edit by shim kyu woong 
typedef struct tARTP_RCS_USERIOCASH_CF_MULTIPLE  : public ARTP_RCS_USERIOCASH
{
	 INT m_item_multiple;
} ARTP_RCS_USERIOCASH_CF_MULTIPLE ;

typedef struct tARTP_RET_RCS_USERIOCASH_CF_MULTIPLE : public ARTP_RET_RCS_USERIOCASH
{
	 INT m_item_multiple;
} ARTP_RET_RCS_USERIOCASH_CF_MULTIPLE;

////========================================================================================================================////
////==============End CF Block         =====================================================================================////
////========================================================================================================================////

#include <PopPack.h>

inline void makeRCSResultHeader( ARTP_RCS_ROOT& in, ARTP_RCS_RESULT& out )
{
	// set request info
	out.m_usn = in.m_usn;
#if _MSC_VER >= 1310 // if .net2003 or more higher version
	wmemcpy(out.m_memberID, in.m_memberID, ARTP_RCS_ROOT::member_len);
#else
	memcpy( (char*)out.m_memberID, (char*)in.m_memberID, ARTP_RCS_ROOT::member_len*sizeof(TCHAR) );
#endif //_MSC_VER

	// set default result code
	out.m_result = ARTPRET_OK;
	out.m_subResult = ARTPRET_OK;
}

#endif // __ARTPBILLING_H__