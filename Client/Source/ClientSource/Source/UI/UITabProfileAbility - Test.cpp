#include "pch.h"
#include "UITabProfileAbility.h"
#include "UIMainFrame.h"
#include "UIPhaseProfile.h"
#include "UIUtil.h"

static UITabProfileAbility * GetThis()
{
	UIMainFrame * pFrame = g_pFramework->GetUIMainframe();
	I3ASSERT( pFrame);
	UIPhaseSubBase* pSubPhase = pFrame->GetCurrentSubPhase();
	UITabProfileBase* pTab = static_cast<UIPhaseProfile*>(pSubPhase)->GetAbility();
	I3ASSERT( pTab->IsExactTypeOf( UITabProfileAbility::GetClassMeta()));
	return static_cast<UITabProfileAbility*>(pTab);
}


//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {
	int UISelectTab( LuaState * L)
	{
		INT32 idx = i3Lua::GetIntegerArg( L, 1);
		if(GetThis()->GetCurrentSelectedTab() == idx)
			return 0;
		else
			GetThis()->SelectTab(idx);
	}
}

LuaFuncReg UITabProfileAbility_Glue[] =
{
	{"SelectTab",			UISelectTab	},
	{NULL,					NULL		},
};

I3_CLASS_INSTANCE( UITabProfileAbility, UITabProfileBase);

UITabProfileAbility::UITabProfileAbility()
{
	//m_SlotCallback.SetObj(this);
	//m_SlotSet.SetCallback(&m_SlotCallback);
}

UITabProfileAbility::~UITabProfileAbility()
{
}

void UITabProfileAbility::VerifyMoveScene(INT32 idx)
{	
	MoveScene(idx);
}

//void UITabProfileAbility::OnLoadSlot(INT32 idx, i3UIScene* scn)
//{
//
//}
//
//void UITabProfileAbility::OnEnableSlot(INT32 idx, i3UIScene* scn)
//{
//
//}

void UITabProfileAbility::MoveScene(INT32 idx)
{
}

void UITabProfileAbility::SelectTab(INT32 TAB)
{
	if(/*g_pGameContext->GetSkillManager()->HasSkill(TAB) == */TRUE)
	{
		m_CurrentTab = TAB;
		EventSkillChange();
		LoadText();
	}
	else
	{
		MoveScene(TAB);
	}
}

void UITabProfileAbility::EventSkillChange()
{
	// Image change in Skill data .
	ChangeSkillImage();
	SetMyAbailableSkill();	//가운데 스킬 이미지 테두리처리
	SetMyFixedSkill();		//이미 획득한 스킬 화살표처리

}

void UITabProfileAbility::ChangeSkillImage()
{

}

void UITabProfileAbility::LoadText()
{

}

/*virtual*/ void UITabProfileAbility::OnCreate( i3GameNode * pParent)
{
	UITabProfileBase::OnCreate( pParent);
	// Load Scene AddScene순서 중요합니다. 바꾸지말것.
	AddScene( "UIRe/PointBlankRe_SkillSystem_NEWBIE_LEFT.i3UIe", UITabProfileAbility_Glue);

	//m_SlotSet.OnCreate(this, MAX_ABILITY_SLOT);
	//m_SlotSet.RegisterSlotScene("UIRe/PointBlankRe_item_itemSlot.i3UIe", UITabProfileAbility_Glue);
	//m_ppImgSetCtrl = new UIImgSetCtrl*[MAX_ABILITY_SLOT];
	//::memset( m_ppImgSetCtrl, 0, sizeof(UIImgSetCtrl*) * MAX_ABILITY_SLOT);
}

/*virtual*/ bool UITabProfileAbility::OnEntranceStart( void * pArg1, void * pArg2)
{
	if( UITabProfileBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	return true;
}

/*virtual*/ void UITabProfileAbility::OnUpdate(REAL32 rDeltaSeconds)
{
	UITabProfileBase::OnUpdate(rDeltaSeconds);

	//m_SlotSet.

}