#pragma once
#include "afxwin.h"
#include "i3TDKColorCtrl.h"

// i3TDKSpec_Attr_Fresnel dialog

class i3TDKSpec_Attr_Fresnel : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Fresnel)
protected:
	i3FresnelAttr *		m_pAttr = nullptr;
	i3TDKColorCtrl		m_ColorCtrl;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Fresnel(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Fresnel::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Fresnel();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_FRESNEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Enable;
	CEdit m_ED_IOR;
	CStatic m_ST_ColorPH;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedTdkChkEnable();
	afx_msg void OnEnKillfocusTdkEdIor();
};
