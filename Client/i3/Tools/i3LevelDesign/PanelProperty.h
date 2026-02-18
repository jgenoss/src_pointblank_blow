#pragma once

#include "PanelPropertyCommon.h"
#include "PanelPropertySpecific.h"

// CPanelProperty dialog

class CPanelProperty : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelProperty)

protected:
//	CPropWorld	m_PropWorld;
	i3TDKWndFoldCtrl		m_FoldCtrl;
	CPanelPropertyCommon	m_CommonPropDlg;
	CPanelPropertySpecific	m_SpecPropDlg;

	// Elements

public:
	CPanelProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelProperty();

// Dialog Data
	enum { IDD = IDD_PANEL_PROPERTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel() override;
	virtual void OnOK() override;
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
