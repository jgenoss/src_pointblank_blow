#pragma once

class i3MFCVSListBox;

// MFC리스트박스 셀렉션 시에 대부분 Deselect -> Select의 과정을 거치므로 가상함수도 2개로 나뉘게 되었다.
struct i3MFCVSListBoxCallback
{
	virtual void OnSelection(i3MFCVSListBox* Ctrl, INT32 idx, void* ItemData) {}
	virtual void OnDeselection(i3MFCVSListBox* Ctrl, INT32 old_idx, void* old_ItemData) {}

	virtual void OnAdd(i3MFCVSListBox* Ctrl, int iItem) {}
	virtual void OnRename(i3MFCVSListBox* Ctrl, int iItem) {}
	virtual bool BeforeRemove(i3MFCVSListBox* Ctrl, int iItem) { return true; }

	virtual void OnMoveItemUp(i3MFCVSListBox* Ctrl, int iItem) {}
	virtual void OnMoveItemDown(i3MFCVSListBox* Ctrl, int iItem) {}

	virtual ~i3MFCVSListBoxCallback() {}
};

class i3MFCVSListBox : public CVSListBox
{
	DECLARE_DYNAMIC(i3MFCVSListBox)
public:
	i3MFCVSListBox();
	
	
	void	SetCallback(i3MFCVSListBoxCallback* cb) { m_callback = cb; }
	i3MFCVSListBoxCallback*	GetCallback() const { return m_callback; }
		
	
	virtual void SetItemText(int iIndex, const CString& strText);
	void		 ResetSelect();
	virtual BOOL SelectItem(int iItem);
private:
	virtual void OnSelectionChanged();
	virtual void OnAfterAddItem(int iItem);
	virtual void OnAfterRenameItem(int iItem);
	virtual BOOL OnBeforeRemoveItem(int iItem); 

	virtual void OnAfterMoveItemUp(int iItem);
	virtual void OnAfterMoveItemDown(int iItem);


	i3MFCVSListBoxCallback*		m_callback;
	INT32						m_oldSel;

	afx_msg LRESULT	OnDelayedDeselected(WPARAM, LPARAM);

protected:

	DECLARE_MESSAGE_MAP()
	
};

