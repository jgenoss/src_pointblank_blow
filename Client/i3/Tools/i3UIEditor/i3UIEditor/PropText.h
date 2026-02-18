#pragma once
#include "afxwin.h"


// PropText dialog

class PropText : public CDialog
{
	DECLARE_DYNAMIC(PropText)

public:
	PropText(CWnd* pParent = NULL);   // standard constructor
	virtual ~PropText();

	i3UIControl * m_pControl;

// Dialog Data
	enum { IDD = IDD_PROPERTY_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edDefaultString;
	CEdit m_edFontType;
	CEdit m_edMaxStringLength;
	CEdit m_edTextSpace;
	CButton m_cbAllowInput;
	CButton m_cbMultiLineText;
	CButton m_cbShadowEnable;
	CComboBox m_cbbFontSize;
	CComboBox m_cbbFontWeight;
	CComboBox m_cbbAlignX;
	CComboBox m_cbbAlignY;
	CComboBox m_cbbShadowQuality;
	CEdit m_edOffsetX;
	CEdit m_edOffsetY;
	CButton m_btnFontColor;
	virtual BOOL OnInitDialog();

	void	SetTextInfo( i3UIControl * pControl);
	afx_msg void OnEnKillfocusFontDefaultstring();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnChangeFontDefaultstring();
};
