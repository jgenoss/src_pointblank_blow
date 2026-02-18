#pragma once
#include "afxcmn.h"
#include "i3TDKOptSpecBase.h"

// i3TDKOptSpecGatherUpAttribute dialog

class I3_EXPORT_TDK i3TDKOptSpecGatherUpAttribute : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecGatherUpAttribute)

public:
	i3TDKOptSpecGatherUpAttribute(CWnd* pParent = nullptr) : i3TDKOptSpecBase(IDD_OPTSPEC_GATHERUPATTRIBUTE, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecGatherUpAttribute() {}

	enum { IDD = IDD_OPTSPEC_GATHERUPATTRIBUTE };

protected:
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
