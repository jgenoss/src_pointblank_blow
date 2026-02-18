#include "stdafx.h"
#include "ContentsDescriptionBar.h"
#include "UIDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	ContentsDescriptionBar::ContentsDescriptionBar()
	{
	}

	ContentsDescriptionBar::~ContentsDescriptionBar()
	{
	}

	void ContentsDescriptionBar::ShowContentsDesc(const rsc::ContentsDataPtr& contentsData, const std::wstring& wstrCountryName)
	{
		std::wstring wstr;

		i3::mb_to_utf16(contentsData->strContentsName, wstr);
		m_descListPtr->SetContentsName(wstr);

		i3::mb_to_utf16(contentsData->strCategoryName, wstr);
		m_descListPtr->SetCategoryName(wstr);

		i3::mb_to_utf16(contentsData->strSubcategoryName, wstr);
		m_descListPtr->SetSubCategoryName(wstr);

		m_descListPtr->SetContentsStatus(rsc::ConvertContentsStatusToString(contentsData->status));

		m_descListPtr->SetCountryName(wstrCountryName);
	}

	void ContentsDescriptionBar::Clear()
	{
		m_descListPtr->SetContentsName(L"");
		m_descListPtr->SetCategoryName(L"");
		m_descListPtr->SetSubCategoryName(L"");
		m_descListPtr->SetCountryName(L"");
	}

	BEGIN_MESSAGE_MAP(ContentsDescriptionBar, CDockablePane)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_CTLCOLOR_REFLECT()
		ON_WM_ERASEBKGND()
	END_MESSAGE_MAP()

	int ContentsDescriptionBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDockablePane::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);

		m_descListPtr = std::make_unique<ContentsDescList>(*this);

		return 0;
	}

	void ContentsDescriptionBar::OnSize(UINT nType, int cx, int cy)
	{
		CDockablePane::OnSize(nType, cx, cy);

		if (m_descListPtr.get())
			m_descListPtr->AdjustLayout(0, 0, cx, cy);
	}

	HBRUSH ContentsDescriptionBar::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		UNREFERENCED_PARAMETER(nCtlColor);

		pDC->SetBkColor(COLOR_IVORY_BLACK);

		return m_bkGndBrush;
	}

	BOOL ContentsDescriptionBar::OnEraseBkgnd(CDC* pDC)
	{
		CRect rect;
		GetClientRect(&rect);
		pDC->FillRect(&rect, &m_bkGndBrush);

		return 1;
	}
}
