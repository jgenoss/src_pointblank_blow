#ifndef __BILLING_PROTOCOL_H__
#define __BILLING_PROTOCOL_H__

#define MAX_ID_SIZE				100
//
enum Protocol_Billing
{
	PROTOCOL_BILLING				= 0x1000,

	PROTOCOL_BILLING_CONNECT_REQ,
	PROTOCOL_BILLING_CONNECT_ACK,

	PROTOCOL_BILLING_HEART_BIT_REQ,
	PROTOCOL_BILLING_HEART_BIT_ACK,

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
	RETURN_CODE_ERROR_CASH_NOT_ENOUGH,

	RETURN_CODE_MAX,
};

#pragma pack( push, 1 )

typedef struct __tag_BillingConnectReq
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
}TYPE_BILLING_CONNECT_REQ;

typedef struct __tag_BillingConnectAck
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
}TYPE_BILLING_CONNECT_ACK;

typedef struct __tag_BillingHeartBitReq
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
}TYPE_BILLING_HEART_BIT_REQ;

typedef struct __tag_BillingHeartBitAcK
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
}TYPE_BILLING_HEART_BIT_ACK;

typedef struct __tag_BillingGetCashReq
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
	UINT8		_ui8IDSize;
	char		_strID[ MAX_ID_SIZE ];
	T_UID		_TUID;
}TYPE_BILLING_GET_CASH_REQ;

typedef struct __tag_BillingGetCashAck
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
	UINT8		_ui8IDSize;
	char		_strID[ MAX_ID_SIZE ];
	T_UID		_TUID;
	INT32		_i32ReturnCode;
	UINT64		_ui64Cash;
}TYPE_BILLING_GET_CASH_ACK;

typedef struct __tag_BillingItemBuyReq
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
	UINT8		_ui8IDSize;
	char		_strID[ MAX_ID_SIZE ];
	T_UID		_TUID;
	UINT32		_ui32Price;
	UINT32		_ui32MainBufferIdx;
}TYPE_BILLING_ITEM_BUY_REQ;

typedef struct __tag_BillingItemBuyAck
{
	//UINT16		_ui16PacketSize;
	//UINT16		_ui16PacketProtocol;
	UINT8		_ui8IDSize;
	char		_strID[ MAX_ID_SIZE ];
	T_UID		_TUID;
	INT32		_i32ReturnCode;
	UINT64		_ui64Cash;
	UINT32		_ui32MainBufferIdx;
}TYPE_BILLING_ITEM_BUY_ACK;

#pragma pack( pop )

#endif