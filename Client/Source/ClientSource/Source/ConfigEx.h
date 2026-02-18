#ifndef __CONFIG_EX_H__
#define __CONFIG_EX_H__

#include "../../../Common/CommonSource/QuestDef.h"

#define	MAX_PING_LEVEL			5
#define MAX_TWITCH_SERVER		4

struct	EndurancePenalty
{
	INT32		Max = 0;
	INT32		Min = 0;
	REAL32		Damage = 0.0f;
	REAL32		HorzRecoil = 0.0f;
	REAL32		VertRecoil = 0.0f;
	REAL32		Deviation = 0.0f;
};

struct	ConfigGame
{
	// 내구도 0인 무기에 적용되는 수치
	REAL32	BrokenWeaponDecreaseDamage = 0.0f;		//	무기 데미지
	REAL32	BrokenWeaponVertRecoil = 0.0f;			//	수직 반동
	REAL32	BrokenWeaponHorzRecoil = 0.0f;			//	수평 반동
	bool	Enable_RPG7 = false;					// RPG7 게임 옵션을 활성화 하는지. ( OPTION_LIMIT_RPG7_ENABLE 에서 변경됨.)
	bool	UnlimitedForRPG7 = false;				// RPG7 게임 모든 맵 플레이가능. Enable_RPG7가 true 되어야 한다. (태국 요청으로 모든 미션/맵에 적용됨)
	bool	CheckMapData = false;					// 게임시 맵을 로딩할 때 맵데이터를 체크하는지. UserFileList.dat가 반드시 있어야함.
	INT32	EndurPenaltyCount = 0;
	bool	CheckMapCollision = false;
	EndurancePenalty*	pEndurancePenalty = nullptr;

	REAL32	LadderPenaltyRate_CrossHairSize = 0.0f;
	REAL32	LadderPenaltyRate_Deviation = 0.0f;
};


struct	ConfigPhysix
{
	REAL32	HeightDecal = 0.0f;
	REAL32	HeightChara = 0.0f;
	REAL32	HeightEquip = 0.0f;
	REAL32	HeightWeapon = 0.0f;
	REAL32	HeightThrow = 0.0f;
	REAL32	HeightBomb = 0.0f;
};


struct	ConfigClan
{
	i3::rc_wstring	AzitAddressBase;		//	클랜 아지트 URL 주소
	bool	Enable = false;
	bool	Enable_Battle = false;
	bool	Enable_Cancellation = false;
	bool	Enable_Search = false;
	bool	Enable_Chat = false;
	INT32	Enable_AdmissionForAge = 0;		// 클랜 가입 나이 제한
};


struct	ConfigGUI
{
	INT32	CustomClanMarkContentCount = 0, CustomClanMarkFrameCount = 0;
	INT32	DefaultTextSpaceX = 0, DefaultTextSpaceY = 0;
	REAL32	DefaultTextOffsetX = 0.0f, DefaultTextOffsetY = 0.0f;
	I3COLOR	DurabilityBarColorInSlot = { 0, 0, 0, 0 };
	bool	Enable_LoadProgressBar = false;
	REAL32	JackpotNoticeLifeTime = 0.0f;

	i3::vector<INT32>	pGaugeLevel_ServerList;
	i3::vector<INT32>	pGaugeLevel_ChannelList;

	typedef		pair<SChannelType, INT32>	ServerTypeIndex;
	typedef		vector<ServerTypeIndex>		ServerTypeIndexContainer;
	ServerTypeIndexContainer *	pServerListTab = nullptr;
};


struct	ConfigMisc
{
public:
	struct	WebAnnounce
	{
		bool	IsActive() const	{return ! URL.empty();}
		i3::rc_wstring	URL;
		REAL32	posX = 0.0f, posY = 0.0f;
		REAL32	sizeWidth = 0.0f, sizeHeight = 0.0f;
		INT32	msgBoxOffsetX = 0, msgBoxOffsetY = 0;
	};

	INT32	AdultAge = 0;			// 성인 나이
	i3::rc_wstring	JoinAddress;	// 가입하기 주소
	i3::rc_wstring	FindIDAddress;	// ID찾기 주소
	i3::rc_wstring	LobbyBGM;
	REAL32	AnnounceFlipTime = 0.0f;	// 이미지 공지 순환 시간
	REAL32	AnnounceWaitingTimeAfterClicked = 0.0f;		//	유저가 이미지 공지를 클릭했을 경우 다시 순환까지 대기 시간

	WebAnnounce	webAnnounce;
};

struct	ConfigNetwork
{
	// 로그인 전까지의 대기 시간. 게임실행 후 사용자 입력이
	// 있더라도 로그인을 하지 않으면 강제종료를 하는 시간.
	REAL32		GameRunWaitTime = 0.0f;
	REAL32		LogInWaitTime_Sec = 0.0f;
	REAL32		RefreshInterval_ServerList = 0.0f;
	REAL32		RefreshInterval_RoomList = 0.0f;
	REAL32		RefreshInterval_RoomInfo = 0.0f;
	INT32		ServerReconnectCount = 0;
	REAL32		PingLevelCheckValue[MAX_PING_LEVEL] = { 0.0f };	//	디스플레이에서 보이는 핑 레벨 기준을 설정합니다.
};

struct	ConfigShop
{	
	struct	Tab {	bool	Enable = false;  	};

	bool	Enable = false;
	bool	Enable_ChargeCash = false;
	bool	ChargeCash_Preparing = false;
	bool	ChargeCash_InGame = false;
	INT32	ChargeCash_InGameSizeWidth = 0;
	INT32	ChargeCash_InGameSizeHeight = 0;
	i3::rc_wstring	ChargeURL;
	i3::rc_wstring	ChargeURL2;
	i3::rc_wstring	ChargeURL_QA;		// QA용 주소
	i3::rc_wstring  ChargeURL2_QA;		// QA용 주소
	Tab		Item;
	Tab		Set;
};

struct	ConfigCommunity
{
	bool		Enable_Note = false;
};

struct ConfigTwitchTV
{
	i3::rc_wstring Server0;
	i3::rc_wstring Server1;
	i3::rc_wstring Server2;
	i3::rc_wstring Server3;
};

/*	채널에 입장 가능한 자격 구조체
	ConfigEx.pef에 Logic/ChannelCondition 에 등록 되어 있음

	CONDITION_ENUM	:	입장 자격 종류
		KILLDEATH	:	킬데스
		RANK	:	계급	(enum RANK_COUNT 참조)
	ChannelType	:	채널 종류
	value_min :	최소 자격(최소 포함)
	value_max : 최대 자격(최대 포함)	주위 : -1일 경우 최대값을 이야기함
*/
struct	ConfigLogic
{
	struct	ChannelConditionInfo
	{
		enum CONDITION_ENUM { INVALID, KILLDEATH, RANK };

		ChannelType		type = CHANNEL_TYPE_NONE;
		CONDITION_ENUM	condition = INVALID;
		INT32			value_min = 0, value_max = -1;
	};

	typedef		vector<ChannelConditionInfo>			ChannelConditionContainer;

	const ChannelConditionInfo *	GetChannelConditionInfo(ChannelType type) const;



	ChannelConditionContainer	ChannelConditions;
};

class CConfigEx :	public i3ElementBase
{
	I3_CLASS_DEFINE( CConfigEx, i3ElementBase );

public:
	CConfigEx(void);
	~CConfigEx(void);


private:
	void	Init();


public:
	bool	Load();
	
	const ConfigGame &		GetGame()				{return m_ConfigGame;}
	const ConfigPhysix &	GetPhysix()				{return m_ConfigPhysix;}
	const ConfigClan &		GetClan()				{return m_ConfigClan;}
	const ConfigGUI &		GetGUI()				{return m_ConfigGUI;}
	const ConfigMisc &		GetMisc()				{return m_ConfigMisc;}
	const ConfigNetwork &	GetNetwork()			{return m_ConfigNetwork;}
	const ConfigShop &		GetShop()				{return m_ConfigShop;}
	const ConfigCommunity &	GetCommunity()			{return m_ConfigCommunity;}
	const ConfigLogic &		GetLogic()				{return m_ConfigLogic;}
	const ConfigTwitchTV &  GetTwitchTV()			{return m_ConfigTwitchTV;}
	char *					GetIP()					{return m_strLabel;}

	void SetIP(char *);
	void SetCardSetComboBox( UINT8 ui8ActiveQuestCount, UINT32*	pui8ActiveQuest );

	INT32				GetEndurancePenaltyNum( UINT8 Endurance );
	void				GetEndurancePenaltyName( UINT8 Endurance, i3::string& Label );		// 게임 스트링을 라벨을 리턴해준다
	EndurancePenalty*	GetEndurancePenalty( UINT8 Endurance );

	REAL32				GetLadderPenaltyRate_CrossHairSize() const;
	REAL32				GetLadderPenaltyRate_Deviation() const;


private:
	ConfigGame		m_ConfigGame;
	ConfigPhysix	m_ConfigPhysix;
	ConfigClan		m_ConfigClan;
	ConfigGUI		m_ConfigGUI;
	ConfigMisc		m_ConfigMisc;
	ConfigNetwork 	m_ConfigNetwork;
	ConfigShop		m_ConfigShop;
	ConfigCommunity	m_ConfigCommunity;
	ConfigLogic		m_ConfigLogic;
	ConfigTwitchTV  m_ConfigTwitchTV;
	
	char			m_strLabel[512];
	UINT8			m_uiEncryptKey;		// 일단 사다리 페널티용도로사용..
};


#ifdef USE_I3EXEC
extern "C" __declspec( dllexport) CConfigEx *			g_pConfigEx;
#else
extern CConfigEx* g_pConfigEx;
#endif



#ifdef NC_BUILD
#ifdef USE_I3EXEC
	//외부 사용
	extern "C" void __declspec( dllexport) __ProhibitFile_Install();
	//extern "C" void __declspec( dllexport) __ProhibitFile_GetFileMD5();
	extern "C" void __declspec( dllexport) __ProhibitFile_GetUserFileListMD5();
#else
	void __ProhibitFile_Install();
	void __ProhibitFile_GetUserFileListMD5();
#endif
	//내부 사용
	void __ProhibitFile_CheckOut();
	void __ProhibitFile_Unstall();
	void __ProhibitFile_AreYouCheating();
#endif
 
#ifdef USE_I3EXEC
	extern "C" void __declspec( dllexport) _setLogName_(LPCSTR logName);
#else
	void _setLogName_(LPCSTR logName);
#endif

	LPCSTR getLogName();


#endif	//__CONFIG_EX_H__

