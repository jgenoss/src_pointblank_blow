#include "pch.h"
#include "UISubTabClanMgt_Etc.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE( UISubTabClanMgt_Etc);//, UISubTabClan_Mgt);

//------------------------------------------------------------------------------//
UISubTabClanMgt_Etc::UISubTabClanMgt_Etc()
{

}

UISubTabClanMgt_Etc::~UISubTabClanMgt_Etc()
{

}

/*virtual*/ void UISubTabClanMgt_Etc::OnCreate(i3GameNode * pParent)
{
	UISubTabClan_Mgt::OnCreate(pParent);
	//LoadScene
	AddScene( "PBRe_ClanMgt_Etc.i3UIs" );
}
