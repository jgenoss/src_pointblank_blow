#include "pch.h"
#include "UIPopupReceiveMail_V10.h"
#include "UI/UIUtil.h"

 extern "C" 
 {
 	int UIPopupReceiveMail_V10_CheckID( LuaState * L)
 	{
 		UIPopupReceiveMail * pThis = static_cast<UIPopupReceiveMail*>(GameUI::GetPopup(UPW_MAILREV));
 		if( pThis != nullptr)
 			pThis->CheckID();
 		return 0;
 	}
 
 	int UIPopupReceiveMail_V10_ClanAskSend( LuaState * L)
 	{
 		bool bAccept = i3Lua::GetBooleanArg(L , 1);
 		UIPopupReceiveMail * pThis = static_cast<UIPopupReceiveMail*>(GameUI::GetPopup(UPW_MAILREV));
 		if( pThis != nullptr)
 			pThis->SendClanJoinAsk(bAccept);
 		return 0;
 	}
 }
 
 LuaFuncReg UIPopupReceiveMail_V10_Glue[] =
 {
 	{"CheckID",		UIPopupReceiveMail_V10_CheckID},
 	{"ClanAskSend", UIPopupReceiveMail_V10_ClanAskSend},
 	{nullptr,			nullptr}
 };


I3_CLASS_INSTANCE( UIPopupReceiveMail_V10);//, UIPopupBase );

UIPopupReceiveMail_V10::UIPopupReceiveMail_V10() 
{
}

UIPopupReceiveMail_V10::~UIPopupReceiveMail_V10()
{
}

void UIPopupReceiveMail_V10::OnCreate( i3GameNode* pParent)
{
	UIPopupBase::OnCreate( pParent);
	AddScene( "Scene/Popup/PBRe_PopUp_Message.i3UIs", UIPopupReceiveMail_V10_Glue );
}