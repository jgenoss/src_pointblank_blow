#include "pch.h"
#include "UIPopupEvent.h"

#include "UIMainFrame.h"
#include "UIPhaseLobby.h"
#include "../TextSet.h"

#if legacy_gui_function
	#include "../GuiNotifyReceiver.h"
#endif

I3_CLASS_INSTANCE( UIPopupEvent);//, UIPopupBase);

UIPopupEvent::UIPopupEvent()
{
}

UIPopupEvent::~UIPopupEvent()
{
}

void UIPopupEvent::_ModifyControl( void)
{

}

/*virtual*/ void UIPopupEvent::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	AddScene( "Scene/Popup/PBRe_PopUp_Christmas.i3UIs");


	
}

/*virtual*/ bool UIPopupEvent::OnKey_Enter( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupEvent::OnKey_Escape( void)
{
	ConfirmOK();
	return true;
}

/*virtual*/ bool UIPopupEvent::OnEntranceStart( void * pArg1 /*= nullptr*/, void * pArg2 /*= nullptr*/)
{
	if( UIPopupBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;

	_ModifyControl();

 	
	OpenEventWin(702001024);


	return true;
}

/*virtual*/ bool UIPopupEvent::OnOKButton( void)
{
	ConfirmOK();

	return true;
}

void UIPopupEvent::ConfirmOK(void)
{
	g_pFramework->GetUIMainframe()->TogglePopup( UPW_EVENT);
}

/*virtual*/ void UIPopupEvent::OnLoadAllScenes()
{
	m_ImgBox = GetItemImageMgr()->CreateImgSetCtrl(GetScene(0), "i3UIButtonImageSet3");
	
	
}

/*virtual*/ void UIPopupEvent::OnUnloadAllScenes()
{
	I3_SAFE_RELEASE(m_ImgBox);

}
void UIPopupEvent::OpenEventWin( T_ItemID ItemId)
{

//	m_ImgBox->SetEnable(true);
	GetItemImageMgr()->SelectImage(m_ImgBox,ItemId);

	

}
