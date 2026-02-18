#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgHeapInspector dialog

struct MEMSUMINFO
{
	const char *	m_pszSource;
	INT32 m_Line;
	UINT32		m_CRC;

	UINT32		m_Count;
	UINT32		m_Size;
};

template<> struct i3::less<MEMSUMINFO*>
{
	bool operator()(const MEMSUMINFO* p1, const MEMSUMINFO* p2) const { return p1->m_CRC < p2->m_CRC; }
	bool operator()(const MEMSUMINFO* p1, UINT32 c2) const { return p1->m_CRC < c2; }
	bool operator()(UINT32 c1, const MEMSUMINFO* p2) const { return c1 < p2->m_CRC; }
};


class CDlgHeapInspector : public CDialog
{
	DECLARE_DYNAMIC(CDlgHeapInspector)

protected:

	i3::vector_set<MEMSUMINFO*>	m_MemList;

protected:
	void	RemoveAllMemSum(void);

	void	_ScanMemory(void);
	void	_ReplaceControls( INT32 cx, INT32 cy);
	void	_Search( const char * pszStr);
	UINT32	_UpdateItem( INT32 idx, MEMSUMINFO * pInfo);
	bool	_CheckSearch( const char * pszStr, MEMSUMINFO * pInfo);

	static int CALLBACK		_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByCount( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data);

	static bool EnumBlockProc( I3MEMBLOCK * pBlock, void * pUserData);

	void	SaveToFile( const char * pszPath);

public:
	CDlgHeapInspector(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHeapInspector();

// Dialog Data
	enum { IDD = IDD_HEAP_INSPECTOR };

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
};

#endif
