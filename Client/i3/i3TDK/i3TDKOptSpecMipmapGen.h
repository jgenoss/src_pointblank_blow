#pragma once
#include "afxwin.h"


// i3TDKOptSpecMipmapGen dialog

class i3TDKOptSpecMipmapGen : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecMipmapGen)

public:
	i3TDKOptSpecMipmapGen(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecMipmapGen::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecMipmapGen() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_MIPMAPGEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_LimitSize;
	CEdit m_ED_MinX;
	CEdit m_ED_MinY;
	CButton m_CHK_LimitLevel;
	CEdit m_ED_MaxLevel;
	CComboBox m_CB_Filter;
	CButton m_CHK_Dither;
	CButton m_CHK_SRGBIn;
	CButton m_CHK_SRGBOut;
	CButton m_CHK_MirrorU;
	CButton m_CHK_MirrorV;
	CButton m_CHK_MirrorW;
	afx_msg void OnCbnSelchangeCbFilter();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedChkLimitsize();
	afx_msg void OnBnClickedChkLimitlevel();
};
