#pragma once

#include "i3MFCListCtrl.h"
// Category2ndView 뷰입니다.
class ItemView;
enum eItemInfo;

class Category2ndView : public CView
{
	DECLARE_DYNCREATE(Category2ndView)

protected:
	Category2ndView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~Category2ndView();

public:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	ItemView*		GetItemView() const { return m_ItemView; }
	void			SetItemView(ItemView* view) { m_ItemView = view; }

	void			UpdateListCtrl(eItemInfo e);
	void			DeleteAllItems();

private:
	i3MFCListCtrl	m_listCtrl;
	
	ItemView*		m_ItemView;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


