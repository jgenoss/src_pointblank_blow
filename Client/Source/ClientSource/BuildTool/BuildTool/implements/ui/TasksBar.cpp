#include "stdafx.h"
#include "TasksBar.h"
#include "Resource.h"
#include "MainFrm.h"
#include "ContentsView.h"
#include "../resource/ResourceController.h"
#include "BuildTool.h"
#include "UIDefines.h"
#include "ToolFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	IMPLEMENT_DYNCREATE(TasksBar, CDockablePane)

	TasksBar::TasksBar()
	{
	}

	TasksBar::~TasksBar()
	{
	}

	void TasksBar::OnClickCountry(const std::wstring& wstrCountryName)
	{
		m_settingsListCtrl->ClearSelection();

		const std::string strCountryName(wstrCountryName.begin(), wstrCountryName.end());
		rsc::ResourceController::i()->OnSelectCountry(strCountryName);
	}

	void TasksBar::BuildupCountries(const std::vector<std::string>& strCountryNameList)
	{
		m_countryListCtrl->Clear();

		for (const auto &currCountryName : strCountryNameList)
			m_countryListCtrl->AddCountry(currCountryName);
	}

#ifdef _DEBUG
	void TasksBar::AssertValid() const
	{
		CDockablePane::AssertValid();
	}

#ifndef _WIN32_WCE
	void TasksBar::Dump(CDumpContext& dc) const
	{
		CDockablePane::Dump(dc);
	}
#endif
#endif //_DEBUG

	BEGIN_MESSAGE_MAP(TasksBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int TasksBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1) return -1;

		m_settingsListCtrl = std::make_unique<SettingsList>(this);

		m_countryListCtrl = std::make_unique<LiveCountryList>(this);

		m_toolListCtrl = std::make_unique<ToolList>(this);
		
		std::vector<std::string> toolNames;
		ToolFactory::i()->GetAllToolName(toolNames);

		for (const auto& strToolName: toolNames)
			m_toolListCtrl->AddTool(strToolName);

		rsc::ResourceController::i()->OnBuildup();

		std::vector<rsc::CountryDataPtr> countryDataList;
		rsc::CountrySettings::i()->GetDataList(countryDataList);

		for (auto &currCountryData : countryDataList)
			m_countryListCtrl->AddCountry(currCountryData->GetCountryName());

		return 0;
	}

	void TasksBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		RECT rt;	
		GetClientRect(&rt);

		rt.top = 20;	
		rt.bottom = 110;

		if (m_settingsListCtrl.get())
			m_settingsListCtrl->AdjustLayout(rt);

		if (m_toolListCtrl.get())
		{
			rt.top = rt.bottom;	

			long h = 30;
			const long numOfTools = m_toolListCtrl->GetNumOfTools();
			h += numOfTools * 20;
			rt.bottom += h;

			m_toolListCtrl->AdjustLayout(rt);
		}

		if (m_countryListCtrl.get())
		{
			rt.top = rt.bottom;
			rt.bottom = cy;
			m_countryListCtrl->AdjustLayout(rt);
		}
	}

	BOOL TasksBar::OnEraseBkgnd(CDC* pDC)
	{
		CBrush brush(COLOR_IVORY_BLACK);

		CBrush* pOldBrush = pDC->SelectObject(&brush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}
}