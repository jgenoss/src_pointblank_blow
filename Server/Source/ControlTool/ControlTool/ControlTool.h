
// ControlTool.h : main header file for the ControlTool application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CControlToolApp:
// See ControlTool.cpp for the implementation of this class
//

class CControlToolApp : public CWinAppEx
{
public:
	CControlToolApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	BOOL  m_bCheckControlServer;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CControlToolApp theApp;
