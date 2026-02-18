#pragma once


// CPaneSpec dialog

class CPaneSpec : public CDialog
{
	DECLARE_DYNAMIC(CPaneSpec)

public:
	CPaneSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneSpec();

// Dialog Data
	enum { IDD = IDD_PANE_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
