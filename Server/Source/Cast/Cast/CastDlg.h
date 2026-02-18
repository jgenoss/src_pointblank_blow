// ClanDlg.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifdef BUILDMODE_DLG

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'pch.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호


// CClanDlgApp:
// 이 클래스의 구현에 대해서는 ClanDlg.cpp을 참조하십시오.
//

class CClanDlgApp : public CWinApp
{
public:
	CClanDlgApp();

// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
};

extern CClanDlgApp theApp;

#endif