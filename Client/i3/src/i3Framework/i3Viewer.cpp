// i3Viewer.cpp : Defines the entry point for the application.
//
#include "i3FrameworkPCH.h"
#include "i3Base.h"
#include "i3Gfx.h"
#include "i3Viewer.h"
#include "i3Framework.h"
#include "i3UI/i3UIManager.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Scene/i3XrayScene.h"

//#if defined( I3_DEBUG)
#include "i3DebugFont.inc"
//#endif

//#define		SIMPLE_PROFILE

#if defined( I3_WINDOWS)
#include "res/Resource.h"
//#include "i3ViewerPanel.h"

extern HINSTANCE g_hInstFramework;
#endif

#include "i3Math/octree/occ_culler.h"
#include "i3Math/structure/frustum_plane.h"
#include "OccDebugInfo.h"


CALLBACK_EXTERNAL_RENDER_LOSTDEVICE_PROC i3Viewer::m_CallBackExternalRenderLostDeviceHandler = nullptr;
CALLBACK_EXTERNAL_RENDER_REVIVE_PROC	i3Viewer::m_CallBackExternalRenderReviveHandler = nullptr;

// Global Variables:

I3_CLASS_INSTANCE( i3Viewer);

I3_EXPORT_FRAMEWORK i3Viewer * g_pMainViewer;

i3Viewer::i3Viewer(void)
{
	g_pMainViewer = this;

	#if !defined( I3_NO_PROFILE)
	_clearProfile();
	#endif
}

i3Viewer::~i3Viewer(void)
{
	delete m_OccDebugInfo;

#if defined( I3_WIPI)	//	WIPI용 추가. 해제될때 랜더링 중단되도록. - KOMET
	if( m_pContext )
	{
	#if defined( I3_WIPI_KTF_G1000)
		I3OGL::glFinish();
		I3OGL::glFlush();	
	#else		 
		m_pContext->End();
	#endif
	}
#endif

#if !defined( I3_NO_PROFILE)
	//I3_SAFE_RELEASE( m_pDebugFont);
	#if defined (I3G_DX)
		I3_SAFE_RELEASE( m_pDebugTextNode);	
	#endif
#endif

	I3_SAFE_RELEASE( m_p2DOutput);
	I3_SAFE_RELEASE( m_pTimer);
	I3_SAFE_RELEASE( m_pTracer);
	I3_SAFE_RELEASE( m_pSgContext);
	I3_SAFE_RELEASE( m_pInputManager);
	I3_SAFE_RELEASE( m_pFramework);
//	I3_SAFE_RELEASE( m_pContext);
	I3_SAFE_RELEASE( m_pSoundCtx);

	I3_SAFE_RELEASE( m_pWorldOutput );
	I3_SAFE_RELEASE( m_pCharacterOutput );

#if defined( I3_PHYSX)
	I3_SAFE_RELEASE( m_pPhysixContext);
#endif

#if defined( I3_WINDOWS)

	if( m_hTimeEvent != 0)
	{
		timeKillEvent( m_hTimeEvent);
		m_hTimeEvent = 0;
	}

	if( m_hSyncEvent != nullptr)
	{
		CloseHandle( m_hSyncEvent);
		m_hSyncEvent = nullptr;
	}

	_UndoSubclassWindow();
#endif

	I3_SAFE_RELEASE( m_pPostProcessManager);
	I3_SAFE_RELEASE( m_pContext);
}

static void _WaitVSyncProc( i3RenderContext * pCtx, void * pUserData)
{
	i3InputDeviceManager * pManager = (i3InputDeviceManager *) pUserData;

	pManager->Update();
}

bool i3Viewer::Create( void * pPlatformData, i3VideoInfo * pVideo, UINT32 DeviceMask, bool bEnableShader, bool bDialogBoxMode)
{
	i3Error::SetMainWindow( pPlatformData);

	i3InputRegisterMetas();
	i3SceneRegisterMetas();



	/////////////////////////////////////////////////////
	//
	{
		m_pInputManager = i3InputDeviceManager::new_object();
		m_pInputManager->Create( pPlatformData, DeviceMask);
	}

	/////////////////////////////////////////////////////
	// Gfx
	m_pContext = i3RenderContext::new_object();

	if (i3UI::getToolDevMode())
	{
		//Tool에서는 Double계산이 가능하게 변경
		m_pContext->SetDoubleCalcul(true);
	}
	//m_pContext->AddWaitVSyncProc( _WaitVSyncProc, m_pInputManager);

#if defined( I3G_DX)
	////////////////////////////////////////////////////
	// Shader 가능여부 설정 (내부적으로는 GfxCaps에서 Shader의 가능여부에 맞추어서 활성화/비활성화 합니다)
	GetRenderContext()->setShaderEnable( bEnableShader);

	m_pContext->SetLostDeviceHandler( _LostDeviceHandler);
	m_pContext->SetReviveHandler( _ReviveHandler);
	m_pContext->SetRecreateHandler( _RecreateHandler);
	m_pContext->setDialogBoxMode( bDialogBoxMode);
#endif

#if defined( I3_WINDOWS)
	if( IsState( I3_VIEWER_STATE_MULTITHREAD))
		pVideo->m_bMultiThread = true;

	m_hWnd = (HWND) pPlatformData;

	_SubclassWindow();

#if defined( I3_PHYSX)
	/////////////////////////////////////////////////////
	// Physix
	{
		m_pPhysixContext = i3PhysixContext::new_object();

		m_pPhysixContext->Create( NX_TIMESTEP_VARIABLE, 1.15f );
	}
#endif

#elif defined( I3G_PSP)
	Format.m_bFullScreen = true;
	Format.m_FrameBufferCount = 2;
	Format.m_Width = 480;
	Format.m_Height = 272;
	Format.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;
	Format.m_ZBufferBits = 16;
#elif defined( I3G_OGLES)
	#if defined( I3_WINCE )
		Format.m_bFullScreen = true;
		Format.m_FrameBufferCount = 2;
		Format.m_Width = 800;
		Format.m_Height = 480;
		Format.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGB_565;
		Format.m_ZBufferBits = 16;
	#else
		Format.m_bFullScreen = true;
		Format.m_FrameBufferCount = 2;
		Format.m_Width = 320;
		Format.m_Height = 240;
		Format.m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGB_565;
		Format.m_ZBufferBits = 16;
	#endif
#endif

	pVideo->m_bMultiThread = true;

	if( !m_pContext->Create( pVideo, pPlatformData))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Hardware test Error:: Could not create render context.");
		return false;
	}

	{
		COLOR col;

		i3Color::Set( &col, (UINT8) 50, 50, 50, 255);

		m_pContext->SetClearColor( &col);
	}

	// Post Process
	{
		m_pPostProcessManager = i3PostProcessManager::new_object();
		m_pPostProcessManager->Create(m_pContext);
	}

		/////////////////////////////////////////////////////
	// Sound
	{
#ifndef I3_WINCE
		I3SND_CONTEXTCREATE_PARAM param;

		param.m_nPlatformData	= (UINT64) pPlatformData;
		param.m_bHasListener	= true;
		param.m_nFlags			= 0;
		param.m_nVoiceCount		= 256;
		param.m_bInitialState	= false;

		m_pSoundCtx = i3SoundContext::new_object();

		if( m_pSoundCtx->Create( &param) == false)
		{
#if defined(I3_DEBUG)
			I3PRINTLOG(I3LOG_NOTICE,  "Hardware test Error:: Could not found any sound device.");
#endif
			I3_MUST_RELEASE( m_pSoundCtx);
			m_pSoundCtx = nullptr;
		}
#endif

	}

	// Scene Tracer
	{
		m_pTracer = i3SceneTracer::new_object();

		m_pTracer->SetRenderContext( m_pContext);
		m_pTracer->setSoundContext( m_pSoundCtx);

		{
			m_p2DOutput = i3RenderAttrListAttr::new_object();
		}
		{
			m_pWorldOutput = i3RenderAttrListAttr::new_object();
			m_pCharacterOutput = i3RenderAttrListAttr::new_object();
		}
	}

	// Timer
	{
		m_pTimer = i3Timer::new_object();
	}

	#if !defined( I3_NO_PROFILE)
	{
		#if defined (I3G_DX)
		// Text Node
		{
			m_pDebugTextNode = i3TextNodeDX2::new_object();
			m_pDebugTextNode->Create( "굴림체", 4096, 9, FW_NORMAL, false, false, DEFAULT_QUALITY, true);
			m_pDebugTextNode->setAutoWrapEnable(true);			
			m_pDebugTextNode->DeactivateNode();
			m_pDebugTextNode->SetText( L"");
			m_pDebugTextNode->setPos( 5, 5);
			m_pDebugTextNode->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pDebugTextNode->SetColor(255, 255, 255, 255);				
		}
		#endif
	}
	#endif



	if( m_pFramework != nullptr)
		m_pFramework->OnResetCameraPosition();

#if defined( I3_WINDOWS)
	m_bPendingUpdate = false;

	// Framerate Sync
	{
		m_hSyncEvent = ::CreateEvent( NULL, TRUE, TRUE, NULL);
		I3ASSERT( m_hSyncEvent != nullptr);

		INT32 interval = (INT32)(1.0f / m_pContext->GetVideoInfo()->m_RefreshRate * 1000.0f);
		//INT32 interval = 1;
		m_hTimeEvent = timeSetEvent( interval, 1, (LPTIMECALLBACK) m_hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	}
#endif

	Play();

	//setDebugTextEnable( true);
	//toggleProfileReportEnable();

	return true;
}

void	i3Viewer::GetVideoInfo( i3VideoInfo * pDest)
{
	i3VideoInfo * pSrc = m_pContext->GetVideoInfo();

	*pDest = *pSrc;
}

void	i3Viewer::SetVideoInfo( i3VideoInfo * pInfo)
{
	if( this->IsState( I3_VIEWER_STATE_INGAME))
	{
		m_pContext->setAdjustWindowPosState( true);
	}

	m_pContext->ChangeVideoInfo( pInfo);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnChangeVideo( pInfo);
	}

#if defined( I3_WINDOWS)
	if( IsState( I3_VIEWER_STATE_SYNC_FPS) && (m_hSyncEvent != nullptr))
	{
		if( m_hTimeEvent != 0)
		{
			timeKillEvent( m_hTimeEvent);
		}

		INT32 interval = (INT32)(1.0f / m_pContext->GetVideoInfo()->m_RefreshRate * 1000.0f);
		//INT32 interval = 1;
		m_hTimeEvent = timeSetEvent( interval, 1, (LPTIMECALLBACK) m_hSyncEvent, 0, TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	}
#endif
}

void i3Viewer::SetFramework( i3Framework * pFrame)
{
	I3_REF_CHANGE( m_pFramework, pFrame);

#if !defined( I3_NO_PROFILE)
	#if defined (I3G_DX)
		m_pDebugTextNode->RemoveFromParent();

		if( m_pFramework != nullptr)
		{
			m_pFramework->GetDebugTextRoot()->AddChild( m_pDebugTextNode);
		}
	#endif
#endif
	if (m_OccDebugInfo)
		m_OccDebugInfo->OnChangeFramework(pFrame);
	
}

void i3Viewer::SetInputUpdateMode( bool bManual)
{
	if( bManual )
	{
		AddState( I3_VIEWER_INPUT_MANUAL_UPDATE);
	}
	else
	{
		RemoveState( I3_VIEWER_INPUT_MANUAL_UPDATE);
	}
}

void i3Viewer::DumpSceneGraph()
{
	#if defined( I3_DEBUG)
	
	#endif
}

static char s_szProfileName[][64] =
{
	"Total",							// 0
	"  Input",							// 1
	"OnUpdate",							// 2
	"  Coll SoftReset",					// 3
	"  Begin",							// 4
	"  Clear",							// 5
	"  BeginRender",						// 6
	"  OnRender",							// 7
	"3D Trace",							// 8
	"2D Trace",							// 9
	"Physix Sim - Start",				// 10
	"  ConfigPostProcess",					// 11
	"3D Draw",							// 12
	"  ApplyPostProcess",					// 13
	"2D Draw",							// 14
	"  EndRender",						// 15
	"  SceneTracer::Reset",				// 16
	"  Coll Calc",						// 17
	"  SND::OnDrive",						// 18
	"End",								// 19
	"  RESET_PROFILE",					// 20
	"  fetchResults",						// 21
	"  Debug",							// 22
	"Effect Render",					// 23
	"sgContext Update",					// 24
	"DXThread",
	"UserDefine0",
	"UserDefine1",
	"UserDefine2",
	"UserDefine3",
};

#if defined( I3_NO_PROFILE)
	#define		SPROFILE(a)		__noop
	#define		EPROFILE(a)		__noop

	#define		S_SPROFILE(a)	__noop
	#define		S_EPROFILE(a)	__noop
#else
	const char * i3Viewer::getProfileName( I3_VIEWER_PROFILE_SLOT idx)
	{
		return (const char *) s_szProfileName[idx];
	}

	#define		SPROFILE(a)			( m_timeProfile[a].m_timeStart = m_pTimer->GetProfileTime())
	#define		EPROFILE(a)			{ m_timeProfile[a].m_timeEnd += m_pTimer->GetProfileTime() - m_timeProfile[a].m_timeStart; m_timeProfile[a].m_bValid = true; }

	#define		S_SPROFILE(a)	( m_timeProfile[a].m_timeStart = m_pTimer->GetProfileTime())
	#define		S_EPROFILE(a)	{ m_timeProfile[a].m_timeEnd += m_pTimer->GetProfileTime() - m_timeProfile[a].m_timeStart; m_timeProfile[a].m_bValid = true; }

	void i3Viewer::startProfile( I3_VIEWER_PROFILE_SLOT s)
	{
		SPROFILE(s);
	}

	void i3Viewer::endProfile( I3_VIEWER_PROFILE_SLOT s)
	{
		EPROFILE(s);
	}
#endif

void i3Viewer::_resetProfile(void)
{
	INT32 i;

	for( i = 0; i < I3_VIEWER_PROFILE_MAX; i++)
	{
		if( m_timeProfile[i].m_timeSumMax < m_timeProfile[i].m_timeEnd)
			m_timeProfile[i].m_timeSumMax = m_timeProfile[i].m_timeEnd;

		m_timeProfile[i].m_timeSum = m_timeProfile[i].m_timeEnd;
		m_timeProfile[i].m_timeEnd = 0;
	}
}

void i3Viewer::_clearProfile(void)
{
	INT32 i;

	for( i = 0; i < I3_VIEWER_PROFILE_MAX; i++)
	{
		m_timeProfile[i].m_timeSum = 0;
		m_timeProfile[i].m_timeSumMax = 0;
		m_timeProfile[i].m_timeEnd = 0;
	}

#if defined( I3_PHYSX)
	if( g_pPhysixCtx != nullptr )
	{
		g_pPhysixCtx->m_nProfileMemAllocMaxCount = 0;
		g_pPhysixCtx->m_nProfileMemReAllocMaxCount = 0;
		g_pPhysixCtx->m_nProfileMemFreeMaxCount = 0;

		g_pPhysixCtx->m_timeAllocMax	= 0L;
		g_pPhysixCtx->m_timeAllocTotalMax = 0L;
		g_pPhysixCtx->m_timeFreeMax = 0L;
		g_pPhysixCtx->m_timeRaycastMax = 0L;
		g_pPhysixCtx->m_nProfileRaycastMaxCount = 0;
	}
#endif
}

static char s_szTemp[4096];

void i3Viewer::Begin( void)
{
	//REAL_PROFILE_START_ROOT();
	_resetProfile();

	#if !defined( I3_NO_PROFILE)
	m_strDebugText.clear();
	#endif

	SPROFILE( 0);
}

void i3Viewer::UpdateInput(void)
{
	m_AttrCount = 0;

	S_SPROFILE( 1);
	if( IsState( I3_VIEWER_STATE_USER_TIME) || IsState( I3_VIEWER_STATE_PAUSED))
		m_DeltaTime = i3Math::Mul( m_UserTime, m_TimeScale);
	else
		m_DeltaTime = i3Math::Mul( m_pTimer->GetDeltaSec(), m_TimeScale);

	if( m_pInputManager != nullptr)
	{
		m_pInputManager->Update();
	}

	if( !IsState( I3_VIEWER_INPUT_MANUAL_UPDATE))
	{
		if( m_pFramework != nullptr)
		{
			m_pFramework->OnInput();
		}
	}

	S_EPROFILE( 1);
}

void i3Viewer::Update(void)
{
	if( m_pFramework != nullptr)
	{
		SPROFILE( 2);
		if(m_pFramework->isPaused())
		{
			m_pFramework->OnUpdatePaused(m_DeltaTime);
		}
		else
		{
			m_pFramework->OnUpdate( m_DeltaTime);
		}
		EPROFILE( 2);

		// 이진균 : 확인용 코드
		#if 0
		{
			double timeRes = i3Timer::GetProfileResolution();

			if( (m_timeProfile[2].m_timeEnd * timeRes) > 10.0)
			{
				I3TRACE( "UPDATE : %.4f\n", m_timeProfile[2].m_timeEnd * timeRes);
				RESET_PROFILE();
				REPORT_PROFILE();
			}
		}
		#endif
	}
}

// m_pPlayerSgContext는 쓰이지 않아 관련 코드를 모두 삭제함...(2015.11.03.수빈)

void i3Viewer::Simulate()
{
	/*#if defined( I3_PHYSX)
	if( m_DeltaTime > 0.0f)
	{
		SPROFILE( 10);
		m_pPhysixContext->simulate( m_DeltaTime);
		EPROFILE( 10);

		SPROFILE( 21);
		m_pPhysixContext->fetchResults( m_DeltaTime);
		m_pPhysixContext->PollForWork();
		EPROFILE( 21);
	}
	#endif*/
}

void i3Viewer::Trace()
{
	bool bRender = false;

	// Pre-Trace
	if( m_pTracer != nullptr)
	{
		m_pTracer->SetTime(m_DeltaTime);
		
		if( m_pSgContext != nullptr)
		{
			SPROFILE( 24);

			m_pTracer->PreTrace(m_pSgContext->get3DRoot());

			i3Camera * pCam = m_pSgContext->getCurrentCamera();
			i3MatrixStack * pStack = m_pTracer->GetModelViewMatrixStack();

			if( pCam->IsFlag( I3_NODEFLAG_CACHEMATRIX))
			{
				// Animation되고 있는 Camera에 대한 대응.
				pStack->Push( pCam->GetCacheMatrix(), nullptr);
			}
			else
			{
				pStack->Push( pCam->GetMatrix(), nullptr);
			}

			m_pTracer->SetCamera( m_pSgContext->getCurrentCamera());

			m_pSgContext->OnUpdate( m_pTracer);

			pStack->Pop();

			//
			i3::occ_culler* pCuller = m_pSgContext->GetOccCuller();
			if (pCuller)		// 존재 안 할수 있다... 존재시에만 처리한다..
			{
				const i3::frustum_plane& frustum = m_pTracer->GetFrustumPlane();
				
				const i3::mat4& matView = reinterpret_cast<const i3::mat4&>(*m_pTracer->GetViewMatrix());
				const i3::mat4& matProj = reinterpret_cast<const i3::mat4&>(*m_pTracer->GetProjectionMatrix());
				const i3::vec3& camPos = reinterpret_cast<const i3::vec3&>(*i3Matrix::GetPos(m_pTracer->GetInverseViewMatrix() ) );

				if (m_OccDebugInfo)
					pCuller->set_statistics_callback(m_OccDebugInfo->GetCallback());

				pCuller->cull_frustum(frustum, matView, matProj, camPos);
			}
			else
			{
				if (m_OccDebugInfo)
					m_OccDebugInfo->ClearText();
			}
			//
			EPROFILE( 24);
		}

		// 여기서 부터 Background Thread로 동작하며 Physics 계산을 한다.
		// 내부적으로 Active Transform 값들을 Update시켜준다.
		// 이 결과는 이전 Frame의 것이다.		

#if defined( I3G_DX)
		if( !m_pContext->GetSkipRender())
#endif
		{			
			//if( m_pContext->IsReady())
			{
				S_SPROFILE( 4);
				m_bRvBegin = m_pContext->Begin();
				S_EPROFILE( 4);

				S_SPROFILE( 3);
				if( m_pTracer->GetCollisionContext() != nullptr)
					m_pTracer->GetCollisionContext()->SoftReset();
				S_EPROFILE( 3);

				if( m_bRvBegin)
				{
					m_bRender = true;
					bRender = true;

					S_SPROFILE( 5);
					m_pContext->Clear();
					S_EPROFILE( 5);

					S_SPROFILE( 6);
					m_bRvBeginRender = m_pContext->BeginRender();
					m_bCallTrace = true;
					S_EPROFILE( 6);
				}
			}
		}

		// 3D
		if( m_pSgContext != nullptr)
		{
			// 3D Root Trace
			SPROFILE( I3_VIEWER_PROFILE_TRACE_3D);

			m_pTracer->Trace( m_pSgContext->get3DRoot());

			EPROFILE( I3_VIEWER_PROFILE_TRACE_3D);

			//xray
			m_pSgContext->GetXrayScene()->Trace(m_pSgContext, m_pTracer);
		}

		if( (m_pFramework != nullptr) )
		{
			if (bRender)
			{
				SPROFILE( 23);
				m_pFramework->RenderEffect( m_pTracer);	
				EPROFILE( 23);
			}
			else
			{
				m_pFramework->ClearEffect();
			}

			m_pFramework->PostTraceEffect();
		}
		
		if( m_pSgContext != nullptr)
		{
			// Physix Sim.에 많은 CPU를 할당하도록 하기 위해
			// Tracer를 우선적으로 끝내고 GPU가 Drawing하는 동안의 시간을 모두 준다.
			// Trace 중에 Physix의 Actor 및 Shape가 변경되는 경우가 발생하기 때문에
			// Trace가 완전히 종료된 후에 Simulation을 시작해야 한다.
			S_SPROFILE( 9);

			m_pTracer->PushOutputList( m_p2DOutput);
			m_pTracer->Trace( m_pSgContext->get2DMainRoot());
			m_pTracer->PopOutputList();

			S_EPROFILE( 9);
		}
	}
}

static REAL32 s_tmProfileElapsed = 0.0f;
static bool	  s_bForceSyncFPS = false;

void i3Viewer::OnlyRender(void)
{
#if defined( I3_PHYSX)
	if( m_DeltaTime > 0.0f)
	{
		SPROFILE( 10);
		m_pPhysixContext->simulate( m_DeltaTime);
		EPROFILE( 10);
	}
#endif

	if( m_pTracer != nullptr)
	{
#if defined( I3G_DX)
		if( !m_pContext->GetSkipRender())
#endif
		{			
			if( m_pContext->IsReady())
			{
				if( m_bRvBegin)
				{
					if(m_bCallTrace == false)
					{
						S_SPROFILE( 6);
						m_bRvBeginRender = m_pContext->BeginRender();
						S_EPROFILE( 6);
					}

					if( m_bRvBeginRender)
					{
						// Rasterize Chain확인
						#if defined(I3G_DX)
							S_SPROFILE( 11);
							ConfigPostProcess();
							S_EPROFILE( 11);
						#endif

						// 3D Rendering
						SPROFILE( 12);
						_RenderView( m_pTracer->GetOutput());
						EPROFILE( 12);

						//CHAR_SOLO_RENDER
						if (bDepthClear1PV)
							m_pContext->Clear( I3G_CLEAR_DEPTH, I3G_CLEAR_COLOR| I3G_CLEAR_STENCIL );

						SPROFILE( 12);
						_RenderView( m_pCharacterOutput);
						EPROFILE( 12);

						#if 0
						{
							double timeRes = i3Timer::GetProfileResolution();

							if( (m_timeProfile[12].m_timeEnd * timeRes) > 70.0)
							{
								I3TRACE( "-------- 3D Draw : %.4f\n", m_timeProfile[12].m_timeEnd * timeRes);
								RESET_PROFILE();
								REPORT_PROFILE();
							}
						}
						#endif

						// Rasterize Chain적용
						#if defined(I3G_DX)
							S_SPROFILE( 13);
							ApplyPostProcess(m_DeltaTime);
							S_EPROFILE( 13);
						#endif
						

						// something to render
						S_SPROFILE( 7);
						if( m_pFramework != nullptr)
							m_pFramework->OnRender();
						S_EPROFILE( 7);
						

						if( m_pUserPostRenderProc != nullptr)
						{
							m_pUserPostRenderProc( this, m_pUserPostRenderData);
						}

						S_SPROFILE( 14);
						_RenderView( m_p2DOutput);
						S_EPROFILE( 14);

						if( m_CallBackExternalRender )
							m_CallBackExternalRender( m_DeltaTime);

						// last post-process shader (above UI).
						#if defined(I3G_DX)
							ApplyLastPostProcess();
						#endif // I3G_DX

						S_SPROFILE( 15);
						m_pContext->EndRender();
						S_EPROFILE( 15);
					}
				}
			}
		}

		S_SPROFILE( 17);
		if( m_pTracer->GetCollisionContext() != nullptr)
			m_pTracer->GetCollisionContext()->Calculate();
		S_EPROFILE( 17);

		S_SPROFILE( 16);
		m_pTracer->Reset();
		S_EPROFILE( 16);
	}

	S_SPROFILE( 18);
	if( m_pSoundCtx != nullptr)
	{
		m_pSoundCtx->OnDrive( m_DeltaTime);
	}
	S_EPROFILE( 18);

#if defined( I3_PHYSX)
	SPROFILE( 21);
	m_pPhysixContext->fetchResults( m_DeltaTime);
	m_pPhysixContext->PollForWork();
	EPROFILE( 21);
#endif

#if defined( I3G_DX)
	if( !m_pContext->GetSkipRender() && (m_bRender))
#endif
	{
		SPROFILE( 19);
		m_pContext->End();
		EPROFILE( 19);
	}

	m_bRender = false;

	#if !defined( I3_NO_PROFILE) 
	SPROFILE( 20);
	RESET_PROFILE();
	EPROFILE( 20);
	#endif

	if( m_pInputManager != nullptr)
	{
		m_pInputManager->Reset();
	}
		
#if !defined( I3_NO_PROFILE)
	Process_Profile();
#endif

	#if 0
	{
		double timeRes = i3Timer::GetProfileResolution();

		if( (m_timeProfile[0].m_timeEnd * timeRes) > 100.0)
		{
			I3TRACE( "-------- Total : %.4f\n", m_timeProfile[0].m_timeEnd * timeRes);
			RESET_PROFILE();
			REPORT_PROFILE();
		}
	}
	#endif

#if defined( I3_WINDOWS)
	if( IsState( I3_VIEWER_STATE_SYNC_FPS))
	{
		if( m_pContext->GetVideoInfo()->m_bVSync == false)
		{
			WaitForSingleObject( m_hSyncEvent, INFINITE);
			ResetEvent( m_hSyncEvent);
			s_bForceSyncFPS = false;
		}
		else
		if ( s_bForceSyncFPS == true)
		{
			WaitForSingleObject( m_hSyncEvent, INFINITE);
			ResetEvent( m_hSyncEvent);
		}
		else	// VSYNC 옵션이 켜져서 Present()에서 수직동기화를 기대하더라도, 하드웨어옵션에서 VSync false가 걸렸다면 이를 확인할수 있는 API가 없다.
		{
			REAL32 fReqDeltaTime = (1.0f / m_pContext->GetVideoInfo()->m_RefreshRate) * 0.9f;		// 60Hz기준이라면 66Hz를 넘길때는 강제 FPS 동기화로 돌린다...

			if ( m_DeltaTime < fReqDeltaTime )
			{
				WaitForSingleObject( m_hSyncEvent, INFINITE);
				ResetEvent( m_hSyncEvent);
				s_bForceSyncFPS = true;
			}
		}
	}
#endif
	m_bCallTrace = false;
}

void i3Viewer::End( void)
{
	if (m_pFramework)
		m_pFramework->PostRenderEffect();

	EPROFILE( 0);
}

void i3Viewer::Render(void)
{
	Begin();

	UpdateInput();
	Update();
	Simulate();
	Trace();
	//TracePlayer();
	OnlyRender();

	End();
}

#if defined(I3G_DX)
void i3Viewer::ConfigPostProcess(void)
{
	if( GetFramework() != nullptr)
	{
		m_pPostProcessManager->ConfigRenderTarget();
	}

	m_pContext->Clear();
}

void i3Viewer::ApplyPostProcess(REAL32 fDeltaSec)
{	
	if( GetFramework() != nullptr)
	{
		m_pPostProcessManager->Apply(fDeltaSec);
	}
}

void i3Viewer::ApplyLastPostProcess()
{
	if (GetFramework() != nullptr)
	{
		m_pPostProcessManager->ApplyLast();
	}
}

#endif

static bool s_bPass0Tbl[ I3_ATTR_MAX] =
{
	false,	//I3_ATTRID_ALPHA_FUNC	=	0,		//  0
	false,	//I3_ATTRID_ALPHA_TEST_ENABLE,		//  1
	false,	//I3_ATTRID_BLEND_ENABLE,				//  2
	false,	//I3_ATTRID_BLEND_MODE,				//  3
	false,	//I3_ATTRID_CLIPPING_ENABLE,			//  4
	false,	//I3_ATTRID_COLOR,					//  5
	false,	//I3_ATTRID_FACECULL_MODE,			//  6
	false,	//I3_ATTRID_FOG_COLOR,				//  7
	false,	//I3_ATTRID_FOG_DENSITY,				//  8
	false,	//I3_ATTRID_FOG_ENABLE,				//  9
	false,	//I3_ATTRID_FOG_MODE,					// 10
	false,	//I3_ATTRID_FOG_NEARFAR,				// 11
	false,	//I3_ATTRID_LIGHTING_ENABLE,			// 12
	false,	//I3_ATTRID_MATERIAL,					// 13
	false,	//I3_ATTRID_RENDER_MODE,				// 14
	false,	//I3_ATTRID_SHADE_MODE,				// 15
	false,	//I3_ATTRID_STENCIL_FUNC,				// 16
	false,	//I3_ATTRID_STENCIL_OPERATION,		// 17
	false,	//I3_ATTRID_STENCILTEST_ENABLE,		// 18
	false,	//I3_ATTRID_Z_FUNC,					// 19
	true,	//I3_ATTRID_Z_TEST_ENABLE,			// 20
	true,	//I3_ATTRID_Z_WRITE_ENABLE,			// 21
	false,	//I3_ATTRID_Z_BIAS,					// 22
	true,	//I3_ATTRID_PROJECT_MATRIX,			// 23
	true,	//I3_ATTRID_VIEW_MATRIX,				// 24
	true,	//I3_ATTRID_MODEL_VIEW_MATRIX,		// 25
	false,	//I3_ATTRID_TEXTURE_ENABLE,			// 26
	false,	//I3_ATTRID_TEXTURE_BIND,				// 27
	false,	//I3_ATTRID_TEXTURE_FILTER,			// 28
	false,	//I3_ATTRID_TEXTURE_WRAP,				// 29
	false,	//I3_ATTRID_TEXTURE_FUNCTION,			// 30
	false,	//I3_ATTRID_TEXTURE_COORD_SOURCE,		// 31
	false,	//I3_ATTRID_TEXTURE_MATRIX,			// 32
	false,	//I3_ATTRID_TEXTURE_COORD_SCALE,		// 33
	false,	//I3_ATTRID_TEXTURE_COORD_OFFSET,		// 34
	true,	//I3_ATTRID_BONEMATRIXLIST,			// 35
	false,	//I3_ATTRID_LIGHT,					// 36
	true,	//I3_ATTRID_RENDER_ATTR_LIST,			// 37
	true,	//I3_ATTRID_GEOMETRY,					// 38
	false,	//I3_ATTRID_POINTSPRITE,				// 39
	true,	//I3_ATTRID_QUAD,						// 40
	true,	//I3_ATTRID_SPRITE,					// 41
	true,	//I3_ATTRID_BILLBOARD,				// 42
	false,	//I3_ATTRID_RENDERTARGET,				// 43
	false,	//I3_ATTRID_CLEARRENDERTARGET,		// 44
	false,	//I3_ATTRID_TEXT,						// 45
	false,	//I3_ATTRID_GPUCOMMANDLIST,			// 46
	false,	//I3_ATTRID_SPRITE2D,					// 47
	false,	//I3_ATTRID_SHADER,					// 48
	false,	//I3_ATTRID_SHADOWMAP_ENABLE,			// 49
	false,	//I3_ATTRID_NORMALMAP_ENABLE,			// 50
	false,	//I3_ATTRID_SPECULARMAP_ENABLE,		// 51
	false,	//I3_ATTRID_NORMALMAP_BIND,			// 52
	false,	//I3_ATTRID_SPECULARMAP_BIND,			// 53
	false,	//I3_ATTRID_LUXMAP_ENABLE,			// 54
	false,	//I3_ATTRID_LUXMAP_BIND,				// 55
	false,	//I3_ATTRID_NORMALMAP_WRAP,			// 56
	false,	//I3_ATTRID_SPECULARMAP_WRAP,			// 57
	false,	//I3_ATTRID_LUXMAP_WRAP,				// 58
	false,	//I3_ATTRID_USER,						// 59
	false,	//I3_ATTRID_VIEWPORT,					// 60
	false,	//I3_ATTRID_TEXTDX,					// 61
	false,	//I3_ATTRID_TEXTDX2,					// 62
	false,	//I3_ATTRID_OCCLUSIONQUERY,				// 63
	false,	//I3_ATTRID_REFLECTMAP_ENABLE,			// 64
	false,	//I3_ATTRID_REFLECTMAP_BIND,				// 65
	false,	//I3_ATTRID_REFLECTMAP_WRAP,				// 66
	false,	//I3_ATTRID_REFLECTMASKMAP_ENABLE,		// 67
	false,	//I3_ATTRID_REFLECTMASKMAP_BIND,			// 68
	false,	//I3_ATTRID_REFLECTMASKMAP_WRAP,			// 69
	false,	//I3_ATTRID_FRESNEL,						// 70
};

void i3Viewer::_ApplyAttr( i3RenderAttrListAttr * pOutput, INT32 pass)
{
	INT32 i;

	for( i = 0; i < pOutput->GetCount(); i++)
	{
		i3RenderAttr * pAttr = pOutput->Get( i);

		if( i3::same_of<i3RenderAttrListAttr* >(pAttr))
		{
			_ApplyAttr( (i3RenderAttrListAttr *) pAttr, pass);
		}
		else if( i3::kind_of<i3GeometryAttr* >(pAttr))
		{
			i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) pAttr;

			pGeoAttr->Apply( m_pContext);
		}
		else
		{
			if( pass <= 1)
			{
				if( s_bPass0Tbl[ pAttr->GetID()])
				{
					pAttr->Apply( m_pContext);
				}
			}
			else
			{
				pAttr->Apply( m_pContext);
			}
		}
	}
}

void i3Viewer::_RenderView( i3RenderAttrListAttr * pOutput, bool bOccCull)
{
	#if !defined( I3_NO_PROFILE)
		m_AttrCount += pOutput->GetCount();
	#endif

	#if defined( I3_DEBUG)
	if( IsState( I3_VIEWER_STATE_DUMP))
	{
		I3TRACE( "-----------------\n");

		pOutput->Dump();

		I3TRACE( "-----------------\n");

		RemoveState( I3_VIEWER_STATE_DUMP);
	}
	#endif

	{
		pOutput->Apply( m_pContext);
	}
}

void i3Viewer::Resize( UINT32 cx, UINT32 cy)
{
	if( isFixedResolution())
		 return;
	if (m_pContext)
	{
		i3VideoInfo* pVideoInfo = m_pContext->GetVideoInfo();
	
		if (pVideoInfo->m_bFullScreen && pVideoInfo->m_bFullScreenWindowModeEnable)
		{
			POINT pt;
			pt.x = cx;		pt.y = cy;
			m_pContext->modifyMouseCursorPosition(pt);
			cx = pt.x;		cy = pt.y;
		}
	}
	
	if( m_pFramework != nullptr)
	{
		i3::pack::POINT2D sz;

		sz.x = cx;
		sz.y = cy;

		m_pFramework->OnSize( &sz);

		cx = sz.x;
		cy = sz.y;
	}

	if( m_pContext)
	{
		if( m_pContext->isWindowed())
		{
			#if defined(I3G_DX)
				m_pContext->SetRenderTargetSize( cx, cy);
			#endif

			//m_pContext->SetViewport( 0, 0, cx, cy);
		}

		if( m_pFramework != nullptr)
		{
			m_pFramework->OnUpdateCameraProjection();

		}
	}
}

#if !defined( I3_NO_PROFILE)
void i3Viewer::Process_Profile(void)
{
	S_SPROFILE( 22);

	s_tmProfileElapsed += m_DeltaTime;

	if( s_tmProfileElapsed > 0.5f)
	{
		switch( m_ProfileMode)
		{
			case PROFILE_PHYSIX :
				{
					#if defined( I3_PHYSX)
						if( m_pPhysixContext != nullptr )
						{
							m_pPhysixContext->_getProfileString( m_strDebugText);

							m_pPhysixContext->_resetProfileData(); 
						}
					#endif
				}
				break;

			case PROFILE_CUSTOM :
				{
					REPORT_PROFILE( m_strDebugText);
				}
				break;

			case PROFILE_FRAMEWORK :
				{
					if( m_pFramework != nullptr)
					{
						m_pFramework->GetProfileString( m_strDebugText);
					}
				}
				break;

			case PROFILE_RENDER :
				{
					i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();
					double timeRes = i3Timer::GetProfileResolution();
					INT32 i;

					sprintf( s_szTemp, "FPS          : %.2f\n", pReport->GetFPS());
					m_strDebugText += s_szTemp; 

					REAL64 total = 0.f;
					for( i = 1; i < I3_VIEWER_PROFILE_MAX; i++)
					{
						if( m_timeProfile[i].m_bValid)
						{
							total += (REAL64)(m_timeProfile[i].m_timeSum * timeRes);
						}
					}

					sprintf( s_szTemp, "Total+- : %.4f\r\n", total);
					m_strDebugText += s_szTemp;

					for( i = 0; i < I3_VIEWER_PROFILE_MAX; i++)
					{
						if( m_timeProfile[i].m_bValid)
						{
							sprintf( s_szTemp, "%-24s : %.4f  (%.4f ps)\n", s_szProfileName[i], m_timeProfile[i].m_timeSum * timeRes, m_timeProfile[i].m_timeSumMax * timeRes);
							m_strDebugText += s_szTemp;
						}
					}

					m_strDebugText += "--------------------------\n";

					sprintf( s_szTemp, " Memory Block        : %d\n", i3mem::GetAllocCount());
					m_strDebugText += s_szTemp; 

					sprintf( s_szTemp, " Triangle Count      : %d\n", pReport->GetTriangleCountPerFrame());
					m_strDebugText += s_szTemp; 

					sprintf( s_szTemp, " Draw Call Count     : %d\n", pReport->GetDrawCallCountPerFrame());
					m_strDebugText += s_szTemp; 

					sprintf( s_szTemp, " Attributes Count    : %d\n", m_AttrCount);
					m_strDebugText += s_szTemp; 

					sprintf( s_szTemp, " Texture Count       : %d\n", pReport->GetTextureCount());
					m_strDebugText += s_szTemp; 

					#if 0
					if( m_pSoundCtx != nullptr)
					{
						i3::string str_report = m_pSoundCtx->ReportPerformance();
						m_strDebugText += str_report;
					}
					#endif
				}
				break;
		}
	}

	if( m_ProfileMode != PROFILE_NONE)
	{
		m_pDebugTextNode->RemoveFlag( I3_NODEFLAG_DISABLE);

		if( s_tmProfileElapsed > 0.5f)
		{
			i3::wstring wstrDebugText;		i3::mb_to_utf16(m_strDebugText, wstrDebugText);
			m_pDebugTextNode->SetText( wstrDebugText );
			m_pDebugTextNode->setSize( 1024, 768);
			
			s_tmProfileElapsed = 0.0f;
		}
	}
	else
	{
		m_pDebugTextNode->AddFlag( I3_NODEFLAG_DISABLE);
	}

	S_EPROFILE( 22);
}
#endif

void i3Viewer::ReportPerformance(void)
{
#if !defined( I3_NO_PROFILE)
	i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();
	double timeRes = i3Timer::GetProfileResolution();
	INT32 i;

	I3TRACE0( "------------------------------ Performance ------------------------------\n");

	I3TRACE( " FPS                     : %f\n", pReport->GetFPS());
	I3TRACE( "--------------------------\n");

	for( i = 0; i < I3_VIEWER_PROFILE_MAX; i++)
	{
		char conv[256];

		if( m_timeProfile[i].m_bValid)
		{
			sprintf( conv, "%-32s : %.4f\n", s_szProfileName[i], m_timeProfile[i].m_timeSum * timeRes);
			I3TRACE( conv);
		}
	}

	I3TRACE( "--------------------------\n");
	I3TRACE( " Triangle Count          : %d\n", pReport->GetTriangleCountPerFrame());
	I3TRACE( " Draw Call Count         : %d\n", pReport->GetDrawCallCountPerFrame());
	I3TRACE( " Attributes Count        : %d\n", m_AttrCount);
	I3TRACE( " GPU Processing Time     : %.4f\n", pReport->GetGPUProcessingTime() * timeRes);
	I3TRACE( " Texture Count           : %d\n", pReport->GetTextureCount());
	I3TRACE( " Free Video Memory       : %d\n", pReport->GetFreeVRam());
	I3TRACE( "--------------------------\n");
	I3TRACE( " Vertex Cache            : %f\n", pReport->getVCache());
	I3TRACE( " GPU Triangle Count      : %d\n", pReport->getGPUTriangleCount());
	I3TRACE( " GPU Clip-Gen TriCount   : %d\n", pReport->getGPUClipGenTriangleCount());
	I3TRACE( " Z-Pass rate             : %d\n", pReport->getOcclusion());
	I3TRACE( " Pipeline Timing         : %f\n", pReport->getPipelineTiming());
	I3TRACE( " Interface Timing        : %f\n", pReport->getInterfaceTiming());
	I3TRACE( " Vertex Timing           : %f\n", pReport->getVertexTiming());
	I3TRACE( " Pixel Timing            : %f\n", pReport->getPixelTiming());
	I3TRACE( " Bandwidth Timing        : %f\n", pReport->getBandwidthTiming());
	I3TRACE( " Cache Utilization rate  : %f\n", pReport->getCacheUtilization());

	if( m_pPhysixContext != nullptr )
	{
		i3::string str;
		m_pPhysixContext->_getProfileString( str);

		I3TRACE( str );
	}
#endif
}

void i3Viewer::SetSceneGraphContext( i3SceneGraphContext * pCtx)
{
	I3_REF_CHANGE( m_pSgContext, pCtx);

	if( m_pTracer != nullptr)
	{
		m_pTracer->setSgContext( pCtx);
	}

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnChangeScene();
	}
}

void i3Viewer::ResetTime(void)
{
	m_pTimer->GetDeltaSec();
	m_DeltaTime = 0.0f;
}

void i3Viewer::Play(void)
{
	RemoveState( I3_VIEWER_STATE_PAUSED);
	m_pTimer->GetDeltaSec();

	#if defined( I3_WINDOWS)
	if( IsState( I3_VIEWER_STATE_INGAME) == false)
	{
		::SetTimer( m_hWnd, 100, 15, NULL);
	}
	#endif
}

void i3Viewer::Pause(void)
{
	AddState( I3_VIEWER_STATE_PAUSED);

	#if defined( I3_WINDOWS)
	if( IsState( I3_VIEWER_STATE_INGAME) == false)
	{
		::KillTimer( m_hWnd, 100);
	}
	#endif
}

#if defined( I3_WINDOWS)
void i3Viewer::ResetMouseButton()
{
	//pMouse->SetButtonState( 0);
}

void i3Viewer::OnLButtonDown( UINT nFlag, POINT point)
{
	m_pContext->modifyMouseCursorPosition(point);
	
	m_pInputManager->OnLButtonDown( nFlag, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnLButtonDown( nFlag, point);
	}
}

void i3Viewer::OnLButtonUp( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnLButtonUp( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnLButtonUp( nFlags, point);
	}
}

void i3Viewer::OnLButtonDblClk( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	if( m_pFramework != nullptr)
	{
		m_pFramework->OnLButtonDblClk( nFlags, point);
	}
}

void i3Viewer::OnMButtonDown( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnMButtonDown( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnMButtonDown( nFlags, point);
	}
}

void i3Viewer::OnMButtonUp( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnMButtonUp( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnMButtonUp( nFlags, point);
	}
}

void i3Viewer::OnRButtonDown( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnRButtonDown( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnRButtonDown( nFlags, point);
	}
}

void i3Viewer::OnRButtonUp( UINT nFlags, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
	
	m_pInputManager->OnRButtonUp( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnRButtonUp( nFlags, point);
	}
}

void i3Viewer::OnMouseMove( UINT nFlags, POINT point)
{
	// 창 사이즈와 백버퍼 사이즈가 일치하지 않는 모드가 이제 존재하게 됩니다. (풀스크린 윈도우 모드) 
	// 이런 경우에 맞추어서 point값을 보정해주어야 합니다.  일단은 급하게 이곳에서 구현하나 관련 구현은 따로 모아두는게 좋습니다.
	
	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnMouseMove( nFlags, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnMouseMove( nFlags, point);
	}
}

void i3Viewer::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{

	m_pContext->modifyMouseCursorPosition(point);
		
	m_pInputManager->OnMouseWheel( nFlag, zDelta, point);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnMouseWheel( nFlag, zDelta, point);
	}
}

void i3Viewer::OnKeyDown( UINT32 nKey)
{
	m_pInputManager->OnKeyDown( nKey);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnKeyDown( nKey);
	}
}

void i3Viewer::OnKeyUp( UINT32 nKey)
{
	m_pInputManager->OnKeyUp( nKey);

	if( m_pFramework != nullptr)
	{
		m_pFramework->OnKeyUp( nKey);
	}
}
#endif

void i3Viewer::OnInput(void)
{
	if( m_pFramework != nullptr)
	{
		m_pFramework->OnInput();
	}
}

REAL32 i3Viewer::GetFPS()
{
	i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();

	return pReport->GetFPS();
}

UINT32 i3Viewer::GetTriangleCount()
{
	i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();

	return pReport->GetTriangleCountPerFrame();
}

UINT32 i3Viewer::GetDrawCallCount()
{
	i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();

	return pReport->GetDrawCallCountPerFrame();
}

void i3Viewer::LogPerformanceReport( const char * pszPath)
{
	i3GfxPerformanceReport * pReport = m_pContext->GetPerformanceReport();
	double timeRes = i3Timer::GetProfileResolution();
	INT32 i;
	i3FileStream strm;

	if( strm.Create( pszPath, STREAM_WRITE | STREAM_READ | STREAM_SHAREREAD) == false)
		return;

	strm.Printf( s_szTemp, "FPS              : %.2f\r\n", pReport->GetFPS());

	REAL32 total = 0.f;
	for( i = 0; i < I3_VIEWER_PROFILE_MAX; i++)
	{
		if( m_timeProfile[i].m_bValid)
		{
			strm.Printf( s_szTemp, "%-24s : %.4f  (%.4f ps)\r\n", s_szProfileName[i], m_timeProfile[i].m_timeSum * timeRes, m_timeProfile[i].m_timeSumMax * timeRes);
			total += (REAL32) (m_timeProfile[i].m_timeSum * timeRes);
		}
	}

	strm.Printf( s_szTemp, "Total+- : %.4f\r\n", total);

	#if defined( I3_PHYSX)
		if( m_pPhysixContext != nullptr )
		{
			i3::string str;
			m_pPhysixContext->_getProfileString( str);

			strm.Printf( s_szTemp, str.c_str());
		}
	#endif

	strm.Printf( s_szTemp, "--------------------------\r\n");
	strm.Printf( s_szTemp, " Triangle Count          : %d\r\n", pReport->GetTriangleCountPerFrame());
	strm.Printf( s_szTemp, " Draw Call Count         : %d\r\n", pReport->GetDrawCallCountPerFrame());
	strm.Printf( s_szTemp, " Attributes Count        : %d\r\n", m_AttrCount);
	strm.Printf( s_szTemp, " Texture Count           : %d\r\n", pReport->GetTextureCount());

	strm.Close();
}

void	i3Viewer::ActivateOccDebugInfo(bool bEnable)
{
	if (bEnable)
	{
		if (m_OccDebugInfo == nullptr)
		{
			m_OccDebugInfo = new OccDebugInfo;
			m_OccDebugInfo->Create();
		}
	}
	else
	{
		if (m_OccDebugInfo != nullptr)
		{
			delete m_OccDebugInfo;
			m_OccDebugInfo = nullptr;
		}
	}
}

bool	i3Viewer::IsActivatedOccDebugInfo() const
{
	return (m_OccDebugInfo != nullptr);
}



#if defined( I3_WINDOWS)
i3Viewer*		i3Viewer::s_pViewer = nullptr;
WNDPROC			i3Viewer::s_OldWndProc = nullptr;

class CEnterMonitor
{
protected:
	bool *		m_pEnter;

public:
	CEnterMonitor( bool * pEnterFlag)
	{
		m_pEnter = pEnterFlag;

		*m_pEnter = true;
	}

	~CEnterMonitor(void)
	{
		*m_pEnter = false;
	}
};

LRESULT WINAPI i3Viewer::_i3ViewerWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	LRESULT Result = 0;
	bool bRender = false;
	bool bDefProc = true;

	if( (i3Viewer::s_pViewer == nullptr) || (i3Error::isErrorReporting()))
		return CallWindowProc( i3Viewer::s_OldWndProc, hwnd, msg, wParam, lParam);

	if(	((msg >= WM_MOUSEFIRST) && (msg <= WM_MOUSELAST)) ||
		((msg >= WM_KEYFIRST) && (msg <= WM_KEYLAST)) ||
		(msg == WM_PAINT) ||
		(msg == WM_SIZE) ||
		(msg == WM_TIMER))
	{
		bRender = true;
	}

	switch( msg)
	{
		case WM_LBUTTONDOWN :
			SetFocus( hwnd);

			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnLButtonDown( (UINT) wParam, pt);
			break;

		case WM_LBUTTONUP :
			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnLButtonUp( (UINT) wParam, pt);
			break;

		case WM_LBUTTONDBLCLK :
			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnLButtonDblClk( (UINT) wParam, pt);
			break;

		case WM_RBUTTONDOWN :
			SetFocus( hwnd);

			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnRButtonDown( (UINT) wParam, pt);
			break;
				
		case WM_RBUTTONUP :
			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnRButtonUp( (UINT) wParam, pt);
			break;

		case WM_MBUTTONDOWN :
			SetFocus( hwnd);

			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnMButtonDown( (UINT) wParam, pt);
			break;
				
		case WM_MBUTTONUP :
			pt.x = (short) LOWORD( lParam);
			pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnMButtonUp( (UINT) wParam, pt);
			break;

		case WM_MOUSEMOVE :
			pt.x = (short)LOWORD( lParam);
			pt.y = (short)HIWORD( lParam);

			i3Viewer::s_pViewer->OnMouseMove( wParam, pt);
			break;

		case WM_MOUSEWHEEL :
			{
				short zDelta = (short) HIWORD( wParam);
				short nFlag = LOWORD( wParam);

				pt.x = (short) LOWORD( lParam);
				pt.y = (short) HIWORD( lParam);

				i3Viewer::s_pViewer->OnMouseWheel( nFlag, zDelta, pt);
			}
			break;

		case WM_INPUT :
			{
				i3Viewer::s_pViewer->m_pInputManager->OnRawInput( wParam, lParam);
			}
			break;

		case WM_SYSKEYDOWN :
		case WM_KEYDOWN :
			if( i3Viewer::s_pViewer->IsState( I3_VIEWER_INPUT_NO_WINMSG) == false)
			{
				i3Viewer::s_pViewer->OnKeyDown( wParam);
			}
			break;

		case WM_SYSKEYUP :
		case WM_KEYUP :
			if( i3Viewer::s_pViewer->IsState( I3_VIEWER_INPUT_NO_WINMSG) == false)
			{
				i3Viewer::s_pViewer->OnKeyUp( wParam);
			}
			break;

		case WM_MOVE :
			i3Viewer::s_pViewer->m_bMoved = true;
			break;

		case WM_SHOWWINDOW:		i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( (bool)!wParam);	break;
		case WM_ACTIVATEAPP :	i3Viewer::s_pViewer->OnActivate( wParam == TRUE);		break;
		case WM_ACTIVATE :		i3Viewer::s_pViewer->OnActivate( LOWORD( wParam) != WA_INACTIVE);	break;
		case WM_SETFOCUS :		i3Viewer::s_pViewer->OnSetFocus();	break;
		case WM_KILLFOCUS :		i3Viewer::s_pViewer->OnKillFocus();	break;

		case WM_SIZE :		
			{
				INT32 cx, cy;

				cx = (INT32)( lParam & 0xFFFF);
				cy = (INT32)((lParam >> 16) & 0xFFFF);

				if((wParam != SIZE_MINIMIZED) && (cx > 0) && (cy > 0))
				{
					i3Viewer::s_pViewer->Resize( cx, cy);

					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender(false);
				}
				else
				{
					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender(true);
				}
			}

			i3Viewer::s_pViewer->m_bMoved = false;
			break;

			/*
		case WM_WINDOWPOSCHANGED :
			{
				WINDOWPOS * pPos = (WINDOWPOS *) lParam;

				if( (pPos->cx > 0) && (pPos->cy > 0))
				{
					i3Viewer::s_pViewer->Resize( pPos->cx, pPos->cy);

					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( false);
				}
				else
				{
					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( true);
				}
			}
			break;
			*/

		case WM_CLOSE :
			{
				if( i3Viewer::s_pViewer->OnQueryClose() == false)
				{
					return 0;
				}
			}
			break;

		case WM_EXITSIZEMOVE :
			if( i3Viewer::s_pViewer->m_bMoved)
			{
				i3Viewer::s_pViewer->UpdateDevice();
			}
			break;

		case WM_CHAR :
			bDefProc = false;
			break;

		case WM_ERASEBKGND :
			bDefProc = false;
			Result = 1;
			break;
	}

	if( bDefProc)
		Result = CallWindowProc( i3Viewer::s_OldWndProc, hwnd, msg, wParam, lParam);

	//if( i3Viewer::s_pViewer->IsState( I3_VIEWER_STATE_PAUSED))
	//{
	if( i3Viewer::s_pViewer != nullptr)
	{
		if( bRender && (i3Viewer::s_pViewer->GetPendingUpdateEnable() == false))
			i3Viewer::s_pViewer->Render();
	}
	//}
	//else
	//{
	//	if( msg == WM_TIMER)
	//		i3Viewer::s_pViewer->Render();
	//}

	return Result;
}

void i3Viewer::_SubclassWindow(void)
{
	i3Viewer::s_OldWndProc = (WNDPROC) ::GetWindowLong( m_hWnd, GWL_WNDPROC);
	i3Viewer::s_pViewer = this;

	//::SetProp( m_hWnd, "OLDPROC", i3Viewer::s_OldWndProc);
	//::SetProp( m_hWnd, "VIEWER", this);

	::SetWindowLong( m_hWnd, GWL_WNDPROC, (LONG) _i3ViewerWndProc);
	//::SetClassLong( m_hWnd, GCL_WNDPROC, (LONG) _i3ViewerWndProc);

	UINT32 style = ::GetWindowLong( m_hWnd, GWL_STYLE);
	::SetWindowLong( m_hWnd, GWL_STYLE, style | WS_CLIPCHILDREN);
}

void i3Viewer::_UndoSubclassWindow(void)
{
	::SetWindowLong( m_hWnd, GWL_WNDPROC, (LONG) i3Viewer::s_OldWndProc);
	i3Viewer::s_pViewer = nullptr;
}

void i3Viewer::OnActivate( bool bActivate)
{
	if (m_pContext)
	{
		m_pContext->Process_WM_ACTIVATE(bActivate);
	}

	if( m_pFramework != nullptr)
		m_pFramework->OnActivate( bActivate);
}

void i3Viewer::OnSetFocus(void)
{
	if( m_pFramework != nullptr)
		m_pFramework->OnSetFocus();
}

void i3Viewer::OnKillFocus(void)
{
	if( m_pFramework != nullptr)
		m_pFramework->OnKillFocus();
}

void i3Viewer::OnExitSizeMove(void)
{
}

bool i3Viewer::OnQueryClose(void)
{
	if( m_pFramework != nullptr)
		return m_pFramework->OnQueryClose();
 
	return true;
}

void i3Viewer::_LostDeviceHandler( bool bLostDevice)
{
	SetInputAccessRight( false);

	if( m_CallBackExternalRenderLostDeviceHandler)
	{
		m_CallBackExternalRenderLostDeviceHandler();
	}

	i3Node::CallLostDevice( bLostDevice);
	i3GameNode::CallLostDevice( bLostDevice);

}

void i3Viewer::_ReviveHandler( i3RenderContext * pCtx)
{
	i3Node::CallRevive( pCtx);
	i3GameNode::CallRevive( pCtx);

	if( m_CallBackExternalRenderReviveHandler )
	{
		m_CallBackExternalRenderReviveHandler();
	}

	SetInputAccessRight( true);
}

void i3Viewer::_RecreateHandler( void)
{
	i3GameNode::CallRecreate();
}

void i3Viewer::SetInputAccessRight( bool bGet)
{
	if( i3Viewer::s_pViewer != nullptr && i3Viewer::s_pViewer->m_pInputManager != nullptr)
	{
		i3Viewer::s_pViewer->m_pInputManager->SetAccessRight( bGet);
	}
}

#endif
