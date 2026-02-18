// ShapeGlobalMatrixDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "ShapeGlobalMatrixDialog.h"


// CShapeGlobalMatrixDialog dialog

IMPLEMENT_DYNAMIC(CShapeGlobalMatrixDialog, CDialog)
CShapeGlobalMatrixDialog::CShapeGlobalMatrixDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CShapeGlobalMatrixDialog::IDD, pParent)
{
	m_pShapeSet = NULL;
	m_iShapeNumber = 0;
}

CShapeGlobalMatrixDialog::~CShapeGlobalMatrixDialog()
{
}

void CShapeGlobalMatrixDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_00, m_edGlobalMatrix_00);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_01, m_edGlobalMatrix_01);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_02, m_edGlobalMatrix_02);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_03, m_edGlobalMatrix_03);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_10, m_edGlobalMatrix_10);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_11, m_edGlobalMatrix_11);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_12, m_edGlobalMatrix_12);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_13, m_edGlobalMatrix_13);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_20, m_edGlobalMatrix_20);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_21, m_edGlobalMatrix_21);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_22, m_edGlobalMatrix_22);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_23, m_edGlobalMatrix_23);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_30, m_edGlobalMatrix_30);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_31, m_edGlobalMatrix_31);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_32, m_edGlobalMatrix_32);
	DDX_Control(pDX, IDC_ED_GLOBAL_MAT_33, m_edGlobalMatrix_33);
}


BEGIN_MESSAGE_MAP(CShapeGlobalMatrixDialog, CDialog)
END_MESSAGE_MAP()


// CShapeGlobalMatrixDialog message handlers
BOOL CShapeGlobalMatrixDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignMatrix();

	return TRUE;
}

void CShapeGlobalMatrixDialog::AssignMatrix()
{
	MATRIX mtxTemp;
	char szTemp[ 256] = {0,};

	if( m_pShapeSet != NULL)
	{	
		NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
		NxMat34 mat = pShape->getGlobalPose();
		NXU::Copy( &mtxTemp, &mat);

		{
			i3String::ftoa( i3Matrix::Get11( &mtxTemp), szTemp);
			m_edGlobalMatrix_00.SetWindowText( szTemp);
			m_edGlobalMatrix_00.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get12( &mtxTemp), szTemp);
			m_edGlobalMatrix_01.SetWindowText( szTemp);
			m_edGlobalMatrix_01.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get13( &mtxTemp), szTemp);
			m_edGlobalMatrix_02.SetWindowText( szTemp);
			m_edGlobalMatrix_02.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get14( &mtxTemp), szTemp);
			m_edGlobalMatrix_03.SetWindowText( szTemp);
			m_edGlobalMatrix_03.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get21( &mtxTemp), szTemp);
			m_edGlobalMatrix_10.SetWindowText( szTemp);
			m_edGlobalMatrix_10.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get22( &mtxTemp), szTemp);
			m_edGlobalMatrix_11.SetWindowText( szTemp);
			m_edGlobalMatrix_11.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get23( &mtxTemp), szTemp);
			m_edGlobalMatrix_12.SetWindowText( szTemp);
			m_edGlobalMatrix_12.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get24( &mtxTemp), szTemp);
			m_edGlobalMatrix_13.SetWindowText( szTemp);
			m_edGlobalMatrix_13.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get31( &mtxTemp), szTemp);
			m_edGlobalMatrix_20.SetWindowText( szTemp);
			m_edGlobalMatrix_20.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get32( &mtxTemp), szTemp);
			m_edGlobalMatrix_21.SetWindowText( szTemp);
			m_edGlobalMatrix_21.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get33( &mtxTemp), szTemp);
			m_edGlobalMatrix_22.SetWindowText( szTemp);
			m_edGlobalMatrix_22.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get34( &mtxTemp), szTemp);
			m_edGlobalMatrix_23.SetWindowText( szTemp);
			m_edGlobalMatrix_23.EnableWindow( FALSE);
		}

		{
			i3String::ftoa( i3Matrix::Get41( &mtxTemp), szTemp);
			m_edGlobalMatrix_30.SetWindowText( szTemp);
			m_edGlobalMatrix_30.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get42( &mtxTemp), szTemp);
			m_edGlobalMatrix_31.SetWindowText( szTemp);
			m_edGlobalMatrix_31.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get43( &mtxTemp), szTemp);
			m_edGlobalMatrix_32.SetWindowText( szTemp);
			m_edGlobalMatrix_32.EnableWindow( FALSE);

			i3String::ftoa( i3Matrix::Get44( &mtxTemp), szTemp);
			m_edGlobalMatrix_33.SetWindowText( szTemp);
			m_edGlobalMatrix_33.EnableWindow( FALSE);
		}
	}
	else
	{
		{
			m_edGlobalMatrix_00.SetWindowText( "0.0");
			m_edGlobalMatrix_00.EnableWindow( FALSE);

			m_edGlobalMatrix_01.SetWindowText( "0.0");
			m_edGlobalMatrix_01.EnableWindow( FALSE);

			m_edGlobalMatrix_02.SetWindowText( "0.0");
			m_edGlobalMatrix_02.EnableWindow( FALSE);

			m_edGlobalMatrix_03.SetWindowText( "0.0");
			m_edGlobalMatrix_03.EnableWindow( FALSE);
		}
		
		{
			m_edGlobalMatrix_10.SetWindowText( "0.0");
			m_edGlobalMatrix_10.EnableWindow( FALSE);

			m_edGlobalMatrix_11.SetWindowText( "0.0");
			m_edGlobalMatrix_11.EnableWindow( FALSE);

			m_edGlobalMatrix_12.SetWindowText( "0.0");
			m_edGlobalMatrix_12.EnableWindow( FALSE);

			m_edGlobalMatrix_13.SetWindowText( "0.0");
			m_edGlobalMatrix_13.EnableWindow( FALSE);
		}

		{
			m_edGlobalMatrix_20.SetWindowText( "0.0");
			m_edGlobalMatrix_20.EnableWindow( FALSE);

			m_edGlobalMatrix_21.SetWindowText( "0.0");
			m_edGlobalMatrix_21.EnableWindow( FALSE);

			m_edGlobalMatrix_22.SetWindowText( "0.0");
			m_edGlobalMatrix_22.EnableWindow( FALSE);

			m_edGlobalMatrix_23.SetWindowText( "0.0");
			m_edGlobalMatrix_23.EnableWindow( FALSE);
		}

		{
			m_edGlobalMatrix_30.SetWindowText( "0.0");
			m_edGlobalMatrix_30.EnableWindow( FALSE);

			m_edGlobalMatrix_31.SetWindowText( "0.0");
			m_edGlobalMatrix_31.EnableWindow( FALSE);

			m_edGlobalMatrix_32.SetWindowText( "0.0");
			m_edGlobalMatrix_32.EnableWindow( FALSE);

			m_edGlobalMatrix_33.SetWindowText( "0.0");
			m_edGlobalMatrix_33.EnableWindow( FALSE);
		}
	}
}


BOOL CShapeGlobalMatrixDialog::Execute( i3PhysixShapeSet * pShapeSet, INT32 iShapeNumber )
{
	m_pShapeSet = pShapeSet;
	m_iShapeNumber = iShapeNumber;

	if( DoModal() == IDCANCEL )
		return FALSE;

	return TRUE;
}

