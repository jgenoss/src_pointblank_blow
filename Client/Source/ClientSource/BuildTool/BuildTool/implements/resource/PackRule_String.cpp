#include "stdafx.h"
#include "PackRule_String.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"

namespace rsc
{
	//PackRule_String::PackRule_String(const Path& srcWorkdir, 
	//								 const Path& dstWorkdir, 
	//								 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_String::~PackRule_String() {}

	//bool PackRule_String::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	// ADD			Script/String
	//	// REMOVE		Script/String/*.xlsm
	//	// REMOVE		Script/String/*.txt
	//	// SAVE			String.i3Pack
	//	// CHECKSUM		String.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Script/String");

	//	pak.RemoveFileFromPack("Script/String/*.xlsm");
	//	pak.RemoveFileFromPack("Script/String/*.txt");

	//	pak.SavePack(packDir.c_str(), "String.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "String.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "String.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);

	//	return true;
	//}
}