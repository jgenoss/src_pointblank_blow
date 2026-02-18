#ifndef __ACCOUNT_TRANS_H__
#define __ACCOUNT_TRANS_H__

class CAccountTrans
{
	HANDLE					m_hThread;
	UINT32					m_ui32ThreadID;
	BOOL					m_bRunning;

	CADODatabase*			m_pDB;
	CDBConfig*				m_pDBConfig;

	UINT32					m_ui32ResultTime;

public:
	CAccountTrans();
	~CAccountTrans();

	BOOL					Create();
	void					Destroy();

	void					Running();

	BOOL					ReConnect( wchar_t* wstrQuery );
};

#endif