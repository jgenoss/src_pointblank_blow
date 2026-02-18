#ifndef __ROOM_MANAGER_
#define __ROOM_MANAGER_

#include "UserManager.h"

class CRoom; 
class CUserSession;
/*-----------------------------------------------------------------------
Name : CRoomRoomManager
2007 03 31 : 소스 정리, 버그확인 : 버그없음 : 
2007 05 03 : 리스트 정리 모두다 리스트로 만들기 
-----------------------------------------------------------------------*/
class CRoomManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CRoomManager );
protected: 
	//=====================================================================================================================================//
	//For Resource Manage	
	i3List			**	m_ppChannelResRoomList; 
	INT32				m_i32UseRoomCount;			//Update함수에서 업데이트
	INT32				m_i32AccRoomCount;			//Update함수에서 업데이트
	
	//=====================================================================================================================================//
	//For Resource Channel Manage 
	i3List			**  m_ppChannelRoomList;		//차후 변경가능 지금은 카운트용으로 사용되고 있음(추후 작업이 있을것 같아 유지)
	i3Mutex			**	m_ppcsChannelRoom;
	UINT32				m_ui32EachChannelMaxCount;
	UINT32				m_ui32ChannelCount;	

	//Check Change RoomList 	
	UINT32				m_ui32ChangeRoomInfo; 
	
	UINT32			*	m_pui32ChangeRoomListTime;	//이두개의 값을 가지고 룸인포를 업데이트 합니다. (m_pRoomRoomInfo0, m_pRoomRoomInfo1)
	UINT32			*	m_pui32RoomListUpdateTime;	//이두개의 값을 가지고 룸인포를 업데이트 합니다. (m_pRoomRoomInfo0, m_pRoomRoomInfo1)

	//=====================================================================================================================================//
	//Update Value 	
	INT32			*	m_pi32InfoSide; 	
	ROOM_INFO_BASIC	**	m_ppRoomRoomInfo0;
	UINT32			*	m_pui32RoomCount0;	
	ROOM_INFO_BASIC	**	m_ppRoomRoomInfo1;
	UINT32			*	m_pui32RoomCount1;	
	
protected: 	
	void				_DestroyRoom(		CRoom * pRoom);

public :
	CRoomManager(); 
	~CRoomManager(); 

	void OnCreate(void); 
	void OnUpdate(void); 
	void OnDestroy(void); 

	void Dump(void); 

	//=====================================================================================================================================//
	//채널 룸 관리 
	UINT32	GetChannelCount(void)						 { return m_ui32ChannelCount;		}
	INT32	GetChannelUseRoomCount(INT32 i32ChannelNum)
	{ 
		if(m_pi32InfoSide[i32ChannelNum] == 0 )return m_pui32RoomCount0[i32ChannelNum]; 
		return m_pui32RoomCount1[i32ChannelNum];
	}	


	INT32	GetChannelUserCount(INT32 i32ChannelNum)
	{
		return m_ppChannelRoomList[i32ChannelNum]->GetCount();
	}
	INT32	GetUseRoomCount(void)						{ return m_i32UseRoomCount; }	
	UINT32	GetRoomListUpdateTime(INT32 i32ChannelNum)		{ return m_pui32RoomListUpdateTime[i32ChannelNum]; }

	//룸 리스트 보내주기 
	void	OnSendRoomList(CUserSession * pSession, INT32 i32Channel, LOBBY_USER_INFO* pLobbyUserInfo, UINT32 ui32UserCount );				//메인, 유저, 로비 리스트
	CRoom *	GetRoom(INT32 i32ChannelNum, INT32 i32Idx)		{ return (CRoom *)m_ppChannelResRoomList[i32ChannelNum]->GetItem(i32Idx); }

	//=====================================================================================================================================//
	//유저 요청 함수 
	
	INT32	OnCreateRoom(		CUserSession * pSession, ROOM_INFO_CREATE* pInfoCreate );
	INT32	OnQuickJoinRoom(	CUserSession * pSession, INT32 i32Channel);
	BOOL	CheckRoomMode(		UINT8 ui8Mode, CRoom * pRoom );
	INT32	OnJoinRoom(			CUserSession * pSession, INT32 i32ChannelNum, UINT32 ui32RoomIdx, ROOM_INFO_JOIN* pInfoJoin );				
	INT32	OnLeaveRoom(		CUserSession * pSession, INT32 i32ChannelNum, INT32 i32WorkThread, INT32 i32EndType );							//메인쓰레드, 워킹쓰레드에서 호출 가능 
};

extern CRoomManager * g_pRoomManager;

#endif