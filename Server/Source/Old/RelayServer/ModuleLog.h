#ifndef __MODULE_LOG_H__
#define __MODULE_LOG_H__

#include "ModuleLogSocket.h"

enum LOG_MESSAGE
{
	//시스템 로그 
	LOG_MESSAGE_		= 0,		
	LOG_MESSAGE_SERVER_START,	//완료
	LOG_MESSAGE_SERVER_END,		//완료
	LOG_MESSAGE_SERVER_STATE,	//완료																																																																																									
};

typedef struct 
{	
	INT16	_Size; 
	char	_szMessage[4096];		//충분
}LOG_SEND_MESSAGE;


class CModuleLog : public i3Thread
{
	I3_CLASS_DEFINE( CModuleLog );
private:	
	HANDLE				m_hKillEvent;

	//로그
	CModuleLogSocket	*	m_pLogModuleSocket; 

	LOG_SEND_MESSAGE	*	m_pMainLogMessage;				// Main Thread 
	INT32					m_MainLog_WritePos;				//
	INT32					m_MainLog_ReadPos;				//
protected:
	void	SendInitMessage(void); 
public: 
	CModuleLog(void); 
	virtual ~CModuleLog(void); 
	
	BOOL	OnCreate( void ); 
	void	OnDestroy(void); 	
	void	SendLogMessage( LOG_MESSAGE LogId );						//공용 메시지 
	virtual UINT32 OnRunning( void * pUserData); 
};

extern CModuleLog * g_pModuleLog; 

#endif