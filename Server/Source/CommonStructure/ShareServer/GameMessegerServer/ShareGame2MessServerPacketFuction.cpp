////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareGame2MessServerPacketFunction.cpp
//	Author:
//		Jiwon Kang.
//	Description:
//		Make the protocol function between Game Sever to Messenger Server.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "pch.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//---------------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				Nick으로 쪽지 보내기 요청
//---------------------------------------------------------------------------------
void MakePacket_OGame2Mess_Note_Send_Req(OGame2Mess_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof (OGame2Mess_Note_Send_Req));	

	pInPacket->ReadData(&pOutPacket->iSessionIdx,				sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,						sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->nicknameSize,				sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->messageSize,				sizeof(UINT8));
	pOutPacket->nicknameSize = MIN(pOutPacket->nicknameSize,	NET_NICK_NAME_SIZE);
	pOutPacket->messageSize	 = MIN(pOutPacket->messageSize,		NET_NOTE_SIZE);
	pInPacket->ReadData(pOutPacket->nickname,					sizeof(TTCHAR) * pOutPacket->nicknameSize);
	pInPacket->ReadData(pOutPacket->message,					sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->nickname[pOutPacket->nicknameSize - 1] = '\0';
	pOutPacket->message[pOutPacket->messageSize	- 1] = '\0';
}

void MakePacket_OGame2Mess_Note_Send_Ack(i3NetworkPacket*	pOutPacket,
										 INT32				iSessionIdx,
										 INT64				iUID,
										 INT32				iResult)
{	
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iUID,			sizeof(INT64));
	pOutPacket->WriteData(&iResult,			sizeof(INT32));
}

void MakePacket_OGame2Mess_Note_Send_Find_UID_Req(OGame2Mess_Note_Send_Find_UID_Req *pOutPacket, i3NetworkPacket* pInPacket)
{	
	memset(pOutPacket, 0, sizeof (OGame2Mess_Note_Send_Find_UID_Req));	

	pInPacket->ReadData(&pOutPacket->iSessionIdx,				sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,						sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iReceiverUID,				sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->messageSize,				sizeof(UINT8));
	pInPacket->ReadData(pOutPacket->message,					sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->message[pOutPacket->messageSize	- 1] = '\0';
}

//void MakePacket_OGame2Mess_Note_List_Req(OGame2Mess_Note_List_Req *pOutPacket);

void MakePacket_OGame2Mess_Note_List_Ack(i3NetworkPacket*	pOutPacket,
										 INT32				iSessionIdx,
										 INT64				iUID,
										 UINT8				curPage,
										 UINT8				noteCount,
										 OUT_NOTE_LIST*		pInNoteList)
{
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iUID,			sizeof(INT64));
	pOutPacket->WriteData(&curPage,			sizeof(UINT8));
	pOutPacket->WriteData(&noteCount,		sizeof(UINT8));

	//Write Head
	for(INT32 i = 0; i < noteCount; i++)
	{
		INT32 idx = (curPage * MAX_NOTE_COUNT_PER_PAGE) + i;
		pOutPacket->WriteData(&pInNoteList->stNode[idx]._head, sizeof(NOTE_HEADER));
	}

	//Write Body 
	for(INT32 i = 0; i < noteCount; i++)
	{
		INT32 idx = (curPage * MAX_NOTE_COUNT_PER_PAGE) + i;

		pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._senderSize,	sizeof(UINT8));
		pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._messageSize, sizeof(UINT8));
		pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._sender,		sizeof(TTCHAR) * pInNoteList->stNode[idx]._body._senderSize);

		if( 0 < pInNoteList->stNode[idx]._body._messageSize )
		{
			pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._message, sizeof(TTCHAR) * pInNoteList->stNode[idx]._body._messageSize);
		}
		else
		{
			pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._message[0], sizeof(TTCHAR));
			pOutPacket->WriteData(&pInNoteList->stNode[idx]._body._message[1], sizeof(TTCHAR) * pInNoteList->stNode[idx]._body._message[0]);
		}
	}

}

void MakePacket_OGame2Mess_Note_Check_Readed_Req(OGame2Mess_Note_Check_Readed_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Note_Check_Readed_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iCount,		sizeof(UINT8));
	pOutPacket->iCount = MIN(pOutPacket->iCount, MAX_NORMAL_NOTE_COUNT);
	pInPacket->ReadData(pOutPacket->pList, sizeof(INT32) * pOutPacket->iCount);

}

void MakePacket_OGame2Mess_Note_Check_Readed_Ack(i3NetworkPacket	*pOutPacket,	
														INT32			iSessionIdx,
														INT64			iUID,
														UINT8			iCount,
														INT32			*pList)
{
	pOutPacket->WriteData(&iSessionIdx,	sizeof(INT32));
	pOutPacket->WriteData(&iUID,		sizeof(INT64));
	pOutPacket->WriteData(&iCount,		sizeof(UINT8));
	pOutPacket->WriteData(pList,		sizeof(INT32) * iCount);
}

void MakePacket_OGame2Mess_Note_Delete_Req(OGame2Mess_Note_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Note_Delete_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iCount,		sizeof(UINT8));
	pOutPacket->iCount = MIN(pOutPacket->iCount, MAX_NORMAL_NOTE_COUNT);
	pInPacket->ReadData(pOutPacket->pList,			sizeof(INT32) * pOutPacket->iCount);

}

void MakePacket_OGame2Mess_Note_Delete_Ack(i3NetworkPacket*	pOutPacket,
										   INT32			iSessionIdx,
										   INT64			iUID,
									 	   INT32			iResult,
										   UINT8			iCount,
										   INT32			*pList)
{
	pOutPacket->WriteData(&iSessionIdx,	sizeof(INT32));
	pOutPacket->WriteData(&iUID,		sizeof(INT64));
	pOutPacket->WriteData(&iResult,		sizeof(INT32));
	pOutPacket->WriteData(&iCount,		sizeof(UINT8));
	pOutPacket->WriteData(pList,		sizeof(INT32) * iCount);

}

void MakePacket_OGame2Mess_Note_Receive_Ack(i3NetworkPacket*	pOutPacket,
										    INT32				iSessionIdx,
										    INT64				iRecvUID,
										    INT32				iResult,
										    NOTE_NODE			NoteNode)
{
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iRecvUID,		sizeof(INT64));
	pOutPacket->WriteData(&iResult,			sizeof(INT32));			

	// 쪽지내용 추가
	// NOTE_NODE
	pOutPacket->WriteData(&NoteNode,		sizeof(NOTE_NODE));			

}

void MakePacket_OGame2Mess_Login_Req(OGame2Mess_Login_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Login_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(pOutPacket->pNickName,		NET_NICK_NAME_SIZE);
	pInPacket->ReadData(&pOutPacket->iRank,			sizeof(UINT32));
	pOutPacket->pNickName[NET_NICK_NAME_SIZE - 1] = '\0';
}

//void MakePacket_OGame2Mess_Login_Ack(OGame2Mess_Login_Ack *pOutPacket)
//{
//
//}

void MakePacket_OGame2Mess_Logout_Req(OGame2Mess_Logout_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Logout_Req));

	pInPacket->ReadData(&pOutPacket->iUID,	sizeof(INT64));

}

//void MakePacket_OGame2Mess_Logout_Ack(OGame2Mess_Logout_Ack *pOutPacket)
//{
//
//}

void MakePacket_OGame2Mess_Enter_User_Req(OGame2Mess_Enter_User_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Enter_User_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));

}

void MakePacket_OGame2Mess_Enter_User_Ack(i3NetworkPacket*	pOutPacket,
										  INT32				iSessionIdx,
										  INT64				iUID,
										  UINT32			ifriendCount
#if defined( USE_BLOCK_LIST )
										  , UINT32			ui32BlockCount
#endif
										  )
{
	pOutPacket->WriteData( &iSessionIdx,		sizeof( INT32 ));
	pOutPacket->WriteData( &iUID,				sizeof( INT64 ));
	pOutPacket->WriteData( &ifriendCount,		sizeof( UINT32 ));
#if defined( USE_BLOCK_LIST )
	pOutPacket->WriteData( &ui32BlockCount,		sizeof( UINT32 ));
#endif

}

void MakePacket_OGame2Mess_Leave_User_Req(OGame2Mess_Leave_User_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Leave_User_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));

}

//void MakePacket_OGame2Mess_Change_NickName_Ack(OGame2Mess_Change_NickName_Ack *pOutPacket)

void MakePacket_OGame2Mess_Change_Rank_Req(OGame2Mess_Change_Rank_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Change_Rank_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iRank,			sizeof(UINT32));
}

//void MakePacket_OGame2Mess_Change_Rank_Ack(OGame2Mess_Change_Rank_Ack *pOutPacket)

void MakePacket_OGame2Mess_Clan_Note_Send_Req(OGame2Mess_Clan_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof (OGame2Mess_Clan_Note_Send_Req));	

	pInPacket->ReadData(&pOutPacket->iSessionIdx,		sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,				sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->receiverUID,		sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->clanIdx,			sizeof(UINT32));
	pInPacket->ReadData(&pOutPacket->isAskNote,			sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->clanNameSize,		sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->messageSize,		sizeof(UINT8));

	pOutPacket->clanNameSize = MIN(pOutPacket->clanNameSize,	NET_CLAN_NAME_SIZE);
	pOutPacket->messageSize	 = MIN(pOutPacket->messageSize,		NET_NOTE_SIZE);			

	pInPacket->ReadData(pOutPacket->clanName,			sizeof(TTCHAR) * pOutPacket->clanNameSize);
	pOutPacket->clanName[pOutPacket->clanNameSize - 1] = '\0';

	//messageSize = 3;
	// Message가 0 이면 클랜관련 이름이다.
	if( 0 < pOutPacket->messageSize )
	{
		pInPacket->ReadData(pOutPacket->message,		sizeof(TTCHAR) * pOutPacket->messageSize);
		pOutPacket->message[pOutPacket->messageSize - 1] = '\0';
	}
	else
	{
		pInPacket->ReadData(&pOutPacket->message[0],	sizeof(TTCHAR));
		pInPacket->ReadData(&pOutPacket->message[1],	sizeof(TTCHAR) * pOutPacket->message[0]);
	}			

}


void MakePacket_OGame2Mess_Clan_Note_Send_Ack(i3NetworkPacket *pOutPacket,
											  INT32	iSessionIdx,
											  INT64	iUID,
											  INT32	iResult)
{	
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iUID,			sizeof(INT64));
	pOutPacket->WriteData(&iResult,			sizeof(INT32));

}

void MakePacket_OGame2Mess_Clan_Group_Note_Send_Req(OGame2Mess_Clan_Group_Note_Send_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof (OGame2Mess_Clan_Group_Note_Send_Req));	

	pInPacket->ReadData(&pOutPacket->iSessionIdx,		sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,				sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iCount,			sizeof(UINT32));
	pOutPacket->iCount = MIN(pOutPacket->iCount,		MAX_MEMBER_COUNT);

	pInPacket->ReadData(&pOutPacket->receiverUID,		sizeof(INT64) * pOutPacket->iCount);
	pInPacket->ReadData(&pOutPacket->clanIdx,			sizeof(UINT32));
	pInPacket->ReadData(&pOutPacket->isAskNote,			sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->clanNameSize,		sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->messageSize,		sizeof(UINT8));
	
	pOutPacket->clanNameSize = MIN(pOutPacket->clanNameSize,	NET_CLAN_NAME_SIZE);
	pOutPacket->messageSize	 = MIN(pOutPacket->messageSize,		NET_NOTE_SIZE);			

	pInPacket->ReadData(pOutPacket->clanName,			sizeof(TTCHAR) * pOutPacket->clanNameSize);
	pOutPacket->clanName[pOutPacket->clanNameSize - 1] = '\0';

	//messageSize = 3;
	// Message가 0 이면 클랜관련 이름이다.
	if( 0 < pOutPacket->messageSize )
	{
		pInPacket->ReadData(pOutPacket->message,		sizeof(TTCHAR) * pOutPacket->messageSize);
		pOutPacket->message[pOutPacket->messageSize - 1] = '\0';
	}
	else
	{
		pInPacket->ReadData(&pOutPacket->message[0],	sizeof(TTCHAR));
		pInPacket->ReadData(&pOutPacket->message[1],	sizeof(TTCHAR) * pOutPacket->message[0]);
	}			
}

void MakePacket_OGame2Mess_Clan_Group_Note_Send_Ack(i3NetworkPacket *pOutPacket,
												    INT32	iSessionIdx,
												    INT64	iUID,
												    INT32	iResult)
{	
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iUID,			sizeof(INT64));
	pOutPacket->WriteData(&iResult,			sizeof(INT32));

}

//void MakePacket_OGame2Mess_Change_Color_Nick_Req(OGame2Mess_Change_Color_Nick_Req *pOutPacket)
//{
//
//}

//void MakePacket_OGame2Mess_Change_Color_Nick_Ack(OGame2Mess_Change_Color_Nick_Ack *pOutPacket);

void MakePacket_OGame2Mess_Friend_Accept_Req(OGame2Mess_Friend_Accept_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Accept_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iFriendIdx,	sizeof(UINT8));

}

void MakePacket_OGame2Mess_Friend_Accept_Ack(i3NetworkPacket *pOutPacket,
											 INT32			iSessionIdx,
											 INT64			iUID,
											 INT32			iResult)
{
	pOutPacket->WriteData(&iSessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&iUID,			sizeof(INT64));
	pOutPacket->WriteData(&iResult,			sizeof(INT32));

}

void MakePacket_OGame2Mess_Friend_Insert_Req(OGame2Mess_Friend_Insert_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Insert_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,		sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,				sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->inData.ui8Rank,	sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->inData.ui8fRank,	sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->inData.ui8NickColor,	sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->inData.ui8fNickColor,	sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->nicknameSize,		sizeof(UINT8));
	if( pOutPacket->nicknameSize > NET_NICK_NAME_SIZE )	pOutPacket->nicknameSize = NET_NICK_NAME_SIZE;
	pInPacket->ReadData(pOutPacket->inData.strNick, sizeof(TTCHAR) * pOutPacket->nicknameSize);
	pOutPacket->inData.strNick[ NET_NICK_NAME_SIZE-1 ] = '\0';
}

void MakePacket_OGame2Mess_Friend_Insert_Ack(i3NetworkPacket *pOutPacket,
											 INT32			iSessionIdx,
											 INT64			iUID,
											 INT32			iResult)
{
	pOutPacket->WriteData(&iSessionIdx, sizeof(INT32));
	pOutPacket->WriteData(&iUID, sizeof(INT64));
	pOutPacket->WriteData(&iResult, sizeof(INT32));

}

void MakePacket_OGame2Mess_Friend_Invited_Req(OGame2Mess_Friend_Invited_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Invited_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iFriendIdx,	sizeof(UINT8));

}

void MakePacket_OGame2Mess_Friend_Invited_Ack(i3NetworkPacket	*pOutPacket,
											  INT32				iSessionIdx,
											  INT64				iUID,
											  INT32				iResult)
{
	pOutPacket->WriteData(&iSessionIdx, sizeof(INT32));
	pOutPacket->WriteData(&iUID, sizeof(INT64));
	pOutPacket->WriteData(&iResult, sizeof(INT32));
}

void MakePacket_OGame2Mess_Friend_Invited_Request_Ack(i3NetworkPacket	*pOutPacket, 
													  INT32			 	iSessionIdx,
													  INT64			 	iUID,
													  UINT8			 	iFriendIdx)
{
	pOutPacket->WriteData(&iSessionIdx, sizeof(INT32));
	pOutPacket->WriteData(&iUID, sizeof(INT64));
	pOutPacket->WriteData(&iFriendIdx, sizeof(UINT8));
}

void MakePacket_OGame2Mess_Friend_Delete_Req(OGame2Mess_Friend_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Delete_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx, sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID, sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iFriendIdx, sizeof(UINT8));			

}

void MakePacket_OGame2Mess_Friend_Delete_Ack(i3NetworkPacket *pOutPacket, 
											 INT32			iSessionIdx,
											 INT64			iUID,
											 INT32			iResult)
{
	pOutPacket->WriteData(&iSessionIdx, sizeof(INT32));
	pOutPacket->WriteData(&iUID, sizeof(INT64));
	pOutPacket->WriteData(&iResult, sizeof(INT32));

}

void MakePacket_OGame2Mess_Friend_Room_Enter_Req(OGame2Mess_Friend_Room_Enter_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Room_Enter_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->channel,		sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->roomIdx,		sizeof(UINT16));

}

//void MakePacket_OGame2Mess_Friend_Room_Enter_Ack(OGame2Mess_Friend_Room_Enter_Ack *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}

void MakePacket_OGame2Mess_Friend_Room_Leave_Req(OGame2Mess_Friend_Room_Leave_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Friend_Room_Leave_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));	

}

//void MakePacket_OGame2Mess_Friend_Room_Leave_Ack(OGame2Mess_Friend_Room_Leave_Ack *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}

void MakePacket_OGame2Mess_Reset_Friend_Info_Req(OGame2Mess_Reset_Friend_Info_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Reset_Friend_Info_Req));

	pInPacket->ReadData(&pOutPacket->iUID, sizeof(INT64));	

}

//void MakePacket_OGame2Mess_Reset_Friend_Info_Ack(OGame2Mess_Reset_Friend_Info_Ack *pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_KickUser_Req(OGame2Mess_KickUser_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_KickUser_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));	
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));

}

//void MakePacket_OGame2Mess_KickUser_Ack(OGame2Mess_KickUser_Ack *pOutPacket, i3NetworkPacket* pInPacket);

void MakePacket_OGame2Mess_Friend_Info_Change_Ack(i3NetworkPacket	*pOutPacket,
														 INT32				iSessionIdx,
														 INT64				iUID,
														 UINT8				iState,
														 UINT8				iFriendIdx,
														 UINT8				nicknameSize,
														 TTCHAR				*nickname,
														 FRIEND_INFO		info)
{
	pOutPacket->WriteData(&iSessionIdx,					sizeof(INT32));
	pOutPacket->WriteData(&iUID,						sizeof(INT64));
	pOutPacket->WriteData(&iState,						sizeof(UINT8));
	pOutPacket->WriteData(&iFriendIdx,					sizeof(UINT8));
	pOutPacket->WriteData(&nicknameSize,				sizeof(UINT8));
	pOutPacket->WriteData(nickname,						sizeof(TTCHAR) * nicknameSize);
	pOutPacket->WriteData(&info,						sizeof(FRIEND_INFO));

}

//void MakePacket_OGame2Mess_Friend_Info_Req(OGame2Mess_Friend_Info_Req *pOutPacket, i3NetworkPacket* pInPacket)
//{
//
//}

void MakePacket_OGame2Mess_Friend_Info_Ack(i3NetworkPacket*			pOutPacket,												  
												  INT32				i32SessionIdx,
												  INT64				i64UID,
												  UINT8				ui8FriendCount,
												  FRIEND_NODE*		pFriendNode)
{
	FRIEND_INFO info;

	pOutPacket->WriteData(&i32SessionIdx,			sizeof(INT32));
	pOutPacket->WriteData(&i64UID,					sizeof(INT64));
	pOutPacket->WriteData(&ui8FriendCount,			sizeof(UINT8));

	for(UINT32 i = 0; i < ui8FriendCount; i++)
	{
		UINT8 length = (UINT8)(i3String::Length(pFriendNode[i]._strnick) + 1);
		pOutPacket->WriteData(&length,					sizeof(UINT8));
		pOutPacket->WriteData(pFriendNode[i]._strnick,	sizeof(TTCHAR) * length);
		info._fuid		= pFriendNode[i]._i64fuid;
		info._state		= pFriendNode[i]._i32state;
		info._rank		= pFriendNode[i]._ui8rank;		
		info._nickcolor = pFriendNode[i]._ui8nickcolor;		
		pOutPacket->WriteData(&info,				sizeof(FRIEND_INFO));
	}

}

void MakePacket_OGame2Mess_Find_User_Req(OGame2Mess_Find_User_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Find_User_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(pOutPacket->strfUserNick,	sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pOutPacket->strfUserNick[NET_NICK_NAME_SIZE - 1] = '\0';

}

void MakePacket_OGame2Mess_Find_User_Ack(i3NetworkPacket *pOutPacket,
												INT32			i32SessionIdx,
												INT64			i64UID,
												INT32			i32Result)
{
	pOutPacket->WriteData(&i32SessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&i64UID,				sizeof(INT64));
	pOutPacket->WriteData(&i32Result,			sizeof(INT32));

}

void MakePacket_OGame2Mess_Send_Whisper_Req(OGame2Mess_Send_Whisper_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Send_Whisper_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));			
	pInPacket->ReadData(pOutPacket->senderNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pInPacket->ReadData(pOutPacket->receiverNick,	sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pInPacket->ReadData(&pOutPacket->bIsGMUser,		sizeof(bool));	
	pInPacket->ReadData(&pOutPacket->messageSize,	sizeof(INT16));
	pOutPacket->messageSize = MIN(pOutPacket->messageSize,	MAX_STRING_COUNT);	

	pInPacket->ReadData(pOutPacket->message,		sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->message[pOutPacket->messageSize - 1] = '\0';

	pInPacket->ReadData(&pOutPacket->ui8NickColor,	sizeof(UINT8));

}

void MakePacket_OGame2Mess_Send_Whisper_Ack(i3NetworkPacket *pOutPacket,
										    INT32		i32SessionIdx,
										    INT64		i64UID,
										    INT32		i32Result, 
										    TTCHAR 		*pstrreceiverNick,
											UINT8		ui8NickColor)
{
	pOutPacket->WriteData(&i32SessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&i64UID,				sizeof(INT64));
	pOutPacket->WriteData(&i32Result,			sizeof(INT32));
	pOutPacket->WriteData(pstrreceiverNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pOutPacket->WriteData(&ui8NickColor,		sizeof(UINT8));
}

void MakePacket_OGame2Mess_Send_Whisper_By_UID_Req(OGame2Mess_Send_Whisper_By_UID_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Send_Whisper_By_UID_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));	
	pInPacket->ReadData(&pOutPacket->iReceiverUID,	sizeof(INT64));				
	pInPacket->ReadData(pOutPacket->senderNick,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pInPacket->ReadData(pOutPacket->receiverNick,	sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pInPacket->ReadData(&pOutPacket->bIsGMUser,		sizeof(bool));

	pInPacket->ReadData(&pOutPacket->messageSize,	sizeof(INT16));
	pOutPacket->messageSize = MIN(pOutPacket->messageSize,	MAX_STRING_COUNT);	

	pInPacket->ReadData(pOutPacket->message,		sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->message[pOutPacket->messageSize - 1] = '\0';

	pInPacket->ReadData(&pOutPacket->ui8NickColor,	sizeof(UINT8));
}

void MakePacket_OGame2Mess_Recv_Whisper_Ack(i3NetworkPacket*	pOutPacket,
										    INT32				i32ReceiverSessionIdx,
										    INT64				i64ReceiverUID,
										    TTCHAR*				pstrsenderNick,
											bool				bIsGMUser,
										    INT16				i16messageSize,
										    TTCHAR				*pstrmessage,
											UINT8				ui8NickColor)
{
	pOutPacket->WriteData(&i32ReceiverSessionIdx,	sizeof(INT32));
	pOutPacket->WriteData(&i64ReceiverUID,			sizeof(INT64));
	pOutPacket->WriteData(pstrsenderNick,			sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	pOutPacket->WriteData(&bIsGMUser,				sizeof(bool));
	pOutPacket->WriteData(&i16messageSize,			sizeof(INT16));
	pOutPacket->WriteData(pstrmessage,				sizeof(TTCHAR) * i16messageSize);
	pOutPacket->WriteData(&ui8NickColor,			sizeof(UINT8));
	
}

void MakePacket_OGame2MessSendChannelNumReq(OGame2MessSendChannelNumReq	*pOutPacket, i3NetworkPacket* pInPacket)
{
	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iChanelNum,	sizeof(INT32));
}
//void MakePacket_OGame2MessSendChannelNumAck()
//{
//}

void MakePacket_OGame2Mess_Alive_User_Req(OGame2Mess_Alive_User_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Alive_User_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID,			sizeof(INT64));
	pInPacket->ReadData(pOutPacket->pNickName,		sizeof(TTCHAR) * NET_NICK_NAME_SIZE);
	
	UINT32 NickNameSize = i3String::Length(pOutPacket->pNickName) + 1;
	NickNameSize = MIN(NickNameSize, NET_NICK_NAME_SIZE);
	pOutPacket->pNickName[NickNameSize - 1] = '\0';

}

void MakePacket_OGame2Mess_Block_Info_Ack(i3NetworkPacket*			pOutPacket,												  
												  INT32				i32SessionIdx,
												  INT64				i64UID,
												  UINT8				ui8BlockCount,
												  BLOCK_NODE*		pBlockNode,
												  BLOCK_COMMENT*	pBlockComment
												  )
{
	BLOCK_INFO pInfo;

	pOutPacket->WriteData(&i32SessionIdx,			sizeof(INT32));
	pOutPacket->WriteData(&i64UID,					sizeof(INT64));
	pOutPacket->WriteData(&ui8BlockCount,			sizeof(UINT8));

	for(UINT32 i = 0; i < ui8BlockCount; i++)
	{
		UINT8 ui8NickSize = (UINT8)(i3String::Length(pBlockNode[i]._strNick) + 1);
		pOutPacket->WriteData(&ui8NickSize,				sizeof(UINT8));
		pOutPacket->WriteData(pBlockNode[i]._strNick,	sizeof(TTCHAR) * ui8NickSize);

		UINT8 ui8CommentSize = (UINT8)(i3String::Length(pBlockComment[i]._strComment) + 1);
		pOutPacket->WriteData(&ui8CommentSize,				sizeof(UINT8));
		pOutPacket->WriteData(pBlockComment[i]._strComment,	sizeof(TTCHAR) * ui8CommentSize);

		pInfo._Idx	 = pBlockNode[i]._i32Idx;
		pInfo._rank  = pBlockNode[i]._ui8rank;		
		pOutPacket->WriteData(&pInfo,				sizeof(BLOCK_INFO));
	}

}

void MakePacket_OGame2Mess_Block_Insert_Ack(i3NetworkPacket *pOutPacket, 
											 INT32			i32SessionIdx,
											 INT64			i64UID,
											 INT32			i32Result,
											 INT32			i32Idx,
											 UINT8			ui8Rank,
											 UINT8			ui8NickLength,
											 char*			strNick,
											 UINT8			ui8CommentLength,
											 char*			strComment
											 )
{
	pOutPacket->WriteData(&i32SessionIdx,		sizeof(INT32));
	pOutPacket->WriteData(&i64UID,				sizeof(INT64));
	pOutPacket->WriteData(&i32Result,			sizeof(INT32));
	pOutPacket->WriteData(&i32Idx,				sizeof(INT32));
	pOutPacket->WriteData(&ui8Rank,				sizeof(UINT8));
	pOutPacket->WriteData(&ui8NickLength,		sizeof(UINT8));
	pOutPacket->WriteData(strNick,				sizeof(char) * ui8NickLength );
	pOutPacket->WriteData(&ui8CommentLength,	sizeof(UINT8));
	pOutPacket->WriteData(strComment,			sizeof(char) * ui8CommentLength );
}


void MakePacket_OGame2Mess_Block_Delete_Ack(i3NetworkPacket *pOutPacket, 
											 INT32			i32SessionIdx,
											 INT64			i64UID,
											 INT32			i32Result,
											 INT32			i32Idx)
{
	pOutPacket->WriteData(&i32SessionIdx,	sizeof(INT32));
	pOutPacket->WriteData(&i64UID,			sizeof(INT64));
	pOutPacket->WriteData(&i32Result,		sizeof(INT32));
	pOutPacket->WriteData(&i32Idx,			sizeof(INT32));
}

void MakePacket_OGame2Mess_Block_Insert_Req(OGame2Mess_Block_Insert_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Block_Insert_Req));

	pInPacket->ReadData(&pOutPacket->_i32SessionIdx,	sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->_i64UID,			sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->_inData._ui8Rank,	sizeof(UINT8));
	pInPacket->ReadData(&pOutPacket->_ui8NicknameSize,	sizeof(UINT8));
	if( pOutPacket->_ui8NicknameSize > NET_NICK_NAME_SIZE )	pOutPacket->_ui8NicknameSize = NET_NICK_NAME_SIZE;
	pInPacket->ReadData(pOutPacket->_inData._strNick, pOutPacket->_ui8NicknameSize);
	pOutPacket->_inData._strNick[ NET_NICK_NAME_SIZE-1 ] = '\0';

	pInPacket->ReadData(&pOutPacket->_ui8CommentSize,	sizeof(UINT8));
	if( pOutPacket->_ui8CommentSize > BLOCK_COMMENT_MAX )	pOutPacket->_ui8CommentSize = BLOCK_COMMENT_MAX;
	pInPacket->ReadData(pOutPacket->_inData._strComment, pOutPacket->_ui8CommentSize);
	pOutPacket->_inData._strComment[ BLOCK_COMMENT_MAX-1 ] = '\0';

}

void MakePacket_OGame2Mess_Block_Delete_Req(OGame2Mess_Block_Delete_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Block_Delete_Req));

	pInPacket->ReadData(&pOutPacket->_i32SessionIdx, sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->_i64UID, sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->_i32Idx, sizeof(INT32));			

}

void MakePacket_OGame2Mess_Reset_Block_Info_Req(OGame2Mess_Reset_Block_Info_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Reset_Block_Info_Req));

	pInPacket->ReadData(&pOutPacket->_i64UID, sizeof(INT64));	

}

void MakePacket_OGame2Mess_Note_Send_Gift_Notice_Req(OGame2Mess_Note_Send_Gift_Notice_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	//memset(pOutPacket, 0, sizeof(OGame2Mess_Note_Send_Gift_Notice_Req));
	
	pInPacket->ReadData(&pOutPacket->iReceiverUID, sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->ui8NickSize, sizeof(UINT8));
	pInPacket->ReadData(pOutPacket->SenderNick, sizeof(TTCHAR) * pOutPacket->ui8NickSize);
	pOutPacket->SenderNick[pOutPacket->ui8NickSize] = '\0';

	pInPacket->ReadData(&pOutPacket->messageSize, sizeof(UINT8));
	pInPacket->ReadData(pOutPacket->message, sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->message[pOutPacket->messageSize] = '\0';
}

void MakePacket_OGame2Mess_Note_Send_User_Gift_Req(OGame2Mess_Note_Send_Find_UID_Req *pOutPacket, i3NetworkPacket* pInPacket)
{
	memset(pOutPacket, 0, sizeof(OGame2Mess_Note_Send_Find_UID_Req));

	pInPacket->ReadData(&pOutPacket->iSessionIdx, sizeof(INT32));
	pInPacket->ReadData(&pOutPacket->iUID, sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->iReceiverUID, sizeof(INT64));
	pInPacket->ReadData(&pOutPacket->messageSize, sizeof(UINT8));
	pInPacket->ReadData(pOutPacket->message, sizeof(TTCHAR) * pOutPacket->messageSize);
	pOutPacket->message[pOutPacket->messageSize] = '\0';
}