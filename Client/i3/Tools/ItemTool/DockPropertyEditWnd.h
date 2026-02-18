#pragma once

#include "DockBaseWnd.h"

#include "PropertyEditor/i3TDKRegistryCtrl_Impl2.h"



class cDockPropertyEditWnd : public cDockBaseWnd
{
	DECLARE_DYNAMIC(cDockPropertyEditWnd)

public:
	cDockPropertyEditWnd();
	virtual ~cDockPropertyEditWnd();

protected:
	DECLARE_MESSAGE_MAP()


protected:
	i3TDKRegistryCtrl_Impl2	m_RegCtrl;

	CPoint					m_CurrentPt;
	int						m_nBarIndex;

protected:
	void _ReSizeWindow(void);
	void _ReSizeControl(POINT pt, int BarIndex);


public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	bool	SetPefData(LPCSTR pFilePath, i3RegKey* pRoot);

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


