#include "pch.h"
#include "ConfigEx.h"

#include "CoreFileChecker.h"
#include "i3Base/string/ext/format_string.h"

CConfigEx* g_pConfigEx = 0;


const ConfigLogic::ChannelConditionInfo * ConfigLogic::GetChannelConditionInfo( ChannelType type ) const
{
	struct	ChannelConditionType
	{
	public:
		ChannelConditionType(ChannelType _type) : type(_type) {}
		bool operator()(const ChannelConditionInfo & target)
		{
			return (type == target.type);
		}
		ChannelType type;
	};

	ChannelConditionContainer::const_iterator ret = find_if(ChannelConditions.begin(), ChannelConditions.end(), ChannelConditionType(type));
	if ( ret != ChannelConditions.end() )
	{
		return &(*ret);
	}
	return nullptr; 
}

//I3_CLASS_INSTANCE( CConfigEx, i3ElementBase);
I3_CLASS_INSTANCE( CConfigEx);

CConfigEx::CConfigEx(void)
{
	Init();
}


CConfigEx::~CConfigEx(void)
{
	I3_SAFE_DELETE_ARRAY( m_ConfigGame.pEndurancePenalty );
	I3_SAFE_DELETE(m_ConfigGUI.pServerListTab);
}


void	CConfigEx::Init()
{
	I3_SAFE_DELETE_ARRAY( m_ConfigGame.pEndurancePenalty );
	i3mem::FillZero(&m_ConfigGame, sizeof(m_ConfigGame));
	i3mem::FillZero(&m_ConfigPhysix, sizeof(m_ConfigPhysix));

	i3mem::FillZero(&m_ConfigNetwork, sizeof(m_ConfigNetwork));
//	i3mem::FillZero(&m_ConfigShop, sizeof(m_ConfigShop));

	i3mem::FillZero(&m_ConfigCommunity, sizeof(m_ConfigCommunity));

	i3mem::FillZero( m_strLabel, sizeof( m_strLabel ) );


	m_ConfigGame.LadderPenaltyRate_CrossHairSize = 3.f;		// pef 읽기실패를 감안해서 일단 이렇게 지정해봄..
	m_ConfigGame.LadderPenaltyRate_Deviation     = 3.f;

	m_uiEncryptKey = (UINT8)(i3Math::Rand() % 7 + 1);
}

#define		CHECK_FIND_KEY(KeyName, pKey)	\
	{\
		pKey = FindKey(KeyName);\
		if ( pKey == nullptr )\
		{\
			I3PRINTLOG(I3LOG_FATAL, "can't found '%s' key", KeyName);\
			return false;\
		}\
	}\


REAL32	CConfigEx::GetLadderPenaltyRate_CrossHairSize() const
{
#if defined( MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue(&m_ConfigGame.LadderPenaltyRate_CrossHairSize, m_uiEncryptKey);
#else
	return m_ConfigGame.LadderPenaltyRate_CrossHairSize;
#endif
}

REAL32	CConfigEx::GetLadderPenaltyRate_Deviation() const
{
#if defined(MEM_ENCRYPTION_LV2)
	return GlobalFunc::DecryptValue(&m_ConfigGame.LadderPenaltyRate_Deviation, m_uiEncryptKey);
#else
	return m_ConfigGame.LadderPenaltyRate_Deviation;
#endif
}


bool	CConfigEx::Load()
{
	Init();

	i3RegKey * pPef = Config::_GetPefFileRegKey("ConfigEx");
	i3RegKey * pChildKey = nullptr;
	i3RegKey * pKey = nullptr;

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Game", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "BrokenWeaponDecreaseDamage",	&m_ConfigGame.BrokenWeaponDecreaseDamage);
		GetKeyData(pKey, "BrokenWeaponVertRecoil",		&m_ConfigGame.BrokenWeaponVertRecoil);
		GetKeyData(pKey, "BrokenWeaponHorzRecoil",		&m_ConfigGame.BrokenWeaponHorzRecoil);
		GetKeyData(pKey, "Enable_RPG7",					&m_ConfigGame.Enable_RPG7);		
		GetKeyData(pKey, "UnlimitedForRPG7",			&m_ConfigGame.UnlimitedForRPG7);
		GetKeyData(pKey, "CheckMapData",				&m_ConfigGame.CheckMapData);
		GetKeyData(pKey, "EndurPenaltyCount",				&m_ConfigGame.EndurPenaltyCount);
		GetKeyData(pKey, "CheckMapCollision",				&m_ConfigGame.CheckMapCollision);
		
		// 사다리 CrossHair / Deviation 벌칙 Rate 적용 값 여기서 처리 (2015.01.06.수빈)
		GetKeyData(pKey, "LadderPenaltyRate_CrossHairSize", &m_ConfigGame.LadderPenaltyRate_CrossHairSize);
		
		if ( m_ConfigGame.LadderPenaltyRate_CrossHairSize == 0.f)	// 못읽으면 3으로 
			m_ConfigGame.LadderPenaltyRate_CrossHairSize = 3.f;

#if defined(MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue(&m_ConfigGame.LadderPenaltyRate_CrossHairSize, m_uiEncryptKey);
#endif
		GetKeyData(pKey, "LadderPenaltyRate_Deviation", &m_ConfigGame.LadderPenaltyRate_Deviation);
		
		if (m_ConfigGame.LadderPenaltyRate_Deviation == 0.f)		// 못읽으면 3으로 
			m_ConfigGame.LadderPenaltyRate_Deviation = 3.f;

#if defined(MEM_ENCRYPTION_LV2)
		GlobalFunc::EncryptValue(&m_ConfigGame.LadderPenaltyRate_Deviation, m_uiEncryptKey);
#endif
		//

		I3_SAFE_DELETE_ARRAY( m_ConfigGame.pEndurancePenalty );
		m_ConfigGame.pEndurancePenalty		= new EndurancePenalty[ m_ConfigGame.EndurPenaltyCount ];
		MEMDUMP_NEW( m_ConfigGame.pEndurancePenalty, sizeof(EndurancePenalty) * m_ConfigGame.EndurPenaltyCount);

		for ( INT32 i = 0 ; i < m_ConfigGame.EndurPenaltyCount; ++i )
		{
			char				szKey[ MAX_STRING_COUNT ];
			EndurancePenalty*	pPenalty	= &m_ConfigGame.pEndurancePenalty[ i ];

			i3::snprintf( szKey, MAX_STRING_COUNT, _T("EndurancePenalty%d"), i);
			pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, szKey);

			GetKeyData(pChildKey, "Max", &pPenalty->Max);
			GetKeyData(pChildKey, "Min", &pPenalty->Min);
			GetKeyData(pChildKey, "Damage", &pPenalty->Damage);
			GetKeyData(pChildKey, "HorzRecoil", &pPenalty->HorzRecoil);
			GetKeyData(pChildKey, "VertRecoil", &pPenalty->VertRecoil);
			GetKeyData(pChildKey, "Deviation", &pPenalty->Deviation);
		}
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Physix", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "HeightDecal",	&m_ConfigPhysix.HeightDecal);
		GetKeyData(pKey, "HeightChara",	&m_ConfigPhysix.HeightChara);
		GetKeyData(pKey, "HeightEquip",	&m_ConfigPhysix.HeightEquip);
		GetKeyData(pKey, "HeightWeapon",	&m_ConfigPhysix.HeightWeapon);
		GetKeyData(pKey, "HeightThrow",	&m_ConfigPhysix.HeightThrow);
		GetKeyData(pKey, "HeightBomb",	&m_ConfigPhysix.HeightBomb);
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Clan", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "AzitAddressBase",				m_ConfigClan.AzitAddressBase );
		if( i3::generic_is_iequal( m_ConfigClan.AzitAddressBase, L"default" )  )
		{
			I3PRINTLOG(I3LOG_FATAL, "클랜 홈페이지 주소를 설정해야 됩니다. [ConfigEx.pef - Clan]");
		}

		GetKeyData(pKey, "Enable",						&m_ConfigClan.Enable);
		GetKeyData(pKey, "Enable_Battle",				&m_ConfigClan.Enable_Battle);
		GetKeyData(pKey, "Enable_Cancellation",			&m_ConfigClan.Enable_Cancellation);
		GetKeyData(pKey, "Enable_Search",				&m_ConfigClan.Enable_Search);
		GetKeyData(pKey, "Enable_Chat",					&m_ConfigClan.Enable_Chat);
		GetKeyData(pKey, "Enable_AdmissionForAge",		&m_ConfigClan.Enable_AdmissionForAge);
		if( m_ConfigClan.Enable_AdmissionForAge == 1234 )
		{
			I3PRINTLOG(I3LOG_FATAL, "클랜 가입 나이를 설정해야 됩니다. [ConfigEx.pef - Clan]");
		}
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("GUI", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "CustomClanMarkContentCount",	&m_ConfigGUI.CustomClanMarkContentCount);
		GetKeyData(pKey, "CustomClanMarkFrameCount",	&m_ConfigGUI.CustomClanMarkFrameCount);
		GetKeyData(pKey, "DefaultTextSpaceX",			&m_ConfigGUI.DefaultTextSpaceX);
		GetKeyData(pKey, "DefaultTextSpaceY",			&m_ConfigGUI.DefaultTextSpaceY);
		GetKeyData(pKey, "DefaultTextOffsetX",			&m_ConfigGUI.DefaultTextOffsetX);
		GetKeyData(pKey, "DefaultTextOffsetY",			&m_ConfigGUI.DefaultTextOffsetY);
		GetKeyData(pKey, "DurabilityBarColorInSlot",	&m_ConfigGUI.DurabilityBarColorInSlot);
		GetKeyData(pKey, "Enable_LoadProgressBar",		&m_ConfigGUI.Enable_LoadProgressBar);
		GetKeyData(pKey, "JackpotNoticeLifeTime",		&m_ConfigGUI.JackpotNoticeLifeTime);

		TCHAR	LevelName[MAX_STRING_COUNT] = {};
		INT32	DataCount	= 0;

		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "GaugeLevelServerList");
		I3ASSERT(pChildKey != nullptr);

		DataCount = pChildKey->GetDataCount();
		m_ConfigGUI.pGaugeLevel_ServerList.clear();
		m_ConfigGUI.pGaugeLevel_ServerList.resize(DataCount);

		for ( INT32 i = 0 ; i < DataCount ; ++i )
		{
			i3::snprintf(LevelName, MAX_STRING_COUNT, _T("Level%d"), i+1);
			INT32	Level = 0;
			GetKeyData(pChildKey, LevelName, &Level);
			m_ConfigGUI.pGaugeLevel_ServerList[i] = Level;
		}


		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "GaugeLevelChannelList");
		I3ASSERT(pChildKey != nullptr);

		DataCount = pChildKey->GetDataCount();
		m_ConfigGUI.pGaugeLevel_ChannelList.clear();
		m_ConfigGUI.pGaugeLevel_ChannelList.resize(DataCount);

		for ( INT32 i = 0 ; i < DataCount ; ++i )
		{
			i3::snprintf(LevelName, MAX_STRING_COUNT, _T("Level%d"), i+1);
			INT32	Level = 0;
			GetKeyData(pChildKey, LevelName, &Level);
			m_ConfigGUI.pGaugeLevel_ChannelList[i] = Level;
		}

		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "ServerListTab");
		I3ASSERT(pChildKey != nullptr);

		m_ConfigGUI.pServerListTab = new ConfigGUI::ServerTypeIndexContainer();
		MEMDUMP_NEW( m_ConfigGUI.pServerListTab, sizeof(ConfigGUI::ServerTypeIndexContainer));
		m_ConfigGUI.pServerListTab->resize(S_CHANNEL_TYPE_MAX);

		//ConfigEx.pef 파일에 등록된 이름을 적어 주면 된다.
		LPCTSTR TabList[S_CHANNEL_TYPE_MAX] = 
		{
			"All",
			"Normal",
			"Normal2",
			"Begin",
			"Clan",
			"Expert",
			"Combination",
			"Championship",
			"Nation",
			"WarnetSilver",	// revision 40858
			"WarnetGold",
			"WarnetClan",
			"Azerbaijan"
		};
		for ( int i = 0 ; i < _countof(TabList) ; ++i )
		{
			I3ASSERT( TabList[i] != nullptr );
			if ( TabList[i] == nullptr)
				continue;

			INT32 Index = -1;
			GetKeyData(pChildKey, TabList[i], &Index);

			m_ConfigGUI.pServerListTab->at(i) = ConfigGUI::ServerTypeIndex(static_cast<SChannelType>(i), Index);
		}

		struct ServerTabSort
		{
			bool operator()(const ConfigGUI::ServerTypeIndex & left, const ConfigGUI::ServerTypeIndex & right) const
			{
				return left.second < right.second;
			}
		};

		// 서버탭을 순서대로 정렬한다.
		std::sort(m_ConfigGUI.pServerListTab->begin(), m_ConfigGUI.pServerListTab->end(), ServerTabSort());
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Misc", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "AdultAge",		&m_ConfigMisc.AdultAge);
		if( m_ConfigMisc.AdultAge == 1234 )
		{
			I3PRINTLOG(I3LOG_FATAL, "성인 나이를 설정해야 합니다. [ConfigEx.pef - Misc]");
		}

		GetKeyDataString(pKey, "JoinAddress",		m_ConfigMisc.JoinAddress);
		if( m_ConfigMisc.JoinAddress == L"default" )
		{
			I3PRINTLOG(I3LOG_FATAL, "가입 하기 주소를 설정해야 합니다. [ConfigEx.pef - Mics]");
		}

		GetKeyDataString(pKey, "FindIDAddress", m_ConfigMisc.FindIDAddress);
		if(m_ConfigMisc.FindIDAddress == L"default")
		{
			I3PRINTLOG(I3LOG_FATAL, "ID찾기 주소를 설정해야 합니다. [ConfigEx.pef - Mics]");
		}

		GetKeyDataString(pKey, "LobbyBGM",		m_ConfigMisc.LobbyBGM);
		GetKeyData(pKey, "AnnounceFlipTime",&m_ConfigMisc.AnnounceFlipTime);
		GetKeyData(pKey, "AnnounceWaitingTimeAfterClicked",&m_ConfigMisc.AnnounceWaitingTimeAfterClicked);
		
		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "WebAnnounce");
		if ( pChildKey )
		{
			GetKeyDataString(pChildKey,	"URL",			m_ConfigMisc.webAnnounce.URL);
			GetKeyData(pChildKey,	"posX",				&m_ConfigMisc.webAnnounce.posX);
			GetKeyData(pChildKey,	"posY",				&m_ConfigMisc.webAnnounce.posY);
			GetKeyData(pChildKey,	"sizeWidth",		&m_ConfigMisc.webAnnounce.sizeWidth);
			GetKeyData(pChildKey,	"sizeHeight",		&m_ConfigMisc.webAnnounce.sizeHeight);
			GetKeyData(pChildKey,	"msgBoxOffsetX",	&m_ConfigMisc.webAnnounce.msgBoxOffsetX);
			GetKeyData(pChildKey,	"msgBoxOffsetY",	&m_ConfigMisc.webAnnounce.msgBoxOffsetY);
		}
	}

	

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Network", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "GameRunWaitTime",				&m_ConfigNetwork.GameRunWaitTime);
		GetKeyData(pKey, "LogInWaitTime_Sec",			&m_ConfigNetwork.LogInWaitTime_Sec);
		GetKeyData(pKey, "RefreshInterval_ServerList",	&m_ConfigNetwork.RefreshInterval_ServerList);
		GetKeyData(pKey, "RefreshInterval_RoomList",	&m_ConfigNetwork.RefreshInterval_RoomList);
		GetKeyData(pKey, "RefreshInterval_RoomInfo",	&m_ConfigNetwork.RefreshInterval_RoomInfo);
		GetKeyData(pKey, "ServerReconnectCount",		&m_ConfigNetwork.ServerReconnectCount);

		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "Ping");

		char	DataName[256] = {};
		for ( INT32 i = 0 ; i < MAX_PING_LEVEL ; ++i )
		{
			StringCchPrintf(DataName, 256, "Level_%d", i+1);
			GetKeyData(pChildKey, DataName, &m_ConfigNetwork.PingLevelCheckValue[i]);

			if( m_ConfigNetwork.PingLevelCheckValue[i] == -1.0f )
			{
				I3PRINTLOG(I3LOG_FATAL, "%d 핑을 설정해야 됩니다. [ConfigEx.pef - Network]", i+1);
			}
		}
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Shop", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "Enable",						&m_ConfigShop.Enable);
		GetKeyData(pKey, "Enable_ChargeCash",			&m_ConfigShop.Enable_ChargeCash);
		GetKeyData(pKey, "ChargeCash_Preparing",		&m_ConfigShop.ChargeCash_Preparing);
		GetKeyData(pKey, "ChargeCash_InGame",			&m_ConfigShop.ChargeCash_InGame);
		GetKeyData(pKey, "ChargeCash_InGameSizeWidth",	&m_ConfigShop.ChargeCash_InGameSizeWidth);
		GetKeyData(pKey, "ChargeCash_InGameSizeHeight",	&m_ConfigShop.ChargeCash_InGameSizeHeight);

		GetKeyData(pKey, "ChargeURL",					m_ConfigShop.ChargeURL);
		if( i3::generic_is_iequal( m_ConfigShop.ChargeURL, L"default" )  )
		{
			I3PRINTLOG(I3LOG_FATAL, "캐쉬충전 URL 주소를 설정해야 됩니다. [ConfigEx.pef - Shop]");
		}

		GetKeyData(pKey, "ChargeURL2",					m_ConfigShop.ChargeURL2 );
		GetKeyData(pKey, "ChargeURL_QA",				m_ConfigShop.ChargeURL_QA );
		if( i3::generic_is_iequal( m_ConfigShop.ChargeURL_QA, L"default" )  )
		{
			I3PRINTLOG(I3LOG_FATAL, "캐쉬충전 URL 주소를 설정해야 됩니다. [ConfigEx.pef - Shop]");
		}
		GetKeyData(pKey, "ChargeURL2_QA",				m_ConfigShop.ChargeURL2_QA );

		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "Tab_Item");
		GetKeyData(pChildKey, "Enable", &m_ConfigShop.Item.Enable);

		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "Tab_Set");
		GetKeyData(pChildKey, "Enable", &m_ConfigShop.Set.Enable);
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Community", pPef );
	if( pKey != nullptr)
	{
		GetKeyData(pKey, "Enable_Note",				&m_ConfigCommunity.Enable_Note);
	}
	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("TwitchTV", pPef );
	if( pKey != nullptr)
	{
		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "EnableSelectServer");
		GetKeyData(pChildKey, "Server0", m_ConfigTwitchTV.Server0);
		GetKeyData(pChildKey, "Server1", m_ConfigTwitchTV.Server1);
		GetKeyData(pChildKey, "Server2", m_ConfigTwitchTV.Server2);
		GetKeyData(pChildKey, "Server3", m_ConfigTwitchTV.Server3);
	}

	//----------------------------------------------------------------------------------
	pKey = Config::FindKey("Logic", pPef );
	if( pKey != nullptr)
	{
		pChildKey = (i3RegKey *)i3TreeNode::FindNodeByName(pKey, "ChannelCondition");

		typedef		std::pair<ChannelType, LPCTSTR>	ChannelTypeName;
		ChannelTypeName	ChannelList[] = 
		{
			ChannelTypeName(CHANNEL_TYPE_BEGIN1,	_T("Begin1")),
			ChannelTypeName(CHANNEL_TYPE_BEGIN2,	_T("Begin2")),
			ChannelTypeName(CHANNEL_TYPE_EXPERT,	_T("Expert1")),
			ChannelTypeName(CHANNEL_TYPE_EXPERT2,	_T("Expert2")),
			ChannelTypeName(CHANNEL_TYPE_MIDDLE,	_T("Middle")),
		};

		for ( INT32 a = 0 ; a < pChildKey->getChildCount() ; ++a )
		{
			i3RegKey * pCondition = (i3RegKey *)pChildKey->getChild(a);
			for ( INT32 b = 0 ; b < _countof(ChannelList) ; ++b )
			{
				if ( i3::generic_is_iequal(pCondition->GetName(), ChannelList[b].second) )
				{
					ConfigLogic::ChannelConditionInfo	info;
					info.type	= ChannelList[b].first;

					i3::rc_wstring condition;
					pCondition->GetData(_T("condition"), condition );

					if ( i3::generic_is_iequal(condition, L"killdeath_rate") )
						info.condition	= ConfigLogic::ChannelConditionInfo::KILLDEATH;
					else if ( i3::generic_is_iequal(condition, L"rank") )
						info.condition	= ConfigLogic::ChannelConditionInfo::RANK;

					pCondition->GetData(_T("value_min"), &info.value_min);
					pCondition->GetData(_T("value_max"), &info.value_max);

					if( (UINT32)info.value_min >= (UINT32)info.value_max )
					{
						//min은 max 보다 작아야 한다.
						I3PRINTLOG(I3LOG_FATAL, "[ %s ]채널 입장 가능한 계급[ min : %d - max : %d ] 이 잘 못 되어 있다.", 
							pCondition->GetName(),
							info.value_min, 
							info.value_max); 
					}
					else
					{
						m_ConfigLogic.ChannelConditions.push_back(info);
					}
				}
			}
		}
	}

	return true;
}

void CConfigEx::SetIP(char *m_IP)
{
	i3::safe_string_copy( m_strLabel,m_IP, 512 );
}


INT32 CConfigEx::GetEndurancePenaltyNum( UINT8 Endurance )
{
	EndurancePenalty*	pPenalty	= GetGame().pEndurancePenalty;
	if( pPenalty == nullptr ) return -1;

	INT32				nCount		= GetGame().EndurPenaltyCount;

	for( INT32 i = 0; i < nCount; i++ )
	{
		if( Endurance <= pPenalty[i].Max
			&& Endurance >= pPenalty[i].Min )
		{
			return i;
		}
	}

	return -1;
}
void CConfigEx::GetEndurancePenaltyName( UINT8 Endurance, i3::string& Label )
{
	INT32	Index = GetEndurancePenaltyNum( Endurance );

	if( Index >= 0 )
	{
		Label	= i3::format_string("%s%d", "STR_POP_REPAIR_PENALTY_", Index);
	}
	else
	{
		Label	= "STR_POP_REPAIR_PENALTY_NOTHING";
	}

	return;
}
EndurancePenalty* CConfigEx::GetEndurancePenalty( UINT8 Endurance )
{
	INT32	Index	= GetEndurancePenaltyNum( Endurance );

	if( Index < 0 ) return nullptr;

	return &GetGame().pEndurancePenalty[ Index ];
}



#ifdef NC_BUILD

#if 0
struct ProhibitFileContext
{
	std::string Name;
	unsigned char Md5[16];
};
static i3::vector< ProhibitFileContext > g_ProhibitFileList;
static bool g_IsCheatUser = false;
static bool g_UseLauncher = false;
#endif

void _InsertlProhibitFile(const char* FileName)
{
#if 0
	ProhibitFileContext Context;

	memset(Context.Md5, 0, sizeof( char ) * 16 );
	Context.Name = FileName;

	g_ProhibitFileList.push_back( Context );
#endif
}

//void __ProhibitFile_GetFileMD5()
//{
//	//파일 md5로 비교하는 방법은 문제가 잇는 방법이라서 잠시 막아 놓았음
//	return;
//
//	size_t size = g_ProhibitFileList.size();
//
//	for( size_t i = 0; i < size; ++i )
//	{
//		md5_file( (char*)g_ProhibitFileList[i].Name.c_str(), g_ProhibitFileList[i].Md5 );
//	}
//}

void __ProhibitFile_GetUserFileListMD5()
{
#if 0
	if( !Config::IsFileExist( "UserFileList.dat" ) )
	{
		g_UseLauncher = false;
		return;
	}
	
	g_UseLauncher = true;

	CCoreFileChecker	Checker;
	Checker.Open();

	size_t size = g_ProhibitFileList.size();

	for( size_t i = 0; i < size; ++i )
	{
		const UINT32* md5 = Checker.GetMd5( g_ProhibitFileList[i].Name.c_str() );

		memcpy(g_ProhibitFileList[i].Md5, md5, sizeof(unsigned char) * 16 );
	}
#endif
}

bool _MatchMD5(unsigned char src_md5[16], unsigned char target_md5[16] )
{
#if 0
	if ( 0 != memcmp(src_md5, target_md5, sizeof(unsigned char) * 16 ) )
	{
		return false;
	}
#endif
	return true;
}

void __ProhibitFile_Install()
{
#if 0
	g_IsCheatUser = false;

	_InsertlProhibitFile("Chara\\SWAT_Male\\SWAT_Male.I3CHR");
	_InsertlProhibitFile("Chara\\SWAT_Female\\SWAT_Female.I3CHR");
	_InsertlProhibitFile("Chara\\REBEL_Male\\REBEL_Male.I3CHR");
	_InsertlProhibitFile("Chara\\REBEL_Female(ViperRed)\\REBEL_ViperRed.I3CHR");
	_InsertlProhibitFile("Chara\\REBEL_Female\\REBEL_Female.I3CHR");
	_InsertlProhibitFile("Chara\\Leopard\\Leopard.I3CHR");
	_InsertlProhibitFile("Chara\\Hide\\Hide.I3CHR");
	_InsertlProhibitFile("Chara\\D-Fox\\D-Fox.I3CHR");
	_InsertlProhibitFile("Chara\\Dino_T-Rex\\Dino_T-Rex.I3CHR");
	_InsertlProhibitFile("Chara\\Dino_Velociraptor\\Dino_Velociraptor.I3CHR");

	_InsertlProhibitFile("Chara\\SWAT_Male\\Chara_SWAT_Male_ShotGun.i3AnimPack");
	_InsertlProhibitFile("Chara\\SWAT_Male\\Chara_SWAT_Male_ShotGun2.i3AnimPack");
	_InsertlProhibitFile("Chara\\SWAT_Male\\Chara_SWAT_Male_ShotGun3.i3AnimPack");
#endif
}

void __ProhibitFile_CheckOut()
{
#if 0
	if( !g_UseLauncher )
		return;

	size_t size = g_ProhibitFileList.size();

	for( size_t i = 0; i < size; ++i )
	{
		unsigned char md5[16];
		memset(md5, 0, sizeof( char ) * 16 );
		md5_file( (char*)g_ProhibitFileList[i].Name.c_str(), md5 );

		//비교해서 다르면 치트 유저다.
		if( !_MatchMD5( g_ProhibitFileList[i].Md5, md5 ) )
		{
			I3PRINTLOG(I3LOG_DEFALUT, "변경 된 파일 [ %s ]", g_ProhibitFileList[i].Name.c_str() );
			g_IsCheatUser = true;
			return;
		}
	}
#endif
}

void __ProhibitFile_Unstall()
{
#if 0
	g_IsCheatUser = false;

	g_ProhibitFileList.clear();
#endif
}

void __ProhibitFile_AreYouCheating()
{
#if 0
	if( g_IsCheatUser )
	{
		HackContext::i()->GoodbyeCheatUser( CC_FILE_FALSIFY );
	}

	__ProhibitFile_Unstall();
#endif
}

#endif

static std::string g_LogName;

void _setLogName_(LPCSTR logName)
{
	g_LogName = logName;
}

LPCSTR getLogName()
{
#if defined(USE_I3EXEC)
	return "PointBlank_ForLevelTool_i3Exec.log";
#else
	return g_LogName.c_str();
#endif
}
