#include "StdAfx.h"
#include "ItemToolViewer.h"
#include "MainFrm.h"
#include "ItemToolDoc.h"
#include "ItemToolView.h"


I3_CLASS_INSTANCE(cItemToolViewer);//, i3Viewer)


vSwapChainCon cItemToolViewer::m_vSwapChain;

cItemToolViewer::cItemToolViewer(void)
:	m_nMaxW(0)
,	m_nMaxH(0)
,	m_fCurrentAnimTime(0.0f)
,	m_bIsLoadedViewer(false)
{
	m_vSwapChain.resize(MAX_SWAPCHAIN);

	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
	{
		m_vSwapChain[i] = NULL;
	}
}

cItemToolViewer::~cItemToolViewer(void)
{
	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
	{
		I3_SAFE_RELEASE(m_vSwapChain[i]);
	}
}


BOOL cItemToolViewer::Create( void * pPlatformData, i3VideoInfo * pVideo, UINT32 DeviceMask, BOOL bEnableShader, BOOL bDialogBoxMode)
{
	//툴은 멀티샘플링8배..!(상관없겟지.'ㅅ')
	//pVideo->m_MultiSampling = D3DMULTISAMPLE_8_SAMPLES;

	if (!i3Viewer::Create(pPlatformData,pVideo,DeviceMask,bEnableShader, bDialogBoxMode))
		return FALSE;

	m_pContext->SetEnableSwapChain(true);

	m_pContext->SetLostDeviceHandler(PrevDeviceReset);
	m_pContext->SetReviveHandler(AfterDeviceReset);
	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
	{
		m_vSwapChain[i] = cSwapChainInfo::new_object();
		m_vSwapChain[i]->Create(m_pContext, m_pSoundCtx);
		
	}
	m_vSwapChain[E_SWAPCHAIN_WEAPON]->SetName("무기");
	m_vSwapChain[E_SWAPCHAIN_ITEM]->SetName("아이템");
	
	FindMaxResolution(m_nMaxW, m_nMaxH);

	Play();
	return TRUE;
}

void cItemToolViewer::CreateLater()
{
	//이전에 등록했던 서브클래스를 해제한후
	//여기서 만든걸로 등록시켜준다.
	SetWndProc(m_hWnd);

	m_bIsLoadedViewer = true;
}

void cItemToolViewer::Render()
{
	//생성이 안된 상태로 MFC에서 메세지를 보낼때가 있다.
	if (false == m_bIsLoadedViewer)
		return;

	m_pContext->setLightList(g_pMainSCFramework->getSgContext()->getLightList());
	//SetFramework(g_pMainSCFramework);
	//i3Viewer::Render();
	
	UpdateInput();

	RenderSwapChain();
}


void cItemToolViewer::Resize( UINT32 cx, UINT32 cy)
{
	//i3Viewer::Resize(cx,cy);
	//return;

	if( isFixedResolution())
		return;
	
	if( m_pFramework != NULL)
	{
		POINT2D sz;

		sz.x = cx;
		sz.y = cy;

		m_pFramework->OnSize( &sz);

		cx = sz.x;
		cy = sz.y;
	}

	if( m_pContext && m_pContext->isWindowed())
	{
		if (m_pContext->isWindowed())
		{
			if (m_pContext->IsIgnoreResize() || (cx <= 0) || (cy <= 0))
				return;

			m_pContext->SetBackBufferSize(GetMaxW(), GetMaxH());

			m_pContext->PrevResetDevice();

			//m_pContext->GetNativeDevice()->Reset();
			m_pContext->ResetDevice();

			m_pContext->SetBackBufferSize(cx, cy);

			m_pContext->PostResetDevice();
		}
	
		//m_pContext->SetRenderTargetSize( cx, cy);

		if( m_pContext->getRasterizeChainManager() != NULL)
		{
			m_pContext->getRasterizeChainManager()->OnSize();
		}

		if( m_pFramework != NULL)
		{
			m_pFramework->OnUpdateCameraProjection();

		}
	}
}

void cItemToolViewer::RenderSwapChain()
{
	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
	{
		if (m_vSwapChain[i] && m_vSwapChain[i]->RenderSwapChain())
		{
			//SetFramework(m_vSwapChain[i]->GetCurrentFrameWork());
			cItemToolViewer::TraceSwapChain(m_vSwapChain[i]);
			cItemToolViewer::OnlyRenderSwapChain(m_vSwapChain[i]);
		}
	}

	
	if (g_pChara && g_pChara->getAICount() != 0)
	{
		i3AIContext* pAIContext = g_pChara->getAI(0)->getAIContext();
		m_fCurrentAnimTime = pAIContext->getLocalTime()*10000.0f;
		if (pAIContext->getCurrentAIState())
		{
			i3AIState* pCurAiState = pAIContext->getCurrentAIState();
			if (pCurAiState == NULL)
				return;

			i3TimeSequence* pTimeSeq = pCurAiState->getTimeSeq();
			if (pTimeSeq == NULL)
				return;

			float fMaxDuration = pTimeSeq->getDuration();//*10000.0f;
			fMaxDuration = MAX( fMaxDuration, pCurAiState->getDuration());
			fMaxDuration = MAX( fMaxDuration, pCurAiState->getAnimDuration());
			fMaxDuration *= 10000.0f;
			if (m_fCurrentAnimTime >= fMaxDuration)
			{
				m_fCurrentAnimTime = fMaxDuration;
				return;
			}

			I3_TDK_UPDATE_INFO Data;
			Data.m_Event = I3_TDK_UPDATE_SELECT;

			if(!IsState( I3_VIEWER_STATE_PAUSED))
				::SendMessage(g_HwndDlgTimeTrack, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)m_fCurrentAnimTime);
		}	
	}
}


void cItemToolViewer::TraceSwapChain(cSwapChainInfo* pSwapChain)
{
	i3SceneTracer *			pSwapChainTracer	= pSwapChain->GetSwapChainTracer();
	i3SceneGraphContext *	pSwapChainSgContext = pSwapChain->GetSwapChainSgContext();
	
	if( pSwapChainTracer != NULL)
	{
		pSwapChainTracer->SetTime( __PT(m_DeltaTime));

		if( pSwapChainSgContext != NULL)
		{
			i3Camera * pCam = pSwapChainSgContext->getCurrentCamera();
			pCam->SetWindowSize((RT_REAL32)pSwapChain->GetDlgWidth(),(RT_REAL32)pSwapChain->GetDlgHeight());

			i3MatrixStack * pStack = pSwapChainTracer->GetModelViewMatrixStack();

			if( pCam->IsFlag( I3_NODEFLAG_CACHEMATRIX))
			{
				// Animation되고 있는 Camera에 대한 대응.
				pStack->Push( pCam->GetCacheMatrix(), NULL);
			}
			else
			{
				pStack->Push( pCam->GetMatrix(), NULL);
			}

			pSwapChainTracer->SetCamera( pSwapChainSgContext->getCurrentCamera());

			pSwapChainSgContext->OnUpdate( pSwapChainTracer);

			pStack->Pop();
		}

		pSwapChain->BegineScene();

		// 3D
		if( pSwapChainSgContext != NULL)
		{
			if (IsState(I3_VIEWER_STATE_PAUSED))
			{
				pSwapChainTracer->SetManualTime(m_pTimer->GetDeltaSec());
				pSwapChainTracer->SetIsManualTimeUpdate(true);
			}	
			else
			{
				pSwapChainTracer->SetIsManualTimeUpdate(false);
			}

			pSwapChainTracer->Trace( pSwapChainSgContext->get3DRoot());

			pSwapChainTracer->PushOutputList( pSwapChain->GetP2DOutput());
			pSwapChainTracer->Trace( pSwapChainSgContext->get2DMainRoot());
			pSwapChainTracer->PopOutputList();
		}
	}

// 	bool bRender = false;
// 
// 	// Pre-Trace
// 	if( pSwapChainTracer != NULL)
// 	{
// 		pSwapChainTracer->SetTime( __PT(m_DeltaTime));
// 
// 		if( pSwapChainSgContext != NULL)
// 		{
// 			//SPROFILE( 24);
// 			i3Camera * pCam = pSwapChainSgContext->getCurrentCamera();
// 			i3MatrixStack * pStack = pSwapChainTracer->GetModelViewMatrixStack();
// 
// 			if( pCam->IsFlag( I3_NODEFLAG_CACHEMATRIX))
// 			{
// 				// Animation되고 있는 Camera에 대한 대응.
// 				pStack->Push( pCam->GetCacheMatrix(), NULL);
// 			}
// 			else
// 			{
// 				pStack->Push( pCam->GetMatrix(), NULL);
// 			}
// 
// 			pSwapChainTracer->SetCamera( pSwapChainSgContext->getCurrentCamera());
// 
// 			pSwapChainSgContext->OnUpdate( pSwapChainTracer);
// 
// 			pStack->Pop();
// 			//EPROFILE( 24);
// 		}
// 
// 		// 여기서 부터 Background Thread로 동작하며 Physics 계산을 한다.
// 		// 내부적으로 Active Transform 값들을 Update시켜준다.
// 		// 이 결과는 이전 Frame의 것이다.		
// 
// // #if defined( I3G_DX)
// // 		if( !m_pContext->GetSkipRender())
// // #endif
// // 		{			
// // 			//if( m_pContext->IsReady())
// // 			{
// // 				S_SPROFILE( 4);
// // 				m_bRvBegin = m_pContext->Begin();
// // 				S_EPROFILE( 4);
// // 
// // 				S_SPROFILE( 3);
// // 				if( pSwapChainTracer->GetCollisionContext() != NULL)
// // 					pSwapChainTracer->GetCollisionContext()->SoftReset();
// // 				S_EPROFILE( 3);
// // 
// // 				if( m_bRvBegin)
// // 				{
// // 					m_bRender = true;
// // 					bRender = true;
// // 
// // 					S_SPROFILE( 5);
// // 					m_pContext->Clear();
// // 					S_EPROFILE( 5);
// // 
// // 					S_SPROFILE( 6);
// // 					m_bRvBeginRender = m_pContext->BeginRender();
// // 					m_bCallTrace = true;
// // 					S_EPROFILE( 6);
// // 				}
// // 			}
// // 		}
// 		pSwapChain->BegineScene();
// 		// 3D
// 		if( pSwapChainSgContext != NULL)
// 		{
// 			// 3D Root Trace
// 			pSwapChainTracer->Trace( pSwapChainSgContext->get3DRoot());
// 
// 			//CHAR_SOLO_RENDER
// 			//기본 월드 카메라를 가지고 와서 붙여준다..
// 			i3ClassMeta * pMeta;
// 
// 			if( m_pPlayerSgContext != NULL)
// 			{
// 				i3Camera * pCam = m_pPlayerSgContext->getCurrentCamera();
// 
// 				pMeta = pSwapChainSgContext->getCurrentCamera()->meta();
// 
// 				if( pCam == NULL ||
// 					(pCam != NULL && (pCam->same_of( pMeta) == FALSE)) )
// 				{					
// 					pCam = (i3Camera *)	pMeta->create_instance();	// CREATEINSTANCE( pMeta);
// 					pSwapChainSgContext->getCurrentCamera()->CopyTo( pCam, I3_COPY_INSTANCE);
// 
// 					m_pPlayerSgContext->setCurrentCamera( pCam);
// 					pCam->addStyle( I3_CAM_STYLE_PROJECTION);
// 				}
// 
// 				I3ASSERT( pCam != NULL);
// 
// 				//1인칭일 경우만 들어와야한다..
// // 				if (bDepthClear1PV)
// // 				{
// // 					//FOV값 조절..
// // 					pCam->SetFOV(pSwapChainSgContext->getCurrentCamera()->GetFOV() + 0.4f);
// // 
// // 					i3MatrixStack * pStack = pSwapChainTracer->GetModelViewMatrixStack();
// // 
// // 					if( pSwapChainSgContext->getCurrentCamera()->IsFlag( I3_NODEFLAG_CACHEMATRIX))
// // 						pStack->Push( m_pPlayerSgContext->getCurrentCamera()->GetCacheMatrix(), NULL);
// // 					else
// // 						pStack->Push( m_pPlayerSgContext->getCurrentCamera()->GetMatrix(), NULL);
// // 
// // 					pSwapChainTracer->SetCamera( m_pPlayerSgContext->getCurrentCamera());
// // 
// // 					m_pPlayerSgContext->OnUpdate( pSwapChainTracer );
// // 
// // 					pStack->Pop();
// // 				}
// // 				else
// 					pCam->SetFOV(pSwapChainSgContext->getCurrentCamera()->GetFOV());
// 
// 				pSwapChainTracer->PushOutputList( m_pCharacterOutput);
// 				pSwapChainTracer->Trace( m_pPlayerSgContext->get3DRoot());
// 				pSwapChainTracer->PopOutputList();
// 			}
// 		}
// 
// 		if( (m_pFramework != NULL) && bRender)
// 		{
// 			//SPROFILE( 23);
// 			m_pFramework->RenderEffect( pSwapChainTracer);
// 			//EPROFILE( 23);
// 		}
// 
// 		if( pSwapChainSgContext != NULL)
// 		{
// 			// Physix Sim.에 많은 CPU를 할당하도록 하기 위해
// 			// Tracer를 우선적으로 끝내고 GPU가 Drawing하는 동안의 시간을 모두 준다.
// 			// Trace 중에 Physix의 Actor 및 Shape가 변경되는 경우가 발생하기 때문에
// 			// Trace가 완전히 종료된 후에 Simulation을 시작해야 한다.
// 			//S_SPROFILE( 9);
// 
// 			pSwapChainTracer->PushOutputList( m_p2DOutput);
// 			pSwapChainTracer->Trace( pSwapChainSgContext->get2DMainRoot());
// 			pSwapChainTracer->PopOutputList();
// 
// 			//S_EPROFILE( 9);
// 		}
// 	}


}
void cItemToolViewer::UpdateInput()
{
	i3Viewer::UpdateInput();

	if (g_pMainSCFramework)
	{
		g_pMainSCFramework->OnInput();
		g_pMainSCFramework->OnUpdate( m_DeltaTime);
	}

	if (g_pWeaponSCFramework)
	{
		g_pWeaponSCFramework->OnInput();
		g_pWeaponSCFramework->OnUpdate( m_DeltaTime);
	}

	if (g_pItemSCFramework)
	{
		g_pItemSCFramework->OnInput();
		g_pItemSCFramework->OnUpdate( m_DeltaTime);
	}
}


void cItemToolViewer::OnlyRenderSwapChain(cSwapChainInfo* pSwapChain)
{
	i3SceneTracer *			pSwapChainTracer	= pSwapChain->GetSwapChainTracer();
	//i3SceneGraphContext *	pSwapChainSgContext = pSwapChain->GetSwapChainSgContext();

	if( pSwapChainTracer != NULL)
	{
		// 3D Rendering
		_RenderView( pSwapChainTracer->GetOutput());
		_RenderView(  pSwapChain->GetP2DOutput());

		pSwapChain->EndScene();

		pSwapChainTracer->Reset();
	}

	if( m_pSoundCtx != NULL)
	{
		m_pSoundCtx->OnDrive( m_DeltaTime);
	}
// 
// #if defined( I3_PHYSX)
// 	if( m_DeltaTime > 0.0f)
// 	{
// 		//SPROFILE( 10);
// 		m_pPhysixContext->simulate( m_DeltaTime);
// 		//EPROFILE( 10);
// 	}
// #endif
// 
// 	if( pSwapChainTracer != NULL)
// 	{
// #if defined( I3G_DX)
// //		if( !m_pContext->GetSkipRender())
// #endif
// 		{			
// 			if( m_pContext->IsReady())
// 			{
// 				//if( m_bRvBegin)
// 				{
// 					//if(m_bCallTrace == false)
// 					{
// 						//S_SPROFILE( 6);
// 						//m_bRvBeginRender = m_pContext->BeginRender();
// 						//S_EPROFILE( 6);
// 					}
// 
// 					//if( m_bRvBeginRender)
// 					{
// 						// Rasterize Chain확인
// #if defined(I3G_DX)
// 						//S_SPROFILE( 11);
// //						_BeginRasterizeChain();
// 						//S_EPROFILE( 11);
// #endif
// 
// 						// 3D Rendering
// 						//SPROFILE( 12);
// 						_RenderView( pSwapChainTracer->GetOutput());
// 						//EPROFILE( 12);
// 
// 						//CHAR_SOLO_RENDER
// 						//if (bDepthClear1PV)
// 						//	m_pContext->Clear( I3G_CLEAR_DEPTH, I3G_CLEAR_COLOR| I3G_CLEAR_STENCIL );
// 
// 						//SPROFILE( 12);
// 						_RenderView( m_pCharacterOutput);
// 						//EPROFILE( 12);
// 
// #if 0
// 						{
// 							double timeRes = i3Timer::GetProfileResolution();
// 
// 							if( (m_timeProfile[12].m_timeEnd * timeRes) > 70.0)
// 							{
// 								I3TRACE( "-------- 3D Draw : %.4f\n", m_timeProfile[12].m_timeEnd * timeRes);
// 								RESET_PROFILE();
// 								REPORT_PROFILE();
// 							}
// 						}
// #endif
// 
// 						// Rasterize Chain적용
// #if defined(I3G_DX)
// 						//S_SPROFILE( 13);
// //						_ApplyRasterizeChain(m_DeltaTime);
// 						//S_EPROFILE( 13);
// #endif
// 
// 						//if( m_bRvBeginRender)
// 						{
// 							// something to render
// 							//S_SPROFILE( 7);
// 							if( m_pFramework != NULL)
// 								m_pFramework->OnRender();
// 							//S_EPROFILE( 7);
// 						}
// 
// 						if( m_pUserPostRenderProc != NULL)
// 						{
// 							m_pUserPostRenderProc( this, m_pUserPostRenderData);
// 						}
// 
// 						//S_SPROFILE( 14);
// 						_RenderView( m_p2DOutput);
// 						//S_EPROFILE( 14);
// 
// 						if( m_CallBackExternalRender )
// 							m_CallBackExternalRender( m_DeltaTime);
// 
// 						//S_SPROFILE( 15);
// 						//m_pContext->EndRender();
// 						pSwapChain->EndScene();
// 						//S_EPROFILE( 15);
// 					}
// 				}
// 			}
// 		}
// 
// 		//S_SPROFILE( 17);
// 		if( pSwapChainTracer->GetCollisionContext() != NULL)
// 			pSwapChainTracer->GetCollisionContext()->Calculate();
// 		//S_EPROFILE( 17);
// 
// 		//S_SPROFILE( 16);
// 		pSwapChainTracer->Reset();
// 		//S_EPROFILE( 16);
// 	}
// 
// 	//S_SPROFILE( 18);
// 	if( m_pSoundCtx != NULL)
// 	{
// 		m_pSoundCtx->OnDrive( m_DeltaTime);
// 	}
// 	//S_EPROFILE( 18);
// 
// #if defined( I3_PHYSX)
// 	//SPROFILE( 21);
// 	m_pPhysixContext->fetchResults( m_DeltaTime);
// 	m_pPhysixContext->PollForWork();
// 	//EPROFILE( 21);
// #endif
// 
// #if defined( I3G_DX)
// 	//if( !m_pContext->GetSkipRender() && (m_bRender))
// #endif
// 	{
// 		//SPROFILE( 19);
// 		//m_pContext->End();
// 		
// 	//	EPROFILE( 19);
// 	}
// 
// 	//m_bRender = false;
// 
// #if !defined( I3_NO_PROFILE) 
// 	//SPROFILE( 20);
// 	RESET_PROFILE();
// 	//EPROFILE( 20);
// #endif
// 
// 	if( m_pInputManager != NULL)
// 	{
// 		m_pInputManager->Reset();
// 	}
// 
// #if !defined( I3_NO_PROFILE)
// 	Process_Profile();
// #endif
// 
// 	//EPROFILE( 0);
// 
// #if 0
// 	{
// 		double timeRes = i3Timer::GetProfileResolution();
// 
// 		if( (m_timeProfile[0].m_timeEnd * timeRes) > 100.0)
// 		{
// 			I3TRACE( "-------- Total : %.4f\n", m_timeProfile[0].m_timeEnd * timeRes);
// 			RESET_PROFILE();
// 			REPORT_PROFILE();
// 		}
// 	}
// #endif
// 
// #if defined( I3_WINDOWS)
// 	if( IsState( I3_VIEWER_STATE_SYNC_FPS))
// 	{
// 		if( m_pContext->GetVideoInfo()->m_bVSync == FALSE)
// 		{
// 			WaitForSingleObject( m_hSyncEvent, INFINITE);
// 			ResetEvent( m_hSyncEvent);
// 		}
// 	}
// #endif
	//m_bCallTrace = false;

// 	i3SceneTracer *			pSwapChainTracer	= pSwapChain->GetSwapChainTracer();
// 	i3SceneGraphContext *	pSwapChainSgContext = pSwapChain->GetSwapChainSgContext();
// 
// 
// 	if( m_DeltaTime > 0.0f)
// 	{
// 		m_pPhysixContext->simulate( m_DeltaTime);
// 	}
// 

}

void cItemToolViewer::SetSwapChainSgContext(i3SceneGraphContext * pCtx, eSwapChainType eType)
{
	if (m_vSwapChain[eType])
	{
		m_vSwapChain[eType]->SetSwapChainSgContext(pCtx);
	}

	if( m_pFramework != NULL)
	{
		m_pFramework->OnChangeScene();
	}
}

void cItemToolViewer::FindMaxResolution(INT32& nMaxW, INT32& nMaxH)
{
	I3G_IMAGE_FORMAT fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	INT32 i, cnt;
	I3G_DISPLAY_MODE_INFO pInfo;

	cnt = i3RenderContext::getDisplayModeCount( fmt);
	
	for( i = 0; i < cnt; i++)
	{
		i3RenderContext::getDisplayMode( i, fmt, &pInfo);

		if (pInfo.m_Width > nMaxW)
		{
			nMaxW = pInfo.m_Width;
		}
		if (pInfo.m_Height > nMaxH)
		{
			nMaxH = pInfo.m_Height;
		}
	}
}

void cItemToolViewer::SetWeaponHwnd(HWND hWnd, eSwapChainType eType) 
{
	if (m_vSwapChain[eType])
	{
		m_vSwapChain[eType]->SetHwnd(hWnd);
	}
}

void cItemToolViewer::SetWeaponRect(RECT rt, eSwapChainType eType)
{
	if (m_vSwapChain[eType])
	{
		m_vSwapChain[eType]->SetRectHwnd(rt);
		m_vSwapChain[eType]->PrevDeviceReset(true);
		m_vSwapChain[eType]->AfterDeviceReset(NULL);
	}
}


void cItemToolViewer::SetSCFrameWork(i3Framework* pFrameWork, eSwapChainType eType)
{
	if (m_vSwapChain[eType])
	{
		m_vSwapChain[eType]->SetCurrentFrameWork(pFrameWork);
	}
}

LRESULT WINAPI cItemToolViewer::_ItemToolViewerWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	LRESULT Result = 0;
	bool bRender = false;
	bool bDefProc = true;

	
// 	if (msg == WM_SIZE)
// 		int a = 0;

	//if (((cItemToolViewer*)i3Viewer::s_pViewer)->GetCurrentHwnd() != hwnd)
	{
		//Result = CallWindowProc( i3Viewer::s_OldWndProc, hwnd, msg, wParam, lParam);
		//return Result;
	}

	if( (i3Viewer::s_pViewer == NULL) || (i3Error::isErrorReporting()))
		return CallWindowProc( i3Viewer::s_OldWndProc, hwnd, msg, wParam, lParam);

	if(	((msg >= WM_MOUSEFIRST) && (msg <= WM_MOUSELAST)) ||
		((msg >= WM_KEYFIRST) && (msg <= WM_KEYLAST)) ||
		(msg == WM_PAINT) ||
		(msg == WM_SIZE) ||
		(msg == WM_TIMER))
	{
		bRender = true;
	}

	
	i3VideoInfo pInfo;
	i3Viewer::s_pViewer->GetVideoInfo(&pInfo);

	INT32 nPrevW = pInfo.m_Width;
	INT32 nPrevH = pInfo.m_Height;

	//INT32 nCurrW = ((cItemToolViewer*)i3Viewer::s_pViewer)->GetWeaponDlgWidth();
	//INT32 nCurrH = ((cItemToolViewer*)i3Viewer::s_pViewer)->GetWeaponDlgHeight();

	//RECT rtSrc	= i3Viewer::s_pViewer->GetRenderContext()->GetSourcePresentRect();
	//RECT rtDest = i3Viewer::s_pViewer->GetRenderContext()->GetDestPresentRect();


	POINT ptTemp;
	ptTemp.x = (short) LOWORD( lParam);
	ptTemp.y = (short) HIWORD( lParam);

	//if (nCurrH == 0 || nCurrW == 0)
	{
		pt = ptTemp;
	}
	
	switch( msg)
	{
		case WM_LBUTTONDOWN :
			{
				SetFocus( hwnd);
				//pt.x = (short) LOWORD( lParam);
				//pt.y = (short) HIWORD( lParam);

				i3Viewer::s_pViewer->OnLButtonDown( (UINT) wParam, pt);
			}
			
			break;

		case WM_LBUTTONUP :
			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnLButtonUp( (UINT) wParam, pt);
			break;

		case WM_LBUTTONDBLCLK :
			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			//i3Viewer::s_pViewer->OnLButtonDblClk( (UINT) wParam, pt);
			break;

		case WM_RBUTTONDOWN :
			//SetFocus( hwnd);

			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnRButtonDown( (UINT) wParam, pt);
			break;
				
		case WM_RBUTTONUP :
			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnRButtonUp( (UINT) wParam, pt);
			break;

		case WM_MBUTTONDOWN :
//			SetFocus( hwnd);

			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnMButtonDown( (UINT) wParam, pt);
			break;
				
		case WM_MBUTTONUP :
			//pt.x = (short) LOWORD( lParam);
			//pt.y = (short) HIWORD( lParam);

			i3Viewer::s_pViewer->OnMButtonUp( (UINT) wParam, pt);
			break;

		case WM_MOUSEMOVE :
			//SetFocus( hwnd);
			//pt.x = (short)LOWORD( lParam);
			//pt.y = (short)HIWORD( lParam);

			i3Viewer::s_pViewer->OnMouseMove( wParam, pt);
			//I3TRACE("%d %d\n",pt.x, pt.y);
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
				i3Viewer::s_pViewer->GetInputDeviceManager()->OnRawInput( wParam, lParam);
			}
			break;

		case WM_SYSKEYDOWN :
		case WM_KEYDOWN :
			if( i3Viewer::s_pViewer->IsState( I3_VIEWER_INPUT_NO_WINMSG) == FALSE)
			{
				i3Viewer::s_pViewer->OnKeyDown( wParam);
			}
			break;

		case WM_SYSKEYUP :
		case WM_KEYUP :
			if( i3Viewer::s_pViewer->IsState( I3_VIEWER_INPUT_NO_WINMSG) == FALSE)
			{
				i3Viewer::s_pViewer->OnKeyUp( wParam);
			}
			break;

		case WM_MOVE :
			i3Viewer::s_pViewer->m_bMoved = true;
			break;

		case WM_SHOWWINDOW:		
			i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( (BOOL)!wParam);	break;
		case WM_ACTIVATEAPP :	i3Viewer::s_pViewer->OnActivate( wParam == TRUE);		break;
		case WM_ACTIVATE :		i3Viewer::s_pViewer->OnActivate( LOWORD( wParam) != WA_INACTIVE);	break;
		case WM_SETFOCUS :		
			//i3Viewer::s_pViewer->OnSetFocus();	
			break;
		case WM_KILLFOCUS :		
			//i3Viewer::s_pViewer->OnKillFocus();
			break;

		case WM_SIZE :		
			{
				cMainRenderView* pView = &((CMainFrame*)AfxGetMainFrame())->GetMainRenderView();
				
				if (pView)
				{
					if (hwnd != pView->m_ViewerCtrl.GetSafeHwnd())
						break;
				}
				
			
				INT32 cx, cy;

				cx = (INT32)( lParam & 0xFFFF);
				cy = (INT32)((lParam >> 16) & 0xFFFF);
				
				//float fAspect = (float)cy / (float)cx;
				float fAspect = (float)cx / (float)cy;

				
				if (fAspect > 1.0f)
					fAspect = (float)cy / (float)cx;

				float fTempX = 0.0f;
				float fTempY = 0.0f;

//  				if (fAspect > 1.0f)
//  				{
//  					fAspect = (float)cx / (float)cy;
//  					
//  					fTempX = (float)g_pViewer->GetMaxH() * fAspect;
//  					fTempY = (float)g_pViewer->GetMaxH();
//  				}
//  				else
				{
					//무조건 최대 해상도에 마춰서 비율을 잰다
					fTempX = (float)g_pViewer->GetMaxW();
					fTempY = (float)g_pViewer->GetMaxW() * fAspect;
				}

				static bool bOnceMainRender = true;
				if(bOnceMainRender)//(wParam != SIZE_MINIMIZED) && (cx > 0) && (cy > 0))
				{
					bOnceMainRender = false;
					RECT SourceRT;
					SourceRT.left = 0;
					SourceRT.right = (LONG)g_pViewer->GetMaxW();
					SourceRT.top = 0;
					SourceRT.bottom = (LONG)g_pViewer->GetMaxH();

					RECT DestRT;
					DestRT.left = 0;
					DestRT.right = g_pViewer->GetMaxW();
					DestRT.top = 0;
					DestRT.bottom = g_pViewer->GetMaxH();

					//i3Viewer::s_pViewer->SetFramework(NULL);
					//i3Viewer::s_pViewer->GetRenderContext()->SetSourcePresentRect(SourceRT);
					//i3Viewer::s_pViewer->GetRenderContext()->SetDestPresentRect(DestRT);
					//i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( FALSE);
					//i3Viewer::s_pViewer->Resize( (UINT32)g_pViewer->GetMaxW(), (UINT32)g_pViewer->GetMaxH());
				}
				else
				{
					//i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( TRUE);
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

					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( FALSE);
				}
				else
				{
					i3Viewer::s_pViewer->GetRenderContext()->SetSkipRender( TRUE);
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

	if( i3Viewer::s_pViewer != NULL)
	{
		if( bRender && (i3Viewer::s_pViewer->GetPendingUpdateEnable() == false))
			i3Viewer::s_pViewer->Render();
	}

	pInfo.m_Width = nPrevW;
	pInfo.m_Height = nPrevH;

	return Result;
}

void cItemToolViewer::SetWndProc(HWND hWnd, bool bIsMainHwnd)
{
	SetWindowHandle(hWnd);

	g_pViewer->_ToolUndoSubclassWindow(hWnd);
	g_pViewer->_ToolSubclassWindow(hWnd);

	I3TRACE("Change Focus >> %d\n", &hWnd);
}


void cItemToolViewer::_ToolSubclassWindow(HWND hWnd)
{
	i3Viewer::s_OldWndProc = (WNDPROC) ::GetWindowLong( hWnd, GWL_WNDPROC);
	i3Viewer::s_pViewer = this;

	::SetWindowLong( hWnd, GWL_WNDPROC, (LONG) _ItemToolViewerWndProc);

	UINT32 style = ::GetWindowLong( hWnd, GWL_STYLE);
	::SetWindowLong( hWnd, GWL_STYLE, style | WS_CLIPCHILDREN);
}

void cItemToolViewer::_ToolUndoSubclassWindow(HWND hWnd)
{
	::SetWindowLong( hWnd, GWL_WNDPROC, (LONG) i3Viewer::s_OldWndProc);
	i3Viewer::s_pViewer = NULL;
}

void cItemToolViewer::PrevDeviceReset(bool bLostDevice)
{
	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
		m_vSwapChain[i]->PrevDeviceReset(bLostDevice);
}

void cItemToolViewer::AfterDeviceReset(i3RenderContext * pCtx)
{
	for (int i = 0; i < MAX_SWAPCHAIN; ++i)
		m_vSwapChain[i]->AfterDeviceReset(pCtx);
}