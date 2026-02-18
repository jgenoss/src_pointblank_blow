// i3PackFileEditor.h : i3PackFileEditor 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호

extern char g_szExeDir[MAX_PATH];

class CMyCommandLineInfo : public CCommandLineInfo
{
protected:
public:
	BOOL m_bForceOverwrite;
	BOOL m_bCompile;
	BOOL m_bCompileAndExit;
	BOOL m_bLoadPSPFile;
	BOOL m_bNoBGM;
	BOOL m_bNoSound;

	CMyCommandLineInfo();
	void MyParseParamFlag(const char* pszParam);
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
};

extern CMyCommandLineInfo g_MyCmdInfo;

// Ci3PackFileEditorApp:
// 이 클래스의 구현에 대해서는 i3PackFileEditor.cpp을 참조하십시오.
//

class Ci3PackFileEditorApp : public CWinApp
{
public:
	Ci3PackFileEditorApp();


// 재정의
public:
	virtual BOOL InitInstance();

// 구현
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern Ci3PackFileEditorApp theApp;
