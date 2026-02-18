// i3ExternRef.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호
#include "OptionInfo.h"


// Ci3ExternRefApp:
// 이 클래스의 구현에 대해서는 i3ExternRef.cpp을 참조하십시오.
//

class Ci3ExternRefApp : public CWinApp
{
public:
	Ci3ExternRefApp();

// 재정의
	public:
	virtual BOOL InitInstance() override;

// 구현

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern Ci3ExternRefApp theApp;
extern COptionInfo		* g_pOption;