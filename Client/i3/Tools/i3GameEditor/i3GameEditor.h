// i3GameEditor.h : i3GameEditor 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

//	=================================================================================
//	i3GameEditor
//	Programed : komet
//	Build Date : 2006.6.
//	Last Update : 2006.6.29
//	=================================================================================

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호
#include "MainFrm.h"

// Ci3GameEditorApp:
// 이 클래스의 구현에 대해서는 i3GameEditor.cpp을 참조하십시오.
//

class Ci3GameEditorApp : public CWinApp
{
public:
	Ci3GameEditorApp();


// 재정의
public:
	virtual BOOL InitInstance() override;
	virtual int ExitInstance() override;
	static I3ERROR_HANDLE CallbackLog( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);

// 구현
	CMainFrame * GetMainFrm( void)		{	return (CMainFrame *) m_pMainWnd;							}

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Ci3GameEditorApp theApp;
