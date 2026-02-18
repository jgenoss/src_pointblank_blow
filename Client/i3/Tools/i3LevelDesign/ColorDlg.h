#pragma once
#include "afxwin.h"


// CColorDlg dialog

class CColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CColorDlg)

public:
	CColorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorDlg();

	void	SetColor( COLOR * pCol);
	void	SetColor( COLORREAL * pCol);

	void	GetColor( COLOR * pCol);
	void	GetColor( COLORREAL * pCol);
// Dialog Data
	enum { IDD = IDD_COLORDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_ColorA;
	int m_ColorR;
	int m_ColorG;
	int m_ColorB;
	CStatic m_ColorStatic;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnColsel();
	afx_msg void OnEnChangeEditColorA();
	afx_msg void OnEnChangeEditColorB();
	afx_msg void OnEnChangeEditColorG();
	afx_msg void OnEnChangeEditColorR();
	afx_msg void OnPaint();
};
