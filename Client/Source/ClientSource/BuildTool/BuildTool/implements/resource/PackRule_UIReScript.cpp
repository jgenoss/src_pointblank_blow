#include "stdafx.h"
#include "PackRule_UIReScript.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"

namespace rsc
{
	//PackRule_UIReScript::PackRule_UIReScript(const Path& srcWorkdir, 
	//										 const Path& dstWorkdir, 
	//										 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_UIReScript::~PackRule_UIReScript() {}

	//bool PackRule_UIReScript::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	add UIRe/*.luab -nosub
	//	//	add UIRe/Script/*.luab -nosub
	//	//	add UIRe/Script/CCMode/*.luab -nosub
	//	//	add UIRe/Script/Domination/*.luab -nosub
	//	//
	//	//	remove UIRe/Script/Mimic_Tab.luab
	//	//	remove UIRe/Script/pbUIlib.luab
	//	//	remove UIRe/Script/PBRe_Clan_Common.luab
	//	//
	//	//	save UI_Re2_Script.i3Pack
	//	//	checksum UI_Re2_Script.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("UIRe/*.luab");

	//	pak.RemoveFileFromPack("UIRe/Script/Mimic_Tab.luab");
	//	pak.RemoveFileFromPack("UIRe/Script/pbUIlib.luab");
	//	pak.RemoveFileFromPack("UIRe/Script/PBRe_Clan_Common.luab");

	//	pak.SavePack(packDir.c_str(), "UI_Re2_Script.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "UI_Re2_Script.i3Pack");

	//	// log.
	//	Path logRelpath("Pack");
	//	logRelpath /= "UI_Re2_Script.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logRelpath);

	//	return true;
	//}
}