/*
버전생성 & 비교기능 클래스
버전생성 : 
파일리스트를 현재 파일의 갯수 만큼 동적으로 생성후 MD5 Encrypt후 보관한다
버전비교 :
넘겨받은 파일리스트 or 단일 파일 과 버전을 비교하여 
파일리스트의 경우 BOOL 값으로 이루어진 배열을 만들어 새로 받을 파일은 TRUE 기존 파일과 같은 파일은 FALSE로 셋팅하여 Return
단일 파일의 경우 해당 파일을 바꿀것인지 아닌지 TRUE / FALSE 를 반환한다.
*/


#pragma once

#define STRING_SET		50
#define OFFSET_COUNT	5
#define MD5_SIZE		17

#include "tchar.h"
#include "MD5.h"

typedef struct FileList
{
	WCHAR	MD5output[MD5_SIZE];
	char	FileName[MAX_PATH];
	BOOL	SetChange;
	
	FileList()
	{
		ZeroMemory(MD5output, sizeof(WCHAR) * MD5_SIZE);
		ZeroMemory(FileName, sizeof(char) * MAX_PATH);
	}

} FILELIST;

//void AttachFileList(FILELIST* &Des, UINT16& _sizeDes, FILELIST* Src, UINT16 _sizeSrc );

class CVersionCreate
{
protected:

	//MD5 Value
	WCHAR			m_MD5output[MD5_SIZE];

	//Version Create Value
	FILE*			m_ConfigFile;
	FILELIST*		m_FileList;
	TCHAR			m_strFilePath[MAX_PATH];
	char			m_strRootPath[MAX_PATH];	//수정
	INT32			m_i32CurFileCount;
	UINT16			m_ui16UpdateFilePos;

	BOOL			m_bCreate;

	INT64			getFileSize(const TCHAR *filename);
	void			GetCount(TCHAR* pDirectory, INT32& Count);
	void			ClearString( TCHAR* strBuf);
	WCHAR*			MD5Encrypt(TCHAR* pFileName, INT32 i32Current);
	void			SetFileVersionList(TCHAR* pDirectory, INT32& Count);
	void			SetFilePath( TCHAR* FullPah, TCHAR* FilePath );


public:

	CVersionCreate();
	virtual ~CVersionCreate();

	//컨트롤툴 전용
	BOOL			CheckVersion_onlyfile(WCHAR* Input);
	void			MakeMD5Value_onlyfile(TCHAR* File)		{ memcpy(m_MD5output, MD5Encrypt(File, -1), MD5_SIZE); }


	//파일 버젼 생성
	BOOL			Create(char* strPath, char* strRootPath);
	//파일 버젼 비교(리스트 : 단일)
	BOOL			CheckVersion(char* UpdateData);
	//파일 버젼 비교(리스트 : 리스트)
	BOOL			CheckVersion(FILELIST* UpdateDataList, INT32 i32ListSize);
	

	FILELIST*			GetFileList()		{ return m_FileList; };
	INT32				GetListcount()		{ return m_i32CurFileCount; };

	WCHAR*			GetMD5ValueFromPath(TCHAR* input);
};