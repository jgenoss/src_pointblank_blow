#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgMemoryPoolInspector dialog

class CDlgMemoryPoolInspector : public CDialog
{
	DECLARE_DYNAMIC( CDlgMemoryPoolInspector)

protected:
	UINT32	m_TotalCount;
	UINT32	m_TotalSize;
	const char *	m_pszSearch;

protected:

	void	_ReplaceControls( INT32 cx, INT32 cy);
	void	_Search( const char * pszStr);
	UINT32	_UpdateItem( INT32 idx, i3FixedMemoryPool * pInfo);
	bool	_CheckSearch( const char * pszStr, i3FixedMemoryPool * pInfo);

	static int CALLBACK		_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByPageCount( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByUnit( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data);

	static bool EnumProc( i3FixedMemoryPool * pObj, void * pUserData);

	void	SaveToFile( const char * pszPath);

public:
	CDlgMemoryPoolInspector(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgMemoryPoolInspector();

// Dialog Data
	enum { IDD = IDD_POOL_INSPECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LIST;
	CEdit m_ED_Search;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK() override;
public:
	afx_msg void OnHdnItemclickListTex(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_ED_TotalCount;
	CEdit m_ED_TotalSize;
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};

#endif
