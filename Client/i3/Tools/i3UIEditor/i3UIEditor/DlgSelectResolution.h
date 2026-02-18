#pragma once
#include "afxwin.h"


// CDlgSelectResolution dialog

class CDlgSelectResolution : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectResolution)
protected:
	INT32	m_nResolutionWidth;
	INT32	m_nResolutionHeight;

public:
	CDlgSelectResolution(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectResolution();

// Dialog Data
	enum { IDD = IDD_SELECT_RESOLUTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edResolutionWidth;
	CEdit m_edResolutionHeight;

public:
	INT32	getResolutionWidth( void)			{	return m_nResolutionWidth;}
	void	setResolutionWidth( INT32 width)	{	m_nResolutionWidth = width;}

	INT32	getResolutionHeight( void)			{	return m_nResolutionHeight;}
	void	setResolutionHeight( INT32 height)	{	m_nResolutionHeight = height;}
	virtual BOOL OnInitDialog();
	afx_msg void OnEnKillfocusResolutionHeight();
	afx_msg void OnEnKillfocusResolutionWidth();
	afx_msg void OnBnClickedOk();
};
