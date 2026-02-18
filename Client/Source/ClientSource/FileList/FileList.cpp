#include "stdafx.h"
#include "FileList.h"
#include "FileListLexer.h"
#include "MD5.h"


#define		FAT_LASTWRITE_RESOLUTION		2000




LPCTSTR		g_pFileListTokens[] =
{
	_T("*Header"),
	_T("*FileInfo"),
};

const int TOKEN_FILELIST_STRING_SIZE = sizeof (g_pFileListTokens) / sizeof(LPCTSTR);

FILELIST_ITEM	GetItemNumber(LPCTSTR ItemName)
{
	for (int i = 0 ; i < TOKEN_FILELIST_STRING_SIZE ; ++i)
	{
		if ( 0 == strcmp(ItemName, g_pFileListTokens[i]) )
			return (FILELIST_ITEM)i;
	}
	return FILELIST_ITEM_INVALID;
}


FileVersion DateToVersion(const CString & VerStr)
{
	assert(VerStr.GetLength() == 19);
	CString		strYear	= VerStr.Mid(0, 4);
	CString		strMonth	= VerStr.Mid(5, 2);
	CString		strDay		= VerStr.Mid(8, 2);
	CString		strHour	= VerStr.Mid(11, 2);
	CString		strMin		= VerStr.Mid(14, 2);
	CString		strSec		= VerStr.Mid(17, 2);


	FileVersion year	= FileVersion(_ttoi(strYear))	* 10000000000;
	FileVersion mon		= FileVersion(_ttoi(strMonth))	* 100000000;
	FileVersion day		= FileVersion(_ttoi(strDay))		* 1000000;
	FileVersion hour	= FileVersion(_ttoi(strHour))	* 10000;
	FileVersion min		= FileVersion(_ttoi(strMin))		* 100;
	FileVersion sec		= FileVersion(_ttoi(strSec));

	FileVersion Ver = year + mon + day + hour + min + sec;
	return Ver;
}

//ex) 20090514102940 -> 2009-05-14_10-29-40
CString	VersionToDate(const FileVersion & Version)
{
	CString	RvName;

	int y = (int)(Version / 10000000000); 
	int m = (int)((Version % 10000000000)/ 100000000); 
	int d = (int)((Version % 100000000)/ 1000000); 
	int h = (int)((Version % 1000000)/ 10000); 
	int mm = (int)((Version % 10000)/ 100); 
	int s = (int)((Version % 100)); 

	RvName.Format( "%04d-%02d-%02d_%02d-%02d-%02d", y,m,d,h,mm,s);
	return RvName; 
}

//2009-05-12_18-45-48_FileList.dat
CString	VersionToLatestFileName(const FileVersion &  Version)
{
	CString	FileName = VersionToDate(Version);
	FileName += '_';
	FileName += FILELIST_NAME;
	return FileName;
}


bool		IsNullTime(const FILETIME * pTime)
{
	assert(pTime != NULL);
	return (pTime->dwHighDateTime == 0 && pTime->dwLowDateTime == 0);
}


void	GetFlagText(CString & str, UINT32 Flag)
{
	if ( Flag == CHECK_FLAG_NULL )
	{
		str = _T("Null");
		return;
	}

	if ( Flag & CHECK_FLAG_EXIST )
	{
		str = _T("Exist");
		return;
	}

	if ( Flag & CHECK_FLAG_SIZE )
	{
		str = _T("Size");
		return;
	}

	if ( Flag & CHECK_FLAG_MD5 )
	{
		str = _T("MD5");
		return;
	}

	if ( Flag & CHECK_FLAG_LASTWRITETIME )
	{
		str = _T("Last Write Time");
		return;
	}
}


void	GetFlagText(CStringArray & strArray, UINT32 Flag)
{
	if ( Flag == CHECK_FLAG_NULL )
	{
		strArray.Add(_T("Null"));
		return;
	}

	if ( Flag & CHECK_FLAG_EXIST )
		strArray.Add(_T("Exist"));
	if ( Flag & CHECK_FLAG_SIZE )
		strArray.Add(_T("Size"));
	if ( Flag & CHECK_FLAG_MD5 )
		strArray.Add(_T("MD5"));
	if ( Flag & CHECK_FLAG_LASTWRITETIME )
		strArray.Add(_T("Last Write Time"));
}

void	GetFlagTextAll(CString & str, UINT32 Flag)
{
	CStringArray strArray;
	GetFlagText(strArray, Flag);

	if ( strArray.GetCount() > 0 )
		str = strArray[0];

	for ( INT_PTR i = 1 ; i < strArray.GetCount() ; ++i )
	{
		str += _T(" / ");
		str += strArray[i];
	}
}

void	SplitFlag(UINT32 Flag, CUIntArray & flags)
{
	if ( Flag & CHECK_FLAG_EXIST )
		flags.Add(CHECK_FLAG_EXIST);

	if ( Flag & CHECK_FLAG_SIZE )
		flags.Add(CHECK_FLAG_SIZE);

	if ( Flag & CHECK_FLAG_MD5 )
		flags.Add(CHECK_FLAG_MD5);

	if ( Flag & CHECK_FLAG_LASTWRITETIME )
		flags.Add(CHECK_FLAG_LASTWRITETIME);
}



void	FileProgressData::Clear()
{
	NumTotal		= 0;
	NumProgCount	= 0;
	FileTotalSize	= 0;
	FileProgEndSize	= 0;
	FileSize		= 0;
	FileProgSize	= 0;
	FileName.Empty();
	ProgText.Empty();
}

void	FileProgressData::ClearNoText()
{
	NumTotal		= 0;
	NumProgCount	= 0;
	FileTotalSize	= 0;
	FileProgEndSize	= 0;
	FileSize		= 0;
	FileProgSize	= 0;
	FileName.Empty();
}

void	FileProgressData::Set(LPCTSTR pText, UINT Total)
{
	Clear();

	ProgText	= pText;
	NumTotal	= Total;
}

void	FileProgressData::Set(UINT nTextID, UINT Total)
{
	Clear();

	ProgText.LoadString(nTextID);
	NumTotal	= Total;
}


CFileListFileInfo::CFileListFileInfo()
: FileName(_T(""))
, FileDir(_T(""))
, FilePath(_T(""))
{
	Version			= 0;
	FileSize		= 0;
	ZeroMemory(&MD5[0], sizeof(MD5));
	ZeroMemory(&CreationTime, sizeof(FILETIME));
	ZeroMemory(&LastWriteTime, sizeof(FILETIME));
	StartCheckOption		= CHECK_FLAG_DEFAULT;
	CommandCheckOption		= CHECK_FLAG_FULL;
}


void		CFileListFileInfo::PrintFileInfo(CString & Buffer) const
{
	UINT Ver[2] = {};
	memcpy(Ver, &Version, sizeof(FileVersion));
	return Buffer.Format( "*FileInfo	\"%s\"	\"%s\"	%u %u	%u	%u %u %u %u	%u %u	%u %u	%u	%u\n",
			FileName, FileDir, Ver[1], Ver[0], FileSize,
			MD5[3], MD5[2], MD5[1], MD5[0],
			CreationTime.dwLowDateTime, CreationTime.dwHighDateTime,
			LastWriteTime.dwLowDateTime, LastWriteTime.dwHighDateTime,
			StartCheckOption, CommandCheckOption);
}


UINT32	CFileListFileInfo::CheckByStart(LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize, UINT * pMD5ProgSize) const
{
	return Check( StartCheckOption, BaseDir, IsNTFS, pMD5TotalSize, pMD5ProgSize);
}

UINT32	CFileListFileInfo::CheckByCommand(LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize, UINT * pMD5ProgSize) const
{
	return Check( CommandCheckOption, BaseDir, IsNTFS, pMD5TotalSize, pMD5ProgSize);
}


UINT32	CFileListFileInfo::Check(UINT32 Flag, LPCTSTR BaseDir, bool IsNTFS, UINT * pMD5TotalSize, UINT * pMD5ProgSize) const
{
	if ( Flag == CHECK_FLAG_NULL )
		return 0;

	assert(BaseDir != NULL);

	CString				FilePath2	= JoinPath(BaseDir, GetFilePath());
	WIN32_FIND_DATA		findFileData;

	HANDLE hFind = FindFirstFile(FilePath2, &findFileData);
	if (hFind == INVALID_HANDLE_VALUE)
		return CHECK_FLAG_EXIST;
	FindClose(hFind);

	// 파일 크기 검사
	if ( Flag & CHECK_FLAG_SIZE )
	{
		UINT64 Size = ((findFileData.nFileSizeHigh * (UINT64(MAXDWORD)+1)) + findFileData.nFileSizeLow);
		if ( (UINT64)GetFileSize() != Size )
			return CHECK_FLAG_SIZE;
	}

	if ( Flag & CHECK_FLAG_MD5 )
	{
		// 로컬파일의 MD5 키를 얻는다.
		UINT32 Key[4] = {};
		int Rv = md5_file_ex(FilePath2, (unsigned char*)Key, pMD5TotalSize, pMD5ProgSize);
		if ( 0 != Rv )
			return CHECK_FLAG_MD5;

		// 키값을 비교한다.
		if ( 0 != memcmp(GetMD5(), Key, sizeof(Key)) )
			return CHECK_FLAG_MD5;
	}

	if ( Flag & CHECK_FLAG_LASTWRITETIME )
	{
		// ** 파일 시스템의 기준시간은 각각 다음과 같습니다.
		//    NTFS : UTC (universal time coordinated)
		//    FAT  : Local (시스템 설정 시간, 국가 셋팅마다 다릅니다.)
		//
		// ** 파일시스템 마다 얻어지는 기준시간이 다르므로
		//    시스템 마다의 각각 처리가 다르게 해줘야 합니다.
		//
		if ( IsNTFS )
		{			
			if ( 0 != CompareFileTime(GetLastWriteTime(), &findFileData.ftLastWriteTime) )
			{
				// 위 MD5 검사를 통과했는데 파일타임이 다르면
				// 파일리스트에 있는 파일타임으로 설정한다.
				if ( ! (Flag & CHECK_FLAG_MD5) )
					return CHECK_FLAG_LASTWRITETIME;

				if ( ! SetFileTime(FilePath2, NULL, GetLastWriteTime()) )
					return CHECK_FLAG_LASTWRITETIME;
			}
		}
		else
		{
			FILETIME	UTCFileTime = {};
			LocalFileTimeToFileTime(&findFileData.ftLastWriteTime, &UTCFileTime);

			if ( ! CheckFileTimeInterval(GetLastWriteTime(), &UTCFileTime, FAT_LASTWRITE_RESOLUTION) )
			{
				// 파일 타임 시간 오차가 FAT_LASTWRITE_RESOLUTION보다 크다면 이 블록으로 들어온다.

				// 위 MD5 검사를 통과했는데 파일타임이 다르면
				// 파일리스트에 있는 파일타임으로 설정한다.
				if ( ! (Flag & CHECK_FLAG_MD5) )
					return CHECK_FLAG_LASTWRITETIME;

				// FileInfo에 저장되어 있는 파일 시간은 UTC시간이다.
				FILETIME	LocalFileTime = {};
				FileTimeToLocalFileTime(GetLastWriteTime(), &LocalFileTime);
				if ( ! SetFileTime(FilePath2, NULL, &LocalFileTime) )
					return CHECK_FLAG_LASTWRITETIME;
			}
		}
	}

	return 0;
}


bool	CFileListFileInfo::Compare(const CFileListFileInfo * pLeft, const CFileListFileInfo * pRight)
{
	if (pLeft->FileName != pRight->FileName)
		return false;
	
	if (pLeft->FileDir != pRight->FileDir)
		return false;

	if (pLeft->Version != pRight->Version)
		return false;

	if (pLeft->FileSize != pRight->FileSize)
		return false;

	if ( 0 != memcmp(pLeft->MD5, pRight->MD5, sizeof(pLeft->MD5)) )
		return false;
#if 0
	if ( 0 != memcmp(&pLeft->GetCreationTime(), &pRight->GetCreationTime(), sizeof(FILETIME)) )
		return false;

	if ( 0 != memcmp(&pLeft->GetLastWriteTime(), &pRight->GetLastWriteTime(), sizeof(FILETIME)) )
		return false;
#endif
	return true;
}

bool	CFileListFileInfo::CompareDeep(CFileListFileInfo * pLeft, CFileListFileInfo * pRight,
	LPCTSTR pLeftDir, LPCTSTR pRightDir, UINT * pFileSize, UINT * pFileProgSize)
{
	if (pLeft->FileName != pRight->FileName)
		return false;
	
	if (pLeft->FileDir != pRight->FileDir)
		return false;

	// 자세히 비교할 때는 버전과 파일타임을 검사하지 않습니다.
#if 0
	if (pLeft->Version != pRight->Version)
		return false;

	if ( 0 != memcmp(&pLeft->GetCreationTime(), &pRight->GetCreationTime(), sizeof(FILETIME)) )
		return false;

	if ( 0 != memcmp(&pLeft->GetLastWriteTime(), &pRight->GetLastWriteTime(), sizeof(FILETIME)) )
		return false;
#else
	// warning 없애기 위한 코드 정리 필요
	pLeftDir;
	pRightDir;
	pFileSize;
	pFileProgSize;
#endif

	if (pLeft->FileSize != pRight->FileSize)
		return false;

	if ( 0 != memcmp(pLeft->MD5, pRight->MD5, sizeof(pLeft->MD5)) )
		return false;

	// 임시로 값을 복사합니다.
	// 처음 설계시 파일리스트에 시간값을 저장하지 않아서 파일타임이 저장되어 있지 않은
	// 파일리스트와 비교시에는 파일타임이 0으로 설정되기 때문입니다.
	if ( IsNullTime(pLeft->GetCreationTime()) )
		pLeft->SetCreationTime(pRight->GetCreationTime());
	if ( IsNullTime(pLeft->GetLastWriteTime()) )
		pLeft->SetLastWriteTime(pRight->GetLastWriteTime());

#if 0
	CString		LeftPath	= JoinPath(pLeftDir, pLeft->FilePath);
	CString		RightPath	= JoinPath(pRightDir, pRight->FilePath);

	CFD_RV	Rv = CompareFileData(LeftPath, RightPath, pFileSize, pFileProgSize);
	if ( CFD_RV_TRUE != Rv )
		return false;
#endif

	return true;
}



bool	CFileListFileInfo::CompPath(CFileListFileInfo * pLeft, CFileListFileInfo * pRight)
{
	return (pLeft->FilePath < pRight->FilePath) ? true : false;
}





void	CompareResult::Clear()
{
	NormalFiles.clear();
	DiffFiles.clear();
	DiffOrigFiles.clear();
	LeftOnlyFiles.clear();
	RightOnlyFiles.clear();
	CheckFlagModifiedCount	= 0;
}


bool	CompareResult::IsNoDiff()
{
	// 바뀌거나 한쪽에만 있는 파일이 없다면 다르지 않음.
	return (LeftOnlyFiles.empty() && RightOnlyFiles.empty() && DiffFiles.empty() && CheckFlagModifiedCount == 0);
}


#if 0
bool	CompareResult::Save(LPCTSTR pFilePath, FileVersion OldFileListVer, FileVersion NewFileListVer)
{
	assert(DiffFiles.size() == DiffOrigFiles.size());

	CFile	File;
	if ( ! File.Open(pFilePath, CFile::modeCreate | CFile::modeWrite) )
		return false;

	TCHAR		strBuffer[1024];
	CString		str;

	//
	// 
	//
	str.Format("Old Ver: %s\n", VersionToDate(OldFileListVer));
	File.Write(str.GetBuffer(), str.GetLength());

	str.Format("New Ver: %s\n", VersionToDate(NewFileListVer));
	File.Write(str.GetBuffer(), str.GetLength());


	//
	// 바뀐 파일
	//
	{
		str.Format(_T("\n\nDiff Files : %d\n\n"), (int)DiffFiles.size());
		File.Write(str.GetBuffer(), str.GetLength());

		CFileListFileInfo * pLeftInfo = NULL, * pRightInfo = NULL;
		ListFileInfo::iterator iterLeft		= DiffFiles.begin();
		ListFileInfo::iterator iterRight	= DiffOrigFiles.begin();

		int	Count	= 0;
		while (iterLeft != DiffFiles.end())
		{
			pLeftInfo	= *iterLeft;
			pRightInfo	= *iterRight;

			str.Format(_T("%d\n"), ++Count);
			File.Write(str.GetBuffer(), str.GetLength());

			pLeftInfo->PrintFileInfo(strBuffer);
			str.Format(_T("	Old : %s\n"), strBuffer);
			File.Write(str.GetBuffer(), str.GetLength());

			pRightInfo->PrintFileInfo(strBuffer);
			str.Format(_T("	New : %s\n"), strBuffer);
			File.Write(str.GetBuffer(), str.GetLength());

			++iterLeft;
			++iterRight;
		}
	}

	//
	// 새로 생긴 파일
	//
	{
		str.Format(_T("\n\nNew Files : %d\n\n"), (int)RightOnlyFiles.size());
		File.Write(str.GetBuffer(), str.GetLength());

		int	Count	= 0;
		CFileListFileInfo *	pInfo = NULL;
		for (ListFileInfo::iterator iter = RightOnlyFiles.begin() ;
			iter != RightOnlyFiles.end() ; ++iter)
		{
			pInfo = (*iter);
			str.Format(_T("%d\n"), ++Count);
			File.Write(str.GetBuffer(), str.GetLength());

			pInfo->PrintFileInfo(strBuffer);
			str.Format(_T("	%s\n"), strBuffer);
			File.Write(str.GetBuffer(), str.GetLength());
		}
	}

	//
	// 지워진 파일
	//
	{
		str.Format(_T("\n\nDelete Files : %d\n\n"), (int)LeftOnlyFiles.size());
		File.Write(str.GetBuffer(), str.GetLength());

		int	Count	= 0;
		CFileListFileInfo *	pInfo = NULL;
		for (ListFileInfo::iterator iter = LeftOnlyFiles.begin() ;
			iter != LeftOnlyFiles.end() ; ++iter)
		{
			pInfo = (*iter);
			str.Format(_T("%d\n"), ++Count);
			File.Write(str.GetBuffer(), str.GetLength());

			pInfo->PrintFileInfo(strBuffer);
			str.Format(_T("	%s\n"), strBuffer);
			File.Write(str.GetBuffer(), str.GetLength());
		}
	}

	//
	// 유지된 파일
	//
	{
		str.Format(_T("\n\nNormal Files : %d\n\n"), (int)NormalFiles.size());
		File.Write(str.GetBuffer(), str.GetLength());

		int	Count	= 0;
		CFileListFileInfo *	pInfo = NULL;
		for (ListFileInfo::iterator iter = NormalFiles.begin() ;
			iter != NormalFiles.end() ; ++iter)
		{
			pInfo = (*iter);
			str.Format(_T("%d\n"), ++Count);
			File.Write(str.GetBuffer(), str.GetLength());

			pInfo->PrintFileInfo(strBuffer);
			str.Format(_T("	%s\n"), strBuffer);
			File.Write(str.GetBuffer(), str.GetLength());
		}
	}

	return true;
}
#endif



CFileList::CFileList()
{
	m_pLexer	= new CFileListLexer();
	m_Version	= 0;
	m_FormatVer	= UNKNOWN_VER;
}

CFileList::CFileList(const CFileList & FileList)
{
	m_pLexer		= new CFileListLexer();
	*m_pLexer		= *FileList.m_pLexer;
	m_FormatVer		= FileList.m_FormatVer;
	m_FullPath		= FileList.m_FullPath;
	m_DirPath		= FileList.m_DirPath;

	m_Version		= FileList.m_Version;
	for ( ListFileInfo::const_iterator iter = FileList.m_FileInfoList.begin() ; iter != FileList.m_FileInfoList.end() ; ++iter )
	{
		const CFileListFileInfo * pInfo = *iter;
		m_FileInfoList.push_back(new CFileListFileInfo(*pInfo));
	}

}

CFileList::~CFileList()
{
	SAFE_DELETE(m_pLexer);
	Clear();
}


bool	CFileList::Open(LPCTSTR pPath)
{
	Clear();

	// 파일 유무 확인
	CFileStatus	Status;
	if (!CFile::GetStatus(pPath, Status))
		return false;

	// 파일 읽기
	if (!m_pLexer->Open(pPath))
		return false;

	// 버전에 맞게 분석작업하기.
	bool	Rv	= false;
	m_FormatVer = (VERSION)m_pLexer->GetFileVersion();
	switch(m_FormatVer)
	{
	case VER_1	:	Rv = Parse_1();	break;
	case VER_2	:	Rv = Parse_2();	break;
	case VER_3	:	Rv = Parse_3();	break;
	case VER_4	:	Rv = Parse_4();	break;
	}


	if ( ! Rv )
		return false;

	// 파일 정보 정렬
	SortFileInfoList();

	m_pLexer->Close();
	SetPath(pPath);
	return true;
}


void	CFileList::Clear()
{
	m_FullPath		= _T("");
	m_DirPath		= _T("");
	m_Version		= 0;
	DELETE_ALL(m_FileInfoList);
}

bool	CFileList::SaveAs(LPCTSTR pFilePath)
{
	CFile File;
	if ( !File.Open(pFilePath, CFile::modeCreate | CFile::modeWrite ) )
		return false;

	CString	strBuffer;
	CString	strTemp;
	UINT	Ver[2] = {};


	// 리스트 헤더 저장
	memcpy(Ver, &m_Version, sizeof(FileVersion));	
	strTemp.Format( "*Header	%u %u\n", Ver[1], Ver[0] );
	strBuffer += strTemp;

	// 파일정보
	CFileListFileInfo * pInfo = NULL;
	for (ListFileInfo::const_iterator iter = m_FileInfoList.begin() ;	iter != m_FileInfoList.end() ; ++iter)
	{
		pInfo = *iter;
		pInfo->PrintFileInfo(strTemp);
		strBuffer += strTemp;
	}

	INT32	HeaderSize		= sizeof(int);
	INT32	BufferSize		= strBuffer.GetLength() + 1;
	INT32	TotalSize		= BufferSize + HeaderSize;
	UINT8 * pFileInfoBuffer = NULL;
	pFileInfoBuffer = (UINT8*)malloc(TotalSize);

	if ( pFileInfoBuffer == NULL ) return false;

	// 파일 버전 저장
	m_FormatVer = LAST_VER;
	int  FileVer = LAST_VER; 
	memcpy(pFileInfoBuffer, &FileVer, HeaderSize);
	memcpy(pFileInfoBuffer + HeaderSize, strBuffer.GetBuffer(), BufferSize);

	BitRotateEncript(pFileInfoBuffer, TotalSize, 4);

	File.Write(pFileInfoBuffer, TotalSize);
	File.Close();
	
	free(pFileInfoBuffer);
	
	m_FullPath = pFilePath;

	return true;
}


void	CFileList::SetFileVersion(const FileVersion & val)
{
	for (ListFileInfo::iterator iter = m_FileInfoList.begin() ; iter != m_FileInfoList.end() ; ++iter)
		(*iter)->SetVersion(&val);
}


void	CFileList::SetCurTimeVersion()
{
	SYSTEMTIME		m_SystemTime;
	::GetLocalTime( &m_SystemTime );	
	FileVersion	Ver		= ( unsigned long long(m_SystemTime.wYear) * 10000000000)+ 
							(unsigned long long(m_SystemTime.wMonth) * 100000000)+
								(unsigned long long(m_SystemTime.wDay) * 1000000)+
								(unsigned long long(m_SystemTime.wHour) * 10000) +
								(unsigned long long(m_SystemTime.wMinute) * 100) +
								unsigned long long(m_SystemTime.wSecond);
	
	SetVersion(Ver);
	SetFileVersion(Ver);
}


FileVersion		CFileList::GetVersion()
{
	return m_Version;
}


void	CFileList::SetVersion(const FileVersion & val)
{
	m_Version	= val;
}


bool	CFileList::Parse_1()
{
	TOKEN_FILELIST	Identifier = TOKEN_FILELIST_INVALID;

	bool	Loop = true;
	while (Loop)
	{
		Identifier = m_pLexer->GetToken();

		switch(Identifier)
		{
		default							:
		case TOKEN_FILELIST_INVALID		:
			return false;
			break;

		case TOKEN_FILELIST_FILEEND		:	Loop = false;	break;

		case TOKEN_FILELIST_IDENTIFIER	:
			{
				bool	Rv = false;
				FILELIST_ITEM ItemNumber = GetItemNumber(m_pLexer->GetLastTokenString());
				switch(ItemNumber)
				{
				default:	assert(0 && "not found file list identifier");	return false;

				case FILELIST_ITEM_HEADER:
					{
						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv ) return false;

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv ) return false;

						memcpy(&m_Version, Ver, sizeof(FileVersion));
					}
					break;
				case FILELIST_ITEM_FILEINFO:
					{
						CFileListFileInfo *	pInfo = new CFileListFileInfo;
						CString		str;

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileName(str);

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileDir(str);

						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetVersion((FileVersion *)&Ver);

						UINT Value;
						Rv = m_pLexer->GetTokenUInt(Value);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileSize(Value);

						UINT32	MD5[4] = {};
						for (int i = 3 ; i >= 0 ; --i)
						{
							Rv = m_pLexer->GetTokenUInt(MD5[i]);
							if ( ! Rv )
							{
								delete pInfo;
								return false;
							}
						}
						pInfo->SetMD5(MD5);

						str = pInfo->GetFileDir();
						str += pInfo->GetFileName();
						pInfo->SetFilePath(str);

						m_FileInfoList.push_back(pInfo);
					}
					break;
				}

			}
			break;
		}
	} 

	return true;
}


bool	CFileList::Parse_2()
{

	bool	Loop = true;
	while (Loop)
	{
		TOKEN_FILELIST Identifier = m_pLexer->GetToken();

		switch(Identifier)
		{
		default							:
		case TOKEN_FILELIST_INVALID		:
			return false;
			break;

		case TOKEN_FILELIST_FILEEND		:	Loop = false;	break;

		case TOKEN_FILELIST_IDENTIFIER	:
			{
				bool	Rv = false;
				FILELIST_ITEM ItemNumber = GetItemNumber(m_pLexer->GetLastTokenString());
				switch(ItemNumber)
				{
				default:	assert(0 && "not found file list identifier");	return false;

				case FILELIST_ITEM_HEADER:
					{
						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv ) return false;

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv ) return false;

						memcpy(&m_Version, Ver, sizeof(FileVersion));
					}
					break;
				case FILELIST_ITEM_FILEINFO:
					{
						CFileListFileInfo *	pInfo = new CFileListFileInfo;
						CString		str;

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileName(str);

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileDir(str);

						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetVersion((FileVersion *)Ver);

						UINT Value;
						Rv = m_pLexer->GetTokenUInt(Value);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileSize(Value);

						UINT32	MD5[4] = {};
						for (int i = 3 ; i >= 0 ; --i)
						{
							Rv = m_pLexer->GetTokenUInt(MD5[i]);
							if ( ! Rv )
							{
								delete pInfo;
								return false;
							}
						}
						pInfo->SetMD5(MD5);


						// File Creation Time
						FILETIME	fTime;
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetCreationTime(&fTime);

						// File Last Write Time
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetLastWriteTime(&fTime);


						// 사용 조합.
						str = pInfo->GetFileDir();
						str += pInfo->GetFileName();
						pInfo->SetFilePath(str);

						m_FileInfoList.push_back(pInfo);
					}
					break;
				}

			}
			break;
		}
	} 

	return true;
}


bool	CFileList::Parse_3()
{
TOKEN_FILELIST	Identifier = TOKEN_FILELIST_INVALID;

	bool	Loop = true;
	while (Loop)
	{
		Identifier = m_pLexer->GetToken();

		switch(Identifier)
		{
		default							:
		case TOKEN_FILELIST_INVALID		:
			return false;
			break;

		case TOKEN_FILELIST_FILEEND		:	Loop = false;	break;

		case TOKEN_FILELIST_IDENTIFIER	:
			{
				bool	Rv = false;
				FILELIST_ITEM ItemNumber = GetItemNumber(m_pLexer->GetLastTokenString());
				switch(ItemNumber)
				{
				default:	assert(0 && "not found file list identifier");	return false;

				case FILELIST_ITEM_HEADER:
					{
						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv ) return false;

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv ) return false;

						memcpy(&m_Version, Ver, sizeof(FileVersion));
					}
					break;
				case FILELIST_ITEM_FILEINFO:
					{
						CFileListFileInfo *	pInfo = new CFileListFileInfo;
						CString		str;

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileName(str);

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileDir(str);

						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetVersion((FileVersion *)Ver);

						UINT Value;
						Rv = m_pLexer->GetTokenUInt(Value);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileSize(Value);

						UINT32	MD5[4] = {};
						for (int i = 3 ; i >= 0 ; --i)
						{
							Rv = m_pLexer->GetTokenUInt(MD5[i]);
							if ( ! Rv )
							{
								delete pInfo;
								return false;
							}
						}
						pInfo->SetMD5(MD5);


						// File Creation Time
						FILETIME	fTime;
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetCreationTime(&fTime);

						// File Last Write Time
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetLastWriteTime(&fTime);

						UINT32	StartCheckFlag = 0;
						Rv = m_pLexer->GetTokenUInt(StartCheckFlag);
						pInfo->SetStartCheckOptionFlag(StartCheckFlag);


						// 사용 조합.
						str = pInfo->GetFileDir();
						str += pInfo->GetFileName();
						pInfo->SetFilePath(str);

						m_FileInfoList.push_back(pInfo);
					}
					break;
				}

			}
			break;
		}
	} 

	return true;
}



bool	CFileList::Parse_4()
{

	bool	Loop = true;
	while (Loop)
	{
		TOKEN_FILELIST Identifier = m_pLexer->GetToken();

		switch(Identifier)
		{
		default							:
		case TOKEN_FILELIST_INVALID		:
			return false;
			break;

		case TOKEN_FILELIST_FILEEND		:	Loop = false;	break;

		case TOKEN_FILELIST_IDENTIFIER	:
			{
				bool	Rv = false;
				FILELIST_ITEM ItemNumber = GetItemNumber(m_pLexer->GetLastTokenString());
				switch(ItemNumber)
				{
				default:	assert(0 && "not found file list identifier");	return false;

				case FILELIST_ITEM_HEADER:
					{
						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv ) return false;

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv ) return false;

						memcpy(&m_Version, Ver, sizeof(FileVersion));
					}
					break;
				case FILELIST_ITEM_FILEINFO:
					{
						CFileListFileInfo *	pInfo = new CFileListFileInfo;
						CString		str;

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileName(str);

						Rv = m_pLexer->GetTokenString(str);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileDir(str);

						UINT	Ver[2] = {};
						Rv = m_pLexer->GetTokenUInt(Ver[1]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}

						Rv = m_pLexer->GetTokenUInt(Ver[0]);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetVersion((FileVersion *)Ver);

						UINT Value;
						Rv = m_pLexer->GetTokenUInt(Value);
						if ( ! Rv )
						{
							delete pInfo;
							return false;
						}
						pInfo->SetFileSize(Value);

						UINT32	MD5[4] = {};
						for (int i = 3 ; i >= 0 ; --i)
						{
							Rv = m_pLexer->GetTokenUInt(MD5[i]);
							if ( ! Rv )
							{
								delete pInfo;
								return false;
							}
						}
						pInfo->SetMD5(MD5);


						// File Creation Time
						FILETIME	fTime;
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetCreationTime(&fTime);

						// File Last Write Time
						Rv = m_pLexer->GetTokenUInt(fTime.dwLowDateTime);
						Rv = m_pLexer->GetTokenUInt(fTime.dwHighDateTime);
						pInfo->SetLastWriteTime(&fTime);

						UINT32	StartCheckFlag = 0;
						Rv = m_pLexer->GetTokenUInt(StartCheckFlag);
						pInfo->SetStartCheckOptionFlag(StartCheckFlag);

						Rv = m_pLexer->GetTokenUInt(StartCheckFlag);
						pInfo->SetCommandCheckOptionFlag(StartCheckFlag);


						// 사용 조합.
						str = pInfo->GetFileDir();
						str += pInfo->GetFileName();
						pInfo->SetFilePath(str);

						m_FileInfoList.push_back(pInfo);
					}
					break;
				}

			}
			break;
		}
	} 

	return true;
}


void	CFileList::SortFileInfoList()
{
	//
	// FileInfo를 경로를 기준으로 정렬시킨다.
	//

	m_FileInfoList.sort(CFileListFileInfo::CompPath);
}


void	CFileList::SetPath(LPCTSTR pPath)
{
	SplitPath(pPath, &m_DirPath, NULL);
	m_FullPath	= pPath;
}


CFileList *	CFileList::MakeFileList(LPCTSTR pPath, CFileFilter & FileFilter, const CStringArray & InclFilePaths, bool GenMD5, FileProgressData * pProgData)
{
	CFileList *			pFileList	= NULL;
	CFileListFileInfo *	FileInfo	= NULL;
	UINT32				MD5[4];
	CString				FileDir, FileName;
	CString				DirPath(pPath);
	CFileStatus			Status;
	CStringArray		Files;			// pPath에 포함되는 모든 파일이 Path가 담긴다.

	FileProgressData	__ProgData;

	if ( ! pProgData )
		pProgData = &__ProgData;
	pProgData->Clear();
	pProgData->ProgText	= _T("load directory files");


	if ( ! GetDirectoryHasFilesEx(DirPath, Files, FileFilter, true) )
		return NULL;

	// 강제 포함 파일이 유효한지 확인하고 넣는다.
	CString	Path;
	for (INT_PTR i = 0 ; i < InclFilePaths.GetCount() ; ++i)
	{
		Path = JoinPath(pPath, InclFilePaths.GetAt(i));

		Path.Replace('/', '\\');

		if ( CFile::GetStatus(Path, Status) )
			Files.Add(Path);
	}

	if ( DirPath.Right(1) != '\\' )
		DirPath += '\\';

	pFileList				= new CFileList();
	pProgData->NumTotal		= (UINT)Files.GetCount();
	pProgData->ProgText		= _T("make file list");

	int NumFile = (int)Files.GetCount();
	for (int i = 0 ; i < NumFile ; ++i)
	{
		CString & FilePath	= Files[i];
		SplitPath(FilePath, &FileDir, &FileName);

		pProgData->FileName	= FileName;
		if ( GenMD5 )
		{
			if ( 0 != md5_file_ex(FilePath, (unsigned char*)MD5, &pProgData->FileSize, &pProgData->FileProgSize) )
			{
				TRACE0("generate MD5 failed..\n");
				delete pFileList;
				return NULL;
			}
		}

		FileInfo				= new CFileListFileInfo();
		FileInfo->SetFileName(FileName);

		// 상대경로로 만든다.
		if ( FileDir == DirPath )
		{
			// 루트 디렉토리에 들어있을때는 .\를 넣어준다.
			FileInfo->SetFileDir(_T(".\\"));
		}
		else
		{
			NEEDED_IMPROVE;		// 영어외는 문제가 일어난다.
			FileInfo->SetFileDir(FileDir.GetBuffer() + DirPath.GetLength());
		}


		FileInfo->SetFilePath(JoinPath(FileInfo->GetFileDir(), FileInfo->GetFileName()));
		FileInfo->SetMD5(MD5);

		CFile::GetStatus(FilePath, Status);
		FileInfo->SetFileSize((UINT32)Status.m_size);

		FILETIME	cTime, wTime;
		if ( ! GetFileTime(FilePath, &cTime, &wTime) )
		{
			TRACE0("GetFileTime Fail..\n");
			delete FileInfo;				
			delete pFileList;
			return NULL;
		}
		FileInfo->SetCreationTime(&cTime);
		FileInfo->SetLastWriteTime(&wTime);

		pFileList->m_FileInfoList.push_back(FileInfo);
		++pProgData->NumProgCount;
	}

	pFileList->SortFileInfoList();

	pFileList->m_Version	= 0;
	pFileList->m_DirPath	= pPath;

	return pFileList;
}

bool	CFileList::MakeUpdateList(CompareResult * pCompResult)
{
	DELETE_ALL(m_FileInfoList);

	SetCurTimeVersion();

	CFileListFileInfo *	pInfo	= NULL;

	// 업데이트되는 파일만 버전을 새로 갱신한다.
	for (ListFileInfo::iterator	iter = pCompResult->DiffFiles.begin() ; iter != pCompResult->DiffFiles.end() ; ++iter)
	{
		pInfo			= new CFileListFileInfo();
		*pInfo			= *(*iter);

		//파일이 변하더라도 검사 옵션은 원래 파일의 설정으로 해준다.

		pInfo->SetVersion(&m_Version);
		m_FileInfoList.push_back(pInfo);
	}

	for (ListFileInfo::iterator	iter = pCompResult->RightOnlyFiles.begin() ; iter != pCompResult->RightOnlyFiles.end() ; ++iter)
	{
		pInfo			= new CFileListFileInfo();
		*pInfo			= *(*iter);
		pInfo->SetVersion(&m_Version);
		m_FileInfoList.push_back(pInfo);
	}

	// 변함이 없는 파일은 버전을 변경하지 않습니다.
	for (ListFileInfo::iterator	iter = pCompResult->NormalFiles.begin() ; iter != pCompResult->NormalFiles.end() ; ++iter)
	{
		pInfo			= new CFileListFileInfo();
		*pInfo			= *(*iter);
		m_FileInfoList.push_back(pInfo);
	}

	SortFileInfoList();

	return true;
}


bool	CFileList::GenerateMD5(FileProgressData * pProgData)
{
	pProgData->ClearNoText();
	pProgData->NumTotal	= (UINT)m_FileInfoList.size();

	CString	Path;
	CFileListFileInfo *	pInfo;
	UINT32				MD5[4];


	for (ListFileInfo::iterator	iter = m_FileInfoList.begin() ;
		iter != m_FileInfoList.end() ; ++iter)
	{
		pInfo = *iter;
		Path = JoinPath(m_DirPath, pInfo->GetFilePath());

		pProgData->FileName	= pInfo->GetFileName();

		if ( 0 != md5_file_ex(Path, (unsigned char*)MD5, &pProgData->FileSize, &pProgData->FileProgSize) )
		{
			TRACE0("generate MD5 failed..");
			return false;
		}

		pInfo->SetMD5(MD5);
		pProgData->NumProgCount++;
	}

	return true;
}


struct	CompareFileInfoPath
{
	CompareFileInfoPath(LPCTSTR Path):m_Path(Path)	{}
	bool	operator ()(CFileListFileInfo * pFileInfo)
	{
		return (0 == strcmp(pFileInfo->GetFilePath(), m_Path));
	}
	LPCTSTR m_Path;
};

bool	CFileList::Compare(CFileList * pLeft, CFileList * pRight, CompareResult * pResult, bool DeepComp, FileProgressData * pProgData)
{
	assert(pLeft != NULL);
	assert(pRight != NULL);

	//
	// *********************************************
	// CFileList가 가지고 있는 FileInfoList는 
	// 경로순으로 정렬되어 있다고 가정한다.
	// *********************************************
	//

	const ListFileInfo &			LeftList	= pLeft->GetFileInfoList();
	const ListFileInfo &			RightList	= pRight->GetFileInfoList();
	ListFileInfo::const_iterator	LeftIter	= LeftList.begin();
	ListFileInfo::const_iterator	RightIter	= RightList.begin();
	CFileListFileInfo *		pLeftInfo	= NULL;
	CFileListFileInfo *		pRightInfo	= NULL;
	FileProgressData		__ProgData;
	CString					LeftPath, RightPath;

	if ( ! pProgData )
		pProgData = &__ProgData;

	pProgData->Clear();
	pProgData->NumTotal = (LeftList.size() > RightList.size()) ? (UINT)LeftList.size() : (UINT)RightList.size();


	while (true)
	{
		// 왼쪽 오른쪽 반복자가 모두 없으면 끝!
		if ( LeftIter == LeftList.end() &&
			RightIter == RightList.end() )
		{
			break;
		}

		// 왼쪽 또는 오른쪽 반복자만 끝났으면 루프를 벗어난다.
		if (LeftIter == LeftList.end() &&
			RightIter != RightList.end())
		{
			pResult->RightOnlyFiles.insert(pResult->RightOnlyFiles.end(), RightIter, RightList.end());
			break;
		}
		else if (LeftIter != LeftList.end() &&
			RightIter == RightList.end())
		{
			pResult->LeftOnlyFiles.insert(pResult->LeftOnlyFiles.end(), LeftIter, LeftList.end());
			break;
		}

		pLeftInfo	= *LeftIter;
		pRightInfo	= *RightIter;

		// 경로로 비교를 한다.
		LeftPath = pLeftInfo->GetFilePath();
		RightPath = pRightInfo->GetFilePath();
		LeftPath.Replace('/', '\\');
		RightPath.Replace('/', '\\');
		if ( 0 == LeftPath.Compare(RightPath) )
		{
			// 경로가 같다면
			// 파일이 다른지 비교를 해서 다르다면 파일정보를 저장한다.
			bool Rv = false;
			if ( DeepComp )
			{
				pProgData->FileName	= pLeftInfo->GetFileName();
				Rv = CFileListFileInfo::CompareDeep( pLeftInfo, pRightInfo, pLeft->GetDirPath(), pRight->GetDirPath(), &pProgData->FileSize, &pProgData->FileProgSize );
			}
			else
			{
				Rv = CFileListFileInfo::Compare( pLeftInfo, pRightInfo );
			}

			if ( ! Rv )
			{
				pResult->DiffFiles.push_back(pRightInfo);
				pResult->DiffOrigFiles.push_back(pLeftInfo);
			}
			else
			{
				pResult->NormalFiles.push_back(pLeftInfo);
			}

			++LeftIter;
			++RightIter;
		}
		else
		{
			// 경로가 다르다면 서로 다른 파일이다.
			// 왼쪽에만 있는 파일인지를 검사한다.
			ListFileInfo::const_iterator Result = find_if(LeftIter, LeftList.end(), CompareFileInfoPath(pRightInfo->GetFilePath()));

			if (Result != LeftList.end())
			{
				// 왼쪽에만 있다면 저장하고 왼쪽반복자만 증가한다.
				pResult->LeftOnlyFiles.push_back(pLeftInfo);
				++LeftIter;
			}
			else
			{
				// 오른쪽에만 있다면 저장하고 오른쪽 반복자만 증가한다.
				pResult->RightOnlyFiles.push_back(pRightInfo);
				++RightIter;				
			}
		}

		pProgData->NumProgCount++;
	}

#if 0
//#ifdef _DEBUG
	TRACE1("*** Diff Files:%d ***\n", pResult->DiffFiles.size());
	for (ListFileInfo::iterator iter = pResult->DiffFiles.begin() ;
		iter != pResult->DiffFiles.end() ; ++iter)
	{
		TRACE1("%s\n", (*iter)->FilePath);
	}

	TRACE1("*** LeftOnly:%d ***\n", pResult->LeftOnlyFiles.size());
	for (ListFileInfo::iterator iter = pResult->LeftOnlyFiles.begin() ;
		iter != pResult->LeftOnlyFiles.end() ; ++iter)
	{
		TRACE1("%s\n", (*iter)->FilePath);
	}

	TRACE1("*** RightOnly:%d ***\n", pResult->RightOnlyFiles.size());
	for (ListFileInfo::iterator iter = pResult->RightOnlyFiles.begin() ;
		iter != pResult->RightOnlyFiles.end() ; ++iter)
	{
		TRACE1("%s\n", (*iter)->FilePath);
	}
#endif
	return true;
}


CFD_RV	CompareFileData(LPCTSTR pLeftFilePath, LPCTSTR pRightFilePath, UINT * pFileSize, UINT * pFileProgSize)
{
	if ( 0 == strcmp(pLeftFilePath, pRightFilePath) )
		return CFD_RV_TRUE;

	// 파일을 직접 비교한다.
	CFile	LeftFile, RightFile;

	if ( ! LeftFile.Open(pLeftFilePath, CFile::modeRead | CFile::shareDenyNone) )
	{
		return CFD_RV_LEFTERROR;
	}

	if ( ! RightFile.Open(pRightFilePath, CFile::modeRead | CFile::shareDenyNone) )
	{
		return CFD_RV_RIGHTERROR;
	}

	// 크기가 다르면 다르다.
	if (LeftFile.GetLength() != RightFile.GetLength())
	{
		return CFD_RV_FALSE;
	}

	CFD_RV		Rv				= CFD_RV_TRUE;
	ULONGLONG	FileSize		= LeftFile.GetLength();
	ULONGLONG	ReadTotal		= 0;
	UINT		LeftRead,			RightRead;
	enum {BUF_SIZE = 0xffff};
	TCHAR		LeftBuffer[BUF_SIZE],	RightBuffer[BUF_SIZE];

	if ( pFileSize )
		*pFileSize = (UINT)FileSize;
	if ( pFileProgSize )
		*pFileProgSize = 0;

	while(ReadTotal < FileSize)
	{
		LeftRead	= LeftFile.Read(LeftBuffer, BUF_SIZE);
		RightRead	= RightFile.Read(RightBuffer, BUF_SIZE);

		for (UINT i = 0 ; i < LeftRead ; ++i)
		{
			if ( LeftBuffer[i] != RightBuffer[i] )
			{
				Rv	= CFD_RV_FALSE;
				goto ExitLoop;
			}
			if ( pFileProgSize )
				(*pFileProgSize)++;
		}

		ReadTotal += LeftRead;
	}

ExitLoop:
	return Rv;
}

