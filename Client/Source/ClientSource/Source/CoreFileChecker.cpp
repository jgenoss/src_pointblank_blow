#include "pch.h"
#include "CoreFileChecker.h"

#include "FileList.h"

namespace CHECKERROR
{
	enum ENUM
	{
		SUCCESS						= 0,
		OPEN_FAILED					= -1,
		GET_DRIVENAME_FAILED		= -2,
		CHECK_FAILED				= -3,
		NOT_FOUND_INLIST			= -4,
	};

}


static const char * CheckingFileNames[] =
{
	"Pack\\String.i3Pack",
	"Pack\\Script.i3Pack",
#if defined(WINDOW_MODE_DISABLE)
	".\\WinLockDll.dll",
#endif

#if defined( LOCALE_NORTHAMERICA)
	".\\PiercingBlow.i3Exec",
	".\\PiercingBlow.exe",
#else
	".\\PointBlank.i3Exec",
	".\\PointBlank.exe",
#endif

	".\\i3SceneDx.dll",
	".\\PhysXCore.2.8.1.dll",
};



CCoreFileChecker::CCoreFileChecker(void)
{
	tstring FSName = GetFileSystemName();
	m_IsNTFS = (FSName == _T("NTFS"));

	CFileList * p = new CFileList();
	MEMDUMP_NEW( p, sizeof( CFileList));
	m_FileList.reset( p);
}


bool CCoreFileChecker::Open()
{

	// 파일 리스트를 연다.
	if ( ! m_FileList->Open("UserFileList.dat") )
	{
		return false;
	}
	return true;
}

int CCoreFileChecker::Check(const tstring & FilePath)
{
	struct NameCompare
	{
	public:
		NameCompare(const string & _name) : name(_name) {}
		bool operator()(const CFileListFileInfo * target)
		{
			return (0 == target->GetFilePath().CompareNoCase(name.c_str()) );
		}
		string name;
	};

	const ListFileInfo & InfoList = m_FileList->GetFileInfoList();
	ListFileInfo::const_iterator iter = InfoList.end();

	iter = find_if(InfoList.begin(), InfoList.end(), NameCompare(FilePath));
	if (iter == InfoList.end())
	{
		return CHECKERROR::NOT_FOUND_INLIST;
	}


	UINT32 Rv = (*iter)->Check((CHECK_FLAG_EXIST | CHECK_FLAG_SIZE | CHECK_FLAG_MD5), _T("./"), m_IsNTFS);
	if ( Rv != 0 )
	{
#if defined(I3_DEBUG)
		I3PRINTLOG(I3LOG_NOTICE,"__3 %s / %d 0x%x", (LPCSTR)(*iter)->GetFilePath(), Rv, Rv);
#endif
		return CHECKERROR::CHECK_FAILED;
	}

	return CHECKERROR::SUCCESS;
}


int CCoreFileChecker::CoreFilesCheck()
{
	if ( ! Open() )
		return CHECKERROR::OPEN_FAILED;


	for ( INT32 i = 0 ; i < _countof(CheckingFileNames) ; ++i )
	{
		int rv = Check(CheckingFileNames[i]);
		if ( CHECKERROR::SUCCESS != rv )
		{
			return rv;
		}
	}

	return 0;
}

const UINT32* CCoreFileChecker::GetMd5(const tstring & FilePath)
{
	struct NameCompare
	{
	public:
		NameCompare(const string & _name) : name(_name) {}
		bool operator()(const CFileListFileInfo * target)
		{
			return (0 == target->GetFilePath().CompareNoCase(name.c_str()) );
		}
		string name;
	};

	const ListFileInfo & InfoList = m_FileList->GetFileInfoList();
	ListFileInfo::const_iterator iter = InfoList.end();

	iter = find_if(InfoList.begin(), InfoList.end(), NameCompare(FilePath));
	if (iter == InfoList.end())
	{
		return nullptr;
	}

	return (*iter)->GetMD5();
}