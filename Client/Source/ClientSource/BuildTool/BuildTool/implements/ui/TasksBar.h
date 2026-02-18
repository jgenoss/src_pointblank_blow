#pragma once

#include "LiveCountryList.h"
#include "SettingsList.h"
#include "ToolList.h"

namespace ui
{
	class TasksBar : public CDockablePane
	{
		DECLARE_DYNCREATE(TasksBar)

	public:
		TasksBar();
		virtual ~TasksBar();

		void OnClickCountry(const std::wstring& wstrCountryName);
		void BuildupCountries(const std::vector<std::string>& strCountryNameList);

	private:
		std::unique_ptr<LiveCountryList> m_countryListCtrl;
		std::unique_ptr<SettingsList> m_settingsListCtrl;
		std::unique_ptr<ToolList> m_toolListCtrl;

	public:
#ifdef _DEBUG
		virtual void AssertValid() const;
#ifndef _WIN32_WCE
		virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};
}