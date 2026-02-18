#include "pch.h"
#include "BattleGUI_BOSS_PreStart.h"

#include "BattleGui.h"
#include "BattleHUD_Define.h"
#include "StageBattle.h"

#include "EquipShapeManager.h"
#include "BattleHud.h"

#include "GameGUI.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif

#include "../StageBattle/UserContext.h"

I3_CLASS_INSTANCE( CBattleGUI_BOSS_PreStart); //, i3GameObjBase);

struct GUI_EDITBOX_INIT
{
	char			m_szName[128];
	INT32			m_idxStrInit;
	INT32			m_MaxLength;
	INT32			m_FontPt;
	INT32			m_AlignHorz;
	INT32			m_AlignVert;
} ;

static GUI_EDITBOX_INIT		s_STATIC_TEXT_INIT[] =
{
	//   Control Name						StringTable Index			         MaxLength  FontPoint    Align(H)      Align(V)
	{	"Union/Text",				-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_TOP },
	{	"Union/TopView/TopViewName",-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Union/Exit/ExitName",		-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Boss/Text",				-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_TOP },
	{	"Boss/TRex/Name",			-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Boss/TRex/KeyMove_Text",	-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Boss/TRex/1_Text",			-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Boss/TRex/2_Text",			-1,		-1,		9,		TEXT_ATTR_ALIGN_LEFT,	TEXT_ATTR_ALIGN_MIDDLE },
	{	"Wait/WaitText",			-1,		-1,		9,		TEXT_ATTR_ALIGN_CENTER,	TEXT_ATTR_ALIGN_MIDDLE },

	{	"",						-1,										0,		0,		0,						0}
};

CBattleGUI_BOSS_PreStart::CBattleGUI_BOSS_PreStart(void)
{
	m_pPanel = 0;

	m_pST_BossTeam = 0;
	m_pST_UnionTeam = 0;
	m_pST_Gauge = 0;

	m_pBossTopViewGuiStatic = 0;
	m_pBossExitGuiStatic = 0;

	s_STATIC_TEXT_INIT[0].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_BOSS_UNION_HELP");
	s_STATIC_TEXT_INIT[1].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ESCAPE_TOPVIEW");
	s_STATIC_TEXT_INIT[2].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ESCAPE_IMAGE");		
	s_STATIC_TEXT_INIT[3].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_BOSS_BOSS_HELP");
	s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_TREX");		
	s_STATIC_TEXT_INIT[5].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_BOSS_KEYHELP");
	s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B");
	s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_ATTACK_HOWL");		
	s_STATIC_TEXT_INIT[8].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_PRESTART_ROUND");

	m_bVisible = false;
}

CBattleGUI_BOSS_PreStart::~CBattleGUI_BOSS_PreStart(void)
{
}

i3GuiEditBox * CBattleGUI_BOSS_PreStart::_InitEditBox( const char * pszPath, INT32 maxLength, INT32 pt, INT32 alignX, INT32 alignY, const i3::wliteral_range& wstrText)
{
	i3GuiEditBox * pCtrl;

//	if((pszText != nullptr) && (maxLength == -1))
//	{
//		maxLength = i3::generic_string_size( pszText);
//	}
	
	if (maxLength == -1)
		maxLength = wstrText.size();
	
	pCtrl = (i3GuiEditBox *) m_pPanel->FindControlByPath( pszPath);
	I3ASSERT( pCtrl != nullptr);

	pCtrl->ReCreateTextEx( GetDefaultFontName(), maxLength, pt);
	pCtrl->SetTextAlign( alignX, alignY);

//	if( pszText != nullptr)
	pCtrl->SetText( wstrText);

	return pCtrl;
}

bool CBattleGUI_BOSS_PreStart::Create( CBattleGui * pGUI)
{
	i3GuiRoot * pGUIRoot;
	INT32 i;

	pGUIRoot = pGUI->getGUIRoot();

	pGUIRoot->ADDControlByGUI( "Gui/Project/BOSS_Start.gui");

	m_pPanel = (i3GuiStatic *) pGUIRoot->GetControlByName( "BOSS_Start");
	I3ASSERT( m_pPanel);
	
	m_pPanel->SetEnable( false);

	// Static Text 초기화
	{
		i3::rc_wstring wstrText;

		for( i = 0; s_STATIC_TEXT_INIT[i].m_szName[0] != 0; i++)
		{
		//	const char * pszText = nullptr;

			wstrText.clear();

			const GUI_EDITBOX_INIT * pInfo = &s_STATIC_TEXT_INIT[i];

			if( pInfo->m_idxStrInit != -1)
			{
				wstrText = GAME_STRING_BY_HANDLE(pInfo->m_idxStrInit);
				if( wstrText.empty() )
				{
					I3PRINTLOG(I3LOG_FATAL, "탈출 미션에 들어가는 스트링[ %s ] 이 번역이 안 되어 있다.", s_STATIC_TEXT_INIT[i].m_szName );
				}
			}

			_InitEditBox( (char *) pInfo->m_szName, pInfo->m_MaxLength, pInfo->m_FontPt, 
							s_STATIC_TEXT_INIT[i].m_AlignHorz, s_STATIC_TEXT_INIT[i].m_AlignVert, wstrText);
		}
	}

	m_pST_BossTeam = (i3GuiStatic *) m_pPanel->FindControlByPath( "Boss");
	I3ASSERT( m_pST_BossTeam);


	m_pST_UnionTeam = (i3GuiStatic *) m_pPanel->FindControlByPath( "Union");
	I3ASSERT( m_pST_UnionTeam);

	m_pBossTopViewGuiStatic = (i3GuiStatic *) m_pST_UnionTeam->FindControlByPath( "TopView/TopViewImage");
	I3ASSERT( m_pBossTopViewGuiStatic && "BossStage Gui 에 Union/TopView/TopViewImage 가 없다.");

	m_pBossExitGuiStatic = (i3GuiStatic *) m_pST_UnionTeam->FindControlByPath( "Exit/ExitImage");
	I3ASSERT( m_pBossExitGuiStatic && "BossStage Gui 에 Union/Exit/ExitImage 가 없다.");

	m_pST_Gauge = (i3GuiProgressBar *) m_pPanel->FindControlByPath( "Gauge_Frame/Gauge");
	I3ASSERT( m_pST_Gauge);

	m_pST_BossTeam->SetEnable( false);
	m_pST_UnionTeam->SetEnable( false);

	m_pPanel->SetPosToCenter();
	m_pST_Gauge->ResetProgressBar();

	return true;
}

void CBattleGUI_BOSS_PreStart::Init(void)
{
}

void CBattleGUI_BOSS_PreStart::SetEnableGUI( bool bFlag, GAME_GUI_STATUS nState, bool bDeath)
{
	//bool bEnable = bFlag;

	//if( bFlag)
	//{
	//	// Respawn
	//	switch( nState)
	//	{
	//		case GAME_GUI_PRESTART_ROUND :
	//			if( this->isEnable() == false)
	//			{
	//				m_bVisible = bEnable;

	//				GameGUI::FLUSH_CONTROL(m_pPanel);
	//				m_pPanel->SetEnable(m_bVisible == true);

	//				SetEnable(m_pPanel->isEnable());

	//				g_pFramework->GetEquipShapeManager()->SetBossStageShape(
	//					m_pBossTopViewGuiStatic, 
	//					m_pBossExitGuiStatic,
	//					MyRoomInfoContext::i()->getStageID() );

	//				{
	//					m_pST_Gauge->ResetProgressBar();
	//					m_pST_Gauge->Start( 5.0f, I3GUI_PROGRESSBAR_LEFT_TO_RIGHT);

	//					CGameCharaBase * pMyChara = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	//					I3ASSERT( pMyChara != nullptr);
	//					

	//					
	//					if( UserContext::i()->IsPlayingSlot(BattleSlotContext::i()->getMySlotIdx()) == false )
	//					{				
	//					}
	//					else
	//					{

	//						if( pMyChara->IsDino())
	//						{
	//							if( i3::kind_of<CGameCharaDinoSting*>(pMyChara)) //->IsSting() || pMyChara->IsCCSting())
	//							{
	//								if( i3::same_of<CGameCharaDinoStingMars*>(pMyChara) )
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_MARS");	
	//								else
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_STING");	

	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_GAS");
	//							}
	//							else if( i3::kind_of<CGameCharaDinoRaptor*>(pMyChara) ) // (pMyChara->IsRaptor() || pMyChara->IsCCRaptor() || pMyChara->IsRaptorBlue() || pMyChara->IsRaptorGreen())
	//							{
	//								if( i3::same_of<CGameCharaDinoRaptorMercury*>(pMyChara) )
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_MERCURY");
	//								else
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_RAPTOR");

	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_RAPTER_ATTACK_A");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_RAPTER_ATTACK_B");
	//							}
	//							else if( i3::kind_of<CGameCharaDinoTank*>(pMyChara) ) // (pMyChara->IsTank())
	//							{
	//								s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_TANK");
	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_TANK_ATTACK_A");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_TANK_ATTACK_B");	

	//							}
	//							else if( i3::kind_of<CGameCharaDinoAcid*>(pMyChara) ) // (pMyChara->IsAcid() || pMyChara->IsCCAcid())							)
	//							{
	//								if( i3::same_of<CGameCharaDinoAcidVulcan*>(pMyChara) )
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_VULCAN");
	//								else
	//									s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_ACID");

	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ACID_ATTACK_A");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ACID_ATTACK_B");	

	//							}
	//							else if( i3::kind_of<CGameCharaDinoElite*>(pMyChara) ) // (pMyChara->IsElite())
	//							{
	//								s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_ELITE");
	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ELITE_ATTACK_A");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ELITE_ATTACK_B");	

	//							}
	//							/*else if (pMyChara->IsRaptorMercury())
	//							{
	//								
	//							}*/
	//							/*else if (pMyChara->IsStingMars())
	//							{
	//								s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_MARS");	
	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_PRICKLE");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_ATTACK_POISON_GAS");
	//							}*/
	//							/*else if (pMyChara->IsAcidVulcan())
	//							{
	//								s_STATIC_TEXT_INIT[4].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_DINO_TYPE_VULCAN");
	//								s_STATIC_TEXT_INIT[6].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ACID_ATTACK_A");
	//								s_STATIC_TEXT_INIT[7].m_idxStrInit = GET_GAME_STRING_HANDLE("STR_TBL_INGAME_HUD_ACID_ATTACK_B");
	//							}*/
	//							
	//							i3::rc_wstring wstrText;

	//							for( INT32 i = 0; s_STATIC_TEXT_INIT[i].m_szName[0] != 0; i++)
	//							{
	//							//	const char * pszText = nullptr;

	//								wstrText.clear();

	//								const GUI_EDITBOX_INIT * pInfo = &s_STATIC_TEXT_INIT[i];

	//								if( pInfo->m_idxStrInit != -1)
	//								{
	//									wstrText = GAME_RCSTRING_BY_HANDLE(pInfo->m_idxStrInit);
	//									if( wstrText.empty() )
	//									{
	//										I3PRINTLOG(I3LOG_FATAL, "탈출 미션에 들어가는 스트링[ %s ] 이 번역이 안 되어 있다.", s_STATIC_TEXT_INIT[i].m_szName );
	//									}
	//								}
	//								else
	//								{
	//									I3PRINTLOG(I3LOG_FATAL, "탈출 미션에 들어가는 스트링[ %s ] 이 번역이 안 되어 있다.", s_STATIC_TEXT_INIT[i].m_szName );
	//								}

	//								_InitEditBox( (char *) pInfo->m_szName, pInfo->m_MaxLength, pInfo->m_FontPt, 
	//									s_STATIC_TEXT_INIT[i].m_AlignHorz, s_STATIC_TEXT_INIT[i].m_AlignVert, wstrText);
	//							}

	//							m_pST_BossTeam->SetEnable( true);
	//							m_pST_UnionTeam->SetEnable( false);


	//						}
	//						else
	//						{
	//							m_pST_BossTeam->SetEnable( false);
	//							m_pST_UnionTeam->SetEnable( true);
	//						}
	//					}
	//				}
	//			}
	//			break;

	//		default :
	//			bEnable = false;
	//			break;
	//	}
	//}

	//if( m_bVisible != bEnable)
	//{
	//	m_bVisible = bEnable;

	//	GameGUI::FLUSH_CONTROL(m_pPanel);
	//	m_pPanel->SetEnable( m_bVisible == true);

	//	SetEnable( m_pPanel->isEnable());
	//}
}
