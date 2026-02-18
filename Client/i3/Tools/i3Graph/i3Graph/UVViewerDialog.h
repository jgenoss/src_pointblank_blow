#include "afxwin.h"
#pragma once


// CUVViewerDialog dialog

class CUVViewerDialog : public CDialog
{
	DECLARE_DYNAMIC(CUVViewerDialog)

protected:
	i3Node *	m_pRoot;

	HBITMAP		m_hBitmap;

protected:
	void		DrawUV(void);

public:
	void	Execute( i3Node * pRoot);

public:
	CUVViewerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUVViewerDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_UV_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_ImageViewCtrl;
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
