#include "pch.h"
#include "UIPopupGachaHelpGuide.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE( UIPopupGachaHelpGuide);

namespace
{
	UIPopupGachaHelpGuide* g_this = 0;
}

extern "C" 
{
	int click_exit( LuaState * L)
	{
		g_this->clicked();
		return 0;
	}
}

LuaFuncReg click_exit_glue[] =
{
	{ "Click",				click_exit	},
	{ nullptr,							nullptr}
};

void UIPopupGachaHelpGuide::clicked()
{
	GetMainFrame()->ClosePopup(UPW_GACHA_HELP_GUIDE);
}

UIPopupGachaHelpGuide::UIPopupGachaHelpGuide()
{
	g_this = this;
}

UIPopupGachaHelpGuide::~UIPopupGachaHelpGuide()
{

}

void UIPopupGachaHelpGuide::OnCreate( i3GameNode * pParent )
{
	UIPopupBase::OnCreate(pParent);

	AddScene("Scene/Main/PBRe_Help_Guide.i3UIs", click_exit_glue);
}

bool UIPopupGachaHelpGuide::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/ )
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
	{
		return false;
	}
	
	GetSlide()->ForceEnd();

	return true;
}

void UIPopupGachaHelpGuide::OnEntranceEnd( void )
{
	UIPopupBase::OnEntranceEnd();

	i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	if( pSndCtx != nullptr)
		pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)0);
}

bool UIPopupGachaHelpGuide::OnExitStart( void )
{
	if( UIPopupBase::OnExitStart() == false)
		return false;

	//i3SoundContext * pSndCtx = g_pViewer->GetSoundContext();
	//if( pSndCtx != nullptr)
	//	pSndCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)100);

	return true;
}

bool UIPopupGachaHelpGuide::OnKey_Escape( void )
{
	GetMainFrame()->ClosePopup(UPW_GACHA_HELP_GUIDE);

	return UIPopupBase::OnKey_Escape();
}