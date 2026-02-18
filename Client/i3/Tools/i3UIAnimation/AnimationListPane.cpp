#include "stdafx.h"
#include "AnimationListPane.h"
#include "resource.h"
#include "GlobalInstance.h"
#include "MainFrm.h"


IMPLEMENT_DYNAMIC(CAnimationListPane, CPaneBase)

BEGIN_MESSAGE_MAP(CAnimationListPane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ANIMATION, &CAnimationListPane::OnNMDblclkListAnimation)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ANIMATION, &CAnimationListPane::OnNMClickListAnimation)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ANIMATION, &CAnimationListPane::OnEndLabelEdit)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_ANIMATION, &CAnimationListPane::OnNMRClickListAnimation)
END_MESSAGE_MAP()

CAnimationListPane::CAnimationListPane()
{
}


CAnimationListPane::~CAnimationListPane()
{
}

int CAnimationListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_DlgBar.Create(this, IDD_ANIMATIONLIST, WS_CHILD | WS_VISIBLE, IDD_ANIMATIONLIST);

	CListCtrl* pList = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);

	CRect rc;
	pList->GetClientRect(&rc);

	LVCOLUMN col;
	memset(&col, 0, sizeof(LVCOLUMN));
	col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	col.fmt = LVCFMT_LEFT;
	col.cx = rc.right - rc.left - 2;
	col.pszText = _T("...");
	pList->InsertColumn(0, &col);
	pList->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	pList->GetHeaderCtrl()->EnableWindow(FALSE);		// 헤더버튼 눌림 막기..

	AdjustLayout();

	return 0;
}


void CAnimationListPane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}

void CAnimationListPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_DlgBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	CWnd* pWindow = m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);
	if (pWindow)
		pWindow->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CAnimationListPane::OnNMDblclkListAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem == -1)
	{
		return;
	}

	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CListCtrl* pControl = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);
	i3::string itemName = pControl->GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

	i3UIAnimation2DSprite* pAnimation = CGlobalInstance::getSingletonPtr()->GetAnimation(itemName);

	if (pAnimation)
	{
		pMainWnd->GetAnimationViewPane()->CopyToAni(pAnimation);
		pMainWnd->GetTimeLinePane()->AddSequence(pAnimation);
	}
	*pResult = 0;
}

void CAnimationListPane::OnNMClickListAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CGlobalInstance::getSingletonPtr()->RemoveUVItems();

	if (pNMItemActivate->iItem == -1)
	{
		return;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListCtrl* pControl = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);
	i3::string itemName = pControl->GetItemText(pNMItemActivate->iItem, pNMItemActivate->iSubItem);

	i3UIAnimation2DSprite* pAnimation = CGlobalInstance::getSingletonPtr()->GetAnimation(itemName);

	i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager(i3Texture::static_meta());

	//UVItem 리스트 등록
	if (pTexManager && pAnimation)
	{
		i3Texture* tex = NULL;
		for (INT32 i = 0; i < pAnimation->GetSpriteCount(); i++)
		{
			tex = (i3Texture *)pTexManager->GetResource(pAnimation->GetUVRes(i).c_str(),
				i3ResourceFile::GetWorkingFolder(),
				pAnimation->GetUVRes(i).c_str());

			CGlobalInstance::getSingletonPtr()->AddUVItem(tex, pAnimation->GetUVPos(i));
		}

		CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();
		i3Texture* pTexture = pMainWnd->GetTextureView()->GetImage();

		if (pTexture)
			pMainWnd->GetTextureView()->DisplayTex(pTexture->GetName());
	}
	*pResult = 0;
}

void CAnimationListPane::OnEndLabelEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (pDispInfo->item.pszText)
	{
		CListCtrl* pControl = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);

		i3::string itemName = pControl->GetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem);
		i3UIAnimation2DSprite* pAni = CGlobalInstance::getSingletonPtr()->GetAnimation(itemName);
		REAL32 fLifeTime = pAni->GetLifeTime();
		CGlobalInstance::getSingletonPtr()->RemoveAnimation(itemName);
		CGlobalInstance::getSingletonPtr()->AddAnimationItem(pDispInfo->item.pszText, fLifeTime);
	}

	*pResult = 0;
}

void CAnimationListPane::Update()
{
	CListCtrl* pControl = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);
	pControl->DeleteAllItems();

	i3UIAnimationMgr* pAnimationMgr = CGlobalInstance::getSingletonPtr()->GetUIAnimationMgr();
	for (INT32 i = 0; i < pAnimationMgr->GetAnimationCount(); i++)
	{
		pControl->InsertItem(i, pAnimationMgr->GetAnimation(i)->GetName());
	}
}

void CAnimationListPane::OnRemove(INT32 nIndex)
{
	CListCtrl* pControl = (CListCtrl*)m_DlgBar.GetDlgItem(IDC_LIST_ANIMATION);

	i3::string itemName = pControl->GetItemText(nIndex, 0);
	CGlobalInstance::getSingletonPtr()->RemoveAnimation(itemName);
	pControl->DeleteItem(nIndex);
}

void CAnimationListPane::OnNMRClickListAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem == -1)return;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMenu menu, *pPopup;
	CPoint pt;
	UINT32 cmd;

	SetFocus();
	GetCursorPos(&pt);

	menu.LoadMenu(IDR_ANIMATION);
	pPopup = menu.GetSubMenu(0);

	cmd = pPopup->TrackPopupMenuEx(TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd *) this, NULL);

	switch (cmd)
	{
		case ID_ANIMATION_REMOVE:
			{
				OnRemove(pNMItemActivate->iItem);
			}
			break;
	}

	*pResult = 0;
}
