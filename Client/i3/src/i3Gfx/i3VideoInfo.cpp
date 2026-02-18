#include "i3GfxType.h"
#include "i3VideoInfo.h"

#ifdef I3G_DX
#include "i3GfxUtilDX.h"
#endif

#ifdef I3G_XBOX
#include "i3GfxUtilXbox.h"
#endif

#if defined( I3G_PSP)
#include "i3GfxUtilPSP.h"
#endif

#if defined( I3G_OGLES)
#include "i3GfxUtilOGLES.h"
#endif

//I3_CLASS_INSTANCE( i3VideoInfo, i3ElementBase);
I3_CLASS_INSTANCE( i3VideoInfo);

i3VideoInfo::i3VideoInfo(void)
{
#if defined( I3G_PSP)
	m_Width = 480;
	m_Height = 272;
	m_RenderTargetFormat = I3G_IMAGE_FORMAT_RGBX_8888;

	m_bInterlaced = false;
	m_bFullScreenWindowModeEnable = FALSE;

#elif defined( I3G_OGLES)
	m_Width = 320;
	m_Height = 240;
	m_RenderTargetFormat = I3G_IMAGE_FORMAT_ARGB_1555;

	m_bFullScreen = true;
#endif
}


i3VideoInfo & i3VideoInfo::operator = ( const i3VideoInfo & src)
{
	m_AdapterIndex				= src.m_AdapterIndex			;
	m_ModeIndex					= src.m_ModeIndex;
	m_Width						= src.m_Width;
	m_Height					= src.m_Height;
	m_RenderTargetFormat		= src.m_RenderTargetFormat;
	m_ZBufferBits				= src.m_ZBufferBits;
	m_StencilBufferBits			= src.m_StencilBufferBits;
								
	m_bFullScreen				= src.m_bFullScreen;

	m_bInterlaced				= src.m_bInterlaced;
	m_bVSync					= src.m_bVSync;
	m_RefreshRate				= src.m_RefreshRate;
	m_FrameBufferCount			= src.m_FrameBufferCount;
	m_bHalfSizeFrameBuffer		= src.m_bHalfSizeFrameBuffer;
	m_VideoType					= src.m_VideoType;
	m_bMultiThread				= src.m_bMultiThread;
	m_bFullScreenWindowModeEnable = src.m_bFullScreenWindowModeEnable;

	m_MultiSampling				= src.m_MultiSampling;
	m_MultiSamplingQuality		= src.m_MultiSamplingQuality;
	m_Flags						= src.m_Flags;

	return * this;
}

#if defined( I3G_DX) || defined( I3G_XBOX)
i3VideoInfo & i3VideoInfo::operator = (D3DPRESENT_PARAMETERS & pp)
{
	m_Width = pp.BackBufferWidth;
	m_Height = pp.BackBufferHeight;
#ifdef I3G_DX
	m_RenderTargetFormat = i3DXUT_GetI3ImageFormat( pp.BackBufferFormat);
#elif I3G_XBOX
	m_RenderTargetFormat = i3_GetI3ImageFormat( pp.BackBufferFormat);
#endif
	m_FrameBufferCount = pp.BackBufferCount;
	m_bFullScreen = !pp.Windowed;
	m_Flags = pp.Flags;
	
	if( m_bFullScreen)
	{
		m_RefreshRate = (REAL32) pp.FullScreen_RefreshRateInHz;
	}

	m_MultiSampling = pp.MultiSampleType;
	m_MultiSamplingQuality = pp.MultiSampleQuality;

	m_bVSync = ( pp.PresentationInterval == D3DPRESENT_INTERVAL_ONE );	// ÀÌ °æ¿ì VSync

	return * this;
}

void i3VideoInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method )
{
	i3ElementBase::CopyTo(pDest, method);

	i3VideoInfo* pVinfo = (i3VideoInfo*)pDest;
	if (method == I3_COPY_REF)
	{
		pVinfo->m_AdapterIndex			=m_AdapterIndex			;
		pVinfo->m_ModeIndex				=m_ModeIndex;
		pVinfo->m_Width					=m_Width;
		pVinfo->m_Height				=m_Height;
		pVinfo->m_RenderTargetFormat	=m_RenderTargetFormat;
		pVinfo->m_ZBufferBits			=m_ZBufferBits;
		pVinfo->m_StencilBufferBits		=m_StencilBufferBits;

		pVinfo->m_bFullScreen			=m_bFullScreen;
		pVinfo->m_bInterlaced			=m_bInterlaced;
		pVinfo->m_bVSync				=m_bVSync;
		pVinfo->m_RefreshRate			=m_RefreshRate;
		pVinfo->m_FrameBufferCount		=m_FrameBufferCount;
		pVinfo->m_bHalfSizeFrameBuffer	=m_bHalfSizeFrameBuffer;
		pVinfo->m_VideoType				=m_VideoType;
		pVinfo->m_bMultiThread			=m_bMultiThread;
		pVinfo->m_bFullScreenWindowModeEnable = m_bFullScreenWindowModeEnable;

		pVinfo->m_MultiSampling			=m_MultiSampling;
		pVinfo->m_MultiSamplingQuality	=m_MultiSamplingQuality;
		pVinfo->m_Flags					=m_Flags;
	}
}

#endif
