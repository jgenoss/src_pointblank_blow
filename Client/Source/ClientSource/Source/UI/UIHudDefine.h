#pragma once

namespace HUD
{
	enum HUD_STYLE 
	{ 
		HS_1_5, HS_1_0, HS_MAX
	};

	enum NoticeMessageType
	{
		NMT_Challenge,
		NMT_Tutorial,
	};

	enum CCVSMessageType
	{
		CCVSMT_PreStartRound,
		CCVSMT_Battle
	};

	enum ChallengeMessage
	{
		CHM_NONE = -1,

		CHM_20_KILL,
		CHM_40_KILL,
		CHM_60_KILL,
		CHM_80_KILL,
		CHM_100_KILL,
		CHM_120_KILL,
		CHM_140_KILL,
		CHM_160_KILL,
		CHM_180_KILL,
		CHM_200_KILL,

		CHM_5_DEATH,
		CHM_20_DEATH,

		CHM_5_CHAIN_KILL,
		CHM_20_CHAIN_KILL,

		CHM_PIERCING_KILL,
		CHM_MASS_KILL,
		CHM_CHAIN_SLUGGER,
		CHM_CHAIN_STOPPER,

		CHM_BATTLE_START,

		MAX_CHM_MESSAGE
	};

	enum UIType
	{
		FIRST_HUD,

		PLAYER_STATE,
		PLAYER_DINO_STATE,
		AMMO_STATE,
		DINO_STATE,
		DASH_BOARD,
		MINIMAP,
		MISSION_BOARD,
		NOTICE,
		CHALLENGE_BOARD,
		CROSS_COUNTER_VS,
		RESPAWN_TIMER,
		TARGET_OBJECT,
		FLOAT_BOARD,
		GAME_RESULT,
		QUEST_EFFECT,
		DEFAUTOCHANGE,
		C4INDICATOR,
		WINTERMODE,
		MADNESSNOTICE,
		BATTLEUSEITEM,
		MULTIWEAPON,

		LAST_HUD,
		UIT_MAX,
	};
	typedef i3::vector<UIType> UITypeList;

	enum CrossCountTeamMark
	{
		RED_HUMAN_OTHERTEAM = 0,
		RED_HUMAN_MYTEAM,

		BLUE_DINO_OTHERTEAM,
		BLUE_DINO_MYTEAM,

		BLUE_HUMAN_OTHERTEAM,
		BLUE_HUMAN_MYTEAM,

		RED_DINO_OTHERTEAM,
		RED_DINO_MYTEAM,
	};

	enum RespawnType
	{
		RT_START,
		RT_SEND_BLOCK,
		RT_SEND_UNBLOCK,

		RT_HOLD,
		RT_RELEASE_HOLD,
		RT_IMMEDIATE,		//immediate
		RT_RESET,
	};

	enum 
	{
		ObserverOffsetX = -100,
	};

	enum MISSION_MODE
	{
		MISSION_NONE = -1,
		MISSION_DEFAULT,
		MISSION_OBSERVER,		
	};

	enum TargetShapeType
	{
		TST_NONE = -1,
		TST_HELICOPTER,
		TST_GIRAN,
		TST_TANK,
		TST_GENERATOR,
	};

	enum KillStarType
	{
		KST_DISABLE = -1,
	};

	enum ItemState //무기 또는 파츠
	{
		IS_NONE = -1, //아이템을 보유하고 있지 않다.
		IS_HAVE, //소유중..
		IS_NOT_HAVE, //소유 안 하고 잇음.
		IS_USE, //사용중
		IS_NOT_USE, //사용안함
		IS_HAVE_MISSION_OBJ, //사용안함
	};

	enum GameResultState
	{
		GRS_Ok, //ok 버튼
		GRS_ScreenSave,  //ScnShot
		GRS_WinTeamState, //win team
		GRS_UpdateKillCount, //유저 킬 업데이트(해당 유저 리스폰시 갱신)
		GRS_UserRoomIn, //유저 입장
		GRS_UserRoomOut, //유저 퇴장
		GRS_HeadShot, //헤드샷 카운터
		GRS_UserKilled, //피격유저
		GRS_Medal, //메달 획득
		GRS_RoundShow, //라운드 종료
		GRS_EndShow, //게임 종료
	};

	enum DashBoardState
	{
		DBS_TargetShow,
		DBS_SetGameTimeZero,
		DBS_SetRunningTime,
		DBS_SetRidingHelicopter,
		DBS_SetUnderAttack,
	};

	enum MadnessNoticeState
	{
		MN_FINISH,		//아무것도 동작 안하는 상태 내부에서 사용.
		MN_START,		//실제 사용자는 이것만 씀.
		MN_UPDATING,	//내부에서 사용.
		MN_STOP,		//물론 내부에서 사용.
	};
}

namespace DinoState
{
	enum Attack	// 이미지 인덱스와 매칭되는 것으로 보임. PBRe_HUD.i3UIe의 Attack1 컨트롤 템플릿 참조바람.
	{
		None = -1,

		HOWL,				//STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL
		SCRATCH,			//STR_TBL_INGAME_HUD_DINO_ATTACK_SCRATCH
		BITE,				//STR_TBL_INGAME_HUD_DINO_ATTACK_BITE
		ELITE_SCRATCH,		//STR_TBL_INGAME_HUD_ELITE_ATTACK_A
		RAPTOR_BUFF,
		POISON_PRICKLE,		//STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE
		STING_BITE,
		BLAST,				//STR_TBL_INGAME_HUD_DINO_ATTACK_BLAST
		BUTT,
		RUSH,
		POISON_GAS,

		MAX_COUNT,
	};

	enum DinoEvent
	{
		De_Show,
		De_AttackIndex1,
		De_AttackIndex2,
		De_SkillGuage,
		De_Name,
	};
}

namespace ResultState
{
	struct UserInform
	{
		i3::rc_wstring wstr_nick; //i3::string 사용하면 컴파일 오류남

		UINT32 clan_idx, clan_mark; //클랜 idx, 클랜 마크
		INT32	idx, score, rank; //user idx, team score, 계급

		UINT16 point, exp;								// 포인트, 경험치
		UINT16 total_bonus_point, total_bonus_exp;		// 총 보너스 포인트, 총 보너스 경험치
		UINT16 kill_count, death_count, assist_count;	// 킬, 데스, 어시스트 카운터
		bool QuestEndFlag;

		UINT8	 mission, result_mark, nick_color, e_sport_mark; //퀘스트, 보너스 포인트 마크, 닉네임 색상, e sport 마크
	
		bool is_challenge; //챌린지 여부
		bool is_my_inform; //내 슬롯 여부
	};
}
