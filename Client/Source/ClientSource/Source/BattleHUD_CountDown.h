#pragma once

#define COUNTDOWN 3

class CHUD_CountDown : public i3GameObjBase
{
	I3_CLASS_DEFINE(CHUD_CountDown, i3GameObjBase);

public:
	CHUD_CountDown(void);
	~CHUD_CountDown(void);

public:
	static CHUD_CountDown* instance();

public:
	void CreateHUD(i3AttrSet* pHudRoot);
	void OnUpdate( REAL32 rDeltaSeconds );

public:
	void start_count_down();
	void stop_count_down();
	bool is_act_count_down() const { return pf_update == &CHUD_CountDown::update_count_down_impl; }

private:
	void update_count_down_impl(REAL32 rDeltaSeconds);
	void update_none_impl(REAL32 rDeltaSeconds) {}

	void modify_count_down_size(REAL32 factor, bool all_shape); //매개인자(1.f, true) 초기 상태로 변경 된다.
	void modify_count_down_alpha(REAL32 factor, bool all_shape);

private:
	enum CountState { CS_Num1, CS_Num2, CS_Num3, CS_NumMax };
	enum ImgState { IS_Width = 110, IS_Height = 110, };

	struct Sprite
	{
		Sprite() : texture(0), sprite(0) {}
		~Sprite() { I3_SAFE_RELEASE(texture); I3_SAFE_NODE_RELEASE(sprite); }

		i3Texture *texture;
		i3Sprite2D *sprite;
	};

	Sprite m_BgSprite, m_CountSprite;
	REAL32 m_origin_center_x, m_origin_center_y;
	REAL32 m_elapse_time;
	INT32 m_curr_count_num;

	i3SoundContext*     m_pSoundContext;
	i3SoundPlayInfo*	m_pCountDownSound[COUNTDOWN];

	void (CHUD_CountDown::*pf_update)(REAL32 rDeltaSeconds);
};
