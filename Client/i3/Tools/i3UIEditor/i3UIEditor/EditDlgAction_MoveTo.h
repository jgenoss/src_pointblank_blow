#pragma once
#include "afxwin.h"


// CEditDlgAction_MoveTo dialog

class CEditDlgAction_MoveTo : public CDialog
{
	DECLARE_DYNAMIC(CEditDlgAction_MoveTo)

public:
	CEditDlgAction_MoveTo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlgAction_MoveTo();

// Dialog Data
	enum { IDD = IDD_EDITACTION_MOVE };

	I3UI_EVT_MOVE_DATA * m_pData;

public:
	void	SetMoveEventData( I3UI_EVT_MOVE_DATA * pMove);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbSelFactor;
	CEdit m_edDuration;
	CEdit m_edPosX;
	CEdit m_edPosY;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
