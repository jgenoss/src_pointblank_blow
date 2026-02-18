#include "stdafx.h"
#include "CustomEditBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	CustomEditBox::CustomEditBox() : m_callback(new CustomEditBoxCallback)
	{
	}

	CustomEditBox::~CustomEditBox()
	{
	}

	void CustomEditBox::SetColor(COLORREF bkColor, COLORREF textColor)
	{
		m_clrBkgnd = bkColor;
		m_clrText = textColor;
		m_brBkgnd.CreateSolidBrush(m_clrBkgnd);
	}

	IMPLEMENT_DYNAMIC(CustomEditBox, CEdit)

	BEGIN_MESSAGE_MAP(CustomEditBox, CEdit)
		ON_WM_CTLCOLOR_REFLECT()
	END_MESSAGE_MAP()

	HBRUSH CustomEditBox::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrText);
		pDC->SetBkColor(m_clrBkgnd);

		return m_brBkgnd;
	}
}