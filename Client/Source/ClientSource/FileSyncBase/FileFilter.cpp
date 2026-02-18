#include "StdAfx.h"
#include "FileFilter.h"




bool	IsIncludeDir(LPCTSTR DirPath, const CStringArray & SrcDirName)
{
	CString		DirName;
	CString		RightPath(DirPath);
	int			NumDir = (int)SrcDirName.GetCount();


	RightPath.Replace('/', '\\');

	while ( ! RightPath.IsEmpty() )
	{
		int FirstSlashIndex = RightPath.Find('\\');

		if ( FirstSlashIndex == -1 )
		{
			DirName = RightPath;
			RightPath.Empty();
		}
		else
		{
			DirName	= RightPath.Left(FirstSlashIndex);
			RightPath	= RightPath.GetBuffer() + FirstSlashIndex + 1;
		}



		for ( int i = 0 ; i < NumDir ; ++i )
		{
			if ( 0 == DirName.CompareNoCase(SrcDirName[i]) )
				return true;
		}
	}

	return false;
}



const CFileFilter & CFileFilter::operator = (const CFileFilter & Src)
{
	Clear();

	SetIncludeFilePath(Src.m_InclFilePaths);
	SetExcludeExt(Src.m_ExclExts);
	SetExcludeFileName(Src.m_ExclFileNames);
	SetExcludeDirName(Src.m_ExclDirNames);
	SetExcludeDirPath(Src.m_ExclDirPaths);

	m_WorkingDirPath = Src.m_WorkingDirPath;

	return *this;
}


bool	CFileFilter::Test(const CString & FileFullPath)
{
	CString		RelativePath = _T(".\\") + FileFullPath.Mid(m_WorkingDirPath.GetLength());


	//for ( INT_PTR i = 0 ; i < m_InclFilePaths.GetCount() ; ++i )
	//{
	//	CString & InclFilePath = m_InclFilePaths[i];
	//	if ( 0 == RelativePath.CompareNoCase(InclFilePath) )
	//		return true;
	//}

	CString		FileTitle, FileExt, FileName;
	SplitPath(FileFullPath, NULL, &FileTitle, &FileExt);

	FileName = FileTitle;
	FileName += FileExt;


	// 확장자 검사
	for ( INT_PTR i = 0 ; i < m_ExclExts.GetCount() ; ++i )
	{
		CString & _Ext = m_ExclExts[i];
		if ( 0 == _Ext.CompareNoCase(FileExt) )
			return false;
	}

	// 파일 이름 검사
	for ( INT_PTR i = 0 ; i < m_ExclFileNames.GetCount() ; ++i )
	{
		CString & _FileName = m_ExclFileNames[i];
		if ( 0 ==  _FileName.CompareNoCase(FileName) )
			return false;
	}

	// 폴더 이름 검사
	if ( IsIncludeDir(FileFullPath, m_ExclDirNames) )
		return false;


	// 폴더 패스 검사
	for ( INT_PTR i = 0 ; i < m_ExclDirPaths.GetCount() ; ++i )
	{
		CString _DirPath = m_ExclDirPaths[i];
		_DirPath.Replace('/', '\\');
		if ( 0 == _DirPath.CompareNoCase(RelativePath) )
			return false;
	}


	return true;
}


void	CFileFilter::Clear()
{
	m_InclFilePaths.RemoveAll();

	m_ExclExts.RemoveAll();
	m_ExclFileNames.RemoveAll();
	m_ExclDirNames.RemoveAll();
	m_ExclDirPaths.RemoveAll();

	m_WorkingDirPath.Empty();
}


void	CFileFilter::SetWorkingDirPath(const CString & DirPath)
{
	m_WorkingDirPath = DirPath;
	TCHAR lastCh = m_WorkingDirPath.GetAt(m_WorkingDirPath.GetLength()-1);
	if ( lastCh != '/' && lastCh != '\\' )
		m_WorkingDirPath += '\\';
}

