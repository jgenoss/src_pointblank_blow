#include "pch.h"
#include "UIFloatingBase.h"
#include "UIFramework.h"
//I3_ABSTRACT_CLASS_INSTANCE( UIFloatingBase, UIBase);
I3_CLASS_INSTANCE( UIFloatingBase);

UIFloatingBase::UIFloatingBase()
{
	m_AlignedPos = UIFLOATING_POS_NONE;
	m_AlignedPos2 = UIFLOATING_POS_NONE;

	m_bFixedWindow = false;
}

UIFloatingBase::~UIFloatingBase()
{
}

UIMainFrame * UIFloatingBase::GetMainFrame( void)
{
	if( i3::same_of<UIFramework*>(m_pParent))
		return ((UIFramework*)m_pParent)->GetUIMainframe();

	return nullptr;
}


