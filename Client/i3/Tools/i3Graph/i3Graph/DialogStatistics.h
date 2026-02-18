#pragma once
#include "afxwin.h"


// CDialogStatistics dialog

class CDialogStatistics : public CDialog
{
	DECLARE_DYNAMIC(CDialogStatistics)

protected:

	i3Node *		m_pRoot;

	INT32			m_NodeCount;
	INT32			m_AttrCount;
	INT32			m_ShapeSetCount;
	INT32			m_ConvexCount;

	static I3SG_TRAVERSAL_RESULT _ScanProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack );

public:
	CDialogStatistics(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogStatistics();

	bool		Execute( i3Node * pNode);

// Dialog Data
	enum { IDD = IDD_DIALOG_STATISTICS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_NodeCount;
	CEdit m_ED_AttrCount;
	CEdit m_ED_ShapeSetCount;
	CEdit m_ED_ConvexCount;
	virtual BOOL OnInitDialog();
};
