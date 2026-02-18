#ifndef		__FILEFILTER_H__
#define		__FILEFILTER_H__

//
// 필터 우선순위는 include 가 우선됩니다.
// include* 조건에 만족된다면 exclude* 조건에
// 만족하더라도 Test는 true를 반환합니다.
//


class CFileFilter
{
public:
	const CFileFilter & operator = (const CFileFilter & Src);


public:
	bool	Test(const CString & FileFullPath);

	void	Clear();


public:
	/// 포함 조건
	void	AddIncludeFilePath(const CString & FilePath)		{m_InclFilePaths.Add(FilePath);}
	void	SetIncludeFilePath(const CStringArray & FilePaths)	{m_InclFilePaths.Copy(FilePaths);}


	/// 미포함 조건
	void	AddExcludeExt(const CString & Ext)				{m_ExclExts.Add(Ext);}
	void	SetExcludeExt(const CStringArray & Exts)		{m_ExclExts.Copy(Exts);}

	// 확장자 포함
	void	AddExcludeFileName(const CString & FileName)		{m_ExclFileNames.Add(FileName);}
	void	SetExcludeFileName(const CStringArray & FileNames)	{m_ExclFileNames.Copy(FileNames);}
	void	AddExcludeDirName(const CString & DirName)			{m_ExclDirNames.Add(DirName);}
	void	SetExcludeDirName(const CStringArray & DirNames)	{m_ExclDirNames.Copy(DirNames);}
	void	AddExcludeDirPath(const CString & DirPath)			{m_ExclDirPaths.Add(DirPath);}
	void	SetExcludeDirPath(const CStringArray & DirPaths)	{m_ExclDirPaths.Copy(DirPaths);}

	void	SetWorkingDirPath(const CString & DirPath);

private:
	CStringArray	m_InclFilePaths;

	CStringArray	m_ExclExts;
	CStringArray	m_ExclFileNames;
	CStringArray	m_ExclDirNames;
	CStringArray	m_ExclDirPaths;

	CString			m_WorkingDirPath;	// 끝에 '/'가 들어갑니다. ex) ../aaa/bbb/
};


#endif

