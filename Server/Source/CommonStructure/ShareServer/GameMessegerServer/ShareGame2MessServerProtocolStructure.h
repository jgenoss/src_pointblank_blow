////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareGame2MessServerProtocolStructure.h
//	Author:
//		Ji-won Kang.
//	Description:
//		Structures used between Game Server to Messenger Server. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_GAME2MESSSERVER_PROTOCOL_STRUCTURE_H__
#define __SHARE_GAME2MESSSERVER_PROTOCOL_STRUCTURE_H__


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////

struct OGame2Mess_Note_Send_Req
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Sender UID
	UINT8	nicknameSize;						// Nick Size
	UINT8	messageSize;						// Note Size

	TTCHAR	nickname[NET_NICK_NAME_SIZE];		// Receiver Nick
	TTCHAR	message[NET_NOTE_SIZE];				// Note
	
}; // PROTOCOL_MESSENGER_NOTE_SEND_REQ,			// Nick으로 쪽지 보내기 요청

struct OGame2Mess_Note_Send_Ack
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Receiver UID
	INT32   iResult;							// Result

};// PROTOCOL_MESSENGER_NOTE_SEND_ACK,			// 쪽지 보내기 응답 - 성공 or 실패	

struct OGame2Mess_Note_Send_Find_UID_Req
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Sender UID
	INT64	iReceiverUID;						// Receiver UID	
	UINT8	messageSize;						// Note Size

	TTCHAR	message[NET_NOTE_SIZE];				// Note

}; // PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ,	// UID로 쪽지 보내기 요청(기존 프로토콜은 유지)

//struct OGame2Mess_Note_Send_Ack
//{
//	INT32	iSessionIdx;
//	INT64	iUID;
//	INT32   iResult;	
//};// PROTOCOL_MESSENGER_NOTE_SEND_ACK,		// 쪽지 보내기 응답 - 성공 or 실패	

//struct OGame2Mess_Note_List_Req
//{
//
//}; // PROTOCOL_MESSENGER_NOTE_LIST_REQ,		// ! 사용안함

struct OGame2Mess_Note_List_Ack
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Receiver UID
	UINT8	curPage;							// 현재 전송될 페이지
	UINT8	noteCount;							// 쪽지 갯수

	// noteCount에 따라 유동적으로 변동되는 값
	// NOTE_HEADER		pHeader[noteCount];	
	// NOTE_BODY		pBody[noteCount];

}; // PROTOCOL_MESSENGER_NOTE_LIST_ACK,			// 쪽지 목록 받기

struct OGame2Mess_Note_Check_Readed_Req
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Sender UID
	UINT8	iCount;								// Read Count
	
	INT32 pList[MAX_NORMAL_NOTE_COUNT];			// 읽은 쪽지 번호(Index)를 표시
	

}; // PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ,	// 쪽지 읽은 표시 하기 요청

struct OGame2Mess_Note_Check_Readed_Ack
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Receiver UID
	UINT8	iCount;								// Read Count

	INT32 pList[MAX_NORMAL_NOTE_COUNT];			// 읽은 쪽지 번호(Index)를 표시

}; // PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK,	// 쪽지 읽은 표시 하기 응답

struct OGame2Mess_Note_Delete_Req
{
	INT32	iSessionIdx;						// Session Inde x
	INT64	iUID;								// Sender UID
	UINT8	iCount;								// 삭제한 쪽지의 개수
	
	INT32	pList[MAX_NORMAL_NOTE_COUNT];		// 삭제한 쪽지의 Index List

}; // PROTOCOL_MESSENGER_NOTE_DELETE_REQ,		// 쪽지 삭제 요청

struct OGame2Mess_Note_Delete_Ack
{
	INT32	iSessionIdx;						// Session Index
	INT64	iUID;								// Receiver UID
	INT32	iResult;							// 삭제 결과
	UINT8	iCount;								// 삭제한 쪽지의 개수

	INT32	pList[MAX_NORMAL_NOTE_COUNT];		// 삭제한 쪽지의 Index List

}; // PROTOCOL_MESSENGER_NOTE_DELETE_ACK,		// 쪽지 삭제 응답

//struct OGame2Mess_Note_Receive_Req
//{
//
//}; // PROTOCOL_MESSENGER_NOTE_RECEIVE_REQ,	// ! 사용안함	

struct OGame2Mess_Note_Receive_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// Result
		
	NOTE_NODE	NoteNode;						// Note

}; // PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK,		// 새 쪽지 도착

struct OGame2Mess_Login_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID
	TTCHAR		pNickName[NET_NICK_NAME_SIZE];	// Sender NickName
	UINT32		iRank;							// Sender Rank
	UINT8		ui8NickColor;					// Sender NickColor
}; // PROTOCOL_MESSENGER_LOGIN_REQ,				// 메신저 서버 로그인 요청

//struct OGame2Mess_Login_Ack
//{
//
//}; // PROTOCOL_MESSENGER_LOGIN_ACK,				// 메신저 서버 로그인 응답(!사용안함)

struct OGame2Mess_Logout_Req
{
	INT64		iUID;							// Sender UID

}; // PROTOCOL_MESSENGER_LOGOUT_REQ,				// 메신저 서버 로그아웃 요청

//struct OGame2Mess_Logout_Ack
//{
//
//}; // PROTOCOL_MESSENGER_LOGOUT_ACK,				// 메신저 서버 로그아웃 응답(!사용안함)

struct OGame2Mess_Enter_User_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID

}; // PROTOCOL_MESSENGER_ENTER_USER_REQ,			// 게임서버 입장 통보

struct OGame2Mess_Enter_User_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	UINT32		ifriendCount;					// 친구 등록수

}; // PROTOCOL_MESSENGER_ENTER_USER_ACK,			// ! 사용안함

struct OGame2Mess_Leave_User_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID

}; // PROTOCOL_MESSENGER_LEAVE_USER_REQ,			// 게임서버 퇴장 통보

//struct OGame2Mess_Leave_User_Ack
//{
//
//}; // PROTOCOL_MESSENGER_LEAVE_USER_ACK,			// ! 사용안함

struct OGame2Mess_Change_NickName_Req
{
	INT32				iSessionIdx;
	INT64				iUID;
	FRIEND_WORK_TYPE	eWorkType;
	TTCHAR				nickname[NET_NICK_NAME_SIZE];
	UINT8				ui8NickColor;
}; // PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ,		// 닉네임 변경 - 초기 설정 시에도 사용

//struct OGame2Mess_Change_NickName_Ack
//{
//
//}; // PROTOCOL_MESSENGER_CHANGE_NICKNAME_ACK,		// ! 사용안함

struct OGame2Mess_Change_Rank_Req
{
	INT32		iSessionIdx;
	INT64		iUID;
	UINT32		iRank;

}; // PROTOCOL_MESSENGER_CHANGE_RANK_REQ,			// 계급 변경 - 초기 설정 시에도 사용

//struct OGame2Mess_Change_Rank_Ack
//{
//
//}; // PROTOCOL_MESSENGER_CHANGE_RANK_ACK,			// ! 사용안함

struct OGame2Mess_Clan_Note_Send_Req
{
	INT32	iSessionIdx;
	INT64	iUID;
	INT64	receiverUID;
	UINT32	clanIdx;
	UINT8	isAskNote;
	UINT8	clanNameSize;
	UINT8	messageSize;
	
	TTCHAR	clanName[NET_CLAN_NAME_SIZE];
	TTCHAR	message[NET_NOTE_SIZE];

}; // PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ,		// 클랜 쪽지 보내기 요청 

struct OGame2Mess_Clan_Note_Send_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// Result

}; // PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK,		// 클랜 쪽지 보내기 응답 - 성공 or 실패

struct OGame2Mess_Clan_Group_Note_Send_Req
{
	INT32	iSessionIdx;
	INT64	iUID;
	UINT32  iCount;
	INT64	receiverUID[MAX_MEMBER_COUNT];
	UINT32	clanIdx;
	UINT8	isAskNote;
	UINT8	clanNameSize;
	UINT8	messageSize;

	TTCHAR	clanName[NET_CLAN_NAME_SIZE];
	TTCHAR	message[NET_NOTE_SIZE];

}; // PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_REQ,		// 클랜 한번에 여러 사람에게 쪽지 보내기 요청(가입 승인, 거부, 강제 제명, 스텝 임명, 강등, 클랜 전체 쪽지)

struct OGame2Mess_Clan_Group_Note_Send_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// Result

}; // PROTOCOL_MESSENGER_CLAN_GROUP_NOTE_SEND_ACK,		// 클랜 한번에 여러 사람에게 쪽지 보내기 응답 - 성공 or 실패


//struct OGame2Mess_Change_Color_Nick_Ack
//{
//
//}; // PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_ACK,	// ! 사용안함

struct OGame2Mess_Friend_Accept_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID
	UINT8		iFriendIdx;						// 친구 목록의 해당 Friend Index 

}; // PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ,			// 친구 등록, 삭제도 여기에 포함됨

struct OGame2Mess_Friend_Accept_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// Result

}; // PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK,			// 친구 승인 에러 보고

struct OGame2Mess_Friend_Insert_Req
{
	INT32				iSessionIdx;			// Session Index
	INT64				iUID;					// Sender UID
	UINT8				nicknameSize;			// Nickname Size

	// NickName만 받을 것임.
	IN_FRIEND_CONTROL	inData;					// 친구 추가에 사용할 친구의 입력시 최소 정보
	
}; // PROTOCOL_MESSENGER_FRIEND_INSERT_REQ,			// 친구 추가 요청

struct OGame2Mess_Friend_Insert_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// Result

}; // PROTOCOL_MESSENGER_FRIEND_INSERT_ACK,			// 친구 추가 에러 보고

struct OGame2Mess_Friend_Invited_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID
	UINT8		iFriendIdx;							// 친구 추가하기 원하는 Friend Idx

}; // PROTOCOL_MESSENGER_FRIEND_INVITED_REQ,			// 친구초대 요청	

//struct OGame2Mess_Friend_Invited_Ack
//{
//
//}; // PROTOCOL_MESSENGER_FRIEND_INVITED_ACK,			// 친구초대 전달

struct OGame2Mess_Friend_Invited_Request_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	UINT8		iFriendIdx;						// 친구 추가에 대한 응답 Friend Index

}; // PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK,	// 친구초대 전달

struct OGame2Mess_Friend_Delete_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID
	UINT8		iFriendIdx;						// 친구 목록의 삭제하기 원하는 Friend Index

}; // PROTOCOL_MESSENGER_FRIEND_DELETE_REQ,			//친구 삭제  

struct OGame2Mess_Friend_Delete_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// 결과

}; // PROTOCOL_MESSENGER_FRIEND_DELETE_ACK,			// 친구 삭제 에러 보고

struct OGame2Mess_Friend_Room_Enter_Req
{
	INT32		iSessionIdx;
	INT64		iUID;
	UINT8		channel;
	UINT16		roomIdx;

}; // PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ,		//방에 들어가기 

//struct OGame2Mess_Friend_Room_Enter_Ack
//{
//
//}; // PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_ACK, 

struct OGame2Mess_Friend_Room_Leave_Req
{
	INT32		iSessionIdx;
	INT64		iUID;

}; // PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ,		//방에서 나가기 

//struct OGame2Mess_Friend_Room_Leave_Ack
//{
//
//
//}; // PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_ACK,	

struct OGame2Mess_Reset_Friend_Info_Req
{
	INT64		iUID;

}; // PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ,		// 친구목록 초기화 요청


//struct OGame2Mess_Reset_Friend_Info_Ack
//{
//
//}; // PROTOCOL_MESSENGER_RESET_FRIEND_INFO_ACK,		// ! 사용안함

struct OGame2Mess_KickUser_Req
{
	INT32		iSessionIdx;
	INT64		iUID;

}; // PROTOCOL_MESSENGER_KICKUSER_REQ,				// GSM에 의한 Kick

//struct OGame2Mess_KickUser_Ack
//{
//
//}; // PROTOCOL_MESSENGER_KICKUSER_ACK,				// ! 사용안함

struct OGame2Mess_Friend_Info_Change_Ack
{
	INT32			iSessionIdx;
	INT64			iUID;
	UINT8			iState;
	UINT8			iFriendIdx;
	UINT8			nicknameSize;
	char			nickname[NET_NICK_NAME_SIZE];

	FRIEND_INFO		info;

}; // PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK,		//상태 변경시 전달됨

//struct OGame2Mess_Friend_Info_Req
//{
//
//}; // PROTOCOL_MESSENGER_FRIEND_INFO_REQ,				// 친구정보 요청하기 

struct OGame2Mess_Friend_Info_Ack
{
	INT32		iSessionIdx;				// Session Index
	INT64		iUID;						// Receiver UID
	UINT8		iFriendCount;				// 친구 정보의 개수

	//FRIEND_INFO		*info;						// 친구 정보(유동적)

}; // PROTOCOL_MESSENGER_FRIEND_INFO_ACK,				// 요청할수도 있고, 로그인후 자동으로 받을수도 있음

struct OGame2Mess_Find_User_Req
{
	INT32		iSessionIdx;						// Session Index
	INT64		iUID;								// Sender UID
	TTCHAR		strfUserNick[NET_NICK_NAME_SIZE];	// 친구의 NickName

}; // PROTOCOL_MESSENGER_FIND_USER_REQ,				// 유저찾기 요청

struct OGame2Mess_Find_User_Ack
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Receiver UID
	INT32		iResult;						// 결과

}; // PROTOCOL_MESSENGER_FIND_USER_ACK,				// 유저찾기 응답

struct OGame2Mess_Send_Whisper_Req
{
	INT32		iSessionIdx;						// Session Index
	INT64		iUID;								// Sender UID
	TTCHAR		senderNick[NET_NICK_NAME_SIZE];		// Sender NickName
	TTCHAR		receiverNick[NET_NICK_NAME_SIZE];	// Receiver NickName
	bool		bIsGMUser;							// 메세지를 보낸사람이 GM인지 아닌지
	INT16		messageSize;						// Message Size
	TTCHAR		message[MAX_STRING_COUNT];			// Message
	UINT8		ui8NickColor;

}; // PROTOCOL_MESSENGER_SEND_WHISPER_REQ,			// 귓속말 보내기 요청

struct OGame2Mess_Send_Whisper_Ack
{
	INT32		iSessionIdx;						// Session Index
	INT64		iUID;								// Sender UID
	INT32		iResult;							// 귓속말 보내기 응답 결과 - 실패(수신자를 못찾음)	
	char		receiverNick[NET_NICK_NAME_SIZE];	// Receiver NickName
	UINT8		ui8NickColor;
}; // PROTOCOL_MESSENGER_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패

struct OGame2Mess_Send_Whisper_By_UID_Req
{
	INT32		iSessionIdx;						// Session Index
	INT64		iUID;								// Sender UID
	INT64		iReceiverUID;						// RecevierUID	
	TTCHAR		senderNick[NET_NICK_NAME_SIZE];		// Sender NickName
	TTCHAR		receiverNick[NET_NICK_NAME_SIZE];	// Receiver NickName
	bool		bIsGMUser;							// 메세지를 보낸사람이 GM인지 아닌지
	INT16		messageSize;						// Message Size
	TTCHAR		message[MAX_STRING_COUNT];			// Message
	UINT8		ui8NickColor;

}; // PROTOCOL_MESSENGER_SEND_WHISPER_BY_UID_REQ,	// 귓속말 Receiver UID로 보내기 요청

//struct OGame2Mess_Recv_Whisper_Req
//{
//
//}; // PROTOCOL_MESSENGER_RECV_WHISPER_REQ,			// ! 사용안함

struct OGame2Mess_Recv_Whisper_Ack
{
	INT32		iSessionIdx;						// Receiver Session Index
	INT64		iUID;								// Receiver UID
	char		senderNick[NET_NICK_NAME_SIZE];		// Sender NickName
	INT16		messageSize;						// Message Size
	char		message[MAX_STRING_COUNT];			// Message
	UINT8		ui8NickColor;
}; // PROTOCOL_MESSENGER_RECV_WHISPER_ACK,			// 귓속말 받기



struct OGame2Mess_Alive_User_Req
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID(UINT32로 해야하지 않나??)
	TTCHAR		pNickName[NET_NICK_NAME_SIZE];	// Sender NickName

}; // PROTOCOL_ALIVE_USER_REQ,						// 유저가 살아있으면 패킷을 보냄 

//struct OGame2Mess_Alive_User_Ack
//{
//
//}; // PROTOCOL_ALIVE_USER_ACK,						// ! 사용안함	

struct OGame2MessSendChannelNumReq
{
	INT32		iSessionIdx;					// Session Index
	INT64		iUID;							// Sender UID(UINT32로 해야하지 않나??)
	INT32		iChanelNum;						// Select Channel Number

};	// PROTOCOL_MESSENGER_FRIEND_LOBBY_ENTER_REQ
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//				일방적으로 서버가 보내는 메시지	
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//struct OGame2Mess_ConnectionSuccess
//{
//		// None
//}; // PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS,		//Connect

//struct OGame2Mess_DisConnectionSuccess
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS,	//DisConnect 	

//struct OGame2Mess_Kick_Player
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_KICK_PLAYER,			//킥되는 유저 Open, Close 하면서 사라지는 사람..
//
//
//struct OGame2Mess_Kick_Battle_Player
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER,		//게임 중간 퇴출
//
//struct OGame2Mess_Invited
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_INVITED,				//초대 받았음//Wirte Nick Name(NET_NICK_NAME_SIZE), RoomIdx, NET_ROOM_PW); 
//
//struct OGame2Mess_Error
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_ERROR,					//Error 메시지 //Client 가 받으면  바로 Disconnect 해야함
//
//struct OGame2Mess_Announce
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_ANNOUNCE,				//운영툴 공지
//
//struct OGame2Mess_Friend_Invited
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED,			//친구에게 초대를 받음							//사용하지 않음 
//
//struct OGame2Mess_Room_Destroy
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY,			//AdminGateWay에서 방 파괴 메시지를 받음 
//
//struct OGame2Mess_Change_Inventory
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY,		//인벤토리 내용이 변경됨.
//
//struct OGame2Mess_GameGuard_Error
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR,		//게임 가드 Recv 에러 
//
//struct OGame2Mess_Event_Coupon
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_EVENT_COUPON,			// 쿠폰 받음  // 사라질 메시지
//
//struct OGame2Mess_New_Event_CardSet
//{
//
//}; // PROTOCOL_SERVER_MESSAGE_NEW_EVENT_CARDSET,		//새 이벤트 카드셋 지급통보 

struct OGame2Mess_Block_Insert_Req
{
	INT32				_i32SessionIdx;				// 자신의 SessionIndex
	INT64				_i64UID;					// 자신의 UID
	UINT8				_ui8NicknameSize;			// 차단할 유저의 닉네임 Size
	UINT8				_ui8CommentSize;			// 차단 사유 size


	IN_BLOCK_CONTROL	_inData;					// 차단 유저 추가에 사용할. 차단 유저 입력시 최소 정보
	
}; // PROTOCOL_MESSENGER_BLOCK_INSERT_REQ,			// 차단 추가 요청

struct OGame2Mess_Block_Delete_Req
{
	INT32		_i32SessionIdx;						// Session Index
	INT64		_i64UID;							// Sender UID
	INT32		_i32Idx;							// 차단 목록의 삭제하기 원하는 DB Idx
}; // PROTOCOL_MESSENGER_BLOCK_DELETE_REQ,			// 차단 삭제 요청

struct OGame2Mess_Reset_Block_Info_Req
{
	INT64		_i64UID;
	}; // PROTOCOL_MESSENGER_RESET_BLOCK_INFO_REQ,		// 차단목록 초기화 요청


struct OGame2Mess_Note_Send_Gift_Notice_Req
{
	INT64		iReceiverUID;			// Receiver UID
	UINT8		ui8NickSize;
	TTCHAR		SenderNick[NET_NICK_NAME_SIZE];
	UINT8		messageSize;			// Note Size
	TTCHAR		message[NET_NOTE_SIZE];	// Note
};

#endif // __SHARE_GAME2MESSSERVER_PROTOCOL_STRUCTURE_H__