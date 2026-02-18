#pragma once
#include "afxwin.h"
#include "resource.h"

// CLeftAlignValueDlg dialog

class CLeftAlignValueDlg : public CDialog
{
	DECLARE_DYNAMIC(CLeftAlignValueDlg)

public:
	CLeftAlignValueDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLeftAlignValueDlg();

// Dialog Data
	enum { IDD = IDD_DLG_LEFTALIGN_VALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void	_UpdateInfo();
public:
	REAL32 m_X,m_Y;

	CEdit m_edPosX;
	CEdit m_edPosY;
	afx_msg void OnEnKillfocusEditPosLeft();
	afx_msg void OnEnKillfocusEditPosTop();
	
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
