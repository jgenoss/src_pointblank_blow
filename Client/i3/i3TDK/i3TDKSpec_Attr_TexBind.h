#pragma once
#include "afxwin.h"
#include "i3TDKImageViewCtrl.h"

// i3TDKSpec_Attr_TexBind dialog

class i3TDKSpec_Attr_TexBind : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_Attr_TexBind)
protected:
	i3TextureBindAttr *		m_pAttr = nullptr;

	i3TDKImageViewCtrl		m_ImageCtrl;

public:
	virtual void		SetObject( i3ElementBase * pObj);

public:
	i3TDKSpec_Attr_TexBind(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_Attr_TexBind::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_Attr_TexBind();

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTR_TEXBIND };

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
	CEdit m_ED_Path;
	CButton m_BTN_Path;
	CEdit m_ED_Width;
	CEdit m_ED_Height;
	CEdit m_ED_FormatPersist;
	CEdit m_ED_FormatRuntime;
	CEdit m_ED_Level;
	CComboBox m_CB_Gen;
	afx_msg void OnBnClickedTdkBtnFile();
	afx_msg void OnCbnSelchangeTdkCbMipmapGenfilter();
	afx_msg void OnCbnSelchangeTdkCbMipmapgen();
	CComboBox m_CB_GenFilter;
	CStatic m_ST_ImagePH;
};
