#include "pch.h"

#if legacy_gui_scene
#include "BattleHUD_Define.h"
//#include "BattleGUI_Result.h"
#include "BattleGUI_Stage_PreStart.h"
#include "BattleGui.h"
#include "EquipShapeManager.h"
#include "BattleHud.h"
#include "ConfigRenderBattle.h"

#include "GuiNotifyReceiver.h"

I3_CLASS_INSTANCE( CBattleGUI_Stage_PreStart);//, i3GameObjBase);

struct GUI_EDITBOX_INIT
{
	char			m_szName[128];
	INT32			m_idxStrInit;
	INT32			m_MaxLength;
	INT32			m_FontPt;
	INT32			m_AlignHorz;
	INT32			m_AlignVert;
} ;

CBattleGUI_Stage_PreStart::CBattleGUI_Stage_PreStart(void)
{
	m_pPanel = nullptr;

	m_pST_BossTeam = nullptr;
	m_pST_UnionTeam = nullptr;
	m_pST_Gauge = nullptr;

	m_pBossTopViewGuiStatic = nullptr;
	m_pBossExitGuiStatic = nullptr;

	m_pTextureHUD3 = nullptr;

	m_pTitleViewGuiStatic = nullptr;

	m_bVisible = false;
}

CBattleGUI_Stage_PreStart::~CBattleGUI_Stage_PreStart(void)
{
	I3_SAFE_RELEASE( m_pTextureHUD3 );
}

i3GuiEditBox * CBattleGUI_Stage_PreStart::_InitEditBox( const char * pszPath, INT32 maxLength, INT32 pt, INT32 alignX, INT32 alignY, const char * pszText)
{
	i3GuiEditBox * pCtrl;

	if((pszText != nullptr) && (maxLength == -1))
	{
		maxLength = i3::generic_string_size( pszText);
	}

	pCtrl = (i3GuiEditBox *) m_pPanel->FindControlByPath( pszPath);
	I3ASSERT( pCtrl != nullptr);

	pCtrl->ReCreateTextEx( GetDefaultFontName(), maxLength, pt);
	pCtrl->SetTextAlign( alignX, alignY);

	if( pszText != nullptr)
		pCtrl->SetText( pszText);

	return pCtrl;
}
//
bool CBattleGUI_Stage_PreStart::Create( CBattleGui * pGUI)
{
	m_pTextureHUD3 = crb::i()->LoadHudTexture( HUD_TEXTURE_HUD3);

	i3GuiRoot * pGUIRoot = pGUI->getGUIRoot();
	pGUIRoot->ADDControlByGUI( "Gui/Project/BOSS_Start.gui"); 

	m_pPanel = (i3GuiStatic *) pGUIRoot->GetControlByName( "BOSS_Start");
	I3ASSERT( m_pPanel);
	m_pPanel->SetPosToCenter();
	m_pPanel->SetEnable( false);
	
	m_pST_BossTeam = (i3GuiStatic *) m_pPanel->FindControlByPath( "Boss");
	I3ASSERT( m_pST_BossTeam);
	m_pST_BossTeam->SetEnable( false);

	i3GuiStatic * m_pWaitTeam = (i3GuiStatic *) m_pPanel->FindControlByPath( "Wait");
	I3ASSERT( m_pWaitTeam);
	m_pWaitTeam->SetEnable( false);

	m_pST_UnionTeam = (i3GuiStatic *) m_pPanel->FindControlByPath( "Union");
	I3ASSERT( m_pST_UnionTeam);
	m_pST_UnionTeam->SetEnable( false);

	m_pTitleViewGuiStatic = (i3GuiStatic *) m_pST_UnionTeam->FindControlByPath( "BOSS_Info_Team_Union");
	I3ASSERT( m_pTitleViewGuiStatic && "BossStage Gui 에 BOSS_Info_Team_Union 가 없다.");
	m_pTitleViewGuiStatic->ChangeFirstShape( m_pTextureHUD3, 272, 184, 177, 33 );

	m_pBossTopViewGuiStatic = (i3GuiStatic *) m_pST_UnionTeam->FindControlByPath( "TopView/TopViewImage");
	I3ASSERT( m_pBossTopViewGuiStatic && "BossStage Gui 에 Union/TopView/TopViewImage 가 없다.");
	m_pBossTopViewGuiStatic->ChangeFirstShape( m_pTextureHUD3, 18, 290, 194, 192 );

	m_pBossExitGuiStatic = (i3GuiStatic *) m_pST_UnionTeam->FindControlByPath( "Exit/ExitImage");
	I3ASSERT( m_pBossExitGuiStatic && "BossStage Gui 에 Union/Exit/ExitImage 가 없다.");
	m_pBossExitGuiStatic->ChangeFirstShape( m_pTextureHUD3, 228, 290, 194, 192 );

	m_pST_Gauge = (i3GuiProgressBar *) m_pPanel->FindControlByPath( "Gauge_Frame");
	I3ASSERT( m_pST_Gauge);
	m_pST_Gauge->SetEnable( false );


	_InitEditBox( "Union/Text", -1, 9, TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP,
		GAME_STRING("STR2_TBL_INGAME_HUD_HEADHUNTER") );

	return true;
}
//
void CBattleGUI_Stage_PreStart::Init(void)
{
}

void CBattleGUI_Stage_PreStart::SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, bool bDeath)
{
	bool bEnable = bFlag;

	if( bFlag)
	{
		// Respawn
		switch( nState)
		{
			case GAME_GUI_PRESTART_ROUND :
				bEnable = true;
				m_pST_UnionTeam->SetEnable( true );
				break;

			default :
				bEnable = false;
				break;
		}
	}

	if( m_bVisible != bEnable)
	{
		m_bVisible = bEnable;

		FLUSH_CONTROL(m_pPanel);
		m_pPanel->SetEnable( m_bVisible == true);

		SetEnable( m_pPanel->isEnable());
	}
}
#endif