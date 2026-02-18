// i3PackFileEditorView.h : iCi3PackFileEditorView 클래스의 인터페이스
//


#pragma once
#include "PaneProperty.h"
#include "PaneLog.h"
#include "PaneCommon.h"
#include "PackNode.h"
#include "PaneTree.h"

class Ci3PackFileEditorView : public CView
{
protected: // serialization에서만 만들어집니다.
	Ci3PackFileEditorView();
	DECLARE_DYNCREATE(Ci3PackFileEditorView)

// 특성
public:
	Ci3PackFileEditorDoc* GetDocument() const;

	void		Log( const char * pszFile, const char * pszMsg);
	void		ClearLog(void);

// 작업
public:
	i3SplitterWnd	m_Splitter0;
	i3SplitterWnd	m_Splitter1;
	i3SplitterWnd	m_Splitter2;
	CPaneTree		m_PaneTree;
	CPaneProperty	m_PaneProperty;
	CPaneLog		m_PaneLog;
	CPaneCommon		m_PaneCommon;

// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현
public:
	virtual ~Ci3PackFileEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HTREEITEM	FindTreeByNode( PackNode * pNode);
	void		OnPopupTreeMenu(void);

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
protected:
//	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};

#ifndef _DEBUG  // i3PackFileEditorView.cpp의 디버그 버전
inline Ci3PackFileEditorDoc* Ci3PackFileEditorView::GetDocument() const
   { return reinterpret_cast<Ci3PackFileEditorDoc*>(m_pDocument); }
#endif

