#pragma once
#include "afxwin.h"
#include "resource.h"


// CColoDlg dialog

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)

public:
	CColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_ColorA;
	int m_ColorB;
	int m_ColorG;
	int m_ColorR;
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditColorR();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnEnChangeEditColorA();
	CStatic m_ColorStatic;
	afx_msg void OnPaint();
};
