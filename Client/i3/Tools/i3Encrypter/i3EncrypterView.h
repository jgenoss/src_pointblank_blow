#pragma once


class i3EncrypterView : public CEditView
{
protected:
	i3EncrypterView();
	DECLARE_DYNCREATE(i3EncrypterView)

public:
	i3EncrypterDoc* GetDocument() const;

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	virtual ~i3EncrypterView();

protected:

protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

#ifndef _DEBUG  // i3EncrypterView.cpp의 디버그 버전
inline i3EncrypterDoc* i3EncrypterView::GetDocument() const
   { return reinterpret_cast<i3EncrypterDoc*>(m_pDocument); }
#endif

