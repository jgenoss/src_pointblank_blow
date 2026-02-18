#pragma once
#include "afxwin.h"


// CShapeLocalMatrixDialog dialog

class CShapeLocalMatrixDialog : public CDialog
{
	DECLARE_DYNAMIC(CShapeLocalMatrixDialog)

public:
	CShapeLocalMatrixDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShapeLocalMatrixDialog();

// Dialog Data
	enum { IDD = IDD_DLG_LOCALMATRIX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

protected :
	i3PhysixShapeSet *		m_pShapeSet;
	INT32					m_iShapeNumber;

public:
	BOOL	Execute( i3PhysixShapeSet * pShapeSet, INT32 iShapeNumber );
	void	AssignMatrix();

	virtual BOOL OnInitDialog();

	CEdit m_edLocalMatrix_00;
	CEdit m_edLocalMatrix_01;
	CEdit m_edLocalMatrix_02;
	CEdit m_edLocalMatrix_03;
	CEdit m_edLocalMatrix_10;
	CEdit m_edLocalMatrix_11;
	CEdit m_edLocalMatrix_12;
	CEdit m_edLocalMatrix_13;
	CEdit m_edLocalMatrix_20;
	CEdit m_edLocalMatrix_21;
	CEdit m_edLocalMatrix_22;
	CEdit m_edLocalMatrix_23;
	CEdit m_edLocalMatrix_30;
	CEdit m_edLocalMatrix_31;
	CEdit m_edLocalMatrix_32;
	CEdit m_edLocalMatrix_33;
	afx_msg void OnEnChangeEdLocalMat00();
	afx_msg void OnEnChangeEdLocalMat01();
	afx_msg void OnEnChangeEdLocalMat02();
	afx_msg void OnEnChangeEdLocalMat03();
	afx_msg void OnEnChangeEdLocalMat10();
	afx_msg void OnEnChangeEdLocalMat11();
	afx_msg void OnEnChangeEdLocalMat12();
	afx_msg void OnEnChangeEdLocalMat13();
	afx_msg void OnEnChangeEdLocalMat20();
	afx_msg void OnEnChangeEdLocalMat21();
	afx_msg void OnEnChangeEdLocalMat22();
	afx_msg void OnEnChangeEdLocalMat23();
	afx_msg void OnEnChangeEdLocalMat30();
	afx_msg void OnEnChangeEdLocalMat31();
	afx_msg void OnEnChangeEdLocalMat32();
	afx_msg void OnEnChangeEdLocalMat33();
};
