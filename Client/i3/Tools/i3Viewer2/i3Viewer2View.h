// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3Viewer2View.h : Ci3Viewer2View 클래스의 인터페이스
//


#pragma once

#include "i3Viewer2Framework.h"

class Ci3Viewer2View : public CView
{
protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	i3Viewer2Framework *		m_pFramework;

protected: // serialization에서만 만들어집니다.
	Ci3Viewer2View();
	DECLARE_DYNCREATE(Ci3Viewer2View)

	void Ci3Viewer2View::OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

// 특성입니다.
public:
	Ci3Viewer2Doc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC) override;  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:

// 구현입니다.
public:
	virtual ~Ci3Viewer2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};

#ifndef _DEBUG  // i3Viewer2View.cpp의 디버그 버전
inline Ci3Viewer2Doc* Ci3Viewer2View::GetDocument() const
   { return reinterpret_cast<Ci3Viewer2Doc*>(m_pDocument); }
#endif

