#pragma once
#include "ColorKeyCtrl.h"

// CColorKeyEditDialog dialog

class CColorKeyEditDialog : public CDialog
{
	DECLARE_DYNAMIC(CColorKeyEditDialog)

protected:
	CColorKeyCtrl		m_ColorKeyCtrl;
	POINT				m_Point;
	i3KeyframeTable *	m_pRTable;
	i3KeyframeTable *	m_pGTable;
	i3KeyframeTable *	m_pBTable;
	i3KeyframeTable *	m_pATable;

	
public:
	BOOL	Execute( POINT pt, i3KeyframeTable * pRTable, i3KeyframeTable * pGTable, i3KeyframeTable * pBTable, i3KeyframeTable * pATable);

public:
	CColorKeyEditDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CColorKeyEditDialog();

// Dialog Data
	enum { IDD = IDD_COLORKEYDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnToolsBgcolor();
};
