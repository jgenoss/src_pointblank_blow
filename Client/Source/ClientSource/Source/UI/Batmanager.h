#if !defined( __UI_BATMAN_H__)
#define __UI_BATMAN_H__

/*	치트키 용도임.. 별 다른것 없음.
*/
namespace BaTMaN
{
	extern bool use_batman;
	extern bool disable_ai_attack;
	extern bool gm_observer_mode;
	extern bool enable_input_away_checker;
	extern bool enable_native_language;
	extern bool enable_jump_booster;
	extern bool enable_debug_item_name;
	extern bool enable_move_inven_wnd;

	extern INT32 jump_booster_factor;
	extern REAL32 magnification_respawn;

	void init_batman();

	void BatMan_Update(REAL32 rDeltaSeconds);
	void BatMan_Ingame_Update(REAL32 rDeltaSeconds);
	void BatMan_Outgame_Update(REAL32 rDeltaSeconds);
}

#endif
