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

// i3Viewer2.h : i3Viewer2 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


// Ci3Viewer2App:
// 이 클래스의 구현에 대해서는 i3Viewer2.cpp을 참조하십시오.
//

class Ci3Viewer2App : public CWinAppEx
{
public:
	Ci3Viewer2App();


// 재정의입니다.
public:
	virtual BOOL InitInstance() override;

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState() override;
	virtual void LoadCustomState() override;
	virtual void SaveCustomState() override;

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance() override;
};

extern Ci3Viewer2App theApp;
