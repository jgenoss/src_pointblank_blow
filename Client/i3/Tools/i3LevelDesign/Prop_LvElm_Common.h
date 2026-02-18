#pragma once
#include "afxwin.h"
#include "resource.h"

// CProp_LvElm_Common dialog

class CProp_LvElm_Common : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_Common)

protected:
	i3LevelElement3D *		m_pCurElm;
	i3MemStream		*		m_pMemStream;

protected:
	void	_UpdateResPath( const char * pszPath);
	void	_UpdateInstanceClass( const char * pszName);
	void	_UpdateInstanceClassList( i3LevelElement3D * pElm);
	void	_AssignValue( i3LevelElement3D * pElm);

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CProp_LvElm_Common(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_Common();

// Dialog Data
	enum { IDD = IDD_PROP_ELM_COMMON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ResPathCtrl;
	CEdit m_ResHashCtrl;
	CComboBox m_InstanceClassCtrl;
	CEdit m_InstanceClassHashCtrl;
	CButton m_InitStateCtrl;
	CComboBox m_DynamicStateCtrl;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnCbnSelchangeCbDynamicState();
	afx_msg void OnCbnKillfocusCbInstanceClass();
	afx_msg void OnCbnSelchangeCbInstanceClass();
	afx_msg void OnBnClickedChkInitstate();
	CComboBox m_NetworkSyncCtrl;
	afx_msg void OnCbnSelchangeCbNetworkSync();
	CEdit m_ED_Name;
	afx_msg void OnEnKillfocusEdName();
};
