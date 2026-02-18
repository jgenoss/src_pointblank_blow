#ifndef _INGAMEQACOMMANDSET_H__
#define _INGAMEQACOMMANDSET_H__

#undef ENUM_LIST
#define ENUM_LIST(enum_value) \
	enum_value(none) \
	enum_value(mission_win) \
	enum_value(time_limit) \
	enum_value(suicide) \
	enum_value(pos) \
	enum_value(move_pos) \
	enum_value(collision) \
	enum_value(cash) \
	enum_value(notice) \
	enum_value(change_grade) \
	enum_value(set_kill) \
	enum_value(respawn_time) \
	enum_value(item_period) \
	enum_value(mastery_point) \
	enum_value(set_hp) \
	enum_value(bullet_limit) \
	enum_value(grenade_limit) \
	enum_value(respawn) \
	enum_value(object) \
	enum_value(camera) \
	enum_value(fly) \
	enum_value(ui_toggle) \
	enum_value(exp) \
	enum_value(pino) \
	enum_value(challenge) \
	enum_value(clan_rank) \
	enum_value(clan_exp) \
	enum_value(log) \
	enum_value(move_offset) \
	enum_value(acceleration_offset) \
	enum_value(jump_height_offset) \
	enum_value(jump_damage_offset) \
	enum_value(clan_match_point) \
	enum_value(clan_match_win_lose) \
	enum_value(clan_match_room_create) \
	enum_value(medal_count) \
	enum_value(death_all) \
	enum_value(change_character) \
	enum_value(change_designation) \
	enum_value(kill_text) \
	enum_value(view_bone) \
	enum_value(invincible) \
	enum_value(skill) \
	enum_value(move_minimap) \
	enum_value(teamband) \
	enum_value(domination) \
	enum_value(killcount) \
	enum_value(ts) \
	enum_value(solo) \
	enum_value(tp) \
	enum_value(obdmg) \
	enum_value(hackoff) \
	enum_value(timergmpause) \
	enum_value(wc) \
	enum_value(max)


//-----------------------------------------------------------------------------------------------------------------------
//command 설명.  'CommandSet::type 검색'

// CommandSet::mission_win				--> 미션전 윈카운드 조정 커맨드		
// CommandSet::time_limit				--> 게임 진행 시간 무제한 커맨드	
// CommandSet::suicide					--> 캐릭터 자살 커맨드 
// CommandSet::pos						--> 캐릭터 위치 좌표 커맨드
// CommandSet::move_pos					--> 캐릭터 텔레포트 커맨드
// CommandSet::collision				--> 맵 Texture, 충돌체크 표시 커맨드
// CommandSet::cash						--> 보유캐쉬만 조정
// CommandSet::notice					--> 공지사항 출력 커맨드
// CommandSet::change_grade				--> 계급조정 커맨드
// CommandSet::set_kill					--> 종합 킬 수 조정 커맨드
// CommandSet::respawn_time				--> 리스폰 타임 조절 커맨드
// CommandSet::item_period				--> 보유 아이템 기간 조정 커맨드
// CommandSet::mastery_point			--> Mastery Point조정 커맨드
// CommandSet::set_hp					--> HP수치 조절 커맨드
// CommandSet::bullet_limit				--> 무한 탄창 사용 커맨드
// CommandSet::grenade_limit			--> 투척무기 및 기타 무한 사용 커맨드
// CommandSet::respawn					--> 캐릭터 리스폰 커맨드
// CommandSet::object					--> 맵 오브젝트 리스폰 커맨드
// CommandSet::camera					--> 프리 카메라 모드 커맨드( 1/3인칭 본인만..)
// CommandSet::fly						--> fly camera로 변환
// CommandSet::ui_toggle				--> 2D UI의 Enable/Disable
// CommandSet::exp						--> 경험치 조정
// CommandSet::pino						--> 피노 조정 
// CommandSet::challenge				--> 임무 0 ~ 9 까지의 숫자를 넣을수 있음
// CommandSet::clan_rank				--> 클랜 랭크 올리기
// CommandSet::clan_exp					--> 클랜 경험치 올리기
// CommandSet::log						--> 로그(PBLog) 파일로 저장
// CommandSet::move_offset				--> 이동속도 등 호칭 관련 속성 수정
// CommandSet::acceleration_offset		--> 이동속도 등 호칭 관련 속성 수정
// CommandSet::jump_height_offset		--> 이동속도 등 호칭 관련 속성 수정
// CommandSet::jump_damage_offset		--> 이동속도 등 호칭 관련 속성 수정

// CommandSet::clan_match_point			--> 클랜 포인트 수정
// CommandSet::clan_match_win_lose		--> 클랜 승, 패 조작
// CommandSet::clan_match_room_create	--> 클랜 매치 팀 강제 만들기
// CommandSet::medal_count				--> 약장, 휘장, 훈장 업데이트

// CommandSet::death_all				--> 모든 캐릭터 죽임
// CommandSet::change_character			--> 캐릭터 변경
// CommandSet::change_designation		--> 호칭 변경 (arg1 : 슬롯, arg2 : 사용될 호칭, arg3 : On/Off)
// CommandSet::kill_text				--> Kill Text를 test한다.
// CommandSet::view_bone				--> 캐릭터 Bone을 봅니다.
// CommandSet::invincible				--> NOT DAMAGED

// CommandSet::skill					--> 스킬 적용하기(arg1 : 스킬번호, arg2 : 스킬값, arg2 : 스킬레벨)
// CommandSet::move_minimap				--> 미니맵 이동 (for Domination)
// CommandSet::teamband					--> 팀 밴드 시스템
// CommandSet::domination				--> 제압 미션 커멘드 타입

//-----------------------------------------------------------------------------------------------------------------------



#undef DO_ENUM
#define DO_ENUM(value) value,

#undef DO_DESC
#define DO_DESC(value) L ## #value,

namespace CommandSet
{
	enum TYPE {ENUM_LIST(DO_ENUM)};
	static wchar_t* g_wstr_cmd[] = {ENUM_LIST(DO_DESC)};

	enum { VALUE_COUNT = 4};
	static REAL32 g_value[VALUE_COUNT];

	static CommandSet::TYPE _type;
	CommandSet::TYPE parsing( const i3::wliteral_range& wCmdRng);
};

namespace Domination_Util
{
	CommandSet::TYPE parsing( const i3::wliteral_range& wCmdRng);
	extern i3::wstring cmd_msg;
	extern bool enable_domi_nick;

	extern char g_domi_rank_web[MAX_RANKING_URL_SIZE];

	extern INT32 g_current_score;
	extern INT32 g_best_score;
	extern bool g_renewal_best;
};

class CGameCommandSet :	public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCommandSet, i3ElementBase );

protected:
	char *			m_pszCommand = nullptr;

	REAL32			m_fValue[CommandSet::VALUE_COUNT] = { 0.0f };
	bool			m_bAlwaysShowPos = false;
	bool			m_bSkillView = false;

	i3TextNodeDX2 *		m_pSkillCmdText;		// [initialize at constructor]	

public:
	CGameCommandSet(void);
	~CGameCommandSet(void);

	void			ProcessCommand();
	void			SkillDebugView(void);

	bool			IsShowPos(void)	const				{ return m_bAlwaysShowPos;	}
	bool			ToogleShowPos(void)					{ return m_bAlwaysShowPos = !m_bAlwaysShowPos; }

	bool			isShowSkill(void) const				{ return m_bSkillView;		}
	void			setShowSkill(bool bEnable)			{ m_bSkillView = bEnable;	}
	i3TextNodeDX2*	GetSkillView(void) const			{ return m_pSkillCmdText;	}
};

#endif //_INGAMEQACOMMANDSET_H__