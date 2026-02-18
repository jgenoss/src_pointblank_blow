#pragma once
#include "afxwin.h"


// CSelectBoxFaceDialog dialog

class CSelectBoxFaceDialog : public CDialog
{
	DECLARE_DYNAMIC(CSelectBoxFaceDialog)

public:
	CSelectBoxFaceDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectBoxFaceDialog();

// Dialog Data
	enum { IDD = IDD_COLLISIONBOX_SIDE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	BOOL m_bTop;
	BOOL m_bLeft;
	BOOL m_bFront;
	BOOL m_bBack;
	BOOL m_bBottom;
	BOOL m_bRight;
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
