#include "pch.h"

#if legacy_gui_scene
#include "UIPopupError.h"

I3_CLASS_INSTANCE( UIPopupError);//, UIPopupBase);

UIPopupError::UIPopupError()
{
}

UIPopupError::~UIPopupError()
{
}

/*virtual*/ void UIPopupError::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	//AddScene( "UI_V15/PopupError.i3UIs");
}

void UIPopupError::OnSetMessage( const i3::wliteral_range& wMessageRng)
{
	LuaState * L = _CallLuaFunction( m_pLuaContextName, "SetMessage");
	if( L != nullptr)
	{
		i3Lua::PushStringUTF16To8( L, wMessageRng);
		_EndLuaFunction( L, 1);
	}
}
#endif