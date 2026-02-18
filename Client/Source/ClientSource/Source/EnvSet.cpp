#include "pch.h"
#include "GlobalVariables.h"
#include "EnvSet.h"
#include "EnvInfoContext.h"

#include "UserInfoContext.h"

#include "i3Base/string/ext/generic_string_cat.h"

CEnvSet * g_pEnvSet = nullptr;

I3_CLASS_INSTANCE(CEnvSet);

namespace
{
	LANGID GetLocaleLangID(void)
	{
#if defined(LOCALE_KOREA)
		return MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT);
#elif defined(LOCALE_THAILAND)
		return MAKELANGID(LANG_THAI, SUBLANG_DEFAULT);
#elif defined(LOCALE_INDONESIA)
		return MAKELANGID(LANG_INDONESIAN, SUBLANG_DEFAULT);
#elif defined(LOCALE_RUSSIA)		//	러시아 (동유럽어)
		return MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
#elif defined(LOCALE_TURKEY)
		return MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT);
#elif defined(LOCALE_BRAZIL)
		return MAKELANGID(LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN);
#elif defined(LOCALE_LATIN_AMERICA)
		return MAKELANGID(LANG_SPANISH, SUBLANG_SPANISH_CHILE);
#elif defined( LOCALE_NORTHAMERICA)
		return MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
#elif defined( LOCALE_ITALY)
		return MAKELANGID(LANG_ITALIAN, SUBLANG_DEFAULT);
#elif defined( LOCALE_ENGLISH)
		return MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
#elif defined( LOCALE_MIDDLE_EAST )
		return MAKELANGID(LANG_ARABIC, SUBLANG_DEFAULT);
#elif defined( LOCALE_PHILIPPINES )
		return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_PHILIPPINES);
#elif defined( LOCALE_TAIWAN )
		return MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL);
#elif defined( LOCALE_SINGMAL )
		return MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);
#elif defined( LOCALE_VIETNAM )
		return MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT);
#elif defined( LOCALE_JAPAN )
		return MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
#else
		return MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT);		// 영어
#endif
	}
}

CEnvSet::CEnvSet(void)
{
	g_pEnvSet = this;

	m_EnvFileName = ENVFILENAME;

	m_pEnvReg = nullptr;
	m_nUIVersion = UFV_NON;
	m_nDefaultNation = -1;
	// ----- Screen Config -----
	m_bFullScreen = true;
	m_bTriLinearFilter = false;
	m_ScreenDepth = 0;
	m_AA = 0;
	m_AAQ = 0;
	m_b16BitsDepth = false;
	m_bVSync = false;
	m_nScreenWidth = 0;
	m_nScreenHeight = 0;
	m_nScreenHz = 0;
	m_nVideoResolution = 1;


	m_fGammaVal = 0.0f;
	m_nSpeakerType = 0;								// 스피커 설정

													// ----- Perfomance -----
	m_nShadowQualityType = 0;						// 그림자 효과
	m_bEnableBulletTrace = true;						// 총알궤적
	m_bEnableBulletSmoke = true;						// 총알궤적(연기)

	m_bDynamicLight = false;
	m_bEnableNormalMap = true;								// Normal Map
	m_bEnableTerrainEffect = true;							// 지형 이펙트
	m_nTextureQualityType = 0;								// 텍스쳐 퀄러티
	m_nEffectLevel = 1;
	m_nSpecularQualityType = 1;
	m_bEnableBloom = false;
	m_bEnablePhysX = true;
	m_nEffectQuality = 2;

	// ----- MultiLanguage -----
	m_nLanguage = 0;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 네트워크로 보내어 저장되어지는것
	/////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Game Play -----
	m_bEnableBlood = true;								// 혈흔 표시
	m_bCrossHairEffect = true;								// 크로스헤어 타격 이펙트

	m_nCrossHairType = 0;								// 크로스헤어 종류
	m_nHandType = 0;								// 무기 사용하는 손 0:오른손	1:왼손

													// ----- Interface -----
	m_bEnableFrindlyName = true;								// 아군 닉네임 표시
	m_bEnableFrindlyMark = false;								// 아군 마크 표시
	m_bTextEffect = false;									// 텍스트 이펙트 on/off
	m_bMiniMapMode = false;									// 미니맵 표시방식( 미니맵(on)/레이다(off))
	m_bChallange = true;									// 도저과제 on/off
	m_b1VS1 = true;											// 일대일 전적

															// ----- Sound -----
	m_bEnableBGM = false;
	m_bEnableEffect = false;
	m_bEnableRadioMessage = false;
	m_nRadioMessageType = 0;									// 0:영문, 1:한글
	m_nRadioMode = 0;										// Radio chat mode (default:0 한글, 1영문)
	m_fVolBGM = 0.0f;
	m_fVolEffect = 0.0f;

	// ----- Control -----
	i3mem::FillZero(m_BaseKeyMapArray, (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT));
	i3mem::FillZero(m_KeyMapArray, (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT));
	m_bEnableMouseInvert = false;							// 마우스 반전
	m_rMouseSensivity = 0.0f;								// 마우스 민감도
	m_fFovValue = 0.0f;									// 시야각

														// ----- Community -----
	m_nChatType = 0;										// 채팅
	m_nWhisperType = 0;										// 귓속말
	m_nInviteType = COMMUNITY_PERMISSION_ALL;					// 초대 메시지
	m_nFriendType = 0;										// 친구 요청 메시지
	m_nMessageType = 0;										// 쪽지 수신
	m_nClanType = 0;										// 클랜 가입 권유



	m_bWrongEnvSet = false;

	i3mem::FillZero(m_bMacroEnableFriend, sizeof(bool) * 5);

	m_bLoadDefault = false;

	LoadDefault();
	InitBaseGameKeyMap();
	InitGameKeyMap();
	Load();

	// DirectX가 생성될 때, 한번 더 Default Option을 조정할 기회를 얻기 위해...
#if defined( LOCALE_RUSSIA) || defined( LOCALE_THAILAND) || defined( LOCALE_KOREA)
	i3RenderContext::setConfigProc(AdjustDefault);
#endif

	m_bEnableLaserSight = true;
}

CEnvSet::~CEnvSet(void)
{
	I3_SAFE_RELEASE(m_pEnvReg);
}



void CEnvSet::LoadDefault()
{
	INT32 i = 0;

	m_bCrossHairEffect = false;								// 크로스헤어 타격 이펙트
	m_nCrossHairType = 0;
	m_nHandType = 0;

	m_nDefaultNation = -1;
	m_nUIVersion = UFV_NON;
	m_bFullScreen = true;
	m_nScreenWidth = 1024;
	m_nScreenHeight = 768;
	m_nScreenHz = 60;

	m_ScreenDepth = 32;
	m_fGammaVal = 50.0f;
	m_AA = 0;
	m_AAQ = 0;
	m_bVSync = false;
	m_bTriLinearFilter = false;

	m_nVideoResolution = 1;

	m_bEnableBGM = true;
	m_bEnableEffect = true;
	m_bEnableRadioMessage = true;
	m_nRadioMessageType = 0;

	m_fVolBGM = 60.0f;
	m_fVolEffect = 100.0f;
	m_nSpeakerType = 0;

	m_bEnableBulletTrace = true;
	m_bEnableBulletSmoke = true;
	m_nShadowQualityType = 0;
	m_bDynamicLight = false;
	m_bEnableNormalMap = false;
	m_bEnableTerrainEffect = true;
	m_nTextureQualityType = 1;
	m_b16BitsDepth = false;
	m_nSpecularQualityType = 1;
	m_bEnableBloom = false;
	m_nEffectQuality = 2;

	// CPU Core의 개수가 1개인 경우 Default로 PhysX를 Off한다.
	{
		UINT32 cpuCount = i3CPUGetCount();

		if (cpuCount > 1)
		{
			m_bEnablePhysX = true;
			if (cpuCount > 3)
				m_bDisplayOtherParts = true;
			else
				m_bDisplayOtherParts = false;
			m_bFixedViewCharacter = false;
		}
		else
		{
			INT32 flops = i3System::Benchmark_FLOPS();

			if (flops >= 255)
			{
				m_bEnablePhysX = true;
			}
			else
			{
				m_bEnablePhysX = false;
			}

			m_bDisplayOtherParts = false;
			m_bFixedViewCharacter = true;
		}
	}

	m_bEnableMouseInvert = false;
	m_rMouseSensivity = 0.5f;
	m_fFovValue = DEF_CAM_FOV;
	m_bEnableFrindlyName = true;
	m_bEnableFrindlyMark = true;
	m_bEnableBlood = true;
	m_nChatType = 0;
	m_nWhisperType = 0;
	m_nInviteType = COMMUNITY_PERMISSION_ALL;
	m_nFriendType = 0;
	m_nMessageType = 0;
	m_nClanType = 0;
	m_bTextEffect = true;
	m_bMiniMapMode = false;
	m_bChallange = true;
	m_b1VS1 = true;

	m_bWrongEnvSet = false;

	for (i = 0; i < MACRO_MAX_COUNT; i++)
	{
		m_wstrMacro[i].clear();
		m_bMacroEnableFriend[i] = true;
	}

	m_nRadioMode = 0;

	m_nUserKey1 = 0;
	m_nUserKey2 = 0;

	m_nLanguage = GetLanguageType();
	m_nUIVersion = GetUIVersionType();

#if defined (LOCALE_INDONESIA) || defined (LOCALE_TURKEY) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_PHILIPPINES  ) || defined( LOCALE_MIDDLE_EAST)
	m_bCrossHairEffect = false;								// 크로스헤어 타격 이펙트
	m_bDynamicLight = true;
	m_nTextureQualityType = 2;
	m_nEffectQuality = 1;
#endif

	m_nGameInterface = 1;
	m_bTeamBand = true;

	////////////////////////////////////////////////////////////


#if defined(LOCALE_KOREA)
	// 한국설정 오버라이드, 전체적으로 그래픽옵션 상향
	_LoadDefaultForKorea(*this);
#elif defined(LOCALE_JAPAN)
	// 일본 기본 설정
	_LoadDefaultForJapan(*this);
#elif defined(LOCALE_RUSSIA)
	//러시아 기본 설정
	_LoadDefaultForRussia(*this);
#endif

}

void CEnvSet::AdjustDefault(i3VideoInfo * pVideoInfo)
{
	I3G_GPU_CLASS GPUClass;

	if (g_pEnvSet->m_bLoadDefault == false)
		return;

	GPUClass = i3GfxCaps::DetermineGPUClass();

	// GPU의 성능에 맞추어 기본 환경 설정치를 바꿔 줍니다.
	switch (GPUClass)
	{
	case I3G_GPU_CLASS_F:
		g_pEnvSet->m_bDynamicLight = false;
		g_pEnvSet->m_nTextureQualityType = 3;

		break;

	case I3G_GPU_CLASS_E:
		g_pEnvSet->m_bDynamicLight = false;
		g_pEnvSet->m_nTextureQualityType = 2;
		break;

	case I3G_GPU_CLASS_D:
		g_pEnvSet->m_bDynamicLight = true;
		g_pEnvSet->m_nTextureQualityType = 1;
		break;

	case I3G_GPU_CLASS_C:
		g_pEnvSet->m_bDynamicLight = true;
		g_pEnvSet->m_nTextureQualityType = 1;
		g_pEnvSet->m_bTriLinearFilter = true;
		break;

	case I3G_GPU_CLASS_B:
		g_pEnvSet->m_bDynamicLight = true;
		g_pEnvSet->m_bEnableNormalMap = true;
		g_pEnvSet->m_nSpecularQualityType = 1;
		g_pEnvSet->m_nTextureQualityType = 0;
		g_pEnvSet->m_nShadowQualityType = 2;
		g_pEnvSet->m_bTriLinearFilter = true;
		break;

	case I3G_GPU_CLASS_A:
		g_pEnvSet->m_bDynamicLight = true;
		g_pEnvSet->m_bEnableNormalMap = true;
		g_pEnvSet->m_nSpecularQualityType = 2;
		g_pEnvSet->m_nTextureQualityType = 0;
		g_pEnvSet->m_nShadowQualityType = 2;
		g_pEnvSet->m_bTriLinearFilter = true;
		break;
	}

	{
		INT32 modeCount = i3RenderContext::getDisplayModeCount(I3G_IMAGE_FORMAT_BGRX_8888);

		I3G_DISPLAY_MODE_INFO info;
		INT32 i;

		INT32 maxW = ::GetSystemMetrics(SM_CXSCREEN);		// 1024
		INT32 maxH = ::GetSystemMetrics(SM_CYSCREEN);		// 768

		REAL32 RealAspactRatio = ((REAL32)maxW / maxH);

		g_pEnvSet->m_nScreenWidth = maxW;
		g_pEnvSet->m_nScreenHeight = maxH;

		bool bFind = false;

		for (i = 0; i < modeCount; i++)
		{
			i3RenderContext::getDisplayMode(i, I3G_IMAGE_FORMAT_BGRX_8888, &info);

			REAL32 rate = (REAL32)info.m_Width / info.m_Height;

			if (i3Math::abs(RealAspactRatio - rate) > 0.1f)			continue;		// 비율이 같지 않다면...
			if ((info.m_Width < 1024) || (info.m_Height < 768))		continue;		// 최소 해상도 Filtering
//			if (info.m_RefreshRate <= 40.0f)						continue;		// 40hz 이하 삭제. 

			if ((info.m_Width <= g_pEnvSet->m_nScreenWidth) && (info.m_Height <= g_pEnvSet->m_nScreenHeight))
			{
				if ((info.m_Width != g_pEnvSet->m_nScreenWidth) || (info.m_Height != g_pEnvSet->m_nScreenHeight))
				{
					g_pEnvSet->m_nScreenHz = (INT32)info.m_RefreshRate;
				}

				g_pEnvSet->m_nScreenWidth = info.m_Width;
				g_pEnvSet->m_nScreenHeight = info.m_Height;

				bFind = true;
			}
		}

		if (bFind == false)
		{
			g_pEnvSet->m_nScreenWidth = 1024;
			g_pEnvSet->m_nScreenHeight = 768;
			g_pEnvSet->m_nScreenHz = 60;
			g_pEnvSet->m_AA = 0;
			g_pEnvSet->m_AAQ = 0;
		}

		pVideoInfo->m_Width = g_pEnvSet->m_nScreenWidth;
		pVideoInfo->m_Height = g_pEnvSet->m_nScreenHeight;
		pVideoInfo->m_RefreshRate = (REAL32)g_pEnvSet->m_nScreenHz;

		I3TRACE("FOUND PROPER RESOLUTION : %dx%d %d Hz\n", g_pEnvSet->m_nScreenWidth, g_pEnvSet->m_nScreenHeight, g_pEnvSet->m_nScreenHz);
	}

	g_pEnvSet->Save();
}

namespace
{
	void set_game_key_map(GAME_KEY_STRUCT * dest, UINT32 state, INT8 device = GAME_DEVICE_KEYBOARD)
	{
		dest->_nDiviceType = device;
		dest->_nState = state;
	}
}


void CEnvSet::InitBaseGameKeyMap()
{
	// Version
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_VERSION], GAME_KEY_VERSION_CUR);	//꼭 해줘야 되나봐.

																						//아래에 있는 내용은 기본 셋팅을 말하며, 화면(옵션창)에 보여지는 내용만 설정해 준다.
																						//화면에 보이지 않는 내용까지 셋팅을 하면, 중복 키 설정으로 표시될 여지가 있다.

																						//OptionControl::init_keyitem_list()
																						// 이동
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_LEFT], I3I_KEY_A);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_RIGHT], I3I_KEY_D);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_FORWARD], I3I_KEY_W);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_BACKWARD], I3I_KEY_S);

	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_JUMP], I3I_KEY_SPACE);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_FOOTSTEP], I3I_KEY_LSHIFT);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CROUCH], I3I_KEY_LCTRL);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_TURNBACK], I3I_KEY_F);

	// 무기사용
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_ATTACK], I3I_MOUSE_LBUTTON, GAME_DEVICE_MOUSE);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SUBFUNC], I3I_MOUSE_RBUTTON, GAME_DEVICE_MOUSE);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_WEAPON_EXTENSION], I3I_KEY_B);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_RELOAD], I3I_KEY_R);

	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_DROP], I3I_KEY_G);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SPECIALACTION], I3I_KEY_E);

	// 무기교체
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON], I3I_KEY_T);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_1], I3I_KEY_1);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_2], I3I_KEY_2);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_3], I3I_KEY_3);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_4], I3I_KEY_4);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_5], I3I_KEY_5);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_6], I3I_KEY_6);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_NEXT], I3I_MOUSE_WHEELDOWN, GAME_DEVICE_MOUSE);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_PREV], I3I_MOUSE_WHEELUP, GAME_DEVICE_MOUSE);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_QUICK], I3I_KEY_Q);

	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_7], UINT_MAX);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_8], UINT_MAX);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHANGEWEAPON_9], UINT_MAX);

	// 제압모드 특수스킬
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SPECIAL_MISSION], UINT_MAX);

	//전체 맵 보기
	//지금 사용하지 않음.
	//set_game_key_map( &m_BaseKeyMapArray[GAME_KEY_MAP_TOGGLE_MAP], I3I_KEY_M);

	// 기타0
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SHOWSCORE], I3I_KEY_TAB);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHAT], I3I_KEY_ENTER);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHAT_ALL], I3I_KEY_F2);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHAT_TEAM], I3I_KEY_F3);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHAT_CLAN], I3I_KEY_F4);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_CHAT_HISTORY], I3I_KEY_L);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MOUSE_SENS_UP], I3I_KEY_PGUP);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MOUSE_SENS_DOWN], I3I_KEY_PGDN);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_VIDEOCAPTURE], I3I_KEY_SCROLLLOCK);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_INGAME_SHOP], I3I_KEY_Y);

#if defined( LOCALE_JAPAN )
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MINIMAP_ENLARGE], I3I_KEY_COMMA);		// 미니맵 확대
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MINIMAP_REDUCTION], I3I_KEY_PERIOD);	// 미니맵 축소
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SCREENSHOT], I3I_KEY_F12);				// 스크린샷
#else
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MINIMAP_ENLARGE], I3I_KEY_EQUAL);		// 미니맵 확대
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_MINIMAP_REDUCTION], I3I_KEY_MINUS);	// 미니맵 축소
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_SCREENSHOT], I3I_KEY_F8);				// 스크린샷
#endif // LOCALE_JAPAN

																							//인게임 리폿
																							//지금 사용하지 않음.
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_INGAME_REPORT], UINT_MAX);

	//OptionAudio::init_keyitem_list()
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_RADIOMESSAGE1], I3I_KEY_Z);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_RADIOMESSAGE2], I3I_KEY_X);
	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_RADIOMESSAGE3], I3I_KEY_C);

	set_game_key_map(&m_BaseKeyMapArray[GAME_KEY_MAP_VOICECHAT], UINT_MAX);	//사용안함
}

void CEnvSet::InitGameKeyMap(void)
{
	memcpy(m_KeyMapArray, m_BaseKeyMapArray, (sizeof(GAME_KEY_STRUCT) * GAME_KEY_MAP_COUNT));
}

void CEnvSet::SetGameKeyMap(GAME_KEY_MAP map)
{
	memcpy(&m_KeyMapArray[map], &m_BaseKeyMapArray[map], sizeof(GAME_KEY_STRUCT));
}

void CEnvSet::LoadDefaultSave()
{
	LoadDefault();
	InitGameKeyMap();
}

void CEnvSet::Load()
{
	char szTemp[1024] = { 0, };
	char szDir[1024];
	i3RegistrySet * pEnvReg = i3RegistrySet::new_object();

	GetCurrentDirectory(1024, szDir);

	// 해당 버전이 삭제되었다는 것은 이전 버전을 이미 삭제한 경우이기에
	// 실패한 경우에만 이전 버전의 env 파일을 삭제하도록 한다.
	{
		bool bDelete = false;
		i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER2_10_1_1.env", szDir);
		bDelete = DeleteFile(szTemp) != false;

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER20121204.env", szDir);
			bDelete = DeleteFile(szTemp) != FALSE;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER0124.env", szDir);
			bDelete = DeleteFile(szTemp) != false;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER1015.env", szDir);
			bDelete = DeleteFile(szTemp) != false;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER0927.env", szDir);
			bDelete = DeleteFile(szTemp) != false;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_VER0617.env", szDir);
			bDelete = DeleteFile(szTemp) != false;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet_Temp.env", szDir);
			bDelete = DeleteFile(szTemp) != false;
		}

		if (bDelete == false)
		{
			i3::sprintf(szTemp, "%s/EnvSet/EnvSet.env", szDir);
			DeleteFile(szTemp);
		}
	}

	if (pEnvReg->LoadRegistrySet(m_EnvFileName.c_str()) == STREAM_ERR)
	{
		m_bLoadDefault = true;

		Save();
	}
	else
	{
		i3RegKey * pKey = pEnvReg->getRoot();
		if (pKey != nullptr)
		{
			// 빌드에 설정된 Default Language와 Env의 Language가 다를 경우 변경해준다
			INT32 nTemp = 0;
			GetKeyData(pKey, "Language", &nTemp);

			// Env에 설정되어 있는 언어 리소스가 없을 수도 있기 때문에
			// 없을 경우 빌드에 셋팅되어 있는 Env에 있는 언어로 Load 하지 않는다.
			char strLanguage[MAX_STRING_COUNT];
			i3::snprintf(strLanguage, sizeof(strLanguage), "Language/%s", GlobalFunc::GetLocaleFolderName(nTemp));

			if (nTemp != m_nLanguage
				&& nTemp < MAX_LANGUAGE_COUNT
				&& LocaleValue::Enable(strLanguage)
				&& nTemp > 0)
				m_nLanguage = nTemp;

			SetStringFilePath(m_nLanguage);

#if defined(LOCALE_PHILIPPINES) || defined(LOCALE_BRAZIL) || defined(LOCALE_NORTHAMERICA) || defined(LOCALE_LATIN_AMERICA)
			// 여기서 강제로 UI_V11로 바꾼다... 바꾸자.. 강제 세팅...
			// 실행 시점에 강제로 1.1로 바꾸는게 맞을까..
			SetUIVersion(UFV_1_1);
#elif defined(LOCALE_RUSSIA)
			// 여기서 강제로 UI_V11로 바꾼다... 바꾸자.. 강제 세팅...
			// 실행 시점에 강제로 1.1로 바꾸는게 맞을까..
			SetUIVersion(UFV_1_2);
#else
			nTemp = -1;
			GetKeyData(pKey, "UI_Version", &nTemp);
			if(nTemp > 0)
				SetUIVersion(nTemp);
#endif
			

			////////////////////////////////////////////////////////////

			GetKeyData(pKey, "Full_Screen", &m_bFullScreen);

#ifdef WINDOW_MODE_DISABLE
			m_bFullScreen = true;
#endif	

			GetKeyData(pKey, "Screen_Width", &m_nScreenWidth);
			GetKeyData(pKey, "Screen_Height", &m_nScreenHeight);
			GetKeyData(pKey, "Screen_Hz", &m_nScreenHz);
			GetKeyData(pKey, "Screen_Depth", (INT32 *)&m_ScreenDepth);
			GetKeyData(pKey, "AntiAliasing", (INT32 *)&m_AA);
			GetKeyData(pKey, "AntiAliasingQ", (INT32 *)&m_AAQ);
			GetKeyData(pKey, "VSync", (INT32 *)&m_bVSync);
			GetKeyData(pKey, "TriLinearFilter", (INT32 *)&m_bTriLinearFilter);
			GetKeyData(pKey, "VideoResolution", (INT32 *)&m_nVideoResolution);
			GetKeyData(pKey, "Screen_Gamma_Value", &m_fGammaVal);

#if defined(USE_LINK_SHOP)
			GetKeyData(pKey, "Day", m_strDay);
#endif		

			GetKeyData(pKey, "Speaker_Type", &m_nSpeakerType);
			GetKeyData(pKey, "Enable_Bullet_Trace", &m_bEnableBulletTrace);
			GetKeyData(pKey, "Enable_Bullet_Smoke", &m_bEnableBulletSmoke);
			GetKeyData(pKey, "Enable_DynamicLight", &m_bDynamicLight);
			GetKeyData(pKey, "Enable_NormalMap", &m_bEnableNormalMap);
			GetKeyData(pKey, "Enable_TerrainEffect", &m_bEnableTerrainEffect);
			GetKeyData(pKey, "Shadow_Quality_Type", &m_nShadowQualityType);
			GetKeyData(pKey, "Tex_Quality_Type", &m_nTextureQualityType);
			GetKeyData(pKey, "16BitsDepth", &m_b16BitsDepth);
			GetKeyData(pKey, "Specular_Quality_Type", &m_nSpecularQualityType);
			GetKeyData(pKey, "BloomEnable", &m_bEnableBloom);
			GetKeyData(pKey, "PhysXEnable", &m_bEnablePhysX);
			GetKeyData(pKey, "EffectQuality", &m_nEffectQuality);

			GetKeyData(pKey, "DefaultNation", &m_nDefaultNation);
			
			if (m_nUIVersion == UFV_1_5)
			{
				GetKeyData(pKey, "GameInterface", &m_nGameInterface);
				GetKeyData(pKey, "Display_Chara", &m_bFixedViewCharacter);
				GetKeyData(pKey, "Display_Parts", &m_bDisplayOtherParts);
			}
			else
			{
				m_nGameInterface		= HUD::HS_1_0;
				m_bFixedViewCharacter	= false;
				m_bDisplayOtherParts	= true;
			}

			GetKeyData(pKey, "Enable_TeamBand", &m_bTeamBand);
#if defined (DISABLE_TEAMBAND)
			m_bTeamBand = false;
#endif
		}
	}

	/*
	// Memory의 크기가 1G 이하인 경우 Texture Quality를 강제로 Down
	{
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof (statex);

	GlobalMemoryStatusEx (&statex);

	INT32 limitSize = 1024 * 1024 * 1024;

	if( statex.ullTotalPhys <= limitSize)
	{
	if( m_nTextureQualityType < 1)
	m_nTextureQualityType = 1;		// 중 이하의 급으로 조정
	}
	}
	*/


	// 16비트 깊이버퍼는 항상 false로 합니다.
	m_b16BitsDepth = false;

	I3_SAFE_RELEASE(pEnvReg);
}

void CEnvSet::Save()
{
	if (m_pEnvReg) I3_SAFE_RELEASE(m_pEnvReg);

	CFileFind finder;
	char szCurDir[MAX_STRING_COUNT] = { 0, };
	char szSaveDirectory[MAX_STRING_COUNT] = { 0, };

	// 만약 세이브할 디렉토리가 없다면 디렉토리 생성합니다.
	::GetCurrentDirectory(MAX_STRING_COUNT, szCurDir);
	i3::safe_string_copy(szSaveDirectory, szCurDir, MAX_STRING_COUNT);
	i3::generic_string_cat(szSaveDirectory, "\\EnvSet\\*.*");
	BOOL bWorking = finder.FindFile(szSaveDirectory);
	finder.Close();
	if (!bWorking)
	{
		i3mem::FillZero(szSaveDirectory, sizeof(char)*MAX_STRING_COUNT);
		i3::safe_string_copy(szSaveDirectory, szCurDir, MAX_STRING_COUNT);
		i3::generic_string_cat(szSaveDirectory, "\\EnvSet");

		CreateDirectory(szSaveDirectory, nullptr);
	}

	m_nUserKey1 = 0;
	m_nUserKey2 = 0;

	m_pEnvReg = i3RegistrySet::new_object();

	m_pEnvReg->getRoot()->AddData("Language", m_nLanguage);
	m_pEnvReg->getRoot()->AddData("DefaultNation", m_nDefaultNation);
	
	m_pEnvReg->getRoot()->AddData("UI_Version", m_nUIVersion);
	m_pEnvReg->getRoot()->AddData("Full_Screen", m_bFullScreen);
	m_pEnvReg->getRoot()->AddData("Screen_Width", m_nScreenWidth);
	m_pEnvReg->getRoot()->AddData("Screen_Height", m_nScreenHeight);
	m_pEnvReg->getRoot()->AddData("Screen_Hz", m_nScreenHz);
	m_pEnvReg->getRoot()->AddData("Screen_Depth", m_ScreenDepth);
	m_pEnvReg->getRoot()->AddData("AntiAliasing", m_AA);
	m_pEnvReg->getRoot()->AddData("AntiAliasingQ", m_AAQ);
	m_pEnvReg->getRoot()->AddData("VSync", m_bVSync);
	m_pEnvReg->getRoot()->AddData("TriLinearFilter", m_bTriLinearFilter);
	m_pEnvReg->getRoot()->AddData("VideoResolution", m_nVideoResolution);

#if defined(USE_LINK_SHOP)
	m_pEnvReg->getRoot()->AddData("Day", m_strDay);
#endif

	m_pEnvReg->getRoot()->AddData("Screen_Gamma_Value", m_fGammaVal);
	m_pEnvReg->getRoot()->AddData("Speaker_Type", m_nSpeakerType);
	m_pEnvReg->getRoot()->AddData("Enable_Bullet_Trace", m_bEnableBulletTrace);
	m_pEnvReg->getRoot()->AddData("Enable_Bullet_Smoke", m_bEnableBulletSmoke);
	m_pEnvReg->getRoot()->AddData("Enable_DynamicLight", m_bDynamicLight);
	m_pEnvReg->getRoot()->AddData("Enable_NormalMap", m_bEnableNormalMap);
	m_pEnvReg->getRoot()->AddData("Enable_TerrainEffect", m_bEnableTerrainEffect);
	m_pEnvReg->getRoot()->AddData("Shadow_Quality_Type", m_nShadowQualityType);
	m_pEnvReg->getRoot()->AddData("Tex_Quality_Type", m_nTextureQualityType);
	m_pEnvReg->getRoot()->AddData("16BitsDepth", m_b16BitsDepth);
	m_pEnvReg->getRoot()->AddData("Specular_Quality_Type", m_nSpecularQualityType);
	m_pEnvReg->getRoot()->AddData("BloomEnable", m_bEnableBloom);
	m_pEnvReg->getRoot()->AddData("PhysXEnable", m_bEnablePhysX);
	m_pEnvReg->getRoot()->AddData("EffectQuality", m_nEffectQuality);

	m_pEnvReg->getRoot()->AddData("GameInterface", m_nGameInterface);
#if defined (DISABLE_TEAMBAND)
	m_bTeamBand = false;
#endif
	m_pEnvReg->getRoot()->AddData("Enable_TeamBand", m_bTeamBand);
	m_pEnvReg->getRoot()->AddData("Display_Parts", m_bDisplayOtherParts);
	m_pEnvReg->getRoot()->AddData("Display_Chara", m_bFixedViewCharacter);

	m_pEnvReg->SaveRegistrySet(m_EnvFileName.c_str());

	I3_SAFE_RELEASE(m_pEnvReg);
}

void CEnvSet::Net_Load(void)
{
	INT32 nTemp = 0;
	const NET_ENV_SET * pNetData = EnvInfoContext::i()->GetNetEnvSet();

	// GamePlay
	m_bEnableBlood = (pNetData->m_GamePlayState & GAME_PLAY_BLOOD_MASK) ? true : false;
	m_bCrossHairEffect = (pNetData->m_GamePlayState & GAME_PLAY_CROSSHAIREFFECT_MASK) ? true : false;
	GetValue(pNetData->m_GamePlayState, &m_nCrossHairType, 0x00ff0000, 16);
	GetValue(pNetData->m_GamePlayState, &m_nHandType, 0xff000000, 24);

	// Interface
	m_bEnableFrindlyName = true;
	m_bEnableFrindlyMark = (pNetData->m_InterfaceState & GAME_INTERFACE_MARK_MASK) ? true : false;
	m_bTextEffect = (pNetData->m_InterfaceState & GAME_INTERFACE_TEXT_MASK) ? true : false;
	m_bMiniMapMode = (pNetData->m_InterfaceState & GAME_INTERFACE_MINIMAP_MASK) ? true : false;
	m_bChallange = (pNetData->m_InterfaceState & GAME_INTERFACE_CHALLANGE_MASK) ? true : false;
	m_b1VS1 = TRUE; //(pNetData->m_InterfaceState & GAME_INTERFACE_1VS1_MASK) ? TRUE : FALSE; - GD팀 요청으로 V3에서는 항상 1:1 전적 기록이 항상 뜬다

	//GameSound
	m_bEnableBGM = (pNetData->m_GameSoundState & GAME_SOUND_BGM_MASK) ? true : false;
	m_bEnableEffect = (pNetData->m_GameSoundState & GAME_SOUND_EFFECT_MASK) ? true : false;
	m_bEnableRadioMessage = (pNetData->m_GameSoundState & GAME_SOUND_RADIO_MESSAGE_MASK) ? true : false;
	GetValue(pNetData->m_GameSoundState, &m_nRadioMessageType, 0xff000000, 24);
	GetValue(pNetData->m_GameSoundState, &m_nRadioMode, 0x00ff0000, 16);
	GetValue(pNetData->m_GameSoundVol, &m_fVolBGM, 0xff000000, 24);
	GetValue(pNetData->m_GameSoundVol, &m_fVolEffect, 0x00ff0000, 16);

	// GameControl
	m_bEnableMouseInvert = (pNetData->m_GameControlState & GAME_CONTROL_MOUSE_INVERT) ? true : false;
	GetValue(pNetData->m_GameControlState, &nTemp, 0xffff0000, 16);
	m_rMouseSensivity = ((REAL32)nTemp) * 0.01000001f;
	if (m_rMouseSensivity < MIN_MOUSE_SENSIVITY) m_rMouseSensivity = MIN_MOUSE_SENSIVITY;
	else if (m_rMouseSensivity > MAX_MOUSE_SENSIVITY) m_rMouseSensivity = MAX_MOUSE_SENSIVITY;
	GetValue(pNetData->m_GameControlState, &nTemp, 0x000000ff, 0);
	m_fFovValue = (REAL32)nTemp;
	if (m_fFovValue < MIN_CAM_FOV) m_fFovValue = DEF_CAM_FOV;
	else if (m_fFovValue > MAX_CAM_FOV) m_fFovValue = MAX_CAM_FOV;

	// GameCommunity
	m_nChatType = 0;
	GetValue(pNetData->m_GameCommunityState, &m_nWhisperType, 0x0f000000, 24);
	GetValue(pNetData->m_GameCommunityState, &m_nInviteType, 0x00f00000, 20);
	GetValue(pNetData->m_GameCommunityState, &m_nFriendType, 0x000f0000, 16);
	GetValue(pNetData->m_GameCommunityState, &m_nMessageType, 0x0000f000, 12);
	GetValue(pNetData->m_GameCommunityState, &m_nClanType, 0x00000f00, 8);

#ifndef ACTIVATE_COMMUNITY_STATE_NOTE
	m_nMessageType = 0;
#endif

	// Macro
	m_bMacroEnableFriend[0] = (pNetData->m_GameMacroState & GAME_MACRO_FRIEND_MASK1) ? true : false;
	m_bMacroEnableFriend[1] = (pNetData->m_GameMacroState & GAME_MACRO_FRIEND_MASK2) ? true : false;
	m_bMacroEnableFriend[2] = (pNetData->m_GameMacroState & GAME_MACRO_FRIEND_MASK3) ? true : false;
	m_bMacroEnableFriend[3] = (pNetData->m_GameMacroState & GAME_MACRO_FRIEND_MASK4) ? true : false;
	m_bMacroEnableFriend[4] = (pNetData->m_GameMacroState & GAME_MACRO_FRIEND_MASK5) ? true : false;

	for (INT32 i = 0; i < MACRO_MAX_COUNT; ++i)
	{
		const i3::rc_wstring& wstrContextMacro = EnvInfoContext::i()->GetMacro(i);
		m_wstrMacro[i] = wstrContextMacro;
	}

	// GameKey Update
	UpdateKeyMap();
}

void CEnvSet::Net_Save(void)
{
	INT32 nTemp = 0;
	NET_ENV_SET NetData;
	memset(&NetData, 0, sizeof(NET_ENV_SET));

	// GamePlay
	if (m_bEnableBlood) AddFlag(&NetData.m_GamePlayState, GAME_PLAY_BLOOD_MASK);
	if (m_bCrossHairEffect) AddFlag(&NetData.m_GamePlayState, GAME_PLAY_CROSSHAIREFFECT_MASK);

	AddValue(&NetData.m_GamePlayState, m_nCrossHairType, 16);
	AddValue(&NetData.m_GamePlayState, m_nHandType, 24);

	// Interface
	if (m_bEnableFrindlyName) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_NAME_MASK);
	if (m_bEnableFrindlyMark) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_MARK_MASK);
	if (m_bTextEffect) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_TEXT_MASK);
	if (m_bMiniMapMode) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_MINIMAP_MASK);
	if (m_bChallange) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_CHALLANGE_MASK);
	if (m_b1VS1) AddFlag(&NetData.m_InterfaceState, GAME_INTERFACE_1VS1_MASK);

	//GameSound
	if (m_bEnableBGM) AddFlag(&NetData.m_GameSoundState, GAME_SOUND_BGM_MASK);
	if (m_bEnableEffect) AddFlag(&NetData.m_GameSoundState, GAME_SOUND_EFFECT_MASK);
	if (m_bEnableRadioMessage) AddFlag(&NetData.m_GameSoundState, GAME_SOUND_RADIO_MESSAGE_MASK);
	AddValue(&NetData.m_GameSoundState, m_nRadioMessageType, 24);
	AddValue(&NetData.m_GameSoundState, m_nRadioMode, 16);
	AddValue(&NetData.m_GameSoundVol, (INT32)m_fVolBGM, 24);
	AddValue(&NetData.m_GameSoundVol, (INT32)m_fVolEffect, 16);

	// GameControl
	if (m_bEnableMouseInvert) AddFlag(&NetData.m_GameControlState, GAME_CONTROL_MOUSE_INVERT);
	nTemp = (INT32)(m_rMouseSensivity * 100.0f);
	AddValue(&NetData.m_GameControlState, nTemp, 16);
	AddValue(&NetData.m_GameControlState, (INT32)m_fFovValue, 0);
	EnvInfoContext::i()->SetKeyMapArray(m_KeyMapArray);

	// GameCommunity
	AddValue(&NetData.m_GameCommunityState, m_nChatType, 28);
	AddValue(&NetData.m_GameCommunityState, m_nWhisperType, 24);
	AddValue(&NetData.m_GameCommunityState, m_nInviteType, 20);
	AddValue(&NetData.m_GameCommunityState, m_nFriendType, 16);
	AddValue(&NetData.m_GameCommunityState, m_nMessageType, 12);
	AddValue(&NetData.m_GameCommunityState, m_nClanType, 8);

	// Macro
	if (m_bMacroEnableFriend[0]) AddFlag(&NetData.m_GameMacroState, GAME_MACRO_FRIEND_MASK1);
	if (m_bMacroEnableFriend[1]) AddFlag(&NetData.m_GameMacroState, GAME_MACRO_FRIEND_MASK2);
	if (m_bMacroEnableFriend[2]) AddFlag(&NetData.m_GameMacroState, GAME_MACRO_FRIEND_MASK3);
	if (m_bMacroEnableFriend[3]) AddFlag(&NetData.m_GameMacroState, GAME_MACRO_FRIEND_MASK4);
	if (m_bMacroEnableFriend[4]) AddFlag(&NetData.m_GameMacroState, GAME_MACRO_FRIEND_MASK5);

	for (INT32 i = 0; i < MACRO_MAX_COUNT; ++i)
		EnvInfoContext::i()->SetMacro(i, m_wstrMacro[i]);

	//Save
	EnvInfoContext::i()->SetNetEnvSet(&NetData);
}

void CEnvSet::AddValue(UINT32* Target, INT32 Val, INT32 Where)
{
	UINT32 nTemp = Val;
	nTemp = nTemp << Where;
	*Target = *Target | nTemp;
}

void CEnvSet::GetValue(UINT32 Target, INT32* Val, UINT32 Mask, INT32 Where)
{
	*Val = (Target & Mask) >> Where;
}

void CEnvSet::GetValue(UINT32 Target, REAL32* Val, UINT32 Mask, INT32 Where)
{
	*Val = (REAL32)((Target & Mask) >> Where);
}

namespace inner
{
	bool is_exception_key(INT32 idx) //GAME_KEY_MAP 배열에는 등록 되었지만, 옵션 창에는 표시 안 되는 키, 필수 등록!
	{
		switch (idx)
		{
		case GAME_KEY_MAP_VIDEOCAPTURE:
			return !(LocaleValue::Enable("EnableVideoCapture") == true);
			break;
		case GAME_KEY_MAP_INGAME_SHOP:
			return !(LocaleValue::Enable("FieldShop") == true);
			break;
		case GAME_KEY_MAP_TOGGLE_MAP: //@Eunil. PBBUG-17450 빠른 무기 교체와 충돌나서 설정 변경이 안된다.
			return true;
		}

		return false;
	}
}

void CEnvSet::CheckKeyMap()
{

	//	bool enable_video_capture = LocaleValue::Enable("EnableVideoCapture");
	//	bool enable_field_shop = LocaleValue::Enable("FieldShop");

	// 1부터 시작하는 이유는 처음부분에는 Version값이 들어가기 때문입니다.
	for (INT32 i = 1; i < GAME_KEY_MAP_COUNT; i++)
	{
		// 해당 옵션을 쓰지 않을경우 체크하지 않음

		if (inner::is_exception_key(i) == true) continue;

		if (m_KeyMapArray[i]._nDiviceType == 0)		// 키보드
		{
			if (m_KeyMapArray[i]._nState == UINT_MAX)
			{
				// 사용하지 않는 키
			}
			else if (m_KeyMapArray[i]._nState >= I3I_KEY_MAX)
			{
				SetGameKeyMap((GAME_KEY_MAP)i);
			}
			else
			{
				for (INT32 x = 1; x < GAME_KEY_MAP_COUNT; x++)
				{
					if (inner::is_exception_key(x) == true) continue;

					if ((x != i) &&
						(m_KeyMapArray[x]._nDiviceType == m_KeyMapArray[i]._nDiviceType) &&
						(m_KeyMapArray[x]._nState == m_KeyMapArray[i]._nState))
					{
						InitGameKeyMap();
						m_bWrongEnvSet = true;

						// GameContext에 변화되어진 Key의 셋팅을 알려줍니다.
						EnvInfoContext::i()->SetNetEnvSetBasicChange(EnvInfoContext::i()->GetNetEnvSetBasicChange() | 0x00000002);
						Net_Save();
						GameEventSender::i()->SetEvent(EVENT_OPTION_SAVE_N);

						return;
					}
				}
			}
		}
		else if (m_KeyMapArray[i]._nDiviceType == 1)	// 마우스
		{
			if (m_KeyMapArray[i]._nState == UINT_MAX)
			{
				// 사용하지 않는 키
			}
			else if (m_KeyMapArray[i]._nState != I3I_MOUSE_LBUTTON && m_KeyMapArray[i]._nState != I3I_MOUSE_RBUTTON && m_KeyMapArray[i]._nState != I3I_MOUSE_MBUTTON &&
				m_KeyMapArray[i]._nState != I3I_MOUSE_WHEELUP && m_KeyMapArray[i]._nState != I3I_MOUSE_WHEELDOWN && m_KeyMapArray[i]._nState != 0)
			{
				SetGameKeyMap((GAME_KEY_MAP)i);
			}
			else
			{
				for (INT32 x = 1; x < GAME_KEY_MAP_COUNT; x++)
				{
					if (x != i &&
						m_KeyMapArray[x]._nDiviceType == m_KeyMapArray[i]._nDiviceType &&
						m_KeyMapArray[x]._nState == m_KeyMapArray[i]._nState)
					{
						InitGameKeyMap();
						m_bWrongEnvSet = true;

						// GameContext에 변화되어진 Key의 셋팅을 알려줍니다.
						EnvInfoContext::i()->SetNetEnvSetBasicChange(EnvInfoContext::i()->GetNetEnvSetBasicChange() | 0x00000002);
						Net_Save();
						GameEventSender::i()->SetEvent(EVENT_OPTION_SAVE_N);

						return;
					}
				}
			}
		}
		else											// 정의 되지 않은...
		{
			SetGameKeyMap((GAME_KEY_MAP)i);
		}
	}
}

void CEnvSet::UpdateKeyMap(void)
{
	bool bVersionChange = false;

	// Key값을 얻어옵니다.
	i3mem::Copy(m_KeyMapArray, EnvInfoContext::i()->GetKeyMapArray(0), sizeof(GAME_KEY_STRUCT)*GAME_KEY_MAP_COUNT);

	// Version을 체크합니다.
	if (m_KeyMapArray[GAME_KEY_MAP_VERSION]._nState < GAME_KEY_VERSION_20111212_1)
	{
		bVersionChange = true;

		// (추가된 키에대하여 -1로 초기화합니다)
		// 2008년 4월 17일 추가된 내용 // 1.GAME_KEY_MAP_CHAT_CLAN 추가
		// 2008년 9월 1일 추가된 내용	// 1.GAME_KEY_MAP_LASERDOT 추가
		// 2011년 12월 12일 추가된 내용		// 비됴캡춰와 야전상점 관련된 것..

		m_KeyMapArray[GAME_KEY_MAP_VERSION]._nState = GAME_KEY_VERSION_20111212_1;
		m_KeyMapArray[GAME_KEY_MAP_VIDEOCAPTURE]._nDiviceType = -1;
		m_KeyMapArray[GAME_KEY_MAP_INGAME_SHOP]._nDiviceType = -1;
	}

	// 로드 후 저장된 키맵이 맞는지 체크하고 없으면 디폴트로 세팅해줍니다.
	CheckKeyMap(); // 기획서 대비 사용하지 않는 키도 존재하므로, 디폴트로 키를 설정하지 않습니다.

#if defined( I3_DEBUG ) || defined( BUILD_RELEASE_QA_VERSION) || defined(LOCALE_BRAZIL)
	if (m_KeyMapArray[GAME_KEY_MAP_CHAT]._nState != I3I_KEY_ENTER)
	{
		InitBaseGameKeyMap();
		InitGameKeyMap();

		EnvInfoContext::i()->SetNetEnvSetBasicChange(EnvInfoContext::i()->GetNetEnvSetBasicChange() | 0x00000002);
		Net_Save();
		GameEventSender::i()->SetEvent(EVENT_OPTION_SAVE_N);
	}
#endif

	// Version이 변경되었다면 변경되어진 Version을 저장합니다.
	// UserInfo 정보를 받았을 경우에만 해당 패킷을 보내야합니다.
	if (bVersionChange && UserInfoContext::i()->GetReceivedUserInfo() )
	{
		EnvInfoContext::i()->SetKeyMapArray(m_KeyMapArray);

		// GameContext에 변화되어진 Key의 셋팅을 알려줍니다.
		EnvInfoContext::i()->SetNetEnvSetBasicChange(EnvInfoContext::i()->GetNetEnvSetBasicChange() | 0x00000002);
		GameEventSender::i()->SetEvent(EVENT_OPTION_SAVE_N);
	}
	else
	{
		I3PRINTLOG(I3LOG_NOTICE, "USERINFO 패킷을 받지 않고 인벤셋을 받음");
	}
}

void CEnvSet::GetVideoInfoString(INT32 width, INT32 height, i3::wstring& out)
{
	if ((REAL32)width / height > 1.5f)
		i3::sprintf(out, L"%d x %d (Wide)", width, height);
	else
		i3::sprintf(out, L"%d x %d", width, height);
}

namespace
{
	const i3::fixed_string	g_str_Crosshair_Cross = "Crosshair_Cross";
	const i3::fixed_string	g_str_Crosshair_CrossDot = "Crosshair_CrossDot";
	const i3::fixed_string	g_str_Crosshair_Dot = "Crosshair_Dot";
	const i3::fixed_string	g_str_Crosshair_Ring = "Crosshair_Ring";
}


const i3::fixed_string& CEnvSet::GetCrosshairName() const
{
	const i3::fixed_string* pstrCrosshairName = nullptr;

	switch (m_nCrossHairType)
	{
	case 0:	pstrCrosshairName = &g_str_Crosshair_Cross; break; //"Crosshair_Cross";	break;
	case 1:	pstrCrosshairName = &g_str_Crosshair_CrossDot; break; //"Crosshair_CrossDot";	break;
	case 2:	pstrCrosshairName = &g_str_Crosshair_Dot; break; //"Crosshair_Dot";	break;
	case 3:	pstrCrosshairName = &g_str_Crosshair_Ring; break; //"Crosshair_Ring";	break;
	default:	pstrCrosshairName = &g_str_Crosshair_CrossDot; break; //"Crosshair_CrossDot";	break;
	}

	return *pstrCrosshairName;
}

const i3::fixed_string& CEnvSet::GetCrosshairName(INT32 nType) const
{
	const i3::fixed_string* pstrCrosshairName = NULL;

	switch (nType)
	{
	case 0:	pstrCrosshairName = &g_str_Crosshair_Cross; break; //"Crosshair_Cross";	break;
	case 1:	pstrCrosshairName = &g_str_Crosshair_CrossDot; break; //"Crosshair_CrossDot";	break;
	case 2:	pstrCrosshairName = &g_str_Crosshair_Dot; break; //"Crosshair_Dot";	break;
	case 3:	pstrCrosshairName = &g_str_Crosshair_Ring; break; //"Crosshair_Ring";	break;
	default:	pstrCrosshairName = &g_str_Crosshair_CrossDot; break; //"Crosshair_CrossDot";	break;
	}

	return *pstrCrosshairName;
}

void CEnvSet::_LoadDefaultForKorea(CEnvSet& envSet)
{
	// screen config
	envSet.m_AA = 0;		// 안티 앨리어싱
	envSet.m_bVSync = false;	// 화면동기화
	envSet.m_bTriLinearFilter = true;		// 부드러운원근효과

											// performance
	envSet.m_nTextureQualityType = 0;		// 상   텍스쳐퀄리티
	envSet.m_nShadowQualityType = 2;		// 중   그림자효과
	envSet.m_nSpecularQualityType = 2;		// 상   빛반사효과
	envSet.m_nEffectQuality = 2;		// 상   이펙트품질
	envSet.m_bEnableBulletTrace = true;		// 총알궤적
	envSet.m_bDynamicLight = true;		// 광원효과
	envSet.m_bEnableNormalMap = true;		// 세부제잴표현
	envSet.m_bEnableTerrainEffect = true;		// 지형이펙트
	envSet.m_bEnablePhysX = true;		// 물리효과
}

void CEnvSet::_LoadDefaultForJapan(CEnvSet& envSet)
{
	// Radio Language
	envSet.m_nRadioMessageType		= LocaleValue::Enable("EnableMultiRadioLanguage") ? 1 : 0;

	envSet.m_bTeamBand				= false;	// 팀밴드 사용 유무

	// screen config
	envSet.m_AA						= 4;		// 안티 앨리어싱
	envSet.m_bVSync					= true;		// 화면동기화
	envSet.m_bTriLinearFilter		= true;		// 부드러운원근효과

	// performance
	envSet.m_nTextureQualityType	= 0;		// 상   텍스쳐퀄리티
	envSet.m_nShadowQualityType		= 3;		// 상   그림자효과
	envSet.m_nSpecularQualityType	= 2;		// 상   빛반사효과
	envSet.m_nEffectQuality			= 2;		// 상   이펙트품질
	envSet.m_bEnableBulletTrace		= true;		// 총알궤적
	envSet.m_bDynamicLight			= true;		// 광원효과
	envSet.m_bEnableNormalMap		= true;		// 세부제잴표현
	envSet.m_bEnableTerrainEffect	= true;		// 지형이펙트
	envSet.m_bEnablePhysX			= true;		// 물리효과
}

void CEnvSet::_LoadDefaultForRussia(CEnvSet& envSet)
{
	// Radio Language
	envSet.m_nRadioMessageType = LocaleValue::Enable("EnableMultiRadioLanguage") ? 1 : 0;

	envSet.m_bTeamBand = false;	// 팀밴드 사용 유무

								// screen config
	envSet.m_AA = 4;		// 안티 앨리어싱
	envSet.m_bVSync = true;		// 화면동기화
	envSet.m_bTriLinearFilter = true;		// 부드러운원근효과

											// performance
	envSet.m_nTextureQualityType = 0;		// 상   텍스쳐퀄리티
	envSet.m_nShadowQualityType = 3;		// 상   그림자효과
	envSet.m_nSpecularQualityType = 2;		// 상   빛반사효과
	envSet.m_nEffectQuality = 2;		// 상   이펙트품질
	envSet.m_bEnableBulletTrace = true;		// 총알궤적
	envSet.m_bDynamicLight = true;		// 광원효과
	envSet.m_bEnableNormalMap = true;		// 세부제잴표현
	envSet.m_bEnableTerrainEffect = true;		// 지형이펙트
	envSet.m_bEnablePhysX = true;		// 물리효과
}