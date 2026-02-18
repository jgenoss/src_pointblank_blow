#pragma once
#include "afxwin.h"


// CEditDlgAction_RotateTo dialog

class CEditDlgAction_RotateTo : public CDialog
{
	DECLARE_DYNAMIC(CEditDlgAction_RotateTo)

public:
	CEditDlgAction_RotateTo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlgAction_RotateTo();

// Dialog Data
	enum { IDD = IDD_EDITACTION_ROTATE };

protected:
	I3UI_EVT_ROTATE_DATA	*	m_pData;

public:
	void	SetRotateEventData( I3UI_EVT_ROTATE_DATA * pData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CComboBox m_cbSelFactor;
	CComboBox m_cbSelSpinDir;
	CEdit m_edDuration;
	CButton m_checkLoopRotate;
};
