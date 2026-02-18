////////////////////////////////////////////////////////////////////////////////////////////////////
//	Module Name:
//		ShareDB2MessServerProtocolStructure.h
//	Author:
//		Ji-won Kang.
//	Description:
//		Structures used between Database to Messenger Server. 
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SHARE_DB2MESSSERVER_PROTOCOL_STRUCTURE_H__
#define __SHARE_DB2MESSSERVER_PROTOCOL_STRUCTURE_H__

//#include "../../CommonSource/CommonDef.h"
//#include "../../CommonServerSource/CommonServerDef.h"

#include "UserFinder.h"
#include "Community.h"
#include "DataBase_Script.h"

class i3ODBC; 
class CADODatabase; 
class CNoteBox;
class CConnectedUser;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------
//				Database <-> MesssengerServer
//---------------------------------------------------------------------------------

//==========================================================================================================================================================================
//GetFriendList
typedef struct In_FriendList
{	
	INT64				i64uid;
	UINT32				iRank;
	BOOL				bReset;		// 친구리스트를 초기화 할때 사용합니다.( TRUE이면 모두 리셋)
}IN_FRIEND_LIST;

typedef struct In_BlockList
{	
	INT64				_i64uid;
}IN_BLOCK_LIST;

typedef struct Out_FriendList
{
	INT32				result;
	INT64				i64uid;
	UINT32				iRank;
	UINT32				friendCount;					
	FRIEND_NODE			friendNode[FRIEND_MAX_COUNT];
}OUT_FRIEND_LIST;

typedef struct BlockComment
{
	TTCHAR				_strComment[BLOCK_COMMENT_MAX];	 
}BLOCK_COMMENT;

typedef struct Out_BlockList
{
	INT32				_i32result;
	INT64				_i64uid;
	UINT32				_ui32blockCount;					
	BLOCK_COMMENT		_pBlockComment[BLOCK_MAX_COUNT];
	BLOCK_NODE			_pBlockNode[BLOCK_MAX_COUNT];
}OUT_BLOCK_LIST;



// Friend Control ( ADD, DELETE, AUTH )
typedef struct In_FriendControl
{	
	FRIEND_WORK_TYPE	eWorkType;
	INT64				i64uid;
	INT64				i64fuid;
	UINT8				idx;				// AUTH에 사용됩니다.
	UINT8				ui8Rank;
	UINT8				ui8fRank;
	TTCHAR				strNick[ NET_NICK_NAME_SIZE ];
	UINT8				ui8NickColor;
	UINT8				ui8fNickColor;
}IN_FRIEND_CONTROL;

typedef struct Out_FriendControl
{	
	INT32				result;
	INT64				i64uid;
	INT64				i64fuid;
	FRIEND_WORK_TYPE	eWorkType;
	UINT8				idx;				// Auth에 사용됩니다.
	FRIEND_NODE			friendNode;			// 친구의 친구 리스트
	FRIEND_NODE			targetFriendNode;	// 자신의 친구 리스트
}OUT_FRIEND_CONTROL;

// Block Control ( ADD, DELETE )
typedef struct In_BlockControl
{	
	INT32				_i32type;
	INT32				_i32Idx;	//  DB Index - Delete에 사용
	INT64				_i64uid;
	INT64				_i64buid;
	UINT8				_ui8Rank;	// Add에 사용
	char				_strNick[ NET_NICK_NAME_SIZE ];
	char				_strComment[ BLOCK_COMMENT_MAX ];	// Add에 사용.
}IN_BLOCK_CONTROL;

typedef struct Out_BlockControl
{	
	INT32				_i32result;
	INT32				_i32Idx;		// DB Index
	INT64				_i64uid;
	INT64				_i64buid;
	INT32				_i32type;
	BLOCK_NODE			_pBlockNode;	// 자신의 차단 리스트
	char				_strComment[ BLOCK_COMMENT_MAX ];	// Add에 사용.
}OUT_BLOCK_CONTROL;

// Change Nick
typedef struct In_ChangeNick
{	
	FRIEND_WORK_TYPE	eWorktype;
	INT64				i64uid;
	INT64				i64fuid;
	UINT8				idx;				// AUTH에 사용됩니다.
	TTCHAR				strNick[ NET_NICK_NAME_SIZE ];
	UINT8				ui8NickColor;
}IN_CHANGE_NICK;

typedef struct Out_ChangeNick
{	
	INT32				result;
	INT64				i64uid;
	INT64				i64fuid;
	FRIEND_WORK_TYPE	eWorktype;
	UINT8				idx;				// Auth에 사용됩니다.
	TTCHAR				strNick[ NET_NICK_NAME_SIZE ];
	UINT8				ui8NickColor;
}OUT_CHANGE_NICK;

// Change Rank
typedef struct In_ChangeRank
{	
	INT32				type;
	INT64				i64uid;
	INT64				i64fuid;
	UINT8				idx;				// AUTH에 사용됩니다.
	UINT8				rank;
}IN_CHANGE_RANK;

typedef struct Out_ChangeRank
{	
	INT32				result;
	INT64				i64uid;
	INT64				i64fuid;
	INT32				type;
	UINT8				idx;				// Auth에 사용됩니다.
	UINT8				rank;
}OUT_CHANGE_RANK;

//==========================================================================================================================================================================
//User Info Search
typedef struct In_UserInfo
{
	INT64	i64UID;
	TTCHAR	strNick[NET_NICK_NAME_SIZE];
}IN_USER_INFO_SEARCH;

typedef struct Out_UserInfo
{
	INT32	i32Reault;
	INT64	i64UID;
	INT64	i64SearchUID;
}OUT_USER_INFO_SEARCH;

//==========================================================================================================================================================================
//Note 
typedef struct In_NoteList
{
	INT64		i64UID; 
}IN_NOTE_LIST; 

typedef struct Out_NoteList
{
	INT64		i64UID; 
	INT32		iResult;
	INT32		iCount;
	NOTE_NODE	stNode[MAX_NORMAL_NOTE_COUNT]; 
}OUT_NOTE_LIST; 

typedef struct In_NoteCommand
{	
	INT64		i64UID;
	INT8		iType; 
	UINT32		iIdx; 
	UINT8		iCount; 
	UINT32		pList[MAX_NORMAL_NOTE_COUNT];
	//일반	//클랜
	INT64		i64ReceiverUID;						//X			O
	TTCHAR		strNickSend[NET_NICK_NAME_SIZE];	//O			O				//따로 갈까?
	TTCHAR		strNickRecv[NET_NICK_NAME_SIZE];	//O			X				//따로 갈까?
	TTCHAR		strMess[NET_NOTE_SIZE];				//O			O				//따로 갈까?
	UINT8		iNickSendSize;						//X			O				//따로 갈까?
	UINT8		iMessSize; 							//O			O				//따로 갈까?
	UINT32		iAppendingData;						//X			O				//따로 갈까?
}IN_NOTE_COMMAND; 

typedef struct Out_NoteInfo
{
	INT64		i64UID; 
	INT64		i64RecvUID;
	INT32		iResult;
	INT8		iType; 
	UINT8		iCount; 
	UINT32		pList[MAX_NORMAL_NOTE_COUNT];
}OUT_NOTE_INFO; 

typedef struct Out_NoteCommand
{	
	OUT_NOTE_INFO		note_info;
	NOTE_NODE			note_node;
}OUT_NOTE_COMMAND; 

//==========================================================================================================================================================================
typedef struct _DataQueryInfo
{
	CADODatabase	*	_pDB;
	char				_pQuery[SQL_STRING_COUNT];
}DB_QUERY_INFO;

#endif // __SHARE_DB2MESSSERVER_PROTOCOL_STRUCTURE_H__