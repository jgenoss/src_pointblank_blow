#include "pch.h"
#include "UITabClanMgt_Etc.h"
#include "UIMainFrame.h"

I3_CLASS_INSTANCE( UITabClanMgt_Etc, UITabClan_Mgt);

//------------------------------------------------------------------------------//
UITabClanMgt_Etc::UITabClanMgt_Etc()
{

}

UITabClanMgt_Etc::~UITabClanMgt_Etc()
{

}

/*virtual*/ void UITabClanMgt_Etc::OnCreate(i3GameNode * pParent)
{
	base::OnCreate(pParent);
	//LoadScene
	AddScene( "UIRe/PBRe_ClanMgt_Etc.i3UIe" );
}
