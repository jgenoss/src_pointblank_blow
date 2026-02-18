#pragma once
#include "afxwin.h"


// i3TDK_Dlg_SelectImageExt dialog

class i3TDK_Dlg_SelectImageExt : public CDialog
{
	DECLARE_DYNAMIC(i3TDK_Dlg_SelectImageExt)

protected:
	TCHAR	m_szExt[MAX_PATH] = { 0 };

public:
	bool	Execute(void);

	const TCHAR*		getExt(void) const	{ return m_szExt; }

public:
	i3TDK_Dlg_SelectImageExt(CWnd* pParent = nullptr) : CDialog(i3TDK_Dlg_SelectImageExt::IDD, pParent) {}   // standard constructor
	virtual ~i3TDK_Dlg_SelectImageExt() {}

// Dialog Data
	enum { IDD = IDD_TDK_SELECT_IMAGE_EXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();
public:

	CComboBox m_CB_Ext;
};
