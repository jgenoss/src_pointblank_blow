#include "stdafx.h"
#include "PackRule_AvatarCommon.h"
#include "i3pack.h"

namespace rsc
{
	//PackRule_AvatarCommon::PackRule_AvatarCommon(const Path& srcWorkdir, 
	//											 const Path& dstWoridir, 
	//											 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWoridir, loggerPtr)
	//{}

	//PackRule_AvatarCommon::~PackRule_AvatarCommon() {}

	//bool PackRule_AvatarCommon::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	add			Avatar - nosub
	//	//	save		Avatar_common.i3Pack
	//	//	checksum	Avatar_common.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Avatar/", false);
	//	pak.SavePack(packDir.c_str(), "Avatar_common.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Avatar_common.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "Avatar_common.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);

	//	return true;
	//}
}