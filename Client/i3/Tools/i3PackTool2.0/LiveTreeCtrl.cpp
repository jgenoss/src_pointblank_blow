#include "stdafx.h"
#include "resource.h"
#include "LiveTreeCtrl.h"
#include "PTTreeworkLive.h"
#include "LiveTreeDroptarget.h"
#include "PTTreeworkMediator.h"
#include "PTFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLiveTreeCtrl::CLiveTreeCtrl()
{
}

CLiveTreeCtrl::~CLiveTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CLiveTreeCtrl, CDragDropTreeCtrl)
	//{{AFX_MSG_MAP(CDragDropTreeCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CLiveTreeCtrl::OnSelchanged)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CLiveTreeCtrl::OnRClicked)
	ON_COMMAND(ID_PACKING, &CLiveTreeCtrl::OnPacking)
	ON_COMMAND(ID_DELETE, &CLiveTreeCtrl::OnDelete)
	ON_COMMAND(ID_EXPORT, &CLiveTreeCtrl::OnExport)
	ON_COMMAND(ID_NEW_PACK, &CLiveTreeCtrl::OnNewPack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CLiveTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_CTreeDropTarget = new CLiveTreeDropTarget;
	m_CTreeDropTarget->Register(this);

	return 0;
}

HTREEITEM CLiveTreeCtrl::AddItem(HTREEITEM hParent, PTFile* fileSelected, HTREEITEM hInsAfter, int iSelImage, int iNonSelImage, long lParam)
{
	return TheMediator().GetTreeworkLive()->AddItem(hParent, fileSelected, hInsAfter, iSelImage, iNonSelImage, lParam);
}

void CLiveTreeCtrl::OnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	TheMediator().GetTreeworkLive()->Selchanged(pNMHDR);

	*pResult = 0;
}

void CLiveTreeCtrl::OnRClicked(NMHDR *pNMHDR, LRESULT *pResult)
{
	TheMediator().GetTreeworkLive()->RClicked(pNMHDR);

	*pResult = 0;
}

void CLiveTreeCtrl::OnPacking()
{
	TheMediator().GetTreeworkLive()->RePackMenuClicked();
}

void CLiveTreeCtrl::OnDelete()
{
	TheMediator().GetTreeworkLive()->DeleteMenuClicked();
}

void CLiveTreeCtrl::OnExport()
{
	TheMediator().GetTreeworkLive()->ExportMenuClicked();
}

void CLiveTreeCtrl::OnNewPack()
{
	TheMediator().GetTreeworkLive()->NewPackMenuClicked();
}