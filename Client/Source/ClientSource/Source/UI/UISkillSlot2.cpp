#include "pch.h"
#include "UISkillSlot2.h"
#include "UIMainFrame.h"
#include "UITabProfileAbility.h"

static UISkillSlot2 * GetThis( LuaState * L)
{
	//UISkillSlot2 * pSelector = GetSelectStagePopup();
	//INT32 idx = pSelector->FindSlot( L);
	//return pSelector->GetSlot( idx);
}

//--------------------------------------------------------------------------//
//								Lua Glue									//
extern "C" {

}

LuaFuncReg UISkillSlot2_Glue[] = {
	{NULL,					NULL}
};

I3_CLASS_INSTANCE( UISkillSlot2, UISlotBase);

UISkillSlot2::UISkillSlot2()
{
}

UISkillSlot2::~UISkillSlot2()
{
}

/*virtual*/ void UISkillSlot2::OnSelect( void)
{
	LuaState * L = _CallLuaFunction( "Select");
	_EndLuaFunction( L, 0);
}

/*virtual*/ void UISkillSlot2::OnUnselect( void)
{
	LuaState * L = _CallLuaFunction( "Unselect");
	_EndLuaFunction( L, 0);
}

/*virtual*/ BOOL UISkillSlot2::OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pParam = (I3UI_CONTROL_NOTIFY*) param2;
		if(pParam->m_nEvent == I3UI_EVT_CLICKED)
		{
			if(!CheckEventInSkillSlot(pParam->m_pCtrl))
			{
				return UISlotBase::OnEvent( event, pObj, param2, code);
			}
			OnClickProcess();
		}
		
	}
	return UISlotBase::OnEvent( event, pObj, param2, code);
}

void UISkillSlot2::OnClickProcess()
{
	m_ActiveCtrl->SetPushFlag(true);
	m_ActiveCtrl->addState(I3UI_STATE_SELECTED);
	m_ActiveCtrl->setModifyState(TRUE);

	m_DeActiveCtrl->SetPushFlag(false);
	m_DeActiveCtrl->removeState(I3UI_STATE_SELECTED);
	m_DeActiveCtrl->setModifyState(TRUE);
}

BOOL UISkillSlot2::CheckEventInSkillSlot(i3UIControl *pCtrl)
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

/*virtual*/ bool UISkillSlot2::OnEntranceStart(void * pArg1 /* = NULL */, void * pArg2 /* = NULL */)
{
	if( UISlotBase::OnEntranceStart( pArg1, pArg2) == false)
		return false;
	m_pCheckBox[0] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonLeft");
	m_pCheckBox[1] = (i3UICheckBox*)GetScene(0)->FindChildByName("i3UIButtonRight");

	return true;
}

/*virtual*/ void UISkillSlot2::OnCreate( i3GameNode * pParent)
{
	UISlotBase::OnCreate( pParent);

	// load scene
	AddScene( "UIRe/PointBlankRe_SkillSystem_AbilitySlot.i3UIe", UISkillSlot2_Glue);
}

void UISkillSlot2::SetLockCtrl(BOOL Sig)
{
	if( Sig == TRUE)
	{

	}
	else
	{
		m_pCheckBox[0]->OnActivate(FALSE);
		m_pCheckBox[0]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[0]->setModifyState(TRUE);

		m_pCheckBox[1]->OnActivate(FALSE);
		m_pCheckBox[1]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[1]->setModifyState(TRUE);
	}
}

void UISkillSlot2::SetSkillImage(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetSkillImage");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L, 1);
}

void UISkillSlot2::SetOutLine(INT32 idx)
{
	LuaState * L = _CallLuaFunction("SetOutLine");
	i3Lua::PushInteger(L, idx);
	_EndLuaFunction(L,1);
}

void UISkillSlot2::SetSelectionSlot(INT32 idx)
{
	if( idx == 1)
	{
		m_pCheckBox[0]->OnActivate(FALSE);
		m_pCheckBox[0]->addState(I3UI_STATE_SELECTED);
		m_pCheckBox[0]->setModifyState(TRUE);

		m_pCheckBox[1]->OnActivate(FALSE);
		m_pCheckBox[1]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[1]->setModifyState(TRUE);
			
	}
	else if(idx == 2)
	{
		m_pCheckBox[1]->OnActivate(FALSE);
		m_pCheckBox[1]->addState(I3UI_STATE_SELECTED);
		m_pCheckBox[1]->setModifyState(TRUE);

		m_pCheckBox[0]->OnActivate(FALSE);
		m_pCheckBox[0]->removeState(I3UI_STATE_SELECTED);
		m_pCheckBox[0]->setModifyState(TRUE);
	}
}