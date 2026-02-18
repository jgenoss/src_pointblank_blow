#include "stdafx.h"
#include "PackRule_Script.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"

namespace rsc
{
	//PackRule_Script::PackRule_Script(const Path& srcWorkdir, 
	//								 const Path& dstWorkdir, 
	//								 SyncLoggerPtr loggerPtr) 
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_Script::~PackRule_Script() {}

	//bool PackRule_Script::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	ADD	config -nosub
	//	//	ADD	config/*.ini
	//	//
	//	//	REMOVE	config/IpFileDecripter.exe
	//	//	REMOVE	config/IpFileEncripter.exe
	//	//	REMOVE	config/lwsi.sif
	//	//	REMOVE	config/lwsi_En.sif
	//	//	REMOVE	config/*.txt
	//	//	REMOVE	config/LocaleValue.pef
	//	//	REMOVE	config/PreCommon.pbc
	//	//
	//	//	ENC	config/BC.i3Game
	//	//	ENC	config/*.pef
	//	//
	//	//	SAVE Script.i3Pack
	//	//	CHECKSUM Script.i3Pack
	//	// --------------------------------------------
	//	
	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("config", false);
	//	pak.AddFileToPack("config/*.ini");

	//	pak.RemoveFileFromPack("config/IpFileDecripter.exe");
	//	pak.RemoveFileFromPack("config/IpFileEncripter.exe");
	//	pak.RemoveFileFromPack("config/lwsi.sif");
	//	pak.RemoveFileFromPack("config/lwsi_En.sif");
	//	pak.RemoveFileFromPack("config/*.txt");
	//	pak.RemoveFileFromPack("config/LocaleValue.pef");
	//	pak.RemoveFileFromPack("config/PreCommon.pbc");

	//	pak.EncryptFileInPack("config/BC.i3Game");
	//	pak.EncryptFileInPack("config/*.pef");

	//	pak.SavePack(packDir.c_str(), "Script.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Script.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "Script.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);
	//	return true;
	//}
}