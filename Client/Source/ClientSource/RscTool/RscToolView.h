
// RscToolView.h : CRscToolView 클래스의 인터페이스
//

#pragma once

#include "i3MFCTabViewCtrl.h"


class CRscToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CRscToolView();
	DECLARE_DYNCREATE(CRscToolView)

// 특성입니다.
public:
	CRscToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CRscToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

private:
	i3MFCTabViewCtrl	m_tabViewCtrl;


// 생성된 메시지 맵 함수

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	virtual void OnInitialUpdate();
};

#ifndef _DEBUG  // RscToolView.cpp의 디버그 버전
inline CRscToolDoc* CRscToolView::GetDocument() const
   { return reinterpret_cast<CRscToolDoc*>(m_pDocument); }
#endif

