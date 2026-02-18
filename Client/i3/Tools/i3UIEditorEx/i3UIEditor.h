// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// i3UIEditor.h : main header file for the i3UIEditor application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// Ci3UIEditorApp:
// See i3UIEditor.cpp for the implementation of this class
//
#include "i3UIEditorView_Template.h"

enum EVIEW
{
	EVIEW_SCREEN = 0,
	EVIEW_TEMPLATE,

	EVIEW_COUNT
};

class Ci3UIEditorApp : public CWinAppEx
{
public:
	Ci3UIEditorApp();

	CView	*	m_pScreenView;
	CView	*	m_pTemplateView;

	bool		m_bRunModule;

// Overrides
public:
	virtual BOOL InitInstance() override;

// Implementation
	UINT  m_nAppLook;
	virtual void PreLoadState() override;
	virtual void LoadCustomState() override;
	virtual void SaveCustomState() override;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	void	setRunModule( bool bRun = true)	{	m_bRunModule = bRun;}
	bool	getRunModuleEnable(void)		{	return m_bRunModule;}
	
	CView *	SwitchView( EVIEW eView = EVIEW_SCREEN );
	
	CView * getScreenView(void)			{	return m_pScreenView;}
	CView * getTemplateView(void)		{	return m_pTemplateView;}
	
	virtual int ExitInstance() override;

public:
	static I3ERROR_HANDLE CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual int Run() override;
};

extern Ci3UIEditorApp theApp;
