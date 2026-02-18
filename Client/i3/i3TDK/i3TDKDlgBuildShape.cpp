// i3TDKDlgBuildShape.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgBuildShape.h"
#include ".\i3tdkdlgbuildshape.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKDlgBuildShape dialog

IMPLEMENT_DYNAMIC(i3TDKDlgBuildShape, CDialog)

void i3TDKDlgBuildShape::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_RBTN_CONVEX, m_RBTN_Convex);
	DDX_Control(pDX, IDC_TDK_RBTN_DYNAMIC, m_RBTN_Dynamic);
	DDX_Control(pDX, IDC_TDK_ED_RESTITUTION, m_ED_Restitution);
	DDX_Control(pDX, IDC_TDK_ED_SFRICTION, m_ED_StaticFriction);
	DDX_Control(pDX, IDC_TDK_ED_DFRICTION, m_ED_DynamicFriction);
	DDX_Control(pDX, IDC_TDK_ED_MASS, m_ED_Mass);
	DDX_Control(pDX, IDC_TDK_CB_USAGE, m_CB_Usage);
	DDX_Control(pDX, IDC_TDK_CB_GROUP, m_CB_Group);
	DDX_Control(pDX, IDC_TDK_ED_LINEAR_DAMP, m_ED_LinearDamping);
	DDX_Control(pDX, IDC_TDK_ED_ANGULAR_DAMP, m_ED_AngularDamping);
}

void i3TDKDlgBuildShape::_setUsage( i3ShapeSetContainer::USAGE usage)
{
	switch( usage)
	{
		case i3ShapeSetContainer::USAGE_SIMULATION :
			m_RBTN_Convex.SetCheck( BST_CHECKED);
			m_RBTN_Dynamic.SetCheck( BST_CHECKED);
			break;

		case i3ShapeSetContainer::USAGE_RAYCAST :

		case i3ShapeSetContainer::USAGE_TRIGGER :
		default :
			break;
	}

	m_Usage = usage;
}

void i3TDKDlgBuildShape::_setShapeType( I3_PHYSIX_SHAPE_TYPE shapeType)
{
	switch( shapeType)
	{
	case I3_PHYSIX_SHAPE_SPHERE :
	case I3_PHYSIX_SHAPE_CAPSULE :
	case I3_PHYSIX_SHAPE_CONVEXMESH :
	case I3_PHYSIX_SHAPE_BOX :
		m_RBTN_Convex.SetCheck( BST_CHECKED);

		break;

	case I3_PHYSIX_SHAPE_TRIMESH :
		m_RBTN_Convex.SetCheck( BST_UNCHECKED);

		break;
	}
	m_ShapeType = shapeType;
}

BEGIN_MESSAGE_MAP(i3TDKDlgBuildShape, CDialog)
END_MESSAGE_MAP()


// i3TDKDlgBuildShape message handlers

BOOL i3TDKDlgBuildShape::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		TCHAR conv[256];

		m_RBTN_Convex.SetCheck( BST_CHECKED);
		m_RBTN_Dynamic.SetCheck( BST_CHECKED);

		m_CB_Group.SetCurSel( (int) 0);
		
		i3::ftoa( m_Restitution, conv);
		m_ED_Restitution.SetWindowText( conv);

		i3::ftoa( m_StaticFriction, conv);
		m_ED_StaticFriction.SetWindowText( conv);

		i3::ftoa( m_DynamicFriction, conv);
		m_ED_DynamicFriction.SetWindowText( conv);

		i3::ftoa( m_Mass, conv);
		m_ED_Mass.SetWindowText( conv);

		i3::ftoa( m_LinearDamping, conv);
		m_ED_LinearDamping.SetWindowText( conv);

		i3::ftoa( m_AngularDamping, conv);
		m_ED_AngularDamping.SetWindowText( conv);

		_setUsage( i3ShapeSetContainer::USAGE_SIMULATION);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgBuildShape::OnOK()
{	
	switch( m_CB_Usage.GetCurSel())
	{
		case 0 :		m_Usage = i3ShapeSetContainer::USAGE_NA;	break;
		case 1 :		m_Usage = i3ShapeSetContainer::USAGE_SIMULATION;	break;
		case 2 :		m_Usage = i3ShapeSetContainer::USAGE_RAYCAST; break;
		case 3 :		m_Usage = i3ShapeSetContainer::USAGE_TRIGGER; break;
	}

	m_Group = m_CB_Group.GetCurSel();
	if( m_Group == -1)
		m_Group = 0;

	if( m_RBTN_Convex.GetCheck() == BST_CHECKED)
		m_bConvex = true;
	else
		m_bConvex = false;

	if( m_RBTN_Dynamic.GetCheck() == BST_CHECKED)
		m_bDynamic = true;
	else
		m_bDynamic = false;

	TCHAR conv[256];
	m_ED_Restitution.GetWindowText( conv, _countof(conv) - 1 );
	m_Restitution = (REAL32) i3::atof( conv);

	m_ED_StaticFriction.GetWindowText( conv, _countof(conv) - 1);
	m_StaticFriction = (REAL32) i3::atof( conv);

	m_ED_DynamicFriction.GetWindowText( conv, _countof(conv) - 1);
	m_DynamicFriction = (REAL32) i3::atof( conv);

	m_ED_Mass.GetWindowText( conv, _countof(conv) - 1);
	m_Mass = (REAL32) i3::atof( conv);

	m_ED_LinearDamping.GetWindowText( conv, _countof(conv) - 1);
	m_LinearDamping = (REAL32) i3::atof( conv);

	m_ED_AngularDamping.GetWindowText( conv, _countof(conv) - 1);
	m_AngularDamping = (REAL32) i3::atof( conv);

	CDialog::OnOK();
}
