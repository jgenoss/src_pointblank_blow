// i3LevelDesign.h : i3LevelDesign 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호
#include "MainFrm.h"

// Ci3LevelDesignApp:
// 이 클래스의 구현에 대해서는 i3LevelDesign.cpp을 참조하십시오.
//

class Ci3LevelDesignApp : public CWinAppEx
{
public:
	Ci3LevelDesignApp();
	virtual ~Ci3LevelDesignApp();

// 재정의
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;

	UINT  m_nAppLook;

public:
	LEVEL_EDIT_MODE		GetLevelEditMode();

public:
	void		SetLevelEditMode( LEVEL_EDIT_MODE _mode );

// 구현

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnFileOpen();

public:
	CMainFrame * GetMainFrm(void) {
		return (CMainFrame *)m_pMainWnd;
	}
};

extern Ci3LevelDesignApp theApp;
