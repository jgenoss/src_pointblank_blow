#if !defined( __I3_VIDEO_INFO_H)
#define __I3_VIDEO_INFO_H

#include "../i3Base/i3CommonType.h"
#include "i3GfxDefine.h"

enum I3G_VIDEO_TYPE
{
	I3G_VIDEO_TYPE_NONE		= 0,
	I3G_VIDEO_TYPE_NTSC,
	I3G_VIDEO_TYPE_NTSCJ,
	I3G_VIDEO_TYPE_PAL,
	I3G_VIDEO_TYPE_MPAL,
	I3G_VIDEO_TYPE_HDTV,
	I3G_VIDEO_TYPE_VESA,
	I3G_VIDEO_TYPE_EUR60,
	I3G_VIDEO_TYPE_PROGRESSIVE
};

class I3_EXPORT_GFX i3VideoInfo : public i3ElementBase
{
	I3_CLASS_DEFINE( i3VideoInfo);
public:
	INT32					m_AdapterIndex;
	INT32					m_ModeIndex;

	INT32					m_Width;
	INT32					m_Height;
	I3G_IMAGE_FORMAT		m_RenderTargetFormat;
	INT32					m_ZBufferBits;
	INT32					m_StencilBufferBits;

	BOOL					m_bFullScreen;
	BOOL					m_bInterlaced;
	BOOL					m_bVSync;
	REAL32					m_RefreshRate;
	INT32					m_FrameBufferCount;
	BOOL					m_bHalfSizeFrameBuffer;
	I3G_VIDEO_TYPE			m_VideoType;

	bool					m_bMultiThread;

	INT32					m_MultiSampling;
	INT32					m_MultiSamplingQuality;

	UINT32					m_Flags; // Extension

	i3VideoInfo(void) NOTHROW;
	virtual ~i3VideoInfo(void) NOTHROW;

#if defined(I3G_DX) || defined(I3G_XBOX)
	i3VideoInfo & operator = (D3DPRESENT_PARAMETERS & pp);
	i3VideoInfo & operator = ( i3VideoInfo & src);
#endif

};

#endif
