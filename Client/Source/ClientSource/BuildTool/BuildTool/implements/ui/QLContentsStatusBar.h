#pragma once

#include "resource/ContentsDefines.h"
#include "resource/Settings.h"
#include "util/ThreadPool.hpp"

namespace { struct QLContentsStatusTreeCallback; }

namespace ui
{
	class QLContentsStatusTree;
	class MQContentsStatusBar;

	class QLContentsStatusBar : public CDockablePane
	{
	public:
		QLContentsStatusBar();
		virtual ~QLContentsStatusBar();

		void SetOwner(MQContentsStatusBar* owner);

		void ShowContentsStatusTree(const std::vector<PathPtr>& fileRelPathList, 
			const rsc::ContentsDataPtr& contentsData, const rsc::CountryDataPtr& countryData);

		void Clear();

		void SelectTreeItem(const std::wstring& wstrPath);

	private:
		CBrush m_bkGndBrush;
		std::unique_ptr<QLContentsStatusTree> m_treeCtrl;
		MQContentsStatusBar* m_owner;
		rsc::ContentsDataPtr m_currContentsData;
		rsc::CountryDataPtr m_currCountryData;

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		friend struct QLContentsStatusTreeCallback;
	};
}

