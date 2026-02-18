#pragma once


// CPanelTimeEvent dialog

class CPanelTimeEvent : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelTimeEvent)
protected:

public:
	i3TDKTimeEventCtrl		m_TimeEventCtrl;

public:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void	OnStartPlay(void);
	void	OnStartPause(void);

public:
	CPanelTimeEvent(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelTimeEvent();

// Dialog Data
	enum { IDD = IDD_PANEL_TIMEEVENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
