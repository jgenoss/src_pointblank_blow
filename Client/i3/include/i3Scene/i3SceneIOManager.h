#if !defined( __I3_SCENE_IO_MANAGER_H)
#define __I3_SCENE_IO_MANAGER_H

#include "i3SceneIOQueue.h"

class i3SceneIOThread;

class I3_EXPORT_SCENE i3SceneIOManager : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SceneIOManager, i3ElementBase);
protected:
	i3SceneIOQueue *	m_pQueue = nullptr;
	i3SceneIOThread *	m_pThread = nullptr;

	struct RESINFO {
		i3SceneGraphInfo *	_pSGInfo = nullptr;
		UINT32				_UsageCount = 0;		// 사용한 횟수
		UINT32				_UsageFrameID = 0;		// 사용된 프레임
	};
	i3::vector<RESINFO*>	m_ResList;

	i3::vector<i3SceneGraphInfo*>	m_CommonResList;
	bool				m_bCommonRes = false;

	bool				m_bEnabled = true;
	i3Mutex2			m_Mutex;

protected:
	INT32				_FindLoadedScene( i3SceneGraphInfo * pInfo);
	INT32				_FindLoadedSceneFromCommon( i3SceneGraphInfo * pInfo);
	
public:
	i3SceneIOManager(void);
	virtual ~i3SceneIOManager(void);

	void				Destroy();									
	bool				Create( void);
	void				SetCommonResMode( bool bVal)		{ m_bCommonRes = bVal; }

	void				DestroyResource(bool bDeleteCommon);		

	INT32				Request( const char * pszPath, bool bLoadedDX = false, bool bCommonRes = false);
	LOADSTATE			getLoadState( INT32 hndl);

	i3SceneGraphInfo *	GetLoadedScene( const char * pszPath);
	i3SceneGraphInfo *	PopLoadedScene( INT32 hndl, UINT32 time);

	static i3SceneIOManager *	getInstance(void);

	INT32				GetResCount() { return (INT32)m_ResList.size(); }
	
	void				AddRes( i3SceneGraphInfo * pSgInfo, UINT32 time);
	void				TrashRes( bool bDeleteTexture);
};

#endif
