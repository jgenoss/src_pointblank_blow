#pragma once
#include "afxwin.h"


// i3TDKOptSpecSpatialPartition dialog

class i3TDKOptSpecSpatialPartition : public i3TDKOptSpecBase
{
	DECLARE_DYNAMIC(i3TDKOptSpecSpatialPartition)

public:
	i3TDKOptSpecSpatialPartition(CWnd* pParent = nullptr) : i3TDKOptSpecBase(i3TDKOptSpecSpatialPartition::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKOptSpecSpatialPartition() {}

// Dialog Data
	enum { IDD = IDD_OPTSPEC_SPATIALPARTITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_SizeX;
	CEdit m_ED_SizeY;
	CEdit m_ED_SizeZ;
	CEdit m_ED_TriCount;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CButton m_CHK_Split;
	afx_msg void OnBnClickedChkSplit();
	CButton m_CHK_CreateBoundBox;
};
