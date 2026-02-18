#include "pch.h"
#include "UIUtil.h"
#include "UIClanSlot.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/format_string.h"

UIClanSlot :: UIClanSlot()
: m_pClanMark	(nullptr)
, m_pClanName	(nullptr)
, m_pVisitBtn	(nullptr)
, m_i32ClanIdx  (-1)
, m_nClanMarkSize  (0)
{

};

UIClanSlot::~UIClanSlot()
{

}

bool UIClanSlot::Init(i3UIFrameWnd * pWnd, INT32 idx)
{
	if(pWnd != nullptr)
	{
		if (g_pEnvSet->IsBaseOnUIVersion_1_0() )
		{
			m_nClanMarkSize = 60;
		}
		else
		{
			m_nClanMarkSize = 64;
		}

		char szClanName[ MAX_NAME ] = {0,};
		char szClanIntro[ MAX_NAME ] = {0,};
		char szClanMark[ MAX_NAME ] = {0,};
		char szVisitBtn [ MAX_NAME ] = {0,};

		i3::snprintf(szClanName,		MAX_NAME, "StaticText_ClanName%d",	idx);
		i3::snprintf(szClanIntro,		MAX_NAME, "Edit_ClanIntro%d",		idx);
		i3::snprintf(szClanMark,		MAX_NAME, "ImgBox_ClanMark%d",		idx);
		i3::snprintf(szVisitBtn,		MAX_NAME, "Btn_VisitClan%d",		idx);

		m_pClanName = (i3UIStaticText *) pWnd->FindChildByName(szClanName);
		m_pClanMark = (i3UIImageBoxEx *) pWnd->FindChildByName(szClanMark);
		m_pVisitBtn = (i3UIButton *) pWnd->FindChildByName(szVisitBtn);
		m_pFrameWnd = pWnd;

		m_FlowEditBox.SetFlowEditBox( static_cast<i3UIEditBox*>( pWnd->FindChildByName(szClanIntro)));

		I3ASSERT( m_pClanName );
		I3ASSERT( m_pClanMark );
		I3ASSERT( m_pVisitBtn );

		return true;
	}
	else
	{
		m_pClanName = nullptr;
		m_pClanMark = nullptr;
		m_pVisitBtn = nullptr;
	}

	return false;
}

void UIClanSlot::SetSlotData(SORT_CLAN_INFO * pInfo, INT32 i32SelSlot)
{
	if(pInfo != nullptr && m_pClanName != nullptr)
	{
		m_i32ClanIdx = (INT32)pInfo->_info._idx;

		{
			i3::wliteral_range wstrName = pInfo->_info._name;
			m_pClanName->SetText(wstrName);
		}

		GameUI::SetClanTexture(m_pClanMark, CLAN_MARK_MEDIUM, pInfo->_info._mark);

		{
			i3::rc_wstring wstrIntro;
			wstrIntro = pInfo->_Intro;
			m_FlowEditBox.SetFlowTextData(wstrIntro, I3UI_EDITBOX_STYLE_SCROLL_LEFT);
		}

		// 여기서 사용되는 클랜마크 사이즈는 64 x 64
		m_pClanMark->SetShapeSize( (REAL32)m_nClanMarkSize, (REAL32)m_nClanMarkSize);
		SetSlotEnable(true, i32SelSlot);
	}
	else
	{
		m_i32ClanIdx = -1;

		if (m_pClanName)
			SetSlotEnable(false, i32SelSlot);
	}
}

void UIClanSlot::SetSlotEnable(bool bEnable, INT32 i32SelSlot)
{
	if( m_pClanMark != nullptr)
	{
		m_pClanMark->EnableCtrl(bEnable);
		m_pClanName->EnableCtrl(bEnable);
		m_pVisitBtn->EnableCtrl(bEnable);
	}

	m_FlowEditBox.SetEnable(bEnable);

	if (m_pFrameWnd)
		m_pFrameWnd->setInputDisable(!bEnable);

	SelectSlot(i32SelSlot);
}

void UIClanSlot::SelectSlot(INT32 i32SelSlot)
{
	if( !m_FlowEditBox.IsStartFlag() )
		m_FlowEditBox.PlayEditTextFlow(TFD_HORIZONTAL, false, true);
	/*if( m_pSelectFrame != nullptr)
	{
		if(m_i32ClanIdx > -1 && m_i32ClanIdx == i32SelSlot)
		{
		//	m_pSelectFrame->EnableCtrl(true);
			m_FlowEditBox.PlayEditTextFlow(TFD_HORIZONTAL, true);
		}
		else
		{
		//	m_pSelectFrame->EnableCtrl(false);
			m_FlowEditBox.StopEditTextFlow();
		}
	}*/
}

void UIClanSlot::OnUpdate(REAL32 rDeltaSeconds)
{
	m_FlowEditBox.OnUpdate(rDeltaSeconds);
}