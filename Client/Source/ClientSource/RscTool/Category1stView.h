#pragma once


// Category1stView 뷰입니다.
#include "i3MFCListCtrl.h"

class ItemView;

class Category1stView : public CView
{
	DECLARE_DYNCREATE(Category1stView)

protected:
	Category1stView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~Category1stView();

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
	void			ResetSelection();

private:

	i3MFCListCtrl	m_listCtrl;		// 일단 pef 목록..
									// pef가 아닐수도 있음..
									// 일단 item_string에 의존해서 enum value 추출후 분기...
	ItemView*		m_ItemView;								
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


