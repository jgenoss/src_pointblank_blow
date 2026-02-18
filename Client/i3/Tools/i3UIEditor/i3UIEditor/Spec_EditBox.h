#pragma once
#include "afxwin.h"


// CSpec_EditBox dialog

class CSpec_EditBox : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpec_EditBox)

protected:
	i3UIEditBox *			m_pCtrl;

	i3TDKColorCtrl			m_ColorCtrl;

protected:
	void		_updateFontInfo(void);

public:
	CSpec_EditBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpec_EditBox();

	virtual void	SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_EDITBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnFont();
	afx_msg void OnCbnSelchangeCbShadowquality();
	afx_msg void OnBnClickedChkReadonly();
	afx_msg void OnBnClickedChkShadow();
	afx_msg void OnBnClickedChkAutowrap();
	afx_msg void OnBnClickedChkPassword();
	afx_msg void OnEnKillfocusEdMaxtext();
	CEdit m_ED_Text;
	CStatic m_ST_ColorPH;
	CEdit m_ED_Font;
	CButton m_CHK_ReadOnly;
	CButton m_CHK_Shadow;
	CStatic m_ST_MaxLength;
	CEdit m_ED_MaxLength;
	CStatic m_ST_ShadowQuality;
	CComboBox m_CB_ShadowQuality;
	CButton m_CHK_Password;
	CButton m_CHK_AutoWrap;
	afx_msg void OnEnKillfocusEdTxt();
	CButton m_CHK_NoBorder;
	afx_msg void OnBnClickedChkNoborder();
};
