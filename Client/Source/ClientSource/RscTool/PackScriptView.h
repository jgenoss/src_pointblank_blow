#pragma once

// PackScriptView view

// SplitterWnd에 대한 헬퍼클래스를 만들어 놓을 것..



class PackScriptView : public CView
{
	DECLARE_DYNCREATE(PackScriptView)

protected:
	PackScriptView();           // protected constructor used by dynamic creation
	virtual ~PackScriptView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	CSplitterWndEx		m_splitter;
	CSplitterWndEx		m_splitter_col3;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


