#ifndef __MODULE_DB_USER_INFO_H__
#define __MODULE_DB_USER_INFO_H__

class CModuleDBUserInfo
{
	INT32					m_i32LogIdx;
	HANDLE					m_hThread;
	BOOL					m_bRunning;

	CADODatabase*			m_pDB;
	i3RingBuffer*			m_pRingIn;
	i3RingBuffer*			m_pRingOut;

	CDBConfig*				m_pUserDBConfig;

protected:
	void					_GetUserInfo( RING_USER_INFO_IN* pRingIn, RING_USER_INFO_OUT* pRingOut );

	void					ErrorDBUserInfo( wchar_t* wstrSQL );

public:
	CModuleDBUserInfo();
	~CModuleDBUserInfo();

	BOOL					Create();
	void					Destroy();
	void					Working();

	INT32					PushUserInfo( RING_USER_INFO_IN* pRingIn )		{	return m_pRingIn->Push( pRingIn );	}
	i3RingBuffer*			GetRingOutBuffer()								{	return m_pRingOut;	}
};

#endif