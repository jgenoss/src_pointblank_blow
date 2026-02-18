#include "pch.h"
#include "UITabClan.h"

#include "UITabClanManagement.h"

I3_CLASS_INSTANCE( UITabClan);
I3_CLASS_INSTANCE( UITabClan_Normal);
I3_CLASS_INSTANCE( UISubTabClan_Mgt);

UITabClan::UITabClan() 
{
	
}

UITabClan::~UITabClan()
{
	
}

void UITabClan::OnCreate( i3GameNode * pParent)
{
	UITabBase::OnCreate(pParent);
	
	GetPhaseClan()->GetSlide()->AddSyncSlide(GetSlide());
}

void UITabClan::OnLoadAllScenes()
{
	UITabBase::OnLoadAllScenes();
}

void UITabClan::OnExitEnd()
{
	UITabBase::OnExitEnd();	
	GetPhaseClan()->ProcessNextTab();
}

UITabClan_Normal::UITabClan_Normal()
{

}

UITabClan_Normal::~UITabClan_Normal()
{

}

UISubTabClan_Mgt::UISubTabClan_Mgt()
{

}

UISubTabClan_Mgt::~UISubTabClan_Mgt()
{

}

void UISubTabClan_Mgt::OnExitEnd()
{
	UITabBase::OnExitEnd();	

	UITabClanManagement * pMgtTab = static_cast<UITabClanManagement*>(getParent());
	if(pMgtTab != nullptr)
	{
		pMgtTab->ProcessNextTab();
	}	
}