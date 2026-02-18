#pragma once

#include "SpecTriangle.h"

// CPanelSpec dialog

class CPanelSpec : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPanelSpec)
public:
	CSpecTriangle		m_SpecTriangle;

public:
	CPanelSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSpec();

// Dialog Data
	enum { IDD = IDD_PANEL_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
