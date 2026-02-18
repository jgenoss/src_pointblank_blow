#include "pch.h"
#include "UIPopupReport.h"

#include "UIMainFrame.h"

//----------------------------------------------------------------------------------//
//									Lua Glue Functions								//
//----------------------------------------------------------------------------------//
extern "C" {
	int UIPopupReport_ClickOK( LuaState * L)
	{
		char szUserNick[ NET_NICK_NAME_SIZE];
		char szCause[ MAX_REPORTCAUSE_SIZE];

#ifdef I3_DEBUG
		INT32 cause = i3Lua::GetIntegerArg( L, 1);
#endif
		if( i3Lua::GetStringArg( L, 2, szUserNick, sizeof( szUserNick), "") != nullptr)
		{
			// 내용은 있을 수도 없을 수도 있다.
			i3Lua::GetStringArg( L, 3, szCause, sizeof( szCause), "");

			// Game Context에 이벤트를 준다.
#ifdef I3_DEBUG
			I3TRACE( "%d", cause);	// 워닝 방지용 cause값 처리 필요
#endif
		}

		return 0;
	}

	int UIPopupReport_ClickCancel( LuaState * L)
	{
		UIMainFrame * pFrame = g_pFramework->GetUIMainframe();

		I3ASSERT( pFrame != nullptr);

		pFrame->TogglePopup( UPW_NULL, false);
		
		return 0;
	}
}


LuaFuncReg UIPopupReport_Glue[] = {
	{"uiClickOK",			UIPopupReport_ClickOK},
	{"uiClickCancel",		UIPopupReport_ClickCancel},
	{nullptr,					nullptr}
};

//----------------------------------------------------------------------------------//

I3_CLASS_INSTANCE( UIPopupReport);//, UIPopupBase);

UIPopupReport::UIPopupReport()
{
}

UIPopupReport::~UIPopupReport()
{
}

/*virtual*/ void UIPopupReport::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	//AddScene( "UI_V15/Report.i3UIs", UIPopupReport_Glue);
}

bool	UIPopupReport::OnKey_Enter( void)
{
	return UIPopupBase::OnOKButton();			// 뭔지 모르지만, 구현이 마땅치 않아 이렇게..
}
