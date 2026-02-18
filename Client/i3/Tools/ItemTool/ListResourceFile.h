#pragma once

#include "ColoredListCtrl.h"

class cListResourceFile : public CColoredListCtrl
{
	DECLARE_DYNAMIC(cListResourceFile)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	cListResourceFile();
	virtual ~cListResourceFile(){}

	void Init();
	void UpdateAllRes();

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);	
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
};
