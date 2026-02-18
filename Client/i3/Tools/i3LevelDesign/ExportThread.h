#if !defined( __EXPORT_THREAD_H)
#define __EXPORT_THREAD_H

#define		WM_EXPORT_STATE					(WM_USER + 20)
#define		WM_EXPORT_END					(WM_USER + 21)
#define		WM_EXPORT_PROGRESS_SET			(WM_USER + 22)
#define		WM_EXPORT_PROGRESS				(WM_USER + 23)
#define		WM_EXPORT_LOG					(WM_USER + 24)
#define		WM_EXPORT_ERROR					(WM_USER + 25)
#define		WM_EXPORT_CANCEL				(WM_USER + 26)

class CExportThread : public i3Thread
{
	I3_CLASS_DEFINE( CExportThread, i3Thread);
protected:
	char					m_szDestPath[MAX_PATH];
	i3LevelResDatabase *	m_pDB;
	HWND					m_hwnd;
	i3LevelScene *			m_pScene;

	i3::vector<i3LevelElement3D*>	m_ElmList;
	i3::vector<i3LevelRes*>	m_ResList;

protected:
	bool		_IsNeedToUpdate( const char * pszPath, i3LevelRes * pRes);

public:
	CExportThread(void);
	virtual ~CExportThread(void);

	void		setDestPath( const char * pszPath)				{ strcpy( m_szDestPath, pszPath); }
	void		setResDatabase( i3LevelResDatabase * pDB)		{ m_pDB = pDB; }
	void		setHWND( HWND hwnd)								{ m_hwnd = hwnd; }
	void		setScene( i3LevelScene * pScene)				{ m_pScene = pScene; }

	virtual UINT32	OnRunning( void * pUserData) override;

	void		BuildUsedResList(void);
	void		ValidateResource(void);
	void		ExportResList(void);
	void		ExportInstanceList(void);
	void		ExportPostProcessList();
};

#endif
