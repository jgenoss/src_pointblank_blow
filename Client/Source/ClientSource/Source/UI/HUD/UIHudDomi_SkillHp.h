#pragma once

namespace DominationUI
{
	enum HUD_HP { HP_BG = 0, HP_BAR, HUD_HP_MAX};

	class skill_hp
	{
	private:
		REAL32 m_bar_width;
		REAL32 m_ratio;
		VEC3D m_pos;

		REAL32 m_view_width;
		REAL32 m_view_height;

		i3Sprite2D* m_sprite;

	public:
		skill_hp();
		~skill_hp();

		void set_ratio( REAL32 ratio) { m_ratio = ratio;}
		void set_enable( bool enable) { m_sprite->SetEnable( HP_BG, enable); m_sprite->SetEnable( HP_BAR, enable);}
		void set_pos( REAL32 x, REAL32 y, REAL32 len);
	};
};

namespace dui = DominationUI;
