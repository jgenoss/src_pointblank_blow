
#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList window

#define MAX_TIME	24

#include "LinePlot.h"

class COutputList : public CListBox
{
// Construction
public:
	COutputList();

// Implementation
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
// Construction
public:
	COutputWnd();

// Attributes
protected:
	CFont m_Font;

	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputBuild;
	CLinePlot	m_wndOutputDebug;
	COutputList m_wndOutputFind;

	FLOATPOINT	m_pGraphData[MAX_TIME];

protected:	
	void AdjustHorzScroll(CListBox& wndListBox);

// Implementation

public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	//implementation 
	void FillWindow( INT32 i32Idx, TCHAR* strText );
	void UpdateHackCount(UINT32 * pHackCount); 

	void SetGraph(INT32 ccuData);
};

