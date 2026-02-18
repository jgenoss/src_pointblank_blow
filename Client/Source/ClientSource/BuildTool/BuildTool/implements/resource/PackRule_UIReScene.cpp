#include "stdafx.h"
#include "PackRule_UIReScene.h"
#include <boost/filesystem.hpp>
#include "i3pack.h"

namespace rsc
{
	//PackRule_UIReScene::PackRule_UIReScene(const Path& srcWorkdir, 
	//									   const Path& dstWorkdir, 
	//									   SyncLoggerPtr loggerPtr)
	//	: IPackRule(srcWorkdir, dstWorkdir, loggerPtr)
	//{}

	//PackRule_UIReScene::~PackRule_UIReScene() {}

	//bool PackRule_UIReScene::_Packing() const
	//{
	//	// ---------원본 팩스크립트 규칙---------------
	//	//	add UIRe/Scene/Clan/*.i3UIs -nosub
	//	//	add UIRe/Scene/Domination/*.i3UIs -nosub
	//	//	add UIRe/Scene/HUD/*.i3UIs -nosub
	//	//	add UIRe/Scene/Main/*.i3UIs -nosub
	//	//	add UIRe/Scene/Option/*.i3UIs -nosub
	//	//	add UIRe/Scene/Popup/*.i3UIs -nosub
	//	//	add UIRe/Scene/Tutorial/*.i3UIs -nosub
	//	//
	//	//	save UI_Re2.i3Pack
	//	//	checksum UI_Re2.i3Pack
	//	// --------------------------------------------

	//	// 위 원본 팩스크립트 규칙을 간단하게 정리하면 아래와 같고 이 규칙으로 패킹 수행.
	//	// add UIRe/Scene/*.i3UIs
	//	// save UI_Re2.i3Pack
	//	// checksum UI_Re2.i3Pack

	//	const std::string& srcWorkdir = m_srcWorkdir.string();

	//	std::string packDir = m_dstWorkdir.string();
	//	packDir += "/Pack";

	//	i3pack pak(srcWorkdir.c_str());

	//	pak.AddFileToPack("UIRe/Scene/*.i3UIs");

	//	pak.SavePack(packDir.c_str(), "UI_Re2.i3Pack");
	//	pak.SetChecksumToPack(packDir.c_str(), "UI_Re2.i3Pack");

	//	// log.
	//	Path logFileRelPath("Pack");
	//	logFileRelPath /= "UI_Re2.i3Pack";

	//	m_loggerPtr->Log(m_dstWorkdir, logFileRelPath);

	//	return true;
	//}
}