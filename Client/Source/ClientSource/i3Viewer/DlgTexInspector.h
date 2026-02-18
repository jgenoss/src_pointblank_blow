#pragma once

#if defined( I3_DEBUG)
#include "resource.h"
#include "afxcmn.h"
#include "afxwin.h"

// CDlgTexInspector dialog

class CDlgTexInspector : public CDialog
{
	DECLARE_DYNAMIC(CDlgTexInspector)

protected:
	void	_ReplaceControls( INT32 cx, INT32 cy);
	void	_Search( const char * pszStr);
	UINT32	_UpdateTex( INT32 idx, i3Texture * pTex);

	UINT32	CalcBufferSize( i3Texture * pTex);

	static int CALLBACK		_SortByName( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByWidth( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByHeight( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByFormat( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByLevel( LPARAM lParam1, LPARAM lParam2, LPARAM data);
	static int CALLBACK		_SortByMemory( LPARAM lParam1, LPARAM lParam2, LPARAM data);

public:
	CDlgTexInspector(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTexInspector();

// Dialog Data
	enum { IDD = IDD_TEX_INSPECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LIST_Tex;
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
