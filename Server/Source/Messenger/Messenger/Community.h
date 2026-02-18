#ifndef _Community_h
#define _Community_h

struct FRIEND_NODE
{
	INT64	_i64fuid;
	INT32	_i32state;
	UINT8	_ui8rank; 
	TTCHAR	_strnick[NET_NICK_NAME_SIZE];
	UINT8	_ui8nickcolor;
};// SIZE(48)

struct BLOCK_NODE
{
	INT64	_i64buid;
	INT32	_i32Idx;	// DB Index
	UINT8	_ui8rank;
	TTCHAR	_strNick[NET_NICK_NAME_SIZE];
};// SIZE(48)

class CCommunity: public i3ElementBase
{
	I3_CLASS_DEFINE(CCommunity);
public:
	CCommunity();
	virtual ~CCommunity();

	INT32	MakeFriendList(		INT64 i64uid );
	INT32	AddFriend(			INT64 i64uid, INT64 i64fuid );
	INT32	AuthFriend(			INT64 i64uid, UINT32 ui32idx );
	INT32	DeleteFriend(		INT64 i64uid, UINT32 ui32idx );
	INT32	InvitedFriend(		INT64 i64uid, UINT32 ui32idx );
	INT32	ChangeState(		INT64 i64uid, UINT32 ui32state );
	void	ChangeNickname(		INT64 i64uid, const char* pstrnickname );
	void	ChangeRank(			INT64 i64uid, UINT32 ui32rank );
	INT32	ResetCommunityInfo(	INT64 i64uid );
};

#endif
