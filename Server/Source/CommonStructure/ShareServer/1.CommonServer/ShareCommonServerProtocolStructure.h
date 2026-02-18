////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareCommonServerProtocolStructure.h
//	Author:
//		Ji-won Kang.
//	Description:
//		Structures used All Server
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_COMMONSERVER_PROTOCOL_STRUCTURE_H__
#define __SHARE_COMMONSERVER_PROTOCOL_STRUCTURE_H__


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Base
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//struct OCommon_Base_Heart_Bit_Req
//{
//
//}; // PROTOCOL_BASE_HEART_BIT_REQ,					// Heart Bit

//struct OCommon_Base_Heart_Bit_Ack
//{
//
//}; // PROTOCOL_BASE_HEART_BIT_ACK,	


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Asc
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//struct OCommon_Asc_Confirmation_Req
//{
//	
//}; // PROTOCOL_ASC_CONFIRMATION_REQ,					//컨넥후 처음 던지는 메시지 자신이 서버 인지, 유저인지를 보냄.
//
//struct OCommon_Asc_Confirmation_Ack
//{
//	
//};// PROTOCOL_ASC_CONFIRMATION_ACK,

struct OCommon_Asc_State_Req
{
	INT32			iCpuQuery;
	INT32			iHandleQuery;
	INT32			ConnectedGameServerCount;
	INT32			UserCount;

};// PROTOCOL_ASC_STATE_REQ,							//상태 보내기 (서버용)

//struct OCommon_Asc_State_Ack
//{
//
//};// PROTOCOL_ASC_STATE_ACK,							//상태 요구하기(관리자툴용)


#endif // __SHARE_COMMONSERVER_PROTOCOL_STRUCTURE_H__