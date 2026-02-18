#pragma once


// PaneObject dialog

class PaneObject : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(PaneObject)

public:
	PaneObject(CWnd* pParent = NULL);   // standard constructor
	virtual ~PaneObject();

// Dialog Data
	enum { IDD = IDD_PANE_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit		m_ObjectNameCtrl;
	CEdit		m_TotalVertexCtrl;
	CEdit		m_SimVertexCtrl;
	CEdit		m_TotalSpringCtrl;

	CEdit		m_TotalMassCtrl;
	BOOL		m_bMassDifferential;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
};
