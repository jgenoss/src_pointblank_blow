#ifndef __WEB_MANAGER_H__
#define __WEB_MANAGER_H__

#define WEB_SESSION_RECV_TIME_OUT			60 * 5			//5분입니다. 
#define WEB_SESSION_WORK_TIME_OUT			60 * 5			//5분입니다. 

class CWebSession;

class CWebSessionManager : public i3ElementBase
{
	I3_CLASS_DEFINE( CWebSessionManager );
protected: 		
	i3List					m_SessionList;

	//서버, 유저의 연결을 위해 필요함
	struct timeval			m_timeout;
	i3NetworkServerSocket	m_AcceptSocket;

public:
	CWebSessionManager(); 
	virtual ~CWebSessionManager();

	virtual BOOL			OnCreate(void);
	virtual void			OnUpdate(void);
	virtual BOOL			OnDestroy(void);

	void					Accept(void);
	void					Update(void);


	//Web tool 명령 전달
	void					SendKickUserWebReq( CWebSession *pSession, INT32 i32WebSessionIdx, INT8 i8Count, INT64 *pi64UID );
	void					RecvKickUserWebAck( INT32 i32WebSessionIdx, INT32 i32Rv );

	void					SendUserInfoLocationWebReq( CWebSession *pSession, INT32 i32WebSessionIdx, INT64 i64UID );
	void					RecvUserInfoLocationWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8ServerIdx = 0, INT8 i8ChannelNum = 0, INT32 i32RoomIdx = 0 );

	void					SendClanInfoBattleWebReq( CWebSession *pSession, INT32 i32WebSessionIdx, UINT32 ui32CID );
	void					RecvClanInfoBattleWebAck( INT32 i32WebSessionIdx, INT32 i32Rv, INT8 i8Count = 0 );

	void					SendNoticeWebReq( UINT8 ui8Type, TTCHAR* pstrMessage, UINT16 ui16Size, INT32 i32RGB);
};

extern CWebSessionManager * g_pWebSessionManager;

#endif