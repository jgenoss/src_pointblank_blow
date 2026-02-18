// 
//
// YouJong Ha
//	Last update : 2011-08-24 (yyyy:mm:dd)
//	
// Description:
//	
//
//
// Dependency: 
//		
//	
#ifndef _PxDef_h
#define _PxDef_h

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P)          (P)
#endif 

#define INVALID_RELAY_GROUP_ID		(0xFFFFFFFF)
#define INVALID_SLOT_IDX			(-1)
#define	INVALID_OBJ_IDX				(-1)

#define MAX_SERVER_COUNT	16					// for dedicated srver ip list
#define DEF_SERVER_COUNT	1
#define DEF_SERVER_IP		"127.0.0.1"
#define DEF_RAY_COUNT		3

#define MAX_CHANNEL_COUNT	VIEW_CHANNEL_COUNT
#define MAX_ROOM_COUNT		VIEW_ROOM_COUNT
#define MAX_WEAPON_COUNT	MAX_INVEN_COUNT		// 현재 224개 사용 (확장을 위해 2바이트 인덱싱)

#define UDP_SEND_PACKET_SIZE		2048


#define UPDATE_CHECK_TIME			1		// 1
#define UPDATE_CHECK_JUMP_COUNT		5		

#define BUFFERS_PER_TRACKBUFFER		1024  // allocated buffers per socket.
										  // each socket uses 2 track bufers; one for send, one for recv
										  // and each track buffer manages this number of buffers.

#define DS_STAGE_INFO_SEND_COUNT	3

// ------------------------------------------------------------------
// Framework
// ------------------------------------------------------------------
#define PX_MAX_TASK_THREAD_COUNT		63	// max task processor threads	
#define PX_MAX_MONITOR_THREAD_COUNT		1	// max monitor threads

#define PX_TASK_TIMEOUT						16	// ms
#define PX_TASK_EPSILON_TIME				1
#define PX_DEFAULT_PORT_BASE				50000
#define PX_DEFAULT_TASK_THREAD_COUNT		10
#define PX_DEFAULT_MONITOR_THREAD_COUNT		1
#define PX_DEFAULT_LOG_LEVEL				0	// 0: all 1: warning 2: error

// thread type
enum DS_THREAD_TYPE
{
	THREAD_TYPE_IO = 0,
	THREAD_TYPE_TASK,
	THREAD_TYPE_MONITOR,
	THREAD_TYPE_UNKNOWN = -1		
};


// ------------------------------------------------------------------
// 데디케이션 서버에서 게임서버에 보내는 정보를 위한 플래그
// ------------------------------------------------------------------
#define DS_SEND_USER_KILL				1UL
#define DS_SEND_MISSION_TRIGGER			2UL
#define DS_SEND_MISSION_DESTRUCTION		4UL
#define DS_SEND_MISSION_DEFENCE			8UL
#define DS_SEND_CHEATING_REPORT			16UL

// ------------------------------------------------------------------
// Check List for Cheating
// ------------------------------------------------------------------
#define DS_CHECKLIST_WORLD_TEST			0x0001	// world ray cast
#define DS_CHECKLIST_CHARACTER_INFO		0x0002	// 무기정보, 캐릭터특성, 아이템

#pragma pack( push, 1 )

///////////////////////////////////////////////////////////////////////////////////////////
// *.i3scol 파일 구조
// ------------------
//	[ DS_I3SCOL_HEADER	]
//	[ DS_I3SCOL_DESC	] 
//  [ DS_I3SCOL_DESC	]
//	....
//  [ DS_I3SCOL_DESC	]  : header의 count 만큼 반복.
//  [ vertex list ][face list][nxData] : 가변 길이
//  [ vertex list ][face list][nxData]
//  ....
//  [ vertex list ][face list][nxData]
//  

#define I3SCOL_VERSION		MAKELONG(1, 0)

// DS_I3SCOL_HEADER
struct DS_I3SCOL_HEADER
{
	DWORD				size;		// this structure's size
	DWORD				version;	// file version
	DWORD				count;		// count of the collision obj. 
	char				pad[4];		// reserved
};

// DS_I3SCOL_DESC를 이용해서 NxShapeSet을 구성한다
struct DS_I3SCOL_DESC
{
	NxMat34				transform;			// matrix for pos, orientation, scale
	DWORD				shapeType;			// shape type (triangle mesh? convex?)
	DWORD				group;				// collision test group
	NxF32				restitution;		// for creating material
	NxF32				staticFriction;		// ..
	NxF32				dynamicFriction;	// ..
	// mesh data
	DWORD				toffset;			// terrain list position
	DWORD				tsize;				// terrain list size (bytes)
	DWORD				tstep;				// size of the list item
	DWORD				voffset;			// vertex list position 
	DWORD				vsize;				// vertex list size (bytes)
	DWORD				vstep;				// size of the list item  
	DWORD				foffset;			// face list position
	DWORD				fsize;				// face list size (bytes)
	DWORD				fstep;				// size of the list item
	// physX data
	DWORD				nxoffset;			// nxData offset
	DWORD				nxsize;				// nxData size (bytes)
};

#pragma pack( pop )

//////////////////////////////////////////////////////////////////////////
// MAP MANAGER
typedef STAGE_ORDINAL_ID	STAGEORD;
#define DS_MAX_MAP			STAGE_ORD_MAX		// 최대 맵 인덱스 
												// : 이 값 보다 작아야 한다. 현재 35개)
//#define DS_MAX_MAP			2				// : 1보다 커야 한다.
#define DS_MIN_MAP				1

// path
#define MEDIA_PATH				"Media"					// base path
#define SOBJ_FILE_EXTENSION		"i3sobj"				// game object
#define SCOL_FILE_EXTENSION		"i3scol"				// collision object

//////////////////////////////////////////////////////////////////////////
// PHYSICS
#define DS_MAX_ACTORS		256		// 월드 하나에 포함할 수 있는 최대 액터 수
#define GRAVITY_CONSTANT	9.8		// 중력 상수

#define DS_MAX_ROOM			4000	// 최대 ROOM
#define DS_MAX_RAYCAST_HITS	1		// size of the raycast hit result array. 

#define SAFE_DELETE(p)	{ if (p) { delete(p); p = NULL; } }
#define SAFE_FREE(p)	{ if (p) { free(p); p = NULL; }	 }
#endif
