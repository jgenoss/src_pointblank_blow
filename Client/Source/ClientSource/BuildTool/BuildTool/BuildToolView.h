
// BuildToolView.h : CBuildToolView 클래스의 인터페이스
//


#pragma once

#include "RscTool/i3MFCTabViewCtrl.h"

class CBuildToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CBuildToolView();
	DECLARE_DYNCREATE(CBuildToolView)

// 특성입니다.
public:
	CBuildToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CBuildToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

private:
	i3MFCTabViewCtrl m_tabViewCtrl;

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // BuildToolView.cpp의 디버그 버전
inline CBuildToolDoc* CBuildToolView::GetDocument() const
   { return reinterpret_cast<CBuildToolDoc*>(m_pDocument); }
#endif

