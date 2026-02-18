// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "GlobalVariable.h"

#include "i3TDKImageViewCtrl_Impl.h"

BEGIN_MESSAGE_MAP(CImageViewCtrl, i3TDKImageViewCtrl)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


void CImageViewCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_RButtonDown)
	{
		//이동한양 계산
		int dX = point.x - m_DragStart.x;
		int dY = point.y - m_DragStart.y;

		//이동한양만큼 스크롤
		Scroll(SB_HORZ ,  - dX , m_posScroll.x);
		Scroll(SB_VERT ,  - dY , m_posScroll.y);

		//현재위치 저장
		m_DragStart.x = point .x;
		m_DragStart.y = point .y;
	}

	CWnd::OnMouseMove(nFlags, point);
}


void CImageViewCtrl::ResetScrollPos()
{
	i3TDKImageViewCtrl::ResetScroll(SB_HORZ , m_posScroll.x);
	i3TDKImageViewCtrl::ResetScroll(SB_VERT , m_posScroll.y);
}

void CImageViewCtrl::ResetImage()
{
	i3TDKImageViewCtrl::SetImage(NULL);
}
