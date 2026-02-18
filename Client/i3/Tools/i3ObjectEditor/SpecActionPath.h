#pragma once
#include "afxwin.h"


// SpecActionPath dialog

class SpecActionPath : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(SpecActionPath)
protected:
	i3ActionPath *		m_pAction;

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	SpecActionPath(CWnd* pParent = NULL);   // standard constructor
	virtual ~SpecActionPath();

// Dialog Data
	enum { IDD = IDD_SPEC_ACTION_PATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_AnimPath;
	afx_msg void OnBnClickedBtnAnimpath();
	CComboBox m_CB_PlayMode;
	afx_msg void OnCbnSelchangeCbPlaymode();
};
