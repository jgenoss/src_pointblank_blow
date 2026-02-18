#pragma once
#include "afxwin.h"


// CEditDlgAction_ScaleTo dialog

class CEditDlgAction_ScaleTo : public CDialog
{
	DECLARE_DYNAMIC(CEditDlgAction_ScaleTo)

public:
	CEditDlgAction_ScaleTo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEditDlgAction_ScaleTo();

// Dialog Data
	enum { IDD = IDD_EDITACTION_SCALE };

protected:
	I3GUI_EVENT_SCALE_DATA * m_pData;

public:
	void	SetScaleEventData( I3GUI_EVENT_SCALE_DATA * pData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbSelFactor;
	CEdit m_edDuration;
	CEdit m_edScaleX;
	CEdit m_edScaleY;
	CButton m_chkLoop;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
};
