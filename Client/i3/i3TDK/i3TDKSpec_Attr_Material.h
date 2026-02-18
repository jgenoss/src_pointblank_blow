#pragma once
#include "i3TDKColorCtrl.h"
#include "afxwin.h"

// i3TDKSpec_Attr_Material dialog

class i3TDKSpec_Attr_Material : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Material)
protected:
	i3MaterialAttr *		m_pAttr = nullptr;
	i3TDKColorCtrl			m_DiffCtrl;
	i3TDKColorCtrl			m_SpecCtrl;
	i3TDKColorCtrl			m_EmisCtrl;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Material(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Material::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Material();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_MATERIAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CButton m_CHK_Diff;
	CButton m_CHK_Spec;
	CButton m_CHK_Emis;
	CButton m_CHK_Shini;
	CStatic m_ST_DiffPH;
	CStatic m_ST_SpecPH;
	CStatic m_ST_EmisPH;
	CEdit m_ED_Shini;
	afx_msg void OnBnClickedTdkChkDiff();
	afx_msg void OnBnClickedTdkChkSpec();
	afx_msg void OnBnClickedTdkChkEmis();
	afx_msg void OnBnClickedTdkChkShini();
	afx_msg void OnEnKillfocusTdkEdShini();
};
