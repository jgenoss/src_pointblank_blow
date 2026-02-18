// i3DbgViewerView.h : iCi3DbgViewerView 클래스의 인터페이스
//


#pragma once
#include "i3DebugViewer.h"

class Ci3DbgViewerView : public CView
{
protected: // serialization에서만 만들어집니다.
	Ci3DbgViewerView();
	DECLARE_DYNCREATE(Ci3DbgViewerView)

protected:
	i3DebugViewer *			m_pViewer;
	i3ViewerFramework *		m_pFramework;

// 특성
public:
	Ci3DbgViewerDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현
public:
	virtual ~Ci3DbgViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	afx_msg void OnDestroy();
public:
	afx_msg void OnPaint();
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // i3DbgViewerView.cpp의 디버그 버전
inline Ci3DbgViewerDoc* Ci3DbgViewerView::GetDocument() const
   { return reinterpret_cast<Ci3DbgViewerDoc*>(m_pDocument); }
#endif

