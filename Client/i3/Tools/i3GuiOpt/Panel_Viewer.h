#pragma once


// CPanel_Viewer dialog

class CPanel_Viewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Viewer)

protected:
	i3TDKViewerCtrl				m_ViewerCtrl;
	i3ViewerFramework *			m_pFramework;

public:
	CPanel_Viewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Viewer();

	void	StartProcessing(void);
	void	EndProcessing(void);

// Dialog Data
	enum { IDD = IDD_PANEL_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
