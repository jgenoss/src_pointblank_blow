#pragma once

namespace dboard //dash board
{
	struct target
	{
	public:
		target() {}
		virtual ~target() {}

		virtual void set_enable(bool enable) = 0;

		void init_control(i3UIScene* pScn, const char* parent, const char* targ, const char* bg, const char* progress, bool bMinimapUpdate = true);

		void update(REAL32 rDeltaSeconds);
		void update_warning(REAL32 rDeltaSeconds);
		void update_color();
		
		void set_bg_idx(INT32 idx, bool enable); //뒷 배경 및 시작 및 warning 후 활성화 상태
		void set_target_normal_shape_idx(INT32 idx); //target 평상시 인덱스
		void set_target_die_shape_idx(INT32 idx); //target 죽은 후 인덱스
		void set_minimap_warning_idx(INT32 idx);
		void set_progress_invert();
		void set_enable_update(bool enable);
		void reset_target_shape();

		void set_hp(INT32 hp, INT32 hp_max);

	protected:
		i3UIFrameWnd* pImgSet_Wnd; //pImgSet_Bg, pImgSet_Target, pImgSet_Target 부모
		i3UIButtonImageSet* pImgSet_Bg; //target에 대한 bg
		i3UIButtonImageSet* pImgSet_Target; //target (둘다 사용)
		i3UIProgressBar* pProgressgBar;

		INT32 hp, max_hp, lastest_hp;
		INT32 delta_time, warning_cnt;
		INT32 minimap_idx; //warning 당할 때 minimap 표시 index
		INT32 target_normal_shape_idx, target_die_shape_idx;	//progress bar가 0 된 후에 target shape index

		bool enable_target;
		bool is_after_warning_bg_enable; //warning 할 시 나오는 색상에 대해서 turn 후에 표시 유무
		bool is_update_warning; //공격 당하는 유무
		bool is_finish_update; //update 유무
		bool is_minimap_update; // warning을 minimap 표시 유무
	};

	struct target_1_5 : public target
	{
	public:
		target_1_5() {}
		virtual ~target_1_5() {}

		virtual void set_enable(bool enable);
	};

	struct target_1_0 : public target
	{
	public:
		target_1_0() {}
		virtual ~target_1_0() {}

		virtual void set_enable(bool enable);
	};
}