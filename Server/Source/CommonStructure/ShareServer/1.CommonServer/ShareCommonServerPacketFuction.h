////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareComonServerPacketFunction.h
//	Author:
//		Jiwon Kang.
//	Description:
//		Make the Common Packet functions in All Server
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_COMMONSERVER_PACKET_FUNCTION_H__
#define __SHARE_COMMONSERVER_PACKET_FUNCTION_H__

#include "SharecommonServerProtocolStructure.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Base
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void MakePacket_OCommon_Base_Heart_Bit_Req(OCommon_Base_Heart_Bit_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OCommon_Base_Heart_Bit_Ack(OCommon_Base_Heart_Bit_Ack *pOutPacket, i3NetworkPacket* pInPacket);


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Asc
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void MakePacket_OCommon_Asc_Confirmation_Req(i3NetworkPacket	*pOutPacket,
//													INT32			iConnectType,
//													UINT8			iServerType);
//void MakePacket_OCommon_Asc_Confirmation_Ack(OCommon_Asc_Confirmation_Ack *pOutPacket, i3NetworkPacket* pInPacket);

//void MakePacket_OCommon_Asc_State_Req(i3NetworkPacket	*pOutPacket,
//											 INT32				iCpuQuery,
//											 INT32				iHandleQuery,
//											 INT32				ConnectedGameServerCount,
//											 INT32				UserCount);
//void MakePacket_OCommon_Asc_State_Ack(OCommon_Asc_State_Ack *pOutPacket, i3NetworkPacket* pInPacket);

#endif // __SHARE_COMMONSERVER_PACKET_FUNCTION_H__
