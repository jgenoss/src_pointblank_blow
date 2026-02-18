#ifndef _ENVSET_H__
#define _ENVSET_H__
#include "UI/UIHudDefine.h"

#define GAME_PLAY_BLOOD_MASK			0x00000001
#define GAME_PLAY_CROSSHAIREFFECT_MASK	0x00000002
#define GAME_PLAY_HITEFFECT_MASK		0x00000004

#define GAME_INTERFACE_NAME_MASK		0x00000001
#define GAME_INTERFACE_MARK_MASK		0x00000002
#define GAME_INTERFACE_TEXT_MASK		0x00000004
#define GAME_INTERFACE_MINIMAP_MASK		0x00000008
#define GAME_INTERFACE_CHALLANGE_MASK	0x00000010
#define GAME_INTERFACE_1VS1_MASK		0x00000020

#define GAME_SOUND_BGM_MASK				0x00000001
#define GAME_SOUND_EFFECT_MASK			0x00000002
#define GAME_SOUND_RADIO_MESSAGE_MASK	0x00000004

#define	GAME_CONTROL_MOUSE_INVERT		0x01000000

#define GAME_COMMUNITY_CHAT_TYPE		0x00000001
#define GAME_COMMUNITY_WHISPER_TYPE		0x00000002
#define GAME_COMMUNITY_INVITE_TYPE		0x00000004
#define GAME_COMMUNITY_FRIEND_TYPE		0x00000008
#define GAME_COMMUNITY_MESSAGE_TYPE		0x00000010
#define GAME_COMMUNITY_CLAN_TYPE		0x00000020

#define GAME_MACRO_FRIEND_MASK1			0x00000001
#define GAME_MACRO_FRIEND_MASK2			0x00000002
#define GAME_MACRO_FRIEND_MASK3			0x00000004
#define GAME_MACRO_FRIEND_MASK4			0x00000008
#define GAME_MACRO_FRIEND_MASK5			0x00000010

// 버젼 추가 디파인문( 맨뒤 한자리는 그날추가되는 버젼의 숫자이며 나머지는 년월일 순으로 표시합니다.
//#define GAME_KEY_VERSION_20080417_1		 804172
//#define GAME_KEY_VERSION_20080901_1		 809011
#define GAME_KEY_VERSION_20111212_1			1112121
#define GAME_KEY_VERSION_CUR				GAME_KEY_VERSION_20111212_1 //GAME_KEY_VERSION_20080901_1

struct VID_INFO
{
	INT32				m_Width = 0;
	INT32				m_Height = 0;
	REAL32				m_Hz = 0.0f;
} ;

enum CROSSHAIR_TYPE
{
	CROSSHAIR_TYPE_CROSS = 0,
	CROSSHAIR_TYPE_CROSSDOT,
	CROSSHAIR_TYPE_DOT,
	CROSSHAIR_TYPE_RING,	

	CROSSHAIR_TYPE_COUNT,
};

enum UI_FRAMEWORK_VERSION 
{ 
	UFV_NON = -1,
	UFV_1_5, 
	UFV_1_1, 
	UFV_1_0, 
	UFV_1_2,
	UFV_MAX
};

class CEnvSet :	public i3ElementBase
{
	I3_CLASS_DEFINE( CEnvSet, i3ElementBase );

public:
	CEnvSet(void);
	~CEnvSet(void);

public:
	i3RegistrySet *	m_pEnvReg;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 파일에 저장되어지는것
	/////////////////////////////////////////////////////////////////////////////////////////////
	//------  Time  ------------
	i3::rc_wstring	m_strDay;

	// 국가 ID
	// MSDN Language identifier
	UI_FRAMEWORK_VERSION			m_nUIVersion;
	INT32							m_nDefaultNation;

	// ----- Screen Config -----
	bool			m_bFullScreen;
	INT32			m_nScreenWidth;											//해상도설정 : 지원가능한 해상도?
	INT32			m_nScreenHeight;
	INT32			m_nScreenHz;
	INT32			m_ScreenDepth;
	INT32			m_AA;
	INT32			m_AAQ;
	bool			m_bVSync;
	bool			m_bTriLinearFilter;
	INT32			m_nVideoResolution;										// 영상 해상도 
	bool			m_bDisplayOtherParts; //상대방 파츠 표시
	bool			m_bFixedViewCharacter; // 캐릭터 보기 고정

	// ----- Gamma -----
	REAL32			m_fGammaVal;

	// ----- Sound -----
	INT32			m_nSpeakerType;											// 스피커 설정	
	
	// ----- Perfomance -----
	bool			m_bEnableBulletTrace;									// 총알궤적
	bool			m_bEnableBulletSmoke;									// 총알연기
	bool			m_bDynamicLight;												
	bool			m_bEnableNormalMap;										// Normal Map
	bool			m_bEnableTerrainEffect;									// 지형 이펙트
	bool			m_b16BitsDepth;
	INT32			m_nTextureQualityType;									// 텍스쳐 퀄러티	0:High, 1:Middle, 2:Low, 3:Very Low
	INT32			m_nShadowQualityType;									// 그림자 효과		0:Low, 1:Middle, 2:High
	INT32			m_nEffectLevel;											// 이팩트 효과		
	INT32			m_nSpecularQualityType;									// 빛반사 효과		0:Low, 1:Middle, 2:High
	bool			m_bEnableBloom;
	bool			m_bEnablePhysX;											// 물리효과
	INT32			m_nEffectQuality;										// 이펙트품질
	// ------ Hidden -------
	INT32			m_nUserKey1;
	INT32			m_nUserKey2;
	ULONGLONG		m_Ori64Mac;
	ULONGLONG		m_ui64Mac[3];											// Mac Address

	// ----- MultiLanguage -----
	INT32			m_nLanguage;

	/////////////////////////////////////////////////////////////////////////////////////////////
	// 네트워크로 보내어 저장되어지는것
	/////////////////////////////////////////////////////////////////////////////////////////////

	// ----- Game Play -----	
	bool			m_bEnableBlood;											// 혈흔 표시
	bool			m_bCrossHairEffect;										// 크로스헤어 타격 이펙트
	INT32			m_nCrossHairType;										// 크로스헤어 종류
	INT32			m_nHandType;											// 무기드는 손(default:0오른손, 1:왼손)


	// ----- Interface -----
	bool			m_bEnableFrindlyName;									// 아군 닉네임 표시
	bool			m_bEnableFrindlyMark;									// 아군 마크 표시
	bool			m_bTextEffect;											// 텍스트 이펙트 on/off
	bool			m_bMiniMapMode;											// 미니맵 표시방식( 미니맵(on)/레이다(off))
	bool			m_bChallange;											// 도저과제 on/off
	bool			m_b1VS1;
	bool m_nGameInterface; //new 가 0이다.
	bool m_bTeamBand;

	// ----- Sound -----
	bool			m_bEnableBGM;
	bool			m_bEnableEffect;
	bool			m_bEnableRadioMessage;	
	INT32			m_nRadioMessageType;									// 0:영문, 1:한글
	INT32			m_nRadioMode;											// Radio chat mode (default:0 한글, 1영문)
	REAL32			m_fVolBGM;
	REAL32			m_fVolEffect;	
	
	// ----- Control -----	
	GAME_KEY_STRUCT	m_BaseKeyMapArray[GAME_KEY_MAP_COUNT];
	GAME_KEY_STRUCT	m_KeyMapArray[GAME_KEY_MAP_COUNT];
	bool			m_bEnableMouseInvert;									// 마우스 반전
	REAL32			m_rMouseSensivity;										// 마우스 민감도
	REAL32			m_fFovValue;											// 시야각
	
	// ----- Community -----
	INT32			m_nChatType;											// 채팅
	INT32			m_nWhisperType;											// 귓속말
	INT32			m_nInviteType;											// 초대 메시지
	INT32			m_nFriendType;											// 친구 요청 메시지
	INT32			m_nMessageType;											// 쪽지 수신
	INT32			m_nClanType;											// 클랜 가입 권유

	// ----- Macro -----
	i3::rc_wstring	m_wstrMacro[MACRO_MAX_COUNT];							// 채팅용 매크로
	bool			m_bMacroEnableFriend[MACRO_MAX_COUNT];					// 채팅용 매크로 플래그	

	bool			m_bWrongEnvSet;

	//
	bool			m_bEnableLaserSight;

protected:
	bool				m_bLoadDefault;
	i3::rc_string		m_EnvFileName;

public:
	void				LoadDefault(void);
	void				LoadDefaultSave(void);
	GAME_KEY_STRUCT *	getGameKeyMap(void)	{ return m_KeyMapArray;		}

	void				InitBaseGameKeyMap(void);
	void				InitGameKeyMap(void);
	void				SetGameKeyMap(GAME_KEY_MAP map);
	const GAME_KEY_STRUCT& GetDefaultGameKeyMap(GAME_KEY_MAP map) { return m_BaseKeyMapArray[map]; }

	void				Load(void);
	void				Save(void);
	void				Net_Load(void);
	void				Net_Save(void);
	void				AddFlag(UINT32* Target, UINT32 nFlag)			{ *Target |= nFlag;	}
	void				AddValue( UINT32* Target, INT32 Val, INT32 Where);
	void				GetValue( UINT32 Target, INT32* Val, UINT32 Mask, INT32 Where);
	void				GetValue( UINT32 Target, REAL32* Val, UINT32 Mask, INT32 Where);

	void				CheckKeyMap(void);
	void				UpdateKeyMap(void);	
	const i3::fixed_string&	GetCrosshairName() const;
	const i3::fixed_string&	GetCrosshairName(INT32 nType) const;

	INT32				GetDefaultNation(void) const					{ return m_nDefaultNation;}
	void				SetDefaultNation(INT32 idx)						{ m_nDefaultNation = idx;  }

	UI_FRAMEWORK_VERSION GetUIVersion(void) const						{ return m_nUIVersion; }
	inline bool			IsV2Version(void)								{ return (m_nUIVersion == UFV_1_5); }

	inline bool			IsBaseOnUIVersion_1_0(void)						{ return (m_nUIVersion == UFV_1_0 || m_nUIVersion == UFV_1_2); }

	inline void			SetUIVersion(INT32 version)								
	{ 
		m_nUIVersion = (UI_FRAMEWORK_VERSION)version;
		if(m_nUIVersion == UFV_NON)
		{
			switch (m_nLanguage)
			{
			case RESOURCE_LANGUAGE_SINGMAL:
				m_nUIVersion = UFV_1_5;
				break;
			case RESOURCE_LANGUAGE_PHILIPPINES:
			case RESOURCE_LANGUAGE_BRAZIL:
			case RESOURCE_LANGUAGE_LATIN_AMERICA:
			case RESOURCE_LANGUAGE_NORTHAMERICA:
			case RESOURCE_LANGUAGE_THAILAND:
			case RESOURCE_LANGUAGE_ITALY:
			case RESOURCE_LANGUAGE_ENGLISH:
			case RESOURCE_LANGUAGE_VIETNAM:
			case RESOURCE_LANGUAGE_JAPAN:
				m_nUIVersion = UFV_1_1;
				break;
			case RESOURCE_LANGUAGE_RUSSIA:
				m_nUIVersion = UFV_1_2;
				break;
			case RESOURCE_LANGUAGE_DEV:
			case RESOURCE_LANGUAGE_KOREA:
			case RESOURCE_LANGUAGE_INDONESIA:
			case RESOURCE_LANGUAGE_TURKEY:
			case RESOURCE_LANGUAGE_MIDDLE_EAST:
			case RESOURCE_LANGUAGE_TAIWAN:
			default:
				m_nUIVersion = UFV_1_5;
			}
		}
		else
		{
			if (m_nUIVersion == UFV_1_5)	m_nGameInterface = HUD::HS_1_5;
			else							m_nGameInterface = HUD::HS_1_0;
		}
	}

	// V10 & V11은 항상 상대방 파츠를 표시. V15는 옵션 설정 가능.
	inline bool			isDisplayOtherParts(void)	{ return (IsV2Version() == true) ? (m_bDisplayOtherParts == true) : true; }

	// V10 & V11은 항상 진영 자유로 표시. V15는 옵션 설정 가능.
	inline bool			isFixedViewCharacter(void)	{ return (IsV2Version() == true) ? (m_bFixedViewCharacter == true) : false; }

	static void			GetVideoInfoString( INT32 width, INT32 height, i3::wstring& out);
	static void			AdjustDefault( i3VideoInfo * pInfo);

	static void			_LoadDefaultForKorea(CEnvSet& envSet);
	static void			_LoadDefaultForJapan(CEnvSet& envSet);
	static void			_LoadDefaultForRussia(CEnvSet& envSet);
};

extern CEnvSet * g_pEnvSet;

#endif //_ENVSET_H__