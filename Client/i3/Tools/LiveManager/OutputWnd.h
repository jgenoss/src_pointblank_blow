
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 창

class COutputList : public CListBox
{
// 생성입니다.
public:
	COutputList();

// 구현입니다.
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 생성입니다.
public:
	COutputWnd();

// 특성입니다.
protected:
	CFont m_Font;

	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputLog;
	//COutputList m_wndOutputDebug;
	//COutputList m_wndOutputFind;

protected:
	void FillBuildWindow();
	void FillDebugWindow();
	void FillFindWindow();

	void AdjustHorzScroll(CListBox& wndListBox);

// 구현입니다.
public:
	virtual ~COutputWnd();
	void cprintf(CString text)
	{ 
		m_wndOutputLog.AddString(text);
		m_wndOutputLog.SetCurSel(m_wndOutputLog.GetCount()-1);  
	}

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

