#pragma once


// CSpaceConvertConfirmDialog dialog

class CSpaceConvertConfirmDialog : public CDialog
{
	DECLARE_DYNAMIC(CSpaceConvertConfirmDialog)

public:
	CSpaceConvertConfirmDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpaceConvertConfirmDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
