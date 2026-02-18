#pragma once
#include "afxwin.h"

#include "i3Base/i3StringListBuffer.h"

// RSCDialog dialog

class RSCDialog : public CDialog
{
	DECLARE_DYNAMIC(RSCDialog)

public:
	RSCDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~RSCDialog();

	virtual BOOL OnInitDialog() override;

// Dialog Data
	enum { IDD = IDD_DIALOG_RSC };

	CListBox m_RSCListBox;
	CButton m_BnRscAdd;
	CButton m_BnRscClose;
	CButton m_BnRscDelete;
	CButton m_BnRscEdit;

protected:
	i3::vector<i3::rc_string>	m_FileList;

	char *			m_pszWorkDir;

	i3::vector<i3::rc_string>* m_pCopyList;

public:
	
	void	ClearRSC( void);
	void	AddRSC( const char * pszPath);
	void	RemoveRSC( INT32 idx);
	void	RemoveRSC( char * pszPath);
	INT32	GetRSCCount( void)				{	return (INT32)m_FileList.size();				}
	INT32	GetRSCSize( void)				{	return GetStringListBufferSize(m_FileList); }
	void	CopyToRSC( char * pBuf, INT32 size)	{	CopyFromStringListToBuffer(m_FileList, pBuf, size); }
	const char *	GetRSC( INT32 idx)	const	{	return m_FileList[idx].c_str();	}

	void	SetCopyList( i3::vector<i3::rc_string>* pList)	{	m_pCopyList = pList;					}

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

	DECLARE_MESSAGE_MAP()	

public:
	afx_msg void OnBnClickedRscAdd();
	afx_msg void OnBnClickedRscDelete();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRscEdit();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnLbnDblclkListRsc();
	afx_msg void OnBnClickedNo();
};
