#pragma once
#include "afxwin.h"

#include "i3LevelDesignFramework.h"

// CPanelViewer dialog

class CPanelViewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelViewer)
protected:
	i3LevelViewport			m_Viewport;

public:
	CPanelViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelViewer();

	void			DropObject( i3LevelElement * pObj, POINT pt);

// Dialog Data
	enum { IDD = IDD_PANEL_VIEWER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void			ReplaceControl( int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	void		 OnUpdate(I3_TDK_UPDATE_INFO * pInfo);	
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
