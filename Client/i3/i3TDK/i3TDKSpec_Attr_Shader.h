#pragma once
#include "afxwin.h"


// i3TDKSpec_Attr_Shader dialog

class i3TDKSpec_Attr_Shader : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_Shader)
protected:
	i3ShaderAttr *			m_pAttr = nullptr;

public:
	virtual void			SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_Shader(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_Shader::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_Shader();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_SHADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Src;
	CComboBox m_VSVersion;
	CComboBox m_PSVersion;
	CButton m_AddToCache;
	CButton m_Recompile;
	afx_msg void OnCbnSelchangeTdkCbVsVersion();
	afx_msg void OnCbnSelchangeTdkCbPsVersion();
	afx_msg void OnBnClickedTdkChkAddcache();
	afx_msg void OnBnClickedTdkBtnRecompile();
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnEnKillfocusTdkEdSrc();
};
