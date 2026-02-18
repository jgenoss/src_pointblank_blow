#pragma once


// i3TDKViewerCtrl

class I3_EXPORT_TDK i3TDKViewerCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKViewerCtrl)

protected:
	i3Viewer *		m_pViewer = nullptr;

public:
	i3TDKViewerCtrl();
	virtual ~i3TDKViewerCtrl() {}

	i3Viewer *		getViewer()				{ return m_pViewer; }

	INT32			GetViewWidth(void)						{ return getViewer()->GetViewWidth(); }
	INT32			GetViewHeight(void)						{ return getViewer()->GetViewHeight(); }

	void			Play(void)								{ m_pViewer->Play(); }
	void			Pause(void)								{ m_pViewer->Pause(); }

	void			SuspendRender(void);
	void			ResumeRender(void);

	BOOL Create( DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

};


