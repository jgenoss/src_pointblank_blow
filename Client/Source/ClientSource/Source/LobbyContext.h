#pragma once

#define MAXIMUM_ROOM_COUNT		300		// 300개 이ㄴ상이되면 늘려야한다.
#define MAXIMUM_USER_COUNT		5000	// 한서버에 5천명까지

class LobbyContext : public i3::shared_ginst<LobbyContext>
{
public:
	~LobbyContext();

	void			InitLobbyData();
	void			InitLobbyUserData();

	void			SetLobbyUserCount( INT32 nCount )		{ m_nLobbyUserCount = nCount;	}
	INT32			GetLobbyUserCount( ) const				{ return m_nLobbyUserCount;		}

	void			setLobbyRoomCount( INT32 nValue )		{ m_LobbyRoomCount = nValue;	}
	INT32			getLobbyRoomCount(void)	const			{ return m_LobbyRoomCount;		}
	
	INT32			getLobbyRoomPage( void ) const			{ return m_LobbyRoomPage;		}
	void			setLobbyRoomPage( INT32 nValue )		{ m_LobbyRoomPage = nValue;		}

	const LOBBY_USER_INFO_LIST*		getLobbyUserInfo( INT32 idx ) const; 
	const LOBBY_USER_INFO_LIST*		getLobbyUserInfoByNick(const char *) const;

	void			SetLobbyUserInfo(INT32 PageNum, INT32 InfoCount, const LOBBY_USER_INFO_LIST * pUserList);

	const ROOM_INFO_BASIC *	getRoomInfo( INT32 idx ) const;		
	void			SetRoomInfo( INT32 PageNum, INT32 InfoCount, const ROOM_INFO_BASIC * pRoomList);

	INT32 				getUserSessionIdxByUserNickName(const i3::wliteral_range& wstrNickName ) const;	// 특정 유저의 닉네임으로 세션 인덱스 가져오기. 2011-03-07 노준영
//	INT32 				getUserSessionIdxByUserNickName(const char* szNickName ) const;	// 특정 유저의 닉네임으로 세션 인덱스 가져오기. 2011-03-07 노준영

	INT32				getSelectedRoomIdx(void) const			{ return m_SelectedRoomIdx; }
	void				setSelectedRoomIdx( INT32 v)			{ m_SelectedRoomIdx = v; }
	const ROOM_INFO_ADD *	getSelectedRoomInfoAdd(void) const	{ return &m_SelectedRoomInfoAdd; }
	ROOM_INFO_ADD *			setSelectedRoomInfoAdd(void)		{ return &m_SelectedRoomInfoAdd; }

	const COUPON_INFO*	getCouponInfo( void) const			{ return &m_couponInfo; }
	COUPON_INFO *		setCouponInfo( void)				{ return &m_couponInfo; }
	void				clearCouponInfo(void)				{ i3mem::FillZero(&m_couponInfo, sizeof(m_couponInfo));	}

private:
	
	//Lobby 유저 리스트	(C->G)	
	i3::vector<LOBBY_USER_INFO_LIST*> m_LobbyUserListInfo;
	INT32			m_nLobbyUserCount = 0;

	//Lobby 방정보		(C->G)	
	i3::vector<ROOM_INFO_BASIC*> m_RoomListInfo;
	INT32			m_LobbyRoomCount = 0;		//Lobby				(C->G)
	INT32			m_LobbyRoomPage = 0;		//Lobby				(G->C)
	INT32			m_LobbyRoomSortType = ROOM_SORT_DEFAULT;	//Lobby				// 거지같아 보인다..
	

	INT32			m_SelectedRoomIdx = 0;						//휘발성 변수 
	ROOM_INFO_ADD	m_SelectedRoomInfoAdd;					//클릭된 정보	(C->G)		// 서버구조체를 그대로 쓰는게 곤란할수 있으므로..
																						// 나중에 별도의 구조체로 변경해야한다...

	COUPON_INFO		m_couponInfo;			// 쿠폰정보

};

