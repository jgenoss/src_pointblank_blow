#pragma once


// i3TDKSingleSelectAttrTypeDialog dialog

class I3_EXPORT_TDK i3TDKSingleSelectAttrTypeDialog : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSingleSelectAttrTypeDialog)

public:
	i3TDKSingleSelectAttrTypeDialog(CWnd* pParent = nullptr) : CDialog(IDD_SINGLESELECTATTRTYPEDIALOG, pParent) {}   // standard constructor
	virtual ~i3TDKSingleSelectAttrTypeDialog() {}

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
