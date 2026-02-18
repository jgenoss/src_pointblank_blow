#include "pch.h"
#include "UIHudDomi_SkillHp.h"

#include "../../ConfigRenderBattle.h"
#include "../../BattleHud.h"

namespace DominationUI
{
	skill_hp::skill_hp() : m_ratio(0.f)
	{
		i3Texture* texture = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);

		m_sprite = i3Sprite2D::new_object();
		m_sprite->SetTexture( texture);
		I3_MUST_RELEASE(texture);
		 
		m_sprite->Create( HUD_HP_MAX, true, false);
 
		m_sprite->SetTextureCoord( HP_BG, 428.f, 224.f, 534.f, 266.f);
		m_sprite->SetTextureCoord( HP_BAR, 467.f, 275.f, 527.f, 281.f); 
  
		m_sprite->SetRect( HP_BG, 0.f, 0.f, 106.f, 42.f);
		m_sprite->SetRect( HP_BAR, 38.f, 17.f, 62.f, 6.f);	//»ÂπÃ..

		if(CHud::i())
			CHud::i()->getHUDRoot()->AddChild( m_sprite);
		m_sprite->SetEnable( HP_BG, true);
		m_sprite->SetEnable( HP_BAR, true);

		m_ratio = 1.f;
		m_bar_width = 62.f;
		i3Vector::Zero( &m_pos);

		i3Viewer* pViewer = g_pFramework->GetViewer();
		m_view_width = (REAL32)pViewer->GetViewWidth();
		m_view_height = (REAL32)pViewer->GetViewHeight();
	}

	skill_hp::~skill_hp()
	{
		set_enable( false);
		I3_SAFE_NODE_RELEASE( m_sprite);
	}

	void skill_hp::set_pos( REAL32 x, REAL32 y, REAL32 len)
	{
		REAL32 fPosX = (m_view_width * x) - (106.f * 0.5f);
		REAL32 fPosY = (m_view_height * y) - len;

		m_pos.x = fPosX; m_pos.y = fPosY;

		m_sprite->SetRect( HP_BG, m_pos.x, m_pos.y, 106.f, 42.f);
		m_sprite->SetRect( HP_BAR, m_pos.x + 38.f, m_pos.y + 17.f, m_bar_width * m_ratio, 6.f);	//»ÂπÃ..
	}
};
