#include "pch.h"

#if defined(SKILLSYSTEM)
#include "UISkillSelectionSlot.h"
#include "UIMainFrame.h"

#include "UITabProfileAbility.h"
#include "UIPopupLearnSkill.h"

//static UISkillSelectionSlot * GetThis( LuaState * L)
//{
//	UISkillSelectionSlot * pSelector = GetSelectStagePopup();
//	INT32 idx = pSelector->FindSlot( L);
//	return pSelector->GetSlot( idx);
//}

extern "C" {

}

LuaFuncReg UISkill_Selection_Slot_Glue[] = {
	{NULL,					NULL}
};

I3_CLASS_INSTANCE( UISkillSelectionSlot);//, UISlotBase);

UISkillSelectionSlot::UISkillSelectionSlot()
{
	m_bCanSelect	= FALSE;
	m_pImageSet		= NULL;
	m_ActiveCtrl	= NULL;
	m_DeActiveCtrl	= NULL;
	m_pCheckBox[0]	= NULL;
	m_pCheckBox[1]	= NULL;
}

UISkillSelectionSlot::~UISkillSelectionSlot()
{
}

/*virtual*/ BOOL UISkillSelectionSlot::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		if(pParam->m_nEvent == I3UI_EVT_CLICKED || pParam->m_nEvent == I3UI_EVT_DBL_CLICKED)
		{
			if(!CheckEventInSkillSlot(pParam->m_pCtrl))
			{
				return UISlotBase::OnEvent( event, pObj, param2, code);
			}
			OnClickProcess();
			if(pParam->m_nEvent == I3UI_EVT_DBL_CLICKED && m_bCanSelect)
				static_cast<UITabProfileAbility*>(m_pParent)->Learn();
		}

		else if(pParam->m_nEvent == I3UI_EVT_MOUSEWHEEL)
		{
			i3UIScrollBar * pScroll = ((UITabProfileAbility*)m_pParent)->getScrollBar();
			if(pScroll == NULL)
				return UISlotBase::OnEvent( event, pObj, param2, code);
			INT32 scroll_pos = pScroll->getScrollPos();
			if(pParam->m_nPriParam < 0)
				pScroll->setScrollPos(scroll_pos + 1);
			else
				pScroll->setScrollPos(scroll_pos - 1);
		}
		
	}
	return UISlotBase::OnEvent( event, pObj, param2, code);
}

void UISkillSelectionSlot::OnClickProcess()
{
	if(!i3::same_of<UITabProfileAbility*>(m_pParent)) //->IsExactTypeOf(UITabProfileAbility::static_meta()) == false)
		return;

	if(m_bCanSelect)
	{
		m_ActiveCtrl->SetPushFlag(true);
		m_ActiveCtrl->addState(I3UI_STATE_SELECTED);
		m_ActiveCtrl->setModifyState(TRUE);

		m_DeActiveCtrl->SetPushFlag(false);
		m_DeActiveCtrl->removeState(I3UI_STATE_SELECTED);
		m_DeActiveCtrl->setModifyState(TRUE);

		if(m_pCheckBox[0] == m_ActiveCtrl)
			SetArrowDirection(3,-1);
		else
			SetArrowDirection(-1,0);
	}

	static_cast<UITabProfileAbility*>(m_pParent)->LoadSkillSlotInfo(this);
	static_cast<UITabProfileAbility*>(m_pParent)->LoadSkillSlotDetailInfo(this,( m_pCheckBox[0] == m_ActiveCtrl ? 0 : 1));
}

BOOL UISkillSelectionSlot::CheckEventInSkillSlot(i3UIControl *pCtrl)
{
	for(INT32 i=0; i < 2; i++)
	{
		if(m_pCheckBox[i] == pCtrl)
		{
			m_ActiveCtrl = m_pCheckBox[i];
			if(i==0)
				m_DeActiveCtrl = m_pCheckBox[1];
			else if(i==1)
				m_DeActiveCtrl = m_pCheckBox[0];
			return TRUE;
		}
	}
	return FALSE;
}

INT32 UISkillSelectionSlot::GetDirection()
{
	if(m_ActiveCtrl == NULL)
		return 0;
	if(m_ActiveCtrl == m_pCheckBox[0])
		return 1;
	return 2;
}

/*virtual*/ void UISkillSelectionSlot::_InitializeAtLoad(i3UIScene * pScene)
{
	UISlotBase::_InitializeAtLoad(pScene);
	m_pImageSet = (i3UIButtonImageSet*)GetScene(0)->FindChildByName("i3UISkillImage");
	
}

/*virtual*/ void UISkillSelectionSlot::SetEnable( BOOL bTrue, REAL32 tm )
{
	UISlotBase::SetEnable(bTrue,tm);
	if(bTrue == FALSE)
		return;

	SkillManager * skillMng = g_pGameContext->getSkillManager();

	INT32 _Tab = 0;
	INT32 _Avatar = 0;
	if(i3::same_of<UITabProfileAbility*>(m_pParent)) //->IsExactTypeOf(UITabProfileAbility::static_meta()))
	{
		_Tab = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentAvatar();
	}
	else if(i3::same_of<UIPopupLearnSkill*>(m_pParent)) //->IsExactTypeOf(UIPopupLearnSkill::static_meta()))
	{
		_Tab = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentAvatar();
	}
	
	eSKILL_CLASS _Class= (eSKILL_CLASS)skillMng->getCurrentClass(_Avatar,_Tab);

	if(UISlotBase::isEnable() == bTrue && bTrue != 0)
	{
		if (_Class == SKILL_ASSAULT)
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL1");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR1");;
			SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_RED);
		}
		else if(_Class == SKILL_SMG)
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL2");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR2");;
			SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_GREEN);
		}
		else if(_Class == SKILL_SNIPER)
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL3");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR3");;
			SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_BLUE);
		}
		else if(_Class == SKILL_MACHINE)
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL4");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR4");;
			SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_YELLOW);
		}
		else if(_Class == SKILL_SHOTGUN)
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL5");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR5");;
			SetCheckBoxEnable(SLOT_TYPE_PURPLE);
		}
		else if(_Class > SKILL_SHOTGUN )
		{
			m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonL6");
			m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonR6");;
			SetCheckBoxEnable(UISkillSelectionSlot::SLOT_TYPE_GRAY);
		}

		m_pCheckBox[0]->setSize(215,38);
		m_pCheckBox[1]->setSize(215,38);
		m_pCheckBox[0]->SetFreeSize(true);
		m_pCheckBox[1]->SetFreeSize(true);

	}
}

/*virtual*/ void UISkillSelectionSlot::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "UIRe/Scene/Main/PointBlankRe_Profile_Skill_Select_Slot.i3UIe", UISkill_Selection_Slot_Glue);
}

void UISkillSelectionSlot::SetSkillImage(INT32 idx)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();

	INT32 _Tab = 0;
	INT32 _Avatar = 0;
	if(i3::same_of<UITabProfileAbility*>(m_pParent)) //->IsExactTypeOf(UITabProfileAbility::static_meta()))
	{
		_Tab = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentAvatar();
	}
	else if(i3::same_of<UIPopupLearnSkill*>(m_pParent)) //->IsExactTypeOf(UIPopupLearnSkill::static_meta()))
	{
		_Tab = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentAvatar();
	}

	eSKILL_CLASS _Class= (eSKILL_CLASS)skillMng->getCurrentClass(_Avatar,_Tab);

	idx += skillMng->getImageStartIndexByClass(_Class);
	//if(_Class == SKILL_SHOTGUN)
	//	idx = -1;

	LuaState * L = _CallLuaFunction("SetSkillImage");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L, 1);
}

void UISkillSelectionSlot::SetSkillText(INT32 idx)
{
	SkillManager * skillMng = g_pGameContext->getSkillManager();
	INT32 _Tab = 0;
	INT32 _Avatar = 0;

	if(i3::same_of<UITabProfileAbility*>(m_pParent)) //->IsExactTypeOf(UITabProfileAbility::static_meta()))
	{
		_Tab = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UITabProfileAbility*>(m_pParent)->GetCurrentAvatar();
	}
	else if(i3::same_of<UIPopupLearnSkill*>(m_pParent)) //->IsExactTypeOf(UIPopupLearnSkill::static_meta()))
	{
		_Tab = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentTab();
		_Avatar = static_cast<UIPopupLearnSkill*>(m_pParent)->GetCurrentAvatar();
	}
	eSKILL_CLASS _Class= (eSKILL_CLASS)skillMng->getCurrentClass(_Avatar,_Tab);
	
	I3_BOUNDCHK(_Class, SKILL_TYPE_MAX);

	const char *LText = skillMng->getSkillTextByClass(_Class, SkillManager::LEFT_SLOT, idx);
	const char *RText = skillMng->getSkillTextByClass(_Class, SkillManager::RIGHT_SLOT, idx);

	LuaState * L = _CallLuaFunction("SetSkillText");
	i3Lua::PushA2UTF8String(L, LText);
	i3Lua::PushA2UTF8String(L, RText);
	_EndLuaFunction(L, 2);
}

void UISkillSelectionSlot::SetOutLine(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetOutLine");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L,1);
}

void UISkillSelectionSlot::SetBG(BOOL Sig)
{
	LuaState * L = _CallLuaFunction("SetBG");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L,1);
}

void UISkillSelectionSlot::SetSlotEnable(BOOL Sig)
{
	LuaState * L = _CallLuaFunction("SetSlotEnable");
	i3Lua::PushBoolean(L, Sig);
	_EndLuaFunction(L,1);
}

void UISkillSelectionSlot::SetCheckBoxEnable(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetCheckBoxEnable");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L,1);
}

void UISkillSelectionSlot::SetArrowDirection(INT32 idx1, INT32 idx2)
{
	LuaState * L = _CallLuaFunction("SetArrowDirection");
	i3Lua::PushInteger(L, idx1);
	i3Lua::PushInteger(L, idx2);
	_EndLuaFunction(L,2);
}

void UISkillSelectionSlot::SetSelectionSlot(INT32 idx)
{
	if( idx == 0)
	{
		m_bCanSelect = FALSE;
		m_pCheckBox[0]->SetHoldImage(TRUE, 5);
		m_pCheckBox[0]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[0]->setModifyState(TRUE);
		m_pCheckBox[1]->SetHoldImage(TRUE, 5);
		m_pCheckBox[1]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[1]->setModifyState(TRUE);

		m_pCheckBox[0]->SetPushFlag(false);
		m_pCheckBox[1]->SetPushFlag(false);
	}

	else if( idx == 1)
	{
		m_bCanSelect = FALSE;
		m_pCheckBox[0]->SetHoldImage(TRUE, 4);
		m_pCheckBox[0]->setModifyState(TRUE);
		m_pCheckBox[1]->SetHoldImage(TRUE, 5);
		m_pCheckBox[1]->setModifyState(TRUE);
			
		m_pCheckBox[0]->SetPushFlag(false);
		m_pCheckBox[1]->SetPushFlag(false);
	}
	else if(idx == 2)
	{
		m_bCanSelect = FALSE;
		m_pCheckBox[0]->SetHoldImage(TRUE, 5);
		m_pCheckBox[0]->setModifyState(TRUE);
		m_pCheckBox[1]->SetHoldImage(TRUE, 4);
		m_pCheckBox[1]->setModifyState(TRUE);

		m_pCheckBox[0]->SetPushFlag(false);
		m_pCheckBox[1]->SetPushFlag(false);
	}
	else
	{
		m_bCanSelect = TRUE;
		m_pCheckBox[0]->setModifyState(TRUE);
		m_pCheckBox[0]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[0]->SetHoldImage(FALSE);
		m_pCheckBox[1]->setModifyState(TRUE);
		m_pCheckBox[1]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[1]->SetHoldImage(FALSE);

		m_pCheckBox[0]->SetPushFlag(false);
		m_pCheckBox[1]->SetPushFlag(false);
	}
}
#endif