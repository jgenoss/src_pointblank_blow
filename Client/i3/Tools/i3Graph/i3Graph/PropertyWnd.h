#pragma once

#include "i3treeProperty.h"
// CPropertyWnd dialog

class CPropertyWnd : public CDialog
{
	DECLARE_DYNAMIC(CPropertyWnd)

protected: 
	PropertyBoard			m_uProperty;
	INT32					m_pCenterx; 
	BOOL					m_pCenterBar;
	RECT					m_Brect;												//For Capture Bar
public:
	CPropertyWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPropertyWnd();
	
	void SetProperty(i3ElementBase * pBase); 
// Dialog Data
	enum { IDD = IDD_DIALOG_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL OnInitDialog(void); 
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};
