#include "pch.h"
#include "UIPopupSettingRoom.h"

#include "UIPopupSelectMode.h"
#include "UIPopupSelectStage.h"

#include "UIMainFrame.h"
#include "UIUtil.h"

#include "../CommunityContext.h"
#include "../TextSet.h"

enum MAP_MODE
{
	MM_NONE = 0,
	// 단체전
	MM_DEATHMATCH,
	MM_DEATHMATCH_SNIPING,
	MM_DEATHMATCH_SHORTGUN,
	MM_DEATHMATCH_MELEE,
	MM_DEATHMATCH_4VS4,
	MM_DEATHMATCH_CHALLENGE,
	MM_HEADHUNTER,
	MM_HEADKILLER,
	MM_SUDDENDEATH,
	MM_RPG7,
	MM_DEATHMATCH_PISTOL,

	// 특수전
	MM_ANNIHILATION,
	MM_BOMBMISSION,
	MM_CTF,
	MM_DEFENCE,
	MM_DESTROY,
	MM_SABOTAGE,
	MM_VIP,

	// 공룡전
	MM_DINOSAUR_CC,
	MM_DINOSAUR_DOMINATION,
};


static UIPopupSettingRoom * GetThis( void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);

	return (UIPopupSettingRoom*) pFrame->GetPopup( UPW_SETTINGROOM);
}

extern "C"
{
	int UIPopupSettingRoom_ChangeObjectCondition( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeObjectCondition(cur);
		return 0;
	}

	int UIPopupSettingRoom_ChangeTimeLimit( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeTimeLimit( cur);
		return 0;
	}

	int UIPopupSettingRoom_ClickOK( LuaState * L)
	{
		GetThis()->SettingRoomOK();
		return 0;
	}

	int UIPopupSettingRoom_ClickCancel( LuaState * L)
	{
		GetThis()->SettingRoomCancel();
		return 0;
	}

	int UIPopupSettingRoom_ClickSelectMode( LuaState * L)
	{
		GetThis()->ClickSelectMode();
		return 0;
	}

	int UIPopupSettingRoom_ClickSelectStage( LuaState * L)
	{
		GetThis()->ClickSelectStage();
		return 0;
	}

	int UIPopupSettingRoom_ChangeAICount ( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeALCount(cur);
		return 0;
	}

	int UIPopupSettingRoom_ChangeAILevel( LuaState * L)
	{
		INT32 cur = i3Lua::GetIntegerArg( L, 1);
		GetThis()->ChangeAILevel( cur);
		return 0;
	}
}

LuaFuncReg UIPoupSettingRoom_Glue[] = {
	{"ChangeObjectCondition",		UIPopupSettingRoom_ChangeObjectCondition},
	{"ChangeTimeLimit",				UIPopupSettingRoom_ChangeTimeLimit},
	{"pbClickCreate",				UIPopupSettingRoom_ClickOK},
	{"pbClickCreateCancel",			UIPopupSettingRoom_ClickCancel},
	{"ClickSelectMode",				UIPopupSettingRoom_ClickSelectMode},
	{"ClickSelectStage",			UIPopupSettingRoom_ClickSelectStage},
	{"ChangeAICount",				UIPopupSettingRoom_ChangeAICount},
	{"ChangeAILevel",				UIPopupSettingRoom_ChangeAILevel},
	{NULL,							NULL}
};

I3_CLASS_INSTANCE( UIPopupSettingRoom, UIPopupBase);

UIPopupSettingRoom::UIPopupSettingRoom()
{
	m_CreateGameMode = STAGE_TYPE_NA;

	m_nStageID = STAGE_ID_NA;

	m_SubType = 0;
	m_LockWeapon = 0;
	m_LockInterEnter = 0;
	m_LockObserver = 0;
	m_LockTeamBalance = 0;

	m_nAIEnemyCnt = 0;
	m_nAILevel = 0;

	// 특수전 관련 코드
	g_pGameContext->ClearPreStageOption();

	m_bRandomMap = false;
}

UIPopupSettingRoom::~UIPopupSettingRoom()
{	
}

void UIPopupSettingRoom::_SetDefaultInfo( ROOM_INFO_BASIC * pInfo)
{
	char strPassword[NET_ROOM_PW + 1] = {0,};
	
	// 비밀번호 값 설정 - NULL종료 문자열이 아니다! 4byte 바이너리!
	i3String::NCopy(strPassword, g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);

	LuaState * L = _CallLuaFunction( "SetDefaultInfo");
	i3Lua::PushA2UTF8String( L, pInfo->_Title);
	i3Lua::PushA2UTF8String( L, "");
	i3Lua::PushInteger(L , -1);
	i3Lua::PushA2UTF8String( L, strPassword);
	_EndLuaFunction( L, 4);
}

void UIPopupSettingRoom::_SetOptionWindow()
{
	LuaState * L ;

	if(m_StageOption == STAGE_OPTION_AI)
	{
		L = _CallLuaFunction( "SetAIModeWindow");
		_EndLuaFunction(L, 0);
	}
	else if( getStageType( getStageID()) == STAGE_TYPE_DOMINATION)	///[제압 미션]옵션 창 설정.
	{
		L = _CallLuaFunction( "SetDominationModeWindow");
		_EndLuaFunction(L, 0);
	}
	else
	{
		L = _CallLuaFunction( "SetDefaultWindow");
		i3Lua::PushBoolean(L , g_pGameContext->IsEscapeRoom() == false ? 1 : 0);
		_EndLuaFunction( L, 1);
	}
}

void UIPopupSettingRoom::_SetStageInfo( void)
{
	LuaState * L;

	STAGE_ID nStageID = getStageID();

	if( nStageID == 0)return; // 맵이동시 나올수 있어 return

	STAGE_TYPE stageType = getStageType( nStageID);

	// 맵 이미지
	INT32 imageIdx = g_pFramework->GetMapShapeIdx( (STAGE_ID) nStageID);

	bool bRandomMap = getRandomMap();

	if(bRandomMap)
	{
		imageIdx = g_pFramework->GetRandomMapIdx();
	}

	// 맵 이름, 모드
	std::string strMapName;
	std::string strModeName;
	BOOL bAllowChangeMode = FALSE;
	
	GameUI::GetStageMapName( bRandomMap, (STAGE_ID)nStageID, strMapName);
	GameUI::GetStageModeName( strModeName, stageType, m_StageOption);
	bAllowChangeMode = GameUI::IsAllowChangeMode( stageType);

	//  X-Mas Mark
	bool bXMasEnable = false;

	if(TESTBIT(getNewMarkStageTable( getStageOrd(nStageID)) , GOODS_MARK_XMAS))
		bXMasEnable = true;

	L = _CallLuaFunction( "SetMapInfo");
	if( L != NULL)
	{
		i3Lua::PushA2UTF8String( L, strModeName.c_str());
		i3Lua::PushA2UTF8String( L, strMapName.c_str());
		i3Lua::PushInteger( L, imageIdx);
		i3Lua::PushBoolean( L , bXMasEnable ? TRUE : FALSE );
		i3Lua::PushColor( L, GameUI::GetStageModeColor( getStageType( nStageID), m_StageOption ));
		_EndLuaFunction( L, 8);
	}

	//SelectStageOK 함수에서 호출되는 경우는 Stage가 변경된 경우이기에 체크 안 된 상태로 진행되야 한다.
	SetLockSniper(FALSE, FALSE);
	SetLockRPG7(FALSE);
}

void UIPopupSettingRoom::_SetCondition( void)
{
	LuaState * L;
	STAGE_ID nStageID = getStageID();
	STAGE_TYPE stageType = getStageType( nStageID);

	// 시작/끝 인덱스 설정
	INT32 i;
	INT32 idxMin, idxMax, curCondition, curTime;
	bool	bIsRound = false;
	bool	bIsObserver = false;
	char szCondition[ MAX_STRING_COUNT] = {0,};
	char szTimes[ MAX_STRING_COUNT] = {0,};

	UINT32 gameType	= m_SubType & 0x0F;
	UINT32 gameTime	= m_SubType & 0xF0;

	m_GameTypeList.Clear();
	
	{	// 라운드 또는 킬카운트 조건
		switch( stageType )
		{
		case STAGE_TYPE_ANNIHILATION	: 
		case STAGE_TYPE_SUDDEN_DEATH	:
		case STAGE_TYPE_HEAD_KILLER	:
		case STAGE_TYPE_BOMB		:
		case STAGE_TYPE_DESTROY		:
		case STAGE_TYPE_DEFENCE		:
			idxMin = BATTLE_ROUND_TYPE_3;
			idxMax = BATTLE_ROUND_TYPE_9;		// 라운드전

			for( i = idxMin; i <= idxMax; i++)
			{
				_AddGameType((void*)i);;
				i3String::Concat( szCondition, TextSet::Round( i));
				i3String::Concat( szCondition, ";");
			}
			bIsRound = true;

			if( stageType == STAGE_TYPE_ANNIHILATION ||
				stageType == STAGE_TYPE_BOMB )
			{
				bIsObserver = true;	// 관전기능을 On
			}
			break;

		case STAGE_TYPE_ESCAPE		:
			i3String::Format(szCondition, MAX_STRING_COUNT, "%s;", GAME_STRING("STBL_IDX_MODE_TYPE1") );
			bIsRound = true;
			//Round 변경 방식이 아니고 공수 교대 이므로
			//현재 ComboBox 초기 설정 값을 0으로 만들어야 한다.
			idxMax = idxMin = gameType;
			break;

		default					:
			idxMin = BATTLE_KILL_TYPE_60;
			idxMax = BATTLE_KILL_TYPE_160;		// Kill전

			for( i = idxMin; i <= idxMax; i++)
			{
				_AddGameType((void*)i);
				i3String::Concat( szCondition, TextSet::KillCount( i));
				i3String::Concat( szCondition, ";");
			}
			break;
		}

		curCondition = gameType - idxMin;
	}

	m_GameTimeList.Clear();

	{	// 게임 시간 설정
		switch( stageType)
		{
		case STAGE_TYPE_BOMB		:
		case STAGE_TYPE_DESTROY		:
		case STAGE_TYPE_ANNIHILATION		:
		case STAGE_TYPE_SUDDEN_DEATH :
		case STAGE_TYPE_HEAD_KILLER :
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_7);
			break;
		case STAGE_TYPE_DEFENCE		:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_3);
			break;

		case STAGE_TYPE_ESCAPE		:
			idxMin = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			idxMax = HINIBBLE(BATTLE_TIME_MISSION_TYPE_5);
			break;
		case STAGE_TYPE_HEAD_HUNTER	:
		case STAGE_TYPE_DEATHMATCH	:
		default						:
			// AI전 = 5~10분 , 기타 = 5~30분 설정
			if(isTrainingMode())
			{
				idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
				idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
			}
			else
			{
				idxMin = HINIBBLE(BATTLE_TIME_TYPE_5);
				idxMax = HINIBBLE(BATTLE_TIME_TYPE_30);
			}
			break;
		}

		for( i = idxMin; i <= idxMax; i++)
		{
			_AddGameTime((void*)i);
			i3String::Concat( szTimes, TextSet::GameTime( MAKEBYTE(i, 0)));
			i3String::Concat( szTimes, ";");
		}

		curTime = HINIBBLE( gameTime) - idxMin;
	}

	if(isTrainingMode())
	{
		_SetAIModeCondition(szTimes , curTime);
	}
	else if( getStageType(getStageID()) == STAGE_TYPE_DOMINATION)	///[제압 미션] 조건 설정.
	{
		_SetDominationModeCondition();
	}
	else
	{
		L = _CallLuaFunction( "SetConditions");
		if( L != NULL)
		{
			i3Lua::PushBoolean( L, bIsRound?TRUE:FALSE);
			i3Lua::PushBoolean( L, bIsObserver?TRUE:FALSE);
			i3Lua::PushA2UTF8String( L, szCondition);
			i3Lua::PushInteger( L, curCondition);
			i3Lua::PushA2UTF8String( L, szTimes);
			i3Lua::PushInteger( L, curTime);
			i3Lua::PushInteger( L, m_LockInterEnter);
			i3Lua::PushInteger( L, m_LockTeamBalance);
			_EndLuaFunction( L, 8);
		}
	}
}

void UIPopupSettingRoom::_SetAIModeCondition(char * szTimes, INT32 curTime)
{
	char szLevel[256] = {0};

	for(INT32 idx = 0 ; idx < TRAINING_DIFFICULTY_LEVEL_HIGH ; idx++)
	{
		i3String::Concat( szLevel, TextSet::AIModeLevel(idx + 1));
		i3String::Concat( szLevel, ";");		
	}

	LuaState* L = _CallLuaFunction("SetAIConditions");
	i3Lua::PushA2UTF8String(L , "1;2;3;4;5;6;7;8");
	i3Lua::PushInteger(L , m_nAIEnemyCnt - 1);
	i3Lua::PushA2UTF8String(L , szTimes);
	i3Lua::PushInteger( L, curTime);
	i3Lua::PushA2UTF8String(L , szLevel);
	i3Lua::PushInteger( L, m_nAILevel - 1 );
	_EndLuaFunction( L, 6);
}

void UIPopupSettingRoom::_SetDominationModeCondition()
{
	char szLevel[256] = {0};

	for(INT32 idx = 0 ; idx < TRAINING_DIFFICULTY_LEVEL_HIGH ; idx++)
	{
		i3String::Concat( szLevel, TextSet::AIModeLevel(idx + 1));
		i3String::Concat( szLevel, ";");		
	}

	LuaState* L = _CallLuaFunction("SetDominationConditions");
	i3Lua::PushA2UTF8String(L , szLevel);
	i3Lua::PushInteger( L, m_nAILevel - 1 );
	_EndLuaFunction( L, 2);
}

void UIPopupSettingRoom::_SetWindowName()
{
	LuaState* L = _CallLuaFunction("SetWindowName");

	if(L != NULL)
	{
		// 로비 & 레디룸의 윈도우 이름을 변경해주기 위한 코드입니다.
		i3Lua::PushBoolean(L , 
			g_pFramework->GetUIMainframe()->GetCurrentPhaseType() == UIPHASE_LOBBY ? TRUE : FALSE);
		_EndLuaFunction(L , 1);
	}
}

void UIPopupSettingRoom::_SetLimits( const LimitAtt & limitAtt )
{
	LuaState * L = _CallLuaFunction( "setLimitWeapon");
	i3Lua::PushInteger( L, limitAtt.Primary );			// 주무기
	i3Lua::PushInteger( L, limitAtt.Secondary);			// 보조무기
	i3Lua::PushInteger( L, limitAtt.Melee );	// 근접 무기
	i3Lua::PushInteger( L, limitAtt.Throw );				// 투척 무기
	i3Lua::PushInteger( L, limitAtt.RPG7 );				// RPG7
	_EndLuaFunction( L, 5);

	// 특수무기전
	INT32 weaponMode = 0;
	
	STAGE_TYPE eStageType = getStageType( m_nStageID);


	// 스나전, 샷건전, 방어미션, 파괴미션은 주무기 Lock
	if( isSniperMode() || isShotGunMode() || isEscapeMode() ||
		eStageType == STAGE_TYPE_DEFENCE ||
		eStageType == STAGE_TYPE_DESTROY)	weaponMode = 1;	// 주무기 Lock
	else if( isKnuckleMode())				weaponMode = 2;	// 주무기,보조무기 off

	L = _CallLuaFunction("SpecialWeaponMode");
	if( L != NULL)
	{
		i3Lua::PushInteger(L, weaponMode);
		_EndLuaFunction(L,1);
	}
}

void UIPopupSettingRoom::_SetObserver( void)
{
	LuaState * L = _CallLuaFunction( "setLimitObserver");
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_CHASE)?1:0);	// 관전 허용
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY)?1:0);	// 상대팀 관전
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_FREE)?1:0);		// 3인칭 관전
	i3Lua::PushInteger( L, TESTBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP)?1:0);	// HP표시
	_EndLuaFunction( L, 4);
}

bool UIPopupSettingRoom::OnEntranceStart( void * pArg1 /*= NULL*/, void * pArg2 /*= NULL*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;


	m_nAIEnemyCnt = g_pGameContext->getEnemyAiCount();
	m_nAILevel = g_pGameContext->getDifficultyLevel();

	//세팅정보 불러오기 및 초기화
	UpdateRoomInfo();

	// [임시] 게임 창 내 스페셜드인 경우에는 게임모드를 비활성화합니다.
	// 기획 내용에 따라 수정예정입니다.
	BOOL bModeSelEnable = TRUE;

	if(isSniperMode() || isHeadHunterMode() || 
		isTrainingMode() || isKnuckleMode() || isEscapeMode() )
	{
		bModeSelEnable = FALSE;
	}

	LuaState* L = _CallLuaFunction("setSpecialModeConfig");
	i3Lua::PushBoolean(L , bModeSelEnable); 
	_EndLuaFunction(L , 1);


	//UpdateRoomInfo 호출시에 _SetStageInfo 이 함수 안에서 SetLockRPG7 를 호출한다.
	//Lobby에서 설정된 값으로 다시 셋팅한다.
	SetLockSniper(TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0,
		TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0);
	SetLockRPG7(TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0);

	_SetWindowName();

	return true;
}

void UIPopupSettingRoom::ChangeMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit /*= ULM_NONE*/)
{
	m_StageOption = STAGE_OPTION_NONE;

	// 샷건전의 경우 활성화 모드의 옵션을 추가한다. (1회성)
	if(limit == ULM_SHOTGUN)
		g_pGameContext->AddPreStageOption(STAGE_OPTION_SHOTGUN);

	switch( mode)
	{
	case UMT_BOMBMISSION :	m_nStageID = getFirstStageByType( STAGE_TYPE_BOMB);	break;
	case UMT_ANNIHILATION :
		m_nStageID = getFirstStageByType( STAGE_TYPE_ANNIHILATION);
		m_StageOption = STAGE_OPTION_ANNIHILATION;
		break;
	case UMT_DEATHMATCH :	m_nStageID = getFirstStageByType( STAGE_TYPE_DEATHMATCH);	break;
	case UMT_DESTROY :		m_nStageID = getFirstStageByType( STAGE_TYPE_DESTROY);		break;
	case UMT_AI :
		g_pGameContext->AddPreStageOption( STAGE_OPTION_AI);
		
		m_nStageID = getFirstStageByType( STAGE_TYPE_DEATHMATCH);
		m_StageOption = STAGE_OPTION_AI;
		m_nAIEnemyCnt = 8;
		m_nAILevel = TRAINING_DIFFICULTY_LEVEL_LOW;

		g_pGameContext->RemovePreStageOption( STAGE_OPTION_AI);
		break;
	case UMT_ESCAPE :		m_nStageID = getFirstStageByType( STAGE_TYPE_ESCAPE);	break;
	case UMT_DEFENCE :		m_nStageID = getFirstStageByType( STAGE_TYPE_DEFENCE);	break;
	case UMT_SUDDENDEATH :
		m_nStageID = getFirstStageByType( STAGE_TYPE_SUDDEN_DEATH);
		m_StageOption = STAGE_OPTION_SUDDENDEATH;
		break;
	case UMT_HEADHUNTER :
		m_nStageID = getFirstStageByType( STAGE_TYPE_HEAD_HUNTER);
		m_StageOption = STAGE_OPTION_HEADHUNTER;
		break;
	case UMT_CLANMATCH :	break;
	case UMT_TUTORIAL :
		m_StageOption = STAGE_OPTION_TUTORIAL;
		break;

	case UMT_DOMINATION :	
		///[제압 미션] 디폴트 스테이지.
		///m_nStageID = getFirstStageByType( STAGE_TYPE_DOMINATION);
		 if( LocaleValue::Enable( "EnableDomination"))
			m_nStageID = STAGE_ID_DEADTOWN;

		break;
	default :
		I3FATAL( "Not set mode!!!");
	}

	// 첫 유효한 스테이지를 찾은 후 샷건전 옵션을 제거한다.
	if(	g_pGameContext->IsPreStageOption(STAGE_OPTION_SHOTGUN))
		g_pGameContext->RemovePreStageOption(STAGE_OPTION_SHOTGUN);

	switch( limit)
	{
	case ULM_ROCKET :
		m_StageOption = STAGE_OPTION_RPG;
		break;
	case ULM_SNIPER :
		m_StageOption = STAGE_OPTION_SNIPER;
		m_nStageID = getFirstSniperStage();
		break;
	case ULM_SHOTGUN :
		m_StageOption = STAGE_OPTION_SHOTGUN;
		break;
	case ULM_KNUCKLE :
		m_StageOption = STAGE_OPTION_KNUCKLE;
		m_nStageID = getFirstKnuckleStage();
		break;
	}

	//STAGE_OPTION_RPG			= 0x0002,

	// 모드 변경시 랜덤맵 초기화
	if(m_bRandomMap)
	{	
		m_bRandomMap = false;
		g_pGameContext->ClearRandomMapIndexList();
	}
	
	_SetOptionWindow();

	GetMainFrame()->ClosePopup( UPW_SELECTMODE);
}

void UIPopupSettingRoom::ChangeStage( STAGE_ID stage)
{
	GetMainFrame()->ClosePopup( UPW_SELECTSTAGE);

	m_nStageID = stage;
}

void UIPopupSettingRoom::ChangeTimeLimit( INT32 comboIndex)
{
	UINT8 gameTime = _GetGameTime(comboIndex);
	UINT8 gameType = LONIBBLE(m_SubType);

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void UIPopupSettingRoom::ChangeObjectCondition( INT32 comboIndex)
{
	UINT8 gameTime = HINIBBLE(m_SubType);
	UINT8 gameType = LONIBBLE(_GetGameType(comboIndex));

	m_SubType = MAKEBYTE(gameTime, gameType);
}

void UIPopupSettingRoom::UpdateRoomInfo()
{
	//Control State는 루아가 관리
	ROOM_INFO_BASIC	roomInfo;
	ROOM_INFO_ADD	roomAddInfo;

	m_bRandomMap = g_pGameContext->IsRandomMap();

	g_pGameContext->ClearRandomMapIndexList();

	if(m_bRandomMap)
	{
		if(g_pGameContext->GetRandomMapCount() < 1)
		{
			g_pGameContext->ClearRandomMapList();
			g_pGameContext->ClearRandomMapIndexList();
			ROOM_INFO_BASIC		MyRoomInfo;
			g_pGameContext->getMyRoomInfo(&MyRoomInfo);
			m_bRandomMap = false;
			MyRoomInfo._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( MyRoomInfo._InfoFlag, m_bRandomMap);
			g_pGameContext->setMyRoomInfo(&MyRoomInfo);
		}
		else
		{
			for(INT32 i = 0 ; i < g_pGameContext->GetRandomMapCount() ; i++)
			{
				INT32 MapIndex = g_pGameContext->GetRandomMap(i);	
				g_pGameContext->AddRandomMapIndex(MapIndex);
			}
		}
	}

	g_pGameContext->getMyRoomInfo(&roomInfo);
	g_pGameContext->getMyRoomInfoAdd(&roomAddInfo);

	if( roomInfo._StageID == 0) return;

	m_nStageID = (STAGE_ID) roomInfo._StageID;
	m_LockWeapon = roomInfo._WeaponFlag;
	m_LockInterEnter = roomAddInfo._LockInterEnter;
	m_LockTeamBalance = roomAddInfo._LockTeamBalance;
	m_LockObserver = roomAddInfo._LockObserver;
	m_SubType = roomAddInfo._SubType;
	
	m_StageOption = (STAGE_OPTION_TYPE)g_pGameContext->GetStageOption();

	// 섬멸전과 단체전은 맵을 공유하나, 섬멸전의 경우 활성화 되지 않는
	// 스테이지를 제거하기 위해, 스테이지 옵션을 강제로 설정해준다.
	if(getStageType(m_nStageID) == STAGE_TYPE_ANNIHILATION)
		m_StageOption = STAGE_OPTION_ANNIHILATION;
	
	// 기본 정보
	_SetDefaultInfo( &roomInfo);

	// Map 설정
	_SetStageInfo();

	// 목표
	_SetCondition();

	// 제한 기능
	LimitAtt att;

	att.Primary = TESTBIT(m_LockWeapon, LOCK_PRIMARY) ? 1 : 0;
	att.Secondary = TESTBIT(m_LockWeapon, LOCK_SECONDARY) ? 1 : 0;
	att.Melee = TESTBIT(m_LockWeapon, LOCK_MELEE) ? 1 : 0;
	att.Throw = TESTBIT(m_LockWeapon, LOCK_THROW) ? 1 : 0;
	att.RPG7 = TESTBIT(m_LockWeapon, LOCK_RPG7) ? 1 : 0;

	_SetLimits(att);

	// 관전 기능
	_SetObserver();

	_SetOptionWindow();
}

/*virtual*/ bool UIPopupSettingRoom::OnExitStart( void)
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	// 이창이 닫히면 나머지 서브창들도 닫자!!!
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE))
		GetMainFrame()->ClosePopup( UPW_SELECTMODE);

	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE))
		GetMainFrame()->ClosePopup( UPW_SELECTSTAGE);

	return true;
}

/*virtual*/ bool UIPopupSettingRoom::OnCloseButton( void)
{
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE))
		return GetMainFrame()->GetPopup( UPW_SELECTMODE)->OnCloseButton();

	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE))
		return GetMainFrame()->GetPopup( UPW_SELECTSTAGE)->OnCloseButton();

	return true;
}

void UIPopupSettingRoom::SettingRoomOK( void)
{
	BOOL bNeedFilter = TRUE;
	char szTemp[MAX_STRING_COUNT] = "";
	char *pszTitleName = NULL;
	LuaState * L;

	L = _CallLuaFunction( "getTitleName");
	pszTitleName = (char*) _ReturnLuaString( L, 0);

	// 빈 단어입력
	i3String::Copy( szTemp, pszTitleName, MAX_STRING_COUNT );
	i3String::RemoveAllBlank( szTemp );
	if( i3String::Length( szTemp ) <= 0) { 
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_PLEASE_WRITE_ROOM_NAME"));/*방 제목을 입력하여 주십시오*/
		return;
	}

	// 방생성 이름이 기본 제공하는 제목이라면 필터링하지 않는다. 기본 방제에서 필터에 걸려 방 생성이 안되는 문제를 해결함. komet
	for(INT32 i=0; i<4; i++)
	{
		i3String::Format(szTemp, MAX_STRING_COUNT, "STBL_IDX_ROOMNAME_%d", i+1);	// 1 ~ 4

		if( i3String::Compare( GAME_STRING( szTemp), pszTitleName) == 0)
		{
			bNeedFilter = FALSE;	// 기본 제공하는 방제목이므로 필터할 필요없다.
			break;
		}
	}	

	if( bNeedFilter)
	{
#if defined(LOCALE_CHINA)
		CStringFilter* pStringFilter = CStringFilter::NewObject();
		CStringFilter* pStringFilter2 = CStringFilter::NewObject();
		pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
		pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);

		if( FALSE == g_pFramework->IsFilter( pszTitleName ) || i3String::ContainNoCase(szTemp, "{col:") >= 0 || FALSE == pStringFilter2->IsFilter( pszTitleName )
			|| FALSE == pStringFilter->IsFilterSame( pszTitleName ))

		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			I3_SAFE_RELEASE( pStringFilter2 );
			I3_SAFE_RELEASE( pStringFilter );
			return;
		}	
		I3_SAFE_RELEASE( pStringFilter2 );
		I3_SAFE_RELEASE( pStringFilter );
#else
		if( FALSE == g_pFramework->IsFilter( pszTitleName ) || i3String::ContainNoCase(szTemp, "{col:") >= 0)
		{
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_STRING("STR_TBL_GUI_LOBBY_WRONG_WORD"));/*입력 할 수 없는 단어가 포함되어 있습니다*/
			return;
		}	
#endif

	}

	ROOM_INFO_ALL Info, OldInfo;

	g_pGameContext->getMyRoomInfo(&Info._Basic);
	g_pGameContext->getMyRoomInfoAdd(&Info._Add);

	i3mem::Copy( &OldInfo, &Info, sizeof(Info));

	SetInputDisable( false);

	// 방생성 정보를 셋합니다!

	// 1.방이름
	{
		// ! praptor - IME에 버그 있습니다. 초기화시 넘겨준 최대길이보다 큰 문자열이
		// 넘어 올 수 있습니다. 문자열복사시 반드시 확인해야합니다.

		// IME에 의해 최대길이보다 큰 문자열이 들어왔을 경우 잘라낸다.
		char strTitle[MAX_STRING_COUNT];
		i3String::Copy( strTitle, pszTitleName, MAX_STRING_COUNT );		

		UINT32 titleLength = i3String::Length(strTitle);

		if (NET_ROOM_NAME_SIZE <= titleLength)
		{
			for(INT32 i = NET_ROOM_NAME_SIZE - 1; i >= 0; i--)
			{
				if (2 == i3String::WhatIsCode(strTitle, i))
				{
					strTitle[i] = '\0';
				}
				else
				{
					strTitle[i] = '\0';
					break;
				}
			}
		}

		i3String::Copy(Info._Basic._Title, strTitle, NET_ROOM_NAME_SIZE );
	}

	// 2.게임모드 / Stage
	{	
		Info._Basic._StageID = m_nStageID;			
	}

	// 3.비밀방 여부
	{
		L = _CallLuaFunction( "getPassword");
		const char * pszPassword = _ReturnLuaString( L, 0);
		bool bChangePW = false;

		if( pszPassword != NULL)
		{
			INT32 Length = i3String::Length( pszPassword);

			if( Length > 0 && Length <= sizeof(char) * NET_ROOM_PW)
			{
				Info._Basic._InfoFlag |=  ROOM_INFO_FLAG_PRIVATE_MODE;

				// 4. 비밀번호
				if( i3String::Compare( g_pGameContext->getMyRoomPW(), pszPassword) != 0)
				{
					//일단 현재 설정된 패스워드를 전부 Clear 후 재설정
					i3mem::FillZero(g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
					i3mem::Copy( g_pGameContext->getMyRoomPW(), pszPassword, sizeof(char) * Length);
					g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_PASSWORD_N);
					bChangePW = true;
				}
			}
			else if(Length == 0)
			{
				Info._Basic._InfoFlag &= ~(ROOM_INFO_FLAG_PRIVATE_MODE);
				i3mem::FillZero( g_pGameContext->getMyRoomPW(), sizeof(char) * NET_ROOM_PW);
				g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_PASSWORD_N);
				bChangePW = true;				
			}
		}
	}

	// 4. 게임 고급 설정 메뉴
	{			
		//
		//// 무기 제한	
		m_LockWeapon = 0;

		UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		UNSETBIT(m_LockWeapon, LOCK_RPG7);
		UNSETBIT(m_LockWeapon, LOCK_THROW);

		L = _CallLuaFunction( "isLimitPrimary");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_PRIMARY);
		else
			UNSETBIT(m_LockWeapon, LOCK_PRIMARY);
		

		L = _CallLuaFunction( "isLimitSecondary");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_SECONDARY); 
		else
			UNSETBIT(m_LockWeapon, LOCK_SECONDARY);
		

		L = _CallLuaFunction( "isLimitRPG7");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_RPG7);
		else
			UNSETBIT(m_LockWeapon, LOCK_RPG7);

		L = _CallLuaFunction( "isLimitThrow");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockWeapon, LOCK_THROW); 
		else
			UNSETBIT(m_LockWeapon, LOCK_THROW);

		SETBIT( m_LockWeapon, LOCK_MELEE);

		Info._Basic._WeaponFlag = m_LockWeapon;

		UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);

		//
		// 관전기능
		L = _CallLuaFunction( "isLimitObserver");
		if( _ReturnLuaBoolean( L, 0))
			SETBIT( m_LockObserver, LOCK_OBSERVER_CHASE);
		else
			UNSETBIT(m_LockObserver, LOCK_OBSERVER_CHASE);

		{
			L = _CallLuaFunction( "isLimitObserver3");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_FREE);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_FREE);
		}

		{
			L = _CallLuaFunction( "isLimitObserverEnemy");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY);
		}

		{
			L = _CallLuaFunction( "isLimitObserverHP");
			if( _ReturnLuaBoolean( L, 0))
				SETBIT( m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
			else
				UNSETBIT(m_LockObserver, LOCK_OBSERVER_ENEMY_HP);
		}

		// 난입제한
		{
			LuaState * L = _CallLuaFunction( "isLimitInter");
			i3Lua::PushBoolean( L, getStageType(getStageID()) == STAGE_TYPE_DOMINATION);
			m_LockInterEnter = _ReturnLuaBoolean( L, 1) ? 1 : 0;
		}

		//// 팀밸런스
		{
			LuaState * L = _CallLuaFunction( "getAutoTeamBalance");
			m_LockTeamBalance = static_cast<UINT8>(_ReturnLuaInteger(L, 0));
		}

		Info._Add._SubType = m_SubType;
		Info._Add._LockInterEnter = m_LockInterEnter;
		Info._Add._LockObserver = m_LockObserver;
		Info._Add._LockTeamBalance = m_LockTeamBalance;
	}

	// 맵 설정이 안바뀌는 문제로 주석 2011.09.21 양승천
	//Info._Basic._StageMask = STAGE_MASK_TYPE_TRAINING;

	// PreStageOption = 방생성UI 특수전을 위한 변수...	
	// 의미가 없는 부분인것같아서 주석처리합니다. 2011.10.25 양승천
	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SNIPER ) ) 
	//{
	//	Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SNIPER( Info._Basic._WeaponFlag );
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_SNIPER );
	//}

	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_SHOTGUN ) ) 
	//{
	//	Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN( Info._Basic._WeaponFlag );
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_SHOTGUN );
	//}

	//if( g_pGameContext->IsPreStageOption( STAGE_OPTION_HEADHUNTER ) )
	//{
	//	g_pGameContext->RemovePreStageOption( STAGE_OPTION_HEADHUNTER );
	//}

	if( isSniperMode())
		SETBIT(Info._Basic._WeaponFlag ,LOCK_SNIPER); 
	else if( isShotGunMode())
		Info._Basic._WeaponFlag = SET_ROOM_BASIC_WEAPON_SHOTGUN( Info._Basic._WeaponFlag );

	if(isTrainingMode())
	{
		g_pGameContext->setEnemyAiCount(m_nAIEnemyCnt);
		g_pGameContext->setDifficultyLevel(m_nAILevel);
		g_pGameContext->setCurrentDifficultyLevel(m_nAILevel);
	}

	// 랜덤맵 셋팅
	{
		Info._Basic._InfoFlag = SET_ROOM_INFO_RANDOM_MODE( Info._Basic._InfoFlag, getRandomMap());

		g_pGameContext->ClearRandomMapList();

		for(INT32 idx = 0 ; idx < g_pGameContext->GetRandomMapIndexCount() ; idx ++)
		{
			INT32 CheckMap = g_pGameContext->GetRandomMapIndex(idx);
			g_pGameContext->AddRandomMap(CheckMap);
		}
	}

	// 비교후 변경된것이 있으면 보낸다.
	bool bChanged = false;
	// 총이 바뀌었을떄
	if( Info._Basic._WeaponFlag != OldInfo._Basic._WeaponFlag ||
		Info._Basic._StageID != OldInfo._Basic._StageID ||
		Info._Basic._InfoFlag != OldInfo._Basic._InfoFlag ||
		i3String::Compare( Info._Basic._Title, OldInfo._Basic._Title) != 0 )
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOMINFO_N, &Info._Basic );
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOMINFO_N\n");
		bChanged = true;
	}

	

	// 총 이외에 다른 것이 바뀌었을때
	if (Info._Add._LockInterEnter != OldInfo._Add._LockInterEnter ||
		Info._Add._LockObserver != OldInfo._Add._LockObserver ||
		Info._Add._LockTeamBalance != OldInfo._Add._LockTeamBalance	||
		Info._Add._SubType != OldInfo._Add._SubType)
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_OPTION_N, &Info._Add);
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOM_OPTION_N\n");
		bChanged = true;
	}

	if( bChanged == false)
	{
		g_pGameContext->SetEvent( EVENT_CHANGE_ROOM_ALLINFO_N, &Info);
		I3TRACE( "SettingRoom EVENT_CHANGE_ROOM_ALLINFO_N\n");
	}
	

	GetMainFrame()->TogglePopup( UPW_SETTINGROOM);
}

void UIPopupSettingRoom::SettingRoomCancel( void)
{
	GetMainFrame()->TogglePopup( UPW_SETTINGROOM);
}


/*virtual*/ void UIPopupSettingRoom::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "UIRe/PointBlankRe_Lobby_CreatRoom.i3UIE", UIPoupSettingRoom_Glue);
}

/*virtual*/ bool UIPopupSettingRoom::OnKey_Enter( void)
{
	if( GetMainFrame()->IsOpenedPopup( UPW_SELECTMODE) == false &&
		GetMainFrame()->IsOpenedPopup( UPW_SELECTSTAGE) == false )
	{
		// ENTER키는 focus상태의 IME control에서 처리하기때문에,
		// focus상태의 IME control이 없을 경우에만 처리한다.
		g_pFramework->SetFocus(this);
		SettingRoomOK();
		return true;
	}

	return false;
}

void UIPopupSettingRoom::ClickSelectStage( void)
{
	GetMainFrame()->OpenPopup( UPW_SELECTSTAGE, &m_nStageID, &m_StageOption);
}

void UIPopupSettingRoom::ClickSelectMode( void)
{
	i3UI::setDisableUIInput(true);
	GetMainFrame()->OpenPopup( UPW_SELECTMODE);
}

/*virtual*/ void UIPopupSettingRoom::SelectMode( UI_MODE_TYPE mode, UI_LIMIT_MODE limit /*= ULM_NONE*/)
{
	ChangeMode(mode , limit);

	STAGE_TYPE stageType = getStageType( m_nStageID);

	// 확인용 
	m_CurModeSelect = (INT32) stageType - 1;

	// 기본 StageID와 SubType을 설정.
	GameUI::GetDefaultStageInfo( m_nStageID, m_SubType, stageType);

	ROOM_INFO_BASIC	roomInfo;
	g_pGameContext->getMyRoomInfo(&roomInfo);

	roomInfo._StageID = m_nStageID;

	_SetStageInfo();

	// 목표 카운트
	_SetCondition();

	// 제한 기능
	LimitAtt att;
	_SetLimits(att);

	// 관전 기능 (모드 변경시 관전기능은 초기화)
	m_LockObserver = 0;
	_SetObserver();
}


/*virtual*/ void UIPopupSettingRoom::SelectStageOK( STAGE_ID stage)
{
	ChangeStage(stage);
	
	_SetStageInfo();
}

void UIPopupSettingRoom::SetLockSniper(int lockSecondary, int lockThrow)
{
	if( m_StageOption == STAGE_OPTION_SNIPER )
	{
		LuaState* L = _CallLuaFunction( "SetLockSniper");
		if( L != NULL)
		{
			i3Lua::PushBoolean( L, lockSecondary);
			i3Lua::PushBoolean( L, lockThrow);
			_EndLuaFunction( L, 2);
		}
	}
}

void UIPopupSettingRoom::SetLockRPG7(int lockRpg7)
{
	LuaState* L = _CallLuaFunction( "SetLockRPG7");
	if( L != NULL)
	{
		i3Lua::PushBoolean( L, isRPG7Mode() ? FALSE : TRUE);

		i3Lua::PushInteger( L, lockRpg7);
		_EndLuaFunction( L, 2);
	}
}

bool UIPopupSettingRoom::isRPG7Mode()
{
	STAGE_ID stageID = getStageID();	
	STAGE_TYPE stageType = getStageType(stageID);
	STAGE_SLOT_MODE stageMode = getStageSlot(stageID);

	bool bRPG7 = true;

	//RPG7은 STAGE_SLOT_MODE_4VS4(Stage Mode) 이다.
	if( stageMode != STAGE_SLOT_MODE_4VS4 )
		bRPG7 = false;

	// RPG7은 파괴/방어 미션에 사용할 수 없다.
	if( bRPG7 && ( stageType == STAGE_TYPE_DESTROY || stageType == STAGE_TYPE_DEFENCE) )
		bRPG7 = false;

	//RPG7은 현재 선택된 Stage Mode가 STAGE_OPTION_NONE 이어야 한다.
	if( bRPG7 && m_StageOption != STAGE_OPTION_NONE )
		bRPG7 = false;

	//RPG7은 서버에 설정된 StageOption이 STAGE_OPTION_RPG 이어야 한다.
	if( bRPG7 && s_StageInfoTableList.IsActiveStageOption( stageID, STAGE_OPTION_RPG ) == false )
		bRPG7 = false;

	return bRPG7;
}