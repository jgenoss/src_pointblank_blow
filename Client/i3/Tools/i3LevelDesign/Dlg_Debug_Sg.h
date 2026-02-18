#pragma once


// CDlg_Debug_Sg dialog

class CDlg_Debug_Sg : public CDialog
{
	DECLARE_DYNAMIC(CDlg_Debug_Sg)
protected:
	i3TDKSceneGraphTreeCtrl		m_SgCtrl;

protected:
	void	PlaceControls( INT32 cx, INT32 cy);

public:
	CDlg_Debug_Sg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_Debug_Sg();

// Dialog Data
	enum { IDD = IDD_DEBUG_SG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
