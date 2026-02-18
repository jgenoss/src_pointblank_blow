#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// i3TDKDlg_Physix_Tool dialog

class I3_EXPORT_TDK i3TDKDlg_Physix_Tool : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlg_Physix_Tool)
protected:
	i3Node *			m_pRoot = nullptr;
	INT32				m_ActorCount = 0;
	INT32				m_ShapeCount = 0;

	enum ATTACH
	{
		ATTACH_TRANSFORM2,
		ATTACH_SHAPESETCONTAINER,
		ATTACH_BODY
	};

protected:
	void				UpdateAll(void);
	void				AddShapeSet( i3PhysixShapeSet * pShapeSet, ATTACH attach);
	void				UpdateShapeSet( INT32 idx, i3PhysixShapeSet * pShapeSet, ATTACH attach);

	void				UpdateAllShape( i3PhysixShapeSet * pShapeSet);
	void				UpdateShape( INT32 idx, NxShape * pShape);
public:
	bool				Execute( i3Node * pNode);

public:
	i3TDKDlg_Physix_Tool(CWnd* pParent = nullptr) : CDialog(i3TDKDlg_Physix_Tool::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlg_Physix_Tool() {}

// Dialog Data
	enum { IDD = IDD_TDK_DLG_PHYSIX_TOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List_ShapeSet;
	CListCtrl m_List_Shapes;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickTdkListShapeset(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_ST_Sum;
};
