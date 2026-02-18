#pragma once
#include "afxcmn.h"

#include "ObjectState.h"

// CPanelState dialog

class CPanelState : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelState)
public:
	INT32	_GetSelectedState(void);

public:
	CPanelState(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelState();

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void	UpdateAllState(void);
	void	UpdateState( INT32 idx);
	void	OnAddState( CObjectState * pState);
	void	OnEditState( CObjectState * pState);
	

// Dialog Data
	enum { IDD = IDD_PANEL_STATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_StateListCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
//	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnNMRclickStatelist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickStatelist(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
