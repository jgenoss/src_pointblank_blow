#pragma once


// MVFGenDialog dialog

class MVFGenDialog : public CDialog
{
	DECLARE_DYNAMIC(MVFGenDialog)

public:
	MVFGenDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~MVFGenDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_MVGEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	UINT m_AlignUnit;
	BOOL m_bCompress;
};
