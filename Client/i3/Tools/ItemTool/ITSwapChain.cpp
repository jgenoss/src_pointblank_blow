#include "StdAfx.h"
#include "ITSwapChain.h"


I3_CLASS_INSTANCE(cSwapChainInfo); //, i3ElementBase)

cSwapChainInfo* cSwapChainInfo::m_spSwapChainInfo = NULL;
cSwapChainInfo::cSwapChainInfo()
:	m_hWnd(NULL)
,	m_pBackBuffer(NULL)
,	m_nDlgWidth(0)
,	m_nDlgHeight(0), m_pSwapChain(NULL)			// 초기화 누락.(2012.09.24.수빈)
,	m_pSwapChainSgContext(NULL)
,	m_pSwapChainTracer(NULL)
,	m_pCurrentFrameWork(NULL)
{

}
cSwapChainInfo::~cSwapChainInfo()
{
	I3_SAFE_RELEASE(m_p2DOutput);
	I3_SAFE_RELEASE_NODBG(m_pBackBuffer);
	I3_SAFE_RELEASE_NODBG(m_pSwapChain);
	I3_SAFE_RELEASE(m_pSwapChainSgContext);
	I3_SAFE_RELEASE(m_pSwapChainTracer);
}
void cSwapChainInfo::Create(i3RenderContext* pContext, i3SoundContext* pSoundContext)
{
	//m_spSwapChainInfo = this;

	m_pSwapChainTracer = i3SceneTracer::new_object();
	m_pSwapChainTracer->SetRenderContext( pContext);
	m_pSwapChainTracer->setSoundContext( pSoundContext);
	
	m_p2DOutput = i3RenderAttrListAttr::new_object();
	//pContext->SetLostDeviceHandler(PrevDeviceReset);
	//pContext->SetReviveHandler(AfterDeviceReset);

	UpdateSwapChain();
}

void cSwapChainInfo::UpdateSwapChain()
{
	if (g_pViewer == NULL)
		return;
	
	POINT2D sz;


	m_nDlgWidth = GetRect().right - GetRect().left;
	m_nDlgHeight= GetRect().bottom - GetRect().top;

	sz.x = m_nDlgWidth;//g_pViewer->GetMaxW();
	sz.y = m_nDlgHeight;//g_pViewer->GetMaxH();
	if (m_pCurrentFrameWork)
		m_pCurrentFrameWork->OnSize(&sz);

// 	i3GfxResource * pRes = i3GfxResource::getHeadOfGfxResource();
// 	while( pRes != NULL)
// 	{
// 		pRes->Revive(g_pViewer->GetRenderContext());
// 		pRes = pRes->getNextRes();
// 	}

	i3RenderContext* pRenderContext = g_pViewer->GetRenderContext();
	//스왑 채인을 생성한다
	i3VideoInfo* pVideoInfo = pRenderContext->GetVideoInfo();
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.Windowed						= TRUE;
	d3dpp.BackBufferWidth				= g_pViewer->GetMaxW();//m_nDlgWidth;
	d3dpp.BackBufferHeight				= g_pViewer->GetMaxH();//m_nDlgHeight;
	d3dpp.BackBufferFormat				= i3DXUT_GetNativeImageFormat( pVideoInfo->m_RenderTargetFormat);
	d3dpp.BackBufferCount				= 1;
	d3dpp.hDeviceWindow					= GetHwnd();	
	d3dpp.EnableAutoDepthStencil		= TRUE;
	d3dpp.AutoDepthStencilFormat		= D3DFMT_D24X8;	
	d3dpp.MultiSampleType				= (D3DMULTISAMPLE_TYPE) pVideoInfo->m_MultiSampling;
	d3dpp.MultiSampleQuality			= pVideoInfo->m_MultiSamplingQuality;
	d3dpp.SwapEffect					= D3DSWAPEFFECT_COPY;
	d3dpp.Flags							= 0;

	if( d3dpp.Windowed == FALSE)
		d3dpp.FullScreen_RefreshRateInHz	= (INT32) pVideoInfo->m_RefreshRate;
	else
		d3dpp.FullScreen_RefreshRateInHz	= 0;

	d3dpp.PresentationInterval			= D3DPRESENT_INTERVAL_IMMEDIATE;

	IDirect3DDevice9* pD3DDevice = (IDirect3DDevice9*)pRenderContext->GetNativeDevice();

	pD3DDevice->CreateAdditionalSwapChain(&d3dpp, &m_pSwapChain);
}

bool cSwapChainInfo::RenderSwapChain()
{
	if (g_pViewer == NULL || m_pSwapChain == NULL || m_hWnd == NULL)
		return false;

	return true;
}

void cSwapChainInfo::BegineScene()
{
	i3RenderContext* pRenderContext = g_pViewer->GetRenderContext();
	IDirect3DDevice9* pDevice = (IDirect3DDevice9*)pRenderContext->GetNativeDevice();

	m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBuffer);
	pDevice->SetRenderTarget(0, m_pBackBuffer);

	pDevice->Clear( 0,
					NULL,
					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					D3DCOLOR_COLORVALUE( 0.4f, 0.4f, 0.4f, 1.0f),
					1.0f,
					0);
	pDevice->BeginScene();
}
void cSwapChainInfo::EndScene()
{
	i3RenderContext* pRenderContext = g_pViewer->GetRenderContext();
	IDirect3DDevice9* pDevice = (IDirect3DDevice9*)pRenderContext->GetNativeDevice();

	pDevice->EndScene();

	RECT rtSrc;
	memset(&rtSrc,0, sizeof(RECT));
	rtSrc.right	= g_pViewer->GetMaxW();
	rtSrc.bottom	= g_pViewer->GetMaxH();

	m_pSwapChain->Present(&rtSrc,&m_rect,m_hWnd,NULL,NULL);
	m_pBackBuffer->Release();
	m_pBackBuffer = NULL;
}

void cSwapChainInfo::PrevDeviceReset(bool bLostDevice)
{
	I3_SAFE_RELEASE_NODBG(m_pSwapChain);
}

void cSwapChainInfo::AfterDeviceReset(i3RenderContext * pCtx)
{
	UpdateSwapChain();
}

void cSwapChainInfo::Trace()
{

}

void cSwapChainInfo::Render()
{

}

void cSwapChainInfo::SetSwapChainSgContext(i3SceneGraphContext * val)
{
	I3_REF_CHANGE( m_pSwapChainSgContext, val); 

	if( m_pSwapChainTracer != NULL)
	{
		m_pSwapChainTracer->setSgContext( val);
	}
}