#include "pch.h"
#include "UIPopupClanWarning.h"
#include "UIMainFrame.h"
#include "GlobalVariables.h"

#include "UIUtil.h"
#include "i3Base/string/ext/format_string.h"
#include "UserInfoContext.h"

I3_CLASS_INSTANCE( UIPopupClanWarning);


LuaFuncReg UIPopupClanWarning_Glue[] =
{
	{nullptr, nullptr}
};

UIPopupClanWarning::UIPopupClanWarning(void)
{
	m_pBtnOK = nullptr;
	m_bPopupOpenYN = false;
}

void	UIPopupClanWarning::_InitializeAtLoad( i3UIScene * pScene)
{
	UIPopupClanBase::_InitializeAtLoad( pScene);

	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;

	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;

	m_pBtnOK = (i3UIButton*) pScene->FindChildByName( "i3UIButtonOK");
	I3ASSERT( m_pBtnOK);

	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		// 에러 발생한것입니다.
		m_pBtnOK->EnableCtrl(false);
	}

	i3::wstring wstr = i3::format_string(GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_FOR_CREATING"), GetCharaRank(RequirementRank), RequirementPoint);

	LuaState * L = _CallLuaFunction("SetGuide");
	i3Lua::PushStringUTF16To8(L, wstr);
	_EndLuaFunction(L, 1);
}


UIPopupClanWarning::~UIPopupClanWarning(void)
{
}

/*virtual*/ void UIPopupClanWarning::OnCreate( i3GameNode * pParent)
{
	UIPopupBase::OnCreate( pParent);

	// Load Scene
	AddScene( "Scene/Popup/PBRe_PopUp_ClanWarning.i3UIs", UIPopupClanWarning_Glue);
}


// 클랜 생성버튼
void UIPopupClanWarning::_CreateConfirm( void)
{
	UINT32 MyPoint = UserInfoContext::i()->GetMyPoint();
	UINT32 wRank = UserInfoContext::i()->GetRealRank();

	UINT8	RequirementRank = 0;
	UINT32	RequirementPoint = 0;
	if ( ! g_pFramework->GetClanContext()->GetClanCreationRequirementInfo(&RequirementRank, &RequirementPoint) )
	{
		return;
	}

	if( MyPoint > RequirementPoint && wRank >= RequirementRank)
	{
		// 클랜 생성화면 호출
//		if( OnOKButton())
			GetMainFrame()->TogglePopup(UPW_CLAN_CREATE);
		return;
	}

	if( MyPoint < RequirementPoint)
	{ 
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_NO_POINT"));		//	"포인트가 부족합니다."
	}
	else if( wRank < RequirementRank)
	{
		i3::rc_wstring str = i3::format_string(L"{col:253,167,0,255}%s{/col} %s", GetCharaRank( RequirementRank), GAME_RCSTRING("STBL_IDX_CLAN_NEED_HIGH_RANK"));
		GameUI::MsgBox( MSG_TYPE_GAME_OK, str);
	}
	else
	{
		//	"죄송합니다. 클랜 생성에 필요한 계급+에\n도달하지 못하였거나 포인트가 부족합니다.\n다음에 다시 시도해 주세요."
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_CLAN_MESSAGE_CANNOT_CREATING_CLAN"));
	}
}

bool	UIPopupClanWarning::OnOKButton( void)
{
	_CreateConfirm();
	g_pFramework->GetClanContext()->ResetClanCreationInfo();
	return UIPopupClanBase::OnOKButton();
}
bool	UIPopupClanWarning::OnCloseButton( void)
{
	g_pFramework->GetClanContext()->ResetClanCreationInfo();
	return UIPopupClanBase::OnCloseButton();
}

void	UIPopupClanWarning::OnExitEnd()
{
	m_bPopupOpenYN = false;
	g_pFramework->GetClanContext()->ResetClanCreationInfo();
	UIPopupClanBase::OnExitEnd();
}