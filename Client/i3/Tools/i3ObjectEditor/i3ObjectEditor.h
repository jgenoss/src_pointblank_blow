// i3ObjectEditor.h : i3ObjectEditor 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호


// Ci3ObjectEditorApp:
// 이 클래스의 구현에 대해서는 i3ObjectEditor.cpp을 참조하십시오.
//

class Ci3ObjectEditorApp : public CWinApp
{
public:
	Ci3ObjectEditorApp();


// 재정의
public:
	virtual BOOL InitInstance() override;

	static I3ERROR_HANDLE CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);

// 구현

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance() override;
};

extern Ci3ObjectEditorApp theApp;
