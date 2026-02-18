#pragma once

typedef void (*OnStart)();
typedef void (*OnEnd)();
typedef void (*OnFile)(const WIN32_FIND_DATA& Data, const String & FileName);

class FileUtil
{
public:
	static void SetProc(OnStart StartProc, OnEnd EndProc, OnFile FileProc);
	static void Spread( const String & Path, bool Spread = false );

	static bool CopyFile(const String & SrcName, const String & DesName);
};

class CStreamDialog
{
public:
	virtual ~CStreamDialog(void) {}

private:
	CString mPathName;

public:
	static int CALLBACK BrowseForFolder_CallbackProc(HWND m_hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

public:
	//폴도 열기
	CString OpenFolder(HWND hwnd, const CString & curDir = CString() );

	//파일 다이얼로그 열기
	void OpenFileDialog(const CString & title, const CString & fileExt, 
		const CString & filer, const CString & curDir = CString() );
	
public:
	//FileDialog Filter 만들기
	CString MakeFileFilter(const WString & item, BOOL all = true);

	//Dialog에서 파일 선택시에 값을 얻는것(FullPath)
	CString GetPath();
};