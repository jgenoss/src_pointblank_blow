#include "StdAfx.h"

#include <queue>

#include "FileUtil.h"

static OnStart g_StartProc = NULL;
static OnEnd g_EndProc = NULL;
static OnFile g_FileProc = NULL;

void FileUtil::SetProc( OnStart StartProc, OnEnd EndProc, OnFile FileProc )
{
	g_StartProc = StartProc;
	g_EndProc = EndProc;
	g_FileProc = FileProc;
}


void FileUtil::Spread( const String & Path, bool Spread)
{
	if( g_StartProc )
		(*g_StartProc)();

	WIN32_FIND_DATA findFileData;			// File Data 구조 Structure
	HANDLE hFileHandle = 0;			// File Descriptor Handle

	std::queue< String > PathQueue;
	PathQueue.push( Path );

	String TmpPath, CurrentPath;
	do
	{
		CurrentPath = PathQueue.front();

		//모든 파일 path를 만든다.
		TmpPath.assign( CurrentPath );
		TmpPath.append( "\\*.*" );

		// 0. 찾고 싶은 파일명/Handle 얻어 옴.
		hFileHandle = FindFirstFile( TmpPath.c_str(), &findFileData);
		if ( hFileHandle != INVALID_HANDLE_VALUE)
		{
			do 
			{
				// 1. 자신/상위 폴더명은 제외
				if ( !((strcmp(findFileData.cFileName, "." ) == 0) ||
					strcmp(findFileData.cFileName, ".." ) == 0) )
				{
					TmpPath.assign( CurrentPath );
					TmpPath.append( "\\" );
					TmpPath.append( findFileData.cFileName );

					// 2. 파일 속성을 참조하여 파일이면 출력 
					if( (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						if( Spread )
						{
							PathQueue.push( TmpPath );
						}
					}
					else	//일반 파일
					{
						if( g_FileProc )
							(*g_FileProc)( findFileData, TmpPath);
					}
				}
				// 3. 다음 File Handle로 접근
			} while( FindNextFile( hFileHandle, &findFileData) );
		}
	PathQueue.pop();
	}while( !PathQueue.empty() );

	::FindClose( hFileHandle );

	if( g_EndProc )
		(*g_EndProc)();
}

void initShFileOpStruct(LPSHFILEOPSTRUCTA op,
		UINT funct, char* from, char* to, USHORT orFlag = 0, USHORT norFlag = 0)
{
	op->hwnd = NULL;
	op->fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI;  // 확인메시지가 안뜨도록 설정
	op->fFlags |= orFlag;
	op->fFlags ^= norFlag;

	op->fAnyOperationsAborted = false;
	op->hNameMappings = NULL;
	op->lpszProgressTitle = NULL;
	op->wFunc = funct;
	op->pFrom = from;
	op->pTo = to;
}

bool processShFileOpStruct(LPSHFILEOPSTRUCTA op)
{
	if( SHFileOperationA(op) == 0 )
		return true;

	return false;
}

bool ShellFileOperation(UINT funct, const String & from,
		const String & to, USHORT orFlag = 0, USHORT norFlag = 0)
{
	String srcDirectory(from);
	srcDirectory.insert( srcDirectory.end(), 0);

	String desDirectory(to);
	desDirectory.insert( desDirectory.end(), 0);

	SHFILEOPSTRUCTA FileOp = {0};

	initShFileOpStruct(&FileOp,
		funct, (char*)srcDirectory.c_str(), (char*)desDirectory.c_str(), orFlag, norFlag);

	return processShFileOpStruct(&FileOp);
}

bool FileUtil::CopyFile(const String & SrcName, const String & DesName)
{
	return ShellFileOperation(FO_COPY, SrcName, DesName);
}






int CStreamDialog::BrowseForFolder_CallbackProc(HWND m_hWnd, 
										  UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if(uMsg == BFFM_INITIALIZED)
		::SendMessage(m_hWnd, BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)lpData);

	return 0;
}

CString CStreamDialog::OpenFolder(HWND hwnd, const CString & curDir)
{
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner =  hwnd;
	bi.pidlRoot = NULL; 
	bi.pszDisplayName = NULL;
	bi.lpszTitle = NULL;

	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = BrowseForFolder_CallbackProc;

	if( curDir.IsEmpty() )
	{
		TCHAR path[MAX_PATH] = {0,};
		GetCurrentDirectory(MAX_PATH, path);
		bi.lParam = (LPARAM)(LPCTSTR)path; //초기값 설정.
	}
	else
		bi.lParam = (LPARAM)(LPCTSTR)curDir.GetString(); //초기값 설정.

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	if( pidl )
	{
		TCHAR szPath[MAX_PATH] = {0,};
		SHGetPathFromIDList( pidl, szPath );
		mPathName = szPath;
	}

	return mPathName;
}

void CStreamDialog::OpenFileDialog(const CString & title, const CString & fileExt, 
							 const CString & filer, const CString & curDir)
{
	CFileDialog fileDlg(TRUE, title, fileExt, 
		OFN_HIDEREADONLY|OFN_ENABLESIZING, 
		filer, 0);

	if( !curDir.IsEmpty() )
		fileDlg.GetOFN().lpstrInitialDir = curDir;

	if( fileDlg.DoModal() == IDOK )
	{
		mPathName = fileDlg.GetPathName().GetString();
	}
	else
	{
		mPathName = _T("");
	}
}

CString CStreamDialog::GetPath()
{
	return mPathName;
}

CString CStreamDialog::MakeFileFilter(const WString & item, BOOL all)
{
	CString str;
	str.Format( _T("%s Files (*.%s)|*.%s|"), item.c_str(), item.c_str(), item.c_str() );

	CString allFilter( _T("All Files (*.*)|*.*|") );

	CString rst;
	rst.Append( str );

	if( all )	rst.Append( allFilter );
	rst.Append( _T("|") );
	return rst;
}