#include "stdafx.h"
#include "CustomButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	CustomButton::CustomButton() : m_callback(new CustomButtonCallback)
	{
	}

	CustomButton::~CustomButton()
	{
	}

	void CustomButton::SetColor(COLORREF bkColor, COLORREF textColor)
	{
		m_clrText = textColor;

		m_clrBkgnd = bkColor;
		m_brBkgnd.CreateSolidBrush(m_clrBkgnd);
	}

	IMPLEMENT_DYNAMIC(CustomButton, CButton)

	BEGIN_MESSAGE_MAP(CustomButton, CButton)
		ON_CONTROL_REFLECT(BN_CLICKED, &CustomButton::OnBnClicked)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		//ON_WM_CTLCOLOR_REFLECT()
	END_MESSAGE_MAP()

	void CustomButton::OnBnClicked()
	{
		m_callback->OnClick(this);
	}


	void CustomButton::OnLButtonDown(UINT nFlags, CPoint point)
	{
		CButton::OnLButtonDown(nFlags, point);

		m_callback->OnLButtonDown(this);
	}

	void CustomButton::OnLButtonUp(UINT nFlags, CPoint point)
	{
		CButton::OnLButtonUp(nFlags, point);

		m_callback->OnLButtonUp(this);
	}

	HBRUSH CustomButton::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(m_clrText);
		pDC->SetBkColor(m_clrBkgnd);

		return m_brBkgnd;
	}
}