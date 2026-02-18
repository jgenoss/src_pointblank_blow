#include "pch.h"

#if defined(SKILLSYSTEM)
#include "UIPhaseProfile.h"
#include "UIMainFrame.h"
#include "UITabProfileSkill.h"
#include "UITabProfileCharacter.h"
#include "UISkillCharacterSlot.h"
#include "UIUtil.h"


static UITabProfileCharacter * GetSelectCharaScene(void)
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	UIPhaseProfile* pProfile = (UIPhaseProfile*)pFrame->GetPopup(UPW_PROFILE);
	UITabProfileBase* pSkill =  (UITabProfileBase*)pProfile->GetTab(UIPhaseProfile::TAB_SKILL);
	UISkillMenu * pScene =  static_cast<UITabProfileSkill*>(pSkill)->GetCurrentScene();
	return static_cast<UITabProfileCharacter*>(pScene);
}

//--------------------------------------------------------------------------//
//								Lua Glue									//
extern "C" {
	int UISlotChara_SelectCharacter( LuaState * L)
	{
		UITabProfileCharacter * pSelector = GetSelectCharaScene();
		BOOL act = i3Lua::GetBooleanArg( L, 1);
		INT32 slot = pSelector->FindSlot( L);

		pSelector->SelectSlot( slot, act );
		return 0;
	}

	int UITabProfileCharaSlot_Scroll( LuaState * L)
	{
		INT32 nDelta = i3Lua::GetIntegerArg( L, 1 );
		GetSelectCharaScene()->UpdateWheel( nDelta );
		return 0;
	}
}

LuaFuncReg UISkill_Character_Slot_Glue[] = {
	{ "SelectCharacter",	UISlotChara_SelectCharacter	},
	{ "UpdateWheel",		UITabProfileCharaSlot_Scroll},
	{NULL,					NULL}
};

I3_CLASS_INSTANCE( UISkillCharacterSlot);//, UISlotBase);

UISkillCharacterSlot::UISkillCharacterSlot()
{
}

UISkillCharacterSlot::~UISkillCharacterSlot()
{
}

void UISkillCharacterSlot::UpdateWheel(INT32 nDelta)
{

}

/*virtual*/ bool UISkillCharacterSlot::OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */)
{
	if( UISlotBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	return true;
}

/*virtual*/ void UISkillCharacterSlot::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "UIRe/Scene/Main/PointBlankRe_Profile_Skill_Chara_Slot.i3UIe", UISkill_Character_Slot_Glue);
}

/*virtual*/ void UISkillCharacterSlot::OnSelect()
{
	m_bSelected = true;
	LuaState * L = _CallLuaFunction("Select");
	_EndLuaFunction(L, 0);
}

/*virtual */void UISkillCharacterSlot::OnUnselect()
{
	m_bSelected = false;
	LuaState * L = _CallLuaFunction("UnSelect");
	_EndLuaFunction(L, 0);
}

void UISkillCharacterSlot::SetSlotState(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetSlotEnable");
	if(idx == 2)
		i3Lua::PushBoolean(L, FALSE);
	else
		i3Lua::PushBoolean(L, TRUE);
	_EndLuaFunction(L, 1);

	L = _CallLuaFunction("SetSlotState");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L,1);
}

void UISkillCharacterSlot::SetSlotInfo(INT32 idx)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	CCharaInfoContext * CChara = g_pFramework->GetCharaInfoContext();
	const CHARA_PARTS_INFO* parts = CChara->GetCharaPartsInfo(idx);
	INT32 charaIdx = CChara->GetCharaInfo(idx)->getBasicInfo()->m_ui8CharaSlotIdx;

	char _playPoint[MAX_STRING_COUNT] = {0,};
	INT32 nCharaImageID = 0;
	if( parts )
		nCharaImageID = GameUI::GetCharaImageIndex( parts->GetCharaItemID() );

	LuaState * L = _CallLuaFunction("SetInfo");
	
	i3Lua::PushInteger(L, nCharaImageID);
	i3Lua::PushA2UTF8String(L, CChara->GetNickName(idx));
	for(INT32 i=0; i < MAX_SKILL_TYPE; i++)
	{
		INT32 _Idx = skillMng->getSkillLastImageIndex(charaIdx,i);
		i3Lua::PushInteger(L, _Idx);
	}
	for(INT32 i=0; i < MAX_SKILL_TYPE; i++)
	{
		UINT16 _Point = skillMng->getSkillPlayPoint(charaIdx,i);
		sprintf_s(_playPoint, "%d", _Point);
		i3Lua::PushA2UTF8String(L, _playPoint);
	}

	INT32 MAX_LUA_VALUE = MAX_SKILL_TYPE * 2 + 2;
	_EndLuaFunction(L, MAX_LUA_VALUE);

}

#endif