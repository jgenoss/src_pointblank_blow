#include "pch.h"
#include "UISubBase.h"

//I3_ABSTRACT_CLASS_INSTANCE( UISubBase, UIBase);
I3_CLASS_INSTANCE( UISubBase);


UISubBase::UISubBase()
{
}

UISubBase::~UISubBase()
{
}

LuaState * UISubBase::_CallLuaFunction( const char * pszFunction)
{
	return ((UIBase*)m_pParent)->_CallLuaFunction( pszFunction);
}

UIMainFrame * UISubBase::GetMainFrame( void)
{
	return g_pFramework->GetUIMainframe();
}

/*virtual*/ void UISubBase::OnCreate( i3GameNode * pParent)
{
	UIBase::OnCreate( pParent);


}

