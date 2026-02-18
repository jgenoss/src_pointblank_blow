////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareGame2MessServerProtocol.h
//	Author:
//		Ji-won Kang.
//	Description:
//		Protocol used between GameServerServer and MessengerServer.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_GAME2MESSSERVER_PROTOCOL_H__
#define __SHARE_GAME2MESSSERVER_PROTOCOL_H__


//#include "ProtocolDef.h"

// Operation codes.
/*
메시지는 전송 방향, 행동 종류, 요청 방식이 하나의 OperationCode값에 나타난다.
	With server      : MN00000 (M->N, M,N is server or client id)
	Client to Server :  0xxxx
	Server to Client :  1xxxx
	
	Request          :   1xxx
	Response(ACK)    :   2xxx
	Report           :   3xxx

	C2GAME_XXX_REQ		 401000
	C2GAME_XXX_RSP		 402000
	C2GAME_XXX_RPT		 403000
	GAME2C_XXX_REQ		 411000
	GAME2C_XXX_RSP		 412000
	GAME2C_XXX_RPT		 413000


	ControlServer	: 1
	TransServer		: 2
	SIAServer		: 3
	ClanServer		: 4
	MessengerServer : 5
	RelayServer		: 6
	GameServer		: 7
	GatewayServer	: 8





	

*/

/*

	PROTOCOL_MESSENGER_SERVER = PROTOCOL_AUTH_SERVER + 0x00A0,
*/


//////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////

// 메신저 서버 프로토콜 시작
//enum Protocol_GameToMessServer
//{
//	PROTOCOL_MESSENGER_SERVER = PROTOCOL_AUTH_SERVER + 0x00A0,
//	//PROTOCOL_MESSENGER_SERVER = 0x0100 + 0x00A0,	
//
//	PROTOCOL_MESSENGER_NOTE_SEND_REQ,			// Nick으로 쪽지 보내기 요청			
//	PROTOCOL_MESSENGER_NOTE_SEND_ACK,			// 쪽지 보내기 응답 - 성공 or 실패	
//
//	PROTOCOL_MESSENGER_NOTE_SEND_FIND_UID_REQ,	// UID로 쪽지 보내기 요청(기존 프로토콜은 유지)
//	//PROTOCOL_MESSENGER_NOTE_SEND_ACK,			// 쪽지 보내기 응답 - 성공 or 실패	
//
//	//PROTOCOL_MESSENGER_NOTE_LIST_REQ,			// ! 사용안함
//	PROTOCOL_MESSENGER_NOTE_LIST_ACK,			// 쪽지 목록 받기
//
//	PROTOCOL_MESSENGER_NOTE_CHECK_READED_REQ,	// 쪽지 읽은 표시 하기 요청
//	PROTOCOL_MESSENGER_NOTE_CHECK_READED_ACK,	// 쪽지 읽은 표시 하기 응답
//
//	PROTOCOL_MESSENGER_NOTE_DELETE_REQ,			// 쪽지 삭제 요청
//	PROTOCOL_MESSENGER_NOTE_DELETE_ACK,			// 쪽지 삭제 응답
//
//	//PROTOCOL_MESSENGER_NOTE_RECEIVE_REQ,		// ! 사용안함	
//	PROTOCOL_MESSENGER_NOTE_RECEIVE_ACK,		// 새 쪽지 도착
//
//	PROTOCOL_MESSENGER_LOGIN_REQ,				// 메신저 서버 로그인 요청
//	//PROTOCOL_MESSENGER_LOGIN_ACK,				// 메신저 서버 로그인 응답
//
//	PROTOCOL_MESSENGER_LOGOUT_REQ,				// 메신저 서버 로그아웃 요청
//	//PROTOCOL_MESSENGER_LOGOUT_ACK,				// 메신저 서버 로그아웃 응답
//
//	PROTOCOL_MESSENGER_ENTER_USER_REQ,			// 게임서버 입장 통보
//	PROTOCOL_MESSENGER_ENTER_USER_ACK,			// ! 사용안함
//
//	PROTOCOL_MESSENGER_LEAVE_USER_REQ,			// 게임서버 퇴장 통보
//	//PROTOCOL_MESSENGER_LEAVE_USER_ACK,			// ! 사용안함
//
//	PROTOCOL_MESSENGER_CHANGE_NICKNAME_REQ,		// 닉네임 변경 - 초기 설정 시에도 사용
//	//PROTOCOL_MESSENGER_CHANGE_NICKNAME_ACK,		// ! 사용안함
//
//	PROTOCOL_MESSENGER_CHANGE_RANK_REQ,			// 계급 변경 - 초기 설정 시에도 사용
//	//PROTOCOL_MESSENGER_CHANGE_RANK_ACK,			// ! 사용안함
//
//	PROTOCOL_MESSENGER_CLAN_NOTE_SEND_REQ,		// 클랜 쪽지 보내기 요청 
//	PROTOCOL_MESSENGER_CLAN_NOTE_SEND_ACK,		// 클랜 쪽지 보내기 응답 - 성공 or 실패
//
//	//PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_REQ,	// 컬러 클랜 변경 - 초기 설정 시에도 사용(!사용안함)
//	//PROTOCOL_MESSENGER_CHANGE_COLOR_NICK_ACK,	// ! 사용안함
//
//	PROTOCOL_MESSENGER_FRIEND_ACCEPT_REQ,			// 친구 등록, 삭제도 여기에 포함됨
//	PROTOCOL_MESSENGER_FRIEND_ACCEPT_ACK,			// 친구 승인 에러 보고
//	PROTOCOL_MESSENGER_FRIEND_INSERT_REQ,			// 친구 추가 요청
//	PROTOCOL_MESSENGER_FRIEND_INSERT_ACK,			// 친구 추가 에러 보고
//	PROTOCOL_MESSENGER_FRIEND_INVITED_REQ,			// 친구초대 요청	
//	//PROTOCOL_MESSENGER_FRIEND_INVITED_ACK,			// 친구초대 전달
//	PROTOCOL_MESSENGER_FRIEND_INVITED_REQUEST_ACK,	// 친구초대 전달
//	PROTOCOL_MESSENGER_FRIEND_DELETE_REQ,			//친구 삭제  
//	PROTOCOL_MESSENGER_FRIEND_DELETE_ACK,			// 친구 삭제 에러 보고
//	PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_REQ,		//방에 들어가기 
//	//PROTOCOL_MESSENGER_FRIEND_ROOM_ENTER_ACK, 
//	PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_REQ,		//방에서 나가기 
//	//PROTOCOL_MESSENGER_FRIEND_ROOM_LEAVE_ACK,	
//	PROTOCOL_MESSENGER_RESET_FRIEND_INFO_REQ,		// 친구목록 초기화 요청
//	PROTOCOL_MESSENGER_RESET_FRIEND_INFO_ACK,		// ! 사용안함
//	PROTOCOL_MESSENGER_KICKUSER_REQ,				// GSM에 의한 Kick
//	//PROTOCOL_MESSENGER_KICKUSER_ACK,				// ! 사용안함
//	PROTOCOL_MESSENGER_FRIEND_INFO_CHANGE_ACK,		//상태 변경시 전달됨
//	//PROTOCOL_MESSENGER_FRIEND_INFO_REQ,				// 친구정보 요청하기 
//	PROTOCOL_MESSENGER_FRIEND_INFO_ACK,				// 요청할수도 있고, 로그인후 자동으로 받을수도 있음
//	PROTOCOL_MESSENGER_FIND_USER_REQ,				// 유저찾기 요청
//	PROTOCOL_MESSENGER_FIND_USER_ACK,				// 유저찾기 응답
//	PROTOCOL_MESSENGER_SEND_WHISPER_REQ,			// 귓속말 보내기 요청
//	PROTOCOL_MESSENGER_SEND_WHISPER_ACK,			// 귓속말 보내기 응답 - 실패
//	//PROTOCOL_MESSENGER_RECV_WHISPER_REQ,			// ! 사용안함
//	PROTOCOL_MESSENGER_RECV_WHISPER_ACK,			// 귓속말 받기
//
//	PROTOCOL_ALIVE_USER_REQ,						// 유저가 살아있으면 패킷을 보냄 
//	//PROTOCOL_ALIVE_USER_ACK,						// ! 사용안함	
//
//	MAX_PROTOCOL_MESSENGER_COUNT					// ! 사용안함
//};

//////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
//				GameServer <-> MesssengerServer
//				일방적으로 서버가 보내는 메시지 
//---------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////////

//enum Protocol_Server_Message
//{//일방적으로 서버가 보내는 메시지 
//	PROTOCOL_SERVER_MESSAGE	= 0x0800,	//	new				
//	PROTOCOL_SERVER_MESSAGE_CONNECTIONSUCCESS,		//Connect
//	
//	//Use Game Server
//	PROTOCOL_SERVER_MESSAGE_DISCONNECTIONSUCCESS,	//DisConnect 
//	PROTOCOL_SERVER_MESSAGE_KICK_PLAYER,			//킥되는 유저 Open, Close 하면서 사라지는 사람..
//	PROTOCOL_SERVER_MESSAGE_KICK_BATTLE_PLAYER,		//게임 중간 퇴출
//	PROTOCOL_SERVER_MESSAGE_INVITED,				//초대 받았음//Wirte Nick Name(NET_NICK_NAME_SIZE), RoomIdx, NET_ROOM_PW); 
//	PROTOCOL_SERVER_MESSAGE_ERROR,					//Error 메시지 //Client 가 받으면  바로 Disconnect 해야함
//	PROTOCOL_SERVER_MESSAGE_ANNOUNCE,				//운영툴 공지
//	PROTOCOL_SERVER_MESSAGE_FRIEND_INVITED,			//친구에게 초대를 받음							//사용하지 않음 
//	PROTOCOL_SERVER_MESSAGE_ROOM_DESTROY,			//AdminGateWay에서 방 파괴 메시지를 받음 
//	PROTOCOL_SERVER_MESSAGE_CHANGE_INVENTORY,		//인벤토리 내용이 변경됨.
//	PROTOCOL_SERVER_MESSAGE_GAMEGUARD_ERROR,		//게임 가드 Recv 에러 
//	PROTOCOL_SERVER_MESSAGE_EVENT_COUPON,			// 쿠폰 받음  // 사라질 메시지
//	PROTOCOL_SERVER_MESSAGE_NEW_EVENT_CARDSET,		//새 이벤트 카드셋 지급통보 
//};

#endif // __SHARE_GAME2MESSSERVER_PROTOCOL_H__


