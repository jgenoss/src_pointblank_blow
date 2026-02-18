
// LiveManager.h : LiveManager 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.


// CLiveManagerApp:
// 이 클래스의 구현에 대해서는 LiveManager.cpp을 참조하십시오.
//

class CLiveManagerApp : public CWinAppEx
{
private:
	char m_V1_MediaPath[512];
	char m_V2_MediaPath[512];
public:
	CLiveManagerApp();


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
	virtual int Run();
	DECLARE_MESSAGE_MAP()

	void MediaSetting();
	void MediaSave();
	void CheckPEF();
	void MergePEF();
	bool SavePEF(i3RegistrySet* pRegistry, const char * pszPath);
	//i3RegData * AddNewData( INT32 nType, INT32 nValue, INT32 nNationCount );

	//툴바쪽
	void OnAddWeapon();
	void OnDeleteWeapon();
	void OnSaveWeaponPef();

public:
	char* GetV1MediaPath(){return m_V1_MediaPath;}
	char* GetV2MediaPath(){return m_V2_MediaPath;}

	void SetV1MediaPath(char* path){ strcpy(m_V1_MediaPath, path); }
	void SetV2MediaPath(char* path){ strcpy(m_V2_MediaPath, path); }
};

extern CLiveManagerApp theApp;
