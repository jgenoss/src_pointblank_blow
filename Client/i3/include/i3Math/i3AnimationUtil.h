#if !defined( __I3_MATH_ANIMATION_H)
#define __I3_MATH_ANIMATION_H

enum I3_ANIM_PLAY_MODE
{
	I3_ANIM_PLAY_MODE_STOP		= 0,		//설정한 애니메이션의 듀레이션이상 플레이 되면 애니메이션의 0 프레임으로.
	I3_ANIM_PLAY_MODE_REPEAT,				//설정한 애니메이션의 듀레이션이상 플레이 되면 애니메이션의 0 프레임으로 다시 재생.
	I3_ANIM_PLAY_MODE_CLAMP,				//설정한 애니메이션의 듀레이션이상 플레이 되면 애니메이션의 마지막 프레임 유지.
	I3_ANIM_PLAY_MODE_BOUNCE,				//구현이 안되어 있음.
	I3_ANIM_PLAY_MODE_MANUAL_CLAMP			//설정한 애니메이션에 설정한 manual_clamp_time 플레이 되면 애니메이션에 설정한 manual_clamp_time으로 프레임 유지.
};

enum I3_ANIM_BLEND_MODE
{
	I3_ANIM_BLEND_MODE_REPLACE		= 0,
	I3_ANIM_BLEND_MODE_BLEND,
};

#endif
