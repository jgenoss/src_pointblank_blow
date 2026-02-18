#pragma once

//
// Win7 스타일로 변경하기 위해 매니페스트 관련 변경 pragma를 심었다. (cpp참고)
//
class i3MFCTreeCtrl;

struct i3MFCTreeCtrlCallback
{
	virtual void OnSelChanged(i3MFCTreeCtrl* Ctrl, HTREEITEM hItem, void* ItemData) {}
	
	// 일단 싱글에 대해서만 처리..
	virtual void OnDragEnd(i3MFCTreeCtrl* thisCtrl, HTREEITEM hItem, CWnd* targetWnd, const CPoint& ptScreen) {}
	virtual void OnDragMove(i3MFCTreeCtrl* thisCtrl, HTREEITEM hItem, CWnd* targetWnd, const CPoint& ptScreen ) {}
	
	virtual void OnContextMenu(i3MFCTreeCtrl* thisCtrl, const CPoint& pt) {}

	virtual void OnEndLabelEdit(i3MFCTreeCtrl* thisCtrl, HTREEITEM hItem, const wchar_t* pszText, bool& apply) {}

	virtual ~i3MFCTreeCtrlCallback() {}
};

struct i3MFCTreeCtrl_FindCallback
{
	virtual void OnTraverse( i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) {}

	virtual void OnNewItem( i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) {}
	virtual void OnAlreadyItemCreated(i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) {}

	virtual void OnRenameItem(i3MFCTreeCtrl* ctrl, const i3::wstring& old_name, const i3::wstring& new_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel) {}
	virtual void OnAlreadyItemRenamed(i3MFCTreeCtrl* ctrl, const i3::wstring& old_name, const i3::wstring& new_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel) {}

	virtual void BeforeDeleteItem(i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, HTREEITEM hItem, int iLevel ) {}
	virtual void OnNotFoundItemToDelete( i3MFCTreeCtrl* ctrl, const i3::wstring& item_name, HTREEITEM hParent, int iLevel) {}

	virtual ~i3MFCTreeCtrl_FindCallback() {}
};


class i3MFCTreeCtrl : public CTreeCtrl
{
	DECLARE_DYNAMIC(i3MFCTreeCtrl)

// Construction
public:
	i3MFCTreeCtrl();	
	virtual ~i3MFCTreeCtrl();
	
	BOOL	CreateEx(CWnd* pParent, UINT nID, DWORD dwAddStyle = 0);
	void	SetSize( int cx, int cy);
	void	SetRect( const CRect& rc);

	HTREEITEM	AddRootItem(const wchar_t* szItemName, void* userData);
	HTREEITEM	AddItem(HTREEITEM parentItem, const wchar_t* szItemName, void* userData);
	BOOL		DeleteItem(HTREEITEM hItem);				// 기본클래스 함수와 이름이 같은데 비가상이므로 가려지게 된다..

	HTREEITEM	FindSiblingItemByName( HTREEITEM hStart, const wchar_t* szItemName);

	HTREEITEM	FindOrCreateItemByNameList( const i3::vector<i3::wstring>& nameList, i3MFCTreeCtrl_FindCallback* cb);
	HTREEITEM   FindOrRenameItemByNameList( const i3::vector<i3::wstring>& nameList, const i3::wstring& new_name, i3MFCTreeCtrl_FindCallback* cb);
	BOOL		DeleteItemByNameList( const i3::vector<i3::wstring>& nameList, i3MFCTreeCtrl_FindCallback* cb, bool ForceCreateItem = false);
			
	HTREEITEM	FindOrCreateItemByNameList( const i3::wstring& path, i3MFCTreeCtrl_FindCallback* cb);
	HTREEITEM   FindOrRenameItemByNameList( const i3::wstring& path, const i3::wstring& new_name, i3MFCTreeCtrl_FindCallback* cb);
	BOOL		DeleteItemByNameList( const i3::wstring& path, i3MFCTreeCtrl_FindCallback* cb, bool ForceCreateItem = false);

	
	void					SetCallback(i3MFCTreeCtrlCallback* cb ) { m_callback = cb; }
	i3MFCTreeCtrlCallback*	GetCallback() const { return m_callback; }





private:

	
	CImageList*	CreateDragImageEx(HTREEITEM hitem);




	DECLARE_MESSAGE_MAP()

private:	
	i3MFCTreeCtrlCallback*		m_callback;
	CImageList*	m_pTreeDragImage;	// 드래그앤드롭이미지..
	HTREEITEM	m_hDragItem;		// 드래그앤드롭해당 아이템..
protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

};

