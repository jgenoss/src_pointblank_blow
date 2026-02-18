#pragma once


// ItemView 뷰입니다.
class Category1stView;
class Category2ndView;
class ItemListView;

class ItemView : public CView
{
	DECLARE_DYNCREATE(ItemView)

protected:
	ItemView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~ItemView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	
	void				UpdateAllListCtrls();
	void				DeleteAllListCtrls();

	Category2ndView*	GetCategory2ndView() const { return m_category_2nd_view; }
	ItemListView*		GetItemListView() const { return m_ItemListView; }
private:
	
	CSplitterWndEx		m_splitter;
	CSplitterWndEx		m_splitter_col0;
	
	Category1stView*	m_category_1st_view;
	Category2ndView*	m_category_2nd_view;
	ItemListView*		m_ItemListView;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


