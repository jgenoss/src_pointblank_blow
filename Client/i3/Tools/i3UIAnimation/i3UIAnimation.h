
// i3UIAnimation.h : i3UIAnimation 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


// i3UIAnimationApp:
// 이 클래스의 구현에 대해서는 i3UIAnimation.cpp을 참조하십시오.
//
#include "PropertyPane.h"
#include "TextureViewer.h"
class i3UIAnimationApp : public CWinAppEx
{
	DECLARE_MESSAGE_MAP()
public:
	i3UIAnimationApp();


// 재정의입니다.
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	afx_msg void OnAppAbout();
	void OnProjectPath();
	void OnSaveAnimationFile();
	void OnOpenAnimationFile();
	void OnNewAnimationFile();
};

extern i3UIAnimationApp theApp;
