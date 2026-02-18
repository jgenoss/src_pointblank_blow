#include "stdafx.h"
#include "CommonModule.h"
#include "FileFilter.h"



namespace	FileSyncBase
{


CString	JoinPath(LPCTSTR Left, LPCTSTR Right)
{
	CString	_Left(Left), _Right(Right);

	if ( _Right.Left(2) == ".\\" || _Right.Left(2) == "./" )
		_Right = _Right.GetBuffer() + 2;

	if ( _Left.GetLength() == 0 )
		return _Right;

	CString	Path;
	TCHAR	LeftLastChar	= _Left.GetAt(_Left.GetLength()-1);
	TCHAR	RightFirstChar	= _Right.GetAt(0);
	if ( true != IsSlash(LeftLastChar) && true != IsSlash(RightFirstChar) )
	{
		Path.Format("%s\\%s", _Left, _Right);
	}
	else if ( true == IsSlash(LeftLastChar) && true == IsSlash(RightFirstChar)  )
	{
		Path.Format("%s%s", _Left, _Right.GetBuffer() + 1);
	}
	else
	{
		Path.Format("%s%s", _Left, _Right);
	}
	return Path;
}



bool	IsSlash(TCHAR c)
{
	if ( c == '\\' || c == '/' )
		return true;
	return false;
}

void	SplitPath(LPCTSTR Path, CString * pDir, CString * pFileName)
{
	TCHAR Drive[32], Dir[512], Name[MAX_PATH], Ext[64];
	_tsplitpath_s(Path, Drive, 32, Dir, 512, Name, MAX_PATH, Ext, 64);

	if ( pDir )
	{
		*pDir = Drive;
		*pDir += Dir;
	}
	if ( pFileName )
	{
		*pFileName = Name;
		*pFileName += Ext;
	}
}

void	SplitPath(LPCTSTR Path, CString * pDirPath, CString * pFileTitle, CString * pExt)
{
	TCHAR Drive[32], Dir[512], Name[MAX_PATH], Ext[64];
	_tsplitpath_s(Path, Drive, 32, Dir, 512, Name, MAX_PATH, Ext, 64);

	if ( pDirPath )
	{
		*pDirPath = Drive;
		*pDirPath += Dir;
	}
	if ( pFileTitle )
		*pFileTitle = Name;
	if ( pExt )
		*pExt = Ext;
}


CString ChangeExt( LPCTSTR OrignalName, LPCTSTR NewExt )
{
	TCHAR Drive[32], Dir[512], Name[MAX_PATH], Ext[64];
	_tsplitpath_s(OrignalName, Drive, 32, Dir, 512, Name, MAX_PATH, Ext, 64);

	CString	newName;

	newName.Format(_T("%s%s%s%s"), Drive, Dir, Name, NewExt);
	return newName;
}

bool	GetFileTime(LPCTSTR pFilePath, FILETIME * pCreationTime, FILETIME * pLastWriteTime, FILETIME * pLastAccessTime)
{
	assert ( pFilePath != NULL );
	if ( ! pFilePath )	return false;

	HANDLE	hFile = CreateFile(pFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	bool	Rv = GetFileTime(hFile, pCreationTime, pLastAccessTime, pLastWriteTime);
	CloseHandle(hFile);
	return Rv;
}


bool	SetFileTime(LPCTSTR pFilePath, const FILETIME * pCreationTime, const FILETIME * pLastWriteTime, const FILETIME * pLastAccessTime)
{
	assert ( pFilePath != NULL );
	if ( ! pFilePath )	return false;

	HANDLE	hFile = CreateFile(pFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hFile == INVALID_HANDLE_VALUE )
		return false;

	bool	Rv = SetFileTime(hFile, pCreationTime, pLastAccessTime, pLastWriteTime);
	CloseHandle(hFile);
	return Rv;
}



bool	CheckFileTimeInterval(const FILETIME* pLeftTime, const FILETIME* pRightTime, UINT CheckMilliSec)
{
	// FILETIME 은 100나노세컨트 간격으로 표현된다.
	// 기준시간은 January 1, 1601 (UTC) 이다.
	// 1초를 기준으로 
	// 밀리초 : 0.001 초
	// 나노초 : 0.000 000 001
	// 100나노초 : 0.000 01

	ULONGLONG	LeftTime, RightTime;

	memcpy(&LeftTime, pLeftTime, sizeof(FILETIME));
	memcpy(&RightTime, pRightTime, sizeof(FILETIME));

	LONGLONG	Result = LeftTime - RightTime;

	// 100나노초단위를 밀리초로 변환한다.
	Result /= 10000;
	Result = _abs64(Result);
	printf("Result : %lld\n", Result);

	if (Result <= CheckMilliSec)
		return true;
	return false;
}



bool	GetDirectoryHasFilesEx(const CString & Directory, CStringArray & Files,
							   CFileFilter & FileFilter, bool SubDirRecursive)
{
	CString	FindDir = Directory;
	if ( FindDir.Right(1) != '\\' && FindDir.Right(1) != '/' )
		FindDir += '\\';
	FindDir += _T("*.*");

	CFileFind	ff;
	bool		bFind = ff.FindFile(FindDir);


	// 파일의 확장자를 얻어낸다.
	CStringArray	nextFindDirs;
	CString			FileName, FilePath;

	while ( bFind ) 
	{
		bFind = ff.FindNextFile();

		if( ff.IsDots() == true )
			continue;

		FilePath = ff.GetFilePath();

		if ( ! FileFilter.Test(FilePath) )
			continue;

		// 폴더일 경우에 추가 탐색 폴더에 추가한다.
		if( ff.IsDirectory() == true && SubDirRecursive == true)
		{
			nextFindDirs.Add(FilePath);
			continue;
		}


		Files.Add(FilePath);
	}

	ff.Close();


	if ( SubDirRecursive )
	{
		CString	nextDir;
		int NumSubDir = (int)nextFindDirs.GetCount();
		for ( int i = 0 ; i < NumSubDir ; ++i)
		{
			FileSyncBase::GetDirectoryHasFilesEx(nextFindDirs[i], Files, FileFilter, SubDirRecursive);
		}
	}

	return true;
}


bool	GetDirectoryHasFilesEx(LPCTSTR pName, CStringArray & Files, bool SubDirRecursive)
{
	CFileFind	ff;
	bool		bFind = ff.FindFile(pName);


	// 파일의 확장자를 얻어낸다.
	CStringArray	nextFindDirs;
	CString			FileName, FilePath;

	while ( bFind ) 
	{
		bFind = ff.FindNextFile();

		if( ff.IsDots() == true )
			continue;

		FilePath = ff.GetFilePath();

		// 포함하지 않는 폴더명인지 검사한다.
		if( ff.IsDirectory() == true )
		{
			if ( SubDirRecursive == true )
				nextFindDirs.Add(FilePath);
			continue;
		}

		Files.Add(FilePath);
	}

	ff.Close();


	if ( SubDirRecursive )
	{
		CString	nextDir;
		int NumSubDir = (int)nextFindDirs.GetCount();
		for ( int i = 0 ; i < NumSubDir ; ++i)
		{
			FileSyncBase::GetDirectoryHasFilesEx(nextFindDirs[i], Files, SubDirRecursive);
		}
	}

	return true;
}

void	BitRotateEncript( UINT8 * pBuf, UINT32 Length, UINT32 c)
{
	UINT8 start = * pBuf, d1, d2;
	INT32 i, rc;

	rc = 8 - c;

	for( i = 0; i < (INT32) Length; i++)
	{
		if( i < (INT32) (Length - 1))
		{
			d1 = pBuf[i];	d2 = pBuf[ i + 1];
		}
		else
		{
			d1 = pBuf[i]; d2 = start;
		}

		d1 = (UINT8)((d1 << c) & 0xFF);
		d2 = (UINT8)((d2 >> rc) & 0xFF);

		pBuf[i] = (UINT8)(( d1 | d2) & 0xFF);
	}
}




void	BitRotateDecript( UINT8 * pBuf, UINT32 Length, UINT32 c)
{
	if( pBuf == NULL || Length == 0 ) return;

	UINT8 d1, d2, last;
	INT32 i, rc;

	rc = 8 - c;

	last = pBuf[ Length - 1];

	for( i = (INT32)Length - 1; i >= 0; i--)
	{
		if( i > 0)
		{
			d1 = pBuf[i];	d2 = pBuf[ i - 1];
		}
		else
		{
			d1 = pBuf[i]; d2 = last;
		}

		d1 = (UINT8)((d1 >> c) & 0xFF);
		d2 = (UINT8)((d2 << rc) & 0xFF);
		pBuf[i] = (UINT8)((d1 | d2) & 0xFF);

	}
}

}