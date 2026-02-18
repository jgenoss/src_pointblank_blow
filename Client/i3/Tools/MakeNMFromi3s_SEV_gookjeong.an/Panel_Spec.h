#pragma once
#include "resource.h"

class Panel_Spec : public i3TDKSpecDialogContainer
{
	DECLARE_DYNAMIC(Panel_Spec)

public:
	Panel_Spec(CWnd* pParent = NULL);   // standard constructor
	virtual ~Panel_Spec();

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
