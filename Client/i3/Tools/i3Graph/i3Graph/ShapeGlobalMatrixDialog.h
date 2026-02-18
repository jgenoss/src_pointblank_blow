#pragma once
#include "afxwin.h"


// CShapeGlobalMatrixDialog dialog

class CShapeGlobalMatrixDialog : public CDialog
{
	DECLARE_DYNAMIC(CShapeGlobalMatrixDialog)

public:
	CShapeGlobalMatrixDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShapeGlobalMatrixDialog();

// Dialog Data
	enum { IDD = IDD_DLG_GLOBALMATRIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	i3PhysixShapeSet *	m_pShapeSet;
	INT32				m_iShapeNumber;

	CEdit m_edGlobalMatrix_00;

	void	AssignMatrix();
	BOOL	Execute( i3PhysixShapeSet * pShapeSet, INT32 iShapeNumber );

	virtual BOOL OnInitDialog();
	CEdit m_edGlobalMatrix_01;
	CEdit m_edGlobalMatrix_02;
	CEdit m_edGlobalMatrix_03;
	CEdit m_edGlobalMatrix_10;
	CEdit m_edGlobalMatrix_11;
	CEdit m_edGlobalMatrix_12;
	CEdit m_edGlobalMatrix_13;
	CEdit m_edGlobalMatrix_20;
	CEdit m_edGlobalMatrix_21;
	CEdit m_edGlobalMatrix_22;
	CEdit m_edGlobalMatrix_23;
	CEdit m_edGlobalMatrix_30;
	CEdit m_edGlobalMatrix_31;
	CEdit m_edGlobalMatrix_32;
	CEdit m_edGlobalMatrix_33;
};
