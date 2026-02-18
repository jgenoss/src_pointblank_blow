#pragma once
#include "afxwin.h"
#include "i3GuiEditorFramework.h"
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
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );

public:
	void	ReplaceControls( INT32 cx, INT32 cy );

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog() override;
	CStatic m_PreviewerPlaceHolderCtrl;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
