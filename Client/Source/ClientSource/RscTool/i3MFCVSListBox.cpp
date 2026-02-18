#include "stdafx.h"
#include "i3MFCVSListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


UINT I3_WM_DELAYED_DESELECTED = ::RegisterWindowMessage(_T("I3_WM_DELAYED_DESELECTED"));


IMPLEMENT_DYNAMIC(i3MFCVSListBox, CVSListBox)

BEGIN_MESSAGE_MAP(i3MFCVSListBox, CVSListBox)
	ON_REGISTERED_MESSAGE(I3_WM_DELAYED_DESELECTED, OnDelayedDeselected)
END_MESSAGE_MAP()

namespace
{
	i3MFCVSListBoxCallback g_callback_null;
}

i3MFCVSListBox::i3MFCVSListBox() : m_callback(&g_callback_null), m_oldSel(-1)
{

}

void i3MFCVSListBox::OnSelectionChanged()
{
	CVSListBox::OnSelectionChanged();

	int idx = this->GetSelItem();
	
	if (idx < 0)
	{
		this->PostMessage(I3_WM_DELAYED_DESELECTED,0,0);
	}
	else
	{
		m_callback->OnSelection(this, idx, reinterpret_cast<void*>(this->GetItemData(idx ) ) );
		m_oldSel = idx;
	}
	
}

LRESULT	i3MFCVSListBox::OnDelayedDeselected(WPARAM, LPARAM)
{
	int idx = this->GetSelItem();
	
	if (idx < 0)
	{
		assert( m_oldSel >= 0 );
		m_callback->OnDeselection(this, m_oldSel, reinterpret_cast<void*>(this->GetItemData(m_oldSel) ) );
	}
	
	return 0;
}


void i3MFCVSListBox::OnAfterAddItem(int iItem)
{
	m_callback->OnAdd(this, iItem);
}

void i3MFCVSListBox::OnAfterRenameItem(int iItem)
{
	m_callback->OnRename(this, iItem);
}

BOOL i3MFCVSListBox::OnBeforeRemoveItem(int iItem)
{
	return m_callback->BeforeRemove(this, iItem);
}


void i3MFCVSListBox::SetItemText(int iIndex, const CString& strText)
{
	CVSListBox::SetItemText(iIndex, strText);
}

void i3MFCVSListBox::OnAfterMoveItemUp(int iItem)
{
	m_callback->OnMoveItemUp(this, iItem);
}

void i3MFCVSListBox::OnAfterMoveItemDown(int iItem) 
{
	m_callback->OnMoveItemDown(this, iItem);
}

void i3MFCVSListBox::ResetSelect()
{
	int idx = this->GetSelItem();

	if (idx < 0) return;

	m_pWndList->SetItemState(idx, 0, LVIS_SELECTED | LVIS_FOCUSED);
		
}

BOOL i3MFCVSListBox::SelectItem(int iItem)
{
	if (iItem < 0 )
	{
		int idx = this->GetSelItem();
		m_pWndList->SetItemState(idx, 0, LVIS_SELECTED | LVIS_FOCUSED);
		return TRUE;
	}

	return CVSListBox::SelectItem(iItem);	
}
