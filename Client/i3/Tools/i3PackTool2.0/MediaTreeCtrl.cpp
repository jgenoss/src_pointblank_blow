#include "stdafx.h"
#include "MediaTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMediaTreeCtrl::CMediaTreeCtrl()
{

}

CMediaTreeCtrl::~CMediaTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMediaTreeCtrl, CDragDropTreeCtrl)
	//{{AFX_MSG_MAP(CDragDropTreeCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CMediaTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}