#ifndef __MODULE_MESSENGER_H__
#define __MODULE_MESSENGER_H__

class CUserSession;
class CModuleMessengerSocket;

class CModuleMessenger : public i3Thread
{
	I3_CLASS_DEFINE( CModuleMessenger );
private:
	BOOL						m_bRunning;
	UINT32						m_ui32ThreadCount;
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Messenger
	CModuleMessengerSocket*		m_pMessengerSocket;
	UINT32						m_ui32MessengerConnectTime;

	SERVER_NET_BUFFER		m_sMessengerMainMessage;
	SERVER_NET_BUFFER		m_sMessengerAuthMessage;
	SERVER_NET_BUFFER		m_sMessengerClanMessage;
	SERVER_NET_BUFFER		m_sMessengerMessMessage;
	SERVER_NET_BUFFER*		m_pMessengerUserMessage;
	
protected:
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Messenger
	INT32						_MessengerConnect(void);
	INT32						_MessengerUpdate(void); 

public: 
	CModuleMessenger(void); 
	virtual ~CModuleMessenger(void); 

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Common
	INT32						OnCreate( UINT32 ui32ThreadCount );
	void						OnDestroy(void);
	virtual UINT32				OnRunning( void * pUserData);
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//MESSENGER SERVER	
	BOOL	MessengerSendNoteByUID(		INT32 i32WorkIdx, CUserSession * pSession, TUID UID, UINT8 ui8messageSize, char * pstrmessage);
	BOOL	MessengerSendNote(			INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8nicknameSize, UINT8 ui8messageSize, char * pstrnickname, char * pstrmessage);
	BOOL	MessengerCheckReadedNote(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8readCount, INT32* pi32noteIdList);
	BOOL	MessengerDeleteNote(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8deleteCount, INT32* pi32noteIdList);
	BOOL	MessengerLogin(				INT32 i32WorkIdx, CUserSession * pSession );
	BOOL	MessengerLogout(			INT32 i32WorkIdx, TUID i64UID);
	BOOL	MessengerSendServerEnter(	INT32 i32WorkIdx, CUserSession * pSession);
	BOOL	MessengerLeaveUser(			INT32 i32WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeNickname(	INT32 i32WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeRank(		INT32 i32WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeColorNick(	INT32 i32WorkIdx, CUserSession * pSession);
	BOOL	MessengerSendClanNote(		INT32 i32WorkIdx, CUserSession * pSession, TUID receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, char * pstrmessage, char* pstrClanName );
	BOOL	MessengerSendClanGroupNote(	INT32 i32WorkIdx, CUserSession * pSession, UINT32 ui32Count, TUID* preceiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, char * pstrmessage, char* pstrClanName );
	BOOL	MessengerAliveUser(			INT32 i32WorkIdx, INT32 iSessionIdx, TUID i64UID, char* pstrNickName); 
	
#ifdef USE_MESS_FIND
	BOOL	MessengerFindUser(				INT32 i32WorkIdx, CUserSession * pSession, char* pstrfindNick);											// 유저찾기
#endif
	BOOL	MessengerFriendAccept(			INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Idx);													//친구 허용하기		
	BOOL	MessengerSendFriendInsert(		INT32 i32WorkIdx, CUserSession * pSession, char* pstrNickName);											//친구 추가하기		
	BOOL	MessengerSendFriendInviteUser(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8FriendIdx);										//친구 초대하기 
	BOOL	MessengerSendFriendDelete(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Idx);												//친구 삭제하기		
	BOOL	MessengerSendFriendRoomEnter(	INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8ChIdx, UINT16 ui16RoomIdx);							//친구 방에 들어가기 
	BOOL	MessengerSendFriendRoomLeave(	INT32 i32WorkIdx, CUserSession * pSession );														//친구 방에서 나오기 
	BOOL	MessengerSendResetFriendInfo(	INT32 i32WorkIdx, TUID iDbIdx);																	// 친구 정보 초기화
	BOOL	MessengerSendChangeNickname(	INT32 i32WorkIdx, CUserSession * pSession, char* pstrnickname);											// 닉네임 변경 통보
	BOOL	MessengerSendKickUser(			INT32 i32WorkIdx, CUserSession * pSession);																	// GSM에서 Kick
	BOOL	MessengerSendWhisper(			INT32 i32WorkIdx, CUserSession * pSession, char* receiverNick, INT16 messageSize, char* message);		// 귓속말 보내기
	BOOL	MessengerSendWhisperByUID(		INT32 i32WorkIdx, CUserSession * pSession, TUID receiverUID, char* receiverNick, INT16 messageSize, char* message);
#ifdef USE_UNDECIDED_DESIGN_TEST
	BOOL	MessengerSendChannelEnter(		INT32 i32WorkIdx, CUserSession * pSession, TUID iUID, INT32 nChannelIdx = 0);
#endif

	BOOL	MessengerSendBlockInsert(		INT32 i32WorkIdx, CUserSession * pSession, UINT8 ui8Rank, char* strNickName, char* strComment );	//차단 추가하기		
	BOOL	MessengerSendBlockDelete(		INT32 i32WorkIdx, CUserSession * pSession, INT32 i32Idx );											//차단 삭제하기
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

extern CModuleMessenger * g_pModuleMessenger; 

#endif
