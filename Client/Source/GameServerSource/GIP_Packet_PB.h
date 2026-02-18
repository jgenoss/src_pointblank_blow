#ifndef __GIP_DEF_H__
#define __GIP_DEF_H__

#pragma pack( push, 1 )
struct Packet_Header
{
	unsigned short	length;
	unsigned char	type;
	unsigned char	version;
	unsigned char	headerLen;
	unsigned char	crc;
	unsigned short	pid;
	unsigned short	qid;
};

struct Packet_TYPE_RESULT
{
	Packet_Header header;
	unsigned short result;
};

struct Packet_TYPE_GREETING
{
	Packet_Header header;
	unsigned short	game_id;
	unsigned short	server_id;
	unsigned char	sub_server_id;	
};

//struct Packet_TYPE_GAMESERVER_DATA
//{
//	Packet_Header header;
//	struct _S_commandList
//	{
//		unsigned char	commandType;
//		unsigned char	commandCrc;
//		unsigned short	command_len;
//		char			*command;
//	} *commandList;
//};

struct _S_commandList
{
	unsigned char	commandType;
	unsigned char	commandCrc;
	unsigned short	command_len;
	char			command[4096];	//충분함
};

struct Packet_TYPE_GAMESERVER_DATA
{
	Packet_Header header;	
	_S_commandList commandList;
};
#pragma pack( pop )



//Define GIP Return Code
#define MSG_RESULT_SUCCESS					0x0000		//성공
#define MSG_RESULT_REQUEST_1				0x0101		//User info request
#define MSG_RESULT_REQUEST_2				0x0102		//Character info request
#define MSG_RESULT_REQUEST_3				0x0103		//Clan info request
#define MSG_RESULT_REQUEST_4				0x0104		//Reserved request
#define MSG_RESULT_REQUEST_5				0x0105		//Reserved request
#define MSG_RESULT_REQUEST_6				0x0106		//Reserved request
#define MSG_RESULT_REQUEST_7				0x0107		//Reserved request
#define MSG_RESULT_REQUEST_8				0x0108		//Reserved request
#define MSG_RESULT_REQUEST_9				0x0109		//Reserved request
#define MSG_RESULT_REQUEST_10				0x010A		//Reserved request
#define MSG_RESULT_REQUEST_11				0x010B		//Reserved request
#define MSG_RESULT_REQUEST_12				0x010C		//Reserved request
#define MSG_RESULT_REQUEST_13				0x010D		//Reserved request
#define MSG_RESULT_REQUEST_14				0x010E		//Reserved request
#define MSG_RESULT_REQUEST_15				0x010F		//Reserved request
#define MSG_RESULT_INVALID_GAME_ID			0x8001		//유효하지 않은 게임 UID 가 전달됨
#define MSG_RESULT_INVALID_SERVER_ID		0x8002		//유효하지 않은 서버 UID 가 전달됨
#define MSG_RESULT_OVERLAPPING_SERVER_ID	0x8003		//동일한 서버 ID, sub 서버 ID로 접속한 세션이 존재함 
#define MSG_RESULT_INVALID_CMD				0x8004		//유효하지 않은 command 가 전달됨
#define MSG_RESULT_INVALID_PARAM			0x8005		//유효하지 않은 parameter 가 전달됨
#define MSG_RESULT_CMD_SYNTAX_ERROR			0x8006		//Command 의 xml syntax error
#define MSG_RESULT_DB_TRANSACT_ERROR		0x8007		//GIDB 에서의 transaction 실패
#define MSG_RESULT_DB_DISCONNECTED			0x800F		//GIP 서버와 GIDB 의 연결이 끊어졌음
#define MSG_RESULT_GENERAL_FAILURE			0x80FF		//GIP 서버에서 일반적인 오류가 발생함
#define MSG_RESULT_UNSUPPORTED_VERSION		0xC001		//헤더 버전 에러
#define MSG_RESULT_INVALID_PACKET_LENGTH	0xC002		//패킷의 전체 길이 값과 실제 길이가 맞지 않음
#define MSG_RESULT_BAD_HEADER_CRC			0xC003		//헤더 CRC 에러
#define MSG_RESULT_UNAUTHORIZED_ID			0xC004		//인증 안 된 헤더 ID

#endif
