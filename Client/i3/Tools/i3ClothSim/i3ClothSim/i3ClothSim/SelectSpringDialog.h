#pragma once
#include "resource.h"
#include "afxwin.h"
#include "InputDirectionCtrl.h"

// CSelectSpringDialog dialog

class CSelectSpringDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectSpringDialog)

protected:
	CInputDirectionCtrl		m_DirCtrl;

public:
	VEC2D *			getDirection(void)			{ return &m_DirCtrl.m_Direction; }
public:
	CSelectSpringDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectSpringDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_SPRING_SELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedCancel();
	CStatic m_PlaceHolderCtrl;
	virtual BOOL OnInitDialog();
};
