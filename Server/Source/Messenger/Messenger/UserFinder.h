#ifndef _UserFinder_h
#define _UserFinder_h

#include "Community.h"

#define USE_TWO_POINTER

struct USER_NODE
{
	INT64		_i64UID;						// 
	INT32		_i32serveridx;					//
	INT32		_i32sessionidx;					//

	UINT32		_ui32Rank; 

	UINT32		_ui32idxBuffer;					// HashTableIdx;

	TTCHAR		_strnick[NET_NICK_NAME_SIZE];	// varchar32
	
	UINT32		_ui32connectState;				// 커뮤니티용 접속상태
	UINT32		_ui32LastRecvTimeST;			// Server Time

	//For Friends
	UINT32		_ui32friendCount;
	FRIEND_NODE _afriend[FRIEND_MAX_COUNT+10];	//송과장 이거 잡아줘요.. 

#if defined( USE_BLOCK_LIST )
	//For Block
	UINT32		_ui32BlockCount;
	BLOCK_NODE	_pBlock[BLOCK_MAX_COUNT];
#endif
	UINT8		_ui8NickColor;
	
};// SIZE(5360)

// 250000 - 노드 생성 개수
// 255000000  - 노드 제외한 메모리 예측 크기

// 5360 -> 1340000000
// 1595000000 - 예측 메모리
// 1340000000 - 차단추가시 노드 메모리

// 2952 -> 738000000
// 993000000 - 예측 메모리
// 738000000 - 차단제거시 노드 메모리

class CHashTable;
class CCircularIndex;

// 싱글 쓰레드만 지원합니다. 다른 쓰레드에서 접근하면 크래쉬 합니다.
class CUserFinder: public i3ElementBase
{
	I3_CLASS_DEFINE(CUserFinder);

private:
	#ifdef USE_TWO_POINTER 
	USER_NODE		**	m_ppUserNode;
	#else
	USER_NODE		*	m_pUserNode;
	#endif
	
	UINT32				m_ui32MaxCount;
	UINT32				m_ui32Count;
	UINT32				m_ui32Diviner;

	CHashTable		*	m_pUIDTable;		//UID - DataBase Idx 
	CHashTable		*	m_pNickTable;		//NickName Idx
	CCircularIndex	*	m_pEmptyBuffer;


public: 
	//For Time Out User 
	UINT32			GetMaxUserCount(void)			{ return m_ui32MaxCount; }
	#ifdef USE_TWO_POINTER
	USER_NODE	*	GetUserNode(INT32 i32idx)		{ return m_ppUserNode[i32idx]; }
	#else
	USER_NODE	*	GetUserNode(INT32 i32idx)		{ return &m_pUserNode[i32idx]; }
	#endif

	USER_NODE	*	GetFindUserNode(INT64 i64UID)	{ return _GetUserNode(i64UID); }

protected:
	USER_NODE	*	_GetUserNode(INT64 i64UID);

public:
	CUserFinder();
	virtual ~CUserFinder();

	BOOL	Create(UINT32 ui32MaxUser = MAX_CONNECT_USER_COUNT, UINT32 ui32Divisor = MAX_CONNECT_USER_DIVISOR);

	BOOL	Insert(USER_NODE* pNode);
	BOOL	Delete(INT32 i32ServerIdx, INT64 i64UID);
	
	BOOL	IsAliveUser(INT64 i64UID); 
	BOOL	UpdateByUid(USER_NODE* pNode);												//Uid, Id, Nick 변경시 사용해선 안됩니다.	
	BOOL	UpdateAlivePacket(INT64 i64UID, INT32 i32SessionIdx, INT32 i32ServerIdx);	//Update PacketRecvTime
	BOOL	UpdateNickName(INT64 i64UID,	TTCHAR * pstrNick);							//Change NickName
	BOOL	UpdateForNickName(INT64 i64UID, TTCHAR * pstrNick);
	BOOL	UpdateFriendListNickName(INT64 i64UID, UINT32 ui32FListIdx, TTCHAR * pstrNick, UINT8 ui8NickColor);
	BOOL	UpdateRank(INT64 i64UID, UINT32 ui32Rank);
	BOOL	UpdateFriendListRank(INT64 i64UID, UINT32 ui32FListIdx, UINT8 ui8Rank);
	BOOL	ChangeState(INT64 i64UID, UINT32 ui32state);

    BOOL	FindUid(	INT64 i64uid,	USER_NODE* pNode);		//UID 검색 (4바이트)
	BOOL	FindNick(	TTCHAR* pstrNick,	USER_NODE* pNode);		//닉네임 검색
	UINT32	GetUserCount(void)	{ return m_ui32Count; }	

	//유저의 현재 정보 받기 
	BOOL	GetUserInfoByUID(	INT64 i64UID, INT32* pi32SessionIdx,INT32* pi32ServerIdx);
	BOOL	GetUserInfoByNick(	TTCHAR * pstrNick, INT64 * pi64OutUID, INT32 * pi32OutSessionIdx,INT32 * pi32OutServerIdx);
	BOOL	GetCurrentUserInfoByUID(  INT64 i64UID, INT32* pi32SessionIdx,INT32* pi32ServerIdx, TTCHAR * pstrNick);
};

#endif //_UserFinder_h
