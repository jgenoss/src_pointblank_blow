#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Resource.h"

// CSkeletonSpecDialog dialog

class CSkeletonSpecDialog : public CDialog
{
	DECLARE_DYNAMIC(CSkeletonSpecDialog)

protected:
	i3LOD *			m_pLOD;
	i3Skeleton *	m_pSkeleton;

protected:
	void		UpdateSkeleton( INT32 idx, i3Skeleton * pSkel);
	void		AssignSkeleton(void);

	INT32		GetSelectedSkeleton(void);
	
	void		SelectSkeleton( i3Skeleton * pSkel);
	void		_Rec_SetBoneTree( HTREEITEM hParent, INT32 idx);

public:
	CSkeletonSpecDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSkeletonSpecDialog();

	BOOL	Execute( i3LOD * pSkin);

// Dialog Data
	enum { IDD = IDD_DIALOG_SKELETON_LIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SkelListCtrl;
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
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickSkelList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_IM00Ctrl;
	CEdit m_IM01Ctrl;
	CEdit m_IM02Ctrl;
	CEdit m_IM03Ctrl;
	CEdit m_IM10Ctrl;
	CEdit m_IM11Ctrl;
	CEdit m_IM12Ctrl;
	CEdit m_IM13Ctrl;
	CEdit m_IM20Ctrl;
	CEdit m_IM21Ctrl;
	CEdit m_IM22Ctrl;
	CEdit m_IM23Ctrl;
	CEdit m_IM30Ctrl;
	CEdit m_IM31Ctrl;
	CEdit m_IM32Ctrl;
	CEdit m_IM33Ctrl;
	CEdit m_ShapeSetNameCtrl;
	CEdit m_JointInfoCtrl;

	CButton m_BtnShapeSetChangeCtrl;
	CButton m_BtnJointChangeCtrl;
	afx_msg void OnBnClickedBtnShapesetchangevalue();
	afx_msg void OnBnClickedBtnJointchangevalue();
};
