#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CWeaponContextDialog dialog

class CWeaponContextDialog : public CDialog
{
	DECLARE_DYNAMIC(CWeaponContextDialog)

public:
	CWeaponContextDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWeaponContextDialog();

// Dialog Data
	enum { IDD = IDD_NEWDATA_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

protected:
	CTreeCtrl m_TreeCtrl;
	CComboBox m_ComboBox_BaseAttribute;

	UINT m_uiClassType, m_uiNumber;
	i3RegKey *m_pRoot, *m_pBaseRegKey;
	i3RegKey *m_pSelectedRegistry;
	INT32 m_SelectedBaseDataIndex;
	CString m_RegistryName;

protected:
	HTREEITEM AddTree(HTREEITEM hParent, i3RegKey * pKey);
	void Rec_TreeItem(HTREEITEM hParent, i3RegKey * pKey);
	void AddBaseRegKey();

public:
	void SetClassType(UINT Value)	{	m_uiClassType = Value;	}
	UINT GetClassType() const	{	return m_uiClassType;	}

	void SetNumber(UINT Value)	{	m_uiNumber = Value;	}
	UINT GetNumber() const	{	return m_uiNumber;	}

	void SetRegRoot( i3RegKey* pRoot )	{	m_pRoot = pRoot;	}
	i3RegKey* GetSelectedRegistry() const	{	return m_pSelectedRegistry;	}

	void SetRegistryName( const CString & Name )	{	m_RegistryName = Name;	}
	CString GetRegistryName() const	{	return m_RegistryName;	}

	void SetBaseRegKey( i3RegKey* pBaseRegKey )	{	m_pBaseRegKey = pBaseRegKey;	}
	INT32 GetSelectedBaseDataIndex() const		{	return m_SelectedBaseDataIndex;	}
};

extern CString _MakeReverseChainString( i3RegKey* pRegKey, TCHAR Delimiter = '/'  );