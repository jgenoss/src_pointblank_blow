#if !defined( __I3_SCENE_IO_DEFINE_H)
#define __I3_SCENE_IO_DEFINE_H

class i3SceneGraphInfo;

enum LOADSTATE
{
	LOADSTATE_NA = 0,
	LOADSTATE_LOADING,
	LOADSTATE_ERROR,
	LOADSTATE_LOADED
};

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct IOTICKET
{
	UINT128				m_Code = { 0, 0 };
	char				m_szPath[MAX_PATH] = { 0 };
	LOADSTATE			m_State = LOADSTATE_NA;
	i3SceneGraphInfo *	m_pSgInfo = nullptr;
	bool				m_bLoadedDX = false;		// DX에 적재하는 경우 true
	bool				m_bCommonRes = false;		// Common Res 여부
};

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

#define	MAX_IOTICKET			256
#define	_toIDX( a)				( (a) % MAX_IOTICKET)

class I3_EXPORT_SCENE i3SceneIOQueue : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3SceneIOQueue, i3ElementBase);
protected:
	IOTICKET		m_Queue[ MAX_IOTICKET];
	INT32			m_idxPush = 0;
	INT32			m_idxLoad = 0;

	i3Signal *			m_pSignal_Kick = nullptr;
	i3Mutex2 *			m_pMutex = nullptr;
	
protected:
	INT32				_FindTicket( UINT128 code);

public:
	virtual ~i3SceneIOQueue(void);

	bool				Create(void);
	void				ForceRaise( void);

	// LoadQue가 되어있는 것을 제거한다. (loading중인것들)
	void				FlushTicket( void);

	IOTICKET *			getTicket( INT32 idx)				{ I3_BOUNDCHK( idx, MAX_IOTICKET); return &m_Queue[ idx]; }

	INT32				PushIO( const char * pszPath, bool bLoadedDX, bool bCommonRes);
	INT32				WaitIO(void);									// Background Thread : Queue에 새로운 IO가 들어올 때까지 대기한다.
	void				EndLoad( INT32 idx, i3SceneGraphInfo * pInfo);	// Background Thread : Loading이 종료되었을 때 호출된다. pInfo == nullptr은 Error다.

	LOADSTATE			getLoadState( INT32 hndl);
	i3SceneGraphInfo *	PopLoadedScene( INT32 hndl);

	// 로딩 thread에서 아직 File을 들고 있는 경우 Ref가 안 맞을 수 있다.
	// Queue의 Mutex를 사용하여 해제시키자.
	void				ReleaseSceneGraphInfo( i3SceneGraphInfo * pInfo);

	i3Mutex2 *			GetMutex( void)			{ return m_pMutex; }
};

#endif
