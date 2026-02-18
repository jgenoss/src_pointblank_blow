#if !defined( __OPT_THREAD_H)
#define __OPT_THREAD_H

class COptThread : public i3Thread
{
	I3_CLASS_DEFINE( COptThread, i3Thread);
protected:
	HWND		m_hwndOwner;
	bool		m_bExit;

protected:
	void		REPORT( INT32 msg, WPARAM wParam = 0, LPARAM lParam = 0);
	void		DoOptimize( const char * pszPath);

public:
	COptThread(void);
	virtual ~COptThread(void);

	void			setOwner( HWND hwnd)				{ m_hwndOwner = hwnd; }
	void			setExit( bool bFlag)				{ m_bExit = bFlag; }

	virtual UINT32		OnRunning( void * pData) override;
};

#endif