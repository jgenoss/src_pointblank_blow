#include "stdafx.h"
#include "PackRule_CharaRSC.h"
#include "i3pack.h"
#include <boost/filesystem.hpp>

namespace rsc
{
	//PackRule_CharaRSC::PackRule_CharaRSC(const Path& srcWorkdir, 
	//									 const Path& dstWorkdir, 
	//									 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_CharaRSC::~PackRule_CharaRSC() {}

	//bool PackRule_CharaRSC::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	// ADD			Chara/*.RSC
	//	// SAVE			Chara.i3Pack
	//	// CHECKSUM		Chara.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Chara/*.RSC");
	//	pak.SavePack(packDir.c_str(), "Chara.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Chara.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "Chara.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);

	//	return true;
	//}
}