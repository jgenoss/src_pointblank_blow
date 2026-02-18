#pragma once


// CPaneView dialog

class CPaneView : public CDialog
{
	DECLARE_DYNAMIC(CPaneView)
protected:
	i3Viewer *			m_pViewer;
	i3ViewerFramework *	m_pFramework;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CPaneView(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneView();

// Dialog Data
	enum { IDD = IDD_PANE_VIEW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
