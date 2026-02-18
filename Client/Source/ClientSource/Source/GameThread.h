#if !defined( __GAME_THREAD_H__)
#define __GAME_THREAD_H__

class CThrowH
{
private:
	bool	m_Result;

public:
	CThrowH(bool rv) : m_Result(rv) {}

	bool	IsRun( void)		{ return m_Result; }
};

//** \brief 게임에서 사용하는 Thread는 이 Class에서 파생됩니다. */
class CGameThread : public i3Thread
{
	I3_ABSTRACT_CLASS_DEFINE( CGameThread, i3Thread);
private:
	i3Signal *		m_pSignal;
	UINT32			m_SignalTime;
	volatile bool	m_bRun;
	volatile bool	m_bExitResult;

protected:
	bool			_isRun( void) const				{ return m_bRun; }
	void			_setSignalTime( UINT32 t)		{ m_SignalTime = t; }
	bool			_enableExit( void) const		{ return m_bExitResult; }

public:
	CGameThread();
	virtual ~CGameThread();

	virtual bool	Create( char * pszName, UINT32 Attr = 0, UINT32 StackSize = 4096, void * pUserData = nullptr, INT32 priority = 3) override;
	/** \brief child class에서 호출하여 쓰레드를 종료시켜야 한다. */
	void			Destroy( void);
	virtual UINT32	OnRunning( void * pData) override;
	virtual void	AfterRunning( void * pUserData) override;

	void			Wakeup( void);
	virtual void	Run( void * pData)=0;
	void			Pause( void);

	void			Stop( void);

	
};

#endif