#pragma once
#include "afxwin.h"
#include "i3TDKOptSpecBase.h"

// i3TDKOptSpecBSPBuilder dialog

class I3_EXPORT_TDK i3TDKOptSpecBSPBuilder : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecBSPBuilder)

public:
	i3TDKOptSpecBSPBuilder(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecBSPBuilder::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecBSPBuilder() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_BSPBUILDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_CHK_Cond_Convex;
	CButton m_CHK_Cond_Depth;
	CButton m_CHK_Cond_PolyCount;
	CEdit m_ED_DepthLimit;
	CEdit m_ED_MinPolyCount;
	CComboBox m_CB_PlaneType;
	CButton m_CHK_SplitPoly;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedChkCondDepth();
	afx_msg void OnBnClickedChkCondPolycount();
};
