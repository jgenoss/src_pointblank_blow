#pragma once


// CPanel_Spec dialog

class CPanel_Spec : public i3TDKSpecDialogContainer
{
	DECLARE_DYNAMIC(CPanel_Spec)

public:
	CPanel_Spec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Spec();

// Dialog Data
	enum { IDD = IDD_PANEL_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
};
