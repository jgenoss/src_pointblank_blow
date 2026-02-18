/****************************************************************************************/
//					배포 (라이브)
/****************************************************************************************/

#if defined( LOCALE_NORTHAMERICA)
#define GAME_TITLE_NAME_A		"PiercingBlow"				// 게임 타이틀
#define GAME_TITLE_NAME_W		L"PiercingBlow"			
#define GAME_APP_NAME_A			"Piercing Blow"		// 어플리케이션 이름
#define GAME_APP_NAME_W			L"Piercing Blow"		
#else
#define GAME_TITLE_NAME_A		"PointBlank"			// 게임 타이틀
#define GAME_TITLE_NAME_W		L"PointBlank"			
#define GAME_APP_NAME_A			"Point Blank"			// 어플리케이션 이름	
#define GAME_APP_NAME_W			L"Point Blank"			
#endif

#define ENVFILENAME			"EnvSet/EnvSet_VER3_17_1_20.env"	// Env File Version 

#define	CLIENT_SIDE				/// Client용 code를 사용합니다. server는 막고 사용하세요

#ifdef I3_DEBUG
#ifndef USE_EDITDLG
#define	USE_EDITDLG
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// 국가별 define
#ifdef NC_BUILD
	//대회 빌드용
	//Stage 모드 변경 불가, 퀘스트 창 비활성화, 퀘스트 완료 비활성화
	//#define	CHAMPETITION_BUILD		
#endif

#ifdef NC_BUILD
	//#define	CHECK_CORE_FILE_MD5		//PB 런처를 사용하는 국가에 대한 파일 변조 검사
#endif

#if defined(DEV_BUILD)
//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
//#define ENABLE_DETECT_AUTO_MOUSE
//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
#define USE_LINK_SHOP
//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_KOREA)
//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
//#define ENABLE_DETECT_AUTO_MOUSE
//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
#define USE_LINK_SHOP
//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_THAILAND)
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	// #define	USE_DUAL_AI_MODE
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

	//#define THAILAND_SONGKRAN_EVENT		//	태국 송크란 축제 이벤트

#elif defined (LOCALE_INDONESIA)
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

	// ***
#ifdef NC_BUILD
	#define WINDOW_MODE_DISABLE				// Window Mode사용불가능
	#define CHECK_CORE_FILE_MD5
#endif

#elif defined (LOCALE_RUSSIA)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_TURKEY)

	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

	#define BUY_WEAPON_BY_CASH				// 상점에서 무기를 톨로 구매가능하다면, 화면에 Tab을 표시합니다.

#elif defined (LOCALE_CHINA)

	#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_BRAZIL)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_LATIN_AMERICA)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined( LOCALE_NORTHAMERICA)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined( LOCALE_ITALY)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)
	

#elif defined (LOCALE_ENGLISH)

	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined (LOCALE_MIDDLE_EAST)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

	//#define THAILAND_SONGKRAN_EVENT		//	태국 송크란 축제 이벤트

#elif defined( LOCALE_PHILIPPINES)
	
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined( LOCALE_SINGMAL)

	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	#define USE_LINK_SHOP
	//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#elif defined( LOCALE_VIETNAM)

	//#define DESIGZATION_ENABLE				// 호칭 시스템
	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	#define USE_LOBBY_NON_REAL_IP_MARK		// (로비)로비에서 리얼 IP가 아닐 경우 마크
	#define BUY_WEAPON_BY_CASH				// 상점에서 무기를 톨로 구매가능하다면, 화면에 Tab을 표시합니다.
	//#define ENABLE_BUY_CASH_WEAPON_IF_NEED_USER_TITLE	//	호칭 무기라도 캐시면 구입 가능 
	#define USE_CLAN_MARK					// (클랜)클랜마크 클랜 생성이나 GUI 노출을 막으려면 주석처리하세요
	//#define ENABLE_DETECT_AUTO_MOUSE
	#define USE_COMMUNITY_BLOCK			// (커뮤니티) 유저 차단 리스트
	#define ENABLE_EQUIP_USER_TITLE_WEAPON	// (인벤) 장비시 호칭 무기일 경우라도 가능하다.
	#define ENABLE_MISSION_CARD				// 임무카드 탭을 활성화 합니다.

	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	#define USE_THROW_DELAY					// 투척류 무기 2종(수류탄, C-5) 연속으로 사용시 딜레이 추가.
	#define	USE_REPAIR_WEAPON				// 무기를 수리할 수 있습니다.
	#define	USE_DUAL_AI_MODE			
	#define USE_RANDOMMAP_MODE	            // 랜덤맵을 사용
	#define	ENABLE_GM_COMMAND

	#define CLAN_UNLIMITED_MATCH			// (클랜) 클랜전의 팀 인원 관계없이 신청할 수 있도록 합니다. 정해진 팀 인원으로만 매치 신청할수있도록 하려면 주석처리하세요

	//#define	CHANGE_ITEM_USE_CHECK			// 구매/사용이 제한된 아이템을 확인하는 코드를 데이터로 변경합니다.
	#define	CHANGE_ADDCOMMAND_METHOD
	#define	CHANGE_MASK_NEW_PATH
	#define CHANGE_NEW_EFFECT_PATH			// 이펙트 RSC파일을 기존의 Scene\_Common 폴더가 아닌 Effect\Define 폴더를 사용합니다. (K-413 무기부터 적용함)
	#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.

	#define USE_ADDITIONAL_DEATH_MOTION		// 데스모션 추가입니다. 리소스도 같이 변경 되어야합니다.
	#define USE_LINK_SHOP
	#define USE_SKILL_SYSTEM

#elif defined( LOCALE_JAPAN)

	//#define CLAN_ADDRESS_ENABLE				// (클랜)AZIT 주소
	//#define ENABLE_DETECT_AUTO_MOUSE
	//#define USE_AI_MODE_DIEHARD				// 변형된 AI모드를 사용.
	//#define USE_DUAL_AI_MODE				// 두개의 AI모드를 사용합니다.
	//#define USE_WEAPON_ENDURANCE			// 무기 내구도를 씁니다.
	//#define USE_LINK_SHOP
	//#define WEAPON_TAKE_DISABLE				// (인게임)무기를 주울수 없다. (RPG7은 제외)

#endif