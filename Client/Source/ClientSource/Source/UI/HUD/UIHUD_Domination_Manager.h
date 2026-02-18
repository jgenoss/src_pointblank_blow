#pragma once

#include "..\UIHUDBase.h"
#include "UIHUD_Domination_Base.h"
#include "UIHudDomi_Skill.h"
#include "UIHudDomi_Indicator.h"
#include "UIHudDomi_Combo.h"
#include "UIHudDomi_SpecialScore.h"
#include "UIHudDomi_Flow.h"
#include "UIHudDomi_WaveResult.h"
#include "UIHudDomi_RoundResult.h"
#include "UIHudDomi_ScoreQueue.h"
#include "UIHudDomi_UserState.h"

#include "tinst.h"

enum	HUD_DOMINATION_TYPE
{
	HUD_DOMI_WAVE_RESULT,
	HUD_DOMI_ROUND_RESULT,
	HUD_DOMI_SKILL,
	HUD_DOMI_INDICATOR,
	HUD_DOMI_COMBO,
	HUD_DOMI_SPECIAL_SCORE,
	HUD_DOMI_FLOW,
	HUD_DOMI_SCORE_QUEUE,
	HUD_DOMI_USER_STATE,

	HUD_DMN_MAX
};

namespace DominationUI
{
	class DinoHp
	{
		i3AttrSet* m_pHUDRoot;
		i3Texture* m_pTexture;

		i3Sprite2D* m_pSprite;
		i3TextNodeDX2* m_pText;

	public:
		DinoHp() : m_pHUDRoot(0), m_pTexture(0), m_pSprite(0), m_pText(0) {}
		~DinoHp() {}

		void create();
		void destroy();
		void print( bool enable, CGameCharaBase* chara = nullptr);
	};
};
namespace dui = DominationUI;


template<> struct is_tinst_manual<class UIHUDDominationMgr> { static const bool value = true; };

class UIHUDDominationMgr : public i3GameNode
{
	I3_CLASS_DEFINE( UIHUDDominationMgr, i3GameNode);

public:
	static UIHUDDominationMgr*	i()	{ return tinst<UIHUDDominationMgr>(); }

protected:
	UIHUDDominationBase*			m_pHudList[ HUD_DMN_MAX];

	dui::DinoHp m_dino_hp;
public:
	UIHUDDominationMgr(void);
	virtual ~UIHUDDominationMgr(void);

	virtual void	Create( i3GameNode* pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSec) override;
	void			Initialize(void);

	void	EnableHUD( HUD_DOMINATION_TYPE type, bool bEnable);
	void	VisibleHUD( HUD_DOMINATION_TYPE type, bool bVisible)		{ return m_pHudList[ type]->SetVisible( bVisible);	}

	void	SetEnableAll( bool bEnable = true)							{ for( INT32 i = 0; i < HUD_DMN_MAX; i++ ) EnableHUD( (HUD_DOMINATION_TYPE)i, bEnable);	}
	bool 	SetHudEvent( HUD_DOMINATION_TYPE type, INT32 nEventNum, INT32 arg = 0);

	UIHUDDominationBase*	GetHUD( HUD_DOMINATION_TYPE type)			{ return m_pHudList[ type];					}
	dui::DinoHp& GetHUD_DinoHp() { return m_dino_hp;}
};

tinst<UIHUDDominationMgr>;

