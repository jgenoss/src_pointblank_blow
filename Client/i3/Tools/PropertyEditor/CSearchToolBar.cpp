
#include "stdafx.h"
#include "Resource.h"
#include "CSearchToolBar.h"

#include <algorithm>

//
// CSearchComboBox
//

IMPLEMENT_DYNAMIC(CSearchComboBox, CComboBoxEx)

BEGIN_MESSAGE_MAP(CSearchComboBox, CComboBoxEx)
END_MESSAGE_MAP()

CSearchComboBox::CSearchComboBox() 
{
	m_resID = 0;
	m_sortCandidate = true;
	m_numMaxDropDownListItems = 20;
	m_callback = NULL;
	m_searchPrefixOnly = false;
}

CSearchComboBox::~CSearchComboBox()
{
}

bool CSearchComboBox::IsShowDropDownList()
{
	return GetComboBoxCtrl()->SendMessage(CB_GETDROPPEDSTATE, 0, 0) ? true : false;
}

void CSearchComboBox::DoWithSearchText()
{
	CString strSearchText;
	GetEditCtrl()->GetWindowText(strSearchText);

	if(strSearchText.IsEmpty())
		return;

	if(m_callback)
	{
		m_callback->CSearchComboBoxCallBack_SelectClosestItem(
			strSearchText.GetString(), GetSearchOptFlag()); 
	}
}

bool CSearchComboBox::OpenDropDownList()
{
	RefreshSearchTextCandidate();
	ShowDropDown(TRUE);

	// prevent being hidden after calling ccombobox::showdropdown
	SetCursor(LoadCursor(NULL, IDC_ARROW));

	return true;
}

BOOL CSearchComboBox::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
    {
		switch(pMsg->wParam)
		{
		case VK_RETURN:
		{
			if(IsShowDropDownList())
			{
				WPARAM wParam = MAKELONG(m_resID, ECUST_NOTIFY_RETURN_KEY);
				PostMessage(WM_COMMAND, wParam, 0);
			}
			else
			{
				if(OpenDropDownList())
					return TRUE;
			}				

			break;
		}

		case VK_DOWN:
		{
			if(!IsShowDropDownList())
			{
				if(OpenDropDownList())
					return TRUE;
			}				
		}

		default:
			break;

		}

		if (::GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			if (pMsg->wParam == 'C')
			{
				Copy();
			}
			else if (pMsg->wParam == 'V')
			{
				Paste();
			}
			else if (pMsg->wParam == 'X')
			{
				Cut();
			}
		}
    }

    return CComboBox::PreTranslateMessage(pMsg);
}

void CSearchComboBox::AddStringAsItem(const char* text, void* arg)
{
	COMBOBOXEXITEM item;
	INT lastItemCount = GetCount();

	item.mask = CBEIF_TEXT | CBEIF_LPARAM;
	item.iItem = lastItemCount;
	item.pszText = (LPTSTR)text;
	item.lParam = (LPARAM)arg;

	InsertItem(&item);
}

void CSearchComboBox::ClearCandidateItems()
{
	CString strEditBox;
	GetEditCtrl()->GetWindowText(strEditBox);

	INT32 num = GetCount();

	for(INT32 i=0; i<num; i++)
		DeleteItem(0);

	GetEditCtrl()->SetWindowText(strEditBox);
}

void CSearchComboBox::FitDropRectToItems()
{	
	CRect area;
	CRect dropArea;

	GetClientRect(&area);
	GetDroppedControlRect(&dropArea);

	INT itemHeight = GetItemHeight(-1);
	INT itemCount = GetCount();

	if(itemCount > m_numMaxDropDownListItems)
		itemCount = 20;

	GetParent()->ScreenToClient(&dropArea);
	dropArea.bottom = dropArea.top + area.Height() + itemHeight * itemCount + 5;

	// CSearchComboBox를 Resize하면 안됨
	CComboBox* cb = GetComboBoxCtrl();
	cb->MoveWindow(&dropArea);
}

void CSearchComboBox::OnComboEditTextChange()
{
	RefreshSearchTextCandidate();
}

void CSearchComboBox::OnSelectItem()
{
	int curSel = GetCurSel();

	if(curSel == -1)
		return;

	COMBOBOXEXITEM item;

	item.mask = CBEIF_LPARAM;
	item.iItem = curSel;
		
	if(GetItem(&item))
	{
		if(m_callback)
			m_callback->CSearchComboBoxCallBack_OnSelectItem((void*)item.lParam);
	}
}

void CSearchComboBox::RefreshSearchTextCandidate(bool atOnce)
{
	_RefreshCandidate();
}

void CSearchComboBox::_RefreshCandidate()
{
	CString strSearchText;
	GetEditCtrl()->GetWindowText(strSearchText);

	if(strSearchText.IsEmpty())
		return;

	ClearCandidateItems();	

	if(!m_callback)
		return;
	
	CSearchComboBoxCallBack::CandidateRst rst;

	m_callback->CSearchComboBoxCallBack_CollectCandidateByText(
		rst, (const char*)strSearchText.GetString(), GetSearchOptFlag());

	std::sort(rst.begin(), rst.end(), CSearchComboBoxCallBack::CandidateSorter());

	for(INT32 i=0; i<(INT32)rst.size(); i++)
		AddStringAsItem(rst[i].m_name.c_str(), rst[i].m_arg);

	FitDropRectToItems();
}

BOOL CSearchComboBox::OnChildNotify(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT* rst)
{
	WORD ctlID = LOWORD(wparam);

	if(m_resID == ctlID)
	{
		if(msg == WM_COMMAND)
		{
			WORD ctlNotify = HIWORD(wparam);

			switch(ctlNotify)
			{
			case ECUST_NOTIFY_RETURN_KEY:
				{
					if(IsShowDropDownList())
					{
						OnSelectItem();
						ShowDropDown(FALSE);
					}
					else
					{
						DoWithSearchText();
					}

					return TRUE; // no more process
				}
				break;

			case CBN_SELCHANGE   : 
				break;

			case CBN_DBLCLK      : break;
			case CBN_SETFOCUS    : break;
			case CBN_KILLFOCUS   : break;
			case CBN_EDITUPDATE  :
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
				break;

			case CBN_DROPDOWN    : 
				{
					RefreshSearchTextCandidate();
					return TRUE;
				}
				break;

			case CBN_CLOSEUP     : break;
			case CBN_SELENDCANCEL: break;
			case CBN_EDITCHANGE  :
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
				break;

			case CBN_SELENDOK    : 
				{ 
					OnSelectItem();
					return TRUE; // no more process
				} 
				break;

			default: 
				break;
			}
		}
	}

	return CComboBox::OnChildNotify(msg, wparam, lparam , rst);
}

DWORD CSearchComboBox::GetSearchOptFlag()
{
	DWORD opt = 0;

	if(m_searchPrefixOnly)
		opt |= CSearchComboBoxCallBack::EOPT_SEARCH_PREFIX_ONLY;

	return opt;
}


//
// CSearchToolBar
//

IMPLEMENT_DYNAMIC(CSearchToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CSearchToolBar, CToolBar)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI_RANGE(ID_TB_SEARCH_CB_SEARCH_LIST, ID_TB_SEARCH_CK_SEARCH_PREFIX_ONLY, 
		&CSearchToolBar::UpdateUIToolBarBtn)
	ON_COMMAND(ID_TB_SEARCH_CK_SEARCH_PREFIX_ONLY, OnSearchPrefixOnlyCkBtn)
	
END_MESSAGE_MAP()


CSearchToolBar::CSearchToolBar()
{
	m_isAdditionalCreated = FALSE;

	m_searchComboBoxWidth = 300;
	m_searchPrefixOnlyCkBtnWidth = 100;
	m_PEFBtnWidth = 80;
	
	m_searchComboStyle = 0;
	m_callbackSearchCombo = NULL;

	m_searchComboPlaceIndexInToolBar = -1;
}

CSearchToolBar::~CSearchToolBar()
{
}

BOOL CSearchToolBar::Create(CWnd* pParentWnd,DWORD dwStyle, UINT nID)
{
	BOOL rst = CToolBar::Create(pParentWnd, dwStyle, nID);
	return rst;
}

BOOL CSearchToolBar::CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle,
	DWORD dwStyle, CRect rcBorders,UINT nID)
{
	BOOL rst = CToolBar::CreateEx(pParentWnd, dwCtrlStyle, dwStyle, rcBorders, nID);
	return rst;
}

void CSearchToolBar::GetItemRect(int nIndex, LPRECT lpRect) const
{
	return CToolBar::GetItemRect(nIndex, lpRect);
}

CSize CSearchToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	return CToolBar::CalcFixedLayout(bStretch, bHorz);
}

CSize CSearchToolBar::CalcDynamicLayout(int nLength, DWORD nMode)
{
	return CToolBar::CalcDynamicLayout(nLength, nMode);
}

void CSearchToolBar::CreateSearchCombo(INT placeIndexInToolBar, INT resID, 
	const CSize siz)
{
	m_searchComboPlaceIndexInToolBar = placeIndexInToolBar;
	CRect rectSearchCombo = CRect(-siz.cx,-siz.cy,0,0);

	m_searchComboStyle = WS_BORDER | WS_CHILD | CBS_DROPDOWN ;

	m_searchCombo.SetResID(resID);
	m_searchCombo.Create(m_searchComboStyle, rectSearchCombo, this, resID);

	m_rectSearchPrefixOnlyCkBtn = CRect(0,0, m_searchPrefixOnlyCkBtnWidth, 20);
	m_searchPrefixOnlyCkBtn.Create("Prefix Only", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_TEXT,
		m_rectSearchPrefixOnlyCkBtn, this, ID_TB_SEARCH_CK_SEARCH_PREFIX_ONLY);

	m_PEFBtnRect = CRect(0, 0, m_PEFBtnWidth, 20);
	m_PEFBtn.Create("Pef Save", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | BS_TEXT,
		m_PEFBtnRect, this, ID_PEFSAVE);
	
	m_searchCombo.ShowWindow(SW_SHOW);
}

BOOL CSearchToolBar::CreateAdditional(INT comboResID, INT comboWidth)
{
	if(m_isAdditionalCreated)
		return TRUE;

	m_searchComboBoxWidth = comboWidth;
	CreateSearchCombo(0, comboResID, CSize(m_searchComboBoxWidth, 32));
	m_isAdditionalCreated = TRUE;

	return TRUE;
}

void CSearchToolBar::SetCallback(CSearchComboBoxCallBack* callback) 
{ 
	m_callbackSearchCombo = callback; 
	m_searchCombo.SetCallback(callback);
}


//
// afx_msg
//
void CSearchToolBar::UpdateUIToolBarBtn(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CSearchToolBar::OnSize(UINT nType, int cx, int cy)
{
	CToolBar::OnSize(nType, cx, cy);

	if(::IsWindow(m_searchCombo.GetSafeHwnd()))
	{
		// TBBS_SEPARATOR일경우 , iImage항목은 이미지인덱스가 아닌 width값이 됩니다.
		GetItemRect(0, &m_rectSearchCombo);
			// 0
		SetButtonInfo(0, m_searchCombo.GetResID(), TBBS_SEPARATOR, m_searchComboBoxWidth);	
		
		m_searchCombo.SetWindowPos(NULL, m_rectSearchCombo.left, m_rectSearchCombo.top, 0, 0, 
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS);
 
 		GetItemRect(1, &m_rectSearchPrefixOnlyCkBtn);
 			// 2
		SetButtonInfo(1, ID_TB_SEARCH_CK_SEARCH_PREFIX_ONLY, TBBS_SEPARATOR,
			m_searchPrefixOnlyCkBtnWidth);

		m_searchPrefixOnlyCkBtn.SetWindowPos(NULL, m_rectSearchPrefixOnlyCkBtn.left,
			m_rectSearchPrefixOnlyCkBtn.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS);

		GetItemRect(3, &m_PEFBtnRect);

		SetButtonInfo(3, ID_PEFSAVE, TBBS_SEPARATOR, m_PEFBtnWidth);

		m_PEFBtn.SetWindowPos(NULL, m_PEFBtnRect.left,
			m_PEFBtnRect.top, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS);
	}
}

void CSearchToolBar::OnSearchPrefixOnlyCkBtn()
{
	m_searchCombo.m_searchPrefixOnly = m_searchPrefixOnlyCkBtn.GetCheck() ? true : false;
}

bool CSearchToolBar::IsPefSave()
{
	return m_PEFBtn.GetCheck() ? true : false;
}