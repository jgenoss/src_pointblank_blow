#pragma once
#include "afxwin.h"


// i3TDKOptSpecOctreeBuilder dialog

class i3TDKOptSpecOctreeBuilder : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecOctreeBuilder)

public:
	i3TDKOptSpecOctreeBuilder(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecOctreeBuilder::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecOctreeBuilder() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_OCTREE_BUILDER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_CB_CenterMethod;
	CButton m_CHK_Split;
	CButton m_CHK_Depth;
	CEdit m_ED_Depth;
	CButton m_CHK_MinPoly;
	CEdit m_ED_MinPoly;
	CEdit m_ED_CellSize;
	CEdit m_ED_EPS;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CEdit m_ED_K;
	afx_msg void OnBnClickedTdkChkLimitdepth();
	afx_msg void OnBnClickedTdkChkMinpoly();
};
