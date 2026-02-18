#if !defined( __I3_GFX_PERFORMANCE_REPORT_H)
#define __I3_GFX_PERFORMANCE_REPORT_H

#include "i3Base.h"
#include "i3GfxResource.h"

class i3RenderContext;

class I3_EXPORT_GFX i3GfxPerformanceReport : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxPerformanceReport);

protected:
	UINT32			m_DrawCallCountPerFrame;
	UINT32			m_TriangleCountPerFrame;
	UINT32			m_FrameCount;
	INT64			m_GPUProcessingTime;
	REAL32			m_FPS;

	REAL32			m_VCacheSize;
	UINT32			m_GPUTriangleCount;
	UINT32			m_GPUClipGenTriangleCount;
	UINT32			m_Occlusion;
	REAL32			m_PipelineTime;
	REAL32			m_InterfaceTime;
	REAL32			m_VertexTime;
	REAL32			m_PixelTime;
	REAL32			m_BandwidthTime;
	REAL32			m_CacheUtilization;

#if defined( I3G_DX)
	IDirect3DQuery9 *		m_pQueryVCache;
	IDirect3DQuery9 *		m_pQueryVertexStats;
	IDirect3DQuery9 *		m_pQueryOcclusion;
	IDirect3DQuery9 *		m_pQueryPipelineTime;
	IDirect3DQuery9 *		m_pQueryInterfaceTime;
	IDirect3DQuery9 *		m_pQueryVertexTime;
	IDirect3DQuery9 *		m_pQueryPixelTime;
	IDirect3DQuery9 *		m_pQueryBandwidthTime;
	IDirect3DQuery9 *		m_pQueryCacheUtilization;
#endif
	
	friend class i3RenderContext;

	void			Reset(void);
public:
	i3GfxPerformanceReport(void);
	virtual ~i3GfxPerformanceReport(void);

	void			Begin(void);
	void			End(void);

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

	virtual BOOL		Destroy( bool bLostDevice);
	virtual BOOL		Revive( i3RenderContext * pCtx);
};

#endif
