#pragma once
#include "afxwin.h"


class i3TDKDlg_NodeBound : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlg_NodeBound)
protected:
	i3Node *		m_pNode = nullptr;

public:
	i3TDKDlg_NodeBound(CWnd* pParent = nullptr) : CDialog(i3TDKDlg_NodeBound::IDD, pParent) {}
	virtual ~i3TDKDlg_NodeBound() {}

	bool	Execute( i3Node * pNode);

	afx_msg void OnEnChangeBoundVal();

	enum { IDD = IDD_BOUND_RESET };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
private:
	VEC3D Min, Max;

public:
	CEdit m_ED_MinX;
	CEdit m_ED_MinY;
	CEdit m_ED_MinZ;
	
	CEdit m_ED_MaxX;
	CEdit m_ED_MaxY;
	CEdit m_ED_MaxZ;

	bool BoxInitFlag = false;
};
