#include "i3GfxType.h"
#include "i3PerformanceReport.h"
#include "i3Texture.h"

#if defined( I3G_DX)
#include "i3Input/dxerr.h"
#include "i3GfxGlobalVariableDx.h"
#endif

I3_CLASS_INSTANCE( i3GfxPerformanceReport);

i3GfxPerformanceReport::~i3GfxPerformanceReport(void)
{
	Destroy( true);
}

void i3GfxPerformanceReport::Reset(void)
{
	m_DrawCallCountPerFrame = 0;
	m_TriangleCountPerFrame = 0;
	m_FrameCount = 0;
	m_FPS = 0.0f;

	m_VCacheSize				= 0.0f;
	m_GPUTriangleCount		= 0;
	m_GPUClipGenTriangleCount = 0;
	m_Occlusion				= 0;
	m_PipelineTime			= 0.0f;
	m_InterfaceTime			= 0.0f;
	m_VertexTime			= 0.0f;
	m_PixelTime				= 0.0f;
	m_BandwidthTime			= 0.0f;
	m_CacheUtilization		= 0.0f;
}

UINT32 i3GfxPerformanceReport::GetTextureCount(void)
{
#if defined( I3_DEBUG)
	return (UINT32) g_GlobalTextureCount;
#else
	return 0;
#endif
}

void i3GfxPerformanceReport::Begin(void)
{
#if defined( I3G_DX)
#endif
}

void i3GfxPerformanceReport::End(void)
{
	m_FrameCount++;

	#if defined( I3G_DX)
	#else
		m_Occlusion				= 0.0f;
		m_PipelineTime			= 0.0f;
		m_InterfaceTime			= 0.0f;
		m_VertexTime			= 0.0f;
		m_PixelTime				= 0.0f;
		m_BandwidthTime			= 0.0f;
		m_CacheUtilization		= 0.0f;
	#endif
}

UINT32	i3GfxPerformanceReport::GetFreeVRam(void)
{
	UINT32 mem = 0;

	#if defined( I3G_DX)
	mem = g_pD3DDevice->GetAvailableTextureMem();
	#endif

	return mem;
}

bool i3GfxPerformanceReport::Destroy( bool bLostDevice)
{
#if defined( I3G_DX)
	I3_SAFE_RELEASE_NODBG( m_pQueryVCache);
	I3_SAFE_RELEASE_NODBG( m_pQueryVertexStats);
	I3_SAFE_RELEASE_NODBG( m_pQueryOcclusion);
	I3_SAFE_RELEASE_NODBG( m_pQueryPipelineTime);
	I3_SAFE_RELEASE_NODBG( m_pQueryInterfaceTime);
	I3_SAFE_RELEASE_NODBG( m_pQueryVertexTime);
	I3_SAFE_RELEASE_NODBG( m_pQueryPixelTime);
	I3_SAFE_RELEASE_NODBG( m_pQueryBandwidthTime);
	I3_SAFE_RELEASE_NODBG( m_pQueryCacheUtilization);
#endif

	return i3GfxResource::Destroy( bLostDevice);
}

bool i3GfxPerformanceReport::Revive( i3RenderContext * pCtx)
{
#if defined( I3G_DX)
	Reset();
#endif

	return i3GfxResource::Revive( pCtx);
}