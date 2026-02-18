#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// i3TDKDlg_Skeleton dialog

class I3_EXPORT_TDK i3TDKDlg_Skeleton : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlg_Skeleton)
protected:
	i3Skeleton *	m_pSkel = nullptr;

protected:
	void			UpdateSkeleton(void);
	void			_Rec_SetBoneTree( HTREEITEM hParent, INT32 idx);

	void			SelectBone( i3Bone * pBone);

public:
	i3TDKDlg_Skeleton(CWnd* pParent = nullptr) : CDialog(i3TDKDlg_Skeleton::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlg_Skeleton() {}

// Dialog Data
	enum { IDD = IDD_TDK_DLG_SKELETON };

	bool		Execute( i3Skeleton * pSkel);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_BoneTreeCtrl;
	CEdit m_M00Ctrl;
	CEdit m_M01Ctrl;
	CEdit m_M02Ctrl;
	CEdit m_M03Ctrl;
	CEdit m_M10Ctrl;
	CEdit m_M11Ctrl;
	CEdit m_M12Ctrl;
	CEdit m_M13Ctrl;
	CEdit m_M20Ctrl;
	CEdit m_M21Ctrl;
	CEdit m_M22Ctrl;
	CEdit m_M23Ctrl;
	CEdit m_M30Ctrl;
	CEdit m_M31Ctrl;
	CEdit m_M32Ctrl;
	CEdit m_M33Ctrl;
	CEdit m_ShapeSetNameCtrl;
	CEdit m_JointInfoCtrl;

	CButton m_BtnShapeSetChangeCtrl;
	CButton m_BtnJointChangeCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	void		Expand( HTREEITEM hItem);
	void		Collapse( HTREEITEM hItem);

	afx_msg void OnEnKillfocusTdkEdM00();
	afx_msg void OnEnKillfocusTdkEdM01();
	afx_msg void OnEnKillfocusTdkEdM02();
	afx_msg void OnEnKillfocusTdkEdM03();
	afx_msg void OnEnKillfocusTdkEdM10();
	afx_msg void OnEnKillfocusTdkEdM11();
	afx_msg void OnEnKillfocusTdkEdM12();
	afx_msg void OnEnKillfocusTdkEdM13();
	afx_msg void OnEnKillfocusTdkEdM20();
	afx_msg void OnEnKillfocusTdkEdM21();
	afx_msg void OnEnKillfocusTdkEdM22();
	afx_msg void OnEnKillfocusTdkEdM23();
	afx_msg void OnEnKillfocusTdkEdM30();
	afx_msg void OnEnKillfocusTdkEdM31();
	afx_msg void OnEnKillfocusTdkEdM32();
	afx_msg void OnEnKillfocusTdkEdM33();
	afx_msg void OnBnClickedTdkBtnShapeset();
	afx_msg void OnBnClickedTdkBtnJoint();
	afx_msg void OnTvnSelchangedTdkSkel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnRclickTree(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_ED_Name;
};
