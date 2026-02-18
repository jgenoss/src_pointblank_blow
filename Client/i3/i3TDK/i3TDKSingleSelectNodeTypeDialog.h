#pragma once

// i3TDKSingleSelectNodeTypeDialog dialog

class I3_EXPORT_TDK i3TDKSingleSelectNodeTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSingleSelectNodeTypeDialog)

public:
	i3TDKSingleSelectNodeTypeDialog(CWnd* pParent = nullptr) : CDialog(IDD_SINGLESELECTNODETYPEDIALOG, pParent) {}   // standard constructor
	virtual ~i3TDKSingleSelectNodeTypeDialog() {}

public:
	CComboBox		m_TypeCtrl;

	TCHAR			m_szSelectedMeta[256] = { 0 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
