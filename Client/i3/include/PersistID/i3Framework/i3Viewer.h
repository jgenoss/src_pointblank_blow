#ifndef __I3_VIEWER_H
#define __I3_VIEWER_H

#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Math.h"
#include "i3Input.h"
#include "i3Scene.h"

class i3Framework;

#define I3_VIEWER_STATE_USER_TIME		0x00000004
#define	I3_VIEWER_STATE_PAUSED			0x00000010
#define I3_VIEWER_STATE_DUMP			0x80000000
#define	I3_VIEWER_STATE_MULTITHREAD		0x00000020
#define	I3_VIEWER_STATE_INGAME			0x00000040
#define	I3_VIEWER_STATE_EXIT			0x40000000
#define I3_VIEWER_STATE_SYNC_FPS		0x00000080

#define I3_VIEWER_INPUT_MANUAL_UPDATE	0x08000000
#define I3_VIEWER_INPUT_NO_WINMSG		0x04000000

#define I3_VIEWER_PROFILE_MAX			32

typedef struct _tagi3ViewerProfileInfo
{
	bool	m_bValid;
	INT64	m_timeStart;
	INT64	m_timeEnd;
	INT64	m_timeSum;
	INT64	m_timeSumMax;
} I3_VIEWER_PROFILE_INFO;

class I3_EXPORT_FRAMEWORK i3Viewer : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Viewer);

protected:
	UINT32					m_State;

#if defined( I3_PHYSX)
	i3PhysixContext *		m_pPhysixContext;
#endif

	i3RenderContext *		m_pContext;
	i3SceneTracer *			m_pTracer;
	i3SoundContext *		m_pSoundCtx;

	i3SceneGraphContext *	m_pSgContext;
	
	i3RenderAttrListAttr *	m_p2DOutput;

#if !defined( I3_NO_PROFILE)
	#if defined (I3G_DX)
		i3TextNodeDX2 *			m_pDebugTextNode;
	#endif
//	i3Font *				m_pDebugFont;
	char					m_szDebugText[4096];

	bool					m_bProfileReport;
	bool					m_bElapseReport;
#endif

	i3InputDeviceManager *	m_pInputManager;

	i3Timer *				m_pTimer;
	RT_REAL32				m_UserTime;
	RT_REAL32				m_TimeScale;
	RT_REAL32				m_DeltaTime;

	i3Framework *			m_pFramework;
	#if defined( I3_WINDOWS)
		HWND				m_hWnd;

		bool				m_bPendingUpdate;

		HANDLE				m_hSyncEvent;
		MMRESULT			m_hTimeEvent;
	#else
		void*				m_hWnd;
	#endif

public:
	I3_VIEWER_PROFILE_INFO	m_timeProfile[I3_VIEWER_PROFILE_MAX];
	UINT32					m_AttrCount;

#if defined( I3_WINDOWS)
	bool					m_bMoved;
	BOOL					m_bEnableRasterizeChain;
#endif

protected:
	i3Node *				LoadScene( const char * pszPath);

#if defined(I3G_DX)
	void					_BeginRasterizeChain(void);
	void					_ApplyRasterizeChain(REAL32 fDeltaSec);
#endif

	void					_RenderView( i3RenderAttrListAttr * pOutput);

	void					_resetProfile(void);
	void					_clearProfile(void);

#if defined( I3_WINDOWS)
	void					_SubclassWindow(void);
	void					_UndoSubclassWindow(void);
#endif

public:
	i3Viewer(void);
	virtual ~i3Viewer(void);

	void					DumpSceneGraph(void);

	UINT32					GetState(void)							{ return m_State; }
	void					SetState( UINT32 state)					{ m_State = state; }
	void					AddState( UINT32 mask)					{ m_State |= mask ;}
	void					RemoveState( UINT32 mask)				{ m_State &= ~mask; }
	BOOL					IsState( UINT32 mask)					{ return (m_State & mask) == mask; }

	void					SetUserTimeMode( BOOL bFlag)			{ bFlag ? AddState( I3_VIEWER_STATE_USER_TIME) : RemoveState( I3_VIEWER_STATE_USER_TIME); }
	BOOL					GetUserTimeMode( BOOL bFlag)			{ return IsState( I3_VIEWER_STATE_USER_TIME); }

	void					SetDumpState( BOOL bFlag)				{ AddState( I3_VIEWER_STATE_DUMP); }

	void					SetInputUpdateMode( BOOL bManual = FALSE);
#if !defined( I3_NO_PROFILE)
	#if defined (I3G_DX)
		void					setDebugTextEnable( bool bFlag)
		{
			if( bFlag)
				m_pDebugTextNode->ActivateNode();
			else
				m_pDebugTextNode->DeactivateNode();
		}
		
		void					toggleDebugTextEnable(void)
		{
			if( m_pDebugTextNode->IsFlag( I3_NODEFLAG_DISABLE))
				m_pDebugTextNode->ActivateNode();
			else
				m_pDebugTextNode->DeactivateNode();
		}

		void					setDebugTextPos( REAL32 x, REAL32 y)
		{
			m_pDebugTextNode->setPos( (UINT32)x, (UINT32)y);
		}
	#endif

	char *					getDebugText( void )
	{
		return m_szDebugText;
	}

	void					toggleProfileReportEnable(void)			
	{ 
		m_bProfileReport = !m_bProfileReport; 

		if( m_bProfileReport)
			_clearProfile();
	}
	void					toggleElapseReportEnable(void)			{ m_bElapseReport = !m_bElapseReport; }
#endif

	i3Timer *				GetTimer(void)							{ return m_pTimer; }

#if defined(	I3_WINDOWS)
	HWND					GetWindowHandle(void)					{ return m_hWnd; }
	INT32					GetViewWidth(void)						{ return m_pContext->GetVideoInfo()->m_Width;}
	INT32					GetViewHeight(void)						{ return m_pContext->GetVideoInfo()->m_Height;}

 	bool					GetPendingUpdateEnable(void)			{ return m_bPendingUpdate; }
	void					SetPendingUpdateEnable( bool bFlag)		{ m_bPendingUpdate = bFlag; }

	BOOL					isFullScreen(void)						{ return m_pContext->GetVideoInfo()->m_bFullScreen;		}
#endif 

#if defined( I3_PHYSX)
	i3PhysixContext *		GetPhysixContext(void)					{ return m_pPhysixContext; }
#endif

	i3InputDeviceManager *	GetInputDeviceManager(void)				{ return m_pInputManager; }
	i3RenderContext *		GetRenderContext(void)					{ return m_pContext; }
	i3SceneTracer *			GetSceneTracer(void)					{ return m_pTracer; }

	void					SetSceneGraphContext( i3SceneGraphContext * pCtx);

	BOOL					IsReady(void)							{ return m_pContext->IsReady(); }

	RT_REAL32				GetUserTime(void)						{ return m_UserTime; }
	void					SetUserTime( RT_REAL32 tm)				{ m_UserTime = tm; }

	RT_REAL32				GetTimeScale(void)						{ return m_TimeScale; }
	void					SetTimeScale( RT_REAL32 scale)			{ m_TimeScale = scale; }

	RT_REAL32				getDeltaTime(void)						{ return m_DeltaTime; }
	void					setDeltaTime( RT_REAL32 tm)				{ m_DeltaTime = tm; }

	void					ResetTime(void);

	i3Framework *			GetFramework(void)						{ return m_pFramework; }
	void					SetFramework( i3Framework * pFrame);

	i3SoundContext*			GetSoundContext()						{ return m_pSoundCtx;	}

	BOOL	Create( void * pPlatformData, i3VideoInfo * pVideo, UINT32 DeviceMask = I3I_DEVICE_MASK_CONTROLLER | I3I_DEVICE_MASK_KEYBOARD, BOOL bEnableShader = TRUE);

	void	GetVideoInfo( i3VideoInfo * pInfo);
	void	SetVideoInfo( i3VideoInfo * pInfo);

	virtual void	Render(void);

	void	Resize( UINT32 cx, UINT32 cy);
	void	UpdateDevice(void)										{ m_pContext->UpdateDevice(); }
	void	ReportPerformance(void);

	void	Play(void);
	void	Pause(void);

	void	OnInput(void);

	#if defined( I3_WINDOWS)
	virtual void ResetMouseButton();
	virtual void OnLButtonDown( UINT nFlag, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnMButtonDown( UINT nFlag, POINT point);
	virtual void OnMButtonUp( UINT nFlag, POINT point);
	virtual void OnRButtonDown( UINT nFlag, POINT point);
	virtual void OnRButtonUp( UINT nFlag, POINT point);
	virtual void OnMouseMove( UINT nFlag, POINT point);
	virtual void OnMouseWheel( UINT nFlag, short zDelta, POINT point);
	virtual void OnKeyDown( UINT32 nKey);
	virtual void OnKeyUp( UINT32 nKey);

	virtual void OnActivate( bool bActivate);
	virtual void OnExitSizeMove(void);

	virtual bool OnQueryClose(void);

	static LRESULT WINAPI _i3ViewerWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	#endif

	void	LogPerformanceReport( const char * pszPath);
};

#endif
