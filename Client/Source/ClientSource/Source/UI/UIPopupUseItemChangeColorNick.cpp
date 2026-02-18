
#include "pch.h"


//////////////////////////////////////////////////
// chunjong.song 2014-12-01 
// 이파일의 클래스는 더이상 사용되지않습니다.
//////////////////////////////////////////////////

//#include "UIPopupUseItemChangeColorNick.h"
//
//#include "UIMainFrame.h"
//#include "ItemConsumer.h"
//#include "UserInfoContext.h"
//
//#include "i3Base/string/ext/mb_to_utf16.h"
//
//
//#define SELECT_COLOR_BOX_COUNT					10	//	변경 색상 샘플 갯수
//
//extern "C" 
//{
//	int UIPopupUseItemChangeColorNick_SelectColor( LuaState * L)
//	{
//		INT32 idx = i3Lua::GetIntegerArg( L, 1);
//		ItemConsumer::instance()->m_nSelected = idx;
//
//		return 0;
//	}
//}
//
//
//I3_CLASS_INSTANCE( UIPopupUseItemChangeColorNick);//, UIPopupColorItemBase);
//
///*virtual*/ LuaState * UIPopupUseItemChangeColorNick::_SetItemInfo( void)
//{
//	i3::wstring wstrNickName;
//
//	LuaState * L = UIPopupColorItemBase::_SetItemInfo();
//
//	if( L != nullptr)
//	{
//		USER_INFO_BASIC		MyUserInfoBasic;
//		UserInfoContext::i()->GetMyUserInfoBasic(&MyUserInfoBasic);
//		
//		
//
//		i3::wliteral_range wstrNick = MyUserInfoBasic.m_strNick;
//
//		SetNameUsingColorTable( wstrNickName, wstrNick, MyUserInfoBasic.m_ui8NickColor);		
//
//		INT8 idxColor = MyUserInfoBasic.m_ui8NickColor;
//
//		I3COLOR * pColor = nullptr;
//
//		if( idxColor == 0)
//			pColor = GetColor( -1);	//	기본 흰색
//		else
//			pColor = GetColor( idxColor-1);	//	서버는 1부터 색상, GUI는 0부터 색상
//
//		i3Lua::PushColor( L, pColor);
//		i3Lua::PushStringUTF16To8( L, wstrNickName);			
//		_EndLuaFunction( L, 6);
//	}
//
//	return L;
//}
//
//
///*virtual*/ void UIPopupUseItemChangeColorNick::OnCreate( i3GameNode * pParent)
//{
//	UIPopupColorItemBase::OnCreate( pParent);
//
//	AddScene( "Scene/Popup/PBRe_PopUp_useItemChangeColor.i3UIs");
//}
//
//void UIPopupUseItemChangeColorNick::SetItemType( UI_CHANGECOLORNICK_TYPE type)
//{
//	LuaState * L = _CallLuaFunction( "SetItemType");
//	if( L != nullptr)
//	{
//		i3Lua::PushInteger( L, (INT32) type);
//		_EndLuaFunction( L, 1);
//	}
//}
