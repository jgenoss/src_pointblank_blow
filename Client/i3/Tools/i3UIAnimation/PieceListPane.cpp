#include "stdafx.h"
#include "resource.h"
#include "PieceListPane.h"
#include "MainFrm.h"
#include "GlobalInstance.h"
#define DEFALUT_UV_IMAGESIZE 64
#define ID_WND_IMAGELISTCTRL		1
IMPLEMENT_DYNAMIC(CPieceListPane, CPaneBase)

BEGIN_MESSAGE_MAP(CPieceListPane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED, ID_WND_IMAGELISTCTRL, &CPieceListPane::OnItemChanged)
	ON_NOTIFY(NM_DBLCLK, ID_WND_IMAGELISTCTRL, &CPieceListPane::OnNMDblclk)
END_MESSAGE_MAP()

CPieceListPane::CPieceListPane()
{
	m_pSelectItem = NULL;
}

CPieceListPane::~CPieceListPane()
{
}

int CPieceListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_UVListCtrl.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 200, 200), this, ID_WND_IMAGELISTCTRL, CPoint(DEFALUT_UV_IMAGESIZE, DEFALUT_UV_IMAGESIZE));
	m_UVListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS);
	AdjustLayout();

	return 0;
}


void CPieceListPane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CPieceListPane::AdjustLayout()
{
	CRect rectClient;
	GetClientRect(rectClient);

	//백그라운드
	m_UVListCtrl.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPieceListPane::AddItem(UVITEM* pUVItem, LVITEM* pControlItem)
{
	m_UVListCtrl.InsertItem(pUVItem, pControlItem);
}

void CPieceListPane::RemoveAllItem()
{
	m_UVListCtrl.DeleteAllItems();
}

//listctrl 갱신
void CPieceListPane::UpdateUVItem(UVITEM* pUVItem, INT32 nIndex)
{
	m_UVListCtrl.UpdateImage(pUVItem, nIndex);

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	pMainWnd->GetPropertys()->Update(pUVItem);
}

void CPieceListPane::SetFocusdItem(INT32 nIndex)
{
	AllItemNonFocuse();

	m_UVListCtrl.SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	m_UVListCtrl.SetFocus();
}

void CPieceListPane::AllItemNonFocuse()
{
	m_UVListCtrl.SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
}

void CPieceListPane::OnItemChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
	m_pSelectItem = CGlobalInstance::getSingletonPtr()->GetUVItem(pNMLV->iItem);

	if (pNMLV->uNewState == 0)
	{
		pMainWnd->GetTextureView()->SetFocusdItem(NULL);
		pMainWnd->GetPropertys()->RemoveAllProperty();
	}
	else if (m_pSelectItem)
	{
		pMainWnd->GetTextureView()->SetFocusdItem(m_pSelectItem->_pControl);
		pMainWnd->GetPropertys()->Update(m_pSelectItem);
		pMainWnd->GetAnimationViewPane()->SelectFrame(m_pSelectItem);
	}

	*pResult = 0;
}


BOOL CPieceListPane::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//if (m_GripManager.GetCurFocusShape())
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case  VK_DELETE:
			{
				CGlobalInstance::getSingletonPtr()->RemoveUVItem(m_pSelectItem);
			}
			break;
			}
		}
	}

	return CPaneBase::PreTranslateMessage(pMsg);
}


void CPieceListPane::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
	m_pSelectItem = CGlobalInstance::getSingletonPtr()->GetUVItem(pNMLV->iItem);

	if(m_pSelectItem)
		pMainWnd->GetTextureView()->DisplayTex(m_pSelectItem->_pTexture->GetName());

	*pResult = 0;
}
