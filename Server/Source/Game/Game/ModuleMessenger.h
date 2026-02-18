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

	CONNECT_SERVER_NET_BUFFER	m_sMessengerMainMessage;
	CONNECT_SERVER_NET_BUFFER	m_sMessengerAuthMessage;
	CONNECT_SERVER_NET_BUFFER	m_sMessengerClanMessage;
	CONNECT_SERVER_NET_BUFFER	m_sMessengerMessMessage;
	CONNECT_SERVER_NET_BUFFER*	m_pMessengerUserMessage;
	
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
	BOOL	MessengerSendNoteByUID(		INT32 WorkIdx, CUserSession * pSession, T_UID UID, UINT8 ui8messageSize, TTCHAR * pstrMessage);
	BOOL	MessengerSendNote(			INT32 WorkIdx, CUserSession * pSession, UINT8 ui8nicknameSize, UINT8 ui8messageSize, TTCHAR * pstrNickname, TTCHAR * pstrMessage);
	BOOL	MessengerCheckReadedNote(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8readCount, INT32* pi32noteIdList);
	BOOL	MessengerDeleteNote(		INT32 WorkIdx, CUserSession * pSession, UINT8 ui8deleteCount, INT32* pi32noteIdList);
	BOOL	MessengerLogin(				INT32 WorkIdx, CUserSession * pSession );
	BOOL	MessengerLogout(			INT32 WorkIdx, T_UID i64UID);
	BOOL	MessengerSendServerEnter(	INT32 WorkIdx, CUserSession * pSession);
	BOOL	MessengerLeaveUser(			INT32 WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeNickname(	INT32 WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeRank(		INT32 WorkIdx, CUserSession * pSession);
	BOOL	MessengerChangeColorNick(	INT32 WorkIdx, CUserSession * pSession);
	BOOL	MessengerSendClanNote(		INT32 WorkIdx, CUserSession * pSession, T_UID receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, TTCHAR * pstrMessage, TTCHAR* pstrClanName );
	BOOL	MessengerSendClanGroupNote(	INT32 WorkIdx, CUserSession * pSession, UINT32 ui32Count, T_UID* receiverDbIdx, UINT32 ui32ClanIdx, UINT8 ui8isAskNote, UINT8 ui8messageSize, TTCHAR * pstrMessage, TTCHAR* pstrClanName );
	BOOL	MessengerAliveUser(			INT32 WorkIdx, INT32 i32SessionIdx, T_UID i64UID, TTCHAR* strNickName); 
	
#ifdef USE_MESS_FIND
	BOOL	MessengerFindUser(				INT32 WorkIdx, CUserSession * pSession, TTCHAR* findNick);										// 유저찾기
#endif
	BOOL	MessengerFriendAccept(			INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Idx);											//친구 허용하기		
	BOOL	MessengerSendFriendInsert(		INT32 WorkIdx, CUserSession * pSession, TTCHAR* pstrNickName);									//친구 추가하기		
	BOOL	MessengerSendFriendInviteUser(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8FriendIdx);									//친구 초대하기 
	BOOL	MessengerSendFriendDelete(		INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Idx);											//친구 삭제하기		
	BOOL	MessengerSendFriendRoomEnter(	INT32 WorkIdx, CUserSession * pSession, UINT8 ui8ChIdx, UINT16 ui16RoomIdx);					//친구 방에 들어가기 
	BOOL	MessengerSendFriendRoomLeave(	INT32 WorkIdx, CUserSession * pSession );														//친구 방에서 나오기 
	BOOL	MessengerSendResetFriendInfo(	INT32 WorkIdx, T_UID iDbIdx);																	// 친구 정보 초기화
	BOOL	MessengerSendChangeNickname(	INT32 WorkIdx, CUserSession * pSession, TTCHAR* pstrNickname);									// 닉네임 변경 통보
	BOOL	MessengerSendKickUser(			INT32 WorkIdx, CUserSession * pSession);														// GSM에서 Kick
	BOOL	MessengerSendWhisper(			INT32 WorkIdx, CUserSession * pSession, TTCHAR* pstrReceiverNick, INT16 i16messageSize, TTCHAR* pstrMessage, bool bIsGMUser );	// 귓속말 보내기
	BOOL	MessengerSendWhisperByUID(		INT32 WorkIdx, CUserSession * pSession, T_UID receiverUID, TTCHAR* strReceiverNick, INT16 i16messageSize, TTCHAR* pstrMessage, bool bIsGMUser );
	BOOL	MessengerSendChannelEnter(		INT32 WorkIdx, CUserSession * pSession, T_UID iUID, INT32 nChannelIdx = 0);
	BOOL	MessengerSendChannelLeave(		INT32 WorkIdx, CUserSession * pSession, T_UID iUID);

	BOOL	MessengerSendBlockInsert(		INT32 WorkIdx, CUserSession * pSession, UINT8 ui8Rank, char* strNickName, char* strComment );	//차단 추가하기		
	BOOL	MessengerSendBlockDelete(		INT32 WorkIdx, CUserSession * pSession, INT32 i32Idx );											//차단 삭제하기
	
	BOOL	MessengerSendChangeState(		INT32 WorkIdx, CUserSession * pSession, FRIEND_STATE eState );
	BOOL	MessengerSendNoteGiftNotice(	INT32 WorkIdx, T_UID UID, UINT8 ui8messageSize, TTCHAR * pstrMessage, UINT8 ui8NickSize, TTCHAR *pSenderNick);
	BOOL	MessengerSendNoteUserGiftNotice(INT32 WorkIdx, CUserSession * pSession, T_UID UID, UINT8 ui8messageSize, TTCHAR * pstrMessage);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

extern CModuleMessenger * g_pModuleMessenger; 

#endif
