#if !defined( __I3_GFX_PERFORMANCE_REPORT_H)
#define __I3_GFX_PERFORMANCE_REPORT_H

#include "i3Base.h"
#include "i3GfxResource.h"

class i3RenderContext;

class I3_EXPORT_GFX i3GfxPerformanceReport : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3GfxPerformanceReport, i3GfxResource );

protected:
	UINT32			m_DrawCallCountPerFrame = 0;
	UINT32			m_TriangleCountPerFrame = 0;
	UINT32			m_FrameCount = 0;
	INT64			m_GPUProcessingTime = 0L;
	REAL32			m_FPS = 0.0f;

	REAL32			m_VCacheSize = 0.0f;
	UINT32			m_GPUTriangleCount = 0;
	UINT32			m_GPUClipGenTriangleCount = 0;
	UINT32			m_Occlusion = 0;
	REAL32			m_PipelineTime = 0.0f;
	REAL32			m_InterfaceTime = 0.0f;
	REAL32			m_VertexTime = 0.0f;
	REAL32			m_PixelTime = 0.0f;
	REAL32			m_BandwidthTime = 0.0f;
	REAL32			m_CacheUtilization = 0.0f;

#if defined( I3G_DX)
	IDirect3DQuery9 *		m_pQueryVCache = nullptr;
	IDirect3DQuery9 *		m_pQueryVertexStats = nullptr;
	IDirect3DQuery9 *		m_pQueryOcclusion = nullptr;
	IDirect3DQuery9 *		m_pQueryPipelineTime = nullptr;
	IDirect3DQuery9 *		m_pQueryInterfaceTime = nullptr;
	IDirect3DQuery9 *		m_pQueryVertexTime = nullptr;
	IDirect3DQuery9 *		m_pQueryPixelTime = nullptr;
	IDirect3DQuery9 *		m_pQueryBandwidthTime = nullptr;
	IDirect3DQuery9 *		m_pQueryCacheUtilization = nullptr;
#endif
	
	friend class i3RenderContext;

	void			Reset(void);

public:
	virtual ~i3GfxPerformanceReport(void);

	void			Begin(void);
	void			End(void);

	void			SetDrawCallCountPerFrame( UINT32 count)	{ m_DrawCallCountPerFrame = count;}
	void			SetTriangleCountPerFrame( UINT32 count)	{ m_TriangleCountPerFrame = count;}
	void			SetFPS( REAL32 fps)						{ m_FPS = fps;}

	UINT32			GetDrawCallCountPerFrame(void)			{ return m_DrawCallCountPerFrame; }
	UINT32			GetTriangleCountPerFrame(void)			{ return m_TriangleCountPerFrame; }
	UINT32			GetTextureCount(void);
	UINT32			GetFrameCount(void)						{ return m_FrameCount; }
	REAL32			GetFPS(void)							{ return m_FPS; }
	INT64			GetGPUProcessingTime(void)				{ return m_GPUProcessingTime; }

	REAL32			getVCache(void)							{ return m_VCacheSize; }
	UINT32			getGPUTriangleCount(void)				{ return m_GPUTriangleCount; }
	UINT32			getGPUClipGenTriangleCount(void)		{ return m_GPUClipGenTriangleCount; }
	UINT32			getOcclusion(void)						{ return m_Occlusion; }
	REAL32			getPipelineTiming(void)					{ return m_PipelineTime; }
	REAL32			getInterfaceTiming(void)				{ return m_InterfaceTime; }
	REAL32			getVertexTiming(void)					{ return m_VertexTime; }
	REAL32			getPixelTiming(void)					{ return m_PixelTime; }
	REAL32			getBandwidthTiming(void)				{ return m_BandwidthTime; }
	REAL32			getCacheUtilization(void)				{ return m_CacheUtilization; }

	UINT32			GetFreeVRam(void);

	virtual bool		Destroy( bool bLostDevice) override;
	virtual bool		Revive( i3RenderContext * pCtx) override;
};

#endif
