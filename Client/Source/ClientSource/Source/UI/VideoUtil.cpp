#include "pch.h"

#include "VideoUtil.h"
#include "UIClanFunction.h"
#include "UIUtil.h"

namespace
{
	struct MemFreeFn
	{	
		template< class t >
		void operator()(t* p) {	i3MemFree(p); }	
	};
	struct DeleteFn
	{
		template<class T>
		void operator()(T* data) const { I3_SAFE_DELETE( data); }
	};
}

VideoUtil::~VideoUtil()
{
	remove_video_info();
	remove_AA_info();
	remove_Hz_info();
	removeAllVCResInfo();
}

void VideoUtil::add_video_info(I3G_DISPLAY_MODE_INFO * pInfo, INT32 width, INT32 height)
{
	VID_INFO * pNew = (VID_INFO *) i3MemAlloc( sizeof( VID_INFO) );

	pNew->m_Width = pInfo->m_Width;
	pNew->m_Height = pInfo->m_Height;
	pNew->m_Hz = pInfo->m_RefreshRate;

	m_VidList.push_back( pNew );
}

void VideoUtil::remove_video_info(void)
{
	for_each(m_VidList.begin(), m_VidList.end(), MemFreeFn() );
	m_VidList.clear();
}

void VideoUtil::init_video_info(INT32 bits, bool win_mode)
{
	remove_video_info();

	i3GfxCaps * pCaps = i3RenderContext::GetCaps();	
	I3G_DISPLAY_MODE_INFO info;
	I3G_IMAGE_FORMAT fmt;

	if( bits == 16)
		fmt = I3G_IMAGE_FORMAT_BGR_565;
	else
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	INT32 src_width = ::GetSystemMetrics( SM_CXSCREEN);
	INT32 src_height = ::GetSystemMetrics( SM_CYSCREEN);
	INT32 max_src_width = src_width, max_src_height = src_height;

	// 물리적인 최대 해상도를 구한다.
	INT32 modeCount = pCaps->getDisplayModeCount( fmt);
	for( INT32 i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if( (info.m_Width * info.m_Height) > ( max_src_width * max_src_height) ) //픽셀 수가 많으면 더 큰 해상도라고 판단함.
		{
			max_src_width = info.m_Width;
			max_src_height = info.m_Height;
		}
	}

	REAL32 ratioMonitor = (REAL32) max_src_width / max_src_height;
	for( INT32 i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if( (info.m_Width < 1024) || (info.m_Height < 768) ) //|| (info.m_RefreshRate != 60.0f) )
			continue;

		REAL32 ratio = (REAL32) info.m_Width / info.m_Height;

		bool bAdd = false;

		
		if( ratioMonitor > 1.5f || ( (ratioMonitor < 1.5f) && (ratio < 1.5f) )  )
			bAdd = true;

		// + 40hz 이하 삭제.
//		if ((info.m_RefreshRate <= 40.0f))
//			bAdd = false;

		if (bAdd)
		{
			int info_idx = find_video_info_idx( info.m_Width, info.m_Height );
			
			if( info_idx == -1 )
			{
				//윈도우 모드일 경우엔 screen 크기보다 작은 해상도만 지원한다.
				//전체화면일 경우엔 모든 해상도가 가능하다
				if(!win_mode || max_src_width > info.m_Width && max_src_height > info.m_Height )
					add_video_info( &info, src_width, src_height);
			}
			else		// 없으면 추가하겠지만, 중복된 해상도라도, 60에 가까운 놈을 선택해준다...
			{
				VID_INFO* existed_info = get_video_info(info_idx);
				
				REAL32 diff_old = fabs(60.0f - existed_info->m_Hz);
				REAL32 diff_new = fabs(60.0f - info.m_RefreshRate);
				
				if (diff_new < diff_old )		// 60에 가까운 놈으로 재조정..
				{
					existed_info->m_Hz = info.m_RefreshRate;
				}
			}
		}
	}

	// 현재의 해상도가 있는지 확인한다.
	if( find_video_info_idx( src_width, src_height ) == -1 )
	{
		// 만약 현재의 해상도가 존재하지 않는다면...
		// 규칙과는 무관하게 현재의 해상도를 추가해둔다. 
		info.m_Width = src_width;
		info.m_Height = src_height;
		info.m_RefreshRate = 0;
		if(!win_mode || max_src_width > info.m_Width && max_src_height > info.m_Height )
			add_video_info( &info, src_width, src_height);
	}
}

int VideoUtil::find_video_info_idx(INT32 width, INT32 height) const
{
	size_t size = m_VidList.size();
	for(size_t i = 0; i < size; i++)
	{
		if((m_VidList[i]->m_Width == width) && (m_VidList[i]->m_Height == height))
		{
			return i;
		}
	}
	return -1;
}

int VideoUtil::find_max_video_info_idx(INT32* width, INT32* height) const
{
	size_t size = m_VidList.size();
	INT32 MaxFixel = 0;
	size_t result =  0xFFFFFFFF;

	for(size_t i = 0; i < size; i++)
	{
		if( (m_VidList[i]->m_Width * m_VidList[i]->m_Height) > MaxFixel)
		{
			MaxFixel = m_VidList[i]->m_Width * m_VidList[i]->m_Height;
			result = i;
		}
	}

	if( result != -1 )
	{
		*width   = m_VidList[result]->m_Width;
		*height  = m_VidList[result]->m_Height;
	}

	return static_cast<int>(result);
}

i3::wstring VideoUtil::get_video_info_string() const
{
	ucf::ComboString cs;

	INT32 Count = m_VidList.size();
	i3::wstring conv;

	for( INT32 i = 0; i < Count; i++)
	{	
		VID_INFO * pInfo = m_VidList[i];
		CEnvSet::GetVideoInfoString( pInfo->m_Width, pInfo->m_Height, conv);	
		cs<<conv;
	}

	return i3::wstring(cs);
}

void VideoUtil::init_AA_Info(INT32 bits, bool win_mode, bool all)
{
	remove_AA_info();

	I3G_IMAGE_FORMAT fmt;
	if( bits == 32)
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;
	else
		fmt = I3G_IMAGE_FORMAT_BGR_565;

	i3::vector<I3G_AA_INFO*> AAList;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	pCaps->buildAAList( fmt, !win_mode, AAList);

	const size_t size = AAList.size();
	for(size_t i = 0; i < size; i++)
	{
		I3G_AA_INFO * info = AAList[i];
		if( all || info->m_QualityCount == 0)
			m_AAList.push_back( info );
		else
		{
			I3MEM_SAFE_FREE( info);
		}
	}
}

void VideoUtil::remove_AA_info(void)
{
	i3::for_each(m_AAList.begin(), m_AAList.end(), MemFreeFn() );
	m_AAList.clear();
}


int VideoUtil::find_AA_info_idx(INT32 type, INT32 quality) const
{
	size_t size = m_AAList.size();
	for(size_t i = 0; i < size; i++)
	{
		if((m_AAList[i]->m_Sampling == type) && (m_AAList[i]->m_QualityCount == quality))
			return i;
	}

	return -1;
}

size_t VideoUtil::size_AA_Info(bool include_eight_anti) const
{
	size_t ret = 0;

	const size_t size = m_AAList.size();
	for(size_t i = 0; i < size; i++)
	{
		if( !include_eight_anti && m_AAList[i]->m_Sampling == 8)
			continue;

		++ret;
	}

	return ret;
}




void VideoUtil::init_Hz_Info(INT32 bits, INT32 s_width, INT32 s_height)
{
	remove_Hz_info();

	INT32 src_width  = s_width;
	INT32 src_height = s_height;

	I3G_IMAGE_FORMAT fmt;
	I3G_DISPLAY_MODE_INFO info;
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	if( bits == 16)
		fmt = I3G_IMAGE_FORMAT_BGR_565;
	else
		fmt = I3G_IMAGE_FORMAT_BGRX_8888;

	UINT32 modeCount = pCaps->getDisplayModeCount( fmt );
	for(UINT32 i = 0; i < modeCount; i++)
	{
		pCaps->getDisplayMode( i, fmt, &info);

		if((info.m_Width == src_width) && (info.m_Height == src_height))
		{
//			if ((info.m_RefreshRate <= 40.0f))
//				continue;

			m_HzList.push_back( (INT32)info.m_RefreshRate );
		}
	}
}

void VideoUtil::remove_Hz_info(void)
{
	m_HzList.clear();
}

int VideoUtil::find_Hz_info_idx(INT32 hz) const
{
	INT32 ret = -1, mindiff = 100;

	size_t size = m_HzList.size();
	for(size_t i = 0; i < size; i++)
	{
		if( abs( m_HzList[i] - hz) < mindiff)
		{
			ret = i;
			mindiff = abs( m_HzList[i] - hz);
		}
	}

	return ret;
}

i3::wstring VideoUtil::get_Hz_info_string() const
{
	ucf::ComboString cs;

	INT32 Count = m_HzList.size();
	i3::wstring conv;

	for (INT32 i = 0; i < Count; i++)
	{
		INT32 Info = m_HzList[i];
		i3::sprintf(conv, GAME_RCSTRING("STR_OPT_TAP_VIDEO_REFRESH_RATE_HZ"), Info);
		cs << conv;
	}

	return i3::wstring(cs);
}





void VideoUtil::init_VCRes_info()
{
}

void VideoUtil::removeAllVCResInfo(void)
{
}




void VideoUtil::ApplyGammaState(REAL32 gamma)
{
#ifdef USE_GAMMAOPTION_BY_SHADER
	// shader로 바꿈
	REAL32 fGammaScale = 1.0f * 0.01f;
	REAL32 fAdjustGamma = fGammaScale * gamma + 0.5f;

	i3PostProcessGamma* pGamma = g_pFramework->GetPostProcessGamma();
	if (nullptr != pGamma)
		pGamma->SetGammaValue(fAdjustGamma);
#else
	D3DGAMMARAMP ramp;
	REAL32 fGammaScale = 1.0f * 0.01f;
	REAL32 fGamma = fGammaScale * gamma + 0.5f;

	for (INT32 i = 0; i < 256; i++)
	{
		WORD f = (WORD)(255 * pow((float)i / 256, 1 / fGamma));
		if (f > 255) 	f = 255;
		ramp.red[i] = ramp.green[i] = ramp.blue[i] = ((WORD)f) << 8;
	}

	// 감마설정
	((IDirect3DDevice9 *)g_pViewer->GetRenderContext()->GetNativeDevice())->SetGammaRamp(0, D3DSGR_NO_CALIBRATION, &ramp);

#endif
}

void VideoUtil::ApplyVideoCaptureResolution(int level)
{
	g_pFramework->SetVideoCaptureInfo(level);
}

void VideoUtil::ApplyVideoEnv()
{
	VideoContext context;

	context.m_bFullScreen = g_pEnvSet->m_bFullScreen;
	context.m_bVSync = g_pEnvSet->m_bVSync;
	context.m_bTriLinearFilter = g_pEnvSet->m_bTriLinearFilter;
	context.m_b16BitsDepth = g_pEnvSet->m_b16BitsDepth;

	context.m_nScreenWidth = g_pEnvSet->m_nScreenWidth;
	context.m_nScreenHeight = g_pEnvSet->m_nScreenHeight;
	context.m_nScreenHz = g_pEnvSet->m_nScreenHz;
	context.m_ScreenDepth = g_pEnvSet->m_ScreenDepth;

	context.m_AA = g_pEnvSet->m_AA;
	context.m_AAQ = g_pEnvSet->m_AAQ;

	ApplyVideo(context);
}

void VideoUtil::ApplyVideo(const VideoContext & cxt)
{
	i3Viewer * pViewer = g_pFramework->GetViewer();
	i3VideoInfo* pVideo = pViewer->GetRenderContext()->GetVideoInfo();
	i3VideoInfo VideoInfo;
	
	VideoInfo = *pVideo;
	pVideo = &VideoInfo;

	pVideo->m_bFullScreen = cxt.m_bFullScreen;
	pVideo->m_Width = cxt.m_nScreenWidth;
	pVideo->m_Height = cxt.m_nScreenHeight;
	pVideo->m_RefreshRate = (REAL32)cxt.m_nScreenHz;
	pVideo->m_bVSync = cxt.m_bVSync;

	if( cxt.m_ScreenDepth == 32)
		pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
	else
		pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGR_565;

	if( cxt.m_b16BitsDepth)
		pVideo->m_ZBufferBits = 16;
	else
		pVideo->m_ZBufferBits = 24;

	pVideo->m_MultiSampling = cxt.m_AA;
	pVideo->m_MultiSamplingQuality	= cxt.m_AAQ;

	// 풀스크린 윈도우 모드 처리의 유일한 변경지점입니다... false로 강제하여 안되도록 하되..생성, 리셋 실패시 내부에서 true로 조정시킵니다..
	pVideo->m_bFullScreenWindowModeEnable = false;
	//

#if !defined( UI2_RESOLUTION_FIX)
	pViewer->SetVideoInfo( pVideo);
#endif

	//  [4/3/2012 dosun.lee]
	//	UI 마우스 HitTest 가 꼬이는 현상이 있어서 제거함	
	if( i3UI::getManager() != nullptr)
		i3UI::getManager()->OnSize();
}


void VideoUtil::ApplySpecularQuality(INT32 SpecularQuality)
{
	switch( SpecularQuality)
	{
	case 0 :
		g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( false);
		g_pFramework->SetFresnelEnable( false);
		break;

	case 1 :
		g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( true);
		g_pFramework->SetFresnelEnable( false);
		break;

	case 2 :
		g_pViewer->GetRenderContext()->getGfxOption()->setSpecularMapEnable( true);
		g_pFramework->SetFresnelEnable( true);
		break;
	}

	g_pCharaManager->SetDummyCharacterOption( g_pEnvSet->m_bEnableNormalMap?true:false, (SpecularQuality>0)?true:false);
}

void VideoUtil::ApplyShadowQuality(INT32 ShadowQuality)
{
	g_pFramework->setShadowQuality( ShadowQuality);
}

void VideoUtil::ApplyNormalMap( bool bEnable)
{
	g_pFramework->SetNormalMapEnable( bEnable);
}


/******************************/
VideoUtil::EnableContext VideoUtil::m_enable_context;
const VideoUtil::EnableContext& VideoUtil::get_ability_context()
{
	static bool init = false;
	if(!init)
	{
		check_enable(m_enable_context);
		init = true;
	}
	return m_enable_context;
}
void VideoUtil::check_enable(EnableContext & cxt)
{
	cxt.normal_map = true;
	cxt.shadow_quality = true;
	cxt.specular_quality = 2;	//최대값이 2 다. {0,1,2}

	// 설정된 Option에 따라 Shader 코드를 정한다.
	i3ShaderCode code;

	code.setDiffuseMapEnable( true);
	code.setReflectMapEnable( true);
	code.setReflectMaskMapEnable( true);
	code.setPPLShader( false);
	code.setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);
	code.setLightType( 0, I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL);
	code.setLightingEnable( true);

	//세부 재질 표현
	code.setNormalMapEnable( true);
	code.setPPLShader( true);

	//광원효과
	code.setLightType( 1, I3G_SHADER_FLAG_LIGHT_TYPE_POINT);

	//그림자 효과
	code.setShadowType( 0, I3G_SHADER_FLAG_SHADOW_SHADOWMAP);

	//빛반사 효과
	code.setSpecularMapEnable( true);	
	code.setFresnelEnable( true);

	i3RenderContext * pCtx = g_pViewer->GetRenderContext();
	if( pCtx->ValidateShader( &code) == false)
	{
		cxt.normal_map = code.getNormalMapEnable();
		cxt.shadow_quality = ( code.getShadowType(0) == I3G_SHADER_FLAG_SHADOW_SHADOWMAP );

		if( code.getFresnelEnable() == false)
		{
			cxt.specular_quality = 1;
			if( code.getSpecularMapEnable() == false)
				cxt.specular_quality = 0;
		}
	}
}

/******************************/
UINT VideoUtil::check_shadow_ability(INT32 type)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	I3G_GPU_CLASS gpu_class = pCaps->getGPUClass();

	if( type != 0 )
	{
		switch(gpu_class)
		{
		case I3G_GPU_CLASS_F: case I3G_GPU_CLASS_E: case I3G_GPU_CLASS_D: return HIGH_RISK;
		case I3G_GPU_CLASS_B: case I3G_GPU_CLASS_C: return LOW_RISK;
		}
	}

	return PASS;
}

UINT VideoUtil::check_texture_ability(INT32 type)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	I3G_GPU_CLASS gpu_class = pCaps->getGPUClass();

	if( type == 1 )
	{
		switch(gpu_class)
		{
		case I3G_GPU_CLASS_F: case I3G_GPU_CLASS_E: return HIGH_RISK;
		}
	}

	return PASS;
}


UINT VideoUtil::check_dynamiclight_ability(bool dynamic_light, bool normal_map)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	I3G_GPU_CLASS gpu_class = pCaps->getGPUClass();

	if( dynamic_light )
	{
		switch(gpu_class)
		{
		case I3G_GPU_CLASS_F: case I3G_GPU_CLASS_E: 			
			if( normal_map )	return HIGH_RISK; else	return LOW_RISK;
		case I3G_GPU_CLASS_D: return LOW_RISK;
		}
	}

	return PASS;
}

UINT VideoUtil::check_normalmap_ability(bool enable)
{
	i3GfxCaps * pCaps = i3RenderContext::GetCaps();
	I3G_GPU_CLASS gpu_class = pCaps->getGPUClass();

	if( enable )
	{
		switch(gpu_class)
		{
		case I3G_GPU_CLASS_F: case I3G_GPU_CLASS_E: case I3G_GPU_CLASS_D: return HIGH_RISK;
		case I3G_GPU_CLASS_C: return LOW_RISK;
		}
	}

	return PASS;
}
