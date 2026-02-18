#pragma once
#include "afxcmn.h"


// CGeometryToolDialog dialog

class CGeometryToolDialog : public CDialog
{
	DECLARE_DYNAMIC(CGeometryToolDialog)
protected:
	i3Node *	m_pRoot;

protected:
	void		AddGeometry( i3GeometryAttr * pGeoAttr);
	void		UpdateAllGeometry(void);

public:
	void		Execute( i3Node * pRoot);

public:
	CGeometryToolDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeometryToolDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_GEOMETRYTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_GeoListCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnSelectall();
	afx_msg void OnBnClickedBtnClearall();
	afx_msg void OnBnClickedBtnSelinverse();
	afx_msg void OnBnClickedBtnMakevertexcolorgrayscale();
	afx_msg void OnBnClickedBtnRemovenormal();
	afx_msg void OnBnClickedBtnRemovevertexcolor();
	afx_msg void OnBnClickedBtnMakeindexedgeometry();
	afx_msg void OnBnClickedBtnMakeTriangleStrip();
	afx_msg void OnBnClickedBtnMakenonindexedgeometry();
};
