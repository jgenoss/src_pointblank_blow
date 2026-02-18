#pragma once
#include "afxwin.h"
#include "Resource.h"

// CShapeSetSpecDialog dialog

class CShapeSetSpecDialog : public CDialog
{
	DECLARE_DYNAMIC(CShapeSetSpecDialog)
	
protected:
	i3PhysixShapeSet * m_pShapeSet;
	INT32			m_iCurrentShapeNumber;

public:
	CShapeSetSpecDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShapeSetSpecDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_SHAPESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EdtShapeName;
	CEdit m_EdtShapeType;
	CEdit m_EdtRestitution;
	CEdit m_EdtStaticFriction;
	CEdit m_EdtDynamicFriction;
	CEdit m_EdtMass;

	virtual BOOL OnInitDialog();

	BOOL	Execute( i3PhysixShapeSet * pShapeSet );
	void	AssignPhysixValue();
	afx_msg void OnEnChangeEditShapename();
	afx_msg void OnEnChangeEditRestitution();
	afx_msg void OnEnChangeEditSfriction();
	afx_msg void OnEnChangeEditDfriction();
	afx_msg void OnEnChangeEditMass();
	CEdit m_EdtLinearDampingCtrl;
	CEdit m_EdtAngularDampingCtrl;
	CEdit m_EdtSleepLinearCtrl;
	CEdit m_EdtSleepAngularCtrl;
	afx_msg void OnEnChangeEditLinearDamping();
	afx_msg void OnEnChangeEditAngularDamping();
	afx_msg void OnEnChangeEditSleepLinear();
	afx_msg void OnEnChangeEditSleepAngular();
	CEdit m_EdtSkinWidthCtrl;
	afx_msg void OnEnChangeEditSkinwidth();
	CEdit m_Ed_TerrainType;
	CEdit m_Ed_TerrainCount;
	CEdit m_ED_ShapeGroup;
	afx_msg void OnEnChangeEditShapegroup();
	afx_msg void OnEnChangeEditTerraintype();
	CEdit m_EdShapeLocalPoseXCtrl;
	CEdit m_EdShapeLocalPoseYCtrl;
	CEdit m_EdShapeLocalPoseZCtrl;
	afx_msg void OnBnClickedBtnCalclocalcenter();
	CEdit m_EdThicknessCtrl;
	afx_msg void OnEnChangeEdThickness();
	
	CEdit m_edShapeNumber;
	CEdit m_edShapeCount;
	afx_msg void OnEnChangeEdShapeNumber();
	CEdit m_edInertiaTensorX;
	CEdit m_edInertiaTensorY;
	CEdit m_edInertiaTensorZ;
	afx_msg void OnBnClickedBtnLocalMatrix();
	afx_msg void OnBnClickedBtnGlobalMatrix();
	afx_msg void OnEnChangeEdInertiatensorx();
	afx_msg void OnEnChangeEdInertiatensory();
	afx_msg void OnEnChangeEdInertiatensorz();
	CButton m_chk_AllTerrainCtrl;
	CComboBox m_CB_Format;
	afx_msg void OnCbnSelchangeCbFormat();
};
