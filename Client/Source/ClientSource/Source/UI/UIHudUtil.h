#pragma once

enum QUEST_STATE
{
	QST_NONE = -1,
	QST_NOT_STARTED = 0,
	QST_DOING,
	QST_COMPLETED
};

namespace HUD
{
	enum ChallengeMessage;
	enum TargetShapeType;
	enum ItemState;
}
namespace DinoState
{
	enum Attack;
}
enum ResultWinState;

namespace ResultState
{
	struct UserInform;
	typedef i3::vector< UserInform > UserInformList;
}

namespace hu	//hud util
{
	/*	Stage에 따라서 WeaponOnObject*가 다르기 때문에 미리 구한다.	 **/
	 void init_m179_object();

	/*	WeaponOnObject* 사용하는 Stage인지 알려준다.	 **/
	bool is_m179_mode();

	/*	타겟이 존재하는 모드 유무를 알려준다. (타켓 종류 : 발전소, 기린, 헬리곱터, 탱크..)
	 *		is_default_shape가 true 이면 (--- : --- 모양) , false는 (=== 모양) 이다.
	 **/
	bool is_target_mode(HUD::TargetShapeType & type, bool & is_default_shape);
	
	/*	pChara 유저가 들고 있는 무기에 대한 총탄 수를 알려준다.	 *	*/
	bool get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet);
	bool get_bullet_cnt(CGameCharaBase* pChara, INT32 & bullet, INT32 & max_bullet, INT32 & max_load_bullet);

	/*	pChara 유저가 들고 있는 무기에 대한 총탄 수를 알려준다.	 *	*/
	bool get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet);
	bool get_bullet_cntF(CGameCharaBase* pChara, REAL32 & bullet, REAL32 & max_bullet, REAL32 & max_load_bullet);

	/*	pChara 유저가 들고 있는 무기에 대한 Slot Type을 알려준다.	 *	*/
	WEAPON_SLOT_TYPE get_weapon_slot_type(CGameCharaBase* pChara);

	/*	pChara 유저의 재장전 타입을 알려준다	 *	*/
	bool get_weapon_bullet_type(CGameCharaBase* pChara);
	
	/*	WeaponM197 무기에 대한 총탄 수를 알려준다.(gauge 값이다)	 *	*/
	void get_m179_gauge(REAL32 & gauge, REAL32 & max_gauge);

	/*	target object에 대한 hp를 알려준다.	**/
	void get_target_object_hp(INT32 & red_hp, INT32 & max_red_hp, INT32 & blue_hp, INT32 & max_blue_hp);

	/*	pChara에 대한 hp를 알려준다.(dino 도 가능)	**/
	void get_chara_hp(CGameCharaBase* pChara, INT32 & hp, INT32 & max_hp);
	void get_chara_hp(CGameCharaBase* pChara, REAL32 & per); //100% 로 알려준다.
	REAL32 get_chara_hp_ratio(CGameCharaBase* pChara);

	/*	WeaponOnObject* 의 이름을 알려준다. **/
	i3::rc_wstring get_m179_name();
	
	/*	pChara가 들고 있는 무기 이름을 알려준다.(없는 경우 "" 반환) **/
	i3::rc_wstring get_weapon_name(CGameCharaBase* pChara);

	/*	현재 챌린지 모드 난이도를 알려준다. **/
	i3::rc_wstring get_challenge_difficulty_text(INT32 diff);
	INT32 get_challenge_difficulty_idx(INT32 diff);
	
	/*	현재 챌린지 모드 점수를 알려준다. **/
	INT32 get_challenge_point();

	/*	현재 캐릭터에 대한 킬/데스/어시스트를 알려준다. **/
	void get_kill_death_assist(INT32 slot_idx, INT32 & kill, INT32 & death, INT32 & assist);
	void get_kill_death_assist(INT32 & kill, INT32 & death, INT32 & assist);
	void get_KDA_to_string(i3::wstring & kill, i3::wstring & assist, i3::wstring & death);

	/*	HUD::ChallengeMessage 에 대한 챌린지 모드 문자를 알려준다. (없는 경우 "" 반환) **/
	i3::rc_wstring get_challenge_msg(HUD::ChallengeMessage idx);

	/*	idx 에 대한 튜토리얼 모드 문자를 알려준다. (없는 경우 "" 반환) **/
	i3::rc_wstring get_tutorial_msg(INT32 idx);

	/*	인게임 진행 시간을 알려준다. **/
	void set_running_time(UINT32 time);
	void get_running_time(INT32 & minute, INT32 & second);

	/* GM 포즈기능 사용시 대쉬보드의 시간을 정지시킴 **/
	void set_time_pause(bool paused);
	
	/*	인게임 팀 점수를 알려준다. **/
	void get_team_score(INT32 & red, INT32 & blue);

	/* mission 활성화 유무를 알려준다. **/
	bool is_enable_mission();

	/*	mission_idx에 대한 미션 내용, 상태를 알려준다.	*/
	QUEST_STATE get_mission_info(INT32 mission_idx, i3::wstring & text);
	
	/*	mission_idx에 대한 튜토리얼 내용, 상태를 알려준다.	*/
	QUEST_STATE get_tutorial_info(INT32 mission_idx, i3::wstring & text);

	/*	현재 stage에 대한 목표 점수를 알려준다. (is_round가 true 이면 round 형식)	*/
	INT32 get_round_goal_count(bool & is_round);
	
	/* 자기가 속한팀을 구합니다 */
	CHARACTER_TEAM_TYPE getMyTeam();

	/*	pPlayer에 대한 dino, human 마크를 알려준다.	*/
	TEAM_TYPE get_team_mark(CGameCharaBase* pPlayer, INT32 & red, INT32 & blue);

	/*	탈출 미션에 block을 알기 위해 시간을 공급한다.	*/
	void update_escape_block_time(REAL32 time);

	/*	탈출 미션에 현재 block 시간 상태를 알려준다.	*/
	INT32 get_escape_block_time_shape_idx();
	
	/*	3D 좌표(vIn) -> 2D 좌표(fOutX, fOutY)	*/
	bool sceneToScreen(REAL32 * fOutX, REAL32 * fOutY, VEC3D * vIn);

	/*	pChara 위치에 따른 target object에 대한 좌표, 크기를 알려준다.	*/
	bool get_target_object_position(CGameCharaBase* pChara, VEC2D & pos, VEC2D & size);

	/*	인게임 현재 접속한 유저 리스트 얻기
	 *		std::pair<INT32, i3::rc_string> : netIdx, nick name
	 *		add_me : 본인 포함 유무
	 *		return : 본인 팀(0 또는 1)
	 **/
	INT32 get_ingame_user_list(i3::vector< i3::pair<INT32, i3::rc_wstring> > & list, bool add_me); //인게임

	/*	임게임에 현재 접속한 유저 리스트를 red, blue 팀으로 분활 한다.
	 *		src : get_ingame_user_list 함수에서 얻은 user list 또는 구분이 안 된 user list
	 *		lhs : red user list
	 *		rhs : blue user list
	**/
	void partition_ingame_user_list(const i3::vector< i3::pair<INT32, i3::rc_wstring> > & src,
		i3::vector< i3::pair<INT32, i3::rc_wstring> > & lhs,
		i3::vector< i3::pair<INT32, i3::rc_wstring> > & rhs);

	/*	주무기 첫 유저 킬에 사용되는 helper fuctions..
	 **/
	void enable_check_primary_weapon_first_shot(); //리스폰 후에 호출된다.
	void diable_check_primary_weapon_first_shot(); //무기를 버리면 호출된다.
	bool is_primary_weapon_first_shot(CGameCharaBase* pChara);

	/*	현재 캐릭터에 액션 상태를 반환한다.
	 **/
	CHARA_ACTION_BODYLOWER get_chara_action_state(CGameCharaBase* pChara);

	/*	현재 캐릭터에 파츠, C5 장착 유무를 반환한다.
	 **/
	HUD::ItemState get_helmet_protect_state(CGameCharaBase* pChara);
	HUD::ItemState get_c5_state(CGameCharaBase* pChara);
	
	/*	리스폰 단축 아이템 사용 여부에 따른 String을 반환 한다.
	 *		return : 단축 아이템 사용에 따른 String 또는 사용 안 할 경우 "" 반환
	 **/
	i3::rc_wstring get_text_using_respawn_item();
}

namespace hc //hud control
{
	void btn_set_shape(i3UIButtonImageSet* p, INT32 idx); //-1은 비활성화다.
}

namespace hu_dino 
{
	//hud에 표시할 정보를 담고 잇다.
	struct attribute
	{
		attribute() : ratio(1.0f) {}

		DinoState::Attack idx1, idx2;
		REAL32 ratio;
		i3::rc_wstring	wstr_name;
	};
	extern attribute att;

	void reset_dino_attribute();
	void set_dino_attribute(const i3::rc_wstring& name, DinoState::Attack idx1, DinoState::Attack idx2) ;
	void set_dino_raio(REAL32 ratio);
	i3::rc_wstring get_dino_attck_string(DinoState::Attack idx);
	INT32 get_dino_shape_index();
}

namespace hu
{
	void sort_ingame_result_inform(i3::vector< ResultState::UserInform > & list); //킬, 데스, point, exp, nick name 정렬

	template <class UnaryPredicate>
	inline void sort_ingame_result_inform(i3::vector< ResultState::UserInform > & list, UnaryPredicate pred, bool use_stable_sort) 
	{
		if(use_stable_sort == true)
			std::stable_sort(list.begin(), list.end(), pred); 
		else
			std::sort(list.begin(), list.end(), pred);
	}

	void get_ingame_result_inform( ResultState::UserInformList & red_list, ResultState::UserInformList & blue_list);
	bool is_my_team_win();

	//혜택의 종류를 알기 위해서는 bitmask으로 benefit_mark값을 분석해야 된다.
	INT32 get_benefit_mark(INT32 benefit_mark, INT32 benefit_mask);
}

namespace hu
{
	bool is_zero_durability_weapon( T_ItemID ItemID,T_ItemDBIdx ware_idx); //내구도가 0인지 아닌지 판단.
}

//***************************//
// Hud Unit Test
namespace hu_ut
{
#if ENABLE_UNIT_TEST
	extern struct _test_factor
	{
		CWeaponInfo* pInfo;
		STAGE_ID stage_id;
		CHARA_ACTION_BODYLOWER chara_action_body_lower;
		HUD::ItemState headgear_parts_state, c5_state;
		REAL32 escape_block_time;
		INT32 bullet;//, max_bullet, max_load_bullet;
		INT32 hp, kill, death, challenge_total_score;
		INT32 remain_battle_time;
		INT32 red_score, blue_score;
		INT32 red_object_hp, blue_object_hp;
		INT32 round_goal_count;
		UINT8 room_sub_type;
		bool chara_dino_type, changing_hud;
	} test_factor;

	void change_test_factor();
#endif

	//#if defined(NC_BUILD) 도 사용한다.
	STAGE_ID	get_stage_id();
	STAGE_MODE	get_stage_type();
	UINT8 get_my_room_sub_type();
	bool is_test_changing_hud();
	bool is_kill_mode();
	bool is_stage_mask_training();
	bool is_stage_mask_madness();
	bool is_stage_mask_diehard();
	bool is_domination_mode();
	bool is_dino_escape_mode();
	bool is_cross_count_mode();
	bool is_annihilation_mode();
	bool is_tutorial_mode();
	bool is_destroy_mission_mode();
	bool is_defence_mission_mode();
	bool is_bomb_mission_mode();
	bool is_dino(CGameCharaBase* pMyChara);
}