#pragma once

#include "resource/ContentsDefines.h"
#include "resource/Settings.h"
#include "util/ThreadPool.hpp"

namespace { struct MQContentsStatusTreeCallback; }

namespace ui
{
	class MQContentsStatusTree;
	class QLContentsStatusBar;

	class MQContentsStatusBar : public CDockablePane
	{
	public:
		MQContentsStatusBar();
		virtual ~MQContentsStatusBar();

		void OnClose();
		void ShowContentsStatusTree(const rsc::ContentsDataPtr& contentsData, const std::wstring& wstrCountryName);
		void SetQLContentsStatusBar(ui::QLContentsStatusBar* bar) { m_QLStatusBar = bar; }
		void SelectQLTreeItem(const std::wstring& wstrPath);
		void SelectTreeItem(const std::wstring& wstrPath);
		void Clear();

	private:
		CBrush m_bkGndBrush;
		std::unique_ptr<MQContentsStatusTree> m_treeCtrl;
		std::unique_ptr<ThreadPool> m_thpool;
		ui::QLContentsStatusBar* m_QLStatusBar = nullptr;
		rsc::ContentsDataPtr m_currContentsData;
		rsc::CountryDataPtr m_currCountryData;

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		friend struct MQContentsStatusTreeCallback;
	};
}
