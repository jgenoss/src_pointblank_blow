#include "pch.h"

#include "UIOptionVideo.h"
#include "UIPopupOption.h"
#include "UIClanFunction.h"
#include "UIPopupOption.h"
#include "UIUtil.h"

I3_CLASS_INSTANCE( UIOptionVideo);

namespace
{
	UIOptionVideo* g_this = 0;
	UIPopupOption* g_Parent = 0;

	enum	{ RESOLUTION = 0, VIDEO_RESOLUTION, QUALITY, DETAIL, MODE, PARTS, CHARACTER, REFRESHRATE };
	enum { GAMMA = 0, FOV }; 

	//enum {GL, L, M, H, GH,MAX_LEVEL, CUSTOM = MAX_LEVEL };
	enum QUALITY_TYPE
	{
		QUALITY_LOWEST, 
		QUALITY_LOW, 
		QUALITY_MIDDLE, 
		QUALITY_HIGH, 
		QUALITY_HIGHEST,
		QUALITY_MAX_COUNT,

		QUALITY_CUSTOM = QUALITY_MAX_COUNT,
	};

	enum	
	{ 
		ANTI, 
		TEXTURE, 
		SHADOW, 
		SPECULAR, 
		EFFECT_QUALITY, 
		V_SYNC, 
		LINEAR_FILTER, 
		BULLET_TRACE, 
		DYNMIC_LIGHT, 
		NORMAL_MAP, 
		TERRAIN, 
		PHYSICAL,
		MAX_DETAIL,
	};

	INT32 g_detail_att[QUALITY_MAX_COUNT][MAX_DETAIL] = 
	{
		{ 0,0,0, 0,0,0, 0,0,0, 0,0,0, },	// QUALITY_LOWEST
		{ 0,1,0, 0,0,0, 1,1,0, 0,1,0, },	// QUALITY_LOW
		{ 1,2,1, 1,0,0, 1,1,0, 0,1,1, },	// QUALITY_MIDDLE
		{ 2,2,2, 1,1,0, 1,1,1, 1,1,1, },	// QUALITY_HIGH
		{ 4,3,3, 2,2,1, 1,1,1, 1,1,1, },	// QUALITY_HIGHEST
	};

	int* g_detail_value[MAX_DETAIL];
	
	MultiSceneInfo g_video_info1, g_video_info2;
	bool g_detail_scn = false;

	void enter_detail()
	{
		g_detail_scn = true;
		g_video_info2.scn->EnableScene(true);
		((UIOptionBase*)g_this)->apply_lua();
		g_video_info1.scn->EnableScene(false);
	}

	void exit_detail()
	{
		g_detail_scn = false;

		g_this->check_quick_setup_level();	//속성 값이 변경 되었는지 검사한다.

		g_video_info1.scn->EnableScene(true);
		((UIOptionBase*)g_this)->apply_lua();
		g_video_info2.scn->EnableScene(false);
	}

	void MsgRestart()
	{
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_OPTION_RESTART_MESSAGE"));
	}

	INT32 get_double_multiply(INT32 value)
	{
		return (1<<value);
	}
	INT32 get_double_divisor(INT32 value)
	{
		INT32 ret = 0;
		while(value > 1)
		{
			value >>= 1;
			++ret;
		}
		return ret;
	}
}

extern "C" 
{
	int option_video_click( LuaState * L)
	{
		INT32 value1 = i3Lua::GetIntegerArg( L, 1);
		INT32 value2 = i3Lua::GetIntegerArg( L, 2);
		g_this->clicked( value1, value2 );
		return 0;
	}
}

LuaFuncReg option_video_glue[] =
{
	{"Click",		option_video_click},
	{nullptr,				nullptr}
};

/************************//************************/
namespace
{
	INT32* g_lv = nullptr;
	INT32 g_rv = 0;
	bool is_restart = false;
	INT32 g_idx = 0;

	void cd_Risk(void * pThis, INT32 nParam)
	{
		switch( nParam )
		{
		case MBR_OK:	
			if( *g_lv != g_rv ) MsgRestart();
			*g_lv = g_rv;
			break;
		case MBR_USER:
			*g_lv = g_rv;
		case MBR_CANCEL:	
			{
				LuaState * L = UILua::CallLuaFunction(g_video_info2.scn, "SetRadio");
				i3Lua::PushInteger( L, g_idx);	
				i3Lua::PushInteger( L, *g_lv);	
				UILua::EndLuaFunction(L , 2);
			}
			break;
		}
	}

	void msg_ability_risk(UINT32 type, void* pThis)
	{
		i3::rc_wstring wstr_msg;
		if( type == VideoUtil::LOW_RISK ) wstr_msg = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_PERFORMANCE_SLOW_DOWN");
		else if( type == VideoUtil::HIGH_RISK ) wstr_msg = GAME_RCSTRING("STR_POPUP_Q_MESSAGE_PERFORMANCE_VERY_SLOW_DOWN");
		else cd_Risk(pThis, MBR_USER);

		if(!wstr_msg.empty())
		{
			GameUI::MsgBox( MSG_TYPE_GAME_QUERY, wstr_msg, nullptr, MAKE_CALLBACK(cd_Risk), pThis);
		}
	}
}

void UIOptionVideo::clicked(int idx1, int idx2)
{
	if( !g_detail_scn )
	{
		switch(idx1)
		{
		case RESOLUTION:
			{
				VID_INFO* info = m_Video.get_video_info( (UINT32)(idx2));
				if( info != nullptr)
				{
					m_nScreenWidth = info->m_Width;
					m_nScreenHeight = info->m_Height;
					adjust_refresh_rate();
				}
			}
			break;
		case VIDEO_RESOLUTION: m_nVideoResolution = idx2;	break;
		case REFRESHRATE: 
			{
				INT32 info = m_Video.get_Hz_info((UINT32)(idx2));
				m_nScreenHz = info;
			}
			break;
		//case QUALITY:	m_nQualityType = idx2; setupQuality();	apply_lua(); break;
		case QUALITY:	
			{
				// 퀄리티 타입값인 idx2와 apply_lua()를 setupQuality 함수에 넣어 처리.
				setupQuality(idx2); 
			}break;
		case DETAIL: enter_detail();		break;
		case MODE:	m_bFullScreen = idx2 != 0; apply_lua(); break;
		case PARTS:	m_bDisplayOtherParts = idx2 != 0; apply_lua(); break;
		case CHARACTER: m_bFixedViewCharacter = idx2; apply_lua(); break;
		}
	}
	else
	{	
		switch(idx1)
		{
		case ANTI: 
			{
				m_AA = (idx2) ? get_double_multiply(idx2-1) : 0;
				m_AAQ = 0; 
			}
			break;
		case TEXTURE: 
			{
				if( m_nTextureQualityType != idx2 )	MsgRestart();

				g_lv = &m_nTextureQualityType; g_idx = idx1; g_rv = idx2; is_restart = true;
				msg_ability_risk( VideoUtil::check_texture_ability(idx2), this );
				
			}
			break;
		case SHADOW:
			{
				g_lv = &m_nShadowQualityType; g_idx = idx1; g_rv = idx2; is_restart = false;
				msg_ability_risk( VideoUtil::check_shadow_ability(idx2), this );
			}
			break;
		case SPECULAR: 
			{
				if( m_nSpecularQualityType != idx2 )	MsgRestart();
				m_nSpecularQualityType = idx2; break;
			}
		case EFFECT_QUALITY:  
			{
				if( m_nEffectQuality != idx2 )	MsgRestart();
				m_nEffectQuality = idx2; break;
			}
		case V_SYNC: m_bVSync = idx2; break;
		case LINEAR_FILTER: m_bTriLinearFilter = idx2; break;
		case BULLET_TRACE: m_bEnableBulletTrace = idx2; break;
		case DYNMIC_LIGHT: 
			{
				g_lv = &m_bDynamicLight; g_idx = idx1; g_rv = idx2; is_restart = false;
				msg_ability_risk( VideoUtil::check_dynamiclight_ability(idx2 != 0, m_bEnableNormalMap != FALSE), this );
			}
			break;
		case NORMAL_MAP: 
			{
				g_lv = &m_bEnableNormalMap; g_idx = idx1; g_rv = idx2; is_restart = true;
				msg_ability_risk( VideoUtil::check_normalmap_ability(idx2 != 0), this );
			}
			break;
		case TERRAIN: m_bEnableTerrainEffect = idx2; break;
		case PHYSICAL: m_bEnablePhysX = idx2; break;
		}

		apply_lua();
	}
}



/************************//************************/

UIOptionVideo::UIOptionVideo() : UIOptionBase()
{
	g_this = this;

	mPEdit[GAMMA].SetRange(0, 100);
	m_Callback1.SetObj(this);
	mPEdit[GAMMA].SetCallback(&m_Callback1);

	mPEdit[FOV].SetRange((int)MIN_CAM_FOV, (int)MAX_CAM_FOV);
	m_Callback2.SetObj(this);
	mPEdit[FOV].SetCallback(&m_Callback2);

	g_detail_value[ANTI] = &m_AA;
	g_detail_value[TEXTURE] = &m_nTextureQualityType;
	g_detail_value[SHADOW] = &m_nShadowQualityType;
	g_detail_value[SPECULAR] = &m_nSpecularQualityType;
	g_detail_value[EFFECT_QUALITY] = &m_nEffectQuality;
	g_detail_value[V_SYNC] = &m_bVSync;
	g_detail_value[LINEAR_FILTER] = &m_bTriLinearFilter;
	g_detail_value[BULLET_TRACE] = &m_bEnableBulletTrace;
	g_detail_value[DYNMIC_LIGHT] = &m_bDynamicLight;
	g_detail_value[NORMAL_MAP] = &m_bEnableNormalMap;
	g_detail_value[TERRAIN] = &m_bEnableTerrainEffect;
	g_detail_value[PHYSICAL] = &m_bEnablePhysX;

	const VideoUtil::EnableContext& enable = VideoUtil::get_ability_context();

	//for(int i=0; i<MAX_LEVEL; ++i)
	for (int i=0; i<QUALITY_MAX_COUNT; ++i)
	{
		if(!enable.normal_map) g_detail_att[i][NORMAL_MAP] = 0;
		if(!enable.shadow_quality) g_detail_att[i][SHADOW] = 0;
		if( g_detail_att[i][SPECULAR] > enable.specular_quality )
			g_detail_att[i][SPECULAR] = enable.specular_quality;
	}
}

UIOptionVideo::~UIOptionVideo()
{
}


/************************//************************/
void UIOptionVideo::_InitializeAtLoad( i3UIScene * pScene )
{
	UIOptionBase::_InitializeAtLoad(pScene);

	bool isctr1 = i3::generic_is_iequal(pScene->getScriptFile(), i3::string("script\\PBRe_Pref_Video_Advanced.lua"));

	if( isctr1 == true )
		return;

	g_Parent = (UIPopupOption*)getParent();
	mPEdit[GAMMA].SetControl((UIPopupBase*)g_Parent, pScene, "i3UIProgressBar", "i3UIButton", "i3UIEditBox" );
	mPEdit[GAMMA].SetNumber((int)g_pEnvSet->m_fGammaVal);
	mPEdit[FOV].SetControl((UIPopupBase*)g_Parent, pScene, "i3UIProgressBar0", "i3UIButton0", "i3UIEditBox0" );
}

/*virtual*/ void UIOptionVideo::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent );

	g_video_info1.idx = GetSceneCount();

	AddScene("Scene/Option/PBRe_Pref_Video.i3UIs", option_video_glue);

	g_video_info2.idx = GetSceneCount();
	AddScene("Scene/Option/PBRe_Pref_Video_Advanced.i3UIs", option_video_glue);

}

void UIOptionVideo::OnLoadAllScenes()
{
	g_video_info1.scn= GetScene( g_video_info1.idx );
	g_video_info2.scn= GetScene( g_video_info2.idx );

	g_detail_scn = false;
	g_video_info1.scn->EnableScene(true);
	g_video_info2.scn->EnableScene(false);
}

bool UIOptionVideo::OnKey_Enter()
{
	if(isVisible() == FALSE) return 0;

	for(int i=0; i<MAX_PE;++i)
		mPEdit[i].OnKey_Enter();

	return 0;
}

bool UIOptionVideo::OnKey_Escape()
{
	if(isVisible() == FALSE) return 0;

	for(int i=0; i<MAX_PE;++i)
		mPEdit[i].OnKey_Escape();
	
	return 0;
}

void UIOptionVideo::OnUpdate( REAL32 rDeltaSeconds)
{
	if(g_detail_scn) return;
	if(isVisible() == FALSE) return;

	for(int i=0; i<MAX_PE;++i)
		mPEdit[i].OnUpdate(rDeltaSeconds);
}


/************************//************************/
void UIOptionVideo::cb_edit_change1( INT32 value )
{
	if (value < 0) value = 0;
	else if (value > 100) value = 100;

	m_fGammaVal = (REAL32)value;

	VideoUtil::ApplyGammaState(m_fGammaVal);
}

void UIOptionVideo::cb_edit_change2( INT32 value )
{
	m_fFovValue = (REAL32)value;
}

/************************//************************/
void UIOptionVideo::load_value()
{
	m_nScreenWidth = g_pEnvSet->m_nScreenWidth;
	m_nScreenHeight = g_pEnvSet->m_nScreenHeight;

	m_nVideoResolution = g_pEnvSet->m_nVideoResolution;

	m_bFullScreen =	g_pEnvSet->m_bFullScreen;
	m_fFovValue = g_pEnvSet->m_fFovValue;
	m_fGammaVal =	g_pEnvSet->m_fGammaVal;

	load_detail_value();

	m_nScreenHz = g_pEnvSet->m_nScreenHz;
	m_ScreenDepth = g_pEnvSet->m_ScreenDepth;
	m_b16BitsDepth = g_pEnvSet->m_b16BitsDepth;

	m_bDisplayOtherParts = g_pEnvSet->m_bDisplayOtherParts;
	m_bFixedViewCharacter = g_pEnvSet->m_bFixedViewCharacter;
}

void UIOptionVideo::load_detail_value()
{
	m_AA = g_pEnvSet->m_AA;
	m_AAQ = g_pEnvSet->m_AAQ;
	m_nTextureQualityType = (3 - g_pEnvSet->m_nTextureQualityType);
	m_nShadowQualityType = g_pEnvSet->m_nShadowQualityType;
	m_nSpecularQualityType = g_pEnvSet->m_nSpecularQualityType;
	m_nEffectQuality = g_pEnvSet->m_nEffectQuality;

	m_bVSync = g_pEnvSet->m_bVSync;
	m_bTriLinearFilter = g_pEnvSet->m_bTriLinearFilter;
	m_bEnableBulletTrace = g_pEnvSet->m_bEnableBulletTrace;
	m_bDynamicLight = g_pEnvSet->m_bDynamicLight;
	m_bEnableNormalMap = g_pEnvSet->m_bEnableNormalMap;
	m_bEnableTerrainEffect = g_pEnvSet->m_bEnableTerrainEffect;
	m_bEnablePhysX = g_pEnvSet->m_bEnablePhysX;

	adjust_max_value();
	check_quick_setup_level();	//속성 값이 변경 되었는지 검사한다.
}

void UIOptionVideo::save_value()
{
	g_pEnvSet->m_nScreenWidth = m_nScreenWidth;
	g_pEnvSet->m_nScreenHeight = m_nScreenHeight;
	g_pEnvSet->m_nVideoResolution = m_nVideoResolution;
	g_pEnvSet->m_bFullScreen = m_bFullScreen != FALSE;
	g_pEnvSet->m_fFovValue = m_fFovValue;
	g_pEnvSet->m_fGammaVal = m_fGammaVal;
	g_pEnvSet->m_nScreenHz = m_nScreenHz;

	g_pEnvSet->m_AA = m_AA;
	g_pEnvSet->m_AAQ = m_AAQ;
	g_pEnvSet->m_nTextureQualityType = (3-m_nTextureQualityType);
	g_pEnvSet->m_nShadowQualityType = m_nShadowQualityType;
	g_pEnvSet->m_nSpecularQualityType = m_nSpecularQualityType;
	g_pEnvSet->m_nEffectQuality = m_nEffectQuality;

	g_pEnvSet->m_bVSync = m_bVSync != FALSE;
	g_pEnvSet->m_bTriLinearFilter = m_bTriLinearFilter != FALSE;
	g_pEnvSet->m_bEnableBulletTrace = m_bEnableBulletTrace != FALSE;
	g_pEnvSet->m_bEnableBulletSmoke = m_bEnableBulletTrace != FALSE;
	g_pEnvSet->m_bDynamicLight = m_bDynamicLight != FALSE;
	g_pEnvSet->m_bEnableNormalMap = m_bEnableNormalMap != FALSE;
	g_pEnvSet->m_bEnableTerrainEffect = m_bEnableTerrainEffect != FALSE;
	g_pEnvSet->m_bEnablePhysX = m_bEnablePhysX != FALSE;

	g_pEnvSet->m_bDisplayOtherParts = m_bDisplayOtherParts != FALSE;
	g_pEnvSet->m_bFixedViewCharacter = m_bFixedViewCharacter != FALSE;
}

void UIOptionVideo::default_value()
{
	m_nScreenWidth	= 1024;
	m_nScreenHeight = 768;
	m_nScreenHz		= 60;
	m_nVideoResolution = 1;

#if !defined(USE_STEAM)
	m_bFullScreen = TRUE;
#else
	m_bFullScreen = FALSE;
#endif 
	m_fGammaVal = 50.f;
	m_fFovValue = DEF_CAM_FOV;

	UINT32 cpuCount = i3CPUGetCount();
	if( cpuCount > 1)	// multicore에서는 상대방 보기를 on
	{
		if( cpuCount > 3)
		{
			m_bDisplayOtherParts = TRUE;
		}
		else
		{
			m_bDisplayOtherParts = FALSE;
		}

		m_bFixedViewCharacter = FALSE;
	}
	else
	{
		m_bDisplayOtherParts = FALSE;
		m_bFixedViewCharacter = TRUE;
	}

	default_detail_value();
}

void UIOptionVideo::default_detail_value()
{
#if defined(LOCALE_JAPAN)
	setupQuality(QUALITY_HIGHEST);
#else
	m_AA = 0;
	m_AAQ = 0;

	m_nTextureQualityType = 0;
	m_nShadowQualityType = 0;
	m_nSpecularQualityType = 0;
	m_nEffectQuality = 2;

	m_bVSync = FALSE;
	m_bTriLinearFilter = FALSE;
	m_bEnableBulletTrace = TRUE;
	m_bEnableBulletSmoke = TRUE;
	m_bDynamicLight = FALSE;
	m_bEnableNormalMap = TRUE;
	m_bEnableTerrainEffect = TRUE;
	m_bEnablePhysX = TRUE;

	adjust_max_value();
#endif // LOCALE_JAPAN
}

void UIOptionVideo::apply_game()
{
	if( ((UIPopupOption*)getParent())->m_StartGame == true )	//로그인 후에 설정값을 게임에 적용하는데.. 그림쪽은 적용 안 한다.
		return;
	
#ifndef USE_GAMMAOPTION_BY_SHADER
	bool bForceApplyGammaRamp = false;
#endif

	if( is_modified_screen() )
	{
		VideoUtil::VideoContext cxt;

		cxt.m_AA = m_AA;
		cxt.m_AAQ = m_AAQ;
		cxt.m_b16BitsDepth = m_b16BitsDepth != FALSE;
		cxt.m_bFullScreen = m_bFullScreen != FALSE;
		cxt.m_bTriLinearFilter = m_bTriLinearFilter != FALSE;
		cxt.m_bVSync = m_bVSync != FALSE;
		cxt.m_nScreenHeight = m_nScreenHeight;
		cxt.m_nScreenHz = m_nScreenHz;
		cxt.m_nScreenWidth = m_nScreenWidth;
		cxt.m_ScreenDepth = m_ScreenDepth;
		
#ifndef USE_GAMMAOPTION_BY_SHADER
		i3Viewer * pViewer = g_pFramework->GetViewer();
		i3VideoInfo* pVideo = pViewer->GetRenderContext()->GetVideoInfo();

		if (pVideo->m_bFullScreen == false && m_bFullScreen != FALSE)		// 창모드에서 전체모드일때는 강제로 감마 셋팅을 해야할 것이다.
		{
			bForceApplyGammaRamp = true;
		}
#endif

		VideoUtil::ApplyVideo(cxt);

		if( UIPopupOption::instance()->GetCurrentOption() == this )
			apply_lua();
	}

	//화면 밝기
#ifdef USE_GAMMAOPTION_BY_SHADER
	if (abs(m_fGammaVal - g_pEnvSet->m_fGammaVal) > I3_EPS)
#else
	if (bForceApplyGammaRamp || m_fGammaVal != g_pEnvSet->m_fGammaVal)
#endif
		VideoUtil::ApplyGammaState(m_fGammaVal);
	
	//capture 해상도
	if( m_nVideoResolution != g_pEnvSet->m_nVideoResolution )
		VideoUtil::ApplyVideoCaptureResolution(m_nVideoResolution);

	//빛반사 효과
	if( m_nSpecularQualityType != g_pEnvSet->m_nSpecularQualityType )
		VideoUtil::ApplySpecularQuality(m_nSpecularQualityType);
	
	//그림자 효과
	if( m_nShadowQualityType != g_pEnvSet->m_nShadowQualityType )
	{
		//if( ((UIPopupOption*)getParent())->m_InGame )
			VideoUtil::ApplyShadowQuality( m_nShadowQualityType);
		/*else
			VideoUtil::ApplyShadowQuality(0);*/
	}

	if( (m_bEnableNormalMap ? true : false) != g_pEnvSet->m_bEnableNormalMap)
	{
		VideoUtil::ApplyNormalMap( m_bEnableNormalMap?true:false);
	}
}

void UIOptionVideo::apply_lua()
{
	LuaState * L = 0;

	if(!g_detail_scn)
	{
		//윈도우 모드 변경 유무 (윈도우 모드를 못 하는 국가가 있다.)
		L = UILua::CallLuaFunction(g_video_info1.scn, "SetScrMode");
		bool enable = TRUE;
#if defined( WINDOW_MODE_DISABLE)
		enable = FALSE;
#endif
		i3Lua::PushBoolean( L, enable);
		UILua::EndLuaFunction(L , 1);

#ifndef USE_GAMMAOPTION_BY_SHADER
		mPEdit[GAMMA].Enable(g_pEnvSet->m_bFullScreen);	//윈도우 모드에서는 사용할 수 없다.

		if (g_pEnvSet->m_bFullScreen == FALSE)
		{
			mPEdit[GAMMA].SetNumber(50);				// 창모드에서는 그냥 중앙에 놓도록 한다..(입력받아서도 안되지만, 설정상 중앙)
		}
		else
#endif
		{
			if (mPEdit[GAMMA].GetNumber() != (int)m_fGammaVal)
				mPEdit[GAMMA].SetNumber((int)m_fGammaVal);
		}
		if(mPEdit[FOV].GetNumber() != (int)m_fFovValue)
			mPEdit[FOV].SetNumber((int)m_fFovValue);

		L = UILua::CallLuaFunction(g_video_info1.scn, "SetValue");
		i3Lua::PushInteger( L, m_nQualityType);
		i3Lua::PushInteger( L, m_bFullScreen);
		i3Lua::PushInteger( L, m_bDisplayOtherParts);
		i3Lua::PushInteger( L, m_bFixedViewCharacter);
		UILua::EndLuaFunction(L , 4);

		//해상도
		m_Video.init_video_info(m_ScreenDepth, !m_bFullScreen);
		i3::wstring resolution = m_Video.get_video_info_string();
		int res_idx = m_Video.find_video_info_idx(m_nScreenWidth, m_nScreenHeight);
		if(res_idx == -1)
			res_idx = m_Video.find_max_video_info_idx(&m_nScreenWidth, &m_nScreenHeight);

		L = UILua::CallLuaFunction(g_video_info1.scn, "SetFilterList1");
		i3Lua::PushStringUTF16To8(L, resolution.c_str(), resolution.size() );
		i3Lua::PushInteger(L, res_idx);
		UILua::EndLuaFunction(L , 2);

		//동영상 해상도
		ucf::ComboString cs; 
		
		cs<<GAME_RCSTRING("STR_POPUP_VIDEO_QUALITY_LOW")
			<<GAME_RCSTRING("STR_POPUP_VIDEO_QUALITY_DEFAULT")
			<<GAME_RCSTRING("STR_POPUP_VIDEO_QUALITY_HIGH");

		L = UILua::CallLuaFunction(g_video_info1.scn, "SetFilterList2");
		const i3::wstring& cs_wstr = cs;

		i3Lua::PushStringUTF16To8(L, cs_wstr.c_str(), cs_wstr.size());

                    		i3Lua::PushInteger(L, m_nVideoResolution);
		UILua::EndLuaFunction(L , 2);

		//주사율
		m_Video.init_Hz_Info(m_ScreenDepth, m_nScreenWidth, m_nScreenHeight);
		i3::wstring Hz = m_Video.get_Hz_info_string();
		int hz_idx = m_Video.find_Hz_info_idx(m_nScreenHz);
		if (hz_idx == -1)	hz_idx = 0;

		L = UILua::CallLuaFunction(g_video_info1.scn, "SetRefreshRate");
		i3Lua::PushStringUTF16To8(L, Hz.c_str(), Hz.size());
		i3Lua::PushInteger(L, hz_idx);
		UILua::EndLuaFunction(L, 2);

		L = UILua::CallLuaFunction(g_video_info1.scn, "EnableParts");
		i3Lua::PushBoolean(L, (g_pEnvSet->m_nUIVersion == UFV_1_5));
		UILua::EndLuaFunction(L, 1);

	}
	else
	{
		// 인게임 캐시충전 팝업 기능을 사용할 경우 인게임 웹페이지 때문에 안티얼라이싱이 적용이 안됩니다. (강제로 적용할 경우 웹페이지 안나옵니다)
		// 안티얼라이싱을 사용하기 위해서는 인게임 내부에 웹페이지를 띄우는 방식이 아니라 별도의 인터넷 브라우저를 띄우는 방식으로 변경해야 합니다.
		L = UILua::CallLuaFunction(g_video_info2.scn, "EnableAnti");
		if( !g_pConfigEx->GetShop().ChargeCash_InGame )
		{
			//안티앨리어싱 조사.
			m_Video.init_AA_Info(m_ScreenDepth, !m_bFullScreen, false);
			i3Lua::PushBoolean( L, true);
			i3Lua::PushInteger( L, m_Video.size_AA_Info(false) );	//none, *8 을 제외한 size를 얻는다.
		}
		else
		{
			i3Lua::PushBoolean( L, false);
			i3Lua::PushInteger( L, -1);
		}
		UILua::EndLuaFunction(L , 2);

		const VideoUtil::EnableContext& enable = VideoUtil::get_ability_context();
		L = UILua::CallLuaFunction(g_video_info2.scn, "SeMax");
		i3Lua::PushInteger( L, (enable.shadow_quality) ? 4 : 1 );	
		i3Lua::PushInteger( L, enable.specular_quality+1 );
		i3Lua::PushInteger( L, enable.normal_map +1 );	
		UILua::EndLuaFunction(L , 3);

		L = UILua::CallLuaFunction(g_video_info2.scn, "SetValue");
		i3Lua::PushInteger( L, (m_AA) ? get_double_divisor(m_AA)+1 : 0 );	
		i3Lua::PushInteger( L, m_nTextureQualityType);
		i3Lua::PushInteger( L, m_nShadowQualityType);

		i3Lua::PushInteger( L, m_nSpecularQualityType);
		i3Lua::PushInteger( L, m_nEffectQuality);
		i3Lua::PushInteger( L, m_bVSync);

		i3Lua::PushInteger( L, m_bTriLinearFilter);
		i3Lua::PushInteger( L, m_bEnableBulletTrace);
		i3Lua::PushInteger( L, m_bDynamicLight);

		i3Lua::PushInteger( L, m_bEnableNormalMap);
		i3Lua::PushInteger( L, m_bEnableTerrainEffect);
		i3Lua::PushInteger( L, m_bEnablePhysX);

		UILua::EndLuaFunction(L , 12);
	}

	if(g_Parent != nullptr)
	{
		// CancelBtn String 변경
		L = UILua::CallLuaFunction(g_Parent->GetScene(0), "SetDetailCancelBtnString");
		i3Lua::PushBoolean( L, g_detail_scn);
		UILua::EndLuaFunction(L , 1);
	}
}

bool UIOptionVideo::is_modified()
{
	if( is_modified_screen() == true ) return true;

	if( fabs(m_fFovValue - g_pEnvSet->m_fFovValue) > std::numeric_limits< REAL32 >::epsilon() )		return true;
	if( fabs(m_fGammaVal - g_pEnvSet->m_fGammaVal) > std::numeric_limits< REAL32 >::epsilon() )		return true;
	if( m_nVideoResolution != g_pEnvSet->m_nVideoResolution )		return true;

	if( (3-m_nTextureQualityType) != g_pEnvSet->m_nTextureQualityType )		return true;
	if( m_nSpecularQualityType != g_pEnvSet->m_nSpecularQualityType )		return true;
	if( m_nEffectQuality != g_pEnvSet->m_nEffectQuality )		return true;
	if( m_bEnableNormalMap != (g_pEnvSet->m_bEnableNormalMap ? TRUE : FALSE))		return true;
	if( m_nShadowQualityType != g_pEnvSet->m_nShadowQualityType )		return true;
	if( m_bDynamicLight != (g_pEnvSet->m_bDynamicLight ? TRUE : FALSE))		return true;
	
	if( m_bEnableBulletTrace != (g_pEnvSet->m_bEnableBulletTrace ? TRUE : FALSE))		return true;
	if( m_bEnableTerrainEffect != (g_pEnvSet->m_bEnableTerrainEffect ? TRUE : FALSE))		return true;
	if( m_bEnablePhysX != (g_pEnvSet->m_bEnablePhysX ? TRUE : FALSE))		return true;

	if( m_bDisplayOtherParts != (g_pEnvSet->m_bDisplayOtherParts ? TRUE : FALSE))		return true;
	if( m_bFixedViewCharacter != (g_pEnvSet->m_bFixedViewCharacter ? TRUE : FALSE))		return true;

	return false;
}

bool UIOptionVideo::is_modified_screen() const
{
	if( m_nScreenWidth != g_pEnvSet->m_nScreenWidth )		return true;
	if (m_nScreenHeight != g_pEnvSet->m_nScreenHeight)		return true;
	if (m_nScreenHz != g_pEnvSet->m_nScreenHz)				return true;
	if( m_bFullScreen != (g_pEnvSet->m_bFullScreen ? TRUE : FALSE) )			return true;
	if( m_AA != g_pEnvSet->m_AA )		return true;
	if( m_AAQ != g_pEnvSet->m_AAQ )		return true;
	if( m_bVSync != (g_pEnvSet->m_bVSync ? TRUE : FALSE) )		return true;
	if (m_bTriLinearFilter != (g_pEnvSet->m_bTriLinearFilter ? TRUE : FALSE) )		return true;

	return false;
}

bool UIOptionVideo::is_diff_default_RefreshRate() const
{
	return (m_nScreenHz != i3RenderContext::GetCaps()->getDefaultRefreshRate());
}

/************************//************************/
bool UIOptionVideo::key_escape()
{
	if(!g_detail_scn) return false;

	exit_detail();
	return true;
}

bool UIOptionVideo::clicked_default()
{
	if(!g_detail_scn) return false;	//detail 옵션이 활성화 상태가 아니면 false로 돌린다.

	default_detail_value();
	apply_lua();

	return true;
}


bool UIOptionVideo::clicked_ok()
{
	if(!g_detail_scn) return false;

	exit_detail();
	return true;
}

bool UIOptionVideo::clicked_cancel()
{
	// 어쨌든 감마옵션은 다시 적용해야 함
	VideoUtil::ApplyGammaState(g_pEnvSet->m_fGammaVal);

	if(!g_detail_scn) return false;

	load_detail_value();

	exit_detail();
	return true;
}

/************************//************************/
//void UIOptionVideo::set_quick_setup()
void UIOptionVideo::setupQuality(const INT32 type)
{
	m_nQualityType = type;

	if (QUALITY_LOWEST <= m_nQualityType && m_nQualityType < QUALITY_MAX_COUNT)	// m_nQualityType이 QUALITY_CUSTOM 일 경우 g_detail_att 배열 범위를 넘어서므로 예외처리. hansoft.11593
	{
		for(int i=0; i<MAX_DETAIL; ++i)
			*g_detail_value[i] = g_detail_att[m_nQualityType][i];

		m_bEnableBulletSmoke = m_bEnableBulletTrace;		//동일한 내용을 같는다.

		adjust_max_value();

		apply_lua();
	}
	else
	{	
		// QUALITY_CUSTOM 일 경우 고급설정 팝업을 띄움. 기획의도.
		enter_detail();
	}
}

void UIOptionVideo::check_quick_setup_level()
{
	for(int lv = 0; lv < QUALITY_MAX_COUNT; ++lv)
	{
		bool matched = true;
		for(int dt =0; dt<MAX_DETAIL; ++dt)
		{
			if( *g_detail_value[dt] != g_detail_att[lv][dt] )
			{
				matched = false;
				break;
			}
		}
		if(matched)
		{
			m_nQualityType = lv;
			return;
		}
	}

	m_nQualityType = QUALITY_CUSTOM;
}

void UIOptionVideo::adjust_max_value()
{
	//사용 가능한지 본다. 그리고 값을 변경 한다.
	const VideoUtil::EnableContext& enable = VideoUtil::get_ability_context();

	if(!enable.normal_map) m_bEnableNormalMap = FALSE;

	while( enable.specular_quality < m_nSpecularQualityType )
		--m_nSpecularQualityType;

	if(!enable.shadow_quality) m_nShadowQualityType = 0;
}

void UIOptionVideo::adjust_refresh_rate()
{
	//주사율
	m_Video.init_Hz_Info(m_ScreenDepth, m_nScreenWidth, m_nScreenHeight);
	i3::wstring Hz = m_Video.get_Hz_info_string();
	int hz_idx = m_Video.find_Hz_info_idx(m_nScreenHz);
	if (hz_idx == -1)	hz_idx = 0;

	LuaState * L = 0;
	L = UILua::CallLuaFunction(g_video_info1.scn, "SetRefreshRate");
	i3Lua::PushStringUTF16To8(L, Hz.c_str(), Hz.size());
	i3Lua::PushInteger(L, hz_idx);
	UILua::EndLuaFunction(L, 2);
}
