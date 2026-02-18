// OptimizeWnd.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "OptimizeWnd.h"
#include ".\optimizewnd.h"


// COptimizeWnd dialog

IMPLEMENT_DYNAMIC(COptimizeWnd, CDialog)
COptimizeWnd::COptimizeWnd(CWnd* pParent /*=NULL*/)
	: CDialog(COptimizeWnd::IDD, pParent)
{
}

COptimizeWnd::~COptimizeWnd()
{
}

void COptimizeWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(COptimizeWnd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// COptimizeWnd message handlers

void COptimizeWnd::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	OnOK();
}

void COptimizeWnd::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
//	OnCancel();
}
