#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


struct PackQueue_ProgressCallback;
 
class CRscToolApp : public CWinAppEx
{
public:
	CRscToolApp();


// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	void	SetProgressCallback(PackQueue_ProgressCallback* cb) { m_progress_cb = cb; }
	PackQueue_ProgressCallback*	GetProgressCallback() const { return m_progress_cb; }


	DECLARE_MESSAGE_MAP()


private:
	PackQueue_ProgressCallback* m_progress_cb;

public:
	virtual BOOL OnIdle(LONG lCount);
	virtual int  ExitInstance();

};

extern CRscToolApp theApp;
