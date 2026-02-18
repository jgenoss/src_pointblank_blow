// ShapeSetSpecDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSpec_ShapeSet.h"
#include "i3TDKSpec_ShapeSet_GlobalPos.h"
#include "i3TDKSpec_ShapeSet_LocalPos.h"
#include "i3TDKGlobalRes.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/itoa.h"
#include "i3Base/string/ext/atof.h"

// i3TDKSpec_ShapeSet dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_ShapeSet, CDialog)


void i3TDKSpec_ShapeSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_EDIT_SHAPENAME, m_EdtShapeName);
	DDX_Control(pDX, IDC_TDK_EDIT_SHAPETYPE, m_EdtShapeType);
	DDX_Control(pDX, IDC_TDK_EDIT_RESTITUTION, m_EdtRestitution);
	DDX_Control(pDX, IDC_TDK_EDIT_SFRICTION, m_EdtStaticFriction);
	DDX_Control(pDX, IDC_TDK_EDIT_DFRICTION, m_EdtDynamicFriction);
	DDX_Control(pDX, IDC_TDK_EDIT_MASS, m_EdtMass);
	DDX_Control(pDX, IDC_TDK_EDIT_LINEAR_DAMPING, m_EdtLinearDampingCtrl);
	DDX_Control(pDX, IDC_TDK_EDIT_ANGULAR_DAMPING, m_EdtAngularDampingCtrl);
	DDX_Control(pDX, IDC_TDK_EDIT_SLEEP_LINEAR, m_EdtSleepLinearCtrl);
	DDX_Control(pDX, IDC_TDK_EDIT_SLEEP_ANGULAR, m_EdtSleepAngularCtrl);
	DDX_Control(pDX, IDC_TDK_EDIT_SKINWIDTH, m_EdtSkinWidthCtrl);
	DDX_Control(pDX, IDC_TDK_CB_TERRAINTYPE, m_CB_TerrainType);
	DDX_Control(pDX, IDC_TDK_EDIT_TERRAINCOUNT, m_Ed_TerrainCount);
	DDX_Control(pDX, IDC_TDK_EDIT_SHAPEGROUP, m_ED_ShapeGroup);
	DDX_Control(pDX, IDC_TDK_ED_LOCALPOSEX, m_EdShapeLocalPoseXCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LOCALPOSEY, m_EdShapeLocalPoseYCtrl);
	DDX_Control(pDX, IDC_TDK_ED_LOCALPOSEZ, m_EdShapeLocalPoseZCtrl);
	DDX_Control(pDX, IDC_TDK_ED_THICKNESS, m_EdThicknessCtrl);
	DDX_Control(pDX, IDC_TDK_ED_SHAPE_NUMBER, m_edShapeNumber);
	DDX_Control(pDX, IDC_TDK_ED_SHAPE_COUNT, m_edShapeCount);
	DDX_Control(pDX, IDC_TDK_ED_INERTIATENSORX, m_edInertiaTensorX);
	DDX_Control(pDX, IDC_TDK_ED_INERTIATENSORY, m_edInertiaTensorY);
	DDX_Control(pDX, IDC_TDK_ED_INERTIATENSORZ, m_edInertiaTensorZ);
	DDX_Control(pDX, IDC_TDK_CHK_ALLTERRAIN, m_chk_AllTerrainCtrl);
	DDX_Control(pDX, IDC_TDK_CB_FORMAT, m_CB_Format);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_ShapeSet, CDialog)
	ON_EN_CHANGE(IDC_TDK_EDIT_SHAPENAME, OnEnChangeEditShapename)
	ON_EN_CHANGE(IDC_TDK_EDIT_RESTITUTION, OnEnChangeEditRestitution)
	ON_EN_CHANGE(IDC_TDK_EDIT_SFRICTION, OnEnChangeEditSfriction)
	ON_EN_CHANGE(IDC_TDK_EDIT_DFRICTION, OnEnChangeEditDfriction)
	ON_EN_CHANGE(IDC_TDK_EDIT_MASS, OnEnChangeEditMass)
	ON_EN_CHANGE(IDC_TDK_EDIT_LINEAR_DAMPING, OnEnChangeEditLinearDamping)
	ON_EN_CHANGE(IDC_TDK_EDIT_ANGULAR_DAMPING, OnEnChangeEditAngularDamping)
	ON_EN_CHANGE(IDC_TDK_EDIT_SLEEP_LINEAR, OnEnChangeEditSleepLinear)
	ON_EN_CHANGE(IDC_TDK_EDIT_SLEEP_ANGULAR, OnEnChangeEditSleepAngular)
	ON_EN_CHANGE(IDC_TDK_EDIT_SKINWIDTH, OnEnChangeEditSkinwidth)
	ON_EN_CHANGE(IDC_TDK_EDIT_SHAPEGROUP, OnEnChangeEditShapegroup)
	ON_EN_CHANGE(IDC_TDK_CB_TERRAINTYPE, OnEnChangeEditTerraintype)
	ON_BN_CLICKED(IDC_TDK_BTN_CALCLOCALCENTER, OnBnClickedBtnCalclocalcenter)
	ON_EN_CHANGE(IDC_TDK_ED_THICKNESS, OnEnChangeEdThickness)
	ON_EN_CHANGE(IDC_TDK_ED_SHAPE_NUMBER, OnEnChangeEdShapeNumber)
	ON_BN_CLICKED(IDC_TDK_BTN_LOCAL_MATRIX, OnBnClickedBtnLocalMatrix)
	ON_BN_CLICKED(IDC_TDK_BTN_GLOBAL_MATRIX, OnBnClickedBtnGlobalMatrix)
	ON_EN_CHANGE(IDC_TDK_ED_INERTIATENSORX, OnEnChangeEdInertiatensorx)
	ON_EN_CHANGE(IDC_TDK_ED_INERTIATENSORY, OnEnChangeEdInertiatensory)
	ON_EN_CHANGE(IDC_TDK_ED_INERTIATENSORZ, OnEnChangeEdInertiatensorz)
	ON_CBN_SELCHANGE(IDC_TDK_CB_FORMAT, OnCbnSelchangeCbFormat)
END_MESSAGE_MAP()


// i3TDKSpec_ShapeSet message handlers
BOOL i3TDKSpec_ShapeSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignPhysixValue();

	return TRUE;
}


BOOL i3TDKSpec_ShapeSet::Execute( i3PhysixShapeSet * pShapeSet )
{
	INT32 rv;

	m_pShapeSet = pShapeSet;

	i3TDK::SetResInstance();

	rv = DoModal();

	i3TDK::RestoreResInstance();
	
	if( rv == IDCANCEL )
		return FALSE;

	return TRUE;
}

void i3TDKSpec_ShapeSet::AssignPhysixValue()
{
	BOOL bCheckValid = TRUE;
	TCHAR szTemp[ 256]={0,};

	if( m_pShapeSet != nullptr )
	{
		i3::itoa( m_iCurrentShapeNumber, szTemp, _countof(szTemp));
		m_edShapeNumber.SetWindowText( szTemp);

		i3::itoa( m_pShapeSet->getNxShapeCount(), szTemp, _countof(szTemp));
		m_edShapeCount.SetWindowText( szTemp);
		m_edShapeCount.EnableWindow( FALSE);

		// Format
		switch( m_pShapeSet->getFormat())
		{
			case i3PhysixShapeSet::FORMAT_XML :		m_CB_Format.SetCurSel( 1);	break;
			default :								m_CB_Format.SetCurSel( 0);	break;
		}

		NxShape * pShape = m_pShapeSet->getNxShape( m_iCurrentShapeNumber);

		if( pShape != nullptr )
		{
			if( pShape->getName() != nullptr)		
			{
#ifdef _UNICODE
				i3::stack_wstring name;		i3::mb_to_utf16(pShape->getName(), name);
				m_EdtShapeName.SetWindowText(name.c_str());
#else
				m_EdtShapeName.SetWindowText( pShape->getName() );
#endif
			}
			else if( m_pShapeSet->hasName())
			{
#ifdef _UNICODE
				i3::stack_wstring name;		i3::mb_to_utf16(m_pShapeSet->GetName(), name);
				m_EdtShapeName.SetWindowText(name.c_str());
#else
				m_EdtShapeName.SetWindowText( m_pShapeSet->GetName() );
#endif
			}

			i3::itoa( m_pShapeSet->getShapeGroup(), szTemp, _countof(szTemp));
			m_ED_ShapeGroup.SetWindowText( szTemp);

			switch( pShape->getType() )
			{
			case NX_SHAPE_PLANE :		i3::generic_string_copy( szTemp, _T("Plane") );	break;
			case NX_SHAPE_SPHERE :		i3::generic_string_copy( szTemp, _T("Sphere") );	break;
			case NX_SHAPE_BOX :			i3::generic_string_copy( szTemp, _T("Box") );	break;
			case NX_SHAPE_CAPSULE :		i3::generic_string_copy( szTemp, _T("Capsule") );	break;
			case NX_SHAPE_WHEEL :		i3::generic_string_copy( szTemp, _T("Wheel") );	break;
			case NX_SHAPE_CONVEX :		i3::generic_string_copy( szTemp, _T("Convex") );	break;
			case NX_SHAPE_MESH :		i3::generic_string_copy( szTemp, _T("TriMesh") );	break;
			case NX_SHAPE_HEIGHTFIELD :	i3::generic_string_copy( szTemp, _T("HeightField") );	break;
			default :	i3::generic_string_copy( szTemp, _T("Unknown") );	break;
			}
			m_EdtShapeType.EnableWindow();
			m_EdtShapeType.SetWindowText( szTemp);
			m_EdtShapeType.EnableWindow( FALSE );

			i3::ftoa( m_pShapeSet->getSkinWidth( 0 ), szTemp);
			m_EdtSkinWidthCtrl.SetWindowText( szTemp);

			i3::ftoa( m_pShapeSet->getRestitution(), szTemp);
			m_EdtRestitution.SetWindowText( szTemp );

			i3::ftoa( m_pShapeSet->getStaticFriction(), szTemp);
			m_EdtStaticFriction.SetWindowText( szTemp );

			i3::ftoa( m_pShapeSet->getDynamicFriction(), szTemp);
			m_EdtDynamicFriction.SetWindowText( szTemp );

			if( m_pShapeSet->isDynamicActor() )
			{
				i3::ftoa( m_pShapeSet->getLinearDamping(), szTemp );
				m_EdtLinearDampingCtrl.SetWindowText( szTemp);

				i3::ftoa( m_pShapeSet->getAngularDamping(), szTemp );
				m_EdtAngularDampingCtrl.SetWindowText( szTemp);

				i3::ftoa( m_pShapeSet->getSleepLinearVelocity(), szTemp );
				m_EdtSleepLinearCtrl.SetWindowText( szTemp);

				i3::ftoa( m_pShapeSet->getSleepAngularVelocity(), szTemp );
				m_EdtSleepAngularCtrl.SetWindowText( szTemp);

				// Mass Center position
				VEC3D vec;
				m_pShapeSet->getMassLocalPose( &vec);
				i3::ftoa( getX( &vec), szTemp );
				m_EdShapeLocalPoseXCtrl.SetWindowText( szTemp);

				i3::ftoa( getY( &vec), szTemp );
				m_EdShapeLocalPoseYCtrl.SetWindowText( szTemp);

				i3::ftoa( getZ( &vec), szTemp );
				m_EdShapeLocalPoseZCtrl.SetWindowText( szTemp);

				// mass
				i3::ftoa( m_pShapeSet->getMass(), szTemp);
				m_EdtMass.SetWindowText( szTemp );

				// inertiatensor
				m_pShapeSet->getInertiaTensor( &vec);
				i3::ftoa( getX( &vec), szTemp);
				m_edInertiaTensorX.SetWindowText( szTemp);
				i3::ftoa( getY( &vec), szTemp);
				m_edInertiaTensorY.SetWindowText( szTemp);
				i3::ftoa( getZ( &vec), szTemp);
				m_edInertiaTensorZ.SetWindowText( szTemp);
			}
			else
			{
				m_EdtLinearDampingCtrl.SetWindowText( _T("0.0") );
				m_EdtLinearDampingCtrl.EnableWindow( FALSE );
				m_EdtAngularDampingCtrl.SetWindowText( _T("0.0") );
				m_EdtAngularDampingCtrl.EnableWindow( FALSE);
				m_EdtSleepLinearCtrl.SetWindowText( _T("0.0") );
				m_EdtSleepLinearCtrl.EnableWindow( FALSE);
				m_EdtSleepAngularCtrl.SetWindowText( _T("0.0") );
				m_EdtSleepAngularCtrl.EnableWindow( FALSE);
				
				m_EdShapeLocalPoseXCtrl.SetWindowText( _T("0.0") );
				m_EdShapeLocalPoseXCtrl.EnableWindow( FALSE);

				m_EdShapeLocalPoseYCtrl.SetWindowText( _T("0.0") );
				m_EdShapeLocalPoseYCtrl.EnableWindow( FALSE);
				
				m_EdShapeLocalPoseZCtrl.SetWindowText( _T("0.0") );
				m_EdShapeLocalPoseZCtrl.EnableWindow( FALSE);
				
				m_EdtMass.SetWindowText( _T("0.0") );
				m_EdtMass.EnableWindow( FALSE);

				m_edInertiaTensorX.SetWindowText( _T("0.0") );
				m_edInertiaTensorX.EnableWindow( FALSE);
				m_edInertiaTensorY.SetWindowText( _T("0.0") );
				m_edInertiaTensorY.EnableWindow( FALSE);
				m_edInertiaTensorZ.SetWindowText( _T("0.0") );
				m_edInertiaTensorZ.EnableWindow( FALSE);

			}

			if( m_pShapeSet->getTerrainCount() > 0 )
			{
				i3::itoa( m_pShapeSet->getTerrain( 0)->m_Terrain, szTemp, _countof(szTemp));
				m_CB_TerrainType.SetWindowText( szTemp);

				i3::itoa( m_pShapeSet->getTerrain( 0)->m_Thickness, szTemp, _countof(szTemp));
				m_EdThicknessCtrl.SetWindowText( szTemp);
			}

			i3::itoa( m_pShapeSet->getTerrainCount(), szTemp, _countof(szTemp) );
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
		m_EdtShapeName.SetWindowText( _T("(none)") );
		m_EdtShapeType.EnableWindow();
		m_EdtShapeType.SetWindowText( _T("(undefined)") );
		m_EdtShapeType.EnableWindow( FALSE );
		m_EdtRestitution.SetWindowText( _T("0.0") );
		m_EdtStaticFriction.SetWindowText( _T("0.0" ) );
		m_EdtDynamicFriction.SetWindowText( _T("0.0" ) );
		m_EdtMass.SetWindowText( _T("0.0" ) );
		m_EdtSkinWidthCtrl.SetWindowText( _T("0.0") );

		m_EdtLinearDampingCtrl.SetWindowText( _T("0.0") );
		m_EdtLinearDampingCtrl.EnableWindow( FALSE );
		m_EdtAngularDampingCtrl.SetWindowText( _T("0.0") );
		m_EdtAngularDampingCtrl.EnableWindow( FALSE);
		m_EdtSleepLinearCtrl.SetWindowText( _T("0.0") );
		m_EdtSleepLinearCtrl.EnableWindow( FALSE);
		m_EdtSleepAngularCtrl.SetWindowText( _T("0.0") );
		m_EdtSleepAngularCtrl.EnableWindow( FALSE);
		m_CB_TerrainType.SetWindowText( _T("0" ) );
		m_Ed_TerrainCount.SetWindowText( _T("0" ) );
		m_Ed_TerrainCount.EnableWindow( FALSE);
		m_EdThicknessCtrl.SetWindowText( _T("0.0") );
		m_ED_ShapeGroup.SetWindowText( _T("0") );

		m_EdShapeLocalPoseXCtrl.SetWindowText( _T("0.0") );
		m_EdShapeLocalPoseXCtrl.EnableWindow( FALSE);
		m_EdShapeLocalPoseYCtrl.SetWindowText( _T("0.0") );
		m_EdShapeLocalPoseYCtrl.EnableWindow( FALSE);
		m_EdShapeLocalPoseZCtrl.SetWindowText( _T("0.0") );
		m_EdShapeLocalPoseZCtrl.EnableWindow( FALSE);

		m_EdtMass.SetWindowText( _T("0.0" ) );
		m_EdtMass.EnableWindow( FALSE);

		m_edInertiaTensorX.SetWindowText( _T("0.0") );
		m_edInertiaTensorX.EnableWindow( FALSE);
		m_edInertiaTensorY.SetWindowText( _T("0.0") );
		m_edInertiaTensorY.EnableWindow( FALSE);
		m_edInertiaTensorZ.SetWindowText( _T("0.0") );
		m_edInertiaTensorZ.EnableWindow( FALSE);
	}
}

void i3TDKSpec_ShapeSet::OnEnChangeEditShapename()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtShapeName.GetWindowText( szValue, 256 );

	if( i3::generic_string_size( szValue) )
	{
#ifdef _UNICODE
		i3::stack_string strValue;	i3::utf16_to_mb(szValue, strValue);
		m_pShapeSet->SetName(strValue);
#else
		m_pShapeSet->SetName( szValue );
#endif
	}
}

void i3TDKSpec_ShapeSet::OnEnChangeEditRestitution()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtRestitution.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setRestitution( value );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditSfriction()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtStaticFriction.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setFriction( value, m_pShapeSet->getDynamicFriction() );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditDfriction()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtDynamicFriction.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setFriction( m_pShapeSet->getStaticFriction(), value );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditMass()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtMass.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setMass( value );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditLinearDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_EdtLinearDampingCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setLinearDamping( value );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditAngularDamping()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_EdtAngularDampingCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setAngularDamping( value );
}

void i3TDKSpec_ShapeSet::OnEnChangeEditSleepLinear()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_EdtSleepLinearCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setSleepLinearVelocity( value );
}



void i3TDKSpec_ShapeSet::OnEnChangeEditSleepAngular()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_EdtSleepAngularCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setSleepAngularVelocity( value );
}


void i3TDKSpec_ShapeSet::OnEnChangeEditSkinwidth()
{
	if( m_pShapeSet == nullptr )
		return;

	TCHAR szValue[256];

	m_EdtSkinWidthCtrl.GetWindowText( szValue, 256 );

	REAL32 value = (REAL32)i3::atof( szValue);

	m_pShapeSet->setSkinWidth( value );
}



void i3TDKSpec_ShapeSet::OnEnChangeEditShapegroup()
{
	if( m_pShapeSet == nullptr)
		return;

	TCHAR szValue[ 256];

	m_ED_ShapeGroup.GetWindowText( szValue, 256 );

	UINT16 value = (UINT16)i3::atof( szValue);

	m_pShapeSet->setShapeGroup( value );

}

void i3TDKSpec_ShapeSet::OnEnChangeEditTerraintype()
{
	if( m_pShapeSet == nullptr)
		return;

	TCHAR szValue[ 256];

	m_CB_TerrainType.GetWindowText( szValue, 256);

	UINT16 value = (UINT16)i3::atof( szValue);

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

void i3TDKSpec_ShapeSet::OnBnClickedBtnCalclocalcenter()
{
	if( m_pShapeSet == nullptr)
		return;

	NxActor * pActor = m_pShapeSet->getNxActor();
	if( pActor == nullptr)
		return;

	pActor->updateMassFromShapes( 0, m_pShapeSet->getMass());
	
	AssignPhysixValue();
}

void i3TDKSpec_ShapeSet::OnEnChangeEdThickness()
{
	if( m_pShapeSet == nullptr)
		return;

	TCHAR szValue[ 256];
	I3_PHYSIX_TERRAIN * pTerrain;

	m_EdThicknessCtrl.GetWindowText( szValue, _countof(szValue));

	INT32 value = (INT32)i3::atof( szValue);

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

void i3TDKSpec_ShapeSet::OnEnChangeEdShapeNumber()
{
	if( m_pShapeSet == nullptr)
		return;

	TCHAR szValue[ 256];
	m_edShapeNumber.GetWindowText( szValue, _countof(szValue));

	INT32 value = (INT32)i3::atof( szValue);

	if( value >= m_pShapeSet->getNxShapeCount())
		value = 0;

	m_iCurrentShapeNumber = value;
}

void i3TDKSpec_ShapeSet::OnBnClickedBtnLocalMatrix()
{
	i3TDKSpec_ShapeSet_LocalPos dlg;

	dlg.Execute( m_pShapeSet, m_iCurrentShapeNumber);
}

void i3TDKSpec_ShapeSet::OnBnClickedBtnGlobalMatrix()
{
	i3TDKSpec_ShapeSet_GlobalPos dlg;

	dlg.Execute( m_pShapeSet, m_iCurrentShapeNumber);
}

void i3TDKSpec_ShapeSet::OnEnChangeEdInertiatensorx()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_edInertiaTensorX.GetWindowText( szValue, _countof(szValue) );
	REAL32 value = (REAL32)i3::atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setX( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void i3TDKSpec_ShapeSet::OnEnChangeEdInertiatensory()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_edInertiaTensorY.GetWindowText( szValue, _countof(szValue) );
	REAL32 value = (REAL32)i3::atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setY( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void i3TDKSpec_ShapeSet::OnEnChangeEdInertiatensorz()
{
	if( m_pShapeSet == nullptr )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	TCHAR szValue[256];

	m_edInertiaTensorZ.GetWindowText( szValue, _countof(szValue) );
	REAL32 value = (REAL32)i3::atof( szValue);

	VEC3D vec;
	m_pShapeSet->getInertiaTensor( &vec);
	setZ( &vec, value);
	m_pShapeSet->setInertiaTensor( &vec);
}

void i3TDKSpec_ShapeSet::OnCbnSelchangeCbFormat()
{
	if( m_pShapeSet == nullptr )
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
