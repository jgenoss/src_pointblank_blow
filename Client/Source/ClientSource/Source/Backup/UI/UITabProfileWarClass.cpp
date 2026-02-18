#include "pch.h"

#if defined(SKILLSYSTEM)
#include "UITabProfileWarClass.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UITabProfileSkill.h"
#include "UIMovieSetCtrl.h"
#include "UIUtil.h"


static UITabProfileWarClass * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPopupBase* pSubPhase = pFrame->GetPopup(UPW_PROFILE);
	UITabProfileBase* pTab = static_cast<UIPhaseProfile*>(pSubPhase)->GetCurrentTab();
	I3ASSERT( i3::same_of<UITabProfileSkill*>(pTab)); //->IsExactTypeOf( UITabProfileSkill::static_meta()));
	UISkillMenu * pScene = static_cast<UITabProfileSkill*>(pTab)->GetCurrentScene();
	return static_cast<UITabProfileWarClass*>(pScene);
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UITabProfileWarClass_ChangeClass( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg(L, 1);
		GetThis()->ChangeClass(idx);
		return 0;
	}
	int UITabProfileWarClass_Confirm( LuaState * L)
	{
		GetThis()->Confirm();
		return 0;
	}
}

LuaFuncReg UITabProfileWarClass_Glue[] =
{
	{"ChangeClass",			UITabProfileWarClass_ChangeClass	},
	{"Confirm",				UITabProfileWarClass_Confirm		},
	{NULL,					NULL},
};

I3_CLASS_INSTANCE( UITabProfileWarClass);//, UISkillMenu);

UITabProfileWarClass::UITabProfileWarClass()
{
	m_pMovieCtrl = NULL;
	m_Selected = -1;
}

UITabProfileWarClass::~UITabProfileWarClass()
{

}

void UITabProfileWarClass::SetMoviePlay(INT32 idx)
{
	if( m_pMovieCtrl != NULL)
	{
		if( m_pMovieCtrl->PlayVideo(getSelectedClass(idx)) == FALSE)
		{
			SetMovieBG(true);
			m_pMovieCtrl->StopVideo();
			return;
		}
	}
	SetMovieBG(false);
}

void UITabProfileWarClass::SetTypeText()
{
	char _skillExp[MAX_STRING_COUNT] = {0,};
	sprintf_s(_skillExp, GAME_STRING("STR_TBL_CLASS_MAINTEXT1"), g_pGameContext->getSkillManager()->getClassTypeName(GetCurrentTab()));

	LuaState * L = _CallLuaFunction("SetMainText1");
	i3Lua::PushA2UTF8String(L, _skillExp);
	_EndLuaFunction(L,1);
}

void UITabProfileWarClass::SetExplainText(INT32 idx)
{
	char _skillExp[MAX_STRING_COUNT] = {0,};
	if(idx != -1)
	{
		sprintf_s(_skillExp, GAME_STRING("STR_TBL_CLASS_MAINTEXT2"),
			g_pGameContext->getSkillManager()->getClassName(getSelectedClass(idx)),
			g_pGameContext->getSkillManager()->getClassTypeName(GetCurrentTab())
			);
	}

	LuaState * L = _CallLuaFunction("SetMainText2");
	i3Lua::PushA2UTF8String(L, _skillExp);
	_EndLuaFunction(L,1);
}

void UITabProfileWarClass::SetMovieText(INT32 idx)
{
	if(idx != -1)
	{
		const char * _skillExp1 = g_pGameContext->getSkillManager()->getClassName(getSelectedClass(idx));
		const char * _skillExp2 = g_pGameContext->getSkillManager()->getSkillExplain(getSelectedClass(idx));
		LuaState * L = _CallLuaFunction("SetSkillText");
		i3Lua::PushA2UTF8String(L, _skillExp1);
		i3Lua::PushA2UTF8String(L, _skillExp2);
		_EndLuaFunction(L,2);
	}
	else
	{
		LuaState * L = _CallLuaFunction("SetSkillText");
		i3Lua::PushA2UTF8String(L, "");
		i3Lua::PushA2UTF8String(L, "");
		_EndLuaFunction(L,2);
	}
}

void UITabProfileWarClass::SetButtonFrame()
{
	LuaState * L = _CallLuaFunction("SetFrame");
	i3Lua::PushInteger(L, GetCurrentTab());
	_EndLuaFunction(L,1);
}

INT32 UITabProfileWarClass::getSelectedClass(INT32 idx)
{
	UITabProfileSkill::SKILL_TAB _tab = (UITabProfileSkill::SKILL_TAB)GetCurrentTab();
	switch(_tab)
	{
	case UITabProfileSkill::TAB_PRIMARY :
		break;
	case UITabProfileSkill::TAB_SECONDARY :
		idx += MAX_SKILL_CLASS_PRIMARY;
		break;
	case UITabProfileSkill::TAB_MODE :
		idx += MAX_SKILL_CLASS_PRIMARY + MAX_SKILL_CLASS_SECONDARY;
		break;
	}
	return idx;
}

void UITabProfileWarClass::SetMovieBG(bool Sig)
{
	LuaState * L = _CallLuaFunction("SetMovieBG");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L, 1);
}

void UITabProfileWarClass::ChangeClass(INT32 idx)
{
	m_Selected = idx;
	SetMoviePlay(idx);
	SetExplainText(idx);
	SetMovieText(idx);
}

void UITabProfileWarClass::Confirm()
{
	if(m_Selected == -1)
		return;
	PACKET_SET_SPECIALITY_CLASS_REQ m_ClassData;
	m_ClassData.m_ui8Slot	=	(UINT8)GetCurrentAvatar();
	m_ClassData.m_ui8Type	=	(UINT8)GetCurrentTab();
	m_ClassData.m_ui8Class	=	(UINT8)m_Selected;
	g_pGameContext->getSkillManager()->setPacketClass(&m_ClassData);

	g_pGameContext->SetEvent(EVENT_SKILL_CLASS_SET);
}

/*virtual*/ void UITabProfileWarClass::OnCreate( i3GameNode * pParent)
{
	UISkillMenu::OnCreate( pParent);
	// Load Scene
	AddScene( "UIRe/Scene/Main/PointBlankRe_Profile_Skill_Class.i3UIe", UITabProfileWarClass_Glue);
}

/*virtual*/ void UITabProfileWarClass::OnMenuStart()
{
	SetButtonFrame();
	SetTypeText();
	SetExplainText();
	SetMovieText();
	SetMovieBG(true);
}

/*virtual*/ void UITabProfileWarClass::OnMenuExit()
{
}

/*virtual*/ void UITabProfileWarClass::OnLoadAllScenes()
{
	UISkillMenu::OnLoadAllScenes();

	std::vector<std::string> fileList;
	fileList.push_back("Gui\\Loading\\S.Skill_AssualtRifle.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_SMG.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Sniper.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Shotgun.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_machine.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Handgun.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Knife.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Grenade.avi");
	fileList.push_back("Gui\\Loading\\S.Skill_Dual.avi");

	m_pMovieCtrl = GetMovieMgr()->CreateMovieSetCtrl(GetScene(0), "i3UIMovieBoxEx");
	if( m_pMovieCtrl != NULL)
	{
		for(size_t i=0; i<fileList.size(); i++)
			m_pMovieCtrl->registFile(fileList[i].c_str());
		m_pMovieCtrl->InitMovie();
	}
}

/*virtual*/ void UITabProfileWarClass::OnUnloadAllScenes()
{
	if(m_pMovieCtrl)
		m_pMovieCtrl->CloseVideo();
	I3_SAFE_RELEASE(m_pMovieCtrl);
	UISkillMenu::OnUnloadAllScenes();
}

void UITabProfileWarClass::ProcessGameEvent( GAME_EVENT event, INT32 arg, const i3::user_data& UserData)
{
	switch(event)
	{
	case EVENT_SKILL_CLASS_SET:
		switch(arg)
		{
		case EVENT_ERROR_SUCCESS:									// 세팅 성공
			static_cast<UITabProfileSkill*>(m_pParent)->ChangeSkillScene(UITabProfileSkill::SCENE_SELECT, (UITabProfileSkill::SKILL_TAB)GetCurrentTab());
			break;
		case EVENT_ERROR_PBV15_SPECIALITY_ALEADY_SET_CLASS :		// 주특기 클래스 셋팅 실패 - 이미 클래스가 셋팅되었다.
			break;
		default:													// 알수없다.
			break;
		}
		break;
	}
}
#endif