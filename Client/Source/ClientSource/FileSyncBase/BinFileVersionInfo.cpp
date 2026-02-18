#include "StdAfx.h"
#include "BinFileVersionInfo.h"


template< typename T >
struct array_deleter
{
	void operator ()( T const * p)
	{
		delete[] p;
	}
};


CBinFileVersionInfo::CBinFileVersionInfo( const tstring& fileVersion, const tstring& productName,
										 const tstring& productVersion, const tstring& fileDescription )
: m_fileVersion(fileVersion)
, m_productName(productName)
, m_productVersion(productVersion)
, m_fileDescription(fileDescription)
{

}


CBinFileVersionInfo CBinFileVersionInfo::GetVersionInfo()
{
	TCHAR	FileName[MAX_PATH];
	GetModuleFileName(NULL, FileName, sizeof(FileName));
	return GetVersionInfo(FileName);
}

CBinFileVersionInfo CBinFileVersionInfo::GetVersionInfo( const tstring & fileName )
{
	// 파일로부터 버전정보 데이터의 크기가 얼마인지를 구합니다.
	DWORD infoSize = GetFileVersionInfoSize(fileName.c_str(), 0);
	if(infoSize == 0) 
		throw std::exception("Getting version information failed!");

	// 버퍼할당
	BYTE * version = new BYTE[infoSize];
	std::tr1::shared_ptr<BYTE> bufferPtr(version, array_deleter<BYTE>() );

	if(version == NULL)
		throw std::exception("Memory allocation failed!");

	// 버전 정보 데이터를 가져옵니다.
	if(GetFileVersionInfo(fileName.c_str(), 0, infoSize, version) == 0)
		throw std::exception("Getting version information failed!");

	VS_FIXEDFILEINFO* pFileInfo = NULL;
	UINT fileInfoSize = 0;
	// buffer로부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
	if(VerQueryValue(version, _T("\\"),(LPVOID*)&pFileInfo, &fileInfoSize) == 0)
		throw std::exception("Getting version information failed!");

	// FileVersion 
	WORD majorVer, minorVer, buildNum, revisionNum;
	majorVer = HIWORD(pFileInfo->dwFileVersionMS);
	minorVer = LOWORD(pFileInfo->dwFileVersionMS);
	buildNum = HIWORD(pFileInfo->dwFileVersionLS);
	revisionNum = LOWORD(pFileInfo->dwFileVersionLS);

	tstringstream ss;
	ss << majorVer << _T(".") << minorVer << _T(".") << buildNum << _T(".") << revisionNum;
	tstring fileVersion(ss.str());


	void  * buffer = NULL;
	UINT bufLen = 0;
	DWORD * translation = NULL;

	// TRANSLATION 
	if(VerQueryValue(version, _T("\\VarFileInfo\\Translation"), (LPVOID*)&translation, &bufLen) == 0)
		throw std::exception("Getting translation failed!");

	assert(translation != NULL);


	// PRODUCT NAME
	TCHAR path[MAX_PATH];
	wsprintf(path, _T("\\StringFileInfo\\%04x%04x\\ProductName"), LOWORD(*translation), HIWORD(*translation) );

	if( ::VerQueryValue(version, path, &buffer, &bufLen) == 0 )
		throw std::exception("Getting a product name failed!");

	assert(buffer != NULL);

	tstring productName( (LPCTSTR)buffer ); 

	// PRODUCT VERSION 
	wsprintf(path, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"), LOWORD(*translation), HIWORD(*translation) );

	if( ::VerQueryValue(version, path, &buffer, &bufLen) == 0 )
		throw std::exception("Getting a product version failed!");

	assert(buffer != NULL);

	tstring productVersion( (LPCTSTR)buffer ); 

	// File Description
	wsprintf(path, _T("\\StringFileInfo\\%04x%04x\\FileDescription"), LOWORD(*translation), HIWORD(*translation) );

	if( ::VerQueryValue(version, path, &buffer, &bufLen) == 0 )
		throw std::exception("Getting a file description failed!");

	assert(buffer != NULL);

	tstring fileDescription( (LPCTSTR)buffer ); // "1, 0, 0, 1

	return CBinFileVersionInfo(fileVersion, productName, productVersion, fileDescription);
}
