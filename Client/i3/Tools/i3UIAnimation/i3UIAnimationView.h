
// i3UIAnimationView.h : i3UIAnimationView 클래스의 인터페이스
//

#pragma once


class i3UIAnimationView : public CView
{
protected: // serialization에서만 만들어집니다.
	i3UIAnimationView();
	DECLARE_DYNCREATE(i3UIAnimationView)

// 특성입니다.
public:
	i3UIAnimationDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC) override;  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;
protected:

// 구현입니다.
public:
	virtual ~i3UIAnimationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
};

#ifndef _DEBUG  // i3UIAnimationView.cpp의 디버그 버전
inline i3UIAnimationDoc* i3UIAnimationView::GetDocument() const
   { return reinterpret_cast<i3UIAnimationDoc*>(m_pDocument); }
#endif

