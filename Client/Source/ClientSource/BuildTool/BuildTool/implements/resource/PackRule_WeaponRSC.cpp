#include "stdafx.h"
#include "PackRule_WeaponRSC.h"
#include "i3pack.h"

namespace rsc
{
	//PackRule_WeaponRSC::PackRule_WeaponRSC(const Path& srcWorkdir, 
	//									   const Path& dstWorkdir, 
	//									   SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_WeaponRSC::~PackRule_WeaponRSC() {}

	//bool PackRule_WeaponRSC::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	add	Weapon\*.RSC
	//	//
	//	//	save Weapon.i3Pack
	//	//	checksum Weapon.i3Pack
	//	// --------------------------------------------

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("Weapon\\*.RSC");

	//	pak.SavePack(packDir.c_str(), "Weapon.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "Weapon.i3Pack");

	//	// log.
	//	Path logRelpath("Pack");
	//	logRelpath /= "Weapon.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logRelpath);
	//	return true;
	//}
}