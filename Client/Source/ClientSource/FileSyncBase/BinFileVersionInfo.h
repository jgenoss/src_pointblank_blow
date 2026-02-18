#pragma once

#include <sstream>
#include <string>
#include <memory>

//using namespace std;
//using namespace std::tr1;

#if defined(_UNICODE)
typedef		std::wstring		tstring;
typedef		std::wstringstream	tstringstream;
#else
typedef		std::string			tstring;
typedef		std::stringstream	tstringstream;
#endif

class CBinFileVersionInfo
{
private:
	explicit CBinFileVersionInfo(
		const tstring& fileVersion
		, const tstring& productName
		, const tstring& productVersion
		, const tstring& fileDescription
		);

public: // class methods
	static 	CBinFileVersionInfo 	GetVersionInfo();
	static 	CBinFileVersionInfo 	GetVersionInfo( const tstring& fileName );

public: // instance methods
	inline	tstring &		FileVersion()		{ return m_fileVersion; }
	inline	const tstring &	FileVersion() const	{ return m_fileVersion; }

	inline 	tstring &		ProductName()		{ return m_productName; }
	inline 	const tstring &	ProductName() const	{ return m_productName; }

	inline 	tstring &		ProductVersion()		{ return m_productVersion; }
	inline 	const tstring &	ProductVersion() const	{ return m_productVersion; }

	inline 	tstring &		FileDescription()		{ return m_fileDescription; }
	inline 	const tstring &	FileDescription() const	{ return m_fileDescription; }

private:
	tstring m_fileVersion;
	tstring m_productName;
	tstring m_productVersion;
	tstring m_fileDescription;
};
