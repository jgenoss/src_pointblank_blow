#ifndef __MODULE_EMAIL_H__
#define __MODULE_EMAIL_H__

class CModuleEmail : public CSmtpMail
{
	I3_CLASS_DEFINE( CModuleEmail );
private:	

	// PushEmail() 함수 호출하는 쓰레드가 추가되면 추가해줘야 합니다.
	Local_CRingBuffer	*		m_pEMailSendBufferMain;
	

public: 
	CModuleEmail(void); 
	virtual ~CModuleEmail(void); 	

	BOOL	OnCreate(void); 
	void	OnDestroy(void);
    	
	//virtual UINT32 OnRunning( void * pUserData ); 

	virtual bool OnRunningEx();

	void	PushEmail( INT32 i32ThreadIdx,  SEND_REASON eSendReason, void* pEmilInfo1 = NULL );
};

extern CModuleEmail * g_pModuleEmail; 

#endif
