#ifndef __FILELIST_H__
#define	__FILELIST_H__

#include "DataType.h"

#include <list>
#include <algorithm>

//
//	CFileList
//
// 파일리스트 파일을 읽어들이는 클래스입니다.
// 읽어들인 파일정보들은 경로를 기준으로 정렬됩니다.
//


#pragma warning(disable : 4995)


class CFileList;
class CFileListLexer;
class CFileListFileInfo;
class CFileFilter;

typedef	std::list<CFileListFileInfo*>	ListFileInfo;


#define			FILELIST_NAME			_T("FileList.dat")



CString		VersionToDate(const FileVersion & Version);
FileVersion DateToVersion(const CString & VerStr);
CString		VersionToLatestFileName(const FileVersion & Version);

bool		IsNullTime(const FILETIME * pTime);

enum	FILELIST_ITEM
{
	FILELIST_ITEM_HEADER,
	FILELIST_ITEM_FILEINFO,

	FILELIST_ITEM_MAX,
	FILELIST_ITEM_INVALID,
};


#define		CHECK_FLAG_NULL				0x00000000
#define		CHECK_FLAG_EXIST			0x00000001		// 다른 플래그와 조합시는 무조건 유무를 검사하게됩니다.
#define		CHECK_FLAG_SIZE				0x00000002
#define		CHECK_FLAG_MD5				0x00000004
#define		CHECK_FLAG_LASTWRITETIME	0x00000008

#define		CHECK_FLAG_INVALID			0x80000000

#define		CHECK_FLAG_DEFAULT			(CHECK_FLAG_EXIST | CHECK_FLAG_SIZE | CHECK_FLAG_LASTWRITETIME)
#define		CHECK_FLAG_FULL				(CHECK_FLAG_EXIST | CHECK_FLAG_SIZE | CHECK_FLAG_MD5 | CHECK_FLAG_LASTWRITETIME)

// 체크 플래그의 텍스트를 반환한다. 플래그가 여러개여도 한개만 채워진다.
void	GetFlagText(CString & str, UINT32 Flag);

// 체크 플래그의 모든 텍스트를 반환한다.
void	GetFlagText(CStringArray & strArray, UINT32 Flag);

// 체크 플래그의 텍스트를 "flag(1) / flag(2) / ... / flag(n)" 이러한 형식으로 만든다.
void	GetFlagTextAll(CString & str, UINT32 Flag);

// 비트별로 조합된 플래그를 각각 UINT로 나눈다.
void	SplitFlag(UINT32 Flag, CUIntArray & flags);


// 다수 파일 작업에 대한 진행 데이터를 나타내는 변수
struct	FileProgressData
{
	UINT	NumTotal;			// 전체 개수
	UINT	NumProgCount;		// 완료된 개수
	UINT	FileTotalSize;		// 파일 전체 크기
	UINT	FileProgEndSize;	// 완료된 파일의 총 크기
	UINT	FileSize;			// 작업중인 파일 크기
	UINT	FileProgSize;		// 작업이 진행된 크기
	CString	FileName;			// 작업파일 이름
	CString	ProgText;			// 작업 내용

	void	Clear();
	void	ClearNoText();
	void	Set(LPCTSTR pText, UINT Total);
	void	Set(UINT nTextID, UINT Total);
};


class	CFileListFileInfo
{
private:
	CString			FileName;
	CString			FileDir;
	CString			FilePath;	// FileDir + FileName
	FileVersion		Version;
	UINT32			FileSize;
	UINT32			MD5[4];

	// 파일 타임은 시스템타임(UTC)로 저장합니다.
	FILETIME		CreationTime;
	FILETIME		LastWriteTime;

	UINT32			StartCheckOption;		// 시작버튼 눌렀을 때 검사 옵션
	UINT32			CommandCheckOption;		// 체크버튼 눌렀을 때 검사 옵션

public:
	CFileListFileInfo();


public:
	void	SetFileName(LPCTSTR pFileName)					{FileName = pFileName;}
	void	SetFileDir(LPCTSTR pFileDir)					{FileDir = pFileDir;}
	void	SetFilePath(LPCTSTR pFilePath)					{FilePath = pFilePath;}
	void	SetVersion(const FileVersion * pVersion)		{Version = *pVersion;}
	void	SetFileSize(UINT32 Size)						{FileSize = Size;}
	void	SetMD5(const UINT32 * pMD5)						{memcpy(MD5, pMD5, sizeof(MD5));}
	void	SetCreationTime(const FILETIME * pTime)			{memcpy(&CreationTime, pTime, sizeof(CreationTime));}
	void	SetLastWriteTime(const FILETIME * pTime)		{memcpy(&LastWriteTime, pTime, sizeof(LastWriteTime));}
	void	SetStartCheckOptionFlag(UINT32 StartCheckFlag)		{StartCheckOption = StartCheckFlag;}
	void	SetCommandCheckOptionFlag(UINT32 StartCheckFlag)		{CommandCheckOption = StartCheckFlag;}

	const CString &		GetFileName() const						{return FileName;}
	const CString &		GetFileDir() const						{return FileDir;}
	const CString &		GetFilePath() const						{return FilePath;}
	const FileVersion *	GetVersion() const						{return &Version;}
	UINT32				GetFileSize() const						{return FileSize;}
	const UINT32 *		GetMD5() const							{return MD5;}
	const FILETIME *	GetCreationTime() const					{return &CreationTime;}
	const FILETIME *	GetLastWriteTime() const				{return &LastWriteTime;}
	UINT32				GetStartCheckOptionFlag() const			{return StartCheckOption;}
	UINT32				GetCommandCheckOptionFlag() const		{return CommandCheckOption;}

public:
	void		PrintFileInfo(CString & Buffer) const;


	// BaseDir은 NULL일 수 없습니다.
	UINT32	CheckByStart(LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize = NULL, UINT * pMD5ProgSize = NULL) const;
	UINT32	CheckByCommand(LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize = NULL, UINT * pMD5ProgSize = NULL) const;

	// 매개변수의 옵션값으로 검사를 합니다.
	UINT32	Check(UINT32 Flag, LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize = NULL, UINT * pMD5ProgSize = NULL) const;


public:
	static	bool	Compare(const CFileListFileInfo * pLeft, const CFileListFileInfo * pRight);
	static	bool	CompareDeep(CFileListFileInfo * pLeft, CFileListFileInfo * pRight,
		LPCTSTR pLeftDir, LPCTSTR pRightDir, UINT * pFileSize = NULL, UINT * pFileProgSize = NULL);

	// 소트에 사용되는 함수
	static	bool	CompPath(CFileListFileInfo * pLeft, CFileListFileInfo * pRight);
};





class	CompareResult
{
public:
	ListFileInfo	NormalFiles;		// 변함이 없는 파일들
	ListFileInfo	DiffFiles;			// 변경된 파일들
	ListFileInfo	DiffOrigFiles;		// 변경된 파일의 이전 파일
	ListFileInfo	LeftOnlyFiles;
	ListFileInfo	RightOnlyFiles;

	UINT			CheckFlagModifiedCount;	// 검사 옵션이 변경된 개수


public:
	void	Clear();
	bool	IsNoDiff();

#if 0
	bool	Save(LPCTSTR pFilePath, FileVersion OldFileListVer, FileVersion NewFileListVer);
#endif
};



class	CFileList
{
private:
	enum	VERSION
	{
		UNKNOWN_VER	= 0,
		VER_1		= 1,
		VER_2		= 2,
		VER_3		= 3,			// 파일 검사 옵션이 추가됨. (시작시에만 적용됨)
		VER_4		= 4,			// 파일 검사 버튼을 눌렀을때의 검사 옵션이 추가됨.
		LAST_VER	= VER_4,
	};

public:
	CFileList();
	CFileList(const CFileList & FileList);
	~CFileList();

public:
	bool	Open(LPCTSTR pPath);
	void	Clear();

	bool	SaveAs(LPCTSTR pFilePath);
	void	SetFileVersion(const FileVersion & val);
	void	SetCurTimeVersion();
	bool	MakeUpdateList(CompareResult * pCompResult);
	bool	GenerateMD5(FileProgressData * pProgData);

	FileVersion		GetVersion();
	void			SetVersion(const FileVersion & val);

	const ListFileInfo&	GetFileInfoList() const		{return m_FileInfoList;}
	LPCTSTR			GetFullPath()		{return m_FullPath;}
	LPCTSTR			GetDirPath()		{return m_DirPath;}
	VERSION			GetFormatVersion()	{return m_FormatVer;}


public:
	static	CFileList *	MakeFileList(LPCTSTR pPath, CFileFilter & FileFilter, const CStringArray & InclFilePaths, bool GenMD5, FileProgressData * pProgData = NULL);
	static	bool		Compare(CFileList * pLeft, CFileList * pRight, CompareResult * pResult, bool DeepComp = false, FileProgressData * pProgData = NULL);


private:
	bool	Parse_1();
	bool	Parse_2();
	bool	Parse_3();
	bool	Parse_4();

	void	SortFileInfoList();
	void	SetPath(LPCTSTR pPath);


private:
	CFileListLexer *	m_pLexer;
	VERSION				m_FormatVer;
	CString				m_FullPath;
	CString				m_DirPath;

	FileVersion			m_Version;
	ListFileInfo		m_FileInfoList;
};


enum	CFD_RV
{
	CFD_RV_TRUE,		// 같다.
	CFD_RV_FALSE,		// 다르다.
	CFD_RV_LEFTERROR,	// 왼쪽파일 문제발생
	CFD_RV_RIGHTERROR,	// 오른쪽파일 문제발생
};
CFD_RV	CompareFileData(LPCTSTR pLeftFilePath, LPCTSTR pRightFilePath, UINT * pFileSize = NULL, UINT * pFileProgSize = NULL);



#endif
