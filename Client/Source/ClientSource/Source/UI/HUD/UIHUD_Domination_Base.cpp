#include "pch.h"

#include "UIHUD_Domination_Base.h"

I3_CLASS_INSTANCE(UIHUDDominationBase);//, UIHUDBase);


UIHUDDominationBase::UIHUDDominationBase(void)
{
}
UIHUDDominationBase::~UIHUDDominationBase(void)
{
}


void UIHUDDominationBase::OnCreate( i3GameNode* pParent)
{
	UIHUDBase::OnCreate( pParent);
}

void UIHUDDominationBase::_InitializeAtLoad(i3UIScene* pScene)
{
	UIHUDBase::_InitializeAtLoad(pScene);
	m_pUIScene = pScene;
}

namespace
{
	void DisableCtrl(i3UIControl* pCtrl)
	{
		pCtrl->EnableCtrl( false, true);

		i3UIControl * pChild = static_cast<i3UIControl*>(pCtrl->getFirstChild());
		while( pChild != nullptr)
		{
			pChild->EnableCtrl( false, true);

			pChild = static_cast<i3UIControl*>(pChild->getNext());
		}
	}
}

bool UIHUDDominationBase::OnExitStart()
{
	if (UIHUDBase::OnExitStart() == false) return false;

//	I3ASSERT(m_pUIScene != nullptr);
//	i3UIControl* pCtrl = static_cast<i3UIControl*>(m_pUIScene->getFirstChild());
//	DisableCtrl(pCtrl);
	/*
	INT32 numScn = UIBase::GetSceneCount();
	
	for (INT32 i = 0 ; i < numScn ; ++i)
	{
		i3UIScene* scn = UIBase::GetScene(i);
		
		if (scn)
		{
			scn->EnableScene(false);
		}
	}
*/
	return true;
}

void UIHUDDominationBase::OnUpdate( REAL32 rDeltaSec)
{
	UIHUDBase::OnUpdate( rDeltaSec);
}
