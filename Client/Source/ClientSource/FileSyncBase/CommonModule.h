#pragma once

class CFileFilter;

namespace	FileSyncBase
{

// 경로를 합칩니다.
CString	JoinPath(LPCTSTR Left, LPCTSTR Right);

// '/' 또는 '\' 이면 true
bool	IsSlash(TCHAR c);

// 주어진 경로를 디렉터리와 파일이름으로 나눕니다.
void	SplitPath(LPCTSTR Path, CString * pDir, CString * pFileName);
void	SplitPath(LPCTSTR Path, CString * pDirPath, CString * pFileTitle, CString * pExt);

CString	ChangeExt(LPCTSTR OrignalName, LPCTSTR NewExt);

bool	GetFileTime(LPCTSTR pFilePath, FILETIME * pCreationTime = nullptr, FILETIME * pLastWriteTime = nullptr, FILETIME * pLastAccessTime = nullptr);

bool	SetFileTime(LPCTSTR pFilePath, const FILETIME * pCreationTime = nullptr, const FILETIME * pLastWriteTime = nullptr, const FILETIME * pLastAccessTime = nullptr);

// 시간차의 절대값으로 비교를 하고 CheckMilliSec와 같거나 작으면 true를 리턴.
bool	CheckFileTimeInterval(const FILETIME* pLeftTime, const FILETIME* pRightTime, UINT CheckMilliSec);


bool	GetDirectoryHasFilesEx(const CString & Directory, CStringArray & Files, CFileFilter & FileFilter, bool SubDirRecursive);

bool	GetDirectoryHasFilesEx(LPCTSTR pName, CStringArray & Files, bool SubDirRecursive);


void	BitRotateEncript( UINT8 * pBuf, UINT32 Length, UINT32 c);

void	BitRotateDecript( UINT8 * pBuf, UINT32 Length, UINT32 c);

}
using namespace FileSyncBase;