#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgClassInspector dialog

struct CLASSSUMINFO
{
	i3ClassMeta *	m_pMeta;
	const char *	m_pszSource;
	INT32 m_Line;
	UINT32		m_CRC;

	UINT32		m_Count;
	UINT32		m_Size;
};

template<> struct i3::less<CLASSSUMINFO*>
{
	bool operator()(const CLASSSUMINFO* p1, const CLASSSUMINFO* p2) const { return p1->m_CRC < p2->m_CRC; }
	bool operator()(const CLASSSUMINFO* p1, UINT32 c2) const { return p1->m_CRC < c2; }
	bool operator()(UINT32 c1, const CLASSSUMINFO* p2) const { return c1 < p2->m_CRC; }
};


class CDlgClassInspector : public CDialog
{
	DECLARE_DYNAMIC(CDlgClassInspector)

protected:

	i3::vector_set<CLASSSUMINFO*>	m_ClassList;					// CRC가 고유하다고 가정..

protected:
	void	RemoveAllClassSum(void);

	void	_Scan(void);
	void	_ReplaceControls( INT32 cx, INT32 cy);
	void	_Search( const char * pszStr);
	UINT32	_UpdateItem( INT32 idx, CLASSSUMINFO * pInfo);
	bool	_CheckSearch( const char * pszStr, CLASSSUMINFO * pInfo);

	static int CALLBACK		_SortByClass( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByLocation( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByCount( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortBySize( LPARAM lParam1, LPARAM lParam2, LPARAM data);

	static bool EnumProc( i3ElementBase * pObj, void * pUserData);

	void	SaveToFile( const char * pszPath);

public:
	CDlgClassInspector(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgClassInspector();

// Dialog Data
	enum { IDD = IDD_CLASS_INSPECTOR };

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
