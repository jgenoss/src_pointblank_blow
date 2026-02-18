// LeftView.h : CLeftView 클래스의 인터페이스
//


#pragma once

class Ci3PackFileEditorDoc;

class CLeftView : public CTreeView
{
protected: // serialization에서만 만들어집니다.
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// 특성
public:
	Ci3PackFileEditorDoc* GetDocument();

// 작업
public:

// 재정의
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현
public:
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // LeftView.cpp의 디버그 버전
inline Ci3PackFileEditorDoc* CLeftView::GetDocument()
   { return reinterpret_cast<Ci3PackFileEditorDoc*>(m_pDocument); }
#endif

