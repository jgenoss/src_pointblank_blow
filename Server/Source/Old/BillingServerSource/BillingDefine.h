#ifndef __BILLING_PROTOCOL_H__
#define __BILLING_PROTOCOL_H__

#define MAX_ID_SIZE				100

enum Protocol_Billing
{
	PROTOCOL_BILLING				= 0x0100,

	PROTOCOL_BILLING_GET_CASH_REQ,
	PROTOCOL_BILLING_GET_CASH_ACK,

	PROTOCOL_BILLING_ITEM_BUY_REQ,
	PROTOCOL_BILLING_ITEM_BUY_ACK,
};

enum RETURN_CODE
{
	RETURN_CODE_SUCCESS					= 0x00000000,
	RETURN_CODE_ERROR					= 0x80000000,
	RETURN_CODE_ERROR_SYSTEM,
	RETURN_CODE_ERROR_NOT_FIND_ID,

	RETURN_CODE_MAX,
};

#pragma pack( push, 1 )

typedef struct __tag_BillingGetCashReq
{
	INT32		_i32IDSize;
	char		_strID[ MAX_ID_SIZE ];
}TYPE_BILLING_GET_CASH_REQ;

typedef struct __tag_BillingGetCashAck
{
	INT32		_i32IDSize;
	char		_strID[ MAX_ID_SIZE ];
	INT32		_i32ReturnCode;
	UINT64		_ui64Cash;
}TYPE_BILLING_GET_CASH_ACK;

typedef struct __tag_BillingItemBuyReq
{
	INT32		_i32IDSize;
	char		_strID[ MAX_ID_SIZE ];
	UINT32		_ui32Price;
}TYPE_BILLING_ITEM_BUY_REQ;

typedef struct __tag_BillingItemBuyAck
{
	INT32		_i32IDSize;
	char		_strID[ MAX_ID_SIZE ];
	INT32		_i32ReturnCode;
	UINT64		_ui64Cash;
}TYPE_BILLING_ITEM_BUY_ACK;

#pragma pack( pop )

#endif