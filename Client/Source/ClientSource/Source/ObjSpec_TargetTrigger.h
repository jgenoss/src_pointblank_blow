#pragma once
#if defined(USE_EDITDLG)//CObjSpec_TargetTrigger
#include "afxwin.h"
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"
#include "i3TDK.h"

// CObjSpec_TargetTrigger dialog

class CObjSpec_TargetTrigger : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_TargetTrigger)

protected:

	void	_EndSelectTarget(void);
	void	_UpdateTarget( INT32 idx, i3GameObj * pObj);
	void	_UpdateAllTarget(void);

public:
	CObjSpec_TargetTrigger(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_TargetTrigger();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_TARGET_TRIGGER };

	virtual void	SetObject( i3ElementBase * pObj) override;

	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox	m_CB_Team;
	CListCtrl	m_ObjListCtrl;
	CButton		m_BTN_SelTarget;

	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedBtnSeltarget();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnCbnSelchangeCbTeam();
	afx_msg void OnNMDblclkObjList(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif