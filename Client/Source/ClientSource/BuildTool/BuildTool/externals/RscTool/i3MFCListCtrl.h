#pragma once

class i3MFCListCtrl;

struct i3MFCListCtrlCallback
{
	virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) {}		// 일단 싱글을 염두에 두고..

	virtual void OnDragEnd(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) {}
	virtual void OnDragMove(i3MFCListCtrl* Ctrl, const i3::vector<int>& DragIndexList, CWnd* targetWnd, const CPoint& ptScreen) {}

	virtual void OnClick(i3MFCListCtrl* Ctrl, int index, int subItem) {}
	virtual void OnSelected(i3MFCListCtrl* Ctrl, int index) {}
	virtual void OnDeselected(i3MFCListCtrl* Ctrl, int index) {}
	virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) {}

	virtual void OnEndLabelEdit(i3MFCListCtrl* Ctrl, int index, 
		int sub_index, const wchar_t* strText, bool& apply) {}
	
	virtual ~i3MFCListCtrlCallback() {}
};
   

// CListCtrl::DeleteAllItems란 함수가 흔히 쓰이는데..
// 아이템이 많으면 이게 고약하게 느리다...
// (쓸데없이 느려서...이걸 안쓰는게 좋겠다..)
// 일단 RemoveAllItem정도의 함수를 둬서 다른 구현을 넣어본다..
//

class i3MFCListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(i3MFCListCtrl)
public:

	i3MFCListCtrl();
	virtual ~i3MFCListCtrl();
	
	using CMFCListCtrl::Create;			// 임시로 일단 기본클래스 오버로딩 허용.

	BOOL	Create(CWnd* pParent, UINT nID);
	BOOL	CreateEx(CWnd* pParent, UINT nID, DWORD dwStyle, DWORD dwExStyle = 0);

	void	HideHeader();

	void	SetSize( int cx, int cy);

	void	AddItem( const wchar_t* strName, void* Data );

	void					SetCallback(i3MFCListCtrlCallback* cb ) { m_callback = cb; }
	i3MFCListCtrlCallback*	GetCallback() const { return m_callback; }
	
	void	SetDropTarget(int idx);
	int		GetDropTarget() const;

	int		GetFirstSelectedItem() const;
	BOOL	GetAllSelectedItems(i3::vector<int>& inout) const;
	void	UnselectAll();
	void	SelectAll();

	void	RemoveAllItems();		// DeleteAllItems 대용으로 일단 구현해둔다..
	
	BOOL	IsLastItemIndexVisible() const;

private:
	static HIMAGELIST GetShellImageList_Large();
	static HIMAGELIST GetShellImageList_Small();
	
	DECLARE_MESSAGE_MAP()


	CImageList*	CreateDragImageEx(const i3::vector<int>& sel_list, POINT& out_offset);

public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDblClk(NMHDR* pNMHDR, LRESULT* pResult);
private:
	i3MFCListCtrlCallback*	m_callback;
	int						m_droptarget_index;

	CImageList*				m_pListDragImage;
	i3::vector<int>			m_DragIndexList;

public:
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

};
