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

enum I3_VIEWER_PROFILE_SLOT
{
	I3_VIEWER_PROFILE_TOTAL = 0,					// Core
	I3_VIEWER_PROFILE_INPUT = 1,
	I3_VIEWER_PROFILE_UPDATE = 2,					// Core
	I3_VIEWER_PROFILE_SOFTRESET = 3,
	I3_VIEWER_PROFILE_BEGIN = 4,
	I3_VIEWER_PROFILE_CLEAR = 5,
	I3_VIEWER_PROFILE_BEGINRENDER = 6,
	I3_VIEWER_PROFILE_ONRENDER = 7,
	I3_VIEWER_PROFILE_TRACE_3D = 8,					// Core
	I3_VIEWER_PROFILE_TRACE_2D = 9,
	I3_VIEWER_PROFILE_PHX_SIMULATE = 10,			// Core
	I3_VIEWER_PROFILE_RASTERCHAIN_BEGIN = 11,
	I3_VIEWER_PROFILE_RENDER_3D = 12,				// Core
	I3_VIEWER_PROFILE_RASTERCHAIN_APPLY = 13,
	I3_VIEWER_PROFILE_RENDER_2D = 14,
	I3_VIEWER_PROFILE_ENDRENDER = 15,
	I3_VIEWER_PROFILE_RESET = 16,
	I3_VIEWER_PROFILE_COLLCTX = 17,
	I3_VIEWER_PROFILE_SOUND = 18,
	I3_VIEWER_PROFILE_END = 19,						// Core
	I3_VIEWER_PROFILE_DEBUG_RESET = 20,
	I3_VIEWER_PROFILE_PHX_FETCHRESULT = 21,
	I3_VIEWER_PROFILE_DEBUG = 22,
	I3_VIEWER_PROFILE_EFFECT = 23,					// Core
	I3_VIEWER_PROFILE_SGCONTEXT = 24,				// Core

	I3_VIEWER_PROFILE_DXTHREADUPDATE,
	I3_VIEWER_PROFILE_USERDEF0,
	I3_VIEWER_PROFILE_USERDEF1,
	I3_VIEWER_PROFILE_USERDEF2,
	I3_VIEWER_PROFILE_USERDEF3,

	I3_VIEWER_PROFILE_MAX
};

#if defined( I3_COMPILER_VC)
#pragma pack(push,4)
#endif

struct I3_VIEWER_PROFILE_INFO
{
	INT64	m_timeStart = 0;
	INT64	m_timeEnd = 0;
	INT64	m_timeSum = 0;
	INT64	m_timeSumMax = 0;
	bool	m_bValid = false;
};

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

typedef void (CALLBACK *CALLBACK_EXTERNAL_RENDER)(REAL32 tm);
typedef void (CALLBACK *CALLBACK_EXTERNAL_RENDER_LOSTDEVICE_PROC)(void);
typedef void (CALLBACK *CALLBACK_EXTERNAL_RENDER_REVIVE_PROC)(void);

class i3Viewer;
class OccDebugInfo;

typedef		void	(*POSTRENDERPROC)( i3Viewer * pViewer, void * pData);

class I3_EXPORT_FRAMEWORK i3Viewer : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3Viewer, i3ElementBase);

protected:
	UINT32					m_State = 0;

#if defined( I3_PHYSX)
	i3PhysixContext *		m_pPhysixContext = nullptr;
#endif

	i3RenderContext *		m_pContext = nullptr;
	i3SceneTracer *			m_pTracer = nullptr;
	i3SoundContext *		m_pSoundCtx = nullptr;
	i3SceneGraphContext *	m_pSgContext = nullptr;
	i3RenderAttrListAttr *	m_p2DOutput = nullptr;
	i3RenderAttrListAttr *	m_pWorldOutput = nullptr;
	i3RenderAttrListAttr *	m_pCharacterOutput = nullptr;

#if !defined( I3_NO_PROFILE)
	#if defined (I3G_DX)
		i3TextNodeDX2 *			m_pDebugTextNode = nullptr;
	#endif
	i3::string					m_strDebugText;

public:
	enum PROFILE_MODE
	{
		PROFILE_NONE,
		PROFILE_RENDER,
		PROFILE_PHYSIX,
		PROFILE_FRAMEWORK,
		PROFILE_CUSTOM
	};

	PROFILE_MODE			m_ProfileMode = PROFILE_NONE;

protected:
	void					Process_Profile(void);
#endif

	OccDebugInfo*			m_OccDebugInfo = nullptr;

	i3InputDeviceManager *	m_pInputManager = nullptr;

	i3Timer *				m_pTimer = nullptr;
	REAL32				m_UserTime = 0.0f;
	REAL32				m_TimeScale = 1.0f;
	REAL32				m_DeltaTime = 0.0f;

	i3Framework *			m_pFramework = nullptr;

	#if defined( I3_WINDOWS)
		HWND				m_hWnd = nullptr;

		bool				m_bPendingUpdate = true;

		HANDLE				m_hSyncEvent = nullptr;
		MMRESULT			m_hTimeEvent = 0;
	#else
		void*				m_hWnd;
	#endif
		   CALLBACK_EXTERNAL_RENDER					m_CallBackExternalRender = nullptr;
	static CALLBACK_EXTERNAL_RENDER_LOSTDEVICE_PROC	m_CallBackExternalRenderLostDeviceHandler;
	static CALLBACK_EXTERNAL_RENDER_REVIVE_PROC		m_CallBackExternalRenderReviveHandler;
		

	bool					m_bFixedResolution = false;

public:
	I3_VIEWER_PROFILE_INFO	m_timeProfile[I3_VIEWER_PROFILE_MAX];
	UINT32					m_AttrCount = 0;

#if defined( I3_WINDOWS)
	bool					m_bMoved = false;
	bool					m_bEnableRasterizeChain = false;
#endif

	i3PostProcessManager *	m_pPostProcessManager = nullptr;
	i3PostProcessManager *	GetPostProcessManager()				{ return m_pPostProcessManager;}

#if defined(I3G_DX)

	void					ConfigPostProcess(void);
	void					ApplyPostProcess(REAL32 fDeltaSec);
	void					ApplyLastPostProcess();
#endif

	void					_ApplyAttr( i3RenderAttrListAttr * pOutput, INT32 pass);
	void					_RenderView( i3RenderAttrListAttr * pOutput, bool bOccCull = false);

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
	bool					IsState( UINT32 mask)					{ return (m_State & mask) == mask; }

	bool					isFixedResolution(void)					{ return m_bFixedResolution;}
	void					setFixedResolution( bool bFix)			{ m_bFixedResolution = bFix;}

	void					SetUserTimeMode( bool bFlag)			{ bFlag ? AddState( I3_VIEWER_STATE_USER_TIME) : RemoveState( I3_VIEWER_STATE_USER_TIME); }
	bool					GetUserTimeMode( bool bFlag)			{ return IsState( I3_VIEWER_STATE_USER_TIME); }

	void					SetDumpState( bool bFlag)				{ AddState( I3_VIEWER_STATE_DUMP); }

	REAL32					GetFPS();
	UINT32					GetTriangleCount();
	UINT32					GetDrawCallCount();

	void					SetInputUpdateMode( bool bManual = false);
#if !defined( I3_NO_PROFILE)
	#if defined (I3G_DX)
		void					setDebugTextEnable( bool bFlag)
		{
			if( bFlag)
				m_pDebugTextNode->ActivateNode();
			else
				m_pDebugTextNode->DeactivateNode();
		}

		void					setDebugTextPos( REAL32 x, REAL32 y)
		{
			m_pDebugTextNode->setPos( (UINT32)x, (UINT32)y);
		}
	#endif

	const char *			getDebugText( void ) const {	return m_strDebugText.c_str();	}

	void					setProfileMode( PROFILE_MODE mode)					{ m_ProfileMode = mode; _clearProfile(); }
	PROFILE_MODE			getProfileMode(void)								{ return m_ProfileMode; }

	const char *			getProfileName( I3_VIEWER_PROFILE_SLOT idx);
#endif

	OccDebugInfo*			getOccDebugInfo() const { return m_OccDebugInfo;  }

	void					startProfile( I3_VIEWER_PROFILE_SLOT s);
	void					endProfile( I3_VIEWER_PROFILE_SLOT s);

	REAL32					getProfileData( I3_VIEWER_PROFILE_SLOT idx)
	{
		if( m_timeProfile[idx].m_bValid)
		{
			double tres = i3Timer::GetProfileResolution();

			return (REAL32) (m_timeProfile[idx].m_timeSum * tres);
		}
		
		return 0.0f;
	}

	void					setExternalRender( CALLBACK_EXTERNAL_RENDER proc)									{ m_CallBackExternalRender = proc;}
	void					setExternalRenderLostDeviceHandler(CALLBACK_EXTERNAL_RENDER_LOSTDEVICE_PROC proc)	{ m_CallBackExternalRenderLostDeviceHandler = proc;}
	void					setExternalRenderReviveHandler(CALLBACK_EXTERNAL_RENDER_REVIVE_PROC proc)			{ m_CallBackExternalRenderReviveHandler = proc;}
	
	void					SetWindowHandle(HWND hWnd) { m_hWnd = hWnd; }

	i3Timer *				GetTimer(void)							{ return m_pTimer; }

#if defined(	I3_WINDOWS)
	HWND					GetWindowHandle(void)					{ return m_hWnd; }
	INT32					GetViewWidth(void)						{ return m_pContext->GetVideoInfo()->m_Width;}
	INT32					GetViewHeight(void)						{ return m_pContext->GetVideoInfo()->m_Height;}

 	bool					GetPendingUpdateEnable(void)			{ return m_bPendingUpdate; }
	void					SetPendingUpdateEnable( bool bFlag)		{ m_bPendingUpdate = bFlag; }

	bool					isFullScreen(void)						{ return m_pContext->GetVideoInfo()->m_bFullScreen;		}

	static void				SetInputAccessRight( bool bGet);

	static void				_LostDeviceHandler( bool bLostDevice);
	static void				_ReviveHandler( i3RenderContext * pCtx);
	static void				_RecreateHandler( void);
#endif 

#if defined( I3_PHYSX)
	i3PhysixContext *		GetPhysixContext(void)					{ return m_pPhysixContext; }
#endif

	i3InputDeviceManager *	GetInputDeviceManager(void)				{ return m_pInputManager; }
	i3RenderContext *		GetRenderContext(void)					{ return m_pContext; }
	i3SceneTracer *			GetSceneTracer(void)					{ return m_pTracer; }

	void					SetSceneGraphContext( i3SceneGraphContext * pCtx);

	bool					IsReady(void)							{ return m_pContext->IsReady(); }

	REAL32				GetUserTime(void)						{ return m_UserTime; }
	void					SetUserTime( REAL32 tm)				{ m_UserTime = tm; }

	REAL32				GetTimeScale(void)						{ return m_TimeScale; }
	void					SetTimeScale( REAL32 scale)			{ m_TimeScale = scale; }

	REAL32				getDeltaTime(void)						{ return m_DeltaTime; }
	void					setDeltaTime( REAL32 tm)				{ m_DeltaTime = tm; }

	void					ResetTime(void);

	i3Framework *			GetFramework(void)						{ return m_pFramework; }
	void					SetFramework( i3Framework * pFrame);

	i3SoundContext*			GetSoundContext()						{ return m_pSoundCtx;	}

	bool	Create( void * pPlatformData, i3VideoInfo * pVideo
		, UINT32 DeviceMask = I3I_DEVICE_MASK_CONTROLLER | I3I_DEVICE_MASK_KEYBOARD
		, bool bEnableShader = true, bool bDialogBoxMode = false);

	void	ActivateOccDebugInfo(bool bEnable);
	bool	IsActivatedOccDebugInfo() const;

	void	GetVideoInfo( i3VideoInfo * pInfo);
	void	SetVideoInfo( i3VideoInfo * pInfo);

	virtual void	Render(void);
	virtual void	Resize( UINT32 cx, UINT32 cy);

	void	UpdateDevice(void)										{ m_pContext->UpdateDevice(); }
	void	ReportPerformance(void);

	void	Play(void);
	void	Pause(void);

	void	OnInput(void);

	#if defined( I3_WINDOWS)
	virtual void ResetMouseButton();
	virtual void OnLButtonDown( UINT nFlag, POINT point);
	virtual void OnLButtonUp( UINT nFlags, POINT point);
	virtual void OnLButtonDblClk( UINT nFlags, POINT point);
	virtual void OnMButtonDown( UINT nFlag, POINT point);
	virtual void OnMButtonUp( UINT nFlag, POINT point);
	virtual void OnRButtonDown( UINT nFlag, POINT point);
	virtual void OnRButtonUp( UINT nFlag, POINT point);
	virtual void OnMouseMove( UINT nFlag, POINT point);
	virtual void OnMouseWheel( UINT nFlag, short zDelta, POINT point);
	virtual void OnKeyDown( UINT32 nKey);
	virtual void OnKeyUp( UINT32 nKey);

	virtual void OnActivate( bool bActivate);
	virtual void OnSetFocus( void);
	virtual void OnKillFocus(void);
	virtual void OnExitSizeMove(void);

	virtual bool OnQueryClose(void);

	static LRESULT WINAPI _i3ViewerWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	#endif

	void	LogPerformanceReport( const char * pszPath);

private:
	bool m_bRvBegin = false;
	bool m_bRvBeginRender = false;
	bool m_bRender = false;
	bool m_bCallTrace = false;

public:
	void	Begin();
	void	UpdateInput();
	void	Trace();
	//void	TracePlayer();
	void	Simulate();
	void	Update();
	void	OnlyRender();
	void	End();

protected:
	POSTRENDERPROC			m_pUserPostRenderProc = nullptr;
	void *					m_pUserPostRenderData = nullptr;

public:
	void			SetUserPostRenderProc( POSTRENDERPROC pUserProc, void * pData)
	{
		m_pUserPostRenderProc = pUserProc;
		m_pUserPostRenderData = pData;
	}

private:
	bool bDepthClear1PV = false;

public:
	static i3Viewer *		s_pViewer;
	static WNDPROC			s_OldWndProc;

	void	SetDepthClear1PV(bool bDepthClear) { bDepthClear1PV = bDepthClear; }
};

#endif
