#include "pch.h"
#include "StepBase.h"
#include "GlobalVariables.h"

I3_ABSTRACT_CLASS_INSTANCE(CStepBase, CGuiNotifyReceiver);

CStepBase::CStepBase()
{
	m_pChatBody = NULL;
	
	m_bVestigeDuty = false;
	m_OldidxStartSlot = -1;
	m_OldidxTotalSlot = -1;
	m_OldWeaponUsageType = WEAPON_SLOT_UNKNOWN;
}

CStepBase::~CStepBase()
{
}


void CStepBase::OnStart(void)
{
	CGuiNotifyReceiver::SlideIn();

	if( !isEnable())
	{
		PlaySound(GUI_SND_SCROLLIN);
	}

	SetEnable( TRUE);
	OnUpdate(__RT_ZERO);
}

void CStepBase::OnEnd(void)
{
	CGuiNotifyReceiver::SlideOut();
}

void CStepBase::OnInitRun(void)
{
	// empty
}

void CStepBase::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CGuiNotifyReceiver::OnUpdate(rDeltaSeconds);
}

BOOL CStepBase::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;
			i3InputKeyboard* pKeyboard = pMgr->GetKeyboard();

			if (pKeyboard->GetStrokeState(I3I_KEY_ESC))
			{
				OnExitKey();
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CStepBase::OnSliding(RT_REAL32 rDeltaSeconds)
{
	if (FALSE == CGuiNotifyReceiver::OnSliding(rDeltaSeconds))
	{
		return FALSE;
	}

	// 활성화 시, 1회 초기화
	if (SS_IN == CGuiNotifyReceiver::GetSlideState())
	{
		OnInitRun();
	}

	if (SS_OUT == CGuiNotifyReceiver::GetSlideState())
	{
		SetEnable( FALSE);
	}

	return TRUE;
}

void CStepBase::AuthStartSuccess(INT32 arg)
{
}

void CStepBase::OnInitControl(void)
{
	CGuiNotifyReceiver::OnInitControl();
}

BOOL CStepBase::IsRun(void)
{
	// 스탭 윈도우의 이동이 끝났으면 참, 이동중이면 거짓.
	// 스탭 윈동우가 등장완료, 퇴장완료 이면 이동이 끝난다.
	if (SS_IN == CGuiNotifyReceiver::GetSlideState() || SS_OUT == CGuiNotifyReceiver::GetSlideState())
	{
		return TRUE;
	}

	return FALSE;
}

void	CStepBase::_ResetVestige()
{
	m_bVestigeDuty = false;
}

void	CStepBase::_SetupVestige()
{
#ifndef NC_BUILD
	m_bVestigeDuty = true;
#else
	if( LocaleValue::Enable("Goods_Vestige") )
		m_bVestigeDuty = true;
	else
		m_bVestigeDuty = false;
#endif
}

WEAPON_SLOT_TYPE	CStepBase::_GetFirstGroup()
{
	if( m_bVestigeDuty )
	{
		return m_OldWeaponUsageType;
	}

	return WEAPON_SLOT_UNKNOWN;
}

UINT32 CStepBase::_GetFirstSelectItem()
{
	if( m_bVestigeDuty )
	{
		return m_OldidxTotalSlot;
	}

	return (UINT32)INVALID_INDEX;
}

UINT32 CStepBase::_GetFirstStartItem()
{
	if( m_bVestigeDuty )
	{
		return m_OldidxStartSlot;
	}

	return 0;
}