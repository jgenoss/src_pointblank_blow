#ifndef _RelayGroupDef_h
#define _RelayGroupDef_h

#define INVALID_RELAY_GROUP_ID		(0xFFFFFFFF)
#define MAX_SERVER_COUNT	5
#define MAX_CHANNEL_COUNT	VIEW_CHANNEL_COUNT
#define MAX_ROOM_COUNT		VIEW_ROOM_COUNT
#define INVALID_SLOT_IDX	(-1)
#define UDP_SEND_PACKET_SIZE		2048
#define GROUP_DESTORY_TIME			300		// 60*5
#define UPDATE_CHECK_TIME			1		// 1
#define UPDATE_CHECK_JUMP_COUNT		5		

enum IoType
{
	IO_RECEIVE = 0,
	IO_SEND
};

enum ENUM_GROUP_STATE
{
	RGS_INVALID = -1,	// 초기화 되지 않았다
	RGS_READY,			// 초기화 완료
	RGS_USE,			// 멤버참여 대기
};

#pragma pack( push, 1 )

#define RELAY_MEMBER_RECV_SIZE		9
// sizeof(UINT32) + sizeof(UINT32) + sizeof(UINT8)
typedef struct __tag_Relay_Member_Info
{
	UINT32		_wGroupIdx;
	UINT32		_wEncryptKey;
	UINT8		_cSlotIdx;
	UINT32		_wIp;
	UINT16		_sPort;
	
} RELAY_MEMBER_INFO;

#pragma pack( pop )

#endif
