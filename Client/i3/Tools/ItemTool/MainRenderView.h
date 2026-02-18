#pragma once

#include "ItemToolFrameWork.h"
#include "ItemToolViewerCtrl.h"

class cMainRenderView : public CWnd
{
	// Construction
public:
	cMainRenderView();

	// Attributes
public:
	cItemToolViewerCtrl			m_ViewerCtrl;
	
	// Operations
public:

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// Implementation
public:
	virtual ~cMainRenderView();

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void _MoveScrollBarPos(INT32 nPos);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void ResetItemList(void);

	void AddScrollBarPos(INT32 nValue);
};

