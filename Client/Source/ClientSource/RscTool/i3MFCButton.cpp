#include "stdafx.h"
#include "i3MFCButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNAMIC(i3MFCButton, CMFCButton)

BEGIN_MESSAGE_MAP(i3MFCButton, CMFCButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &i3MFCButton::OnBnClicked)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


namespace
{
	i3MFCButtonCallback		g_default_callback;
}


i3MFCButton::i3MFCButton() : m_callback(&g_default_callback)
{
	
}

i3MFCButton::~i3MFCButton()
{
	
}


void i3MFCButton::OnBnClicked()
{
	m_callback->OnClick(this);
}


void i3MFCButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CMFCButton::OnLButtonDown(nFlags, point);

	m_callback->OnLButtonDown(this);

}

void i3MFCButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CMFCButton::OnLButtonUp(nFlags, point);

	m_callback->OnLButtonUp(this);

}
