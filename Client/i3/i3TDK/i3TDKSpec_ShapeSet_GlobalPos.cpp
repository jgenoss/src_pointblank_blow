// ShapeGlobalMatrixDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDKSpec_ShapeSet_GlobalPos.h"
#include "i3TDKGlobalRes.h"
#include "i3Base/string/ext/ftoa.h"
// i3TDKSpec_ShapeSet_GlobalPos dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_ShapeSet_GlobalPos, CDialog)

void i3TDKSpec_ShapeSet_GlobalPos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_00, m_edGlobalMatrix_00);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_01, m_edGlobalMatrix_01);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_02, m_edGlobalMatrix_02);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_03, m_edGlobalMatrix_03);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_10, m_edGlobalMatrix_10);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_11, m_edGlobalMatrix_11);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_12, m_edGlobalMatrix_12);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_13, m_edGlobalMatrix_13);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_20, m_edGlobalMatrix_20);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_21, m_edGlobalMatrix_21);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_22, m_edGlobalMatrix_22);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_23, m_edGlobalMatrix_23);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_30, m_edGlobalMatrix_30);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_31, m_edGlobalMatrix_31);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_32, m_edGlobalMatrix_32);
	DDX_Control(pDX, IDC_TDK_ED_GLOBAL_MAT_33, m_edGlobalMatrix_33);
}


BEGIN_MESSAGE_MAP(i3TDKSpec_ShapeSet_GlobalPos, CDialog)
END_MESSAGE_MAP()


// i3TDKSpec_ShapeSet_GlobalPos message handlers
BOOL i3TDKSpec_ShapeSet_GlobalPos::OnInitDialog()
{
	CDialog::OnInitDialog();

	AssignMatrix();

	return TRUE;
}

void i3TDKSpec_ShapeSet_GlobalPos::AssignMatrix()
{
	MATRIX mtxTemp;
	TCHAR szTemp[ 256] = {0,};

	if( m_pShapeSet != nullptr)
	{	
		NxShape * pShape = m_pShapeSet->getNxShape( m_iShapeNumber);
		NxMat34 mat = pShape->getGlobalPose();
		NXU::Copy( &mtxTemp, &mat);

		{
			i3::ftoa( i3Matrix::Get11( &mtxTemp), szTemp);
			m_edGlobalMatrix_00.SetWindowText( szTemp);
			m_edGlobalMatrix_00.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get12( &mtxTemp), szTemp);
			m_edGlobalMatrix_01.SetWindowText( szTemp);
			m_edGlobalMatrix_01.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get13( &mtxTemp), szTemp);
			m_edGlobalMatrix_02.SetWindowText( szTemp);
			m_edGlobalMatrix_02.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get14( &mtxTemp), szTemp);
			m_edGlobalMatrix_03.SetWindowText( szTemp);
			m_edGlobalMatrix_03.EnableWindow( FALSE);
		}

		{
			i3::ftoa( i3Matrix::Get21( &mtxTemp), szTemp);
			m_edGlobalMatrix_10.SetWindowText( szTemp);
			m_edGlobalMatrix_10.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get22( &mtxTemp), szTemp);
			m_edGlobalMatrix_11.SetWindowText( szTemp);
			m_edGlobalMatrix_11.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get23( &mtxTemp), szTemp);
			m_edGlobalMatrix_12.SetWindowText( szTemp);
			m_edGlobalMatrix_12.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get24( &mtxTemp), szTemp);
			m_edGlobalMatrix_13.SetWindowText( szTemp);
			m_edGlobalMatrix_13.EnableWindow( FALSE);
		}

		{
			i3::ftoa( i3Matrix::Get31( &mtxTemp), szTemp);
			m_edGlobalMatrix_20.SetWindowText( szTemp);
			m_edGlobalMatrix_20.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get32( &mtxTemp), szTemp);
			m_edGlobalMatrix_21.SetWindowText( szTemp);
			m_edGlobalMatrix_21.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get33( &mtxTemp), szTemp);
			m_edGlobalMatrix_22.SetWindowText( szTemp);
			m_edGlobalMatrix_22.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get34( &mtxTemp), szTemp);
			m_edGlobalMatrix_23.SetWindowText( szTemp);
			m_edGlobalMatrix_23.EnableWindow( FALSE);
		}

		{
			i3::ftoa( i3Matrix::Get41( &mtxTemp), szTemp);
			m_edGlobalMatrix_30.SetWindowText( szTemp);
			m_edGlobalMatrix_30.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get42( &mtxTemp), szTemp);
			m_edGlobalMatrix_31.SetWindowText( szTemp);
			m_edGlobalMatrix_31.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get43( &mtxTemp), szTemp);
			m_edGlobalMatrix_32.SetWindowText( szTemp);
			m_edGlobalMatrix_32.EnableWindow( FALSE);

			i3::ftoa( i3Matrix::Get44( &mtxTemp), szTemp);
			m_edGlobalMatrix_33.SetWindowText( szTemp);
			m_edGlobalMatrix_33.EnableWindow( FALSE);
		}
	}
	else
	{
		{
			m_edGlobalMatrix_00.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_00.EnableWindow( FALSE);

			m_edGlobalMatrix_01.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_01.EnableWindow( FALSE);

			m_edGlobalMatrix_02.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_02.EnableWindow( FALSE);

			m_edGlobalMatrix_03.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_03.EnableWindow( FALSE);
		}
		
		{
			m_edGlobalMatrix_10.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_10.EnableWindow( FALSE);

			m_edGlobalMatrix_11.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_11.EnableWindow( FALSE);

			m_edGlobalMatrix_12.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_12.EnableWindow( FALSE);

			m_edGlobalMatrix_13.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_13.EnableWindow( FALSE);
		}

		{
			m_edGlobalMatrix_20.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_20.EnableWindow( FALSE);

			m_edGlobalMatrix_21.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_21.EnableWindow( FALSE);

			m_edGlobalMatrix_22.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_22.EnableWindow( FALSE);

			m_edGlobalMatrix_23.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_23.EnableWindow( FALSE);
		}

		{
			m_edGlobalMatrix_30.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_30.EnableWindow( FALSE);

			m_edGlobalMatrix_31.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_31.EnableWindow( FALSE);

			m_edGlobalMatrix_32.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_32.EnableWindow( FALSE);

			m_edGlobalMatrix_33.SetWindowText( _T("0.0") );
			m_edGlobalMatrix_33.EnableWindow( FALSE);
		}
	}
}


BOOL i3TDKSpec_ShapeSet_GlobalPos::Execute( i3PhysixShapeSet * pShapeSet, INT32 iShapeNumber )
{
	m_pShapeSet = pShapeSet;
	m_iShapeNumber = iShapeNumber;

	i3TDK::SetResInstance();
	INT32 rv = DoModal();
	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL )
		return FALSE;

	return TRUE;
}

