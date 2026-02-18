
// MFCTest.h : main header file for the MFCTest application
//
#pragma once

#if defined( I3_DEBUG)
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCApp:
// See MFCTest.cpp for the implementation of this class
//

class CMFCApp : public CWinAppEx
{
public:
	CMFCApp();


// Overrides
public:
	virtual BOOL InitInstance() override;

// Implementation

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	DECLARE_MESSAGE_MAP()
};

extern CMFCApp theApp;

#endif
