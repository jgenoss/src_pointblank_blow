#pragma once

#include "Spec_EditBox.h"
#include "Spec_Button.h"

// CPaneSpec dialog

class CPaneSpec : public i3TDKDialogContainer
{
	DECLARE_DYNAMIC(CPaneSpec)
protected:

	CSpec_EditBox		m_Spec_EditBox;
	CSpec_Button		m_Spec_Button;

public:

public:
	CPaneSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneSpec();

// Dialog Data
	enum { IDD = IDD_PANE_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
