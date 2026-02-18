#pragma once
#include "afxcmn.h"


// CDlgSelectSound dialog

class CDlgSelectSound : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectSound)

protected:
	i3Sound *		m_pSelectedSound;

public:
	CDlgSelectSound(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectSound();

	i3Sound *		Execute(void);

	// Dialog Data
	enum { IDD = IDD_DLG_SELECTSOUND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SoundListCtrl;
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedOk();
};
