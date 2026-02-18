#pragma once

#include "UIPaneBase.h"

// CUIPaneScriptList

class CUIPaneScriptList : public CUIPaneBase
{
	DECLARE_DYNAMIC(CUIPaneScriptList)

protected:
	CListBox		m_ScriptList;

public:
	CUIPaneScriptList();
	virtual ~CUIPaneScriptList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLBDblClicked( void);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

};


