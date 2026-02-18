// DebugPanel.cpp : implementation file
//

#include "stdafx.h"

#if defined( I3_DEBUG)
#include "DebugPanel.h"
#include "DlgVRAMState.h"
#include "DlgRAMState.h"
#include "DlgGameGraph.h"

// CDebugPanel dialog

IMPLEMENT_DYNAMIC(CDebugPanel, CDialog)

CDebugPanel::CDebugPanel(CWnd* pParent /*=NULL*/)
	: CDialog(CDebugPanel::IDD, pParent)
{

}

CDebugPanel::~CDebugPanel()
{
}

void CDebugPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDebugPanel, CDialog)
	ON_BN_CLICKED(IDC_BTN_VRAM, &CDebugPanel::OnBnClickedBtnVram)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_RAM, &CDebugPanel::OnBnClickedBtnRam)
	ON_BN_CLICKED(IDC_BTN_GAMEGRAPH, &CDebugPanel::OnBnClickedBtnGamegraph)
END_MESSAGE_MAP()


// CDebugPanel message handlers
void CDebugPanel::OnClose()
{
	ShowWindow( SW_HIDE);
}

void CDebugPanel::OnBnClickedBtnVram()
{
	CDlgVRAMState dlg;

	dlg.DoModal();
}

void CDebugPanel::OnBnClickedBtnRam()
{
	CDlgRAMState dlg;

	dlg.DoModal();
}

void CDebugPanel::OnBnClickedBtnGamegraph()
{
	CDlgGameGraph dlg;

	dlg.DoModal();
}

#endif


