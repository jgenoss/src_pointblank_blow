#pragma once
#include "afxwin.h"
#include "i3UIEditorFramework.h"
// CPanePreviewer dialog

class CPanePreviewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanePreviewer)

public:
	CPanePreviewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanePreviewer();

// Dialog Data
	enum { IDD = IDD_PANE_PREVIEW };

protected:
	i3Viewer			 *	m_pViewer;
	i3GuiEditorFramework *	m_pFramework;
	i3SoundContext		 *	m_pSoundCtx;

	UINT32					m_nViewerWidth;
	UINT32					m_nViewerHeight;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );

public:
	void	SetViewerResolution( UINT32 width, UINT32 height, BOOL bUpdate = FALSE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	CStatic m_PreviewerPlaceHolderCtrl;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
