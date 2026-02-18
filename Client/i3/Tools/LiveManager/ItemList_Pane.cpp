#include "StdAfx.h"
#include "ItemList_Pane.h"
#include "resource.h"
#include "MainFrm.h"
#include "LiveManager.h"
#include "GlobalInstance.h"

IMPLEMENT_DYNAMIC(CItemList_Pane, CDockablePane)


BEGIN_MESSAGE_MAP(CItemList_Pane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


CItemList_Pane::CItemList_Pane(void)
{
	m_vecWeaonList.clear();
	m_vecEquipList.clear();
}

CItemList_Pane::~CItemList_Pane(void)
{
}

void CItemList_Pane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
}

int CItemList_Pane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_TOOLTIPS;
	
	m_DlgBar.Create( this, IDD_ITEMLIST,  WS_CHILD | WS_VISIBLE, IDD_ITEMLIST);

	AdjustLayout();
	return 0;
}

void CItemList_Pane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//백그라운드
	m_DlgBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	//무기
	CButton* pGroupBox = (CButton*)m_DlgBar.GetDlgItem(IDC_STATIC_WP);
	pGroupBox->SetWindowPos(NULL, rectClient.left + 10, rectClient.top+10, rectClient.Width() - 20, 230, SWP_NOACTIVATE | SWP_NOZORDER);

	CListBox* listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_WEAPONLIST);
	listbox->SetWindowPos(NULL, rectClient.left + 25, rectClient.top+30, rectClient.Width() - 50, 200, SWP_NOACTIVATE | SWP_NOZORDER);

	int nBottom = 230 + 10 + 20;

	//장비
	pGroupBox = (CButton*)m_DlgBar.GetDlgItem(IDC_STATIC_EQUIP);
	pGroupBox->SetWindowPos(NULL, rectClient.left + 10, nBottom, rectClient.Width() - 20, 230, SWP_NOACTIVATE | SWP_NOZORDER);

	listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_EQUIPLIST);
	listbox->SetWindowPos(NULL, rectClient.left + 25, nBottom+20, rectClient.Width() - 50, 200, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CItemList_Pane::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
}

bool CItemList_Pane::AddWeapon(ITEMSUBINFO subinfo)
{
	if( subinfo.sItemName.GetLength() <= 0 )
	{
		MessageBox(L"무기이름을 입력해주세요", L"경고", MB_OK);
		return false;
	}

	if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_WEAPON_PANE )
	{
		CListBox* listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_WEAPONLIST);
		listbox->AddString(subinfo.sItemName);

		m_vecWeaonList.push_back(subinfo);
	}
	else if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_EQUIP_PANE )
	{
		CListBox* listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_EQUIPLIST);
		listbox->AddString(subinfo.sItemName);

		m_vecEquipList.push_back(subinfo);
	}

	return true;
}

void CItemList_Pane::DeleteWeapon()
{
	CListBox* listbox = NULL;
	if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_WEAPON_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_WEAPONLIST);

		int ncursel = listbox->GetCurSel();

		if( ncursel != -1 )
		{
			m_vecWeaonList.erase(m_vecWeaonList.begin()+ncursel);
			listbox->DeleteString(ncursel);

			if( listbox->GetCount() > ncursel )
				listbox->SetCurSel(ncursel);
			else
				listbox->SetCurSel(ncursel-1);
		}
	}
	else if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_EQUIP_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_EQUIPLIST);

		int ncursel = listbox->GetCurSel();

		if( ncursel != -1 )
		{
			m_vecEquipList.erase(m_vecEquipList.begin()+ncursel);
			listbox->DeleteString(ncursel);

			if( listbox->GetCount() > ncursel )
				listbox->SetCurSel(ncursel);
			else
				listbox->SetCurSel(ncursel-1);
		}
	}
	
}

ITEMSUBINFO* CItemList_Pane::GetSelectItem()
{
	CListBox* listbox = NULL;
	if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_WEAPON_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_WEAPONLIST);
		int ncursel = listbox->GetCurSel();

		if( ncursel != -1 )
		{
			return &m_vecWeaonList[ncursel];
		}
	}
	else if( CGlobalInstance::getSingleton().GetCurFocusWindow() == ID_EQUIP_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_EQUIPLIST);
		int ncursel = listbox->GetCurSel();

		if( ncursel != -1 )
		{
			return &m_vecEquipList[ncursel];
		}
	}

	return NULL;
}

INT32 CItemList_Pane::GetWeaponListSize()
{
	return m_vecWeaonList.size();
}

INT32 CItemList_Pane::GetEquipListSize()
{
	return m_vecEquipList.size();
}

ITEMSUBINFO CItemList_Pane::GetItemListInfo(INT32 Index, INT32 nID)
{ 
	if( nID == ID_WEAPON_PANE )
		return m_vecWeaonList[Index]; 
	else
		return m_vecEquipList[Index]; 
}

void CItemList_Pane::ClearSubinfo(int nID)
{ 
	CListBox* listbox = NULL;
	if( nID == ID_WEAPON_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_WEAPONLIST);
		m_vecWeaonList.clear(); 
		listbox->ResetContent();
	}
	else if( nID == ID_EQUIP_PANE )
	{
		listbox = (CListBox*)m_DlgBar.GetDlgItem(IDC_EQUIPLIST);
		m_vecEquipList.clear();
		listbox->ResetContent();
	}
}