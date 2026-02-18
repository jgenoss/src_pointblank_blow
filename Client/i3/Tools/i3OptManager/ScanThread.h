#if !defined( __SCAN_THREAD_H)
#define __SCAN_THREAD_H

#include "Warn.h"

//typedef bool	(*SCAN_PROC)( char * pszPath, char * pszFile, i3LevelRes * pRes);

#define		WM_SCAN_START				(WM_USER + 20)
#define		WM_SCAN_FILE				(WM_USER + 21)
#define		WM_SCAN_ADD					(WM_USER + 22)
#define		WM_SCAN_END					(WM_USER + 23)

class ScanThread : public i3Thread
{
	I3_CLASS_DEFINE( ScanThread);
protected:
	i3List *				m_pWarnList;

	char					m_szResPath[MAX_PATH];
	HWND					m_hwnd;

protected:
	bool					_CheckTexture( char * pszPath);
	bool					_Check( char * pszPath, char * pszName);

public:
	ScanThread(void);
	virtual ~ScanThread(void);

	void		setResourcePath( const char * pszPath)			{ strcpy( m_szResPath, pszPath); }
	void		setHWND( HWND hwnd)								{ m_hwnd = hwnd; }
	void		setWarnList( i3List * pList)					{ m_pWarnList = pList; }

	virtual UINT32	OnRunning( void * pUserData);

	static BOOL _ResScanProc( int Level, LPCTSTR Path, WIN32_FIND_DATA * pInfo, LPVOID pUserData);
};

#endif
