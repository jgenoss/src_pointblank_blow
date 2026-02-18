#include "stdafx.h"
#include "PackRule_WeaponAnimPack.h"
#include "i3pack.h"

namespace rsc
{
	//PackRule_WeaponAnimPack::PackRule_WeaponAnimPack(const Path& srcWorkdir, 
	//												 const Path& dstWorkdir, 
	//												 SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_WeaponAnimPack::~PackRule_WeaponAnimPack() {}

	//bool PackRule_WeaponAnimPack::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	add	Weapon\WeaponAnimPack
	//	//
	//	//	save Weapon_WeaponAnimPack.i3Pack
	//	//	checksum Weapon_WeaponAnimPack.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();
	//	
	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Weapon\\WeaponAnimPack");

	//	pak.SavePack(packDir.c_str(), "Weapon_WeaponAnimPack.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Weapon_WeaponAnimPack.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "Weapon_WeaponAnimPack.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);
	//	return true;
	//}
}