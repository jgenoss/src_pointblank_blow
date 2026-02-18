#include "stdafx.h"
#include "QATreeCtrl.h"
#include "PTTreeworkQA.h"
#include "QATreeDroptarget.h"
#include "PTTreeworkMediator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CQATreeCtrl::CQATreeCtrl()
{

}

CQATreeCtrl::~CQATreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CQATreeCtrl, CDragDropTreeCtrl)
	//{{AFX_MSG_MAP(CDragDropTreeCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CQATreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_CTreeDropTarget = new CQATreeDropTarget;
	m_CTreeDropTarget->Register(this);

	return 0;
}

void CQATreeCtrl::OnNMClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	TheMediator().GetTreeworkQA()->LClicked(pNMHDR);

	*pResult = 0;
}