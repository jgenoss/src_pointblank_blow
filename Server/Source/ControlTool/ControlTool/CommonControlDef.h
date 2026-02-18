#define STRING_LENGTH_MAX			256
#define PATH_LENGTH_MAX				256

#define BUFFER_SIZE_MAX				128

#define IP_ADDR_SIZE				32

#define MAXIMUM_FILELIST_PER_PACKET	10
#define MAXIMUM_FILE_PACKET_SIZE	8000

#define SERVER_TYPE_MAX				16

#define	CONNECTION_MAX				20

#define UPDATE_THREAD_MAX			20

#define MAX_CONTROLTOOL_CONNECT		3

#define UPDATE_NOTI_PERIOD			3.0f

#define NORMAL_PACKET_TIMEOUT		60.0f

#define RING_BUFFER_SIZE			100

#define SERVICE_NAME_SIZE			64

extern TCHAR g_wstrCurrentPath[STRING_LENGTH_MAX];
extern TCHAR g_wstrServiceName[STRING_LENGTH_MAX];

enum UPDATE_RESULT {

	UPDATE_RESULT_FAIL = 0,	//업데이트 실패
	UPDATE_RESULT_SUCCESS,	//업데이트 성공	
	UPDATE_RESULT_UPDATING	//업데이트 중입니다.

};

enum TARGET_BUFFER {
	TARGET_BUFFER_NONE = -1,
	TARGET_BUFFER_IN,
	TARGET_BUFFER_OUT
};

enum TIMEOUT_TYPE {

	TIMEOUT_TYPE_NORMAL,
	TIMEOUT_TYPE_FILESEND

};

enum UPDATE_INFO_TYPE {
	
	UPDATE_INFO_ALL,
	UPDATE_INFO_SC

};

enum UPDATE_STATE {

	UPDATE_STATE_IDLE,
	UPDATE_STATE_MAKELIST,
	UPDATE_STATE_RUNNING,
	UPDATE_STATE_FINISH

};

enum SESSION_STATE {

	SESSION_STATE_PARSING,
	SESSION_STATE_SEND,
	SESSION_STATE_LISTSEND,
	SESSION_STATE_FILESEND

};


//서버 상태
enum SERVER_STATUS {
	
	SERVER_STATUS_UNKNWON,
	SERVER_STATUS_ON,
	SERVER_STATUS_OFF,
	SERVER_STATUS_MAX

};

//서버 명령 종류
enum SERVER_OPERATION {

	SERVER_OPER_UNKNOWN,	//알수 없는 명령
	SERVER_OPER_ON,			//서버 가동 명령
	SERVER_OPER_OFF,		//서버 정지 명령	
	SERVER_OPER_MAX

};

enum RUNNING_MODE {

	RUNNING_MODE_SC,
	RUNNING_MODE_SC_MANAGER

};

enum CONNECTION_TYPE
{
	CONNECTION_TYPE_ALL		= 0x0000,
	CONNECTION_TYPE_TOOL	= 0x0001,
	CONNECTION_TYPE_SERVER	= 0x0002,
	CONNECTION_TYPE_SCM		= 0x0004,
	CONNECTION_TYPE_SC		= 0x0008,

	CONNECTION_TYPE_MAX
};

enum Protocol_SC
{
	PROTOCOL_SC_OPER = 0x1000,				//서버 ON/OFF 요청
	
	PROTOCOL_SC_UPDATE_SERVER_REQ,			//서버 Update 요청(Cont. Tool -> SC Manager)
	PROTOCOL_SC_UPDATE_SERVER_NOTI,			//서버 Update 전체진행 noti (SCM -> Control Tool)	
	PROTOCOL_SC_UPDATE_SERVER_ACK,			//서버 Update 응답(SC Manager -> Cont. Tool)
	
	PROTOCOL_SC_UPDATE_CONTROLTOOL_REQ,		//Control Tool Update 요청
	PROTOCOL_SC_UPDATE_CONTROLTOOL_ACK,		//Control Tool Update 응답(이때 Tool MD5 값도 같이 전송함)
	
	PROTOCOL_SC_TOOLSEND_REQ,				//Control Tool 업데이트를 위한 파일 전송 요청
	PROTOCOL_SC_TOOLSEND_HEAD,				//Control Tool 전송 시작
	PROTOCOL_SC_TOOLSEND_BODY,				//Control Tool 전송 데이터	
	
	PROTOCOL_SC_CONTROLTOOL_ON,				//SCM으로 Control Tool에서 연결
	PROTOCOL_SC_CONTROLTOOL_FULL,			//연결 가능한 Control Tool수 제한.
	PROTOCOL_SC_CONTROLTOOL_OFF,			//SCM으로 Control Tool에서 연결해제

	PROTOCOL_SC_SERVER_STATUS_REQ,			//Control Tool에서 SCM으로 서버 상태 요청
	PROTOCOL_SC_SERVER_STATUS_ACK,			//SCM에서 Control Tool로 서버 상태 전송

	PROTOCOL_SC_MAX
};

enum Protocol_SS
{
	PROTOCOL_SS_OPER = 0x2000,				//서버 ON/OFF 요청

	PROTOCOL_SS_SERVER_STATUS_REQ,			//SCM에서 SC로 서버 상태 요청
	PROTOCOL_SS_SERVER_STATUS_ACK,			//SC에서 SCM으로 서버 상태 전송

	PROTOCOL_SS_UPDATE_REQ,					//서버 Update 요청(SC Manager -> SC)
	PROTOCOL_SS_UPDATE_NOTI,				//서버 Update 진행 noti (SC -> SCM)
	PROTOCOL_SS_UPDATE_RES,					//서버 UPdate 결과(SC -> SC Manager)

	PROTOCOL_SS_SERVER_INFO,				//SC에서 SCM으로 자신이 갖고있는 서버정보를 전송

	PROTOCOL_SS_FILELIST_INFO,				//업데이트를 위한 FILELIST 전송
	
	PROTOCOL_SS_FILESEND_REQ,				//SC가 SCM에 업데이트를 위한 파일 전송 요청
	PROTOCOL_SS_FILESEND_HEAD,				//업데이트를 위한 서버파일 전송 시작
	PROTOCOL_SS_FILESEND_BODY,				//업데이트를 위한 서버파일 전송 데이터
	PROTOCOL_SS_MAX
};

typedef struct _scList
{
	TCHAR	_strIp[IP_ADDR_SIZE]; 
	UINT16	_ui16Port;
	UINT8	_ui8Percent;
	BOOL	_bIsConnect;
}SC_LIST;

typedef struct _workData
{
	UINT16			_ui16Protocol;
	TCHAR			_WorkData[320];
}WORK_DATA;

//PROTOCOL_SC_OPER
//PROTOCOL_SS_OPER
typedef struct _commonServerOperReq
{
	TCHAR	_strIp[IP_ADDR_SIZE];	//서버IP 주소
	TCHAR	_strServerName[64];		//서버이름	(서비스 이름)
	INT8	_i8ServerOper;			//수행동작 1 : ON		2 : OFF
}P_COMMON_OPER_REQ;

//PROTOCOL_SC_UPDATE_SERVER_NOTI
typedef struct _scUpdateServerNoti
{
	UINT8	_ui8TotalPercent;		//전체 업데이트 현황 Percentage
	UINT8	_ui8Percent;			//현재업데이트 중인 SC중의 하나의 업데이트 Percentage
	TCHAR	_strIp[IP_ADDR_SIZE];	//현재업데이트 중인 SC중의 하나의 IP 주소	
}P_SC_UPDATE_SERVER_NOTI;

//PROTOCOL_SS_UPDATE_NOTI
typedef struct _ssUpdateServerNoti
{	
	TCHAR	_strIp[IP_ADDR_SIZE];	//SC IP 주소
	UINT8	_ui8Percent;			//현재 업데이트 진행 Percentage
}P_SS_UPDATE_SERVER_NOTI;

//PROTOCOL_SC_UPDATE_SERVER_ACK
typedef struct _scUpdateServerAck
{
	UINT8	_ui8UpdateResult;
}P_SC_UPDATE_SERVER_ACK;

//PROTOCOL_SC_CONTROLTOOL_UPDATE_ACK
typedef struct _scControlToolUpdateAck
{
	TCHAR	_md5[MD5_SIZE];	//최신버전 ControlTool MD5 해쉬값
}P_SC_CONTROLTOOL_UPDATE_ACK;

//PROTOCOL_SC_FILELIST_INFO
typedef struct _scFileListInfo
{
	BOOL	_bIsListEnd;	//현재 리스트가 마지막 내용인가?
	UINT16	_ui16ListSize;	//해당 패킷에 전송된 List 길이
}P_SC_FILELIST_INFO;

//PROTOCOL_SC_FILESEND_REQ
typedef struct _scFileSendReq
{
	TCHAR	_filename[MAX_PATH];	//전송요청된 파일명
}P_SC_FILESEND_REQ;

//PROTOCOL_SC_FILESEND_HEAD
typedef struct _scFileSendHead
{	
	TCHAR	_filename[MAX_PATH];	//전송요청된 파일명
	UINT32	_ui32FileSize;			//전송할 파일의 전체 크기
}P_SC_FILESEND_HEAD;

//PROTOCOL_SC_TOOLSEND_HEAD
typedef struct _scToolSendHead
{		
	UINT32	_ui32FileSize;		//전송할 파일의 전체 크기
}P_SC_TOOLSEND_HEAD;

//PROTOCOL_SC_TOOLSEND_BODY
//PROTOCOL_SC_FILESEND_BODY
typedef struct _scFileSendBody
{
	BOOL	_bIsFileEnd;		//현재 패킷이 전송한 파일의 마지막 부분인가?
	UINT32	_ui32PacketSize;
}P_SC_FILESEND_BODY, P_SC_TOOLSEND_BODY;

typedef struct _scControlToolOn
{
	TCHAR	_strServerName[64];
}P_SC_CONTROLTOOL_ON;

//PROTOCOL_SC_SERVER_STATUS_REQ
typedef struct _scServerStatusReq
{
	TCHAR	_strIp[IP_ADDR_SIZE];
	INT8	_i8ServerType;		//서버 타입 (각 게임별로 별도의 규칙을 두고 정의합니다.)	
}P_COMMON_SERVER_STATUS_REQ;

//PROTOCOL_SC_SERVER_STATUS_ACK
typedef struct _scServerStatusAck
{
	TCHAR	_strIp[IP_ADDR_SIZE];
	INT8	_i8ServerType;		//서버 타입 (각 게임별로 별도의 규칙을 두고 정의합니다.)
	INT8	_i8ServerStatus;	//서버 현재 상태 - 1 : On		 2 : Off
}P_COMMON_SERVER_STATUS_ACK;

