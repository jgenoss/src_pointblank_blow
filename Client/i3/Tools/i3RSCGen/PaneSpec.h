#pragma once
#include "afxcmn.h"
#include "RSCNode.h"
#include "afxcview.h"
#include "ListViewCtrl.h"

// CPaneSped dialog

class CPaneSpec : public CDialog
{
	DECLARE_DYNAMIC(CPaneSpec)

protected:
	RSCNode		*	m_pNode;

public:
	CPaneSpec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneSpec();

// Dialog Data
	enum { IDD = IDD_PANE_SPEC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListViewCtrl	m_ctrlList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;

	void			SetNode( RSCNode * pNode);

protected:
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam) override;

	void			_ClearList();

	//NodeÀÇ Type°ü·Ã
	void			_SetNodeInfo( RSCNode * pNode);

	void			_SetField( RSC_FIELD_INFO * pData);

	void			_EditItemValue( INT32 nItem, INT32 nCol);

public:
	afx_msg void OnNMClickListSpec(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
};
