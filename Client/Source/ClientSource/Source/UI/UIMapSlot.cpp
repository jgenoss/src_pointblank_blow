#include "pch.h"
#include "UIMapSlot.h"

#include "ClientStageInfoUtil.h"

UIMapSlot::UIMapSlot()
	: m_wnd(nullptr)
	, m_img(nullptr)
	, m_frame(nullptr)
	, m_text(nullptr)
	, m_bg (nullptr)
	, m_stage(nullptr)
	, m_isSelected(false)
	, m_isMouseOver(false)
	, m_isModeVisible(false)
{
}

UIMapSlot::~UIMapSlot()
{
}

bool	UIMapSlot::Init(i3UIFrameWnd* wnd)
{
	m_wnd = wnd;
	I3ASSERT(m_wnd);

	if (nullptr != m_wnd)
	{
		const char* name = m_wnd->GetName();

		char imgName[MAX_NAME];
		char frameName[MAX_NAME];
		char textName[MAX_NAME];

		char bgName[MAX_NAME];
		char lockName[MAX_NAME];

		char modetext[MAX_NAME];
		char modebg[MAX_NAME];

		if (name)
		{
			i3::snprintf(imgName, MAX_NAME, "%s_MapImg", name);
			i3::snprintf(frameName, MAX_NAME, "%s_Frame", name);
			i3::snprintf(textName, MAX_NAME, "%s_MapName", name);

			i3::snprintf(bgName, MAX_NAME, "%s_bg", name);
			i3::snprintf(lockName, MAX_NAME, "%s_lock", name);

			i3::snprintf(modetext, MAX_NAME, "%s_ModeName", name);
			i3::snprintf(modebg, MAX_NAME, "%s_ModeNameBG", name);
		}

		m_img = (i3UIButtonImageSet*)m_wnd->FindChildByName(imgName);
		m_frame = (i3UIButtonImageSet*)m_wnd->FindChildByName(frameName);
		m_text = (i3UIStaticText*)m_wnd->FindChildByName(textName);

		m_bg = (i3UIControl*)m_wnd->FindChildByName(bgName);
		m_lock = (i3UIControl*)m_wnd->FindChildByName(lockName);

		m_modename_bg = (i3UIControl*)m_wnd->FindChildByName(modebg);
		m_modetext = (i3UIStaticText*)m_wnd->FindChildByName(modetext);

		I3ASSERT(m_img);
		I3ASSERT(m_frame);
		I3ASSERT(m_text);

		if (m_wnd)
		{
			m_wnd->setInputDisable(false);
			m_wnd->removeStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_img)
		{
			m_img->setInputDisable(true);
			m_img->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_frame)
		{
			m_frame->setInputDisable(true);
			m_frame->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_text)
		{
			m_text->setInputDisable(true);
			m_text->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_bg)
		{
			m_bg->EnableCtrl(false);
			m_bg->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_lock)
		{
			m_lock->EnableCtrl(false);
			m_lock->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_modename_bg)
		{
			m_modename_bg->EnableCtrl(false);
			m_modename_bg->addStyle(I3UI_STYLE_IGNOREINPUT);
		}

		if (m_modetext)
		{
			m_modetext->EnableCtrl(false);
			m_modetext->addStyle(I3UI_STYLE_IGNOREINPUT);
		}


		SetSelect(false);
		OnMouseOver(false);
		
		m_stage = nullptr;

		m_isModeVisible = false;

		return true;
	}

	return false;
}

void	UIMapSlot::SetMapInfo(CSI_STAGE* info)
{
	if (m_img)
		m_img->SetShapeIdx( g_pFramework->GetMapShapeIdx(info->GetStageUID()) );

	if (m_text)
		m_text->SetText(CStageInfoUtil::GetMapData((UINT8)info->GetStageUID())->getDisplayName().c_str());

	if (!info->IsEventforEventType(STAGE_EVENT_NORMAL))
	{
		if (m_modetext) m_modetext->SetText(CStageInfoUtil::GetRuleData(info->m_ui32StageID)->getRuleName());
		m_isModeVisible = true;
	}
	else
		m_isModeVisible = false;
		
		
	m_stage = info;
}

void	UIMapSlot::SetVisible(bool bVisible)
{
	if (m_wnd)
		m_wnd->SetVisible(bVisible ? true : false);

	if (m_img)
		m_img->SetVisible(bVisible ? true : false);

	if (m_text)
		m_text->SetVisible(bVisible ? true : false);

	if (m_frame)
		m_frame->SetVisible(bVisible ? true : false);

	if (m_modename_bg )
		m_modename_bg->SetVisible(bVisible && m_isModeVisible ? true : false);

	if (m_modetext )
		m_modetext->SetVisible(bVisible && m_isModeVisible ? true : false);
}

void	UIMapSlot::SetEnable(bool bEnable)
{
	if (m_wnd)
		m_wnd->EnableCtrl(bEnable ? true : false);

	if (m_img)
		m_img->EnableCtrl(bEnable ? true : false);

	if (m_text)
		m_text->EnableCtrl(bEnable ? true : false);

	if (m_frame)
		m_frame->EnableCtrl(bEnable ? true : false);

	if (m_modename_bg )
		m_modename_bg->EnableCtrl(bEnable && m_isModeVisible ? true : false);

	if (m_modetext )
		m_modetext->EnableCtrl(bEnable && m_isModeVisible ? true : false);
}


void UIMapSlot::SetActivate(bool bActivate)
{
	if (nullptr != m_bg)
		m_bg->EnableCtrl(!bActivate);
}

void	UIMapSlot::SetSelect(bool bSel)
{
	m_isSelected = bSel;

	if (m_frame)
	{
		if (m_isSelected)
			m_frame->SetShapeIdx(0);
		else
		{
			if (m_isMouseOver == true)
				m_frame->SetShapeIdx(1);
			else
				m_frame->EnableCtrl(false);
		}
	}

	if (m_text)
	{
		COLOR cr;

		if (false == bSel)
		{
			/*cr.r = 140;
			cr.g = 124;
			cr.b = 100;
			cr.a = 255;*/
			cr.SetARGB(0xff8c7c64);
		}
		else
		{
			cr.SetRGBA(0xffffffff);
		}

		m_text->SetTextColor(&cr);
	}
}

void	UIMapSlot::OnMouseOver(bool bOver)
{
	m_isMouseOver = bOver;

	if (m_frame)
	{
		if (m_isSelected == false)
		{
			if (m_isMouseOver)
			{
				m_frame->EnableCtrl(true);
				m_frame->SetShapeIdx(1);
			}
			else
			{
				m_frame->EnableCtrl(false);
			}
		}
	}
}