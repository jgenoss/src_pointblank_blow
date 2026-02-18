#pragma once
#include "afxwin.h"


// i3TDKSpec_ShapeSet_GlobalPos dialog

class i3TDKSpec_ShapeSet_GlobalPos : public CDialog
{
	DECLARE_DYNAMIC(i3TDKSpec_ShapeSet_GlobalPos)

public:
	i3TDKSpec_ShapeSet_GlobalPos(CWnd* pParent = nullptr) : CDialog(i3TDKSpec_ShapeSet_GlobalPos::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_ShapeSet_GlobalPos() {}

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_SHAPESET_GLOBALPOS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	i3PhysixShapeSet *	m_pShapeSet = nullptr;
	INT32				m_iShapeNumber = 0;

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
