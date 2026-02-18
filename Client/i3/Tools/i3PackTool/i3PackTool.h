// i3PackTool.h : i3PackTool 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호
#include "MainFrm.h"

// Ci3PackToolApp:
// 이 클래스의 구현에 대해서는 i3PackTool.cpp을 참조하십시오.
//	Pack 툴관련해서 command 기능의 클레스

class Ci3PackToolApp : public CWinApp
{
protected:
	bool			m_bNoWindow;
	i3::vector<class ScriptCmd*>	m_CmdList;

public:
	Ci3PackToolApp();

	// command line에서 파일폴더 경로를 추출합니다. 입력변수인 pszCmd의 시작문자열이 파일폴더경로이어야 합니다.
	// ex)	O : pszCmd == "  C:/aaa/bbb/ -a -x"
	//		X " pszCmd == "-x -a C:/aaa/bbb/"
	//
	// 리턴값 : pszCmd의 다음 포인터 (처리한 만큼 이동된 포인터입니다)
	// 성공유무 : pszPath에 채워진 문자열이 NULL이면 실패. 값이 있으면 성공.
	char *		ParsePath(char * pszCmd, char * pszPath);

	// command line에서 제외파일 리스트를 추출합니다. (*)기호 사용가능합니다. (;)기호로 개별파일을 구분합니다.
	// ex)		pszCmd == "-E *.log;*.txt;*.exe"
	//
	// 리턴값 : pszCmd의 다음 포인터 (처리한 만큼 이동된 포인터입니다)
	char *		ParseException( char * pszCmd, i3::vector<i3::rc_string>& ExceptionList);

	void		PrintUsage(void);
	void		ParseCmd( CMainFrame * pFrame);
	void		SetScriptCmd(char* szScript);
	void		SetBatCmd(char* pszPath);
	void		Export_i3RegXMLToPEF(const char* pszPath);
// 재정의
public:
	virtual BOOL InitInstance() override;

// 구현

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance() override;
};

extern Ci3PackToolApp theApp;
