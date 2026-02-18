#if !defined( __I3_LV_RESOURCE_SCAN_THREAD_H)
#define __I3_LV_RESOURCE_SCAN_THREAD_H

#include "i3LevelRes.h"

#include "i3Base/itl/set.h"

typedef bool	(*I3LV_RES_SCAN_PROC)( char * pszPath, char * pszFile, i3LevelRes * pRes);

class i3LevelResDatabase;

#define		WM_RESSCAN_START				(WM_USER + 20)
#define		WM_RESSCAN_CONFIG				(WM_USER + 21)
#define		WM_RESSCAN_RES					(WM_USER + 22)
#define		WM_RESSCAN_END					(WM_USER + 23)

class I3_EXPORT_TDK i3LevelResScanThread : public i3Thread
{
	I3_EXPORT_CLASS_DEFINE( i3LevelResScanThread, i3Thread);

protected:
	char					m_szResPath[MAX_PATH] = { 0 };
	i3LevelResDatabase *	m_pDB = nullptr;
	HWND					m_hwnd = nullptr;

	i3::vector<i3::rc_string>*		m_pFileList;
	i3::set< i3::string >			m_ingnoreFolders;

public:
	i3LevelResScanThread(void) {}
	virtual ~i3LevelResScanThread(void) {}

	void		setResourcePath( const char * pszPath)			{ strncpy( m_szResPath, pszPath, sizeof( m_szResPath) - 1); }
	void		setResDatabase( i3LevelResDatabase * pDB)		{ m_pDB = pDB; }
	void		setHWND( HWND hwnd)								{ m_hwnd = hwnd; }

	virtual UINT32	OnRunning( void * pUserData) override;
	
public:
	void		InsertIgnoreFolder( const i3::set< i3::string >& ignoreFolder )
	{
		m_ingnoreFolders = ignoreFolder;
	}

private:
	void		_ScanResourceAll();
	void		_ScanResource( const i3::string& rootPath, const i3::string& parentPath, bool bRecursive );

	bool		_AddResourceFile( const i3::string& rootPath, const i3::string& path, const i3::string& fileName );
};

#endif
