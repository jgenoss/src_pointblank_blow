#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// i3TDKDlgGeometryTool dialog

class I3_EXPORT_TDK i3TDKDlgGeometryTool : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgGeometryTool)
protected:
	i3Node *	m_pRoot = nullptr;
	INT32		m_PrimCount = 0;
	i3::vector<i3VertexArray*>		m_VAList;
	i3::vector<i3IndexArray*>		m_IAList;

protected:
	void		AddGeometry( i3GeometryAttr * pGeoAttr);
	void		UpdateAllGeometry(void);

public:
	void		Execute( i3Node * pRoot);

public:
	i3TDKDlgGeometryTool(CWnd* pParent = nullptr) : CDialog(i3TDKDlgGeometryTool::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgGeometryTool() {}

// Dialog Data
	enum { IDD = IDD_TDK_DLG_GEOMETRYTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_GeoListCtrl;
	afx_msg void OnBnClickedTdkBtnRemoveColor();
	afx_msg void OnBnClickedTdkBtnRemoveNormal();
	afx_msg void OnBnClickedTdkBtnNonindexedgeo();
	afx_msg void OnBnClickedTdkBtnIndexedgeo();
	afx_msg void OnBnClickedTdkBtnTristrip();
	afx_msg void OnBnClickedTdkBtnTrylist();
	afx_msg void OnBnClickedTdkBtnDumpGeometry();
	CEdit m_ED_TotalPrim;
	CEdit m_ED_TotalVertex;
	CEdit m_ED_VACount;
	CEdit m_ED_IACount;
};
