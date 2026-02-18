#pragma once

#include "ContentsDescriptionList.h"
#include "resource/ContentsDefines.h"

namespace rsc { struct ContentsData; }

namespace ui
{
	class ContentsDescriptionBar : public CDockablePane
	{
	public:
		ContentsDescriptionBar();
		virtual ~ContentsDescriptionBar();

		void ShowContentsDesc(const rsc::ContentsDataPtr& contentsData, const std::wstring& wstrCountryName);
		void Clear();

	private:
		CBrush m_bkGndBrush;
		std::unique_ptr<ContentsDescList> m_descListPtr;

	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	};
}