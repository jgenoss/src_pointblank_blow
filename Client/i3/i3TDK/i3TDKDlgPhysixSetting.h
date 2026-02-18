#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// i3TDKDlgPhysixSetting dialog

class I3_EXPORT_TDK i3TDKDlgPhysixSetting : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgPhysixSetting)
protected:
	i3Node *	m_pScene = nullptr;
	i3PhysixShapeSet *	m_pShapeSet = nullptr;
	HTREEITEM	m_hSelItem = nullptr;
	CImageList	m_ImageList;

	void				_Rec_BuildTree( i3Node * pNode, HTREEITEM hParent);
	void				_SelectShape( INT32 idx);
	void				_SelectItem( HTREEITEM hItem);
	i3PhysixShapeSet *	_getSelectedShapeSet(void);
	void				_InsertShapeSet( i3Node * pNode, i3PhysixShapeSet * pShapeSet, i3ShapeSetContainer::USAGE usage);

	void				_OnLoadNewShapes(void);
	void				_OnInsertNewShapes(void);
	void				_OnRemoveShapeSetOnly(void);
	void				_OnRemoveNode(void);

public:

	bool	Execute( i3Node * pNode);

	i3Node *	getScene(void)			{ return m_pScene; }

public:
	i3TDKDlgPhysixSetting(CWnd* pParent = nullptr) : CDialog(i3TDKDlgPhysixSetting::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgPhysixSetting();

// Dialog Data
	enum { IDD = IDD_TDK_DLG_PHYSIX_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	CComboBox m_UsageCtrl;
	CComboBox m_CollGroupCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnTvnSelchangedTdkPhysixtree(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_RestitutionCtrl;
	CEdit m_StaticFrictionCtrl;
	CEdit m_DynamicFrictionCtrl;
	CEdit m_MassCtrl;
	afx_msg void OnCbnSelchangeTdkCbUsage();
	afx_msg void OnCbnSelchangeTdkCbGroup();
	afx_msg void OnEnKillfocusTdkEdRestitution();
	afx_msg void OnEnKillfocusTdkEdStaticFriction();
	afx_msg void OnEnKillfocusTdkEdDynamicFriction();
	afx_msg void OnEnKillfocusTdkEdDynamicFriction2();
	afx_msg void OnNMRclickTdkPhysixtree(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_ShapeListCtrl;
	afx_msg void OnNMClickTdkShapelist(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_ED_LinearDamping;
	CEdit m_ED_AngularDamping;
	afx_msg void OnEnKillfocusTdkEdLinearDamping();
	afx_msg void OnEnKillfocusTdkEdAngularDamping();
	CEdit m_SkinWidthCtrl;
	CComboBox m_CB_TerrainType;
	afx_msg void OnCbnSelchangeCbTerrainType();
	CEdit m_ED_Thickness;
	afx_msg void OnEnKillfocusEditThickness();
};
