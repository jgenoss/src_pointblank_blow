#pragma once
#include "afxwin.h"


// i3TDKDlgBuildShape dialog

class i3TDKDlgBuildShape : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgBuildShape)
public:
	i3ShapeSetContainer::USAGE	m_Usage = i3ShapeSetContainer::USAGE_NA;
	I3_PHYSIX_SHAPE_TYPE		m_ShapeType = I3_PHYSIX_SHAPE_NA;
	INT32		m_Group = 0;
	bool		m_bConvex = true;
	bool		m_bDynamic = true;
	REAL32		m_Restitution = 0.1f;
	REAL32		m_StaticFriction = 1.1f;
	REAL32		m_DynamicFriction = 1.1f;
	REAL32		m_Mass = 0.5f;
	REAL32		m_LinearDamping = 0.5f;
	REAL32		m_AngularDamping = 0.01f;

protected:
	void		_setUsage( i3ShapeSetContainer::USAGE usage);
	void		_setShapeType( I3_PHYSIX_SHAPE_TYPE	shapeType );

public:
	i3TDKDlgBuildShape(CWnd* pParent = nullptr) : CDialog(i3TDKDlgBuildShape::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgBuildShape() {}

// Dialog Data
	enum { IDD = IDD_TDK_BUILD_SHAPESET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_RBTN_Convex;
	CButton m_RBTN_Dynamic;
	CEdit m_ED_Restitution;
	CEdit m_ED_StaticFriction;
	CEdit m_ED_DynamicFriction;
	CEdit m_ED_Mass;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CComboBox m_CB_Usage;
	CComboBox m_CB_Group;
	CEdit m_ED_LinearDamping;
	CEdit m_ED_AngularDamping;
};
