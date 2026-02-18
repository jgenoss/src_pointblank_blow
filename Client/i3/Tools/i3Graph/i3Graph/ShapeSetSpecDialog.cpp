// ShapeSetSpecDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ShapeSetSpecDialog.h"
#include ".\shapesetspecdialog.h"
#include "ShapeLocalMatrixDialog.h"
#include "ShapeGlobalMatrixDialog.h"


// CShapeSetSpecDialog dialog

IMPLEMENT_DYNAMIC(CShapeSetSpecDialog, CDialog)
CShapeSetSpecDialog::CShapeSetSpecDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeSetSpecDialog::IDD, pParent)
{
	m_pShapeSet				= NULL;
	m_iCurrentShapeNumber	= 0;
}

CShapeSetSpecDialog::~CShapeSetSpecDialog()
{
}

void CShapeSetSpecDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SHAPENAME, m_EdtShapeName);
	DDX_Control(pDX, IDC_EDIT_SHAPETYPE, m_EdtShapeType);
	DDX_Control(pDX, IDC_EDIT_RESTITUTION, m_EdtRestitution);
	DDX_Control(pDX, IDC_EDIT_SFRICTION, m_EdtStaticFriction);
	DDX_Control(pDX, IDC_EDIT_DFRICTION, m_EdtDynamicFriction);
	DDX_Control(pDX, IDC_EDIT_MASS, m_EdtMass);
	DDX_Control(pDX, IDC_EDIT_LINEAR_DAMPING, m_EdtLinearDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_ANGULAR_DAMPING, m_EdtAngularDampingCtrl);
	DDX_Control(pDX, IDC_EDIT_SLEEP_LINEAR, m_EdtSleepLinearCtrl);
	DDX_Control(pDX, IDC_EDIT_SLEEP_ANGULAR, m_EdtSleepAngularCtrl);
	DDX_Control(pDX, IDC_EDIT_SKINWIDTH, m_EdtSkinWidthCtrl);
	DDX_Control(pDX, IDC_EDIT_TERRAINTYPE, m_Ed_TerrainType);
	DDX_Control(pDX, IDC_EDIT_TERRAINCOUNT, m_Ed_TerrainCount);
	DDX_Control(pDX, IDC_EDIT_SHAPEGROUP, m_ED_ShapeGroup);
	DDX_Control(pDX, IDC_ED_LOCALPOSEX, m_EdShapeLocalPoseXCtrl);
	DDX_Control(pDX, IDC_ED_LOCALPOSEY, m_EdShapeLocalPoseYCtrl);
	DDX_Control(pDX, IDC_ED_LOCALPOSEZ, m_EdShapeLocalPoseZCtrl);
	DDX_Control(pDX, IDC_ED_THICKNESS, m_EdThicknessCtrl);
	DDX_Control(pDX, IDC_ED_SHAPE_NUMBER, m_edShapeNumber);
	DDX_Control(pDX, IDC_ED_SHAPE_COUNT, m_edShapeCount);
	DDX_Control(pDX, IDC_ED_INERTIATENSORX, m_edInertiaTensorX);
	DDX_Control(pDX, IDC_ED_INERTIATENSORY, m_edInertiaTensorY);
	DDX_Control(pDX, IDC_ED_INERTIATENSORZ, m_edInertiaTensorZ);
	DDX_Control(pDX, IDC_CHECK1, m_chk_AllTerrainCtrl);
	DDX_Control(pDX, IDC_CB_FORMAT, m_CB_Format);
}


BEGIN_MESSAGE_MAP(CShapeSetSpecDialog, CDialog)
	ON_EN_CHANGE(IDC_EDIT_SHAPENAME, OnEnChangeEditShapename)
	ON_EN_CHANGE(IDC_EDIT_RESTITUTION, OnEnChangeEditRestitution)
	ON_EN_CHANGE(IDC_EDIT_SFRICTION, OnEnChangeEditSfriction)
	ON_EN_CHANGE(IDC_EDIT_DFRICTION, OnEnChangeEditDfriction)
	ON_EN_CHANGE(IDC_EDIT_MASS, OnEnChangeEditMass)
	ON_EN_CHANGE(IDC_EDIT_LINEAR_DAMPING, OnEnChangeEditLinearDamping)
	ON_EN_CHANGE(IDC_EDIT_ANGULAR_DAMPING, OnEnChangeEditAngularDamping)
	ON_EN_CHANGE(IDC_EDIT_SLEEP_LINEAR, OnEnChangeEditSleepLinear)
	ON_EN_CHANGE(IDC_EDIT_SLEEP_ANGULAR, OnEnChangeEditSleepAngular)
	ON_EN_CHANGE(IDC_EDIT_SKINWIDTH, OnEnChangeEditSkinwidth)
	ON_EN_CHANGE(IDC_EDIT_SHAPEGROUP, OnEnChangeEditShapegroup)
	ON_EN_CHANGE(IDC_EDIT_TERRAINTYPE, OnEnChangeEditTerraintype)
	ON_BN_CLICKED(IDC_BTN_CALCLOCALCENTER, OnBnClickedBtnCalclocalcenter)
	ON_EN_CHANGE(IDC_ED_THICKNESS, OnEnChangeEdThickness)
	ON_EN_CHANGE(IDC_ED_SHAPE_NUMBER, OnEnChangeEdShapeNumber)
	ON_BN_CLICKED(IDC_BTN_LOCAL_MATRIX, OnBnClickedBtnLocalMatrix)
	ON_BN_CLICKED(IDC_BTN_GLOBAL_MATRIX, OnBnClickedBtnGlobalMatrix)
	ON_EN_CHANGE(IDC_ED_INERTIATENSORX, OnEnChangeEdInertiatensorx)
	ON_EN_CHANGE(IDC_ED_INERTIATENSORY, OnEnChangeEdInertiatensory)
	ON_EN_CHANGE(IDC_ED_INERTIATENSORZ, OnEnChangeEdInertiatensorz)
	ON_CBN_SELCHANGE(IDC_CB_FORMAT, OnCbnSelchangeCbFormat)
END_MESSAGE_MAP()


// CShapeSetSpecDialog message handlers
BOOL CShapeSetSpecDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignPhysixValue();

	return TRUE;
}


BOOL CShapeSetSpecDialog::Execute( i3PhysixShapeSet * pShapeSet )
{
	m_pShapeSet = pShapeSet;

	if( DoModal() == IDCANCEL )
		return FALSE;

	return TRUE;
}

void CShapeSetSpecDialog::AssignPhysixValue()
{
	BOOL bCheckValid = TRUE;
	char szTemp[ 256]={0,};

	if( m_pShapeSet != NULL )
	{
		itoa( m_iCurrentShapeNumber, szTemp, 10);
		m_edShapeNumber.SetWindowText( szTemp);

		itoa( m_pShapeSet->getNxShapeCount(), szTemp, 10);
		m_edShapeCount.SetWindowText( szTemp);
		m_edShapeCount.EnableWindow( FALSE);

		// Format
		switch( m_pShapeSet->getFormat())
		{
			case i3PhysixShapeSet::FORMAT_XML :		m_CB_Format.SetCurSel( 1);	break;
			default :								m_CB_Format.SetCurSel( 0);	break;
		}

		NxShape * pShape = m_pShapeSet->getNxShape( m_iCurrentShapeNumber);

		if( pShape != NULL )
		{
			if( pShape->getName() != NULL)		m_EdtShapeName.SetWindowText( pShape->getName() );
			else if( m_pShapeSet->hasName())	m_EdtShapeName.SetWindowText( m_pShapeSet->GetName() );

			itoa( m_pShapeSet->getShapeGroup(), szTemp, 10);
			m_ED_ShapeGroup.SetWindowText( szTemp);

			switch( pShape->getType() )
			{
			case NX_SHAPE_PLANE :		strcpy( szTemp, "Plane" );	break;
			case NX_SHAPE_SPHERE :		strcpy( szTemp, "Sphere" );	break;
			case NX_SHAPE_BOX :			strcpy( szTemp, "Box" );	break;
			case NX_SHAPE_CAPSULE :		strcpy( szTemp, "Capsule" );	break;
			case NX_SHAPE_WHEEL :		strcpy( szTemp, "Wheel" );	break;
			case NX_SHAPE_CONVEX :		strcpy( szTemp, "Convex" );	break;
			case NX_SHAPE_MESH :		strcpy( szTemp, "TriMesh" );	break;
			case NX_SHAPE_HEIGHTFIELD :	strcpy( szTemp, "HeightField" );	break;
			default :	strcpy( szTemp, "Unknown" );	break;
			}
			m_EdtShapeType.EnableWindow();
			m_EdtShapeType.SetWindowText( szTemp);
			m_EdtShapeType.EnableWindow( FALSE );

			i3String::ftoa( m_pShapeSet->getSkinWidth( 0 ), szTemp);
			m_EdtSkinWidthCtrl.SetWindowText( szTemp);

			i3String::ftoa( m_pShapeSet->getRestitution(), szTemp);
			m_EdtRestitution.SetWindowText( szTemp );

			i3String::ftoa( m_pShapeSet->getStaticFriction(), szTemp);
			m_EdtStaticFriction.SetWindowText( szTemp );

			i3String::ftoa( m_pShapeSet->getDynamicFriction(), szTemp);
			m_EdtDynamicFriction.SetWindowText( szTemp );

			if( m_pShapeSet->isDynamicActor() )
			{
				i3String::ftoa( m_pShapeSet->getLinearDamping(), szTemp );
				m_EdtLinearDampingCtrl.SetWindowText( szTemp);

				i3String::ftoa( m_pShapeSet->getAngularDamping(), szTemp );
				m_EdtAngularDampingCtrl.SetWindowText( szTemp);

				i3String::ftoa( m_pShapeSet->getSleepLinearVelocity(), szTemp );
				m_EdtSleepLinearCtrl.SetWindowText( szTemp);

				i3String::ftoa( m_pShapeSet->getSleepAngularVelocity(), szTemp );
				m_EdtSleepAngularCtrl.SetWindowText( szTemp);

				// Mass Center position
				VEC3D vec;
				m_pShapeSet->getMassLocalPose( &vec);
				i3String::ftoa( getX( &vec), szTemp );
				m_EdShapeLocalPoseXCtrl.SetWindowText( szTemp);

				i3String::ftoa( getY( &vec), szTemp );
				m_EdShapeLocalPoseYCtrl.SetWindowText( szTemp);

				i3String::ftoa( getZ( &vec), szTemp );
				m_EdShapeLocalPoseZCtrl.SetWindowText( szTemp);

				// mass
				i3String::ftoa( m_pShapeSet->getMass(), szTemp);
				m_EdtMass.SetWindowText( szTemp );

				// inertiatensor
				m_pShapeSet->getInertiaTensor( &vec);
				i3String::ftoa( getX( &vec), szTemp);
				m_edInertiaTensorX.SetWindowText( szTemp);
				i3String::ftoa( getY( &vec), szTemp);
				m_edInertiaTensorY.SetWindowText( szTemp);
				i3String::ftoa( getZ( &vec), szTemp);
				m_edInertiaTensorZ.SetWindowText( szTemp);
			}
			else
			{
				m_EdtLinearDampingCtrl.SetWindowText( "0.0");
				m_EdtLinearDampingCtrl.EnableWindow( FALSE );
				m_EdtAngularDampingCtrl.SetWindowText( "0.0");
				m_EdtAngularDampingCtrl.EnableWindow( FALSE);
				m_EdtSleepLinearCtrl.SetWindowText( "0.0");
				m_EdtSleepLinearCtrl.EnableWindow( FALSE);
				m_EdtSleepAngularCtrl.SetWindowText( "0.0");
				m_EdtSleepAngularCtrl.EnableWindow( FALSE);
				
				m_EdShapeLocalPoseXCtrl.SetWindowText( "0.0");
				m_EdShapeLocalPoseXCtrl.EnableWindow( FALSE);

				m_EdShapeLocalPoseYCtrl.SetWindowText( "0.0");
				m_EdShapeLocalPoseYCtrl.EnableWindow( FALSE);
				
				m_EdShapeLocalPoseZCtrl.SetWindowText( "0.0");
				m_EdShapeLocalPoseZCtrl.EnableWindow( FALSE);
				
				m_EdtMass.SetWindowText( "0.0" );
				m_EdtMass.EnableWindow( FALSE);

				m_edInertiaTensorX.SetWindowText( "0.0");
				m_edInertiaTensorX.EnableWindow( FALSE);
				m_edInertiaTensorY.SetWindowText( "0.0");
				m_edInertiaTensorY.EnableWindow( FALSE);
				m_edInertiaTensorZ.SetWindowText( "0.0");
				m_edInertiaTensorZ.EnableWindow( FALSE);

			}

			if( m_pShapeSet->getTerrainCount() > 0 )
			{
				itoa( m_pShapeSet->getTerrain( 0)->m_Terrain, szTemp, 10);
				m_Ed_TerrainType.SetWindowText( szTemp);

				itoa( m_pShapeSet->getTerrain( 0)->m_Thickness, szTemp, 10);
				m_EdThicknessCtrl.SetWindowText( szTemp);
			}

			itoa( m_pShapeSet->getTerrainCount(), szTemp, 10 );
			m_Ed_TerrainCount.SetWindowText( szTemp );
			m_Ed_TerrainCount.EnableWindow( FALSE);
		}
		else
		{
			bCheckValid = FALSE;
		}
	}
	else
	{
		bCheckValid = FALSE;
	}

	if( !bCheckValid )
	{
		m_EdtShapeName.SetWindowText( "(none)" );
		m_EdtShapeType.EnableWindow();
		m_EdtShapeType.SetWindowText( "(undefined)");
		m_EdtShapeType.EnableWindow( FALSE );
		m_EdtRestitution.SetWindowText( "0.0" );
		m_EdtStaticFriction.SetWindowText( "0.0" );
		m_EdtDynamicFriction.SetWindowText( "0.0" );
		m_EdtMass.SetWindowText( "0.0" );
		m_EdtSkinWidthCtrl.SetWindowText( "0.0");

		m_EdtLinearDampingCtrl.SetWindowText( "0.0");
		m_EdtLinearDampingCtrl.EnableWindow( FALSE );
		m_EdtAngularDampingCtrl.SetWindowText( "0.0");
		m_EdtAngularDampingCtrl.EnableWindow( FALSE);
		m_EdtSleepLinearCtrl.SetWindowText( "0.0");
		m_EdtSleepLinearCtrl.EnableWindow( FALSE);
		m_EdtSleepAngularCtrl.SetWindowText( "0.0");
		m_EdtSleepAngularCtrl.EnableWindow( FALSE);
		m_Ed_TerrainType.SetWindowText( "0" );
		m_Ed_TerrainCount.SetWindowText( "0" );
		m_Ed_TerrainCount.EnableWindow( FALSE);
		m_EdThicknessCtrl.SetWindowText( "0.0");
		m_ED_ShapeGroup.SetWindowText( "0");

		m_EdShapeLocalPoseXCtrl.SetWindowText( "0.0");
		m_EdShapeLocalPoseXCtrl.EnableWindow( FALSE);
		m_EdShapeLocalPoseYCtrl.SetWindowText( "0.0");
		m_EdShapeLocalPoseYCtrl.EnableWindow( FALSE);
		m_EdShapeLocalPoseZCtrl.SetWindowText( "0.0");
		m_EdShapeLocalPoseZCtrl.EnableWindow( FALSE);

		m_EdtMass.SetWindowText( "0.0" );
		m_EdtMass.EnableWindow( FALSE);

		m_edInertiaTensorX.SetWindowText( "0.0");
		m_edInertiaTensorX.EnableWindow( FALSE);
		m_edInertiaTensorY.SetWindowText( "0.0");
		m_edInertiaTensorY.EnableWindow( FALSE);
		m_edInertiaTensorZ.SetWindowText( "0.0");
		m_edInertiaTensorZ.EnableWindow( FALSE);
	}
}

void CShapeSetSpecDialog::OnEnChangeEditShapename()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtShapeName.GetWindowText( szValue, 256 );

	if( i3String::Length( szValue) )
	{
		m_pShapeSet->SetName( (const char*) szValue );
	}
}

void CShapeSetSpecDialog::OnEnChangeEditRestitution()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtRestitution.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setRestitution( value );
}

void CShapeSetSpecDialog::OnEnChangeEditSfriction()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtStaticFriction.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setFriction( value, m_pShapeSet->getDynamicFriction() );
}

void CShapeSetSpecDialog::OnEnChangeEditDfriction()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtDynamicFriction.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setFriction( m_pShapeSet->getStaticFriction(), value );
}

void CShapeSetSpecDialog::OnEnChangeEditMass()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtMass.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setMass( value );
}

void CShapeSetSpecDialog::OnEnChangeEditLinearDamping()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_EdtLinearDampingCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setLinearDamping( value );
}

void CShapeSetSpecDialog::OnEnChangeEditAngularDamping()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_EdtAngularDampingCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setAngularDamping( value );
}

void CShapeSetSpecDialog::OnEnChangeEditSleepLinear()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_EdtSleepLinearCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setSleepLinearVelocity( value );
}



void CShapeSetSpecDialog::OnEnChangeEditSleepAngular()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_EdtSleepAngularCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setSleepAngularVelocity( value );
}


void CShapeSetSpecDialog::OnEnChangeEditSkinwidth()
{
	if( m_pShapeSet == NULL )
		return;

	char szValue[256];

	m_EdtSkinWidthCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)atof( szValue);

	m_pShapeSet->setSkinWidth( value );
}



void CShapeSetSpecDialog::OnEnChangeEditShapegroup()
{
	if( m_pShapeSet == NULL)
		return;

	char szValue[ 256];

	m_ED_ShapeGroup.GetWindowText( szValue, 256 );

	UINT16 value = (UINT16)atof( szValue);

	m_pShapeSet->setShapeGroup( value );

}

void CShapeSetSpecDialog::OnEnChangeEditTerraintype()
{
	if( m_pShapeSet == NULL)
		return;

	char szValue[ 256];

	m_Ed_TerrainType.GetWindowText( szValue, 256);

	UINT16 value = (UINT16)atof( szValue);

	if( m_pShapeSet->getTerrainCount() != m_pShapeSet->getNxShapeCount() )
	{
		m_pShapeSet->PrepareTerrainArrayForConvex();
	}

	for( INT32 i = 0; i < m_pShapeSet->getTerrainCount(); i++)
	{
		I3_PHYSIX_TERRAIN * pTerrain = m_pShapeSet->getTerrain( i);
		pTerrain->m_Terrain = value;
	}
	
}

void CShapeSetSpecDialog::OnBnClickedBtnCalclocalcenter()
{
	if( m_pShapeSet == NULL)
		return;

	NxActor * pActor = m_pShapeSet->getNxActor();
	if( pActor == NULL)
		return;

	pActor->updateMassFromShapes( 0, m_pShapeSet->getMass());
	
	AssignPhysixValue();
}

void CShapeSetSpecDialog::OnEnChangeEdThickness()
{
	if( m_pShapeSet == NULL)
		return;

	char szValue[ 256];
	I3_PHYSIX_TERRAIN * pTerrain;

	m_EdThicknessCtrl.GetWindowText( szValue, 256);

	INT32 value = (INT32)atof( szValue);

	if( m_pShapeSet->getTerrainCount() != m_pShapeSet->getNxShapeCount() )
	{
		m_pShapeSet->PrepareTerrainArrayForConvex();
	}

	if( m_chk_AllTerrainCtrl.GetCheck() == BST_CHECKED )
	{
		for( INT32 i = 0; i < m_pShapeSet->getTerrainCount(); i++)
		{
			pTerrain = m_pShapeSet->getTerrain( i);
			pTerrain->m_Thickness = value;
		}
	}
	else
	{
		pTerrain = m_pShapeSet->getTerrain( m_iCurrentShapeNumber);
		pTerrain->m_Thickness = value;
	}
}

void CShapeSetSpecDialog::OnEnChangeEdShapeNumber()
{
	if( m_pShapeSet == NULL)
		return;

	char szValue[ 256];
	m_edShapeNumber.GetWindowText( szValue, 256);

	INT32 value = (INT32)atof( szValue);

	if( value >= m_pShapeSet->getNxShapeCount())
		value = 0;

	m_iCurrentShapeNumber = value;
}

void CShapeSetSpecDialog::OnBnClickedBtnLocalMatrix()
{
	CShapeLocalMatrixDialog dlg;
	dlg.Execute( m_pShapeSet, m_iCurrentShapeNumber);
}

void CShapeSetSpecDialog::OnBnClickedBtnGlobalMatrix()
{
	CShapeGlobalMatrixDialog dlg;
	dlg.Execute( m_pShapeSet, m_iCurrentShapeNumber);
}

void CShapeSetSpecDialog::OnEnChangeEdInertiatensorx()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edInertiaTensorX.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setX( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void CShapeSetSpecDialog::OnEnChangeEdInertiatensory()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edInertiaTensorY.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setY( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void CShapeSetSpecDialog::OnEnChangeEdInertiatensorz()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edInertiaTensorZ.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setZ( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void CShapeSetSpecDialog::OnCbnSelchangeCbFormat()
{
	if( m_pShapeSet == NULL )
		return;

	if( m_CB_Format.GetCurSel() == 1)
	{
		m_pShapeSet->setFormat( i3PhysixShapeSet::FORMAT_XML);
	}
	else
	{
		m_pShapeSet->setFormat( i3PhysixShapeSet::FORMAT_BINARY);
	}
}
