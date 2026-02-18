////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareGame2MessServerPacketFunction.h
//	Author:
//		Jiwon Kang.
//	Description:
//		Make the Packet functions between GameServer to MessengerServer.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_GAME2MESSSERVER_PACKET_FUNCTION_H__
#define __SHARE_GAME2MESSSERVER_PACKET_FUNCTION_H__

#include "ShareDB2MessServerProtocolStructure.h"
#include "ShareGame2MessServerProtocolStructure.h"
//#include "../../CommonStructure/ShareServer/GameMessegerServer/ShareGameMessengerServerProtocolStructure.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//-------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MakePacket_OGame2Mess_Note_Send_Req(OGame2Mess_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Note_Send_Ack(i3NetworkPacket *pOutPacket,
												INT32			iSessionIdx,
												INT64			iUID,
												INT32			iResult);

void MakePacket_OGame2Mess_Note_Send_Find_UID_Req(OGame2Mess_Note_Send_Find_UID_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Note_List_Req(OGame2Mess_Note_List_Req *pOutPacket);

void MakePacket_OGame2Mess_Note_List_Ack(i3NetworkPacket *pOutPacket,
												INT32			iSessionIdx,
												INT64			iUID,
												UINT8			curPage,
												UINT8			noteCount,
												OUT_NOTE_LIST	*pInNoteList);

void MakePacket_OGame2Mess_Note_Check_Readed_Req(OGame2Mess_Note_Check_Readed_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Note_Check_Readed_Ack(i3NetworkPacket *pOutPacket,												
														INT32			iSessionIdx,
														INT64			iUID,
														UINT8			iCount,
														INT32			*pList);

void MakePacket_OGame2Mess_Note_Delete_Req(OGame2Mess_Note_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Note_Delete_Ack(i3NetworkPacket	*pOutPacket,
												  INT32				iSessionIdx,
												  INT64				iUID,
												  INT32				iResult,
												  UINT8				iCount,
												  INT32				*pList);

//void MakePacket_OGame2Mess_Note_Receive_Req(OGame2Mess_Note_Receive_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Note_Receive_Ack(i3NetworkPacket *pOutPacket,
												   INT32			iSessionIdx,
												   INT64			iUID,
												   INT32			iResult,
												   NOTE_NODE		NoteNode);

void MakePacket_OGame2Mess_Login_Req(OGame2Mess_Login_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Login_Ack(OGame2Mess_Login_Ack *pOutPacket);
void MakePacket_OGame2Mess_Logout_Req(OGame2Mess_Logout_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Logout_Ack(OGame2Mess_Logout_Ack *pOutPacket);

void MakePacket_OGame2Mess_Enter_User_Req(OGame2Mess_Enter_User_Req *pOutPacketm, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Enter_User_Ack(i3NetworkPacket	*pOutPacket,
												 INT32				iSessionIdx,
												 INT64				iUID,
												 UINT32				ifriendCount
#if defined( USE_BLOCK_LIST )
												 , UINT32				ui32BlockCount
#endif
												 );

void MakePacket_OGame2Mess_Leave_User_Req(OGame2Mess_Leave_User_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Change_Rank_Req(OGame2Mess_Change_Rank_Req *pOutPacket, i3NetworkPacket* pInPacket);
	
void MakePacket_OGame2Mess_Clan_Note_Send_Req(OGame2Mess_Clan_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Clan_Note_Send_Ack(i3NetworkPacket *pOutPacket,
													 INT32	iSessionIdx,
													 INT64	iUID,
													 INT32	iResult);

void MakePacket_OGame2Mess_Clan_Group_Note_Send_Req(OGame2Mess_Clan_Group_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Clan_Group_Note_Send_Ack(i3NetworkPacket *pOutPacket,
														   INT32	iSessionIdx,
														   INT64	iUID,
														   INT32	iResult);

//void MakePacket_OGame2Mess_Change_Color_Nick_Req(OGame2Mess_Change_Color_Nick_Req *pOutPacket);
//void MakePacket_OGame2Mess_Change_Color_Nick_Ack(OGame2Mess_Change_Color_Nick_Ack *pOutPacket);
void MakePacket_OGame2Mess_Friend_Accept_Req(OGame2Mess_Friend_Accept_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Friend_Accept_Ack(i3NetworkPacket *pOutPacket,
													INT32			iSessionIdx,
													INT64			iUID,
													INT32			iResult);

void MakePacket_OGame2Mess_Friend_Insert_Req(OGame2Mess_Friend_Insert_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Friend_Insert_Ack(i3NetworkPacket *pOutPacket,
													INT32			iSessionIdx,
													INT64			iUID,
													INT32			iResult);

void MakePacket_OGame2Mess_Friend_Invited_Req(OGame2Mess_Friend_Invited_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Friend_Invited_Ack(i3NetworkPacket	*pOutPacket,
													 INT32				iSessionIdx,
													 INT64				iUID,
													 INT32				iResult);
void MakePacket_OGame2Mess_Friend_Invited_Request_Ack(i3NetworkPacket	*pOutPacket, 
															 INT32			 	iSessionIdx,
															 INT64			 	iUID,
															 UINT8			 	iFriendIdx);

void MakePacket_OGame2Mess_Friend_Delete_Req(OGame2Mess_Friend_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Friend_Delete_Ack(i3NetworkPacket *pOutPacket, 
													INT32			iSessionIdx,
													INT64			iUID,
													INT32			iResult);

void MakePacket_OGame2Mess_Friend_Room_Enter_Req(OGame2Mess_Friend_Room_Enter_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Friend_Room_Enter_Ack(i3NetworkPacket *pOutPacket);
void MakePacket_OGame2Mess_Friend_Room_Leave_Req(OGame2Mess_Friend_Room_Leave_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Friend_Room_Leave_Ack(i3NetworkPacket *pOutPacket);
void MakePacket_OGame2Mess_Reset_Friend_Info_Req(OGame2Mess_Reset_Friend_Info_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Reset_Friend_Info_Ack(OGame2Mess_Reset_Friend_Info_Ack *pOutPacket);
void MakePacket_OGame2Mess_KickUser_Req(OGame2Mess_KickUser_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_KickUser_Ack(OGame2Mess_KickUser_Ack *pOutPacket);
void MakePacket_OGame2Mess_Friend_Info_Change_Ack(i3NetworkPacket	*pOutPacket,
														 INT32				iSessionIdx,
														 INT64				iUID,
														 UINT8				iState,
														 UINT8				iFriendIdx,
														 UINT8				nicknameSize,
														 TTCHAR				*nickname,
														 FRIEND_INFO		info);

//void MakePacket_OGame2Mess_Friend_Info_Req(OGame2Mess_Friend_Info_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Friend_Info_Ack(i3NetworkPacket	*pOutPacket,												  
												  INT32				iSessionIdx,
												  INT64				iUID,
												  UINT8				iFriendCount,
												  FRIEND_NODE		*pFriendNode);

void MakePacket_OGame2Mess_Find_User_Req(OGame2Mess_Find_User_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Find_User_Ack(i3NetworkPacket *pOutPacket,
												INT32			iSessionIdx,
												INT64			iUID,
												INT32			iResult);

void MakePacket_OGame2Mess_Send_Whisper_Req(OGame2Mess_Send_Whisper_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Send_Whisper_Ack(i3NetworkPacket *pOutPacket,
												   INT32		iSessionIdx,
												   INT64		iUID,
												   INT32		iResult, 
												   TTCHAR 		*receiverNick,
												   UINT8		ui8NickColor);

void MakePacket_OGame2Mess_Send_Whisper_By_UID_Req(OGame2Mess_Send_Whisper_By_UID_Req *pOutPacket, i3NetworkPacket* pInPacket);

//void MakePacket_OGame2Mess_Recv_Whisper_Req(OGame2Mess_Recv_Whisper_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Recv_Whisper_Ack(i3NetworkPacket	*pOutPacket,
												   INT32			iReceiverSessionIdx,
												   INT64			iReceiverUID,
												   TTCHAR			*senderNick,
												   bool				bIsGMUser,
												   INT16			messageSize,
												   TTCHAR			*message,
												   UINT8			ui8NickColor);

void MakePacket_OGame2MessSendChannelNumReq(OGame2MessSendChannelNumReq	*pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_Alive_User_Req(OGame2Mess_Alive_User_Req *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Alive_User_Ack(OGame2Mess_Alive_User_Ack *pOutPacket, i3NetworkPacket* pInPacket);


// Block
void MakePacket_OGame2Mess_Reset_Block_Info_Req(OGame2Mess_Reset_Block_Info_Req *pOutPacket, i3NetworkPacket* pInPacket);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				일방적으로 서버가 보내는 메시지	
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void MakePacket_OGame2Mess_ConnectionSuccess(OGame2Mess_ConnectionSuccess *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_DisConnectionSuccess(OGame2Mess_DisConnectionSuccess *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Kick_Player(OGame2Mess_Kick_Player *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Kick_Battle_Player(OGame2Mess_Kick_Battle_Player *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Invited(OGame2Mess_Invited *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Error(OGame2Mess_Error *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Announce(OGame2Mess_Announce *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Friend_Invited(OGame2Mess_Friend_Invited *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Room_Destroy(OGame2Mess_Room_Destroy *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Change_Inventory(OGame2Mess_Change_Inventory *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_GameGuard_Error(OGame2Mess_GameGuard_Error *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_Event_Coupon(OGame2Mess_Event_Coupon *pOutPacket, i3NetworkPacket* pInPacket);
//void MakePacket_OGame2Mess_New_Event_CardSet(OGame2Mess_New_Event_CardSet *pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_Block_Info_Ack(i3NetworkPacket*			pOutPacket,												  
												  INT32				i32SessionIdx,
												  INT64				i64UID,
												  UINT8				ui8BlockCount,
												  BLOCK_NODE*		pBlockNode,
												   BLOCK_COMMENT*	pBlockComment);


void MakePacket_OGame2Mess_Block_Insert_Ack(i3NetworkPacket *pOutPacket, 
											 INT32			i32SessionIdx,
											 INT64			i64UID,
											 INT32			i32Result,
											 INT32			i32Idx,
											 UINT8			ui8Rank,
											 UINT8			ui8NickLength,
											 char*			strNick,
											 UINT8			ui8CommentLength,
											 char*			strComment);

void MakePacket_OGame2Mess_Block_Delete_Ack(i3NetworkPacket *pOutPacket, 
											 INT32			i32SessionIdx,
											 INT64			i64UID,
											 INT32			i32Result,
											 INT32			i32Idx);

void MakePacket_OGame2Mess_Block_Insert_Req(OGame2Mess_Block_Insert_Req *pOutPacket, i3NetworkPacket* pInPacket);
 
void MakePacket_OGame2Mess_Block_Delete_Req(OGame2Mess_Block_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_Reset_Block_Info_Req(OGame2Mess_Reset_Block_Info_Req *pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_Note_Send_Gift_Notice_Req(OGame2Mess_Note_Send_Gift_Notice_Req *pOutPacket, i3NetworkPacket* pInPacket);
void MakePacket_OGame2Mess_Note_Send_User_Gift_Req(OGame2Mess_Note_Send_Find_UID_Req *pOutPacket, i3NetworkPacket* pInPacket);


#endif // __SHARE_GAME2MESSSERVER_PACKET_FUNCTION_H__
