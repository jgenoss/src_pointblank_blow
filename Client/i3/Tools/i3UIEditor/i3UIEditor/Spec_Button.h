#pragma once
#include "afxwin.h"


// CSpec_Button dialog

class CSpec_Button : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CSpec_Button)

protected:
	i3UIButton *		m_pCtrl;

	i3TDKColorCtrl		m_ColorCtrl;

protected:
	void		_updateFontInfo(void);

public:
	CSpec_Button(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSpec_Button();

	virtual void SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_SPEC_BUTTON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Caption;
	CEdit m_ED_Font;
	CButton m_CHK_Shadow;
	CComboBox m_CB_ShadowQuality;
	CStatic m_ST_ColorPH;
	CComboBox m_CB_AlignHorz;
	CComboBox m_CB_AlignVert;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEnKillfocusEdCaption();
	afx_msg void OnBnClickedBtnFont();
	afx_msg void OnBnClickedChkShadow();
	afx_msg void OnCbnSelchangeCbShadowquality();
	afx_msg void OnCbnSelchangeCbHorzalign();
	afx_msg void OnCbnSelchangeCbVertalign();
};
