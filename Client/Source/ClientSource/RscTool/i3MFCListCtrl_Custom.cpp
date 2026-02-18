#include "stdafx.h"
#include "i3MFCListCtrl_Custom.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(i3MFCListCtrl_Custom, i3MFCListCtrl)

BEGIN_MESSAGE_MAP(i3MFCListCtrl_Custom, i3MFCListCtrl)

	ON_WM_PAINT()
END_MESSAGE_MAP()

namespace 
{
	i3MFCListCtrl_CustomDrawCallback g_default_cb;
}

i3MFCListCtrl_Custom::i3MFCListCtrl_Custom() : m_callback_customdraw(&g_default_cb)
{

}

i3MFCListCtrl_Custom::~i3MFCListCtrl_Custom()
{

}

void i3MFCListCtrl_Custom::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call i3MFCListCtrl::OnPaint() for painting messages

	i3MFCListCtrl::OnPaint();

	int num_idx = this->GetItemCount();
	int first = this->GetTopIndex();
	int last =  first + this->GetCountPerPage() + 1;

	if ( last > num_idx ) 
		last = num_idx;
	
	CRect rc_first;
	if ( this->GetItemRect(first, rc_first, LVIR_BOUNDS) )
	{
		if (rc_first.top >= 0 )		// 첫줄이 상단스크롤로 잘리는 경우엔 재업데이트가 기대되며..
		{							// 깜박이 현상을 줄이기 위해서, 여기서는 콜백함수 호출을 스킵해준다.
			m_callback_customdraw->OnPaint(this, first, last);		// 반개구간..
		}
	}
	else
	{
		m_callback_customdraw->OnPaint(this, first, last);		// 
	}

}

