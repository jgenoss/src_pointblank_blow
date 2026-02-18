#ifndef __UDP_RECEIVER__
#define __UDP_RECEIVER__

class CUdpReceiver : public i3Thread
{
	I3_CLASS_DEFINE( CUdpReceiver ); 
private: 

protected: 
	i3Udp			m_UdpSocket;
	BOOL			m_bIsRun; 
	CListCtrl	*	m_pLog_Control;
	i3Mutex			m_Mutex;
public :
	CUdpReceiver(); 
	virtual ~CUdpReceiver(); 

	BOOL OnCreate(INT32 Port, CListCtrl	* pListControl);
	BOOL OnDestroy(void); 

	virtual void	BeforeRunning( void * pUserData);
	virtual UINT32	OnRunning( void * pUserData);		
	virtual void	AfterRunning( void * pUserData);

	void	InsertList(INT32 LogLevel, REAL32 Time, char * pString); 
	void	InsertBlank(void); 
	void	CleanList(void); 
}; 

#endif