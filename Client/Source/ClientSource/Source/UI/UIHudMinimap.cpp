#include "pch.h"
#include "UIHudMinimap.h"

#include "BattleHud.h"
#include "BattleHUD_MiniMap.h"

#include "UIUtil.h"
#include "UIHUDIngameChat.h"
#include "MyRoomInfoContext.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "UIBattleFrame.h"
#include "UIHudManager.h"


UIHudMinimap::UIHudMinimap(UIHudManager* p) : iHudBase(p), m_pAreaName(nullptr)
{
	m_name = "Minimap";
}

UIHudMinimap::~UIHudMinimap()
{
}

/*********************************/
void UIHudMinimap::InitializeAtLoad( i3UIScene * pScene)
{
	i3_prof_func();

	iHudBase::AddFrameWnd(pScene, "Minimap");
	m_pAreaFrmWnd = FindControl<i3UIFrameWnd*>(pScene, "mm_Frm_Area");
	m_pAreaName = FindControl<i3UIStaticText*>(pScene, "mm_ST_Area");
}

void	UIHudMinimap::Update( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	UIHUDIngameChat* pChat = UIHUDIngameChat::instance();

	if( CHud::i() == nullptr) return;
	if( pChat)
	{
		if( pChat->GetEnableChat() ) return;
	}
	CBattleHUD_MiniMap* mini_map = CBattleHUD_MiniMap::instance();

	HUD_MODE HudMode = CHud::i()->getHudMode();

	UINT64 nStrokeKey	= g_pFramework->getKeyStroke();	

	if( mini_map != nullptr)
	{
		if( nStrokeKey & GAME_KEY_FUNC_TOGGLEMAP )
		{
		}

		if( HudMode != HUD_MODE_MISSION_OBSERVER)
		{
			i3InputKeyboard * pKey = g_pFramework->getKeyboard();
			if( (nStrokeKey & GAME_KEY_PLUS) || ( pKey->GetStrokeState( I3I_KEY_NUMPADPLUS)))
			{
				mini_map->IncreaseMiniMapScale( true);
			}
			else if( (nStrokeKey & GAME_KEY_MINUS) || ( pKey->GetStrokeState( I3I_KEY_NUMPADMINUS)))
			{
				mini_map->IncreaseMiniMapScale( false);
			}
		}
	}
}

void UIHudMinimap::SetParameter(INT32 arg1, INT32 arg2)
{
	i3_prof_func();

	enum { BTN =0, };
	enum { ENLARGE= 0, REDUCE=1, };

	switch( arg1 )
	{
	case BTN:
		{
			switch( arg2 )
			{
			case ENLARGE:	CBattleHUD_MiniMap::instance()->IncreaseMiniMapScale( true);			break;
			case REDUCE:		CBattleHUD_MiniMap::instance()->IncreaseMiniMapScale( false);			break;
			}
		}
		break;
	}
}

/*********************************/
void UIHudMinimap::event_start_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();

	iHudBase::SetEnable(true);
}

void UIHudMinimap::event_respawn_battle(INT32 arg, const i3::user_data& UserData)
{
	i3_prof_func();
	UIHUDBase *	pHud = UIBattleFrame::i()->GetHUD(UIHUD_TABMINIMAP);

	if(pHud && pHud->isVisible() == false)
	{
		i3::pack::RECT rect;
		m_pAreaFrmWnd->getAccumulatedRect(&rect);

		i3UIManager* uiMan = i3UI::getManager();
		REAL32 ratioX = uiMan->getScreenStretchRatioX();
		REAL32 ratioY = uiMan->getScreenStretchRatioY();

		INT32 left = abs(GameUI::AutoPosition::get_max_left() - (INT32)rect.left);
		INT32 top = abs(GameUI::AutoPosition::get_max_top() - (INT32)rect.top);

		rect.left /= ratioX;	rect.top /= ratioY;		
		rect.right /= ratioX;	rect.bottom /= ratioY;

		left = static_cast<INT32>((REAL32)left / ratioX); 
		top = static_cast<INT32>((REAL32)top / ratioY);

		CBattleHUD_MiniMap* pMinMap = CBattleHUD_MiniMap::instance();

		if(pMinMap)
		{
			pMinMap->SetEnableMiniMap(true);
			pMinMap->SetPosition(left, top);
			pMinMap->SetSize((INT32)rect.Width(), (INT32)rect.Height());
			pMinMap->SetMiniMapMode(CBattleHUD_MiniMap::instance()->GetMiniMapMode(), true);
		}

		if (MyRoomInfoContext::i()->getStage()->IsHudOff())
		{
			if (pMinMap)
				pMinMap->SetEnableMiniMap(false);
			SetEnable(false);
		}
	}
}

void UIHudMinimap::event_m_pre_roundstart_battle(INT32 arg, const i3::user_data& UserData)
{
	iHudBase::SetEnable(true);
}

/*********************************/
void	UIHudMinimap::SetMiniMapLocationText( const char* pszLocation)
{
	i3_prof_func();
	if( m_pAreaName != nullptr)
		m_pAreaName->SetTextMB(pszLocation);
}

void	UIHudMinimap::SetObjectUnderAttack( INT32 idx, bool bFlag )
{
	i3_prof_func();
	CBattleHUD_MiniMap* pMiniMap = CBattleHUD_MiniMap::instance();

	if (pMiniMap != nullptr)
	{
		switch (MyRoomInfoContext::i()->getStage()->GetStageMode())
		{
		case STAGE_MODE_BOMB:
		case STAGE_MODE_DESTROY:
		case STAGE_MODE_DEFENCE:
			{
				pMiniMap->SetDestroyObjectUnderAttack(idx, bFlag);
			}
			break;
		}
	}
}