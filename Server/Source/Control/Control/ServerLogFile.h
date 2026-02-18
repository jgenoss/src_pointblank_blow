#ifndef __SERVER_LOGFILE_H__
#define __SERVER_LOGFILE_H__

class CServerLogFile : public i3Thread
{
	I3_CLASS_DEFINE( CServerLogFile ); 
private:	
	HANDLE	m_hFile;
	INT32	m_i32Hour;
	INT32	m_i32ServerLogSaveTime;
	BOOL	m_bRunning; 
	INT32	m_i32ServerLogIdx;

protected:
	BOOL FileOpen(void);

public: 
	CServerLogFile(void);
	virtual ~CServerLogFile(void);

	virtual UINT32 OnRunning( void * pUserData );

	BOOL	OnCreate(void);
	void	OnUpdate(void);
	void	OnDestroy(void);
	void	WriteServerLog(void);
};

extern CServerLogFile * g_pServerLogFile;

#endif
