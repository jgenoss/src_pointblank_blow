#include "stdafx.h"
#include "PackRule_SoundRSC.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"
#include "util/PathTokenizer.h"

namespace rsc
{
	//PackRule_SoundRSC::PackRule_SoundRSC(const Path& srcWorkdir,
	//									 const Path& dstWorkdir, 
	//									 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_SoundRSC::~PackRule_SoundRSC() {}

	//bool PackRule_SoundRSC::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	// ADD			Sound/*.RSC
	//	// SAVE			Sound.i3Pack
	//	// CHECKSUM		Sound.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Sound/*.RSC");
	//	pak.SavePack(packDir.c_str(), "Sound.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Sound.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "Sound.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);

	//	return true;
	//}
}
