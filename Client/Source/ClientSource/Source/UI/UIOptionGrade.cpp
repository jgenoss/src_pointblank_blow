#include "pch.h"

#include "UIOptionGrade.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( UIOptionGrade);

/************************//************************/
/*virtual*/ void UIOptionGrade::OnCreate( i3GameNode * pParent)
{
	UIOptionBase::OnCreate( pParent);

	AddScene("Scene/Option/PBRe_Pref_Grade.i3UIs");
}

/*virtual*/ void UIOptionGrade::_InitializeAtLoad(i3UIScene * pScene)
{
	UIOptionBase::_InitializeAtLoad( pScene);

	INT32 age = -1;		// -1은 평가용

	if (LocaleValue::Enable("EnableShutDown"))
		age = UserInfoContext::i()->getAge();

	LuaState * L = UILua::CallLuaFunction(pScene, "SetGradeImage");
	
	if (L != nullptr)
	{
		i3Lua::PushInteger(L, age);
		UILua::EndLuaFunction(L, 1);
	}
}

void UIOptionGrade::apply_lua()
{
	INT32 age = -1;		// -1은 평가용

	if (LocaleValue::Enable("EnableShutDown"))
		age = UserInfoContext::i()->getAge();

	LuaState * L = _CallLuaFunction(0, "SetGradeImage");

	if (L != nullptr)
	{
		i3Lua::PushInteger(L, age);
		UILua::EndLuaFunction(L, 1);
	}
}