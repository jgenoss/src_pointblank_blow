#pragma once
#if defined(USE_EDITDLG)//CObjSpec_TargetObject
#include "afxwin.h"
#include "afxcmn.h"
#include "i3Framework.h"
#include "resource.h"

// CObjSpec_TargetObject dialog

class CObjSpec_TargetObject : public i3GameObjPropertyDialog
{
	DECLARE_DYNAMIC(CObjSpec_TargetObject)

protected:

public:
	CObjSpec_TargetObject(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CObjSpec_TargetObject();

// Dialog Data
	enum { IDD = IDD_PB_SPEC_TARGET_OBJECT };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox	m_CB_Type;
	CComboBox	m_CB_Appear;
	CEdit		m_ED_OffsetTime;
	CEdit		m_ED_StartTime;
	CEdit		m_ED_KeepTime;
	CEdit		m_ED_EndTime;
	
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnCbnSelchangeType();
	afx_msg void OnCbnSelchangeAppear();
	afx_msg void OnEnKillfocusEdOffset();
	afx_msg void OnEnKillfocusEdStarttime();
	afx_msg void OnEnKillfocusEdKeeptime();
	afx_msg void OnEnKillfocusEdDisappeartime();
};

#endif