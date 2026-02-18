#include "pch.h"
#include "UITabBase.h"

//I3_ABSTRACT_CLASS_INSTANCE( UITabBase, UIBase);

I3_CLASS_INSTANCE( UITabBase);

UITabBase::UITabBase()
{
}

UITabBase::~UITabBase()
{
}


UIMainFrame * UITabBase::GetMainFrame( void)
{
	return g_pFramework->GetUIMainframe();
		//(UIMainFrame*) ((UIPhaseBase*)m_pParent)->GetMainFrame();
}
