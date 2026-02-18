#pragma once
#include "afxcmn.h"


// CPaneAnim dialog

class CPaneAnim : public CDialog
{
	DECLARE_DYNAMIC(CPaneAnim)

protected:
	void	UpdateAnim( INT32 idx, i3Animation * pAnim);
	void	UpdateAllAnim(void);
	INT32	FindAnim( i3Animation * pAnim);

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void	OnAddAnim( i3Animation * pAnim);
	void	OnRemoveAnim( i3Animation * pAnim);

public:
	CPaneAnim(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneAnim();

// Dialog Data
	enum { IDD = IDD_PANE_ANIM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_AnimListCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnNMDblclkAnimlist(NMHDR *pNMHDR, LRESULT *pResult);
};
