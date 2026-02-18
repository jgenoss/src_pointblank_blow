////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareCommonServerPacketFunction.cpp
//	Author:
//		Jiwon Kang.
//	Description:
//		Make the common protocol function in All Server
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Base
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void MakePacket_OCommon_Base_Heart_Bit_Req(OCommon_Base_Heart_Bit_Req *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}

//void MakePacket_OCommon_Base_Heart_Bit_Ack(OCommon_Base_Heart_Bit_Ack *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				All Server
//				enum Protocol_Asc
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////void MakePacket_OCommon_Asc_Confirmation_Req(OCommon_Asc_Confirmation_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OCommon_Asc_Confirmation_Req(i3NetworkPacket	*pOutPacket,
//                                                    INT32			iConnectType,
//													UINT8			iServerType)
//{	
//	pOutPacket->WriteData(&iConnectType, sizeof(INT32)); 	
//	pOutPacket->WriteData(&iServerType, sizeof(UINT8)); 
//}
//
//void MakePacket_OCommon_Asc_Confirmation_Ack(OCommon_Asc_Confirmation_Ack *pOutPacket, i3NetworkPacket* pInPacket)
//{
//	pInPacket->ReadData( &pOutPacket->, sizeof(UINT32));	
//	g_pConfig->m_pServerIp = (UINT32*)i3MemAlloc( sizeof(UINT32) * wServerCount );
//	pInPacket->ReadData( g_pConfig->m_pServerIp, sizeof(UINT32) * wServerCount );
//}

//void MakePacket_OCommon_Asc_State_Req(i3NetworkPacket	*pOutPacket,
//											 INT32				iCpuQuery,
//											 INT32				iHandleQuery,
//											 INT32				ConnectedGameServerCount,
//											 INT32				UserCount)
//{
//	pOutPacket->WriteData( &iCpuQuery,					sizeof(INT32) ); 
//	pOutPacket->WriteData( &iHandleQuery,				sizeof(INT32) ); 				
//	pOutPacket->WriteData( &ConnectedGameServerCount,	sizeof(INT32) ); 				
//	pOutPacket->WriteData( &UserCount,					sizeof(INT32) ); 	
//}

//void MakePacket_OCommon_Asc_State_Ack(OCommon_Asc_State_Ack *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}




