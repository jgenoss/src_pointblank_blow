#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgVBInspector dialog

class CDlgVBInspector : public CDialog
{
	DECLARE_DYNAMIC(CDlgVBInspector)

protected:
	void	_ReplaceControls( INT32 cx, INT32 cy);
	void	_Search( const char * pszStr);
	UINT32	_UpdateItem( INT32 idx, i3VertexArray * pTex);

	void	_FormatName( i3VertexArray * pVA, char * pszStr);

	UINT32	CalcBufferSize( i3VertexArray * pTex);

	bool	_CheckSearch( const char * pszStr, i3VertexArray * pVA);

	static int CALLBACK		_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByPath( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortBySrc( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByFormat( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByMemory( LPARAM lParam1, LPARAM lParam2, LPARAM data);

public:
	CDlgVBInspector(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVBInspector();

// Dialog Data
	enum { IDD = IDD_VB_INSPECTOR };

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
};

#endif
