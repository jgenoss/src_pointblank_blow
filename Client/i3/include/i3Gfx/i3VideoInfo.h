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
	I3_EXPORT_CLASS_DEFINE( i3VideoInfo, i3ElementBase);
public:
	INT32					m_AdapterIndex = -1;
	INT32					m_ModeIndex = 0;

	INT32					m_Width = 800;
	INT32					m_Height = 600;
	I3G_IMAGE_FORMAT		m_RenderTargetFormat = I3G_IMAGE_FORMAT_BGRX_8888;
	INT32					m_ZBufferBits = 24;
	INT32					m_StencilBufferBits = 0;
	I3G_IMAGE_FORMAT		m_DepthFormat = I3G_IMAGE_FORMAT_D24X8;

	bool					m_bFullScreen = false;
	bool					m_bInterlaced = false;
	bool					m_bVSync = false;
	REAL32					m_RefreshRate = 60.0f;
	INT32					m_FrameBufferCount = 1;
	bool					m_bHalfSizeFrameBuffer = false;
	I3G_VIDEO_TYPE			m_VideoType = I3G_VIDEO_TYPE_NONE;

	bool					m_bMultiThread = false;
	bool					m_bFullScreenWindowModeEnable;						// 창모드인 경우엔 쓰이지 않고, 전체화면의 경우의 추가옵션이다..
	INT32					m_MultiSampling = 0;
	INT32					m_MultiSamplingQuality = 0;

	UINT32					m_Flags = 0; // Extension

	i3VideoInfo(void) NOTHROW;

#if defined(I3G_DX) || defined(I3G_XBOX)
	i3VideoInfo & operator = (D3DPRESENT_PARAMETERS & pp);
	i3VideoInfo & operator = ( const i3VideoInfo & src);
#endif

	virtual void CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method);

};

#endif
