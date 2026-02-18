// ShapeLocalMatrixDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ShapeLocalMatrixDialog.h"
#include ".\shapelocalmatrixdialog.h"


// CShapeLocalMatrixDialog dialog

IMPLEMENT_DYNAMIC(CShapeLocalMatrixDialog, CDialog)
CShapeLocalMatrixDialog::CShapeLocalMatrixDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeLocalMatrixDialog::IDD, pParent)
{
	m_pShapeSet		= NULL;
	m_iShapeNumber	= 0;
}

CShapeLocalMatrixDialog::~CShapeLocalMatrixDialog()
{
}

void CShapeLocalMatrixDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_00, m_edLocalMatrix_00);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_01, m_edLocalMatrix_01);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_02, m_edLocalMatrix_02);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_03, m_edLocalMatrix_03);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_10, m_edLocalMatrix_10);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_11, m_edLocalMatrix_11);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_12, m_edLocalMatrix_12);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_13, m_edLocalMatrix_13);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_20, m_edLocalMatrix_20);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_21, m_edLocalMatrix_21);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_22, m_edLocalMatrix_22);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_23, m_edLocalMatrix_23);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_30, m_edLocalMatrix_30);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_31, m_edLocalMatrix_31);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_32, m_edLocalMatrix_32);
	DDX_Control(pDX, IDC_ED_LOCAL_MAT_33, m_edLocalMatrix_33);
}


BEGIN_MESSAGE_MAP(CShapeLocalMatrixDialog, CDialog)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_00, OnEnChangeEdLocalMat00)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_01, OnEnChangeEdLocalMat01)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_02, OnEnChangeEdLocalMat02)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_03, OnEnChangeEdLocalMat03)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_10, OnEnChangeEdLocalMat10)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_11, OnEnChangeEdLocalMat11)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_12, OnEnChangeEdLocalMat12)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_13, OnEnChangeEdLocalMat13)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_20, OnEnChangeEdLocalMat20)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_21, OnEnChangeEdLocalMat21)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_22, OnEnChangeEdLocalMat22)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_23, OnEnChangeEdLocalMat23)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_30, OnEnChangeEdLocalMat30)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_31, OnEnChangeEdLocalMat31)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_32, OnEnChangeEdLocalMat32)
	ON_EN_CHANGE(IDC_ED_LOCAL_MAT_33, OnEnChangeEdLocalMat33)
END_MESSAGE_MAP()

BOOL CShapeLocalMatrixDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignMatrix();

	return TRUE;
}

// CShapeLocalMatrixDialog message handlers
void CShapeLocalMatrixDialog::AssignMatrix()
{
	MATRIX mtxTemp;
	char szTemp[ 256] = {0,};

	if( m_pShapeSet != NULL)
	{	
		NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);

		{
			i3String::ftoa( i3Matrix::Get11( &mtxTemp), szTemp);
			m_edLocalMatrix_00.SetWindowText( szTemp);
			m_edLocalMatrix_00.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get12( &mtxTemp), szTemp);
			m_edLocalMatrix_01.SetWindowText( szTemp);
			m_edLocalMatrix_01.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get13( &mtxTemp), szTemp);
			m_edLocalMatrix_02.SetWindowText( szTemp);
			m_edLocalMatrix_02.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get14( &mtxTemp), szTemp);
			m_edLocalMatrix_03.SetWindowText( szTemp);
			m_edLocalMatrix_03.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get21( &mtxTemp), szTemp);
			m_edLocalMatrix_10.SetWindowText( szTemp);
			m_edLocalMatrix_10.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get22( &mtxTemp), szTemp);
			m_edLocalMatrix_11.SetWindowText( szTemp);
			m_edLocalMatrix_11.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get23( &mtxTemp), szTemp);
			m_edLocalMatrix_12.SetWindowText( szTemp);
			m_edLocalMatrix_12.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get24( &mtxTemp), szTemp);
			m_edLocalMatrix_13.SetWindowText( szTemp);
			m_edLocalMatrix_13.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get31( &mtxTemp), szTemp);
			m_edLocalMatrix_20.SetWindowText( szTemp);
			m_edLocalMatrix_20.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get32( &mtxTemp), szTemp);
			m_edLocalMatrix_21.SetWindowText( szTemp);
			m_edLocalMatrix_21.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get33( &mtxTemp), szTemp);
			m_edLocalMatrix_22.SetWindowText( szTemp);
			m_edLocalMatrix_22.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get34( &mtxTemp), szTemp);
			m_edLocalMatrix_23.SetWindowText( szTemp);
			m_edLocalMatrix_23.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get41( &mtxTemp), szTemp);
			m_edLocalMatrix_30.SetWindowText( szTemp);
			m_edLocalMatrix_30.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get42( &mtxTemp), szTemp);
			m_edLocalMatrix_31.SetWindowText( szTemp);
			m_edLocalMatrix_31.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get43( &mtxTemp), szTemp);
			m_edLocalMatrix_32.SetWindowText( szTemp);
			m_edLocalMatrix_32.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get44( &mtxTemp), szTemp);
			m_edLocalMatrix_33.SetWindowText( szTemp);
			m_edLocalMatrix_33.EnableWindow( FALSE);
		}
	}
	else
	{
		{
			m_edLocalMatrix_00.SetWindowText( "0.0");
			m_edLocalMatrix_00.EnableWindow( FALSE);

			m_edLocalMatrix_01.SetWindowText( "0.0");
			m_edLocalMatrix_01.EnableWindow( FALSE);

			m_edLocalMatrix_02.SetWindowText( "0.0");
			m_edLocalMatrix_02.EnableWindow( FALSE);

			m_edLocalMatrix_03.SetWindowText( "0.0");
			m_edLocalMatrix_03.EnableWindow( FALSE);
		}
		
		{
			m_edLocalMatrix_10.SetWindowText( "0.0");
			m_edLocalMatrix_10.EnableWindow( FALSE);

			m_edLocalMatrix_11.SetWindowText( "0.0");
			m_edLocalMatrix_11.EnableWindow( FALSE);

			m_edLocalMatrix_12.SetWindowText( "0.0");
			m_edLocalMatrix_12.EnableWindow( FALSE);

			m_edLocalMatrix_13.SetWindowText( "0.0");
			m_edLocalMatrix_13.EnableWindow( FALSE);
		}

		{
			m_edLocalMatrix_20.SetWindowText( "0.0");
			m_edLocalMatrix_20.EnableWindow( FALSE);

			m_edLocalMatrix_21.SetWindowText( "0.0");
			m_edLocalMatrix_21.EnableWindow( FALSE);

			m_edLocalMatrix_22.SetWindowText( "0.0");
			m_edLocalMatrix_22.EnableWindow( FALSE);

			m_edLocalMatrix_23.SetWindowText( "0.0");
			m_edLocalMatrix_23.EnableWindow( FALSE);
		}

		{
			m_edLocalMatrix_30.SetWindowText( "0.0");
			m_edLocalMatrix_30.EnableWindow( FALSE);

			m_edLocalMatrix_31.SetWindowText( "0.0");
			m_edLocalMatrix_31.EnableWindow( FALSE);

			m_edLocalMatrix_32.SetWindowText( "0.0");
			m_edLocalMatrix_32.EnableWindow( FALSE);

			m_edLocalMatrix_33.SetWindowText( "0.0");
			m_edLocalMatrix_33.EnableWindow( FALSE);
		}
	}
}

BOOL CShapeLocalMatrixDialog::Execute( i3PhysixShapeSet * pShapeSet, INT32 iShapeNumber )
{
	m_pShapeSet = pShapeSet;
	m_iShapeNumber = iShapeNumber;

	if( DoModal() == IDCANCEL )
		return FALSE;

	return TRUE;
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat00()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_00.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get11( &mtxTemp) != value)
		{
			i3Matrix::SetRight( &mtxTemp, value, i3Matrix::Get12( &mtxTemp), i3Matrix::Get13( &mtxTemp), i3Matrix::Get14( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat01()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_01.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get12( &mtxTemp) != value)
		{
			i3Matrix::SetRight( &mtxTemp, i3Matrix::Get11( &mtxTemp), value, i3Matrix::Get13( &mtxTemp), i3Matrix::Get14( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat02()
{
	if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_02.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get13( &mtxTemp) != value)
		{
			i3Matrix::SetRight( &mtxTemp, i3Matrix::Get11( &mtxTemp), i3Matrix::Get12( &mtxTemp), value, i3Matrix::Get14( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat03()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_03.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get14( &mtxTemp) != value)
		{
			i3Matrix::SetRight( &mtxTemp, i3Matrix::Get11( &mtxTemp), i3Matrix::Get12( &mtxTemp), i3Matrix::Get13( &mtxTemp), value);
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat10()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_10.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get21( &mtxTemp) != value)
		{
			i3Matrix::SetUp( &mtxTemp, value, i3Matrix::Get22( &mtxTemp), i3Matrix::Get23( &mtxTemp), i3Matrix::Get24( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat11()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_11.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get22( &mtxTemp) != value)
		{
			i3Matrix::SetUp( &mtxTemp, i3Matrix::Get22( &mtxTemp), value, i3Matrix::Get23( &mtxTemp), i3Matrix::Get24( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat12()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_12.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get23( &mtxTemp) != value)
		{
			i3Matrix::SetUp( &mtxTemp, i3Matrix::Get21( &mtxTemp), i3Matrix::Get22( &mtxTemp), value, i3Matrix::Get24( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat13()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_13.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get24( &mtxTemp) != value)
		{
			i3Matrix::SetUp( &mtxTemp, i3Matrix::Get21( &mtxTemp), i3Matrix::Get22( &mtxTemp), i3Matrix::Get23( &mtxTemp), value);
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat20()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_20.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get31( &mtxTemp) != value)
		{
			i3Matrix::SetAt( &mtxTemp, value, i3Matrix::Get32( &mtxTemp), i3Matrix::Get33( &mtxTemp), i3Matrix::Get34( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat21()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_21.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get32( &mtxTemp) != value)
		{
			i3Matrix::SetAt( &mtxTemp, i3Matrix::Get31( &mtxTemp), value, i3Matrix::Get33( &mtxTemp), i3Matrix::Get34( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat22()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_22.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get33( &mtxTemp) != value)
		{
			i3Matrix::SetAt( &mtxTemp, i3Matrix::Get31( &mtxTemp), i3Matrix::Get32( &mtxTemp), value, i3Matrix::Get34( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat23()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_23.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get34( &mtxTemp) != value)
		{
			i3Matrix::SetAt( &mtxTemp, i3Matrix::Get31( &mtxTemp), i3Matrix::Get32( &mtxTemp), i3Matrix::Get33( &mtxTemp), value);
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat30()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_30.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get41( &mtxTemp) != value)
		{
			i3Matrix::SetPos( &mtxTemp, value, i3Matrix::Get42( &mtxTemp), i3Matrix::Get43( &mtxTemp), i3Matrix::Get44( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat31()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_31.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get42( &mtxTemp) != value)
		{
			i3Matrix::SetPos( &mtxTemp, i3Matrix::Get41( &mtxTemp), value, i3Matrix::Get43( &mtxTemp), i3Matrix::Get44( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat32()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_32.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get43( &mtxTemp) != value)
		{
			i3Matrix::SetPos( &mtxTemp, i3Matrix::Get41( &mtxTemp), i3Matrix::Get42( &mtxTemp), value, i3Matrix::Get44( &mtxTemp));
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}

void CShapeLocalMatrixDialog::OnEnChangeEdLocalMat33()
{
if( m_pShapeSet == NULL )
		return;

	if( !m_pShapeSet->isDynamicActor() )
		return;

	char szValue[256];

	m_edLocalMatrix_33.GetWindowText( szValue, 256 );
	REAL32 value = (REAL32)atof( szValue);

	MATRIX mtxTemp;
	NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
	if( pShape != NULL)
	{
		NxMat34 mat = pShape->getLocalPose();
		NXU::Copy( &mtxTemp, &mat);
		if( i3Matrix::Get44( &mtxTemp) != value)
		{
			i3Matrix::SetPos( &mtxTemp, i3Matrix::Get41( &mtxTemp), i3Matrix::Get42( &mtxTemp), i3Matrix::Get43( &mtxTemp), value);
			NXU::Copy( &mat, &mtxTemp);
			pShape->setLocalPose( mat);
		}
	}
}
